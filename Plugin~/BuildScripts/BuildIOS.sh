#!bin/bash

# Builds libraries for iOS
echo -- iOS plugin build


# Initialize arguments
BuildScriptsDirectory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
PluginDirectory="$BuildScriptsDirectory/.."
TempDirectory="$PluginDirectory/.temp"
BuildDirectory="$PluginDirectory/../KLab/Profiling/Plugins/iOS"
UnityPluginApiDirectory="$UNITY_PLUGIN_API_ROOT"
ToolchainFile="$PluginDirectory/Vendor/ios-cmake/ios.toolchain.cmake"

BuildType="Release"


# Parse command line options
echo 
echo 
echo [User Options]


for arg in "$0"
do
    if [ "arg" = "--debug" ]; then
        BuildType="Debug"
    fi
done


if [ "$BuildType" = "Release" ]; then    
    echo -- Build \'$BuildType\' \(pass \'--debug\' on command line for debug build\)
else
    echo -- Build \'$BuildType\'
fi


# Try locate Unity API relatively
if [ ! -d "$UnityPluginApiDirectory" ]; then
    UnityPluginApiDirectory="$PluginDirectory/Vendor/UnityPluginApi"
fi


# Validate
echo 
echo 
echo [Validation]

if [ -f "$ToolchainFile" ]; then
    echo -- CMake iOS toolchain found
else
    echo -- \[ERROR\] CMake iOS toolchain not found
    echo "           (CMake iOS toolchain (https://github.com/leetal/ios-cmake) expected in './Plugin~/Vendor/ios-cmake')"

    exit -1
fi
if [ -f "$UnityPluginApiDirectory/IUnityInterface.h" ]; then
    echo -- Unity Plugin API found
else
    echo -- \[ERROR\] Unity Plugin API not found
    echo "           ('UNITY_PLUGIN_API_ROOT' environment variable or header files in './Plugin~/Vendor/UnityPluginApi' expected)"

    exit -1
fi


# Clear temp
if [ -d "$TempDirectory" ]; then
    rm -rf "$TempDirectory"
fi


# Build binary
echo 
echo 
echo [Fat Build]

mkdir $TempDirectory

cd $TempDirectory
cmake "$PluginDirectory" -G"Unix Makefiles" -DCMAKE_BUILD_TYPE="$BuildType" -DCMAKE_TOOLCHAIN_FILE=$ToolchainFile -DKLAB_PROFILING_UNITY_PLUGIN_API_PATH="$UnityPluginApiDirectory" -DKLAB_PROFILING_OUTPUT_DIRECTORY="$BuildDirectory"
make
cd ..

rm -rf "$TempDirectory"
