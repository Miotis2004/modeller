#include "RonnieAudioProcessor.h"

#if defined(JUCE_MODULE_AVAILABLE_juce_gui_basics)
#include "UI/MainLayout.h"

namespace ronnie
{

class RonnieAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit RonnieAudioProcessorEditor(RonnieAudioProcessor& owner)
        : juce::AudioProcessorEditor(&owner),
          processor(owner),
          mainLayout(owner)
    {
        setSize(900, 600);
        addAndMakeVisible(mainLayout);
    }

    void resized() override
    {
        mainLayout.setBounds(getLocalBounds());
    }

private:
    RonnieAudioProcessor& processor;
    MainLayout mainLayout;
};

juce::AudioProcessorEditor* RonnieAudioProcessor::createEditor()
{
    return new RonnieAudioProcessorEditor(*this);
}

} // namespace ronnie
#endif
