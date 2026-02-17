cmake_minimum_required(VERSION 3.22)

add_executable(HTekEffectsTests
    DistortionTests.cpp
    # EffectChainTests.cpp
)

target_link_libraries(HTekEffectsTests PRIVATE HTekEffects Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(HTekEffectsTests)