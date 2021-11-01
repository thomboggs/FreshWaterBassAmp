/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "FilterHelperFunctions.h"

//==============================================================================
/**
*/
class FreshwaterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FreshwaterAudioProcessorEditor (FreshwaterAudioProcessor&);
    ~FreshwaterAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FreshwaterAudioProcessor& processor;
    
    
    juce::Slider inputGainSlider, outputGainSlider;
    
    juce::Slider lowFreqSlider, lowOrderSlider;
    juce::Slider midFreqSlider, midGainSlider, midQualitySlider;
    juce::Slider highFreqSlider, highOrderSlider;
    
    juce::ToggleButton compBypassButton, lowBypassButton, midBypassButton, highBypassButton;
    
    juce::Slider compAttackSlider, compReleaseSlider, compThresholdSlider, compRatioSlider;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using sliderAttachment = APVTS::SliderAttachment;
    
    sliderAttachment  inputGainSliderAttachment
                    , outputGainSliderAttachment
                    , lowFreqSliderAttachment
                    , lowOrderSliderAttachment
                    , midFreqSliderAttachment
                    , midGainSliderAttachment
                    , midQualitySliderAttachment
                    , highFreqSliderAttachment
                    , highOrderSliderAttachment
                    , compAttackSliderAttachment
                    , compReleaseSliderAttachment
                    , compThresholdSliderAttachment
                    , compRatioSliderAttachment;
    
    using buttonAttachment = APVTS::ButtonAttachment;
    
    buttonAttachment  compBypassButtonAttachment
                    , lowBypassButtonAttachment
                    , midBypassButtonAttachment
                    , highBypassButtonAttachment;
    
    std::vector<juce::Component*> getComps();
    std::vector<juce::Slider*> getGainSliders();
    std::vector<juce::Slider*> getCompFilterSliders();

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreshwaterAudioProcessorEditor)
};
