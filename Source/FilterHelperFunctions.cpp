/*
  ==============================================================================

    FilterHelperFunctions.cpp
    Created: 11 Oct 2021 7:44:14pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "FilterHelperFunctions.h"


juce::String getParamPrefix(int filterNum, const juce::String& paramType)
{
    juce::String str;
    
    str << "Filter_";
    str << filterNum;
    str << "_";
    str << paramType;
    return str;
}


juce::String getGainParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Gain");
}

juce::String getQualityParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Quality");
}

juce::String getFreqParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Freq");
}

juce::String getTypeParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Type");
}

juce::String getBypassParamName(int filterNum)
{
    return getParamPrefix(filterNum, "Bypass");
}
