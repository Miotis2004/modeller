#pragma once

class AmpModelerEngine
{
public:
    AmpModelerEngine() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(const float* const* input,
        float* const* output,
        int                 numSamples,
        int                 numChannels);

    void setInputGain(float newGain) { inputGain = newGain; }

private:
    double currentSampleRate = 44100.0;
    int    currentBlockSize = 512;
    float  inputGain = 1.0f;
};
