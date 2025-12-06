#include "SignalChain.h"

void SignalChain::prepare(double sampleRate, int blockSize)
{
    // prepare each DSP block here later
}

void SignalChain::process(float** input, float** output, int numSamples, int numChannels)
{
    // this is where your final DSP chain logic will run
    // For now, just pass through:
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < numSamples; ++i)
            output[ch][i] = input ? input[ch][i] : 0.0f;
    }
}
