/*
  ==============================================================================

    Decibel.h
    Created: 15 Oct 2021 7:28:19pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <cmath>


template <typename FloatType>
struct Decibel
{
    // Constructors
    // default constructor
    Decibel () noexcept = default;
    // a constructor that takes a FloatType parameter to initialize your member variable.
    Decibel (FloatType inputGainInDb) : gainInDb(inputGainInDb) {};
    
    // Rule of 3
    // Copy Constructor
    Decibel (const Decibel& decibel) : gainInDb(decibel.gainInDb) {};
    
    // Copy Assignment Operator
    Decibel& operator=(const Decibel& other) noexcept = default;
    
    // Destructor
    ~Decibel() noexcept = default;
    
    // Math Operators
    // LHS
    Decibel& operator+=(const Decibel& other)
    {
        gainInDb = gainInDb + other.gainInDb;
        return *this;
    }
    
    Decibel& operator-=(const Decibel& other)
    {
        gainInDb = gainInDb - other.gainInDb;
        return *this;
    }
    
    Decibel& operator*=(const Decibel& other)
    {
        gainInDb = gainInDb * other.gainInDb;
        return *this;
    }
    
    Decibel& operator/=(const Decibel& other)
    {
        gainInDb = gainInDb / other.gainInDb;
        return *this;
    }
    
    // RHS
    friend Decibel operator+ (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb + rhs.gainInDb);
    }

    
    friend Decibel operator- (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb - rhs.gainInDb);
    }

    friend Decibel operator* (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb * rhs.gainInDb);
    }
    
    friend Decibel operator/ (const Decibel& lhs, const Decibel& rhs)
    {
        return Decibel(lhs.gainInDb / rhs.gainInDb);
    }

    // Comparison Operators - these should be friend functions
    friend bool operator==(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb == rhs.gainInDb;
    }

    friend bool operator!=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb < rhs.gainInDb;
    }

    friend bool operator>(const Decibel& lhs, const Decibel& rhs)
    {
        return lhs.gainInDb > rhs.gainInDb;
    }

    friend bool operator<=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const Decibel& lhs, const Decibel& rhs)
    {
        return !(lhs < rhs);
    }
    
    
    // Setters and getters
    FloatType getGain() const
    {
        return juce::Decibels::decibelsToGain(gainInDb);
    }

    FloatType getDb() const
    {
        return gainInDb;
    }

    void setGain(FloatType gain)
    {
        gainInDb = juce::Decibels::gainToDecibels(gain)
    }

    void setDb(FloatType db)
    {
        gainInDb = db;
    }
    
    
private:
    FloatType gainInDb {0.f};
};
