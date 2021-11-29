/*
  ==============================================================================

    CompressorResponseComponent.h
    Created: 28 Nov 2021 5:40:18pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CompressorResponseComponent  : public juce::Component
{
public:
    CompressorResponseComponent();
    ~CompressorResponseComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorResponseComponent)
};
