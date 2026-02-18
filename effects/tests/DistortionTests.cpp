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
            const float lower = std::min(absX, t);
            const float upper = std::max(absX, t);
            REQUIRE(absY >= lower - 1e-6f);
            REQUIRE(absY <= upper + 1e-6f);
        }
    }
}