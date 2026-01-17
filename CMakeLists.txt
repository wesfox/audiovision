cmake_minimum_required(VERSION 3.15)
include(FetchContent)

##########################
## Adding librairies with FetchContent_Declare

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
)

FetchContent_MakeAvailable(spdlog)

## FetchContent
########################

set (TARGET_NAME AudioVision)

if (DEFINED PROJECT_NAME)
    message ("Adding ${TARGET_NAME} to the ${PROJECT_NAME} project")
else()
    file(READ VERSION.md CURRENT_VERSION)
    project(${TARGET_NAME} VERSION ${CURRENT_VERSION})
    add_subdirectory(../modules/juce ./cmake_build_juce)
endif()

# If we are using MSVC we want static runtime linkage
if (MSVC)
    set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# If we are compiling for macOS we want to target OS versions down to 10.13
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message ("-- building for macOS")
    set (CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE INTERNAL "")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "17.0" CACHE INTERNAL "")
    set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Apple Development" CACHE INTERNAL "")
endif()

# Adds all the module sources so they appear correctly in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

if (DEFINED ENV{VST2_SDK_DIR})
    MESSAGE(STATUS "Building with VST2 SDK: $ENV{VST2_SDK_DIR}")
    juce_set_vst2_sdk_path($ENV{VST2_SDK_DIR})
else()
    MESSAGE(STATUS "Not building with VST2")
endif()

juce_add_gui_app(${TARGET_NAME}
        BUNDLE_ID "com.juce.AudioVision"
        MICROPHONE_PERMISSION_ENABLED TRUE
        MICROPHONE_PERMISSION_TEXT "Audio recording"
        REQUIRES_FULL_SCREEN TRUE
        STATUS_BAR_HIDDEN TRUE
        PLUGINHOST_AU TRUE)

juce_generate_juce_header(${TARGET_NAME})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

set_target_properties(${TARGET_NAME} PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden)

### Target include directories
target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

file(GLOB_RECURSE COMMON_SOURCES "Source/AudioEngine/*.cpp" "Source/Gui/Components/*.cpp" "Source/Utils/*.cpp" "Source/Core/*.cpp" "Source/Gui/*.cpp")
set(SourceFiles
        Source/Main.cpp
        Source/Gui/MainComponent.cpp
        ${COMMON_SOURCES})

target_sources(${TARGET_NAME} PRIVATE ${SourceFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. PREFIX Source FILES ${SourceFiles})

# Enable testing
enable_testing()

# Add JUCE Unit Tests
juce_add_console_app(AudioVisionTests
        PRODUCT_NAME "AudioVisionTests")

juce_generate_juce_header(AudioVisionTests)

target_compile_features(AudioVisionTests PRIVATE cxx_std_20)

target_include_directories(AudioVisionTests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Source)

file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
target_sources(AudioVisionTests PRIVATE ${TEST_SOURCES} ${COMMON_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. PREFIX Tests FILES ${TEST_SOURCES})

target_link_libraries(AudioVisionTests PRIVATE
        spdlog::spdlog
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics)

# Set target properties for macOS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_compile_options(AudioVisionTests PRIVATE "-fno-aligned-allocation")
endif()

add_test(NAME AudioVisionTests COMMAND AudioVisionTests)

# addModuleSourceTarget()

if (DEFINED ENV{VST2_SDK_DIR})
    target_compile_definitions(${TARGET_NAME} PRIVATE
            JUCE_PLUGINHOST_VST=1)
endif()


target_compile_definitions(${TARGET_NAME} PRIVATE
        JUCE_PLUGINHOST_AU=1
        JUCE_PLUGINHOST_LADSPA=1
        JUCE_PLUGINHOST_VST3=1
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_STRICT_REFCOUNTEDPOINTER=1)

target_link_libraries(${TARGET_NAME} PRIVATE
        spdlog::spdlog
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_processors_headless
        juce::juce_audio_utils
        juce::juce_box2d
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_javascript
        juce::juce_opengl
        juce::juce_video)
#${TARGET_NAME}BinaryData)

# Compile with werror in release builds
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:-Werror>")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${TARGET_NAME} PRIVATE "/W4")
    target_compile_options(${TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:/WX>")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_compile_options(${TARGET_NAME} PRIVATE "-fno-aligned-allocation")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${TARGET_NAME} PRIVATE "-latomic")

    if (NOT ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64")
        target_link_options(${TARGET_NAME} PRIVATE "-m64")
    endif()
endif()