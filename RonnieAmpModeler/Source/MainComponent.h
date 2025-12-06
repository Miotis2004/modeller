#pragma once

#include <JuceHeader.h>
#include "AmpModelerEngine.h"

//==============================================================================
// This is the main UI + audio component for your app.
class AmpModelerComponent : public juce::AudioAppComponent
{
public:
    AmpModelerComponent();
    ~AmpModelerComponent() override;

    // Audio callbacks
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // === DSP engine ===
    AmpModelerEngine engine;

    // === UI controls ===
    juce::Slider inputGainSlider;
    juce::Label  inputGainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpModelerComponent)
};
