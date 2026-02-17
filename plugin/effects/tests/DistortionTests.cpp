#include <catch2/catch_test_macros.hpp>

#include "../effects/Distortion.hpp"

TEST_CASE("Distortion basic sanity")
{
    HTekDistortionEffect distortion;
    distortion.prepare(48000.0f, 512, 2);
    REQUIRE(true);
}