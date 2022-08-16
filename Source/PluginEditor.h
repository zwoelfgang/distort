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
class DistortAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistortAudioProcessorEditor (DistortAudioProcessor&);
    ~DistortAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider driveKnob;
    juce::Slider preVolumeKnob;
    juce::Slider postVolumeKnob;
    juce::Slider lowCut;
    juce::Slider highCut;
    juce::Slider character;

    juce::AudioProcessorValueTreeState::SliderAttachment driveAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment preVolumeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment postVolumeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment lowCutAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment highCutAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment characterAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistortAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortAudioProcessorEditor)
};
