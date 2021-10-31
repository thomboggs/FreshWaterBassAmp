/*
  ==============================================================================

    FilterParametersBase.h
    Created: 10 Oct 2021 9:44:34pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "FilterInfo.h"


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
        filterType(filterType), gainInDb(gainInDb)
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
    float gainInDb { 0 };
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




bool operator== (const FilterParametersBase& lhs, const FilterParametersBase& rhs)
{
    return (lhs.frequency == rhs.frequency)
            && (lhs.bypassed == rhs.bypassed)
            && (lhs.quality == rhs.quality)
            && (lhs.sampleRate == rhs.sampleRate);
}


bool operator!= (const FilterParametersBase& lhs, const FilterParametersBase& rhs)
{
    return !(lhs == rhs);
}


bool operator== (const FilterParameters& lhs, const FilterParameters& rhs)
{
    // check if base class members are the same
    if ( static_cast<const FilterParametersBase&>(lhs) == static_cast<const FilterParametersBase&>(rhs))
    {
        return (lhs.filterType == rhs.filterType) && (lhs.gainInDb == rhs.gainInDb);
    }
    // If base class parameters aren't equal, return false
    return false;
}

bool operator!= (const FilterParameters& lhs, const FilterParameters& rhs)
{
    return !(lhs == rhs);
}


bool operator== (const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs)
{
    // check if base class members are the same
    if (static_cast<const FilterParametersBase&>(lhs) == static_cast<const FilterParametersBase&>(rhs))
    {
        return (lhs.order == rhs.order) && (lhs.isLowcut == rhs.isLowcut);
    }
    // If base class parameters aren't equal, return false
    return false;
}

bool operator!= (const HighCutLowCutParameters& lhs, const HighCutLowCutParameters& rhs)
{
    return !(lhs == rhs);
}



