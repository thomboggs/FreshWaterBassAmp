/*
  ==============================================================================

    FilterHelperFunctions.h
    Created: 11 Oct 2021 7:44:14pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


// Free Functions to create Parameter Names
juce::String getGainParamName(int filterNum);
juce::String getQualityParamName(int filterNum);
juce::String getFreqParamName(int filterNum);
juce::String getTypeParamName(int filterNum);
juce::String getBypassParamName(int filterNum);

juce::String getCompAttackParamName();
juce::String getCompReleaseParamName();
juce::String getCompThresholdParamName();
juce::String getCompRatioParamName();
juce::String getCompBypassParamName();
