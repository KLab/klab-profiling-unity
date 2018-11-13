cmake_minimum_required(VERSION 3.10)


# Detect macOS/iOS
if (IOS_PLATFORM)
    set(IOS TRUE)
elseif (APPLE)
    set(MACOS TRUE)
endif ()


# Expose options
set(KLAB_PROFILING_UNITY_PLUGIN_API_PATH "" CACHE PATH   "[Required] Path to folder containing Unity native plugin headers")
set(KLAB_PROFILING_EXTERN_TRACE_TARGET   "" CACHE STRING "[Optional] Name of extern trace library CMake target")
set(KLAB_PROFILING_EXTERN_UTILS_TARGET   "" CACHE STRING "[Optional] Name of extern utility library CMake target")


# Validate options
if (EXISTS "${KLAB_PROFILING_UNITY_PLUGIN_API_PATH}/IUnityInterface.h")
    if (NOT EXISTS "${KLAB_PROFILING_UNITY_PLUGIN_API_PATH}/IUnityProfilerCallbacks.h")
        message(FATAL_ERROR "Unity 2018.2+ required")
    endif ()
else ()
    message(FATAL_ERROR "Path to Unity Plugin API folder required")
endif ()


# Assemble source files and settings
set(privateDefines "")
set(privateIncludes ${KLAB_PROFILING_UNITY_PLUGIN_API_PATH} Internal)
set(privateLinkLibraries "")
set(sourceFiles
    SourceFiles/ATrace.cpp
    SourceFiles/CSharpTrace.cpp
    SourceFiles/ExternTrace.cpp
    SourceFiles/Plugin.cpp
    SourceFiles/PluginContext.cpp
    SourceFiles/Utils.cpp)


if (TARGET ${KLAB_PROFILING_EXTERN_TRACE_TARGET})
    message(STATUS "Extern utils found")
    list(APPEND privateDefines       KLAB_PROFILING_HAS_EXTERN_UTILS=1)
    list(APPEND privateLinkLibraries ${KLAB_PROFILING_EXTERN_TRACE_TARGET})
endif ()

if (TARGET ${KLAB_PROFILING_EXTERN_UTILS_TARGET})
    message(STATUS "Extern trace found")
    list(APPEND privateDefines       KLAB_PROFILING_HAS_EXTERN_TRACE=1)
    list(APPEND privateLinkLibraries ${KLAB_PROFILING_EXTERN_UTILS_TARGET})
endif ()

if (ANDROID)
    message(STATUS "Android trace found")
    list(APPEND privateDefines KLAB_PROFILING_HAS_ATRACE=1)
endif ()

find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    message(STATUS "pthread found")
    list(APPEND privateDefines       KLAB_PROFILING_HAS_PTHREAD=1)
    list(APPEND privateLinkLibraries Threads::Threads)
endif()


# Create library
set(libraryType SHARED)
if (IOS)
    set(libraryType STATIC)
elseif (MACOS)
    set(libraryType MODULE)
endif ()

add_library(KLab_Profiling ${libraryType} ${sourceFiles})
target_compile_definitions(KLab_Profiling PRIVATE ${privateDefines})
target_link_libraries(KLab_Profiling PRIVATE ${privateLinkLibraries})
target_include_directories(KLab_Profiling PUBLIC Include PRIVATE ${privateIncludes})

if (KLAB_PROFILING_OUTPUT_DIRECTORY)
    set_target_properties(KLab_Profiling
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${KLAB_PROFILING_OUTPUT_DIRECTORY}"
        LIBRARY_OUTPUT_DIRECTORY "${KLAB_PROFILING_OUTPUT_DIRECTORY}"
        RUNTIME_OUTPUT_DIRECTORY "${KLAB_PROFILING_OUTPUT_DIRECTORY}")
endif ()

if (IOS OR MACOS)
    set_target_properties(KLab_Profiling PROPERTIES CXX_STANDARD 11)
endif ()

if (MACOS)
    set_target_properties(KLab_Profiling PROPERTIES BUNDLE TRUE)
endif ()
