#ifndef EFFECT_CHAIN_HPP
#define EFFECT_CHAIN_HPP

#include <array>
#include "Effect.hpp"

class EffectChain
{
    public:
        static constexpr int numMaxEffects = 8;

        void clear() noexcept
        {
            _count = 0;
            _effects.fill(nullptr);
        }

        bool addEffect (IEffect* effect) noexcept
        {
            jassert(effect != nullptr);
            jassert(_count < numMaxEffects);

            if (effect == nullptr || _count >= numMaxEffects)
                return false;
            
            _effects[_count++] = effect;
            return true;
        }

        void prepare (double sampleRate, int maxBlockSize, int numChannels)
        {
            jassert(_count > 0);

            for (int i = 0; i < _count; ++i)
                _effects[i]->prepare (sampleRate, maxBlockSize, numChannels);
        }

        void reset()
        {
            jassert(!_effects.empty());

            for (int i = 0; i < _count; ++i)
                _effects[i]->reset();
        }

        void process (juce::dsp::AudioBlock<float>& block) noexcept
        {
            jassert(!_effects.empty());

            for (int i = 0; i < _count; ++i)
                _effects[i]->process (block);
        }

    private:
        std::array<IEffect*, numMaxEffects> _effects {};
        int _count = 0;
};

#endif