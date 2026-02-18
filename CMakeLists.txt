cmake_minimum_required(VERSION 3.22)

project(HTekDistortion LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If (JUCE_LIB EXISTS)     # JUCE_LIB must point to the JUCE source root (the one containing CMakeLists.txt and modules/)
# Setup JUCE first via environment variable JUCE_LIB
# Add JUCE as a subdirectory and keep its build artifacts out of the main tree

if(DEFINED ENV{JUCE_LIB} AND NOT EXISTS "$ENV{JUCE_LIB}/modules")
  message(FATAL_ERROR
    "JUCE_LIB is set to '$ENV{JUCE_LIB}', but that does not look like the JUCE source root (missing 'modules/' folder)."
  )
endif()

if(DEFINED ENV{JUCE_LIB} AND EXISTS "$ENV{JUCE_LIB}/CMakeLists.txt")

  message(STATUS "JUCE: using local checkout at $ENV{JUCE_LIB}")
  set(JUCE_LOCAL_SOURCE "$ENV{JUCE_LIB}")
  add_subdirectory("${JUCE_LOCAL_SOURCE}" "${CMAKE_BINARY_DIR}/_deps/juce-build" EXCLUDE_FROM_ALL)

else()
  message(STATUS "JUCE: fetching via FetchContent")

  include(FetchContent)
  FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.11
  )
  # By default, FetchContent uses ${CMAKE_BINARY_DIR}/_deps for sources/builds.
  FetchContent_MakeAvailable(JUCE)
endif()

# Testing -> add -DBUILD_TESTS=ON
# Catch2 (env var override otherwise FetchContent)
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)

  enable_testing()
  include(CTest)

  message(STATUS "BUILD_TESTS is ON. Configuring Catch2...")

  if(DEFINED ENV{CATCH2_LIB} AND NOT EXISTS "$ENV{CATCH2_LIB}/CMakeLists.txt")
    message(FATAL_ERROR "CATCH2_LIB is set to '$ENV{CATCH2_LIB}', but that does not look like the Catch2 source root.")
  endif()

  if(DEFINED ENV{CATCH2_LIB} AND EXISTS "$ENV{CATCH2_LIB}/CMakeLists.txt")
    message(STATUS "Catch2: using local checkout at $ENV{CATCH2_LIB}")
    set(catch2_SOURCE_DIR "$ENV{CATCH2_LIB}")
    add_subdirectory("$ENV{CATCH2_LIB}" "${CMAKE_BINARY_DIR}/_deps/catch2-build" EXCLUDE_FROM_ALL)
  else()
    message(STATUS "Catch2: fetching via FetchContent")
    include(FetchContent)
    FetchContent_Declare(Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
  endif()

  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

endif()

add_subdirectory(effects)
add_subdirectory(plugin)