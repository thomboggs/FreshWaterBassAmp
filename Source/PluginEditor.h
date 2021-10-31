/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Pfmcpp_project11AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Pfmcpp_project11AudioProcessorEditor (Pfmcpp_project11AudioProcessor&);
    ~Pfmcpp_project11AudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Pfmcpp_project11AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project11AudioProcessorEditor)
};
