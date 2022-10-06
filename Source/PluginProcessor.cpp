/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DigiCompressorAudioProcessor::DigiCompressorAudioProcessor()
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
    attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
    jassert(attack != nullptr);
    release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
    jassert(release != nullptr);
    threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
    jassert(threshold != nullptr);
    ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
    jassert(ratio != nullptr);
    bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypass"));
    jassert(bypass != nullptr);
}

DigiCompressorAudioProcessor::~DigiCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String DigiCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DigiCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DigiCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DigiCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DigiCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DigiCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DigiCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DigiCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DigiCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void DigiCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DigiCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    compressor.prepare(spec);
}

void DigiCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DigiCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DigiCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // Create a reference to get float from declared parameters here..
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(release->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());

    // Passing the signal through blocks..
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    // Enable bypassing function here..
    context.isBypassed = bypass->get();

    compressor.process(context);
}

//==============================================================================
bool DigiCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DigiCompressorAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DigiCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream memout(destData, true);
    apvts.state.writeToStream(memout);
}

void DigiCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

// The layout of the parameters is initialised here..
juce::AudioProcessorValueTreeState::ParameterLayout DigiCompressorAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    
    using namespace juce;
    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);

    // THRESHOLD
    layout.add(std::make_unique<AudioParameterFloat>("Threshold", "Threshold", 
                                                      NormalisableRange<float>(-60, +12, 1, 1), 0));
    
    // ATTACK
    
    layout.add(std::make_unique<AudioParameterFloat>("Attack", "Attack",
                                                      attackReleaseRange, 50));

    // RELEASE
    layout.add(std::make_unique<AudioParameterFloat>("Release", "Release",
                                                      attackReleaseRange, 250));

    // RATIO (declare choices then add to string array)
    auto choices = std::vector<double>{ 1, 1.5, 2, 2.5, 3, 4, 5, 6, 7, 8, 10, 15, 25, 50, 100 }; // 15 steps
    juce::StringArray ratio;
    for (auto choice : choices)
    {
        ratio.add(juce::String(choice, 1));
    }
    layout.add(std::make_unique<AudioParameterChoice>("Ratio", "Ratio",
                                                       ratio, 2.5));

    // BYPASS
    layout.add(std::make_unique<AudioParameterBool>("Bypass", "Bypass", 
                                                       false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DigiCompressorAudioProcessor();
}
