#pragma once
#include <JuceHeader.h>
#include "../RonnieAudioProcessor.h"

namespace ronnie
{

class CabOutputPanel : public juce::Component
{
public:
    CabOutputPanel(RonnieAudioProcessor& processor) : audioProcessor(processor)
    {
        addAndMakeVisible(loadIrButton);
        loadIrButton.setButtonText("Load IR");
        loadIrButton.onClick = [this] { loadIR(); };

        addAndMakeVisible(irNameLabel);
        irNameLabel.setJustificationType(juce::Justification::centredLeft);
        irNameLabel.setText("No IR loaded", juce::dontSendNotification);

        // Output Gain
        addAndMakeVisible(outGainSlider);
        outGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        outGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        outGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "output_gain", outGainSlider);

        addAndMakeVisible(outGainLabel);
        outGainLabel.setText("Output Level", juce::dontSendNotification);
        outGainLabel.setJustificationType(juce::Justification::centred);
    }

    void loadIR()
    {
        chooser = std::make_unique<juce::FileChooser>("Select an Impulse Response...",
                                                      juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                      "*.wav;*.ir");

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                audioProcessor.loadCabIR(file);
                irNameLabel.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
            }
        });
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        auto topRow = area.removeFromTop(30);
        loadIrButton.setBounds(topRow.removeFromLeft(80));
        irNameLabel.setBounds(topRow);

        area.removeFromTop(20);

        outGainLabel.setBounds(area.removeFromTop(20));
        outGainSlider.setBounds(area.removeFromTop(100));
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 1.0f);

        g.setFont(18.0f);
        g.drawText("CAB / OUT", getLocalBounds().removeFromTop(20), juce::Justification::centred, true);
    }

private:
    RonnieAudioProcessor& audioProcessor;

    juce::TextButton loadIrButton;
    juce::Label irNameLabel;

    juce::Slider outGainSlider;
    juce::Label outGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;

    std::unique_ptr<juce::FileChooser> chooser;
};

}
