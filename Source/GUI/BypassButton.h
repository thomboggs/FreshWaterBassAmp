/*
  ==============================================================================

    BypassButton.h
    Created: 31 Oct 2021 10:26:56pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class BypassButton  : public juce::Component
{
public:
    BypassButton();
    ~BypassButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BypassButton)
};
