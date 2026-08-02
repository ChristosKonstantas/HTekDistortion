#include "EffectChain.hpp"

void EffectChain::clear() noexcept
{
    _count = 0;
    _effects.fill(nullptr);
}

bool EffectChain::addEffect (IEffect* effect) noexcept
{
    jassert(effect != nullptr);
    jassert(_count < numMaxEffects);

    if (effect == nullptr || _count >= numMaxEffects)
        return false;
    
    _effects[_count++] = effect;
    return true;
}

void EffectChain::prepare (double sampleRate, int maxBlockSize, int numChannels)
{
    jassert(_count > 0);

    for (int i = 0; i < _count; ++i)
        _effects[i]->prepare (sampleRate, maxBlockSize, numChannels);
}

void EffectChain::reset()
{
    jassert(!_effects.empty());

    for (int i = 0; i < _count; ++i)
        _effects[i]->reset();
}

void EffectChain::process (juce::dsp::AudioBlock<float>& block) noexcept
{
    jassert(!_effects.empty());

    for (int i = 0; i < _count; ++i)
        _effects[i]->process (block);
}