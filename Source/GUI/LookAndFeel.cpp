/*
  ==============================================================================

    RotarySliderLookAndFeel.cpp
    Created: 31 Oct 2021 2:36:01pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "RotarySlider.h"
#include "BypassButton.h"

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
        
        g.setColour(Colour(uint8(0),uint8(0), uint8(0), uint8(120)));
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


void LookAndFeel::drawToggleButton(juce::Graphics &g, juce::ToggleButton &toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    if ( auto* bb = dynamic_cast<BypassButton*>(&toggleButton) )
    {
        Path bypassButton;
        
        auto bounds = toggleButton.getLocalBounds();
        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 7; //JUCE_LIVE_CONSTANT(7);
        auto r = bounds.withSizeKeepingCentre(size, size);
        
        float ang = 30.f; //JUCE_LIVE_CONSTANT(30); // 30.f
        
        size -= 7; //JUCE_LIVE_CONSTANT(7);
        
        bypassButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY(),
                                  size * 0.5f,
                                  size * 0.5f,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360 - ang),
                                  true);
        bypassButton.startNewSubPath(r.getCentreX(), r.getY());
        bypassButton.lineTo(r.getCentre().toFloat());
        
        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightskyblue ;
        
        g.setColour(color);
        g.strokePath(bypassButton, pst);
        g.drawEllipse(r.toFloat(), 2.f);
    }
}
