/*
  ==============================================================================

    FilterInfo.h
    Created: 10 Oct 2021 9:16:36pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


namespace FilterInfo
{
    enum FilterType
    {
        FirstOrderLowPass,
        FirstOrderHighPass,
        FirstOrderAllPass,
        LowPass,
        HighPass,
        BandPass,
        Notch,
        AllPass,
        LowShelf,
        HighShelf,
        Peak
    };

    const std::map<FilterType, juce::String>& getFilterTypeMap();
}
