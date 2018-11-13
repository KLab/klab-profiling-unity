:: Builds libraries for Android
@echo off
echo -- Android plugin build


:: Initialize arguments
set PluginDirectory=%~dp0..
set TempDirectory=%PluginDirectory%\.temp
set BuildDirectory=%PluginDirectory%\..\Plugins\Android

set UnityPluginApiDirectory=%UNITY_PLUGIN_API_ROOT%

set AndroidNdkRoot=%ANDROID_NDK_ROOT%
set MakeProgram=%AndroidNdkRoot%\prebuilt\windows-x86_64\bin\make.exe
set ToolchainFile=%AndroidNdkRoot%\build\cmake\android.toolchain.cmake

set BuildType="Release"


:: Parse command line options
echo.
echo.
echo [User Options]

for /f "tokens=1*" %%a in ('echo %*') do (
    if "%%a" == "--debug" (
        set BuildType="Debug"
    )
)

if %BuildType% == "Release" (    
    echo -- Build %BuildType% ^(pass '--debug' on command line for debug build^)
) else (
    echo -- Build %BuildType%
)


:: Try locate Unity Plugin API relatively
if not exist "%UnityPluginApiDirectory%" (
    set UnityPluginApiDirectory=%PluginDirectory%\Vendor\UnityPluginAPI
)


:: Validate environment
echo.
echo.
echo [Validation]
if exist "%AndroidNdkRoot%" (
    echo -- Android NDK found
) else (
    echo -- [ERROR] Android NDK not found
    echo            ^('ANDROID_NDK_ROOT' environment variable expected^)

    exit /b -1
)
if exist "%MakeProgram%" (
    echo -- Android NDK make found
) else (
    echo -- [ERROR] Android NDK make found
    echo            ^(64-bit environment expected^)

    exit /b -1
)
if exist "%UnityPluginApiDirectory%\IUnityInterface.h" (
    echo -- Unity Plugin API found
) else (
    echo -- [ERROR] Unity Plugin API not found
    echo            ^(Either 'UNITY_PLUGIN_API_ROOT' environment variable or headers in './Plugin~/Vendor/UnityPluginApi' expected^)

    exit -1
)


:: Clear temp
if exist "%TempDirectory%" (
    rmdir /s /q "%TempDirectory%"
)


:: Build arm64-v8a binary
set Abi="arm64-v8a"
set OutputDirectory=%BuildDirectory%\arm64-v8a

call :ExecuteBuild


:: Build armeabi-v7a binary
set Abi="armeabi-v7a"
set OutputDirectory=%BuildDirectory%\armeabi-v7a

call :ExecuteBuild


:: Build x86 binary
set Abi="x86"
set OutputDirectory=%BuildDirectory%\x86

call :ExecuteBuild


:: Exit
echo.
echo.

if %ERRORLEVEL%==0 (
    echo -- Exiting with success
) else (
    echo -- Exiting with error %ERRORLEVEL%
)

exit /b %ERRORLEVEL%


:: Executes builds
:ExecuteBuild
echo.
echo.
echo [%Abi:~1,-1% Build]

mkdir "%TempDirectory%"

cd "%TempDirectory%"
cmake %PluginDirectory% -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=%BuildType% -DCMAKE_MAKE_PROGRAM=%MakeProgram% -DCMAKE_TOOLCHAIN_FILE=%ToolchainFile% -DANDROID_ABI=%Abi% -DKLAB_PROFILING_UNITY_PLUGIN_API_PATH="%UnityPluginApiDirectory%" -DKLAB_PROFILING_OUTPUT_DIRECTORY="%OutputDirectory%"
call %MakeProgram%
cd ..

rmdir /s /q "%TempDirectory%"
