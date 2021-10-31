/*
  ==============================================================================

    FilterInfo.cpp
    Created: 10 Oct 2021 9:16:36pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "FilterInfo.h"


const std::map<FilterInfo::FilterType, juce::String>& FilterInfo::getFilterTypeMap()
{
    static std::map<FilterInfo::FilterType, juce::String> FilterTypeMap
    {
        {FilterInfo::FilterType::FirstOrderLowPass, "FirstOrderLowPass"},
        {FilterInfo::FilterType::FirstOrderHighPass, "FirstOrderHighPass"},
        {FilterInfo::FilterType::FirstOrderAllPass, "FirstOrderAllPass"},
        {FilterInfo::FilterType::LowPass, "LowPass"},
        {FilterInfo::FilterType::HighPass, "HighPass"},
        {FilterInfo::FilterType::BandPass, "BandPass"},
        {FilterInfo::FilterType::Notch, "Notch"},
        {FilterInfo::FilterType::AllPass, "AllPass"},
        {FilterInfo::FilterType::LowShelf, "LowShelf"},
        {FilterInfo::FilterType::HighShelf, "HighShelf"},
        {FilterInfo::FilterType::Peak, "Peak"},
    };
    
    return FilterTypeMap;
};
