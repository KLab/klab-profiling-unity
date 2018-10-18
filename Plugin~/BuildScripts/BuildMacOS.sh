#!bin/bash

# Builds libraries for macOS
echo -- macOS plugin build


# Initialize arguments
BuildScriptsDirectory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
PluginDirectory="$BuildScriptsDirectory/.."
TempDirectory="$PluginDirectory/.temp"
BuildDirectory="$PluginDirectory/../KLab/Profiling/Plugins/MacOS"
UnityPluginApiDirectory="$UNITY_PLUGIN_API_ROOT"

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
echo [Bundle Build]

mkdir $TempDirectory

cd $TempDirectory
cmake "$PluginDirectory" -G"Unix Makefiles" -DCMAKE_BUILD_TYPE="$BuildType" -DKLAB_PROFILING_UNITY_PLUGIN_API_PATH="$UnityPluginApiDirectory" -DKLAB_PROFILING_OUTPUT_DIRECTORY="$BuildDirectory"
make
cd ..

rm -rf "$TempDirectory"
