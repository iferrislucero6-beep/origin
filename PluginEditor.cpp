/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhaserAudioProcessorEditor::PhaserAudioProcessorEditor (PhaserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    lfoAmpSlider.setRange(0.0, 1.0);
    lfoAmpSlider.addListener(this);
    addAndMakeVisible(&lfoAmpSlider);
    
    addAndMakeVisible(lfoAmpLabel);
    lfoAmpLabel.setText("LFO Amplitude", juce::dontSendNotification);
    lfoAmpLabel.attachToComponent(&lfoAmpSlider, true);
    
    lfoWidthSlider.setRange(0.0, 1.0);
    lfoWidthSlider.addListener(this);
    addAndMakeVisible(&lfoWidthSlider);
    
    addAndMakeVisible(lfoWidthLabel);
    lfoWidthLabel.setText("LFO Width", juce::dontSendNotification);
    lfoWidthLabel.attachToComponent(&lfoWidthSlider, true);
    
    exponential_triangle.setRadioGroupId(voiceButtons);
    square.setRadioGroupId(voiceButtons);
    sine.setRadioGroupId(voiceButtons);
    
    addAndMakeVisible(exponential_triangle);
    exponential_triangle.setButtonText("Exponential Triangle");
    exponential_triangle.setClickingTogglesState(true);
    exponential_triangle.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::dimgrey);
    exponential_triangle.addListener(this);
    
    addAndMakeVisible(square);
    square.setButtonText("Square");
    square.setClickingTogglesState(true);
    square.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::dimgrey);
    square.addListener(this);
    
    addAndMakeVisible(sine);
    sine.setButtonText("Sine");
    sine.setClickingTogglesState(true);
    sine.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::dimgrey);
    sine.addListener(this);
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
}

//==============================================================================
void PhaserAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Phaser", getLocalBounds(), juce::Justification::centred, 1);
}

void PhaserAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    lfoAmpSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    lfoWidthSlider.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
    
    exponential_triangle.setBounds(sliderLeft, 80, 60, 20);
    square.setBounds(exponential_triangle.getX() + exponential_triangle.getWidth() + 10, 80, 60, 20);
    sine.setBounds(square.getX() + square.getWidth() + 10, 80, 60, 20);
}

void PhaserAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &lfoAmpSlider)
    {
        //adj value of amp
        audioProcessor.amp = lfoAmpSlider.getValue();
    }
    else if (slider == &lfoWidthSlider)
    {
        //adj value of lfoWidth
        audioProcessor.lfoWidth = lfoWidthSlider.getValue();
    }
}

void PhaserAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &exponential_triangle)
    {
        audioProcessor.waveform = audioProcessor.wTriangleExp;
    }
    else if (button == &square)
    {
        audioProcessor.waveform = audioProcessor.wSquare;
    }
    else if (button == &sine)
    {
        audioProcessor.waveform = audioProcessor.wSine;
    }
}
