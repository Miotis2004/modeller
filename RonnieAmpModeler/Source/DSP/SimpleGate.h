#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace ronnie
{

class SimpleGate
{
public:
    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        updateCoeffs();
    }

    void setThreshold(float thresholdLinear)
    {
        this->threshold = thresholdLinear;
    }

    void setAttackRelease(float attackMs, float releaseMs)
    {
        this->attackMs = attackMs;
        this->releaseMs = releaseMs;
        updateCoeffs();
    }

    float processSample(float input)
    {
        float inputLevel = std::abs(input);

        // Envelope Follower
        if (inputLevel > envelope)
            envelope = attackCoeff * (envelope - inputLevel) + inputLevel;
        else
            envelope = releaseCoeff * (envelope - inputLevel) + inputLevel;

        // Gate Logic with Hysteresis
        if (envelope > threshold)
            isOpen = true;
        else if (envelope < threshold * 0.5f) // Hysteresis lower bound
            isOpen = false;

        // Smooth gain transition
        float targetGain = isOpen ? 1.0f : 0.0f;

        // Simple one-pole smoothing for the gain itself to avoid clicks
        // Use a fast attack, slower release for the gain transition
        if (targetGain > currentGain)
            currentGain = 0.0f * (currentGain - targetGain) + targetGain; // Instant open
        else
            currentGain = 0.9995f * (currentGain - targetGain) + targetGain; // Smooth close

        return input * currentGain;
    }

private:
    double sampleRate = 44100.0;
    float threshold = 0.0f;
    float envelope = 0.0f;
    float currentGain = 0.0f;
    bool isOpen = true;

    float attackMs = 2.0f;
    float releaseMs = 100.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;

    void updateCoeffs()
    {
        attackCoeff = std::exp(-1000.0f / (attackMs * sampleRate));
        releaseCoeff = std::exp(-1000.0f / (releaseMs * sampleRate));
    }
};

}
