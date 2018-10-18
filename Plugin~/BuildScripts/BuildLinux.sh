#!bin/bash

# Builds libraries for Linux
echo -- Linux plugin build


# Initialize arguments
BuildScriptsDirectory="$(dirname $(readlink -f $0))"
PluginDirectory="$BuildScriptsDirectory/.."
TempDirectory="$PluginDirectory/.temp"
BuildDirectory="$PluginDirectory/../KLab/Profiling/Plugins/Linux"
UnityPluginApiDirectory="$UNITY_PLUGIN_API_ROOT"

BuildType="Release"
Abi="x86"
echo $BuildScriptsDirectory
if [ $(uname -m) = "x86_64" ]; then
  Abi="x64"
fi


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
echo $UnityPluginApiDirectory
if [ -f "$UnityPluginApiDirectory/IUnityInterface.h" ]; then
    echo -- Unity Plugin API found
else
    echo -- \[ERROR\] Unity Plugin API not found
    echo "           ('UNITY_PLUGIN_API_ROOT' environment variable or header files in './Plugin~/Vendor/UnityPluginApi' expected)"

    exit 1
fi


# Clear temp
if [ -d "$TempDirectory" ]; then
    rm -rf "$TempDirectory"
fi


# Build binary
echo 
echo 
echo [$Abi Build]

mkdir $TempDirectory

cd $TempDirectory
cmake "$PluginDirectory" -G"Unix Makefiles" -DCMAKE_BUILD_TYPE="$BuildType" -DKLAB_PROFILING_UNITY_PLUGIN_API_PATH="$UnityPluginApiDirectory" -DKLAB_PROFILING_OUTPUT_DIRECTORY="$BuildDirectory/$Abi"
make
cd ..

rm -rf "$TempDirectory"
