#pragma once
class SignalChain
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(float** input, float** output, int numSamples, int numChannels);

private:
    // Later: NoiseGate gate;
    //        Drive pedal;
    //        NamAmp amp;
    //        CabIR cab;
    //        EQ eq;
};


