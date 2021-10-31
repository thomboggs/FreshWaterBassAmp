/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FilterHelperFunctions.h"





//==============================================================================
Freshwater::Freshwater()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    
#endif
{
    attachCompressorParams();
//    auto floatHelper = [&apvts = this->apvts](auto& param, const auto& paramName)
//    {
//        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramName));
//        jassert(param != nullptr);
//    };
//
//    floatHelper(compressor.attack, getCompAttackParamName());
//    floatHelper(compressor.release, getCompReleaseParamName());
//    floatHelper(compressor.threshold, getCompThresholdParamName());
//
//    compressor.ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getCompRatioParamName()));
//    jassert(compressor.ratio != nullptr);
//
//    compressor.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getCompBypassParamName()));
//    jassert(compressor.bypassed != nullptr);
}

Freshwater::~Freshwater()
{
}

//==============================================================================
const juce::String Freshwater::getName() const
{
    return JucePlugin_Name;
}

bool Freshwater::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Freshwater::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Freshwater::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Freshwater::getTailLengthSeconds() const
{
    return 0.0;
}

int Freshwater::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Freshwater::getCurrentProgram()
{
    return 0;
}

void Freshwater::setCurrentProgram (int index)
{
}

const juce::String Freshwater::getProgramName (int index)
{
    return {};
}

void Freshwater::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Freshwater::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec filterSpec, compressorSpec;
    filterSpec.maximumBlockSize = samplesPerBlock;
    filterSpec.sampleRate = sampleRate;
    filterSpec.numChannels = 1;
    
    compressorSpec.maximumBlockSize = samplesPerBlock;
    compressorSpec.sampleRate = sampleRate;
    compressorSpec.numChannels = getTotalNumOutputChannels();
    
    compressor.prepare(compressorSpec);
    
    leftChain.prepare(filterSpec);
    rightChain.prepare(filterSpec);
}

void Freshwater::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Freshwater::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Freshwater::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    compressor.updateCompressorSettings();
    
    updateParams();
    
    refreshFilters();
    
    compressor.process(buffer);

    // Process The Chain
    auto filterBlock = juce::dsp::AudioBlock<float> (buffer);
    
    auto leftBlock = filterBlock.getSingleChannelBlock(0);
    auto rightBlock = filterBlock.getSingleChannelBlock(1);
    
    auto leftContext = juce::dsp::ProcessContextReplacing<float> (leftBlock);
    auto rightContext = juce::dsp::ProcessContextReplacing<float> (rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool Freshwater::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Freshwater::createEditor()
{
//    return new Pfmcpp_project10AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Freshwater::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void Freshwater::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() )
    {
        apvts.replaceState(tree);
        // This updates the apvts. In the processBlock, any apvts changes will be automatically applied.
    }
}


void Freshwater::createCutParams(juce::AudioProcessorValueTreeState::ParameterLayout &layout, const int filterNum, const bool isLowCut)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(filterNum),
                                                          getBypassParamName(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(filterNum),
                                                           getFreqParamName(filterNum),
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                                500.f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(getTypeParamName(filterNum),
                                                          getTypeParamName(filterNum),
                                                          isLowCut));
    
    juce::StringArray stringArray;
    
    // Orders 1 through 8
    for (auto i=1; i<9; ++i)
    {
        stringArray.add(juce::String(i));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getQualityParamName(filterNum),
                                                            getQualityParamName(filterNum),
                                                            stringArray,
                                                            0));
}


void Freshwater::createFilterParamas(juce::AudioProcessorValueTreeState::ParameterLayout &layout, const int filterNum)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(getBypassParamName(filterNum),
                                                          getBypassParamName(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getGainParamName(filterNum),
                                                           getGainParamName(filterNum),
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                               0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(getQualityParamName(filterNum),
                                                           getQualityParamName(filterNum),
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                               1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(getFreqParamName(filterNum),
                                                           getFreqParamName(filterNum),
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                               500.f));
    
    juce::StringArray stringArray;
    auto filterTypeMap = FilterInfo::getFilterTypeMap();
    auto it = filterTypeMap.begin();
    
    while (it != filterTypeMap.end())
    {
        stringArray.add(it->second);
        it++;
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getTypeParamName(filterNum),
                                                            getTypeParamName(filterNum),
                                                            stringArray,
                                                            0));
}


void Freshwater::createCompressorParams ( juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto attackReleaseRange =  juce::NormalisableRange<float>( 5.f, 500.f, 1.f, 1.f );
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getCompAttackParamName(),
                                                           getCompAttackParamName(),
                                                           attackReleaseRange,
                                                           50.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getCompReleaseParamName(),
                                                           getCompReleaseParamName(),
                                                           attackReleaseRange,
                                                           50.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getCompThresholdParamName(),
                                                           getCompThresholdParamName(),
                                                           juce::NormalisableRange<float>(-60.f, 12.f, 1.f, 1.f),
                                                           0.f));
    
    juce::StringArray strArray;
    std::vector<double> threshVec = {1.0, 2.0, 3.0, 5.0, 8.0, 13.0, 21.0, 34.0, 55.0, 89.0};
    for (auto thresh : threshVec)
    {
        strArray.add(juce::String(thresh, 1));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getCompRatioParamName(),
                                                            getCompRatioParamName(),
                                                            strArray,
                                                            2));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(getCompBypassParamName(),
                                                          getCompBypassParamName(),
                                                          true));
    
}


juce::AudioProcessorValueTreeState::ParameterLayout Freshwater::createParameterLayout ()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    /*
     I want to add the params for:
        - 1 LowCut Filter
            - Freq
            - Byp
            - Order
            - IsLowCut = True
        - 1 MultiType Filter
            - Freq
            - Byp
            - Quality
            - gain
        - 1 HighCut Filter
            - Freq
            - Byp
            - Order
            - IsLowCut = False
     */
    
    createCompressorParams(layout);
    
    for ( int i = 0; i < chainLength; ++i)
    {
        // Add Low Cut Params to layout
        if (i == 0)
        {
            createCutParams(layout, i, true);
            continue;
        }
        // Add High Cut Params
        if ( i == (chainLength-1) )
        {
            createCutParams(layout, i, false);
            continue;
        }
        
        createFilterParamas(layout, i);
    }
    
    return layout;
}


void Freshwater::attachCompressorParams ()
{
    auto floatHelper = [&apvts = this->apvts](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramName));
        jassert(param != nullptr);
    };
    
    floatHelper(compressor.attack, getCompAttackParamName());
    floatHelper(compressor.release, getCompReleaseParamName());
    floatHelper(compressor.threshold, getCompThresholdParamName());
    
    compressor.ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getCompRatioParamName()));
    jassert(compressor.ratio != nullptr);
    
    compressor.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getCompBypassParamName()));
    jassert(compressor.bypassed != nullptr);
}



FilterParameters Freshwater::getFilterParams(int bandNum)
{
    FilterParameters params;

    params.sampleRate = getSampleRate();
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.quality = p->get();
    }

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.filterType = static_cast<FilterInfo::FilterType>(p->getIndex());
    }

    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getGainParamName(bandNum))))
    {
        params.gainInDb = p->get();
    }
    
    return params;
}


HighCutLowCutParameters Freshwater::getCutParams(int bandNum)
{
    HighCutLowCutParameters params;
    
    params.sampleRate = getSampleRate();
    
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(getQualityParamName(bandNum))))
    {
        params.order = p->getIndex()+1;
    }
//    params.order = 1;
    
    if (auto* p = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(getFreqParamName(bandNum))))
    {
        params.frequency = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(bandNum))))
    {
        params.bypassed = p->get();
    }
    
    if (auto* p = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getTypeParamName(bandNum))))
    {
        params.isLowcut = p->get();
    }
    
    return params;
}



void Freshwater::setChainBypass(const bool isBypassed, FilterPosition pos)
{
    switch (pos)
    {
        case FilterPosition::LowCut:
            leftChain.template setBypassed<0>(isBypassed);
            rightChain.template setBypassed<0>(isBypassed);
            break;
        case FilterPosition::Multi1:
            leftChain.template setBypassed<1>(isBypassed);
            rightChain.template setBypassed<1>(isBypassed);
            break;
        case FilterPosition::HighCut:
            leftChain.template setBypassed<2>(isBypassed);
            rightChain.template setBypassed<2>(isBypassed);
            break;
    }
}


void Freshwater::updateParams()
{
    // Check if params have changed if so, update via fcg
    /*
     Low Cut
     */
    // Check for not bypassed
    bool filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::LowCut)))->get();
    if ( !filterBypassed )
    {
        setChainBypass(false, FilterPosition::LowCut);
        
        auto tempHighCutLowCutParams = getCutParams(0);
        if (currentLowCutParams != tempHighCutLowCutParams)
        {
            // if changed, calc new Coeffs
//            DBG("Updating Coefficients");
            currentLowCutParams = tempHighCutLowCutParams;
//            updateLowCutCoefficients(currentLowCutParams);
            leftLowCutFCG.changeParameters(currentLowCutParams);
            rightLowCutFCG.changeParameters(currentLowCutParams);
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::LowCut);
    }
    
    /*
     Multi1
     */
    filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::Multi1)))->get();
    if ( !filterBypassed )
    {
        setChainBypass(false, FilterPosition::Multi1);
        
        // Middle Filter Params
        auto tempFilterParams = getFilterParams(FilterPosition::Multi1);
        
        if (currentFilterParams != tempFilterParams)
        {
            // if changed, calc new Coeffs
            currentFilterParams = tempFilterParams;
            leftFilterFCG.changeParameters(currentFilterParams);
            rightFilterFCG.changeParameters(currentFilterParams);
            
            
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::Multi1);
    }
    
    /*
     High Cut Filter
     */
    filterBypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(getBypassParamName(FilterPosition::HighCut)))->get();
    if ( !filterBypassed)
    {
        setChainBypass(false, FilterPosition::HighCut);
        
        auto tempHighCutLowCutParams = getCutParams(FilterPosition::HighCut);
        if (currentHighCutParams != tempHighCutLowCutParams)
        {
            // if changed, calc new Coeffs
            currentHighCutParams = tempHighCutLowCutParams;
//            updateHighCutCoefficients(currentHighCutParams);
            leftHighCutFCG.changeParameters(currentHighCutParams);
            rightHighCutFCG.changeParameters(currentHighCutParams);
        }
    }
    else
    {
        setChainBypass(true, FilterPosition::HighCut);
    }
}


void Freshwater::refreshFilters()
{
    refreshLowCutFilter(leftLowCutFifo,
                        leftChain,
                        leftLowCutReleasePool);
    refreshLowCutFilter(rightLowCutFifo,
                        rightChain,
                        rightLowCutReleasePool);
    
    refreshHighCutFilter(leftHighCutFifo,
                        leftChain,
                        leftHighCutReleasePool);
    refreshHighCutFilter(rightHighCutFifo,
                        rightChain,
                        rightHighCutReleasePool);
    
    // Peak Filter
    CoefficientsPtr ptr;
    
    if ( leftFilterCoeffFifo.getNumAvailableForReading() > 0 )
    {
        auto& leftFilter = leftChain.template get<FilterPosition::Multi1>();
        if ( leftFilterCoeffFifo.pull(ptr) )
        {
            leftFilterReleasePool.add(ptr);
            *(leftFilter.coefficients) = *ptr;
        }
    }
    
    if ( rightFilterCoeffFifo.getNumAvailableForReading() > 0 )
    {
        auto& rightFilter = rightChain.template get<FilterPosition::Multi1>();
        if ( rightFilterCoeffFifo.pull(ptr) )
        {
            rightFilterReleasePool.add(ptr);
            *(rightFilter.coefficients) = *ptr;
        }
    }
}


void Freshwater::refreshLowCutFilter (Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32>& cutFifo,
                                                          FilterChain& chain,
                                                          ReleasePool<CoefficientsPtr>& cutPool)
{
    // Refresh Low Cut
    // Left
    if ( cutFifo.getNumAvailableForReading() > 0 )
    {
        auto& LowCutFilterChain = chain.template get<FilterPosition::LowCut>();
        juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> tempArray {};
    
        if ( cutFifo.pull(tempArray) )
        {
            auto tempSize = tempArray.size();
            if ( tempSize > 0)
            {
                LowCutFilterChain.setBypassed<0>(true);
                LowCutFilterChain.setBypassed<1>(true);
                LowCutFilterChain.setBypassed<2>(true);
                LowCutFilterChain.setBypassed<3>(true);
  
                switch (tempSize)
                {
                    case 4:
                        LowCutFilterChain.setBypassed<3>(false);
                        *LowCutFilterChain.get<3>().coefficients = *tempArray[3];
                        cutPool.add(tempArray[3]);
                    case 3:
                        LowCutFilterChain.setBypassed<2>(false);
                        *LowCutFilterChain.get<2>().coefficients = *tempArray[2];
                        cutPool.add(tempArray[2]);
                    case 2:
                        LowCutFilterChain.setBypassed<1>(false);
                        *LowCutFilterChain.get<1>().coefficients = *tempArray[1];
                        cutPool.add(tempArray[1]);
                    case 1:
                        LowCutFilterChain.setBypassed<0>(false);
                        *LowCutFilterChain.get<0>().coefficients = *tempArray[0];
                        cutPool.add(tempArray[0]);
                }
            }
        }
    }
}


void Freshwater::refreshHighCutFilter (Fifo<juce::ReferenceCountedArray<CutCoeffs>, 32>& cutFifo,
                                                          FilterChain& chain,
                                                          ReleasePool<CoefficientsPtr>& cutPool)
{
    // Refresh Low Cut
    // Left
    if ( cutFifo.getNumAvailableForReading() > 0 )
    {
        auto& LowCutFilterChain = chain.template get<FilterPosition::HighCut>();
        juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> tempArray {};
    
        if ( cutFifo.pull(tempArray) )
        {
            auto tempSize = tempArray.size();
            if ( tempSize > 0)
            {
                LowCutFilterChain.setBypassed<0>(true);
                LowCutFilterChain.setBypassed<1>(true);
                LowCutFilterChain.setBypassed<2>(true);
                LowCutFilterChain.setBypassed<3>(true);
  
                switch (tempSize)
                {
                    case 4:
                        LowCutFilterChain.setBypassed<3>(false);
                        *LowCutFilterChain.get<3>().coefficients = *tempArray[3];
                        cutPool.add(tempArray[3]);
                    case 3:
                        LowCutFilterChain.setBypassed<2>(false);
                        *LowCutFilterChain.get<2>().coefficients = *tempArray[2];
                        cutPool.add(tempArray[2]);
                    case 2:
                        LowCutFilterChain.setBypassed<1>(false);
                        *LowCutFilterChain.get<1>().coefficients = *tempArray[1];
                        cutPool.add(tempArray[1]);
                    case 1:
                        LowCutFilterChain.setBypassed<0>(false);
                        *LowCutFilterChain.get<0>().coefficients = *tempArray[0];
                        cutPool.add(tempArray[0]);
                }
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Freshwater();
}
