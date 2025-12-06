#pragma once

#include <JuceHeader.h>
#include "../RonnieAudioProcessor.h"
#include "InputPanel.h"
#include "AmpPanel.h"
#include "CabOutputPanel.h"
#include "../PresetManager.h"

namespace ronnie
{

class MainLayout : public juce::Component
{
public:
    MainLayout(RonnieAudioProcessor& processor)
        : audioProcessor(processor),
          presetManager(processor),
          inputPanel(processor),
          ampPanel(processor),
          cabPanel(processor)
    {
        addAndMakeVisible(inputPanel);
        addAndMakeVisible(ampPanel);
        addAndMakeVisible(cabPanel);

        addAndMakeVisible(savePresetButton);
        savePresetButton.setButtonText("Save Preset");
        savePresetButton.onClick = [this] { savePreset(); };

        addAndMakeVisible(loadPresetButton);
        loadPresetButton.setButtonText("Load Preset");
        loadPresetButton.onClick = [this] { loadPreset(); };
    }

    void savePreset()
    {
        // Simple alert window to get name (blocking for simplicity in this context)
        // In real app use async.
        // We'll just default to "MyPreset" + timestamp for this proof of concept
        juce::String name = "Preset_" + juce::Time::getCurrentTime().formatted("%Y%m%d-%H%M%S");
        presetManager.savePreset(name);
    }

    void loadPreset()
    {
        chooser = std::make_unique<juce::FileChooser>("Load Preset...",
                                                      presetManager.getPresetsDirectory(),
                                                      "*.json");
        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(flags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                presetManager.loadPreset(file);
            }
        });
    }

    void resized() override
    {
        auto area = getLocalBounds();

        auto topBar = area.removeFromTop(30);
        savePresetButton.setBounds(topBar.removeFromRight(100).reduced(2));
        loadPresetButton.setBounds(topBar.removeFromRight(100).reduced(2));

        int width = area.getWidth() / 3;

        inputPanel.setBounds(area.removeFromLeft(width));
        cabPanel.setBounds(area.removeFromRight(width));
        ampPanel.setBounds(area); // Center
    }

private:
    RonnieAudioProcessor& audioProcessor;
    PresetManager presetManager;

    juce::TextButton savePresetButton;
    juce::TextButton loadPresetButton;
    std::unique_ptr<juce::FileChooser> chooser;

    InputPanel inputPanel;
    AmpPanel ampPanel;
    CabOutputPanel cabPanel;
};

}
