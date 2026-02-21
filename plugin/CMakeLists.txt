set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(FX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/effects")


juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME HTEK
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE TSRA
    PLUGIN_CODE 4444
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "HTekDistortion"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${SRC_DIR}/PluginEditor.cpp
        ${SRC_DIR}/PluginProcessor.cpp
        ${SRC_DIR}/PluginEditor.hpp
        ${SRC_DIR}/PluginProcessor.hpp
)

# Plugin links to effects only (not Catch2)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        HTekEffects
        juce::juce_audio_utils
        juce::juce_gui_basics
        juce::juce_core
        juce::juce_audio_formats
        juce::juce_audio_basics
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
# Visual Studio filters to separate Plugin files from JUCE files.

# Plugin files
source_group(
    TREE ${SRC_DIR}
    PREFIX "Plugin Sources"
    FILES ${SRC_DIR}/PluginEditor.cpp ${SRC_DIR}/PluginProcessor.cpp
)

source_group(
    TREE ${SRC_DIR}
    PREFIX "Plugin Headers"
    FILES ${SRC_DIR}/PluginEditor.hpp ${SRC_DIR}/PluginProcessor.hpp
)

# JUCE files
if (DEFINED JUCE_SOURCE_DIR)
    source_group(
        TREE ${JUCE_SOURCE_DIR}
        PREFIX "JUCE"
    )
endif()