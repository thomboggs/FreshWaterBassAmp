/*
  ==============================================================================

    CompressorResponseComponent.cpp
    Created: 28 Nov 2021 5:40:18pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CompressorResponseComponent.h"
#include "../FilterHelperFunctions.h"

//==============================================================================
CompressorResponseComponent::CompressorResponseComponent(FreshwaterAudioProcessor& p) :
 audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for ( auto param : params)
    {
        param->addListener(this);
    }
    
    startTimerHz(60);
}

CompressorResponseComponent::~CompressorResponseComponent()
{
    const auto& params = audioProcessor.getParameters();
    for ( auto param : params)
    {
        param->removeListener(this);
    }
}


void CompressorResponseComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}


void CompressorResponseComponent::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        // signal a repaint
        repaint();
    }
}


void CompressorResponseComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

//    auto r = getLocalBounds();
//    r.reduce(10, 10);
//
//    g.setColour (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//    g.fillRect(r);
//
//    g.setColour (juce::Colours::grey);
//    g.drawRect (r, 1);   // draw an outline around the component
//
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("CompressorResponseComponent", r,
//                juce::Justification::centred, true);   // draw some placeholder text
    using namespace juce;
    
    g.fillAll (Colours::black);
    g.drawImage(background, getLocalBounds().toFloat());
    
    auto responseArea = getAnalysisArea();
    
    auto w = responseArea.getWidth();
    
    std::vector<double> gains;
    
    gains.resize(w);
    
    float threshold {0}, ratio {1};
    bool bypassed {false};
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(audioProcessor.apvts.getParameter(getCompBypassParamName())))
    {
        bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.apvts.getParameter(getCompThresholdParamName())))
    {
        threshold = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter(getCompRatioParamName())))
    {
        ratio = p->getCurrentChoiceName().getFloatValue();
    }
    
    for (int i = 0; i < w; ++i)
    {
        auto inputGain = jmap ( double(i) / double(w), -60.0, 12.0 );
        
        if (!bypassed)
        {
            if (inputGain < threshold)
            {
                gains[i] = inputGain;
            }
            else
            {
                gains[i] = threshold + (inputGain - threshold) / ratio;
            }
        }
        else
        {
            gains[i] = inputGain;
        }
    }
    
    Path responseCurve;
    
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax] (double input)
    {
        return jmap(input, -60.0, 12.0, outputMin, outputMax);
    };
    
    responseCurve.startNewSubPath(responseArea.getX(), map(gains.front()));
    
    for ( size_t i = 1; i < gains.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(gains[i]));
    }
    
    g.setColour(Colours::lightskyblue);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
    
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));
    
}

void CompressorResponseComponent::resized()
{
    using namespace juce;
    
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    
    Graphics g(background);
    
    Array<float> levels
    {
        -60, -48, -36, -24, -12,
        -6, 0, 6, 12
    };
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    auto height = renderArea.getHeight();
    
    Array<float> xs, ys;
    for ( auto level : levels)
    {
        auto normLevel = jmap(level, -60.f, 12.f, 0.f, 1.f);
        xs.add(left + width * normLevel);
        
        ys.add(bottom - height * normLevel);
    }
    
    // Draw Grid
    g.setColour(Colours::dimgrey);
    for (auto x : xs )
    {
        g.drawVerticalLine(x, top, bottom);
    }
    
    for ( auto y : ys )
    {
        g.drawHorizontalLine(y, float(left), float(right));
    }
    
    // Draw Text
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    for (int i = 0; i < levels.size(); ++i)
    {
        auto l = levels[i];
        auto x = xs[i];
        auto y = ys[i];
        
        String str;
        if (l > 0)
            str << "+";
        str << l;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        //
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(bottom+5);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
        
        r.setCentre(r.getCentreX(), y);
        r.setX(1);
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
}


juce::Rectangle<int> CompressorResponseComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

//    bounds.reduce(10, //JUCE_LIVE_CONSTANT(5),
//                  8 //JUCE_LIVE_CONSTANT(5)
//                  );
    bounds.removeFromTop(2);
    bounds.removeFromBottom(12);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}


juce::Rectangle<int> CompressorResponseComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();

    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);

    return bounds;

}
