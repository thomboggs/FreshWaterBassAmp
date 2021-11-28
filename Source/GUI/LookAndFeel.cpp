/*
  ==============================================================================

    RotarySliderLookAndFeel.cpp
    Created: 31 Oct 2021 2:36:01pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "RotarySlider.h"

void LookAndFeel::drawRotarySlider (juce::Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPosProportional,
                                    float rotaryStartAngle,
                                    float rotaryEndAngle,
                                    juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
}


void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    
}
