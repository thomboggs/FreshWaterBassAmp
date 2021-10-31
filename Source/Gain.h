/*
  ==============================================================================

    Gain.h
    Created: 31 Oct 2021 2:07:08pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


struct Gain
{
    juce::AudioParameterFloat* gainInDb { nullptr };
    
    void prepare (const juce::dsp::ProcessSpec spec)
    {
        gain.prepare(spec);
    }
    
    void updateGain ()
    {
        gain.setGainDecibels(gainInDb->get());
    }
    
    void process (juce::AudioBuffer<float>& buffer)
    {
        auto block = juce::dsp::AudioBlock<float> ( buffer );
        auto context = juce::dsp::ProcessContextReplacing<float> ( block );
        
        gain.process( context );
    }
    
private:
    juce::dsp::Gain<float> gain;
    
};

