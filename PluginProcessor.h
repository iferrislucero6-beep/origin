/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class PhaserAudioProcessor  : public juce::AudioProcessor, public juce::dsp::IIR::Coefficients<float>
{
public:
    //==============================================================================
    PhaserAudioProcessor();
    ~PhaserAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    float processSingleSampleRaw(const float sToProcess, float prevInput, float b0, float b1, float a0, float a1);
    
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
    
    enum Waveform
    {
        wTriangleExp,
        wSquare,
        wSine
    };
    
    int waveform; //type of waveform the LFO is set to
    float amp; //amplitude of lfo
    float lfoWidth; //ratio of min to max frequency
    
    void setSampleRate(float sampleRate);
    void setBaseFrequency(float baseFrequency);
    void setLfoFrequency(float lfoFrequency);
    void setDepth(float depth);
    
    float sampleRate = 44100.0f;
    float depth = 0.5f; //depth of phaser
    float baseFrequency; //center frequency of allpass filters
    float lfoFrequency = 10.0f;
    float lfoMinFreq;
    int filtersPerChannel = 2; //number of allpass filters per channel
    float lastFilterOutput;
    int numSamples;
    float *channelData; //array of samples
    float phase; //phase of LFO
    int sampleCount;
    int filterUpdateInterval = sampleRate/10000.0f; //when to update allpass coefficients
    float feedback = 0.2f; //amount of feedback [0,1) - create slider for this?
    juce::dsp::IIR::Coefficients<float> allpassFilters[2];
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserAudioProcessor)
    
    float triangle(float phase, float amp)
    {
        if (phase < 0.25f)
            return 0.5f + 2.0f * amp * phase;
        else if (phase < 0.75f)
            return 1.0f - 2.0f * amp * (phase - 0.25f);
        else
            return 2.0f * amp * (phase - 0.75f);
    }
    
    float lfo (float phase, int waveform)
    {
        //float normLfoFreq = (juce::MathConstants<float>::twoPi * lfoFrequency)/sampleRate;
        
        switch (waveform)
        {
        case wSine:
            return amp * sinf(juce::MathConstants<float>::twoPi * lfoFrequency + phase);
            
        case wSquare:
            if (phase < 0.5f)
                return amp * 1.0f;
            else
                return 0.0f;
            
        case wTriangleExp:
        default:
            //implement exponential triangle waveform
            return log(lfoMinFreq) + log(lfoWidth) * triangle(phase, amp);
        }
    }
    
    juce::dsp::Oscillator<float> osc{ [](float x) {return std::sin(x);} };
    
};
