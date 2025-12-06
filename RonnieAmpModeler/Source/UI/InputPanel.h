#pragma once
#include <JuceHeader.h>
#include "../RonnieAudioProcessor.h"

namespace ronnie
{

class InputPanel : public juce::Component
{
public:
    InputPanel(RonnieAudioProcessor& processor) : audioProcessor(processor)
    {
        // Gain Slider
        addAndMakeVisible(inputGainSlider);
        inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "input_gain", inputGainSlider);

        addAndMakeVisible(inputGainLabel);
        inputGainLabel.setText("Input Gain", juce::dontSendNotification);
        inputGainLabel.setJustificationType(juce::Justification::centred);

        // Gate Threshold
        addAndMakeVisible(gateThresholdSlider);
        gateThresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        gateThresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "gate_threshold", gateThresholdSlider);

        addAndMakeVisible(gateLabel);
        gateLabel.setText("Gate Thresh", juce::dontSendNotification);
        gateLabel.setJustificationType(juce::Justification::centred);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        auto topHalf = area.removeFromTop(area.getHeight() / 2);

        inputGainLabel.setBounds(topHalf.removeFromTop(20));
        inputGainSlider.setBounds(topHalf);

        gateLabel.setBounds(area.removeFromTop(20));
        gateThresholdSlider.setBounds(area);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 1.0f);

        g.setFont(18.0f);
        g.drawText("INPUT", getLocalBounds().removeFromTop(20), juce::Justification::centred, true);
    }

private:
    RonnieAudioProcessor& audioProcessor;

    juce::Slider inputGainSlider;
    juce::Label inputGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;

    juce::Slider gateThresholdSlider;
    juce::Label gateLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttachment;
};

}
