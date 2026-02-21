#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "effects/Distortion.hpp"

TEST_CASE("Waveshaper: Odd function") 
{ 
    float t = 0.8; float k = 0.2; 
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) 
    { 
        float y1 = HTekDistortionEffect::waveshapeTest(x, t, k); 
        float y2 = HTekDistortionEffect::waveshapeTest(-x, t, k); 
        REQUIRE(y1 + y2 == Catch::Approx(0.f).epsilon(1e-8f));
    } 
    
    t = 0.3; k = 0.2; 
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) 
    { 
        float y1 = HTekDistortionEffect::waveshapeTest(x, t, k); 
        float y2 = HTekDistortionEffect::waveshapeTest(-x, t, k); 
        REQUIRE(y1 + y2 == Catch::Approx(0.f).epsilon(1e-8f));
    } 
}

TEST_CASE("Waveshaper: Region 1 / Linear region is identity")
{
    const float t = 0.5f;
    const float knee = 0.2f;
    const float a = t * (1.0f - knee); // = 0.4

    // Check linear region [-a, +a]
    const float step = 0.01f;

    for (float x = -a; x <= a; x += step)
    {
        float y = HTekDistortionEffect::waveshapeTest(x, t, knee);

        REQUIRE(y == Catch::Approx(x).epsilon(1e-8f));
    }

    float y = HTekDistortionEffect::waveshapeTest(a + 1e-2, t, knee);
    REQUIRE(y > a);   // knee starts to bend towards t
}

TEST_CASE("Waveshaper: Region 2 / Hard clipping")
{
    const float t = 0.6f;
    const float knee = 0.3f;
    const float b = t * (1.0f + knee); // = 0.78

    const float step = 0.01f;

    // [b to 1.0f]
    for (float x = b; x <= 1.0f; x += step)
    {
        float y = HTekDistortionEffect::waveshapeTest(x, t, knee);
        REQUIRE(y == Catch::Approx(t).epsilon(1e-8f));
    }

    // [-1.0f to b]
    for (float x = -b; x >= -1.0f; x -= step)
    {
        float y = HTekDistortionEffect::waveshapeTest(x, t, knee);
        REQUIRE(y == Catch::Approx(-t).epsilon(1e-8f));
    }
}

TEST_CASE("Waveshaper: Region 3 (knee) properties")
{
    const float t    = 0.5f;
    const float knee = 0.2f;

    const float a = t * (1.0f - knee);
    const float b = t * (1.0f + knee);

    // re-check region boundaries (done already in Region 1 and Region 2 tests)
    REQUIRE(HTekDistortionEffect::waveshapeTest( a, t, knee) == Catch::Approx( a).margin( 1e-6f));
    REQUIRE(HTekDistortionEffect::waveshapeTest(-a, t, knee) == Catch::Approx(-a).margin( 1e-6f));
    REQUIRE(HTekDistortionEffect::waveshapeTest( b, t, knee) == Catch::Approx( t).margin( 1e-6f));
    REQUIRE(HTekDistortionEffect::waveshapeTest(-b, t, knee) == Catch::Approx(-t).margin( 1e-6f));

    const int N = 1000;
    float prevAbsY = 0.0f;

    for (int i = 1; i < N; ++i)
    {
        // absX in range (a, b)
        const float absX = a + (b - a) * (static_cast<float>(i) / static_cast<float>(N)); // (a, b) <= i=1 to i = N-1

        for (float x : {absX, -absX})
        {
            const float y   = HTekDistortionEffect::waveshapeTest(x, t, knee);
            const float absY = y >= 0 ? y : -y;

            // sign preserved
            REQUIRE((y >= 0.0f) == (x >= 0.0f));

            // |y| in range  (|x|, t)
            // as |y| is a weighted average between |x| and t -> y = (1-s) * |x| + s * t
            const float lower = std::min(absX, t);
            const float upper = std::max(absX, t);
            REQUIRE(absY >= lower - 1e-6f);
            REQUIRE(absY <= upper + 1e-6f);
        }
    }
}

TEST_CASE("Distortion: mix=0 ignores distortion parameters (filters only)")
{
    const double fs = 48000.0;
    const int N = 512;

    juce::AudioBuffer<float> input(1, N);
    for (int i = 0; i < N; ++i)
        input.setSample(0, i, 0.8f * std::sin(6.28f * 440.0f * i / fs));

    // create buffers to process
    juce::AudioBuffer<float> buf1(input);
    juce::AudioBuffer<float> buf2(input);

    HTekDistortionEffect distortion1;
    distortion1.prepare(fs, N, 1);

    HTekDistortionEffect::Params p1;
    p1.mix = 0.0f; // !!
    p1.driveDb = 0.0f;
    p1.threshold = 0.9f;
    p1.knee = 0.1f;
    p1.bias = 0.0f;
    p1.outputDb = 0.0f;
    p1.preHPFHz = 200.0f;
    p1.postLPFHz = 4000.0f;
    distortion1.setParams(p1);

    // change distortion settings for distortion2 (mix=0 now see p1)
    HTekDistortionEffect distortion2;
    distortion2.prepare(fs, N, 1);

    HTekDistortionEffect::Params p2 = p1;
    p2.driveDb = 60.0f;
    p2.threshold = 0.1f;
    p2.knee = 0.5f;
    p2.bias = 0.5f;
    p2.outputDb = 18.0f;
    distortion2.setParams(p2);

    juce::dsp::AudioBlock<float> block1(buf1);
    juce::dsp::AudioBlock<float> block2(buf2);
    distortion1.process(block1);
    distortion2.process(block2);

    for (int i = 0; i < N; ++i)
    {
        REQUIRE(buf1.getSample(0, i) == Catch::Approx(buf2.getSample(0, i)).margin(1e-6f));
    }
}

TEST_CASE("Distortion process: silence in gives silence out")
{
    const int bufSize = 256;
    HTekDistortionEffect distortion;
    distortion.prepare(48000.0, bufSize, 1);

    HTekDistortionEffect::Params p;
    p.mix = 1.0f;
    p.driveDb = 30.0f;
    p.threshold = 0.2f;
    p.knee = 0.6f;
    p.bias = 0.3f;
    distortion.setParams(p);

    juce::AudioBuffer<float> buf1(1, bufSize);
    for (int i = 0; i < bufSize; ++i)
        buf1.setSample(0, i, 0.0f);

    juce::dsp::AudioBlock<float> block1(buf1);
    distortion.process(block1);

    for (int i = 0; i < bufSize; ++i)
        REQUIRE(block1.getSample(0, i) == Catch::Approx(0.0f).margin(1e-8f));
}