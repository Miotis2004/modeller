#include "AmpModelerEngine.h"

void AmpModelerEngine::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
}

void AmpModelerEngine::process(const float* const* input,
    float* const* output,
    int                 numSamples,
    int                 numChannels)
{
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* in = (input != nullptr ? input[ch] : nullptr);
        float* out = (output != nullptr ? output[ch] : nullptr);

        if (in == nullptr || out == nullptr)
            continue;

        for (int i = 0; i < numSamples; ++i)
            out[i] = in[i] * inputGain; // simple gain for now
    }
}
