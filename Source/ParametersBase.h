/*
  ==============================================================================

    ParametersBase.h
    Created: 26 Oct 2021 7:09:24pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once


struct ParametersBase
{
    bool bypassed { false };
    double sampleRate { 0.0 };
    
    friend bool operator== (const ParametersBase& lhs, const ParametersBase& rhs);
    friend bool operator!= (const ParametersBase& lhs, const ParametersBase& rhs);
}


struct PreampParams : ParametersBase
{
    float attack { 20.f };
    float release { 50.f };
    float threshold { 0.f };
    float ratio { 2.f };
    float gain { 1.f };
    
    friend bool operator== (const PreampParams& lhs, const PreampParams& rhs);
    friend bool operator!= (const PreampParams& lhs, const PreampParams& rhs);
}

struct FilterParams : ParametersBase
{
    float frequency { 100.f };
    float quality { 1.f };
    float gain { 1.f };
    
    friend bool operator== (const FilterParams& lhs, const FilterParams& rhs);
    friend bool operator!= (const FilterParams& lhs, const FilterParams& rhs);
}
