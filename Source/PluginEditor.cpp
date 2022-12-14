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
    addAndMakeVisible(&driveKnob);
    driveKnob.setTextValueSuffix("Drive");
    driveKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    driveKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&preVolumeKnob);
    preVolumeKnob.setTextValueSuffix("Pre Volume");
    preVolumeKnob.setSliderStyle (juce::Slider::LinearVertical);
    preVolumeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&postVolumeKnob);
    postVolumeKnob.setTextValueSuffix("Post Volume");
    postVolumeKnob.setSliderStyle (juce::Slider::LinearVertical);
    postVolumeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&lowCutKnob);
    lowCutKnob.setTextValueSuffix("Low Cut");
    lowCutKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowCutKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&highCutKnob);
    highCutKnob.setTextValueSuffix("High Cut");
    highCutKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&characterKnob);
    characterKnob.setTextValueSuffix("Character");
    characterKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    characterKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    driveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "drive", driveKnob));
    preVolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "pre volume", preVolumeKnob));
    postVolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "post volume", postVolumeKnob));
    lowCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "low cut", lowCutKnob));
    highCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "high cut", highCutKnob));
    characterAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "character", characterKnob));

    setSize (600, 400);
}

DistortAudioProcessorEditor::~DistortAudioProcessorEditor()
{
}

//==============================================================================
void DistortAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::antiquewhite);
    g.setColour(juce::Colours::black);

    driveKnob.setColour(0xff000000, juce::Colours::black);
    preVolumeKnob.setColour(0xff000000, juce::Colours::black);
    postVolumeKnob.setColour(0xff000000, juce::Colours::black);
    lowCutKnob.setColour(0xff000000, juce::Colours::black);
    highCutKnob.setColour(0xff000000, juce::Colours::black);
    characterKnob.setColour(0xff000000, juce::Colours::black);

    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 20.0f, juce::Font::bold));
    g.drawText("Drive", ((getWidth() / 2) - 200 / 2), ((getHeight() / 2) - 150 / 2), 200, 200, juce::Justification::centred);
    g.drawText("Pre Volume", ((getWidth() / 2) - 550 / 2), ((getHeight() / 2) - 370 / 2), 150, 100, juce::Justification::centred);
    g.drawText("Post Volume", ((getWidth() / 2) + 250 / 2), ((getHeight() / 2) - 370 / 2), 150, 100, juce::Justification::centred);
    g.drawText("Low Cut", ((getWidth() / 2) - 350 / 2), ((getHeight() / 2) + 170 / 2), 100, 100, juce::Justification::centred);
    g.drawText("High Cut", ((getWidth() / 2) + 150 / 2), ((getHeight() / 2) + 170 / 2), 100, 100, juce::Justification::centred);
    g.drawText("Character", ((getWidth() / 2) - 150 / 2), ((getHeight() / 2) + 250 / 2), 150, 100, juce::Justification::centred);

}

void DistortAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    driveKnob.setBounds(((getWidth() / 2) - 150 / 2), ((getHeight() / 2) - 250 / 2), 150, 150);
    preVolumeKnob.setBounds(((getWidth() / 2) - 420 / 2), ((getHeight() / 2) - 250 / 2), 20, 250);
    postVolumeKnob.setBounds(((getWidth() / 2) + 380 / 2), ((getHeight() / 2) - 250 / 2), 20, 250);
    lowCutKnob.setBounds(((getWidth() / 2) - 330 / 2), ((getHeight() / 2) + 100 / 2), 80, 80);
    highCutKnob.setBounds(((getWidth() / 2) + 170 / 2), ((getHeight() / 2) + 100 / 2), 80, 80);
    characterKnob.setBounds(((getWidth() / 2) - 70 / 2), ((getHeight() / 2) + 170 / 2), 70, 70);
}
