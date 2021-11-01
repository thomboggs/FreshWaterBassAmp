/*
  ==============================================================================

    GainSlider.h
    Created: 31 Oct 2021 11:52:20pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GainSlider  : public juce::Slider
{
public:
    GainSlider();
    ~GainSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String suffix { "dB" };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainSlider)
};
