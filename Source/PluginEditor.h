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
class Test_circ_bufferAudioProcessorEditor  : public juce::AudioProcessorEditor,
    private juce::Slider::Listener
{
public:
    Test_circ_bufferAudioProcessorEditor (Test_circ_bufferAudioProcessor&);
    ~Test_circ_bufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    juce::Slider delaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delaySliderAttachment;

    juce::Slider fbkSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fbkSliderAttachment;

    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;

    juce::Label delayLabel;
    juce::Label fbkLabel;
    juce::Label mixLabel;

    Test_circ_bufferAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Test_circ_bufferAudioProcessorEditor)
};
