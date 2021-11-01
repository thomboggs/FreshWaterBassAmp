/*
  ==============================================================================

    RotarySlider.h
    Created: 31 Oct 2021 2:34:36pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
struct RotarySlider  : public juce::Component
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                            juce::Slider::TextEntryBoxPosition::NoTextBox), param(&rap), suffix(unitSuffix)
        {
//            setLookAndFeel(&lnf);
        }
        
        ~RotarySliderWithLabels()
        {
//            setLookAndFeel(nullptr);
        }
        
        struct LabelPos
        {
            float pos;
            juce::String label;
        };
        
        juce::Array<LabelPos> labels;
        
        void paint(juce::Graphics& g) override ;
    
        juce::Rectangle<int> getSliderBounds() const;
    
        int getTextHeight() const { return 14; }
    
        juce::String getDisplayString() const;
    
    private:
        LookAndFeel lnf;
        juce::RangedAudioParameter* param;
        juce::String suffix;
};


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override ;
    
    void drawToggleButton (juce::Graphics& g,
                           juce::ToggleButton& toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

};
