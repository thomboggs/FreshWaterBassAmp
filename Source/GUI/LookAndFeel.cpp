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
    
    // Dial Color = light blue
    g.setColour(Colour(118, 161, 178));
    g.fillEllipse(bounds);
    
    // Dial Outline
    g.setColour(Colours::darkgreen);
    g.drawEllipse(bounds, 1.f);
    
    if ( auto* rs = dynamic_cast<RotarySlider*>(&slider))
    {
        // Draw Line Pointer
        auto center = bounds.getCentre();
        
        Path p;
        
        Rectangle<float> rec;
        rec.setLeft(center.getX() - 2);
        rec.setRight(center.getX() + 2);
        rec.setTop(bounds.getY());
        rec.setBottom(center.getY() - 2);
        
        p.addRoundedRectangle(rec, 2.f);
        
        jassert (rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngInRadians = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated( sliderAngInRadians, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        // Draw Text
        g.setFont(rs->getTextHeight());
        auto text  = rs->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        rec.setSize(strWidth + 4, rs->getTextHeight() + 2);
        rec.setCentre(bounds.getCentre());
        
        g.setColour(Colours::black);
        g.fillRect(rec);
        
        g.setColour(Colours::white);
        g.drawFittedText(text, rec.toNearestInt(), juce::Justification::centred, 1);
    }
}


void LookAndFeel::drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                       float sliderPos,
                       float minSliderPos,
                       float maxSliderPos,
                       const juce::Slider::SliderStyle,
                       juce::Slider&)
{
    
}
