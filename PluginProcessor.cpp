/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhaserAudioProcessor::PhaserAudioProcessor()
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

PhaserAudioProcessor::~PhaserAudioProcessor()
{
}

//==============================================================================
const juce::String PhaserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PhaserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PhaserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PhaserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PhaserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PhaserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PhaserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PhaserAudioProcessor::getProgramName (int index)
{
    return {};
}

void PhaserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PhaserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    phase = 0.0f;
}

void PhaserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PhaserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    //auto numChannels = buffer.getNumChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    
    for (int n = 0; n < numSamples; ++n )
    {
        float out = channelData[n];
        float prevInput = 0;
        if (n > 0)
            prevInput = channelData[n-1];
        
        if (feedback != 0.0f)
            out += feedback * lastFilterOutput;
        
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto channelData = buffer.getWritePointer(channel);
            
            for (int j = 0; j < filtersPerChannel; ++j)
            {
                if (sampleCount % filterUpdateInterval == 0)
                {
                    allpassFilters[j].makeAllPass(1.0/sampleRate, baseFrequency + lfoWidth * lfo(phase, waveform));
                }
                float b0 = allpassFilters[j].getRawCoefficients()[0];
                float b1 = allpassFilters[j].getRawCoefficients()[1];
                float a0 = allpassFilters[j].getRawCoefficients()[2];
                float a1 = allpassFilters[j].getRawCoefficients()[3];
                out = processSingleSampleRaw(out, prevInput, b0, b1, a0, a1);
            }
            lastFilterOutput = out;
            
            channelData[n] = (1.0f-0.5f*depth) * channelData[n] + 0.5f*depth*out;
        }
        // update phase for LFO
        phase += lfoFrequency * (1.0f/sampleRate);
        if (phase >= 1.0f)
            phase -= 1.0f;
        // update sample count
        sampleCount++;
    }
}

float PhaserAudioProcessor::processSingleSampleRaw(const float sampleToProcess, float prevInput, float b0, float b1, float a0, float a1)
{
    float y1 = lastFilterOutput;
    float x1 = prevInput;
    y1 = (b0 * sampleToProcess) + (b1 * x1) + (a1 * y1);
    return y1;
}
//==============================================================================
bool PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    return new PhaserAudioProcessorEditor (*this);
}

//==============================================================================
void PhaserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PhaserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void PhaserAudioProcessor::setSampleRate (float sampleRate)
{
    this->sampleRate = sampleRate;
}

void PhaserAudioProcessor::setBaseFrequency (float baseFrequency)
{
    this->baseFrequency = baseFrequency;
}

void PhaserAudioProcessor::setLfoFrequency(float lfoFrequency)
{
    this->lfoFrequency = lfoFrequency;
}

void PhaserAudioProcessor::setDepth (float depth)
{
    this->depth = depth; 
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
