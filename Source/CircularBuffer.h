/*
  ==============================================================================

    CircularBuffer.h
    Created: 7 Apr 2024 2:45:36pm
    Author:  regnier

    A simple (and efficient) implementation of a circular buffer. Size must be a power of 2.
    Based on bitwise AND instead of modulo or branching. Ref. https://homepage.cs.uiowa.edu/~jones/bcd/mod.shtml#exmod2

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// nice convenient DEFINE from Mutable Instruments
#define GET_INTEGRAL_FRACTIONAL(x)\
    int32_t x ## _integral = static_cast<int32_t>(x); \
    float x ## _fractional = x - static_cast<float>(x ## _integral);

class CircularBuffer 
{
   public:
       CircularBuffer();
       ~CircularBuffer();
       void prepare(const juce::dsp::ProcessSpec& spec);
       void initBuffer(int numSamples);

       void writeBuffer(float value);

       float readBuffer(float delay);
       float readBufferLinear(float delay);
       float readBufferCubic(float delay);
       float readBufferHermite(float delay);
       
   
   private: 
       
       // float *buffer;

       juce::AudioBuffer<float> buffer;

       uint32_t size;
       uint32_t mask;

       int sampleRate;
       int writePointer{ 0 };
       
};