cmake_minimum_required(VERSION 3.25)

project(CSPiky64 VERSION 0.33.0 LANGUAGES C CXX)

set(JUCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../_Tools/JUCE/_8.0.15")

if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE 8.0.15 was not found. Expected: ${JUCE_DIR}/CMakeLists.txt")
endif()

add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")

juce_add_plugin(CSPiky64
    VERSION 0.33.0
    COMPANY_NAME "SynthMaker"
    BUNDLE_ID "com.synthmaker.cspiky64"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_AUTO_MANIFEST FALSE
    PLUGIN_MANUFACTURER_CODE SyMk
    PLUGIN_CODE Pk64
    FORMATS VST3
    VST3_CATEGORIES Instrument Synth
    PRODUCT_NAME "CSPiky64")

juce_generate_juce_header(CSPiky64)

target_sources(CSPiky64
    PRIVATE
        PluginEditor.cpp
        PluginEditor.h
        PluginProcessor.cpp
        PluginProcessor.h
        Wave2Tables.h)

target_compile_features(CSPiky64 PRIVATE cxx_std_17)

target_compile_definitions(CSPiky64
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(CSPiky64
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
