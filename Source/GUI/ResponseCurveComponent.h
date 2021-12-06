/*
  ==============================================================================

    ResponseCurveComponent.h
    Created: 31 Oct 2021 2:36:58pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../FilterParametersBase.h"


//==============================================================================
/*
*/
class ResponseCurveComponent  : public  juce::Component, juce::AudioProcessorParameter::Listener,
    juce::Timer
{
public:
    ResponseCurveComponent(FreshwaterAudioProcessor&);
    ~ResponseCurveComponent() override;

    void parameterValueChanged (int parameterIndex, float newValue) override;
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {};
    
    void timerCallback() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    

private:
    FreshwaterAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged { true };
    
    using Filter = juce::dsp::IIR::Filter<float>;
//    using CoefficientsPtr = juce::dsp::IIR::Filter<float>::CoefficientsPtr;
    
//    using SingleFilter = FilterLink<Filter, CoefficientsPtr, FilterParameters, CoefficientsMaker<float>>;
    using Chain = juce::dsp::ProcessorChain<Filter,
                                            Filter,
                                            Filter>;
    static const int chainLength { 3 };
    
    Chain chain;
    
    FilterParameters updateFilter(const int bandNum);
    void updateCoefficients();
    void updateChain();
    
    juce::Image background;
    
    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResponseCurveComponent)
};
