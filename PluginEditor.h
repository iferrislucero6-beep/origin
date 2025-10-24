/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PhaserAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Button::Listener
{
public:
    PhaserAudioProcessorEditor (PhaserAudioProcessor&);
    ~PhaserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PhaserAudioProcessor& audioProcessor;
    
    // amp of lfo
    juce::Slider lfoAmpSlider;
    juce::Label lfoAmpLabel;
    
    // lfoWidth
    juce::Slider lfoWidthSlider;
    juce::Label lfoWidthLabel;
    
    // waveform of lfo
    enum RadioButtonIds
    {
        voiceButtons = 1001
    };
    
    juce::TextButton exponential_triangle;
    juce::TextButton square;
    juce::TextButton sine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserAudioProcessorEditor)
};
