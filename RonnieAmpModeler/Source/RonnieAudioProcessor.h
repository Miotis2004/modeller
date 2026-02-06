#pragma once

#include <JuceHeader.h>
#include "DSP/AmpModelerEngine.h"

namespace ronnie
{

class RonnieAudioProcessor : public juce::AudioProcessor
{
public:
    RonnieAudioProcessor()
        : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
    {
        // Add listeners to parameters to update engine
        // In a real plugin, we process parameters in processBlock or use atomics.
        // For simplicity here, we'll use the float pointers from APVTS in processBlock.
    }

    ~RonnieAudioProcessor() override = default;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        engine.prepare(sampleRate, samplesPerBlock);
    }

    void releaseResources() override
    {
        engine.reset();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ignoreUnused(midiMessages);
        juce::ScopedNoDenormals noDenormals;

        // Update Engine Parameters from APVTS
        // Note: In strict realtime code, getRawParameterValue is safe.
        engine.setInputGain(*apvts.getRawParameterValue("input_gain"));
        engine.setGateThreshold(*apvts.getRawParameterValue("gate_threshold"));
        engine.setAmpGain(*apvts.getRawParameterValue("amp_gain"));
        engine.setAmpMaster(*apvts.getRawParameterValue("amp_master"));
        engine.setOutputGain(*apvts.getRawParameterValue("output_gain"));

        // Update EQ
        // Convert 0..10 or similar ranges to linear gain (e.g., 0dB to +12dB or +/-dB)
        // Let's assume params are gain in dB, so we convert to linear.
        // Actually, let's make params -12 to +12 dB
        float bass = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("eq_bass")->load());
        float mid = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("eq_mid")->load());
        float treble = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("eq_treble")->load());

        // Only update if changed to avoid allocations in audio thread
        if (bass != lastBass || mid != lastMid || treble != lastTreble)
        {
            engine.setPostEq(bass, mid, treble);
            lastBass = bass;
            lastMid = mid;
            lastTreble = treble;
        }

        // Process Audio
        // We need to pass raw pointers to the engine
        auto numSamples = buffer.getNumSamples();
        auto numChannels = buffer.getNumChannels();

        // Since engine expects const float* const*, we can cast
        // But engine might assume input != output if we pass different pointers.
        // Here we do in-place.

        // Helper to get array of pointers
        const float* inputPtrs[2];
        float* outputPtrs[2];

        for (int i=0; i<numChannels && i<2; ++i)
        {
            inputPtrs[i] = buffer.getReadPointer(i);
            outputPtrs[i] = buffer.getWritePointer(i);
        }

        engine.process(inputPtrs, outputPtrs, numSamples, numChannels);
    }

    //==============================================================================
#if defined(JUCE_MODULE_AVAILABLE_juce_gui_basics)
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
#else
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
#endif

    const juce::String getName() const override { return "Ronnie Amp Modeler"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
    const juce::String getProgramName(int index) override
    {
        juce::ignoreUnused(index);
        return "Default";
    }
    void changeProgramName(int index, const juce::String& newName) override
    {
        juce::ignoreUnused(index, newName);
    }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName(apvts.state.getType()))
                apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }

    // Accessors
    AmpModelerEngine& getEngine() { return engine; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Helpers for file loading (to be called from UI)
    void loadAmpModel(const juce::File& file) { engine.loadAmpModel(file); }
    void loadCabIR(const juce::File& file) { engine.loadCabImpulse(file); }

private:
    AmpModelerEngine engine;
    juce::AudioProcessorValueTreeState apvts;

    // Cache for EQ params
    float lastBass = -1.0f;
    float lastMid = -1.0f;
    float lastTreble = -1.0f;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        // Input
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "input_gain", "Input Gain", juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f), 1.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "gate_threshold", "Gate Threshold", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

        // Amp Controls (Placeholder for future NAM params)
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "amp_gain", "Amp Gain", 0.0f, 10.0f, 5.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "amp_master", "Master", 0.0f, 2.0f, 1.0f));

        // EQ
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "eq_bass", "Bass", -12.0f, 12.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "eq_mid", "Mid", -12.0f, 12.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "eq_treble", "Treble", -12.0f, 12.0f, 0.0f));

        // Cab / Output
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "output_gain", "Output Level", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

        return layout;
    }
};

} // namespace ronnie
