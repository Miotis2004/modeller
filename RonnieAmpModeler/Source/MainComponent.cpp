#include "MainComponent.h"

//==============================================================================

AmpModelerComponent::AmpModelerComponent()
{
    setSize(800, 600);

    // Request mic permission where needed and open audio channels
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [this](bool granted)
            {
                setAudioChannels(granted ? 2 : 0, 2);
            });
    }
    else
    {
        setAudioChannels(2, 2); // 2 inputs, 2 outputs
    }

    // === UI SETUP ===
    inputGainSlider.setRange(0.0, 2.0, 0.01);
    inputGainSlider.setValue(1.0);
    inputGainSlider.setSliderStyle(juce::Slider::Rotary);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(inputGainSlider);

    inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    inputGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inputGainLabel);

    inputGainSlider.onValueChange = [this]
        {
            engine.setInputGain((float)inputGainSlider.getValue());
        };
}

AmpModelerComponent::~AmpModelerComponent()
{
    shutdownAudio();
}

//==============================================================================

void AmpModelerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    engine.prepare(sampleRate, samplesPerBlockExpected);
}

void AmpModelerComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* buffer = bufferToFill.buffer;
    auto  numChannels = buffer->getNumChannels();
    auto  numSamples = bufferToFill.numSamples;

    // We will support up to 2 channels for now
    const float* inputPointers[2]{ nullptr, nullptr };
    float* outputPointers[2]{ nullptr, nullptr };

    for (int ch = 0; ch < numChannels && ch < 2; ++ch)
    {
        inputPointers[ch] = buffer->getReadPointer(ch, bufferToFill.startSample);
        outputPointers[ch] = buffer->getWritePointer(ch, bufferToFill.startSample);
    }

    engine.process(inputPointers, outputPointers, numSamples, numChannels);
}

void AmpModelerComponent::releaseResources()
{
    // Called when the audio device stops or is restarted
}

//==============================================================================

void AmpModelerComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Ronnie Amp Modeler - Test UI",
        getLocalBounds().removeFromTop(40),
        juce::Justification::centred,
        1);
}

void AmpModelerComponent::resized()
{
    auto area = getLocalBounds();
    auto bottom = area.removeFromBottom(200);
    auto sliderArea = bottom.reduced(20);

    inputGainSlider.setBounds(sliderArea.removeFromLeft(150));
    inputGainLabel.setBounds(inputGainSlider.getX(),
        inputGainSlider.getBottom(),
        inputGainSlider.getWidth(),
        20);
}
