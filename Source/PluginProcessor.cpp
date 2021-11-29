/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
FreshwaterAudioProcessor::FreshwaterAudioProcessor()
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
    attachCompressorGainParams();
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

FreshwaterAudioProcessor::~FreshwaterAudioProcessor()
{
}

//==============================================================================
const juce::String FreshwaterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FreshwaterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FreshwaterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FreshwaterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FreshwaterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FreshwaterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FreshwaterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FreshwaterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FreshwaterAudioProcessor::getProgramName (int index)
{
    return {};
}

void FreshwaterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FreshwaterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec filterSpec, stereoSpec;
    filterSpec.maximumBlockSize = samplesPerBlock;
    filterSpec.sampleRate = sampleRate;
    filterSpec.numChannels = 1;
    
    stereoSpec.maximumBlockSize = samplesPerBlock;
    stereoSpec.sampleRate = sampleRate;
    stereoSpec.numChannels = getTotalNumOutputChannels();
    
    compressor.prepare(stereoSpec);
    inputGain.prepare(stereoSpec);
    outputGain.prepare(stereoSpec);
    
    leftChain.prepare(filterSpec);
    rightChain.prepare(filterSpec);
}

void FreshwaterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FreshwaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FreshwaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    // Update Params
    inputGain.updateGain();
    outputGain.updateGain();
    compressor.updateCompressorSettings();
    
    updateFilterParams();
    
    // Process
    inputGain.process(buffer);
    compressor.process(buffer);

    // Process The Chain
    juce::dsp::AudioBlock<float> block(buffer);

    auto maxChunkSize = 32;
    for (auto offset = 0; offset < block.getNumSamples();)
    {
        auto samplesToProcess = static_cast<int>(block.getNumSamples() - offset);
        auto chunkSize = samplesToProcess > maxChunkSize ? maxChunkSize : samplesToProcess;

        // Loop through filters and call performInnerLoopFilterUpdate()
        updateFilterState(chunkSize);

        // Process The Audio
        auto subBlock = block.getSubBlock(offset, chunkSize);
        auto leftSubBlock = subBlock.getSingleChannelBlock(0);
        auto rightSubBlock = subBlock.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> leftContext (leftSubBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext (rightSubBlock);

        leftChain.process(leftContext);
        rightChain.process(rightContext);

        // Update Offset
        offset += chunkSize;
    }
    
    outputGain.process(buffer);
}

void FreshwaterAudioProcessor::updateFilterState(const int chunkSize)
{
    updateSingleFilterState<0>(true, chunkSize);
    updateSingleFilterState<1>(true, chunkSize);
    updateSingleFilterState<2>(true, chunkSize);
}

void FreshwaterAudioProcessor::updateFilterParams()
{
    updateSingleFilterParams<0, FilterParameters>();
    updateSingleFilterParams<1, FilterParameters>();
    updateSingleFilterParams<2, FilterParameters>();
}

//==============================================================================
bool FreshwaterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FreshwaterAudioProcessor::createEditor()
{
    return new FreshwaterAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FreshwaterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void FreshwaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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


void FreshwaterAudioProcessor::createFilterParams(juce::AudioProcessorValueTreeState::ParameterLayout &layout, const int filterNum)
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


void FreshwaterAudioProcessor::createCompressorParams ( juce::AudioProcessorValueTreeState::ParameterLayout& layout)
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
    std::vector<int> threshVec = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89};
    for (auto thresh : threshVec)
    {
        strArray.add(juce::String(thresh));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(getCompRatioParamName(),
                                                            getCompRatioParamName(),
                                                            strArray,
                                                            2));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(getCompBypassParamName(),
                                                          getCompBypassParamName(),
                                                          true));
    
}


void FreshwaterAudioProcessor::createGainParams (juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto gainRange = juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(getInputGainParamName(),
                                                           getInputGainParamName(),
                                                           gainRange,
                                                           0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(getOutputGainParamName(),
                                                           getOutputGainParamName(),
                                                           gainRange,
                                                           0.f));
}


juce::AudioProcessorValueTreeState::ParameterLayout FreshwaterAudioProcessor::createParameterLayout ()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    createCompressorParams(layout);
    
    createGainParams(layout);
    
    for ( int i = 0; i < chainLength; ++i)
    {
        createFilterParams(layout, i);
    }
    
    return layout;
}


void FreshwaterAudioProcessor::attachCompressorGainParams ()
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
    
    // Gain
    floatHelper(inputGain.gainInDb, getInputGainParamName());
    floatHelper(outputGain.gainInDb, getOutputGainParamName());
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreshwaterAudioProcessor();
}
