cmake_minimum_required(VERSION 3.22)

project(Omniverse VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin sources
set(PLUGIN_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Utils/Parameters.cpp
    Source/Sampler/SampleSlot.cpp
    Source/Sampler/OmniverseVoice.cpp
    Source/Sampler/OmniverseSampler.cpp
    Source/UI/SlotPanel.cpp
    Source/UI/WaveformDisplay.cpp
    Source/UI/OmniverseLookAndFeel.cpp
    Source/UI/FiltersPanel.cpp
    Source/UI/EffectsPanel.cpp
)

# Plugin target
juce_add_plugin(Omniverse
    COMPANY_NAME "OmniverseAudio"
    BUNDLE_ID "com.omniverseaudio.omniverse"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Omni
    PLUGIN_CODE Omvs
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Omniverse"
)

target_sources(Omniverse PRIVATE ${PLUGIN_SOURCES})

target_include_directories(Omniverse PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_compile_definitions(Omniverse PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Omniverse PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_graphics
    juce::juce_core
    juce::juce_events
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
