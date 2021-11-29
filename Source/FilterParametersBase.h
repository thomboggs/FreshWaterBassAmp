/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterInfo.h"
#include "Decibel.h"


struct FilterParametersBase
{
    float frequency { 100.f };
    bool bypassed { false };
    float quality { 1.0 };
    double sampleRate { 0 };
    
    friend bool operator== (const FilterParametersBase& lhs, const FilterParametersBase& rhs);
    friend bool operator!= (const FilterParametersBase& lhs, const FilterParametersBase& rhs);
    
};

struct FilterParameters : FilterParametersBase
{
    FilterParameters () = default;
    FilterParameters ( float frequency, bool bypassed, float quality, double sampleRate, FilterInfo::FilterType filterType, float gainInDb) :
        filterType(filterType), gain(gainInDb)
    {
        this->frequency = frequency;
        this->bypassed = bypassed;
        this->quality = quality;
        this->sampleRate = sampleRate;
    }
    friend bool operator== (const FilterParameters& lhs, const FilterParameters& rhs);
    friend bool operator!= (const FilterParameters& lhs, const FilterParameters& rhs);
    
    // Member Variables
    FilterInfo::FilterType filterType { FilterInfo::FilterType::LowPass };
    Decibel<float> gain { 0 };
};

struct HighCutLowCutParameters : FilterParametersBase
{
    HighCutLowCutParameters () = default;
    HighCutLowCutParameters ( float frequency, bool bypassed, float quality, double sampleRate, int order, bool isLowcut) :
        order(order), isLowcut(isLowcut)
    {
        this->frequency = frequency;
        this->bypassed = bypassed;
        this->quality = quality;
        this->sampleRate = sampleRate;
    }
    friend bool operator== (const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs);
    friend bool operator!= (const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs);
    
    // Member Variables
    int order { 1 };
    bool isLowcut { true };
};






