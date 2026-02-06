#pragma once
#include <JuceHeader.h>
#include "../RonnieAudioProcessor.h"

namespace ronnie
{

class AmpPanel : public juce::Component
{
public:
    AmpPanel(RonnieAudioProcessor& processor) : audioProcessor(processor)
    {
        addAndMakeVisible(loadButton);
        loadButton.setButtonText("Load NAM Model");
        loadButton.onClick = [this] { loadModel(); };

        addAndMakeVisible(mockButton);
        mockButton.setButtonText("Use Default Amp");
        mockButton.onClick = [this]
        {
            audioProcessor.getEngine().enablePlaceholderAmp();
            modelNameLabel.setText("Default Model", juce::dontSendNotification);
        };

        // Amp Gain
        addAndMakeVisible(ampGainSlider);
        ampGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        ampGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        ampGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "amp_gain", ampGainSlider);

        addAndMakeVisible(ampGainLabel);
        ampGainLabel.setText("Gain", juce::dontSendNotification);
        ampGainLabel.setJustificationType(juce::Justification::centred);

        // EQ Sliders
        auto makeSlider = [&](juce::Slider& s, juce::Label& l, const juce::String& id, const juce::String& name)
        {
            addAndMakeVisible(s);
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

            addAndMakeVisible(l);
            l.setText(name, juce::dontSendNotification);
            l.setJustificationType(juce::Justification::centred);

            return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.getAPVTS(), id, s);
        };

        bassAttachment = makeSlider(bassSlider, bassLabel, "eq_bass", "Bass");
        midAttachment = makeSlider(midSlider, midLabel, "eq_mid", "Mid");
        trebleAttachment = makeSlider(trebleSlider, trebleLabel, "eq_treble", "Treble");

        // Fix for missing label visibility
        addAndMakeVisible(modelNameLabel);
        modelNameLabel.setJustificationType(juce::Justification::centred);
        modelNameLabel.setText("Default Model", juce::dontSendNotification);
    }

    void loadModel()
    {
        chooser = std::make_unique<juce::FileChooser>("Select a NAM model...",
                                                      juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                      "*.nam");

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                audioProcessor.loadAmpModel(file);
                modelNameLabel.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
            }
        });
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        auto header = area.removeFromTop(40);
        loadButton.setBounds(header.removeFromLeft(120));
        mockButton.setBounds(header.removeFromLeft(120));
        modelNameLabel.setBounds(header.removeFromLeft(200));

        // Upper section: Gain
        auto upper = area.removeFromTop(area.getHeight() / 2);
        ampGainLabel.setBounds(upper.getX(), upper.getBottom() - 100, upper.getWidth(), 20);
        ampGainSlider.setBounds(upper.getX(), upper.getBottom() - 80, upper.getWidth(), 80);

        // Lower section: EQ
        auto lower = area;
        int w = lower.getWidth() / 3;

        auto setBounds = [&](juce::Slider& s, juce::Label& l, juce::Rectangle<int> r)
        {
            l.setBounds(r.removeFromTop(20));
            s.setBounds(r);
        };

        setBounds(bassSlider, bassLabel, lower.removeFromLeft(w));
        setBounds(midSlider, midLabel, lower.removeFromLeft(w));
        setBounds(trebleSlider, trebleLabel, lower);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 1.0f);

        g.setFont(18.0f);
        g.drawText("AMP MODELER", getLocalBounds().removeFromTop(20), juce::Justification::centred, true);
    }

private:
    RonnieAudioProcessor& audioProcessor;

    juce::TextButton loadButton;
    juce::TextButton mockButton;
    juce::Label modelNameLabel;

    juce::Slider ampGainSlider;
    juce::Label ampGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampGainAttachment;

    juce::Slider bassSlider, midSlider, trebleSlider;
    juce::Label bassLabel, midLabel, trebleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment, midAttachment, trebleAttachment;

    std::unique_ptr<juce::FileChooser> chooser;
};

}
