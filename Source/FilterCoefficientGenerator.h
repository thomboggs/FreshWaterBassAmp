/*
  ==============================================================================

    FilterCoefficientGenerator.h
    Created: 23 Oct 2021 5:33:31pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterParametersBase.h"
#include "Fifo.h"


//=================================================================================
template <typename T>
struct IsCutParameterType : std::false_type { };

template <>
struct IsCutParameterType<HighCutLowCutParameters> : std::true_type { };

//=================================================================================
template<typename CoefficientType, class ParamType, class MakeFunction, size_t Size>
struct FilterCoefficientGenerator : juce::Thread
{
    FilterCoefficientGenerator(Fifo<CoefficientType, Size>& fifo_, juce::String threadName) : Thread(threadName), coefficientsFifo(fifo_)
    {
        startThread();
    }
    
    ~FilterCoefficientGenerator()
    {
        stopThread(100);
    }
    
    // changeParameters will push params into a Fifo<> that the run() function will consume.
    void changeParameters(ParamType params)
    {
        paramFifo.push(params);
        parametersChanged.set(true);
//        DBG("FCG::changeParameters - Set parametersChanged to true");
    }
    
    
    void run() override
    {
        ParamType params;
        while ( ! threadShouldExit() )
        {
//            DBG("FCG::run - Checking if Parameters have changed");
            // If parametersChanged == true, compute new coefficients
            if ( parametersChanged.compareAndSetBool(false, true) )
            {
//                DBG("FCG::run - about to pull params from FIFO");
                while (paramFifo.pull(params))
                {
//                    DBG("FCG::run - pulled params from FIFO");
                    if constexpr ( IsCutParameterType<ParamType>::value )
                    {
                        // Calc Cut Parameters
                        auto cutCoeffArray = MakeFunction::calcCoefficients(params);
                        
                        if ( cutCoeffArray.size() > 0 )
                            coefficientsFifo.push(cutCoeffArray);
                    }
                    else
                    {
                        // Calc Filter Parameters
                        auto filterCoeffs = MakeFunction::calcCoefficients(params);
                        
                        // Check if not null
                        if (filterCoeffs.get() != nullptr)
                            coefficientsFifo.push(filterCoeffs);
                    }
                    
                    if (threadShouldExit()) break;
                }
            }
            
            // Wait
            wait(10);
        }
    }
    

private:
    juce::Atomic<bool> parametersChanged { false };
    
    Fifo<ParamType, Size> paramFifo;
    Fifo<CoefficientType, Size>& coefficientsFifo;

};
