#include "MainComponent.h"

//==============================================================================

AmpModelerComponent::AmpModelerComponent()
    : mainLayout(processor)
{
    setSize(900, 600);

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

    addAndMakeVisible(mainLayout);

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Settings");
    settingsButton.onClick = [this] { openSettings(); };
}

AmpModelerComponent::~AmpModelerComponent()
{
    shutdownAudio();
}

void AmpModelerComponent::openSettings()
{
    juce::DialogWindow::LaunchOptions opt;
    opt.dialogTitle = "Audio Settings";
    opt.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    opt.content.setOwned(new juce::AudioDeviceSelectorComponent(deviceManager, 0, 2, 0, 2, false, false, true, false));
    opt.content->setSize(400, 600);
    opt.launchAsync();
}

//==============================================================================

void AmpModelerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    processor.prepareToPlay(sampleRate, samplesPerBlockExpected);
}

void AmpModelerComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::MidiBuffer midi; // Empty MIDI for now

    // Create a proxy buffer that points to the correct section of the main buffer
    juce::AudioBuffer<float> proxyBuffer(bufferToFill.buffer->getArrayOfWritePointers(),
                                         bufferToFill.buffer->getNumChannels(),
                                         bufferToFill.startSample,
                                         bufferToFill.numSamples);

    // Process the block through the engine
    processor.processBlock(proxyBuffer, midi);
}

void AmpModelerComponent::releaseResources()
{
    processor.releaseResources();
}

//==============================================================================

void AmpModelerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    // Header background
    g.setColour(juce::Colours::darkred.withAlpha(0.5f));
    g.fillRect(0, 0, getWidth(), 40);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Ronnie Amp Modeler",
        0, 0, getWidth(), 40,
        juce::Justification::centred,
        1);
}

void AmpModelerComponent::resized()
{
    auto area = getLocalBounds();
    auto topBar = area.removeFromTop(40); // Title bar

    settingsButton.setBounds(topBar.removeFromRight(100).reduced(5));

    mainLayout.setBounds(area);
}
