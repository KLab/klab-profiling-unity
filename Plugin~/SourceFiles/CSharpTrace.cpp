// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#include <KLab/Profiling/Internal.hpp>


// ------- //
// HELPERS //
// ------- //

namespace KLab { namespace Profiling { namespace Trace
{
    // Copies string
    // @param out - Output buffer
    // @param in - Input string
    // @param outCapacity - Capacity of output buffer in characters
    static void _copyString(char *out, const char *in, const size_t outCapacity)
    {
        // Null-terminate out
        out[outCapacity - 1] = '\0';


        for (auto end = (out + outCapacity); out < end; ++out, ++in)
        {
            *out = *in;


            if (!(*in))
            {
                break;
            }
        }
    }


    // Initializes event KLab_Profiling_Trace_EventInfo
    // @param event - Info to initialize
    // @param type - Trace event type
    // @param info - Trace section info
    // @param timestampNs - Trace event timestamp in nanoseconds
    static void _initializeEvent(KLab_Profiling_Trace_EventInfo &event, const KLab_Profiling_Trace_EventType type, const SectionInfo &section, const uint64_t timestampNs)
    {
        // Store scalars
        event.Type        = type;
        event.TimestampNs = timestampNs;
        event.ThreadID    = section.ThreadID;
        event.Color       = section.Color;


        // Copy strings
        _copyString(event.GroupName, section.GroupName, sizeof(event.GroupName));
        _copyString(event.Name,      section.Name,      sizeof(event.Name));
    }
}}}


// -------- //
// C# TRACE //
// -------- //

namespace KLab { namespace Profiling { namespace Trace
{
    bool CSharpTrace::IsTracing() const
    {
        return _isTracing;
    }


    void CSharpTrace::EnterSection(const SectionInfo &section)
    {
        auto event = _eventBuffer.Allocate();


        if (event)
        {
            _initializeEvent(*event, KLab_Profiling_Trace_EventType_EnterSection, section, _timer.GetTimestampNs());
        }
        else
        {
            _didEventBufferRunOutOfMemory = true;
        }
    }


    void CSharpTrace::LeaveSection(const SectionInfo &section)
    {
        auto event = _eventBuffer.Allocate();


        if (event)
        {
            _initializeEvent(*event, KLab_Profiling_Trace_EventType_LeaveSection, section, _timer.GetTimestampNs());
        }
        else
        {
            _didEventBufferRunOutOfMemory = true;
        }
    }


    bool CSharpTrace::_isEnabled() const
    {
        return _isTracing;
    }


    void CSharpTrace::_enable(KLab_Profiling_Trace_EventInfo *eventBuffer, const uint32_t eventBufferCapacity)
    {
        // Initialize state
        _timer.Reset();
        _eventBuffer.Initialize(eventBuffer, eventBufferCapacity);

        _isTracing = true;
    }
    

    KLab_Profiling_Trace_TraceInfo CSharpTrace::_disable()
    {
        _isTracing = false;


        return
        {
            _timer.GetTimestampNs(),
            _eventBuffer.GetLength(),
            _didEventBufferRunOutOfMemory
        };
    }


    CSharpTrace &GetCSharpTrace()
    {
        static CSharpTrace trace;


        return trace;
    }
}}}


// ----- //
// TRACE //
// ----- //

KLab_Profiling_ErrorCode KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_TraceUtility_BeginTrace(KLab_Profiling_Trace_EventInfo *eventBuffer, const int32_t eventBufferSize)
{
    auto &trace  = KLab::Profiling::Trace::GetCSharpTrace();


    // Validate state
    if (trace._isEnabled())
    {
        return KLab_Profiling_ErrorCode_InvalidState;
    }


    // Validate arguments
    if (!eventBuffer || (eventBufferSize <= 0))
    {
        return KLab_Profiling_ErrorCode_InvalidArgument;
    }


    trace._enable(eventBuffer, uint32_t(eventBufferSize));


    return KLab_Profiling_ErrorCode_NoError;
}


KLab_Profiling_ErrorCode KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_TraceUtility_EndTrace(KLab_Profiling_Trace_TraceInfo *info)
{
    auto &trace = KLab::Profiling::Trace::GetCSharpTrace();


    // Validate state
    if (!trace._isEnabled())
    {
        return KLab_Profiling_ErrorCode_InvalidState;
    }


    // Store trace result
    *info = trace._disable();


    return KLab_Profiling_ErrorCode_NoError;
}
