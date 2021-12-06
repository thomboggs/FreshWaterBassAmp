/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterParametersBase.h"
#include "CoefficientsMaker.h"
#include "FilterInfo.h"
#include "Fifo.h"
#include "FilterCoefficientGenerator.h"
#include "ReleasePool.h"
#include "Compressor.h"
#include "Gain.h"
#include "FilterLink.h"
#include "FilterHelperFunctions.h"


//==============================================================================
/**
*/
class FreshwaterAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FreshwaterAudioProcessor();
    ~FreshwaterAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // APVTS and Audio Parameter Creation
    static void createCompressorParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void createGainParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void createCutParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout, const int filterNum, const bool isLowCut);
    static void createFilterParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout, const int filterNum);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout ();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Params", createParameterLayout() };
    
private:
    Compressor compressor;
    Gain inputGain, outputGain;
    
    void attachCompressorGainParams ();
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using CoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    
    using SingleFilter = FilterLink<Filter, CoefficientsPtr, FilterParameters, CoefficientsMaker<float>>;
    using Chain = juce::dsp::ProcessorChain<SingleFilter,
                                            SingleFilter,
                                            SingleFilter>;
    static const int chainLength { 3 };
    
    Chain leftChain, rightChain;
    
    void initializeFilters (const double sampleRate, const float rampTime);
    
    template <int Index, typename ParamType>
    void initializeFilter (const double sampleRate, const float rampTime);
    
    template <typename ParamType>
    ParamType getParams (const int bandNum, const double sampleRate);
    
    template <int Index>
    void setChainBypass(const bool isBypassed);
    
    void updateFilterParams ();
    
    template <int Index, typename ParamType>
    void updateSingleFilterParams ();
    
    void updateFilterState (const int chunkSize);
    
    template <int Index>
    void updateSingleFilterState (const bool onRealTimeThread, const int chunkSize);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreshwaterAudioProcessor)
};


//==============================================================================
/*
        Template Function Definition
 */
//==============================================================================

template<typename ParamType>
ParamType FreshwaterAudioProcessor::getParams (const int bandNum, const double sampleRate)
{
    ParamType params;
    
    params.sampleRate = sampleRate;
    
    if constexpr (std::is_same_v<ParamType, FilterParameters>)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getTypeParamName(bandNum))))
        {
//            DBG("getParams filtertype");
//            DBG(bandNum);
            switch (bandNum)
            {
                case 0:
                    params.filterType = FilterInfo::FilterType::LowShelf;
                    break;
                case 1:
                    params.filterType = FilterInfo::FilterType::Peak;
                    break;
                case 2:
                    params.filterType = FilterInfo::FilterType::HighShelf;
                    break;
            }
        
        }

        if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getGainParamName(bandNum))))
        {
            params.gain.setDb(p->get());
        }
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    return params;
}


template<int Index>
void FreshwaterAudioProcessor::setChainBypass(const bool isBypassed)
{
    leftChain.template setBypassed<Index>(isBypassed);
    rightChain.template setBypassed<Index>(isBypassed);
}


template <int Index, typename ParamType>
void FreshwaterAudioProcessor::initializeFilter (const double sampleRate, const float rampTime)
{
    auto tempCutParams = getParams<ParamType>(Index, sampleRate);
    leftChain.get<Index>().initialize(tempCutParams, rampTime, false, sampleRate);
    rightChain.get<Index>().initialize(tempCutParams, rampTime, false, sampleRate);
    
}


template <int Index, typename ParamType>
void FreshwaterAudioProcessor::updateSingleFilterParams ()
{
    auto sampleRate = getSampleRate();
    leftChain.get<Index>().performPreloopUpdate(getParams<ParamType>(Index, sampleRate));
    rightChain.get<Index>().performPreloopUpdate(getParams<ParamType>(Index, sampleRate));
}

template <int Index>
void FreshwaterAudioProcessor::updateSingleFilterState (const bool onRealTimeThread, const int chunkSize)
{
    leftChain.get<Index>().performInnerLoopFilterUpdate(onRealTimeThread, chunkSize);
    rightChain.get<Index>().performInnerLoopFilterUpdate(onRealTimeThread, chunkSize);
}
