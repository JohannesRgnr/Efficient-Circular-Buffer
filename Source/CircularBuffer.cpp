/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 7 Apr 2024 2:45:36pm
    Author:  regnier
    
    A simple (and efficient) implementation of a circular buffer. Size must be a power of 2.
    Based on bitwise AND instead of modulo or branching. Ref. https://homepage.cs.uiowa.edu/~jones/bcd/mod.shtml#exmod2 

  ==============================================================================
*/

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer()
{
    writePointer = 0;
    size = 0;
    mask = 0;
    sampleRate = 44100;
    buffer = nullptr;
}


CircularBuffer::~CircularBuffer()
{
    free(buffer);
}

void CircularBuffer::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

}


/**
* @brief initializes and clears buffer
* @param int numSamples
*   numSamples needs to be a power of 2
*/
void CircularBuffer::initBuffer(int numSamples)
{
    jassert(numSamples >= 0);
    jassert((numSamples & (numSamples - 1)) == 0); // check if power of two
    
    size = numSamples;
    mask = size - 1;
    
    buffer = (float*)malloc(size * sizeof(float));
    // buffer = new float[size];

    if (buffer != nullptr)
    {
        for (int i = 0; i < size; i++)
        {
            buffer[i] = 0.0f;
        }
    }

}

/**
* @brief writes value into the buffer
* @param float value
*   value to be written in buffer
*/
void CircularBuffer::writeBuffer( float value) {
    *(buffer + (++writePointer & mask)) = value;
}

/**
// If using a juce::AudioBuffer class instead of a simple float array:

void CircularBuffer::writeBuffer(juce::AudioBuffer<float> *buffer, float value) {
    buffer->setSample(0, ++writePointer & delayMask, value); 
}

*/

/**
========================================== Read from buffer =============================================
* @param delay: value (in samples) to be read from buffer counting backwards from the last written value
* For instance, the last written value can be read with readBuffer(0) 
* Interpolation methods: no interpolation, linear, cubic, Hermite
* Ref.  https://www.musicdsp.org/en/latest/Other/93-hermite-interpollation.html
*       https://paulbourke.net/miscellaneous/interpolation/
=========================================================================================================
*/

/**
* @brief reads value from buffer, with no interpolation
* @param float delay
*/
float CircularBuffer::readBuffer(float delay) {
    return *(buffer + ((uint32_t)(writePointer - delay) & mask));
}

/**
// If using a juce::AudioBuffer class instead of a simple float array:

float CircularBuffer::readBuffer(juce::AudioBuffer<float> *buffer, float delay) {
    readPointer = (uint32_t)(writePointer - delayTimeSmps);
    return buffer->getSample(0, readPointer & delayMask);
}

*/


/**
* @brief reads value from buffer, with linear interpolation
* @param float delay
*/
float CircularBuffer::readBufferLinear(float delay)
{
    float readPointer = writePointer - delay;
    GET_INTEGRAL_FRACTIONAL(readPointer); // get integral and fractional parts

    float x0, x1;
    x0 = *(buffer + (readPointer_integral & mask));
    x1 = *(buffer + ((readPointer_integral + 1) & mask));

    return x0 + (x1 - x0) * readPointer_fractional;
}

/**
* @brief reads value from buffer, with cubic interpolation
* @param float delay
*/
float CircularBuffer::readBufferCubic(float delay)
{
    float readPointer = writePointer - delay;
    GET_INTEGRAL_FRACTIONAL(readPointer);

    float xm1, x0, x1, x2;
    
    xm1 = *(buffer + ((readPointer_integral - 1) & mask));
    x0 = *(buffer + (readPointer_integral & mask));
    x1 = *(buffer + ((readPointer_integral + 1) & mask));
    x2 = *(buffer + ((readPointer_integral + 2) & mask));

    float a0, a1, a2, a3, mu2;
    float mu = readPointer_fractional;

    mu2 = mu * mu;
    a0 = x2 - x1 - xm1 + x0;
    a1 = xm1 - x0 - a0;
    a2 = x1 - xm1;
    a3 = x0;

    return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

/**
* @brief reads value from buffer, with Hermite interpolation
* @param float delay
*/
float CircularBuffer::readBufferHermite(float delay) {
    float readPointer = writePointer - delay;
    GET_INTEGRAL_FRACTIONAL(readPointer);

    float xm1, x0, x1, x2;
    float c0, c1, c2, c3;

    xm1 = *(buffer + ((readPointer_integral - 1) & mask));
    x0  = *(buffer + (readPointer_integral & mask));
    x1  = *(buffer + ((readPointer_integral + 1) & mask));
    x2  = *(buffer + ((readPointer_integral + 2) & mask));

    c0 = x0;
    c1 = 0.5 * (x1 - xm1);
    c3 = 1.5 * (x0 - x1) + 0.5f * (x2 - xm1);
    c2 = xm1 - x0 + c1 - c3;

    return ((c3 * readPointer_fractional + c2) * readPointer_fractional + c1) * readPointer_fractional + c0;
}
