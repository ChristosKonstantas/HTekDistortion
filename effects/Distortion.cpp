#include "Distortion.hpp"

void HTekDistortionEffect::prepare (double sampleRate, int maxBlockSize, int numChannels)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) maxBlockSize;
    spec.numChannels = (juce::uint32) numChannels;

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

inline float HTekDistortionEffect::waveshape(float x, float threshold, float kneeFrac) noexcept
{
    const float t = juce::jlimit(0.05f, 1.0f, threshold);
    const float knee = juce::jlimit(0.0f, 0.5f, kneeFrac);

    const float a = t * (1.0f - knee); // a: start of knee region
    const float b = t * (1.0f + knee); // b: end of knee region
    
    const float sign = (x >= 0.0f) ? 1.0f : -1.0f;

    const float absX = x >= 0 ? x : -x;
    /* Region 1 (linear region)*/
    if (absX <= a)
        return x;

    /* Region 3 (hard-clip region) */
    if (absX >= b)
        return sign * t;
    
    /* Region 2 (knee region) */
    // Map ax in [a,b] to u in [0,1].
    const float u  = (absX - a) / (b - a);
    // C^2 quartic transition
    const float u2 = u * u;
    const float u3 = u2 * u;
    const float u4 = u3 * u;
    const float q = u - u3 + 0.5f * u4; //  q has no audio amplitude scale here

    // convert the normalized curved progress back to output amplitude.
    // |-> at start q = 0 => y = a
    // |-> at the end q = 0.5 => y = (a+b)/2 = t
    // |-> during the transition from start to end => quartic transition
    const float y = a + (b - a) * q; 
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
    const float y0 = waveshape (b, p.threshold, p.knee);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = block.getChannelPointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            const float dry = data[i];
            const float x   = dry * drive;

            float y = waveshape (x + b, p.threshold, p.knee) - y0; // DC-correction (subtract y0 = f(b)) -> y = f(x+b) - f(b) and if x = 0 then y = 0
            y *= outGain;

            data[i] = dry + (y - dry) * mix; // dry*(1−mix) + y*mix but with 3 operations than 4
        }
    }

    _postLPF.process (ctx);
}