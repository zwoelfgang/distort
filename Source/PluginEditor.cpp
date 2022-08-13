/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortAudioProcessorEditor::DistortAudioProcessorEditor (DistortAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    driveKnob.setTextValueSuffix ("Drive");
    driveKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    driveKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&driveKnob);

    preVolumeKnob.setTextValueSuffix ("Pre Volume");
    preVolumeKnob.setSliderStyle (juce::Slider::LinearVertical);
    preVolumeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&preVolumeKnob);

    postVolumeKnob.setTextValueSuffix ("Post Volume");
    postVolumeKnob.setSliderStyle (juce::Slider::LinearVertical);
    postVolumeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&postVolumeKnob);

    lowCut.setTextValueSuffix ("Low Cut");
    lowCut.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowCut.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&lowCut);

    highCut.setTextValueSuffix ("High Cut");
    highCut.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highCut.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&highCut);

    driveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "drive", driveKnob));
    preVolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "pre volume", preVolumeKnob));
    postVolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "post volume", postVolumeKnob));
    lowCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "low cut", lowCut));
    highCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "high cut", highCut));

    setSize (600, 400);
}

DistortAudioProcessorEditor::~DistortAudioProcessorEditor()
{
}

//==============================================================================
void DistortAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DistortAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    driveKnob.setBounds(((getWidth() / 2) - 150 / 2), ((getHeight() / 2) - 50 / 2), 150, 150);
    preVolumeKnob.setBounds(((getWidth() / 4) - 100 / 2), ((getHeight() / 2) - 50 / 2), 20, 250);
    postVolumeKnob.setBounds(((getWidth() / 4) + 100 / 2), ((getHeight() / 2) - 50 / 2), 20, 250);
    highCut.setBounds(((getWidth() / 4) - 100 / 2), ((getHeight() / 2) - 50 / 2), 75, 75);
    lowCut.setBounds(((getWidth() / 4) + 100 / 2), ((getHeight() / 2) - 50 / 2), 75, 75);
}
