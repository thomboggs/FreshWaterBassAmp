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
#include "./GUI/RotarySlider.h"
#include "./GUI/ResponseCurveComponent.h"
#include "./GUI/CompressorResponseComponent.h"

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
    
    RotarySlider    lowFreqSlider,
                    lowGainSlider,
                    lowQualitySlider,
                    midFreqSlider,
                    midGainSlider,
                    midQualitySlider,
                    highFreqSlider,
                    highGainSlider,
                    highQualitySlider;
    
    juce::ToggleButton compBypassButton, lowBypassButton, midBypassButton, highBypassButton;
    
    RotarySlider    compAttackSlider,
                    compReleaseSlider,
                    compThresholdSlider,
                    compRatioSlider;
    
    juce::Label nameLabel,
                inGainLabel,
                compSectionLabel,
                compAttackLabel,
                compReleaseLabel,
                compThresholdLabel,
                compRatioLabel,
                equalizerSectionLabel,
                lowFilterSectionLabel,
                midFilterSectionLabel,
                highFilterSectionLabel,
                outGainLabel;
    
    ResponseCurveComponent responseCurveComponent;
    CompressorResponseComponent compressorResponseComponent;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using sliderAttachment = APVTS::SliderAttachment;
    
    sliderAttachment  inputGainSliderAttachment
                    , outputGainSliderAttachment
                    , lowFreqSliderAttachment
                    , lowGainSliderAttachment
                    , lowQualitySliderAttachment
                    , midFreqSliderAttachment
                    , midGainSliderAttachment
                    , midQualitySliderAttachment
                    , highFreqSliderAttachment
                    , highGainSliderAttachment
                    , highQualitySliderAttachment
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
    void initializeSectionLabels();
    void initializeLabel(juce::Label& label, const juce::String labelText, const float textSize);

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreshwaterAudioProcessorEditor)
};
