/*
  ==============================================================================

    RotarySliderLookAndFeel.h
    Created: 31 Oct 2021 2:36:01pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override ;
    
    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const juce::Slider::SliderStyle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics& g,
                           juce::ToggleButton& toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};
