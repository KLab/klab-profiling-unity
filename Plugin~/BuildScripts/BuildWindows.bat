:: Builds libraries for Windows
@echo off
echo -- Windows plugin build


:: Initialize arguments
set PluginDirectory=%~dp0..
set TempDirectory=%PluginDirectory%\.temp
set BuildDirectory=%PluginDirectory%\..\KLab\Profiling\Plugins\Windows

set UnityPluginApiDirectory=%UNITY_PLUGIN_API_ROOT%

if "%VSWHERE%"=="" (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
  set VsDirectory=%%i
)
set VsDevCmd="%VsDirectory%\Common7\Tools\VsDevCmd.bat"

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

if %BuildType%=="Release" (    
    echo -- Build %BuildType%  ^(pass '--debug' on command line for debug build^)
) else (
    echo -- Build %BuildType%
)


:: Try locate Unity Plugin API relatively
if not exist "%UnityPluginApiDirectory%" (
    set UnityPluginApiDirectory=%PluginDirectory%\Vendor\UnityPluginAPI
)


:: Validate
echo.
echo.
echo [Validation]
if exist "%VsDirectory%" (
    echo -- Visual Studio found
) else (
    echo -- [ERROR] Visual Studio not found
    echo            ^(Make sure to have Visual Studio 2017 installed^)

    exit /b -1
)
if exist "%UnityPluginApiDirectory%\IUnityInterface.h" (
    echo -- Unity Plugin API found
) else (
    echo -- [ERROR] Unity Plugin API not found
    echo            ^(Either 'UNITY_PLUGIN_API_ROOT' environment variable or headers in './Plugin~/Vendor/UnityPluginApi' expected^)

    exit /b -1
)


:: Clear temp
if exist "%TempDirectory%" (
    rmdir /s /q "%TempDirectory%"
)


:: Build x64 binary
echo.
echo.
echo [x64 Build]

mkdir "%TempDirectory%"

cd "%TempDirectory%"
call %VsDevCmd% -arch=x64
cmake %PluginDirectory% -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=%BuildType% -DKLAB_PROFILING_UNITY_PLUGIN_API_PATH="%UnityPluginApiDirectory%" -DKLAB_PROFILING_OUTPUT_DIRECTORY="%BuildDirectory%\x64"
nmake
cd ..

rmdir /s /q "%TempDirectory%"



:: Exit
echo.
echo.

if %ERRORLEVEL%==0 (
    echo -- Exiting with success
) else (
    echo -- Exiting with error %ERRORLEVEL%
)

exit /b %ERRORLEVEL%
