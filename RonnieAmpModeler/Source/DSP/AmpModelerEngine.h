#pragma once

#include <JuceHeader.h>
#include "NamModelProcessor.h"
#include "SimpleGate.h"

namespace ronnie
{

class AmpModelerEngine
{
public:
    AmpModelerEngine();
    ~AmpModelerEngine();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(const float* const* inputChannelData, float* const* outputChannelData, int numSamples, int numChannels);
    void reset();

    // Parameters
    void setInputGain(float gain);
    void setGateThreshold(float thresholdLinear);
    void setAmpGain(float gain);
    void setAmpMaster(float gain);
    void setOutputGain(float gain);

    // EQ Parameters
    void setPostEq(float bass, float mid, float treble);

    // Model Loading
    void loadAmpModel(const juce::File& file);
    void loadCabImpulse(const juce::File& file);

    // Testing helper
    void enablePlaceholderAmp() { namProcessor.loadPlaceholderAmp(); }

private:
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    // --- Signal Chain Components ---

    // 1. Input Gain
    juce::dsp::Gain<float> inputGain;

    // 2. Noise Gate
    SimpleGate gate[2]; // Stereo gate

    // 3. Amp Model (NAM)
    juce::dsp::Gain<float> ampGain;
    NamModelProcessor namProcessor;

    // 4. Amp Master
    juce::dsp::Gain<float> ampMaster;

    // 5. Cabinet Simulation
    juce::dsp::Convolution cabSim;

    // 6. Post EQ
    // Low Shelf, Peaking (Mid), High Shelf
    using Filter = juce::dsp::IIR::Filter<float>;
    juce::dsp::ProcessorChain<Filter, Filter, Filter> postEq;

    // 7. Output Gain
    juce::dsp::Gain<float> outputGain;

    // Internal processing spec
    juce::dsp::ProcessSpec spec;
};

} // namespace ronnie
