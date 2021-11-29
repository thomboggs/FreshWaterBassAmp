/*
  ==============================================================================

    ResponseCurveComponent.cpp
    Created: 31 Oct 2021 2:36:58pm
    Author:  Thomas Boggs

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ResponseCurveComponent.h"
#include "../FilterInfo.h"
#include "../CoefficientsMaker.h"

//==============================================================================
ResponseCurveComponent::ResponseCurveComponent(FreshwaterAudioProcessor& p) : audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    const auto& params = audioProcessor.getParameters();
    for ( auto param : params)
    {
        param->addListener(this);
    }
    
    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for ( auto param : params)
    {
        param->removeListener(this);
    }
}


void ResponseCurveComponent::parameterValueChanged (int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}


void ResponseCurveComponent::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        
        updateChain();
        // signal a repaint
        repaint();
    }
}


FilterParameters ResponseCurveComponent::updateFilter(const int bandNum)
{
    FilterParameters params;
    
    params.sampleRate = audioProcessor.getSampleRate();
    
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.filterType = static_cast<FilterInfo::FilterType>(p->getIndex());
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.apvts.getParameter(getGainParamName(bandNum))))
    {
        params.gain.setDb( p->get() );
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(audioProcessor.apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }

    return params;
}

void ResponseCurveComponent::updateChain()
{
    auto filter0 = updateFilter(0);
    auto filter1 = updateFilter(1);
    auto filter2 = updateFilter(2);
//    auto chainSettings = getChainSettings(audioProcessor.apvts);
    
//    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypass);
    chain.setBypassed<0>(filter0.bypassed);
    auto filter0Coeff = CoefficientsMaker<float>::calcCoefficients(filter0);
    *(chain.get<0>().coefficients) = *filter0Coeff;
    
    chain.setBypassed<1>(filter1.bypassed);
    auto filter1Coeff = CoefficientsMaker<float>::calcCoefficients(filter1);
    *(chain.get<1>().coefficients) = *filter1Coeff;
    
    chain.setBypassed<2>(filter2.bypassed);
    auto filter2Coeff = CoefficientsMaker<float>::calcCoefficients(filter2);
    *(chain.get<2>().coefficients) = *filter2Coeff;
    
    
//    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
//    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
//    
//    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypass);
//    
//    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
//    
//    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypass);
//    
//    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());
//    
//    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
//    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}



void ResponseCurveComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

//    auto r = getLocalBounds();
//    r.reduce(10, 10);
//    
//    g.setColour (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//    g.fillRect(r);
//    
//    g.setColour (juce::Colours::grey);
//    g.drawRect (r, 1);   // draw an outline around the component
//    
//
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("ResponseCurveComponent", r,
//                juce::Justification::centred, true);   // draw some placeholder text
    
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
    
    g.drawImage(background, getLocalBounds().toFloat());
    
    auto responseArea = getAnalysisArea();
    
    auto w = responseArea.getWidth();
    auto& low = chain.get<0>();
    auto& mid = chain.get<1>();
    auto& high = chain.get<2>();
    
    auto sampleRate = audioProcessor.getSampleRate();
    
    std::vector<double> mags;
    
    mags.resize(w);
    
    for (int i = 0; i < w; ++i)
    {
        double mag = 1.f;
        auto freq = mapToLog10 ( double(i) / double(w), 20.0, 20000.0 );
        
        if ( !chain.isBypassed<0>())
            mag *= low.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        if ( !chain.isBypassed<1>())
            mag *= mid.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        if ( !chain.isBypassed<2>())
            mag *= high.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    Path responseCurve;
    
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax] (double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };
    
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));
    
    for ( size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
    
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
    
    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f)); 
    
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    
    Graphics g(background);
    
    Array<float> freqs
    {
        20, /*30, 40,*/ 50, 100,
        200, /*300, 400,*/ 500, 1000,
        2000, /*3000, 4000,*/ 5000, 10000,
        20000
    };
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    Array<float> xs;
    for ( auto f : freqs)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        xs.add(left + width * normX);
    }
    
    g.setColour(Colours::dimgrey);
    for (auto x : xs )
    {
//        auto normX = mapFromLog10(f, 20.f, 20000.f);
//        g.drawVerticalLine(normX, 0, getHeight());
        g.drawVerticalLine(x, top, bottom);
    }
    
    Array<float> gains
    {
        -24, -12, 0, 12, 24
    };
    
    for ( auto gain : gains )
    {
        auto y = jmap(gain, -24.f, 24.f, float(bottom), float(top));
//        g.drawHorizontalLine(y, 0, getWidth());
        g.setColour(gain == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, float(left), float(right));
    }
     
//    g.drawRect(getAnalysisArea());
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    for (int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];
        
        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }
        
        str << f;
        if ( addK )
            str << "k";
        str << "Hz";
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    for ( auto gain : gains )
    {
        auto y = jmap(gain, -24.f, 24.f, float(bottom), float(top));
        
        String str;
        if (gain > 0)
            str << "+";
        str << gain;
//        str << "dB";
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        
        const int fontHeight = 10;
        g.setFont(fontHeight);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gain == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
//        r.setCentre(getRenderArea().getRight() + 10, y);
//        g.drawFittedText(str, r, juce::Justification::centred, 1);
        
        str.clear();
        str << (gain - 24.f);
        
        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centred, 1);
        
            
    }

}


juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

//    bounds.reduce(10, //JUCE_LIVE_CONSTANT(5),
//                  8 //JUCE_LIVE_CONSTANT(5)
//                  );
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}


juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();

    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);

    return bounds;

}
