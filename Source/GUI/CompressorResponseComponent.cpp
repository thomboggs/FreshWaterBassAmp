/*
  ==============================================================================

    CompressorResponseComponent.cpp
    Created: 28 Nov 2021 5:40:18pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CompressorResponseComponent.h"

//==============================================================================
CompressorResponseComponent::CompressorResponseComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

CompressorResponseComponent::~CompressorResponseComponent()
{
}

void CompressorResponseComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    auto r = getLocalBounds();
    r.reduce(10, 10);
    
    g.setColour (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillRect(r);
    
    g.setColour (juce::Colours::grey);
    g.drawRect (r, 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("CompressorResponseComponent", r,
                juce::Justification::centred, true);   // draw some placeholder text
}

void CompressorResponseComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
