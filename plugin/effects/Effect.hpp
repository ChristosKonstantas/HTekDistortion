#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <juce_audio_basics/juce_audio_basics.h>

struct EffectParamsBase
{
    virtual ~EffectParamsBase() = default;
};

class IEffect
{
    public:
        virtual ~IEffect() = default;

        virtual void prepare (double sampleRate, int maxBlockSize, int numChannels) = 0;
        virtual void reset() = 0;

        // Real-time safe
        virtual void process (juce::AudioBuffer<float>& buffer) noexcept = 0;
};

#endif