#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "NAM/NamLoader.h"

namespace ronnie
{

class NamModelProcessor
{
public:
    NamModelProcessor()
    {
#if !(defined(RONNIE_USE_NAM_CORE) && RONNIE_USE_NAM_CORE)
        // Load default model (bypass/linear)
        auto m = std::make_unique<nam::MockModel>();
        m->setGain(1.0f);
        model = std::move(m);
#endif
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
    }

    void reset()
    {
    }

    void loadModel(const juce::File& file)
    {
        // In a real scenario, we pass the file path to nam::get_dsp
        // For now, we simulate loading by switching to our Model placeholder
        if (file.existsAsFile())
        {
            auto newModel = nam::get_dsp(file.getFullPathName().toStdString());
            const juce::ScopedLock sl(lock);
            model = std::move(newModel);
        }
    }

    // Helper to manually engage the "Amp" tone without a file for testing
    void loadPlaceholderAmp()
    {
        const juce::ScopedLock sl(lock);
#if defined(RONNIE_USE_NAM_CORE) && RONNIE_USE_NAM_CORE
        model.reset();
#else
        model = std::make_unique<nam::MockModel>();
#endif
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        // Thread safety: Try to lock. If busy (loading), skip processing to avoid audio glitches.
        const juce::ScopedTryLock sl(lock);
        if (!sl.isLocked())
            return;

        if (!model) return;

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const int numSamples = (int)outputBlock.getNumSamples();
        const int numChannels = (int)outputBlock.getNumChannels();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* src = inputBlock.getChannelPointer(ch);
            auto* dst = outputBlock.getChannelPointer(ch);

            model->process(const_cast<float*>(src), dst, numSamples);
        }
    }

private:
    juce::CriticalSection lock;
    std::unique_ptr<nam::DSP> model;
    double sampleRate = 44100.0;
};

} // namespace ronnie
