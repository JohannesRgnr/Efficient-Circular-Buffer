/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Test_circ_bufferAudioProcessor::Test_circ_bufferAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    apvts.addParameterListener("DELAY", this);
    apvts.addParameterListener("FEEDBACK", this);
    apvts.addParameterListener("MIX", this);
}

Test_circ_bufferAudioProcessor::~Test_circ_bufferAudioProcessor()
{
}

//==============================================================================
const juce::String Test_circ_bufferAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Test_circ_bufferAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Test_circ_bufferAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Test_circ_bufferAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Test_circ_bufferAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Test_circ_bufferAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Test_circ_bufferAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Test_circ_bufferAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Test_circ_bufferAudioProcessor::getProgramName (int index)
{
    return {};
}

void Test_circ_bufferAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
} 

//==============================================================================
void Test_circ_bufferAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };

    circBuff.prepare(spec);
    circBuff.initBuffer(262144); // around 5.5 seconds in 48kHz

    //delayBuffer.setSize(getTotalNumOutputChannels(), circBuff.delaySize);
    //delayBuffer.clear();

    smoothDelay.reset(getSampleRate(), 0.03);     // ramp length of 30 ms.. arbitrary.. 
    smoothDelay.setCurrentAndTargetValue(0.0);
}

void Test_circ_bufferAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Test_circ_bufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Test_circ_bufferAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto leftInSamples = buffer.getReadPointer(0);
    auto rightInSamples = buffer.getReadPointer(1);
    auto leftOutSamples = buffer.getWritePointer(0);
    auto rightOutSamples = buffer.getWritePointer(1);

    
    smoothDelay.setTargetValue(delayTime);
    
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        float inputSample = (*(leftInSamples + sample) + *(rightInSamples + sample)) * 0.5; // mixdown to mono

        auto currentDelayTime = smoothDelay.getNextValue();

        float delayTimeSmps = currentDelayTime * getSampleRate();
       
        /***************************** read from delay line *******************************/
        // Without interpolation.. delay time truncated to integer samples
        // float delaySample = circBuff.readBuffer(delayTimeSmps);

        // With Hermite interpolation
        float delayedSample = circBuff.readBufferHermite(delayTimeSmps);

        /***************************** dry/wet mix and output *****************************/
        float outputSample = delayedSample * mix + inputSample * (1 - mix);
        *(leftOutSamples + sample)  = outputSample;
        *(rightOutSamples + sample) = outputSample;
         
        /**************************** write into delay line *******************************/
        circBuff.writeBuffer(inputSample + feedback*delayedSample);         
    }      
}




//==============================================================================
bool Test_circ_bufferAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Test_circ_bufferAudioProcessor::createEditor()
{
    return new Test_circ_bufferAudioProcessorEditor (*this);
}

//==============================================================================
void Test_circ_bufferAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Test_circ_bufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Test_circ_bufferAudioProcessor();
}



juce::AudioProcessorValueTreeState::ParameterLayout Test_circ_bufferAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY", "delay", juce::NormalisableRange<float>(0.0f, 5.0f, 0.0001f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "feedback", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "mix", 0.0f, 1.0f, 0.0f));

    return { params.begin(), params.end() };

}

void Test_circ_bufferAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "DELAY")
    {
        delayTime = newValue;
    }

    if (parameterID == "FEEDBACK")
    {
        feedback = newValue;
    }

    if (parameterID == "MIX")
    {
        mix = newValue;
    }
      
}
