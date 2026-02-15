#pragma once
#include "Effect.hpp"
#include <juce_dsp/juce_dsp.h>

class HTekDistortionEffect final : public IEffect
{
    public:
        struct Params
        {
            float driveDb = 18.0f;      // pre-gain
            float preHPFHz = 80.0f;     // pre HPF
            float threshold = 0.7f;     // clip point
            float knee = 0.12f;         // 0..0.4 soft knee width (fraction of threshold)
            float bias = 0.08f;         // asymmetry
            float postLPFHz = 12000.0f; // post LPF
            float mix = 1.0f;           // 0..1
            float outputDb = -1.0f;     // output gain
        };

        HTekDistortionEffect() = default;

        void prepare (double sampleRate, int maxBlockSize, int numChannels) override;
        void reset() override;
        void process (juce::dsp::AudioBlock<float> block) noexcept override;

        // Real time safe: called from audio thread with a local snapshot
        void setParams (const Params& p) noexcept { _params = p; }

    protected:
        /* Smooth hard clip with soft-knee transition*/
        // - bias adds asymmetry (even harmonics)
        // - knee softens around threshold
        inline float waveshape(float x, float threshold, float kneeFrac) noexcept;

    private:
        inline float dbToLin(float db) noexcept;

        float _sr = 48000.0f;
        int   _channels = 2;

        Params _params {};
        juce::dsp::StateVariableTPTFilter<float> _preHPF;
        juce::dsp::StateVariableTPTFilter<float> _postLPF;
};
