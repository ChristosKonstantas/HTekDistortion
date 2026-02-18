#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class IEffect
{
    public:
        virtual ~IEffect() = default;

        virtual void prepare (double sampleRate, int maxBlockSize, int numChannels) = 0;
        virtual void reset() = 0;

        // Should be real-time safe
        virtual void process (juce::dsp::AudioBlock<float>& block) noexcept = 0;
};

#endif