cmake_minimum_required(VERSION 3.22)

set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(HTekEffectsTests
    DistortionTests.cpp
    # EffectChainTests.cpp
)

target_link_libraries(HTekEffectsTests PRIVATE HTekEffects Catch2::Catch2WithMain)

target_compile_definitions(HTekEffectsTests PRIVATE ACTIVE_TEST=1)

source_group(
    TREE ${TEST_DIR}
    PREFIX "Tests"
    FILES ${TEST_DIR}/DistortionTests.cpp
)

include(Catch)
catch_discover_tests(HTekEffectsTests)