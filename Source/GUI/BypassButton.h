/*
  ==============================================================================

    BypassButton.h
    Created: 5 Dec 2021 1:27:18pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
struct BypassButton : juce::ToggleButton
{
    BypassButton()
    {
        setLookAndFeel(&lnf);
    }
    
    ~BypassButton()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override
    {
        getLookAndFeel().drawToggleButton(g, *this, true, true);
    }
    
private:
    LookAndFeel lnf;
};
