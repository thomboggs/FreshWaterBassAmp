/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FreshWaterAudioProcessor::FreshWaterAudioProcessor()
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
}

FreshWaterAudioProcessor::~FreshWaterAudioProcessor()
{
}

//==============================================================================
const juce::String FreshWaterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FreshWaterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FreshWaterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FreshWaterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FreshWaterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FreshWaterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FreshWaterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FreshWaterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FreshWaterAudioProcessor::getProgramName (int index)
{
    return {};
}

void FreshWaterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FreshWaterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    inputCompressor.prepare(spec);
    
    // Get apvts Values
    auto attack =  dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"))->get();
    auto release =  dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"))->get();
    auto threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"))->get();
    auto ratio =  dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"))->getCurrentChoiceName().getFloatValue();
    inputCompressor.setAttack(attack);
    inputCompressor.setRelease(release);
    inputCompressor.setThreshold(threshold);
    inputCompressor.setRatio(ratio);

    postCompGain.prepare(spec);
    auto gainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Gain"))->get();
    postCompGain.setGainLinear(gainParam);
    
    leftFilters.prepare(spec);
    rightFilters.prepare(spec);
}

void FreshWaterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FreshWaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void FreshWaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // Get Value from APVTS
    
    auto attack =  dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"))->get();
    auto release =  dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"))->get();
    auto threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"))->get();
    auto ratio =  dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"))->getCurrentChoiceName().getFloatValue();
    
    auto gainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Gain"))->get();
    
    inputCompressor.setAttack(attack);
    inputCompressor.setRelease(release);
    inputCompressor.setThreshold(threshold);
    inputCompressor.setRatio(ratio);
    
    postCompGain.setGainLinear(gainParam);
    
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    postCompGain.process(context);
    
}

//==============================================================================
bool FreshWaterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FreshWaterAudioProcessor::createEditor()
{
//    return new FreshWaterAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FreshWaterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void FreshWaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}


juce::AudioProcessorValueTreeState::ParameterLayout FreshWaterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack",
                                                           "Attack",
                                                           juce::NormalisableRange<float>(10.f, 500.f, 1.f, 1.f),
                                                           50.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release",
                                                           "Release",
                                                           juce::NormalisableRange<float>(10.f, 500.f, 1.f, 1.f),
                                                           100.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Threshold",
                                                           "Threshold",
                                                           juce::NormalisableRange<float>(-48.f, 24.f, 1.f, 1.f),
                                                           0.f));
    
    juce::StringArray strArray;
    std::vector<double> threshVec = {1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 10.0, 20.0, 50.0, 100.0};
    for (auto thresh : threshVec)
    {
        strArray.add(juce::String(thresh));
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("Ratio",
                                                            "Ratio",
                                                            strArray,
                                                            2));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain",
                                                           "Gain",
                                                           juce::NormalisableRange<float>(0.f, 5.f, 0.05f),
                                                           0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq",
                                                           "LowCut Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.5f),
                                                           20.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mid Freq",
                                                           "Mid Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.5f),
                                                           500.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mid Gain",
                                                           "Mid Gain",
                                                           juce::NormalisableRange<float>(0.f, 5.f, 1.f, 1.f),
                                                           1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq",
                                                           "HighCut Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.5f),
                                                           20000.f));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreshWaterAudioProcessor();
}
