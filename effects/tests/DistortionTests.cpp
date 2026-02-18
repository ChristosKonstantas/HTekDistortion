#include <catch2/catch_test_macros.hpp>

#include "effects/Distortion.hpp"

bool approxEqual(float a, float b, float eps) noexcept 
{ 
    return std::fabs(a - b) <= eps; 
}

TEST_CASE("Waveshaper: Odd function") 
{ 
    float t = 0.8; float k = 0.2; 
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) 
    { 
        float y1 = HTekDistortionEffect::waveshapeTest(x, t, k); 
        float y2 = HTekDistortionEffect::waveshapeTest(-x, t, k); 
        REQUIRE(approxEqual(y1 + y2, 0.0f, 1e-8f)); 
    } 
    
    t = 0.3; k = 0.2; 
    for (float x = -1.0f; x <= 1.0f; x += 0.01f) 
    { 
        float y1 = HTekDistortionEffect::waveshapeTest(x, t, k); 
        float y2 = HTekDistortionEffect::waveshapeTest(-x, t, k); 
        REQUIRE(approxEqual(y1 + y2, 0.0f, 1e-8f)); 
    } 
}