#pragma once

#include <JuceHeader.h>
#include "../RonnieAudioProcessor.h"

namespace ronnie
{

class PresetManager
{
public:
    PresetManager(RonnieAudioProcessor& processor) : audioProcessor(processor)
    {
        presetsDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("RonnieAmpModeler")
            .getChildFile("Presets");

        if (!presetsDirectory.exists())
            presetsDirectory.createDirectory();
    }

    void savePreset(const juce::String& presetName)
    {
        auto state = audioProcessor.getAPVTS().copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());

        // We can create a JSON object from the XML attributes for a cleaner format,
        // or just wrap the XML in JSON, or just use XML.
        // The spec asked for JSON.

        juce::DynamicObject* presetObj = new juce::DynamicObject();
        presetObj->setProperty("presetName", presetName);

        // We save the entire APVTS state as an XML string inside the JSON.
        // This is the most reliable way to restore the state exactly as it is.
        presetObj->setProperty("stateXML", xml->toString());

        juce::var json(presetObj);

        juce::File presetFile = presetsDirectory.getChildFile(presetName + ".json");
        juce::FileOutputStream stream(presetFile);

        if (stream.openedOk())
        {
            stream.setPosition(0);
            stream.truncate();
            juce::JSON::writeToStream(stream, json);
        }
    }

    void loadPreset(const juce::File& file)
    {
        juce::var json = juce::JSON::parse(file);
        if (json.hasProperty("stateXML"))
        {
            juce::String xmlString = json.getProperty("stateXML", "").toString();
            std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse(xmlString);

            if (xmlState != nullptr)
                audioProcessor.getAPVTS().replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }

    juce::File getPresetsDirectory() const { return presetsDirectory; }

private:
    RonnieAudioProcessor& audioProcessor;
    juce::File presetsDirectory;
};

} // namespace ronnie
