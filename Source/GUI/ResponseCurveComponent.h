/*
  ==============================================================================

    ResponseCurveComponent.h
    Created: 31 Oct 2021 2:36:58pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ResponseCurveComponent  : public juce::Component
{
public:
    ResponseCurveComponent();
    ~ResponseCurveComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResponseCurveComponent)
};
