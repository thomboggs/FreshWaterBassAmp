/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
FreshwaterAudioProcessorEditor::FreshwaterAudioProcessorEditor (FreshwaterAudioProcessor& p)
    : AudioProcessorEditor (&p)
, processor (p)
, inputGainSliderAttachment(processor.apvts, getInputGainParamName(), inputGainSlider)
, outputGainSliderAttachment(processor.apvts, getOutputGainParamName(), outputGainSlider)
, lowFreqSliderAttachment(processor.apvts, getFreqParamName(0), lowFreqSlider)
, lowOrderSliderAttachment(processor.apvts, getQualityParamName(0), lowOrderSlider)
, midFreqSliderAttachment(processor.apvts, getFreqParamName(1), midFreqSlider)
, midGainSliderAttachment(processor.apvts, getGainParamName(1), midGainSlider)
, midQualitySliderAttachment(processor.apvts, getQualityParamName(1), midQualitySlider)
, highFreqSliderAttachment(processor.apvts, getFreqParamName(2), highFreqSlider)
, highOrderSliderAttachment(processor.apvts, getQualityParamName(2), highOrderSlider)
, compAttackSliderAttachment(processor.apvts, getCompAttackParamName(), compAttackSlider)
, compReleaseSliderAttachment(processor.apvts, getCompReleaseParamName(), compReleaseSlider)
, compThresholdSliderAttachment(processor.apvts, getCompThresholdParamName(), compThresholdSlider)
, compBypassButtonAttachment(processor.apvts, getCompBypassParamName(), compBypassButton)
, lowBypassButtonAttachment(processor.apvts, getBypassParamName(0), lowBypassButton)
, midBypassButtonAttachment(processor.apvts, getBypassParamName(1), midBypassButton)
, compRatioSliderAttachment(processor.apvts, getCompRatioParamName(), compRatioSlider)
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
    }
    
    for ( auto slider : getCompFilterSliders() )
    {
        slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    }
}

FreshwaterAudioProcessorEditor::~FreshwaterAudioProcessorEditor()
{
}

//==============================================================================
void FreshwaterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    
}

void FreshwaterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto titleBar = bounds.removeFromTop(10);
    auto inputGainArea = bounds.removeFromLeft(bounds.getWidth() * 0.125);
    auto compressorArea = bounds.removeFromLeft(bounds.getWidth() * 0.28);
    auto filterArea = bounds.removeFromLeft(bounds.getWidth() * 0.8);
    
    inputGainSlider.setBounds(inputGainArea);
    
    // Compressor Layout
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
    auto filterResponseArea = filterArea.removeFromTop(filterArea.getHeight() * 0.5);
    auto lowFilterArea = filterArea.removeFromLeft(filterArea.getWidth() * 0.33);
    auto midFilterArea = filterArea.removeFromLeft(filterArea.getWidth() * 0.5);
    auto highFilterArea = filterArea;
    
    // Low Filter
    auto lowFreqArea = lowFilterArea.removeFromTop(lowFilterArea.getHeight() * 0.33);
    auto lowOrderArea = lowFilterArea.removeFromTop(lowFilterArea.getHeight() * 0.5);
    auto lowBypassArea = lowFilterArea;
    
    lowFreqSlider.setBounds(lowFreqArea);
    lowOrderSlider.setBounds(lowOrderArea);
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
    auto highFreqArea = highFilterArea.removeFromTop(highFilterArea.getHeight() * 0.33);
    auto highOrderArea = highFilterArea.removeFromTop(highFilterArea.getHeight() * 0.5);
    auto highBypassArea = highFilterArea;
    
    highFreqSlider.setBounds(highFreqArea);
    highOrderSlider.setBounds(highOrderArea);
    highBypassButton.setBounds(highBypassArea);
    
    // Output Gain
    outputGainSlider.setBounds(bounds);
    
}


std::vector<juce::Component*> FreshwaterAudioProcessorEditor::getComps()
{
    return
    {
        &inputGainSlider,
        &outputGainSlider,
        &lowFreqSlider,
        &lowOrderSlider,
        &midFreqSlider,
        &midGainSlider,
        &midQualitySlider,
        &highFreqSlider,
        &highOrderSlider,
        &compBypassButton,
        &lowBypassButton,
        &midBypassButton,
        &highBypassButton,
        &compAttackSlider,
        &compReleaseSlider,
        &compThresholdSlider,
        &compRatioSlider
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
        &lowOrderSlider,
        &midFreqSlider,
        &midGainSlider,
        &midQualitySlider,
        &highFreqSlider,
        &highOrderSlider,
        &compAttackSlider,
        &compReleaseSlider,
        &compThresholdSlider,
        &compRatioSlider
    };
}
