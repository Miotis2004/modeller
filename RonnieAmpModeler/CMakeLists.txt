cmake_minimum_required(VERSION 3.15)

project(RonnieAmpModeler VERSION 0.0.1)

# Manually configure JUCE modules
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
set(JUCE_MODULES_PATH "${JUCE_DIR}/modules")

# Optional: Neural Amp Modeler Core integration (requires local checkout)
option(RONNIE_USE_NAM_CORE "Enable NeuralAmpModelerCore inference" OFF)
set(NAM_CORE_PATH "" CACHE PATH "Path to NeuralAmpModelerCore source")
set(EIGEN_PATH "" CACHE PATH "Path to Eigen include directory")

# Fetch JUCE if the submodule is missing
if (NOT EXISTS "${JUCE_MODULES_PATH}/juce_core/juce_core.cpp")
    include(FetchContent)
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.12
    )
    FetchContent_MakeAvailable(juce)
    set(JUCE_DIR "${juce_SOURCE_DIR}")
    set(JUCE_MODULES_PATH "${JUCE_DIR}/modules")
endif()

# ==============================================================================
# HEADLESS ENGINE LIBRARY (For Verification & CI)
# ==============================================================================
# This target builds ONLY the DSP logic and avoids GUI/Windowing dependencies.
# It is used to verify the core logic compiles on headless Linux environments.

add_library(RonnieAmpEngine STATIC
    Source/DSP/AmpModelerEngine.cpp
    Source/DSP/AmpModelerEngine.h
    Source/RonnieAudioProcessor.h # Logic only, no GUI code usage in header if careful
    # We include headers for layout but don't compile UI .cpp files here

    # JUCE Modules Sources (Subset for DSP)
    ${JUCE_MODULES_PATH}/juce_core/juce_core.cpp
    ${JUCE_MODULES_PATH}/juce_dsp/juce_dsp.cpp
    ${JUCE_MODULES_PATH}/juce_audio_basics/juce_audio_basics.cpp
    ${JUCE_MODULES_PATH}/juce_data_structures/juce_data_structures.cpp
    ${JUCE_MODULES_PATH}/juce_events/juce_events.cpp
)

target_include_directories(RonnieAmpEngine PRIVATE
    Source
    Source/DSP
    ${JUCE_MODULES_PATH}
)

if (RONNIE_USE_NAM_CORE)
    if (NOT EXISTS "${NAM_CORE_PATH}")
        message(FATAL_ERROR "RONNIE_USE_NAM_CORE is ON but NAM_CORE_PATH is not set or invalid.")
    endif()
    if (NOT EXISTS "${EIGEN_PATH}")
        message(FATAL_ERROR "RONNIE_USE_NAM_CORE is ON but EIGEN_PATH is not set or invalid.")
    endif()

    file(GLOB_RECURSE NAM_CORE_SOURCES
        "${NAM_CORE_PATH}/src/*.cpp"
        "${NAM_CORE_PATH}/src/*.c"
    )
    if (NAM_CORE_SOURCES)
        target_sources(RonnieAmpEngine PRIVATE ${NAM_CORE_SOURCES})
    endif()

    target_include_directories(RonnieAmpEngine PRIVATE
        "${NAM_CORE_PATH}/include"
        "${NAM_CORE_PATH}/src"
        "${EIGEN_PATH}"
    )

    target_compile_definitions(RonnieAmpEngine PRIVATE RONNIE_USE_NAM_CORE=1)
endif()

target_compile_definitions(RonnieAmpEngine PRIVATE
    JUCE_APP_VERSION=0.0.1
    JUCE_APP_VERSION_HEX=0x0001
    JUCE_STANDALONE_APPLICATION=0
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_WebBrowser=0
    JUCE_USE_CURL=0
    _CONSOLE=1
)

if(UNIX AND NOT APPLE)
    target_link_libraries(RonnieAmpEngine PRIVATE pthread dl)
endif()

target_compile_features(RonnieAmpEngine PRIVATE cxx_std_17)

# ==============================================================================
# FULL GUI APPLICATION (Target for Windows/Mac/Desktop Linux)
# ==============================================================================
# This is the standard JUCE application target.
# It requires system libraries (X11 on Linux) which might not be present in all envs.

if (NOT DEFINED HEADLESS_ONLY)
    # We define the source list for the full app
    set(RONNIE_APP_SOURCES
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h
        Source/RonnieAudioProcessor.cpp
        Source/RonnieAudioProcessor.h
        Source/DSP/AmpModelerEngine.cpp
        Source/DSP/AmpModelerEngine.h
        Source/PresetManager.h
        Source/UI/MainLayout.h
        Source/UI/InputPanel.h
        Source/UI/AmpPanel.h
        Source/UI/CabOutputPanel.h
    )

    # We use a custom target instead of juce_add_gui_app to manually control sources
    # if we wanted, but since we have the JUCE repo, we can try to use the modules directly.
    # However, to avoid 'juceaide' issues on Linux, we will just DEFINE what the
    # build WOULD look like for the user.

    # NOTE: To build this on Windows, the user would run cmake .
    # We will add a target that only builds if we have the GUI deps or on Windows.

    if (WIN32 OR APPLE)
        add_subdirectory(${JUCE_DIR} JUCE) # This brings in juce_add_gui_app

        juce_add_gui_app(RonnieAmpModeler
            PRODUCT_NAME "Ronnie Amp Modeler"
            VERSION "0.0.1"
        )

        target_sources(RonnieAmpModeler PRIVATE ${RONNIE_APP_SOURCES})

        target_link_libraries(RonnieAmpModeler PRIVATE
            juce::juce_audio_utils
            juce::juce_audio_devices
            juce::juce_audio_formats
            juce::juce_audio_processors
            juce::juce_core
            juce::juce_data_structures
            juce::juce_events
            juce::juce_graphics
            juce::juce_gui_basics
            juce::juce_gui_extra
            juce::juce_dsp
        )

        if (RONNIE_USE_NAM_CORE)
            target_sources(RonnieAmpModeler PRIVATE ${NAM_CORE_SOURCES})
            target_include_directories(RonnieAmpModeler PRIVATE
                "${NAM_CORE_PATH}/include"
                "${NAM_CORE_PATH}/src"
                "${EIGEN_PATH}"
            )
            target_compile_definitions(RonnieAmpModeler PRIVATE RONNIE_USE_NAM_CORE=1)
        endif()
    endif()
endif()
