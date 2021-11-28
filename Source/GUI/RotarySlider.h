/*
  ==============================================================================

    RotarySlider.h
    Created: 31 Oct 2021 2:34:36pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

//==============================================================================
/*
*/
struct RotarySlider  : public juce::Slider
{
    RotarySlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                            juce::Slider::TextEntryBoxPosition::NoTextBox), param(&rap), suffix(unitSuffix)
        {
            setLookAndFeel(&lnf);
        }
        
        ~RotarySlider()
        {
            setLookAndFeel(nullptr);
        }
        
        struct LabelPos
        {
            float pos;
            juce::String label;
        };
        
        
        void paint(juce::Graphics& g) override ;
    
        juce::Rectangle<int> getSliderBounds() const;
    
        int getTextHeight() const { return 14; }
    
        juce::String getDisplayString() const;
    
    private:
        LookAndFeel lnf;
        juce::RangedAudioParameter* param;
        juce::String suffix;
};
