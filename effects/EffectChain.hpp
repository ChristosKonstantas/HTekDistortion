#ifndef EFFECT_CHAIN_HPP
#define EFFECT_CHAIN_HPP

#include <array>
#include "Effect.hpp"

class EffectChain
{
    public:
        static constexpr int numMaxEffects = 8;

        void clear() noexcept;

        bool addEffect(IEffect *effect) noexcept;

        void prepare(double sampleRate, int maxBlockSize, int numChannels);

        void reset();

        void process(juce::dsp::AudioBlock<float> &block) noexcept;

    private:
        std::array<IEffect*, numMaxEffects> _effects {};
        int _count = 0;
};

#endif