// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#pragma once


// -------- //
// INCLUDES //
// -------- //

#include <stdint.h>


#if (__cplusplus)
extern "C"
{
#endif


#if (_WIN32)
#define KLAB_PROFILING_CSHARP_INTERFACE __declspec(dllexport) __stdcall
#else
#define KLAB_PROFILING_CSHARP_INTERFACE __attribute__ ((visibility("default")))
#endif


// ----------- //
// ERROR CODES //
// ----------- //

/// Error codes
enum
{
    /// Operation was successful
    KLab_Profiling_ErrorCode_NoError         = 0,
    /// Requested feature not available
    KLab_Profiling_ErrorCode_NotAvailable    = -1,
    /// Invalid arguments passed
    KLab_Profiling_ErrorCode_InvalidArgument = -2,
    /// Invalid state
    KLab_Profiling_ErrorCode_InvalidState    = -3
};
typedef int32_t KLab_Profiling_ErrorCode;


// ------ //
// PLUGIN //
// ------ //

/// Initializes plugin
/// @return ::KLab_Profiling_ErrorCode_NoError on success; an error code otherwise
KLab_Profiling_ErrorCode KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_Plugin_Initialize();
/// Flips (trace) frame
void KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_Plugin_FlipFrame();


// ----------- //
// PLUGIN INFO //
// ----------- //

/// Gets whether native Android tracing API is available
/// @return 1 if available; 0 otherwise
int32_t KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_PluginInfo_SupportsAndroidNativeTrace();
/// Gets whether extern tracer is available
/// @return 1 if available; 0 otherwise
int32_t KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_PluginInfo_SupportsExternTrace();


// ----- //
// TRACE //
// ----- //

/// Trace event type
enum
{
    /// Section enter event
    KLab_Profiling_Trace_EventType_EnterSection = 0,
    /// Section leave event
    KLab_Profiling_Trace_EventType_LeaveSection = 1
};
typedef uint32_t KLab_Profiling_Trace_EventType;


/// Trace event info
typedef struct
{
    /// Name of section
    char Name[32];
    /// Name of group
    char GroupName[16];
    /// Offset since trace frame flip in nanoseconds
    uint64_t TimestampNs;
    /// C-casted thread ID
    uint64_t ThreadID;
    /// Event type
    uint32_t Type;
    /// RGBA Color
    uint32_t Color;
}
KLab_Profiling_Trace_EventInfo;


/// Trace frame flip info
typedef struct
{
    /// Index of traced frame
    uint64_t FrameIndex;
    /// Duration of traced frame in milliseconds
    uint32_t FrameDurationMs;
    /// Number of trace events
    uint32_t EventCount;
    /// Flag whether buffer couldn't fit trace events
    uint32_t DidRunOutOfEventMemory;
}
KLab_Profiling_Trace_FlipFrameInfo;


/// Trace frame flip function
/// @param info - Info on traced frame
/// @param events - Trace events (that is null on first invokation)
/// @return next buffer to use for tracing next events
typedef KLab_Profiling_Trace_EventInfo *(*KLab_Profiling_Trace_FlipFrameFunction)(const KLab_Profiling_Trace_FlipFrameInfo info, KLab_Profiling_Trace_EventInfo *events);


/// Enables C# callback driven tracing
/// @param flipFrame - Function for flipping trace frame
/// @param eventBufferSize - Capacity of event frame buffer(s)
/// @param rateS - Rate to trace frames at (in seconds)
/// @return ::KLab_Profiling_ErrorCode_NoError on success; an error code otherwise
KLab_Profiling_ErrorCode KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_Trace_Enable(KLab_Profiling_Trace_FlipFrameFunction flipFrame, const int32_t eventBufferSize, const float rateS);
/// Disables C# callback driven tracing
void KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_Trace_Disable();


#if (__cplusplus)
}
#endif
