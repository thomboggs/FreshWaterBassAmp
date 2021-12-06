/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "FilterHelperFunctions.h"

//==============================================================================
FreshwaterAudioProcessorEditor::FreshwaterAudioProcessorEditor (FreshwaterAudioProcessor& p)
    : AudioProcessorEditor (&p)
, processor (p)
, lowFreqSlider(*processor.apvts.getParameter(getFreqParamName(0)), "Hz")
, lowGainSlider(*processor.apvts.getParameter(getGainParamName(0)), "dB")
, lowQualitySlider(*processor.apvts.getParameter(getQualityParamName(0)), "")
, midFreqSlider(*processor.apvts.getParameter(getFreqParamName(1)), "Hz")
, midGainSlider(*processor.apvts.getParameter(getGainParamName(1)), "dB")
, midQualitySlider(*processor.apvts.getParameter(getQualityParamName(1)), "")
, highFreqSlider(*processor.apvts.getParameter(getFreqParamName(2)), "Hz")
, highGainSlider(*processor.apvts.getParameter(getGainParamName(2)), "dB")
, highQualitySlider(*processor.apvts.getParameter(getQualityParamName(2)), "")
, compAttackSlider(*processor.apvts.getParameter(getCompAttackParamName()), "ms")
, compReleaseSlider(*processor.apvts.getParameter(getCompReleaseParamName()), "ms")
, compThresholdSlider(*processor.apvts.getParameter(getCompThresholdParamName()), "dB")
, compRatioSlider(*processor.apvts.getParameter(getCompRatioParamName()), "")
, responseCurveComponent(processor)
, compressorResponseComponent(processor)
, inputGainSliderAttachment(processor.apvts, getInputGainParamName(), inputGainSlider)
, outputGainSliderAttachment(processor.apvts, getOutputGainParamName(), outputGainSlider)
, lowFreqSliderAttachment(processor.apvts, getFreqParamName(0), lowFreqSlider)
, lowGainSliderAttachment(processor.apvts, getGainParamName(0), lowGainSlider)
, lowQualitySliderAttachment(processor.apvts, getQualityParamName(0), lowQualitySlider)
, midFreqSliderAttachment(processor.apvts, getFreqParamName(1), midFreqSlider)
, midGainSliderAttachment(processor.apvts, getGainParamName(1), midGainSlider)
, midQualitySliderAttachment(processor.apvts, getQualityParamName(1), midQualitySlider)
, highFreqSliderAttachment(processor.apvts, getFreqParamName(2), highFreqSlider)
, highGainSliderAttachment(processor.apvts, getGainParamName(2), highGainSlider)
, highQualitySliderAttachment(processor.apvts, getQualityParamName(2), highQualitySlider)
, compAttackSliderAttachment(processor.apvts, getCompAttackParamName(), compAttackSlider)
, compReleaseSliderAttachment(processor.apvts, getCompReleaseParamName(), compReleaseSlider)
, compThresholdSliderAttachment(processor.apvts, getCompThresholdParamName(), compThresholdSlider)
, compRatioSliderAttachment(processor.apvts, getCompRatioParamName(), compRatioSlider)
, compBypassButtonAttachment(processor.apvts, getCompBypassParamName(), compBypassButton)
, lowBypassButtonAttachment(processor.apvts, getBypassParamName(0), lowBypassButton)
, midBypassButtonAttachment(processor.apvts, getBypassParamName(1), midBypassButton)
, highBypassButtonAttachment(processor.apvts, getBypassParamName(2), highBypassButton)


{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 600);
    
    for ( auto comp : getComps() )
    {
        addAndMakeVisible(comp);
    }
    
    for ( auto slider : getGainSliders() )
    {
        slider->setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
//        slider->getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::forestgreen);
    }
    
    for ( auto slider : getCompFilterSliders() )
    {
        slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    }
    
    initializeSectionLabels();
}

FreshwaterAudioProcessorEditor::~FreshwaterAudioProcessorEditor()
{
}

//==============================================================================
void FreshwaterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(3, 45, 184));
}

void FreshwaterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto titleBar = bounds.removeFromTop(30);
    nameLabel.setBounds(titleBar);
    
    auto inputGainArea = bounds.removeFromLeft(bounds.getWidth() * 0.125);
    auto inputGainTitleArea = inputGainArea.removeFromTop(20);
    
    inGainLabel.setBounds(inputGainTitleArea);
    inputGainSlider.setBounds(inputGainArea);
    
    auto compressorArea = bounds.removeFromLeft(bounds.getWidth() * 0.28);
    auto compressorTitleArea = compressorArea.removeFromTop(20);
    auto compressorResponseArea = compressorArea.removeFromTop(compressorArea.getHeight() * 0.5);
    
    compSectionLabel.setBounds(compressorTitleArea);
    compressorResponseComponent.setBounds(compressorResponseArea);
    
    auto filterArea = bounds.removeFromLeft(bounds.getWidth() * 0.8);
    
    // Compressor Layout
    compressorArea.removeFromTop(30);
    auto compBypassArea = compressorArea.removeFromBottom(compressorArea.getHeight() * 0.2);
    auto compAreaLeft = compressorArea.removeFromLeft(compressorArea.getWidth() * 0.5);
    auto compAreaTopLeft = compAreaLeft.removeFromTop(compAreaLeft.getHeight() * 0.5);
    auto compAreaTopRight = compressorArea.removeFromTop(compressorArea.getHeight() * 0.5);
    
    compAttackSlider.setBounds(compAreaTopLeft);
    compReleaseSlider.setBounds(compAreaTopRight);
    compThresholdSlider.setBounds(compAreaLeft);
    compRatioSlider.setBounds(compressorArea);
    compBypassButton.setBounds(compBypassArea);
    
    // Filter Layout
    auto eqTitleArea = filterArea.removeFromTop(20);
    auto spectrumResponseArea = filterArea.removeFromTop(filterArea.getHeight() * 0.5);
    
    equalizerSectionLabel.setBounds(eqTitleArea);
    responseCurveComponent.setBounds(spectrumResponseArea);
    
    auto lowFilterArea = filterArea.removeFromLeft(filterArea.getWidth() * 0.33);
    auto midFilterArea = filterArea.removeFromLeft(filterArea.getWidth() * 0.5);
    auto highFilterArea = filterArea;
    
    auto lowFilterTitleArea = lowFilterArea.removeFromTop(20);
    auto midFilterTitleArea = midFilterArea.removeFromTop(20);
    auto highFilterTitleArea = highFilterArea.removeFromTop(20);
    
    lowFilterSectionLabel.setBounds(lowFilterTitleArea);
    midFilterSectionLabel.setBounds(midFilterTitleArea);
    highFilterSectionLabel.setBounds(highFilterTitleArea);
    
    // Low Filter
    auto lowFreqArea = lowFilterArea.removeFromTop(lowFilterArea.getHeight() * 0.25);
    auto lowGainArea = lowFilterArea.removeFromTop(lowFilterArea.getHeight() * 0.33);
    auto lowQualityArea = lowFilterArea.removeFromTop(lowFilterArea.getHeight() * 0.5);
    auto lowBypassArea = lowFilterArea;
    
    lowFreqSlider.setBounds(lowFreqArea);
    lowGainSlider.setBounds(lowGainArea);
    lowQualitySlider.setBounds(lowQualityArea);
    lowBypassButton.setBounds(lowBypassArea);
    
    // mid filter
    auto midFreqArea = midFilterArea.removeFromTop(midFilterArea.getHeight() * 0.25);
    auto midGainArea = midFilterArea.removeFromTop(midFilterArea.getHeight() * 0.33);
    auto midQualityArea = midFilterArea.removeFromTop(midFilterArea.getHeight() * 0.5);
    auto midBypassArea = midFilterArea;
    
    midFreqSlider.setBounds(midFreqArea);
    midGainSlider.setBounds(midGainArea);
    midQualitySlider.setBounds(midQualityArea);
    midBypassButton.setBounds(midBypassArea);
    
    // high Filter
    auto highFreqArea = highFilterArea.removeFromTop(highFilterArea.getHeight() * 0.25);
    auto highGainArea = highFilterArea.removeFromTop(highFilterArea.getHeight() * 0.33);
    auto highQualityArea = highFilterArea.removeFromTop(highFilterArea.getHeight() * 0.5);
    auto highBypassArea = highFilterArea;
    
    highFreqSlider.setBounds(highFreqArea);
    highGainSlider.setBounds(highGainArea);
    highQualitySlider.setBounds(highQualityArea);
    highBypassButton.setBounds(highBypassArea);
    
    // Output Gain
    outGainLabel.setBounds(bounds.removeFromTop(20));
    outputGainSlider.setBounds(bounds);
    
}


std::vector<juce::Component*> FreshwaterAudioProcessorEditor::getComps()
{
    return
    {
        &inputGainSlider,
        &outputGainSlider,
        &lowFreqSlider,
        &lowGainSlider,
        &lowQualitySlider,
        &midFreqSlider,
        &midGainSlider,
        &midQualitySlider,
        &highFreqSlider,
        &highGainSlider,
        &highQualitySlider,
        &compBypassButton,
        &lowBypassButton,
        &midBypassButton,
        &highBypassButton,
        &compAttackSlider,
        &compReleaseSlider,
        &compThresholdSlider,
        &compRatioSlider,
        &responseCurveComponent,
        &compressorResponseComponent
    };
}


std::vector<juce::Slider*> FreshwaterAudioProcessorEditor::getGainSliders()
{
    return
    {
        &inputGainSlider,
        &outputGainSlider
    };
}


std::vector<juce::Slider*> FreshwaterAudioProcessorEditor::getCompFilterSliders()
{
    return
    {
        &lowFreqSlider,
        &lowGainSlider,
        &lowQualitySlider,
        &midFreqSlider,
        &midGainSlider,
        &midQualitySlider,
        &highFreqSlider,
        &highGainSlider,
        &highQualitySlider,
        &compAttackSlider,
        &compReleaseSlider,
        &compThresholdSlider,
        &compRatioSlider
    };
}


void FreshwaterAudioProcessorEditor::initializeSectionLabels()
{
    initializeLabel(nameLabel, "FreshWater Bass Amp", 25.0f);
    initializeLabel(inGainLabel, "Input Gain", 16.0f);
    initializeLabel(compSectionLabel, "Compressor", 16.0f);
    
    initializeLabel(compAttackLabel, "Attack", 16.0f);
    compAttackLabel.attachToComponent(&compAttackSlider, false);
    initializeLabel(compReleaseLabel, "Release", 16.0f);
    compReleaseLabel.attachToComponent(&compReleaseSlider, false);
    initializeLabel(compThresholdLabel, "Threshold", 16.0f);
    compThresholdLabel.attachToComponent(&compThresholdSlider, false);
    initializeLabel(compRatioLabel, "Ratio", 16.0f);
    compRatioLabel.attachToComponent(&compRatioSlider, false);
    
    initializeLabel(equalizerSectionLabel, "Equalizer", 16.0f);
    initializeLabel(lowFilterSectionLabel, "Bass", 16.0f);
    initializeLabel(midFilterSectionLabel, "Mid", 16.0f);
    initializeLabel(highFilterSectionLabel, "Treble", 16.0f);
    initializeLabel(outGainLabel, "Output Gain", 16.0f);
}

void FreshwaterAudioProcessorEditor::initializeLabel(juce::Label& label, const juce::String labelText, const float textSize)
{
    addAndMakeVisible(label);
    label.setFont (juce::Font (textSize, juce::Font::bold));
    label.setText (labelText, juce::dontSendNotification);
    label.setColour (juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType (juce::Justification::centred);
}
