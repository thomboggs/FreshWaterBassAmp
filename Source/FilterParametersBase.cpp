/*
  ==============================================================================

    FilterParametersBase.cpp
    Created: 1 Nov 2021 9:00:22pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include "FilterParametersBase.h"


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
        return (lhs.filterType == rhs.filterType) && (lhs.gain == rhs.gain);
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

