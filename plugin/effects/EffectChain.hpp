#ifndef EFFECT_CHAIN_HPP
#define EFFECT_CHAIN_HPP

#include <array>
#include "Effect.hpp"

class EffectChain
{
    public:
        static constexpr int kMaxEffects = 8;

        void clear() noexcept 
        { 
            count = 0; 
        }

        bool addEffect (IEffect* effect) noexcept
        {
            if (effect == nullptr || count >= kMaxEffects) 
                return false;
            
            _effects[count++] = effect;
            return true;
        }

        void prepare (double sampleRate, int maxBlockSize, int numChannels)
        {
            for (int i = 0; i < count; ++i)
                _effects[i]->prepare (sampleRate, maxBlockSize, numChannels);
        }

        void reset()
        {
            for (int i = 0; i < count; ++i)
                _effects[i]->reset();
        }

        void process (juce::dsp::AudioBlock<float> block) noexcept
        {
            for (int i = 0; i < count; ++i)
                _effects[i]->process (block);
        }

    private:
        std::array<IEffect*, kMaxEffects> _effects {};
        int count = 0;
};

#endif