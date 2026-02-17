cmake_minimum_required(VERSION 3.22)

set(FX_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(HTekEffects STATIC
    ${FX_DIR}/Effect.hpp
    ${FX_DIR}/EffectChain.hpp
    ${FX_DIR}/EffectChain.cpp
    ${FX_DIR}/Distortion.hpp
    ${FX_DIR}/Distortion.cpp
)

target_include_directories(HTekEffects PUBLIC ${FX_DIR})

target_link_libraries(HTekEffects PUBLIC
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
)

# Build tests only if BUILD_TESTS is enabled
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()