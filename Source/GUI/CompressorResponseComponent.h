/*
  ==============================================================================

    CompressorResponseComponent.h
    Created: 28 Nov 2021 5:40:18pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

//==============================================================================
/*
*/
class CompressorResponseComponent  : public juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
public:
    CompressorResponseComponent(FreshwaterAudioProcessor&);
    ~CompressorResponseComponent() override;
    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {};
    
    void timerCallback() override;
    

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FreshwaterAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged { false };
    
    juce::Image background;
    
    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorResponseComponent)
};
