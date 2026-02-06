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

        addAndMakeVisible(presetList);

        addAndMakeVisible(loadPresetButton);
        loadPresetButton.setButtonText("Load Preset");
        loadPresetButton.onClick = [this] { loadPreset(); };

        refreshPresetList();
    }

    void savePreset()
    {
        presetNamePrompt = std::make_unique<juce::AlertWindow>(
            "Save Preset",
            "Enter a preset name:",
            juce::AlertWindow::NoIcon);
        presetNamePrompt->addTextEditor("presetName", "");
        presetNamePrompt->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
        presetNamePrompt->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        presetNamePrompt->enterModalState(true, juce::ModalCallbackFunction::create([this](int result)
        {
            if (result == 1 && presetNamePrompt != nullptr)
            {
                auto name = presetNamePrompt->getTextEditor("presetName")->getText().trim();
                if (name.isEmpty())
                    name = "Preset_" + juce::Time::getCurrentTime().formatted("%Y%m%d-%H%M%S");

                presetManager.savePreset(name);
                refreshPresetList();
            }
            presetNamePrompt.reset();
        }), true);
    }

    void loadPreset()
    {
        auto index = presetList.getSelectedItemIndex();
        if (index >= 0 && index < presetFiles.size())
        {
            presetManager.loadPreset(presetFiles.getReference(index));
        }
    }

    void resized() override
    {
        auto area = getLocalBounds();

        auto topBar = area.removeFromTop(30);
        savePresetButton.setBounds(topBar.removeFromRight(100).reduced(2));
        loadPresetButton.setBounds(topBar.removeFromRight(100).reduced(2));
        presetList.setBounds(topBar.removeFromRight(220).reduced(2));

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
    juce::ComboBox presetList;
    juce::Array<juce::File> presetFiles;
    std::unique_ptr<juce::AlertWindow> presetNamePrompt;

    InputPanel inputPanel;
    AmpPanel ampPanel;
    CabOutputPanel cabPanel;

    void refreshPresetList()
    {
        presetList.clear();
        presetFiles = presetManager.getPresetFiles();
        struct PresetFileSorter
        {
            static int compareElements(const juce::File& first, const juce::File& second)
            {
                if (first.getLastModificationTime() > second.getLastModificationTime())
                    return -1;
                if (first.getLastModificationTime() < second.getLastModificationTime())
                    return 1;
                return 0;
            }
        };

        presetFiles.sort(PresetFileSorter{});

        for (int i = 0; i < presetFiles.size(); ++i)
        {
            const auto& file = presetFiles.getReference(i);
            auto label = file.getFileNameWithoutExtension() + " (" +
                         file.getLastModificationTime().formatted("%Y-%m-%d %H:%M") + ")";
            presetList.addItem(label, i + 1);
        }

        if (presetFiles.isEmpty())
        {
            presetList.setText("No presets", juce::dontSendNotification);
            presetList.setEnabled(false);
        }
        else
        {
            presetList.setSelectedItemIndex(0, juce::dontSendNotification);
            presetList.setEnabled(true);
        }
    }
};

}
