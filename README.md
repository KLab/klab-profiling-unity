# *KLab Profiling for Unity*

*KLab Profiling for Unity* helps you profile your *Unity* projects.


## Tracing

Building on the [native profiler plugin API introduced with Unity 2018.2](https://unity3d.com/jp/unity/whats-new/unity-2018.2.0), this library  
1. allows you to get detailed info on Unity profiler trace events through *C#* callbacks,
1. automatically forwards trace events to [Android's tracing API](https://developer.android.com/ndk/guides/tracing) if available, and  
1. allows you to handle trace events in *C++*.

If you're interested in how to handle trace events in *C#*, see [here](KLab/Profiling/Runtime/LowLevel/TraceUtility.cs) for the interface,
and [here](KLab/Profiling/Runtime/Tests/LowLevel/TraceUtilityTests#L16) for an example.
At *KLab* we use the *C#* interface for uploading the trace data to cloud storage for later analysis in long run performance tests.

If you want to handle trace events in *C++* you have to implement the trace interface and
link your implementation to the library during native build.
See [here](Plugin~/Include/Klab/Profiling.hpp#L39) for the interface you have to implement.
The *CMake* project provides a [convenience option for linking your trace handler](Plugin~/CMakeLists.txt#L14).

The library uses a [utility interface](Plugin~/Include/Klab/Profiling.hpp#L83)
for querying the ID of the execution thread and for converting UTF-16 strings to UTF-8.
The interface works out-of-the-box on *Win32* and *POSIX* platforms.
You can replace the default interface by providing a custom implementation and linking to it during native build.
Again, the *CMake* project provides a [convenience option for that](Plugin~/CMakeLists.txt#L15).


## Importing Into Unity

The library includes prebuilt plugins for *Android*, *iOS*, *Linux*, *macOS*, and *Win32* and can be imported easily as a Unity package.
It doesn't have any dependencies on other packages, but currently requires *Unity* 2018.2+.


## Native Plugin Build

[Build scripts](Plugin~/BuildScripts) are available for building native *Android*, *iOS*, *Linux*, *macOS*, and *Win32* libraries.

### Common Prerequisites

1. *Makefiles* are generated with [*CMake*](https://cmake.org/). Make sure to have *CMake* 3.10+ installed and added to your path.
1. *Unity* plugin API header files have to be provided. The headers come with the *Unity Editor*. Either set `UNITY_PLUGIN_API_PATH` to point at the folder including the header files or opy them into './Plugin~/Vendor/UnityPluginApi/'.

### *Android* Prerequisites

1. Make sure to have the [*Android NDK*](https://developer.android.com/ndk/) available and `ANDROID_NDK_ROOT` as an environment variable pointing at its root folder.

### *iOS* Prerequisites

1. Make sure to have [*XCode*](https://developer.apple.com/xcode/) installed and available on the command line.
1. Clone (or download) [*ios-cmake*](https://github.com/leetal/ios-cmake) into `./Plugin~/Vendor/ios-cmake/`

### *Linux* Prerequisites

1. Make sure to have build essentials available on the command line.

### *macOS* Prerequisites

1. Make sure to have [*XCode*](https://developer.apple.com/xcode/) installed and available on the command line.

### *Win32* Prerequisites

1. Make sure to have [Visual Studio](https://visualstudio.microsoft.com) (2017+) and components for building *C++* installed.


## Contributing

Fixing issues and contributing directly to the code base is more than welcome. A *rebase workflow* is preferred over branching out.
Please stick to the [*Microsoft* naming guidelines](https://docs.microsoft.com/en-ca/dotnet/standard/design-guidelines/naming-guidelines)
when contributing *C#* code and document public interfaces with *XML*. Please also stick to the guidelines for *C++*.
For *C*, "emulate" namespaces by using underscores instead of the double colons in *C++*, i.e. write `KLab::Profiling` as `KLab_Profiling`.
Public *C/C++* interfaces should be documented in [*Doxygen*](http://www.doxygen.org/) syntax if possible.


## Feedback

- Request a new feature on GitHub
- File a bug in GitHub Issues


## License

Copyright (c) KLab Inc.. All rights reserved.

Licensed under the [MIT](LICENSE) License.
