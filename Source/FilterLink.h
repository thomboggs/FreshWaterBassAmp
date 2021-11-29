/*
  ==============================================================================

    FilterLink.h
    Created: 2 Nov 2021 9:50:47am
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ReleasePool.h"
#include "FilterCoefficientGenerator.h"
#include "Fifo.h"
#include "FilterParametersBase.h"
#include "FilterCoefficientGenerator.h"
#include "CoefficientsMaker.h"
#include "Decibel.h"


//=============================================================================
/*
 Template Specialization To check if T is ReferenceCountedObjectPtr
 */

template <typename T>
struct IsFilterParameterType : std::false_type { };

template <>
struct IsFilterParameterType<FilterParameters> : std::true_type { };

//=============================================================================

template<typename FilterType, typename FifoDataType, typename ParamType, typename FunctionType>
struct FilterLink
{
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        
        filter.prepare(spec);
    }
    
    void reset()
    {
        filter.reset();
    }
    
    template<typename ContextType>
    void process(const ContextType& context)
    {
        if ( !currentParams.bypassed )
        {
            filter.process(context);
        }
    }
    
    //stuff for the juce::SmoothedValue instances.
    void updateSmootherTargets()
    {
        if ( currentParams.frequency != freqSmoother.getTargetValue() )
        {
            freqSmoother.setTargetValue(currentParams.frequency);
        }
        
        if ( currentParams.quality != qualitySmoother.getTargetValue() )
        {
            qualitySmoother.setTargetValue(currentParams.quality);
        }
        if constexpr (IsFilterParameterType<ParamType>::value)
        {
            if ( currentParams.gain != gainSmoother.getTargetValue() )
            {
                gainSmoother.setTargetValue(currentParams.gain);
            }
        }
    }
    
    void resetSmoothers(float rampTime)
    {
        freqSmoother.reset(sampleRate, rampTime);
        freqSmoother.setCurrentAndTargetValue(currentParams.frequency);
        
        qualitySmoother.reset(sampleRate, rampTime);
        qualitySmoother.setCurrentAndTargetValue(currentParams.quality);
        
        if constexpr (IsFilterParameterType<ParamType>::value)
        {
            gainSmoother.reset(sampleRate, rampTime);
            gainSmoother.setCurrentAndTargetValue(currentParams.gain);
        }
    }
    
    bool isSmoothing() const
    {
        if constexpr (IsFilterParameterType<ParamType>::value)
        {
            return freqSmoother.isSmoothing() || qualitySmoother.isSmoothing() || gainSmoother.isSmoothing();
        }
        
        return freqSmoother.isSmoothing() || qualitySmoother.isSmoothing();
    }
    
    void checkIfStillSmoothing()
    {
        if (isSmoothing()) shouldComputeNewCoefficients = true;
    }
    
    void advanceSmoothers(int numSamples)
    {
        freqSmoother.skip(numSamples);
        qualitySmoother.skip(numSamples);
        
        if constexpr (IsFilterParameterType<ParamType>::value)
        {
            gainSmoother.skip(numSamples);
        }
    }
    
    //stuff for updating the params
    void updateParams(const ParamType& params)
    {
        shouldComputeNewCoefficients = false;
        
        if (currentParams != params)
        {
            // if changed, calc new Coeffs
            currentParams = params;
            shouldComputeNewCoefficients = true;
        }
    }
    
    //stuff for updating the coefficients from processBlock, prepareToPlay, or setStateInformation
    void updateCoefficients(const FifoDataType& coefficients)
    {
        /*
         Depending on FilterType, Choose which provate function to call to update Coeffs
         */
        if constexpr (IsFilterParameterType<ParamType>::value)
        {
            updateFilterState(filter.coefficients, coefficients);
        }
        else
        {
            configureCutFilterChain(coefficients);
        }
    }
    
    void loadCoefficients(bool fromFifo)
    {
        if ( fromFifo )
        {
    //        DBG("Attempting to Pull Coeffs From FIFO");
            
            while ( linkFifo.getNumAvailableForReading() > 0 )
            {
                FifoDataType ptr;
                if ( linkFifo.exchange(std::move(ptr)) )
                {
                    updateCoefficients(ptr);
                }
            }
        }
        else
        {
            updateCoefficients( FunctionType::calcCoefficients(currentParams) );
        }
    }
    
    void generateNewCoefficientsIfNeeded()
    {
        if (shouldComputeNewCoefficients)
        {
            // Using Smoothers
            ParamType tempParams;

            tempParams = currentParams;
            tempParams.frequency = freqSmoother.getNextValue();
            tempParams.quality = qualitySmoother.getNextValue();

            if constexpr (IsFilterParameterType<ParamType>::value)
            {
                tempParams.gain = gainSmoother.getNextValue();
            }

            // Send Params to the FCG
            linkFCG.changeParameters(tempParams);

            // Before Leaving, reset flag
            shouldComputeNewCoefficients = false;
        }
    }
    
    //stuff for configuring the filter before processing
    void performPreloopUpdate(const ParamType& params)
    {
        updateParams(params);
        updateSmootherTargets();
    }
    
    void performInnerLoopFilterUpdate(bool onRealTimeThread, int numSamplesToSkip)
    {
        // Exit early if Params are bypassed
        if ( currentParams.bypassed )
        {
            return;
        }
        
        // generate new coefficients if needed
        generateNewCoefficientsIfNeeded();
        
        // load any coefficients that are ready to go
        loadCoefficients( onRealTimeThread );
        
        // Advance the Smoothers
        advanceSmoothers( numSamplesToSkip );
        
        // Check If still Smoothing
        checkIfStillSmoothing();
    }

    
    void initialize(const ParamType& params, float rampTime, bool onRealTimeThread, double sr)
    {
        // Store Sample Rate
        sampleRate = sr;
        
        // Update the Params
        updateParams( params );
        
        // Reset The Smoothers
        resetSmoothers( rampTime );
        
        // load Coefficients
        loadCoefficients( onRealTimeThread );
    }

    
private:
    
    static const int fifoSize = 32;
    
    // releasePool
    using SingleCoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    ReleasePool<SingleCoefficientsPtr> linkDeletionPool { };
    // Fifo
    Fifo<FifoDataType, fifoSize> linkFifo;
    // FCG
    FilterCoefficientGenerator<FifoDataType, ParamType, CoefficientsMaker<float>, fifoSize> linkFCG {linkFifo, "Filter Thread"};
    // Params
    ParamType currentParams {};
    // shouldComputeNewCoefficients flag
    bool shouldComputeNewCoefficients { false };
    // sampleRate
    double sampleRate { 0 };
    // FilterType
    FilterType filter;
    
    // SmoothedValue Instances
    juce::SmoothedValue<float> freqSmoother {100.f}, qualitySmoother {1.f};
    juce::SmoothedValue<Decibel<float>> gainSmoother {0.f};

    //stuff for setting the coefficients of the FilterType instance.
    using Ptr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    
    void updateFilterState(Ptr& oldState, Ptr newState)
    {
        linkDeletionPool.add(newState);
        *(oldState) = *newState;
    }
    
    void configureCutFilterChain(const FifoDataType& coefficients)
    {
        if constexpr ( !IsFilterParameterType<ParamType>::value )
        {
            auto tempSize = coefficients.size();
            if ( tempSize > 0)
            {
                filter.template setBypassed<0>(true);
                filter.template setBypassed<1>(true);
                filter.template setBypassed<2>(true);
                filter.template setBypassed<3>(true);
                
                switch (tempSize)
                {
                    case 4:
                        filter.template setBypassed<3>(false);
                        updateFilterState(filter.template get<3>().coefficients, coefficients[3]);
                    case 3:
                        filter.template setBypassed<2>(false);
                        updateFilterState(filter.template get<2>().coefficients, coefficients[2]);
                    case 2:
                        filter.template setBypassed<1>(false);
                        updateFilterState(filter.template get<1>().coefficients, coefficients[1]);
                    case 1:
                        filter.template setBypassed<0>(false);
                        updateFilterState(filter.template get<0>().coefficients, coefficients[0]);
                }
            }
        }
    }
};
