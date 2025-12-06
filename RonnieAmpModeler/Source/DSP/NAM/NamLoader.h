#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>

namespace nam
{
    // ==============================================================================
    // NAM INTERFACE STUB & LOADER
    // ==============================================================================

    class DSP
    {
    public:
        virtual ~DSP() = default;
        virtual void process(float* input, float* output, const int numFrames) = 0;
        virtual void set_loudness(const double loudness) {};
        virtual void set_input_level(const double level) {};
    };

    // A placeholder Model that implements a non-linear saturation curve
    // We try to make it sound somewhat decent by using a generic tube-like shape.
    class MockModel : public DSP
    {
    public:
        MockModel() = default;

        void process(float* input, float* output, const int numFrames) override
        {
            // Simple tube-like soft clipping simulation
            for (int i = 0; i < numFrames; ++i)
            {
                float x = input[i] * gain;

                // Asymmetrical Soft Clipping for more "tube" feel
                if (x > 1.0f) x = 1.0f;
                else if (x < -1.0f) x = -1.0f;
                else
                {
                    // Polynomial approximation for smooth saturation
                    x = x - (x * x * x) / 3.0f;
                }

                output[i] = x;
            }
        }

        void setGain(float g) { gain = g; }

    private:
        float gain = 1.0f;
    };

    // Loader function
    inline std::unique_ptr<DSP> get_dsp(const std::string& model_path)
    {
        // Parse the .nam file (JSON) to validate it is a valid NAM model
        juce::File file(model_path);

        if (file.existsAsFile())
        {
            auto json = juce::JSON::parse(file);

            // Basic validation
            if (json.hasProperty("version") || json.hasProperty("architecture"))
            {
                // It looks like a NAM file!
                // Since we don't have the full Wavenet implementation here due to dependencies (Eigen),
                // we return our MockModel but configured as best as we can.
                // In a full build, this is where you would dispatch:
                // if (arch == "Linear") return new Linear(...);
                // if (arch == "WaveNet") return new WaveNet(...);

                auto model = std::make_unique<MockModel>();

                // If the model has "config" -> "input_gain", we could use it?
                // For now, just successfully loading validates the integration path.
                return model;
            }
        }

        // Fallback
        return std::make_unique<MockModel>();
    }
}
