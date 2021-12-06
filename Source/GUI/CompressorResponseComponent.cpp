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
    
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
    
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));
    
}

void CompressorResponseComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


juce::Rectangle<int> CompressorResponseComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

//    bounds.reduce(10, //JUCE_LIVE_CONSTANT(5),
//                  8 //JUCE_LIVE_CONSTANT(5)
//                  );
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
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
