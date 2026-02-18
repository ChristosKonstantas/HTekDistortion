#include "Distortion.hpp"

void HTekDistortionEffect::prepare (double sampleRate, int maxBlockSize, int numChannels)
{
    _sr = sampleRate;
    _channels = juce::jmax (1, numChannels);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = _sr;
    spec.maximumBlockSize = maxBlockSize;
    spec.numChannels = (juce::uint32) _channels;

    _preHPF.reset();
    _postLPF.reset();

    _preHPF.prepare (spec);
    _postLPF.prepare (spec);

    _preHPF.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    _postLPF.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    _preHPF.setCutoffFrequency (juce::jlimit  (20.0f, 20000.0f, _params.preHPFHz));
    _postLPF.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, _params.postLPFHz));
}

void HTekDistortionEffect::reset()
{
    _preHPF.reset();
    _postLPF.reset();
}

inline float HTekDistortionEffect::_waveshape(float x, float threshold, float kneeFrac) noexcept
{
    const float t = juce::jlimit(0.01f, 1.0f, threshold);
    const float knee = juce::jlimit(0.0f, 0.8f, kneeFrac);

    const float a = t * (1.0f - knee); // a: start of knee region
    const float b = t * (1.0f + knee); // b: end of knee region

    const float absX = x >= 0 ? x : -x;
    // Region 1
    if (absX <= a) // normally assignment falls in Region 3 but since we'll get the same result with |x| = a we avoid the Region 3 extra calculations
        return x;

    // Region 2
    const float sign = (x >= 0.0f) ? 1.0f : -1.0f;
    if (absX >= b) // normally assignment falls in Region 3 but since we'll get the same result with |x| = a we avoid the Region 3 extra calculations
        return sign * t;
    
    // Region 3
    // Map ax in [a,b] to u in [0,1], then smoothstep to interpolate to hard clip.
    const float u = (absX - a) / (b - a);
    // const float s = u * u * (3.0f - 2.0f * u); // smoothstep
    const float s = u*u*u*(u*(u*6.0f - 15.0f) + 10.0f); // smootherstep
    const float y = (1.0f - s) * absX + s * t;
    return sign * y;
}

void HTekDistortionEffect::process (juce::dsp::AudioBlock<float>& block) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels   = (int) block.getNumChannels();
    const int numSamples = (int) block.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0) return;

    const Params p = _params;

    const float drive   = juce::Decibels::decibelsToGain(p.driveDb);
    const float outGain = juce::Decibels::decibelsToGain(p.outputDb);
    const float mix     = juce::jlimit (0.0f, 1.0f, p.mix);

    _preHPF.setCutoffFrequency  (juce::jlimit (20.0f, 20000.0f, p.preHPFHz));
    _postLPF.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, p.postLPFHz));

    juce::dsp::ProcessContextReplacing<float> ctx (block);

    _preHPF.process (ctx);

    const float b  = juce::jlimit (-0.5f, 0.5f, p.bias);
    const float y0 = _waveshape (b, p.threshold, p.knee);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = block.getChannelPointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            const float dry = data[i];
            const float x   = dry * drive;

            float y = _waveshape (x + b, p.threshold, p.knee) - y0; // DC-correction (subtract y0)
            y *= outGain;

            data[i] = dry + (y - dry) * mix; // dry*(1−mix) + y*mix but with 3 operations than 4
        }
    }

    _postLPF.process (ctx);
}