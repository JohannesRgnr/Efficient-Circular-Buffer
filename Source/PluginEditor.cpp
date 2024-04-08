/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Test_circ_bufferAudioProcessorEditor::Test_circ_bufferAudioProcessorEditor (Test_circ_bufferAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (320, 200);

    delaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 90, 24);
    delaySlider.setTextValueSuffix("s");
    delaySlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    delaySlider.setValue(0.0);

    fbkSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fbkSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 90, 24);
    fbkSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    fbkSlider.setValue(0.0);

    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 90, 24);
    mixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    mixSlider.setValue(0.0);

    delayLabel.setText("delay time", juce::dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    delayLabel.setJustificationType(juce::Justification::centredBottom);

    fbkLabel.setText("feedback", juce::dontSendNotification);
    fbkLabel.attachToComponent(&fbkSlider, false);
    fbkLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fbkLabel.setJustificationType(juce::Justification::centredBottom);

    mixLabel.setText("mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mixLabel.setJustificationType(juce::Justification::centredBottom);

    addAndMakeVisible(&delaySlider);
    addAndMakeVisible(&fbkSlider);
    addAndMakeVisible(&mixSlider);

    addAndMakeVisible(&delayLabel);
    addAndMakeVisible(&fbkLabel);
    addAndMakeVisible(&mixLabel);

    delaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY", delaySlider);
    fbkSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FEEDBACK", fbkSlider);
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixSlider);

    delaySlider.addListener(this);
    fbkSlider.addListener(this);
    mixSlider.addListener(this);
}

Test_circ_bufferAudioProcessorEditor::~Test_circ_bufferAudioProcessorEditor()
{
}

//==============================================================================
void Test_circ_bufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
}

void Test_circ_bufferAudioProcessorEditor::resized()
{
    delaySlider.setBounds(10, 40, 100, 100);
    fbkSlider.setBounds(110, 40, 100, 100);
    mixSlider.setBounds(210, 40, 100, 100);
}


void Test_circ_bufferAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{



}