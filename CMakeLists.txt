cmake_minimum_required(VERSION 3.22)

project(MicrosoundSymphony VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MICROSOUND_FETCH_JUCE "Fetch JUCE from GitHub when JUCE CMake package is unavailable" ON)
set(MICROSOUND_JUCE_SOURCE_DIR "" CACHE PATH "Path to a local JUCE source tree (contains JUCE/CMakeLists.txt)")

if (MICROSOUND_JUCE_SOURCE_DIR AND EXISTS "${MICROSOUND_JUCE_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using JUCE source from MICROSOUND_JUCE_SOURCE_DIR: ${MICROSOUND_JUCE_SOURCE_DIR}")
    add_subdirectory("${MICROSOUND_JUCE_SOURCE_DIR}" JUCE)
elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    message(STATUS "Using JUCE source from project root: ${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/JUCE" JUCE)
elseif (EXISTS "$ENV{JUCE_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using JUCE source from JUCE_SOURCE_DIR: $ENV{JUCE_SOURCE_DIR}")
    add_subdirectory("$ENV{JUCE_SOURCE_DIR}" JUCE)
elseif (EXISTS "/Users/md/JUCE/CMakeLists.txt")
    message(STATUS "Using JUCE source from default local path: /Users/md/JUCE")
    add_subdirectory("/Users/md/JUCE" JUCE)
elseif (MICROSOUND_FETCH_JUCE)
    include(FetchContent)
    FetchContent_Declare(
        juce
        URL https://github.com/juce-framework/JUCE/archive/refs/tags/8.0.4.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(juce)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

juce_add_plugin(MicrosoundSymphony
    COMPANY_NAME "MicrosoundLabs"
    BUNDLE_ID "com.microsoundlabs.microsoundsymphony"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE MdMS
    PLUGIN_CODE McSy
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "unfoldings"
)

juce_generate_juce_header(MicrosoundSymphony)

target_sources(MicrosoundSymphony
    PRIVATE
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
)

target_compile_definitions(MicrosoundSymphony
    PRIVATE
        JUCE_FORCE_USE_LEGACY_PARAM_IDS=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(MicrosoundSymphony
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
