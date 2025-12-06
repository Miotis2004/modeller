#pragma once

#include <JuceHeader.h>
#include "RonnieAudioProcessor.h"
#include "UI/MainLayout.h"

//==============================================================================
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
    // We own the processor here for standalone app
    ronnie::RonnieAudioProcessor processor;

    // The main UI
    ronnie::MainLayout mainLayout;

    // Settings Button
    juce::TextButton settingsButton;
    void openSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpModelerComponent)
};
