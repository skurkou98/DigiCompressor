/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

/* 
***ISSUE: Input signal not being affected by compressor***
Possible causes:
1) PARAMETERS NOT PROPERLY REFERENCED WITH UI?
2) PrepareToPlay and/or ProcessBlock not properly processing blocks and/or input signal?
3) Undeclared sampling rate and/or buffer size?
4) Improper channel configuration?
5) Bypass sending true indefinitely.
Possible solutions:
1) Changed editor classes to reference GenericAudioProcessorEditor.
2) Commented out 'bypass' parameter.
3) USED BREAKPOINTS AND DEBUGGER TO READ VALUES STORED IN PARAMETERS, FOUND THRESHOLD INCORRECTLY REFERENCING RELEASE VALUE. (LINE 44 - PP.h)
Outcome:
!!!RESOLVED!!!
*/

#pragma once

#include <JuceHeader.h>

// Declare the multiband parameters here, must be expanded to account for 3 bands..
struct CompressorBand
{
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterChoice* ratio{ nullptr };
    juce::AudioParameterBool* bypass{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }

    void updateCompressorSettings()
    {
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        // Passing the signal through blocks..
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        // Enable bypassing function here..
        context.isBypassed = bypass->get();

        compressor.process(context);
    }

private:
    juce::dsp::Compressor<float> compressor;
};

//==============================================================================
/**
*/
class DigiCompressorAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DigiCompressorAudioProcessor();
    ~DigiCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:
    //juce::dsp::Compressor<float> compressor;

    //// Create pointers to AudioPrameterFloat declared for every parameter in .cpp file for compressor declared above i.e 'cached'..
    //juce::AudioParameterFloat* attack{ nullptr };
    //juce::AudioParameterFloat* release{ nullptr };
    //juce::AudioParameterFloat* threshold{ nullptr };
    //juce::AudioParameterChoice* ratio{ nullptr };
    //juce::AudioParameterBool* bypass{ nullptr };
    CompressorBand compressor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DigiCompressorAudioProcessor)
};