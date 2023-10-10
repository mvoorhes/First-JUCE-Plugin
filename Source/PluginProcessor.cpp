/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FirstJUCEpluginAudioProcessor::FirstJUCEpluginAudioProcessor()
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

FirstJUCEpluginAudioProcessor::~FirstJUCEpluginAudioProcessor()
{
}

//==============================================================================
const juce::String FirstJUCEpluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FirstJUCEpluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FirstJUCEpluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FirstJUCEpluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FirstJUCEpluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FirstJUCEpluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FirstJUCEpluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FirstJUCEpluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FirstJUCEpluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void FirstJUCEpluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FirstJUCEpluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
 
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void FirstJUCEpluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FirstJUCEpluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FirstJUCEpluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    
    // For this plugin, the default loop is unnecessary. Set up audio blocks
    // Two audio blocks, designed for stereo
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(LEFT_CHANNEL);
    auto rightBlock = block.getSingleChannelBlock(RIGHT_CHANNEL);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool FirstJUCEpluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FirstJUCEpluginAudioProcessor::createEditor()
{
//    return new FirstJUCEpluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FirstJUCEpluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FirstJUCEpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// Code I've written
juce::AudioProcessorValueTreeState::ParameterLayout FirstJUCEpluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("LowCut Freq", 1),
            "LowCut Freq",
            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
            20.f
        ),
        std::make_unique<juce::AudioParameterFloat>(
           juce::ParameterID("HighCut Freq", 1),
           "HighCut Freq",
           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
           20000.f
        ),
        std::make_unique<juce::AudioParameterFloat>(
           juce::ParameterID("Peak Freq", 1),
           "Peak Freq",
           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
           750.f
        ),
        std::make_unique<juce::AudioParameterFloat>(
           juce::ParameterID("Peak Gain", 1),
           "Peak Gain",
           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
           0.f
        ),
        std::make_unique<juce::AudioParameterFloat>(
           juce::ParameterID("Peak Quality", 1),
           "Peak Quality",
           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
           1.f
        )
    );
    
    juce::StringArray choices;
    for (int i = 0; i < 4; i++) {
        juce::String str;
        str << (12 + i*12) << " db/Oct";
        choices.add(str);
    }
    
    layout.add(
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("LowCut Slope", 1), "LowCut Slope", choices, 0),
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("HighCut Slope", 1), "HighCut Slope", choices, 0)
    );
    
    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FirstJUCEpluginAudioProcessor();
}
