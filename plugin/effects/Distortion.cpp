#include "Distortion.hpp"

void HTekDistortionEffect::prepare (double sampleRate, int /*maxBlockSize*/, int numChannels)
{
    _sr = sampleRate;
    _channels = juce::jmax (1, numChannels);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = _sr;
    spec.maximumBlockSize = 2048; // safe upper bound; actual is wrapped by AudioBlock
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

inline float HTekDistortionEffect::dbToLin(float db) noexcept
{
    return std::pow(10.0f, db / 20.0f);
}

void HTekDistortionEffect::reset()
{
    _preHPF.reset();
    _postLPF.reset();
}

inline float HTekDistortionEffect::waveshape(float x, float threshold, float kneeFrac) noexcept
{
    const float t = juce::jlimit(0.01f, 1.0f, threshold);
    const float knee = juce::jlimit(0.0f, 0.8f, kneeFrac);

    const float a = t * (1.0f - knee); // a: start of knee region
    const float b = t * (1.0f + knee); // b: end of knee region

    const float ax = std::abs(x);
    if (ax <= a) 
        return x;

    const float sign = (x >= 0.0f) ? 1.0f : -1.0f;
    if (ax >= b) 
        return sign * t;
    
    //else

    // Map ax in [a,b] to u in [0,1], then smoothstep to interpolate to hard clip.
    const float u = (ax - a) / (b - a);
    const float s = u * u * (3.0f - 2.0f * u); // smoothstep
    const float y = (1.0f - s) * ax + s * t;
    return sign * y;
}

void HTekDistortionEffect::process (juce::dsp::AudioBlock<float> block) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh   = (int) block.getNumChannels();
    const int numSamp = (int) block.getNumSamples();
    if (numCh == 0 || numSamp == 0) return;

    const Params p = _params;

    const float drive   = dbToLin (p.driveDb);
    const float outGain = dbToLin (p.outputDb);
    const float mix     = juce::jlimit (0.0f, 1.0f, p.mix);

    _preHPF.setCutoffFrequency  (juce::jlimit (20.0f, 20000.0f, p.preHPFHz));
    _postLPF.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, p.postLPFHz));

    juce::dsp::ProcessContextReplacing<float> ctx (block);

    _preHPF.process (ctx);

    const float b  = juce::jlimit (-0.5f, 0.5f, p.bias);
    const float y0 = waveshape (b, p.threshold, p.knee);

    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = block.getChannelPointer ((size_t) ch);

        for (int i = 0; i < numSamp; ++i)
        {
            const float dry = data[i];
            const float x   = dry * drive;

            float y = waveshape (x + b, p.threshold, p.knee) - y0;
            y *= outGain;

            data[i] = dry + (y - dry) * mix; // dry*(1−mix) + y*mix but with 3 operations than 4
        }
    }

    _postLPF.process (ctx);
}