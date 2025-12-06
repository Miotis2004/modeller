#include "AmpModelerEngine.h"

namespace ronnie
{

AmpModelerEngine::AmpModelerEngine()
{
}

AmpModelerEngine::~AmpModelerEngine()
{
}

void AmpModelerEngine::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(0.05);

    outputGain.prepare(spec);
    outputGain.setRampDurationSeconds(0.05);

    for (int i = 0; i < 2; ++i)
        gate[i].prepare(sampleRate);

    namProcessor.prepare(spec);

    cabSim.prepare(spec);

    postEq.prepare(spec);
    postEq.reset();

    // Initialize EQ filters
    setPostEq(1.0f, 1.0f, 1.0f);
}

void AmpModelerEngine::process(const float* const* inputChannelData, float* const* outputChannelData, int numSamples, int numChannels)
{
    juce::dsp::AudioBlock<const float> inputBlock(inputChannelData, numChannels, numSamples);
    juce::dsp::AudioBlock<float> outputBlock(outputChannelData, numChannels, numSamples);

    // Safety check for aliasing:
    // Only copy if the actual buffers are different locations.
    if (inputChannelData[0] != outputChannelData[0])
        outputBlock.copyFrom(inputBlock);

    juce::dsp::ProcessContextReplacing<float> context(outputBlock);

    // 1. Input Gain
    inputGain.process(context);

    // 2. Noise Gate
    for (int ch = 0; ch < numChannels && ch < 2; ++ch)
    {
        auto* data = outputBlock.getChannelPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            data[i] = gate[ch].processSample(data[i]);
        }
    }

    // 3. Amp Model (NAM)
    namProcessor.process(context);

    // 4. Cabinet Simulation
    cabSim.process(context);

    // 5. Post EQ
    postEq.process(context);

    // 6. Output Gain
    outputGain.process(context);
}

void AmpModelerEngine::reset()
{
    inputGain.reset();
    namProcessor.reset();
    cabSim.reset();
    postEq.reset();
    outputGain.reset();
}

void AmpModelerEngine::setInputGain(float gain)
{
    inputGain.setGainLinear(gain);
}

void AmpModelerEngine::setGateThreshold(float thresholdLinear)
{
    for (int i = 0; i < 2; ++i)
        gate[i].setThreshold(thresholdLinear);
}

void AmpModelerEngine::setOutputGain(float gain)
{
    outputGain.setGainLinear(gain);
}

void AmpModelerEngine::setPostEq(float bass, float mid, float treble)
{
    // Bass: Low Shelf, 100Hz
    *postEq.get<0>().coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, 100.0f, 0.707f, bass);

    // Mid: Peak, 800Hz
    *postEq.get<1>().coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 800.0f, 1.0f, mid);

    // Treble: High Shelf, 3000Hz
    *postEq.get<2>().coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, 3000.0f, 0.707f, treble);
}

void AmpModelerEngine::loadAmpModel(const juce::File& file)
{
    namProcessor.loadModel(file);
}

void AmpModelerEngine::loadCabImpulse(const juce::File& file)
{
    // loadImpulseResponse(file, stereoMode, trim, size, normalise)
    cabSim.loadImpulseResponse(file,
                               juce::dsp::Convolution::Stereo::yes,
                               juce::dsp::Convolution::Trim::yes,
                               0,
                               juce::dsp::Convolution::Normalise::yes);
}

} // namespace ronnie
