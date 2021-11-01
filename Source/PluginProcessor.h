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
    
    enum FilterPosition
    {
        LowCut,
        Multi1,
        HighCut
    };
    
    HighCutLowCutParameters currentLowCutParams, currentHighCutParams;
    FilterParameters currentFilterParams;
    
    using Filter = juce::dsp::IIR::Filter<float>;
//    using FilterChain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using FilterChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    static const int chainLength { 3 };
    
    FilterChain leftChain, rightChain;
    
    using CoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    using CutCoeffs = juce::dsp::IIR::Coefficients<float>;
    
    void setChainBypass(const bool isBypassed, FilterPosition pos);
    
    void updateParams ();
    
    void refreshLowCutFilter (Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32>& cutFifo,
                              FilterChain& chain,
                              ReleasePool<CoefficientsPtr>& cutPool);
    void refreshHighCutFilter (Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32>& cutFifo,
                              FilterChain& chain,
                              ReleasePool<CoefficientsPtr>& cutPool);
    void refreshFilters ();
    
    HighCutLowCutParameters getCutParams (int bandNum);
    FilterParameters getFilterParams (int bandNum);
    
    
    Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32> leftLowCutFifo, rightLowCutFifo;
    Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32> leftHighCutFifo, rightHighCutFifo;
    Fifo<CoefficientsPtr, 32> leftFilterCoeffFifo, rightFilterCoeffFifo;
    
    FilterCoefficientGenerator<CoefficientsPtr, FilterParameters, CoefficientsMaker<float>, 32> leftFilterFCG { leftFilterCoeffFifo , "Left Filter Thread"};
    FilterCoefficientGenerator<CoefficientsPtr, FilterParameters, CoefficientsMaker<float>, 32> rightFilterFCG { rightFilterCoeffFifo , "Right Filter Thread"};
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> leftLowCutFCG {leftLowCutFifo , "Left LowCut Thread" };
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> rightLowCutFCG {rightLowCutFifo , "Right LowCut Thread" };
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> leftHighCutFCG {leftHighCutFifo , "Left HighCut Thread" };
    FilterCoefficientGenerator<juce::ReferenceCountedArray<CutCoeffs>, HighCutLowCutParameters, CoefficientsMaker<float>, 32> rightHighCutFCG {rightHighCutFifo , "Right HighCut Thread" };
    
    ReleasePool<CoefficientsPtr> leftFilterReleasePool { }, rightFilterReleasePool { };
    ReleasePool<CoefficientsPtr> leftLowCutReleasePool { }, rightLowCutReleasePool { };
    ReleasePool<CoefficientsPtr> leftHighCutReleasePool { }, rightHighCutReleasePool { };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreshwaterAudioProcessor)
};

