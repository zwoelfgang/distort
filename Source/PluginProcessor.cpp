/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortAudioProcessor::DistortAudioProcessor()
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
    state = std::make_unique<juce::AudioProcessorValueTreeState>(*this, nullptr);

    state->createAndAddParameter("drive", "Drive", "Drive", juce::NormalisableRange<float>(1.f, 24.f, 0.001f), 1.f, nullptr, nullptr);
    state->createAndAddParameter("pre volume", "Pre Volume", "Pre Volume", juce::NormalisableRange<float>(1.f, 6.f, 0.001f), 1.f, nullptr, nullptr);
    state->createAndAddParameter("post volume", "Post Volume", "Post Volume", juce::NormalisableRange<float>(1.f, 6.f, 0.001f), 1.f, nullptr, nullptr);
    state->createAndAddParameter("low cut", "Low Cut", "Low Cut", juce::NormalisableRange<float>(20.f, 20000.f, 1.f), 20.f, nullptr, nullptr);
    state->createAndAddParameter("high cut", "High Cut", "High Cut", juce::NormalisableRange<float>(20.f, 20000.f, 1.f), 20000.f, nullptr, nullptr);
    state->createAndAddParameter("character", "Character", "Character", juce::NormalisableRange<float>(1.f, 6.f, 0.5f), 2.5f, nullptr, nullptr);

    state->state = juce::ValueTree("drive");
    state->state = juce::ValueTree("pre volume");
    state->state = juce::ValueTree("post volume");
    state->state = juce::ValueTree("low cut");
    state->state = juce::ValueTree("high cut");
    state->state = juce::ValueTree("character");
}

DistortAudioProcessor::~DistortAudioProcessor()
{
}

//==============================================================================
const juce::String DistortAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(*state);
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCut, sampleRate, 4);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.highCut, sampleRate, 4);

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    leftLowCut.get<0>().coefficients = lowCutCoefficients[0];
    rightLowCut.get<0>().coefficients = lowCutCoefficients[0];

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftHighCut.get<0>().coefficients = highCutCoefficients[0];
    rightHighCut.get<0>().coefficients = highCutCoefficients[0];
}

void DistortAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DistortAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    auto chainSettings = getChainSettings(*state);

    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCut, getSampleRate(), 4);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCut, getSampleRate(), 4);

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    leftLowCut.get<0>().coefficients = lowCutCoefficients[0];
    rightLowCut.get<0>().coefficients = lowCutCoefficients[0];

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftHighCut.get<0>().coefficients = highCutCoefficients[0];
    rightHighCut.get<0>().coefficients = highCutCoefficients[0];

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float drive = *state->getRawParameterValue("drive");
    float preVol = *state->getRawParameterValue("pre volume");
    float postVol = *state->getRawParameterValue("post volume");
    float lCut = *state->getRawParameterValue("low cut");
    float hCut = *state->getRawParameterValue("high cut");
    float character = *state->getRawParameterValue("character");

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            *channelData *= preVol;

            *channelData =  ((*channelData * drive) / pow((1 + pow(abs(*channelData * drive), character)), 1 / character));
                
            *channelData *= postVol;

            channelData++;
        }
    }
}

juce::AudioProcessorValueTreeState& DistortAudioProcessor::getState()
{
    return *state;
}

//==============================================================================
bool DistortAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortAudioProcessor::createEditor()
{
    return new DistortAudioProcessorEditor (*this);
}

//==============================================================================
void DistortAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void DistortAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        state->state = tree;
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& state)
{
    ChainSettings settings;

    settings.lowCut = state.getRawParameterValue("low cut")->load();
    settings.highCut = state.getRawParameterValue("high cut")->load();

    return settings;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortAudioProcessor();
}
