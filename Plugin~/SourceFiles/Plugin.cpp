// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#include <KLab/Profiling/Internal.hpp>


// -------- //
// INCLUDES //
// -------- //

#include <IUnityInterface.h>
#include <IUnityProfilerCallbacks.h>


// ------ //
// PLUGIN //
// ------ //

namespace KLab { namespace Profiling { namespace Plugin
{
    // Checks whether Android native is tracing
    // param contxt - Plugin context
    // @return true if tracing; false otherwise
    #if (KLAB_PROFILING_HAS_ATRACE)
    static inline bool _isATraceTracing(const PluginContext &context)
    {
        return (context.Trace.ATrace && context.Trace.ATrace->IsTracing());
    }
    #else
    #define _isATraceTracing(context) (false)
    #endif

    // Checks whether C# is tracing
    // param contxt - Plugin context
    // @return true if tracing; false otherwise
    static inline bool _isCSharpTracing(const PluginContext &context)
    {
        return (context.Trace.CSharpTrace->IsTracing());
    }

    // Checks whether extern is tracing
    // param contxt - Plugin context
    // @return true if tracing; false otherwise
    #if (KLAB_PROFILING_HAS_EXTERN_TRACE)
    static inline bool _isExternTracing(const PluginContext &context)
    {
        return (context.Trace.ExternTrace && context.Trace.ExternTrace->IsTracing());
    }
    #else
    #define _isExternTracing(context) (false)
    #endif


    // Handles Unity marker event
    // @param descriptor - Marker descriptor
    // @param type - Event type
    // @param dataCount - Number of data
    // @param data - Data
    static void UNITY_INTERFACE_API _handleMarkerEvent(const UnityProfilerMarkerDesc *descriptor, UnityProfilerMarkerEventType type, uint16_t dataCount, const UnityProfilerMarkerData *data, void *_unused)
    {
        Utils::Utf8Buffer utf8Buffer;

        auto              &context = GetPluginContext();
        const auto         group   = context.Trace.SectionGroups.GetAtOrDefault(uint32_t(descriptor->categoryId));
        Trace::SectionInfo section =
        {
            group.Name,
            descriptor->name,
            context.Utils->GetThreadID(),
            group.Color
        };


        // Convert 'Profiler.Default' emitted UTF-16 to UTF-8
        if ((dataCount > 1) && (descriptor == context.Trace.DefaultMarkerDescriptor))
        {
            utf8Buffer   = context.Utils->ConvertUtf16ToUtf8(reinterpret_cast<const char16_t *>(data[1].ptr), (data[1].size / 2));
            section.Name = utf8Buffer.CString;
        }


        // Forward event
        switch (type)
        {
            // Begin section
            case kUnityProfilerMarkerEventTypeBegin:
            {
                if (_isATraceTracing(context))
                {
                    context.Trace.ATrace->EnterSection(section.Name);
                }
                if (_isCSharpTracing(context))
                {
                    context.Trace.CSharpTrace->EnterSection(section);
                }

                if (_isExternTracing(context))
                {
                    context.Trace.ExternTrace->EnterSection(section);
                }
                break;
            }

            // End section
            case kUnityProfilerMarkerEventTypeEnd:
            {
                if (_isATraceTracing(context))
                {
                    context.Trace.ATrace->LeaveSection();
                }
                if (_isCSharpTracing(context))
                {
                    context.Trace.CSharpTrace->LeaveSection(section);
                }

                if (_isExternTracing(context))
                {
                    context.Trace.ExternTrace->LeaveSection(section);
                }
                break;
            }
        }
    }


    // Handles category creation
    // @param descriptor - Category descriptor
    static void UNITY_INTERFACE_API _handleCreateCategory(const UnityProfilerCategoryDesc *descriptor, void *_unused)
    {
        Trace::SectionGroupInfo group;
        
        auto &context = GetPluginContext();
        
        
        group.Name  = descriptor->name;
        group.Color = descriptor->rgbaColor;


        context.Trace.SectionGroups.Add(group);
    }


    // Handles Unity marker creation event
    // @param descriptor - Marker descriptor
    static void UNITY_INTERFACE_API _handleCreateMarker(const UnityProfilerMarkerDesc *descriptor, void *_unused)
    {
        // Checks whether descriptor is default marker descriptor
        // @param descriptor - Marker descriptor
        // @return tre if default; false otherwise
        auto isDefaultMarkerDescriptor = [](const UnityProfilerMarkerDesc &descriptor)
        {
            auto lhs = "Profiler.Default";
            auto rhs = descriptor.name;


            for (;; ++lhs, ++rhs)
            {
                if (*lhs != *rhs)
                {
                    return false;
                }
                if (!*lhs)
                {
                    break;
                }
            }


            return true;
        };


        auto &context = GetPluginContext();


        // Store default marker descriptor
        if (!context.Trace.DefaultMarkerDescriptor && isDefaultMarkerDescriptor(*descriptor))
        {
            context.Trace.DefaultMarkerDescriptor = descriptor;
        }


        // Register callback
        context.Unity.ProfilerCallbacks->RegisterMarkerEventCallback(descriptor, _handleMarkerEvent, _unused);
    }


    // Flips frame
    static void _flipFrame()
    {
        auto       &context = GetPluginContext();
        const bool  shouldRegisterCallbacks = (_isATraceTracing(context) || _isCSharpTracing(context) || _isExternTracing(context));
        static bool hasRegisteredCallbacks = false;


        // Register/Unregister callbacks dynamically
        if (shouldRegisterCallbacks != hasRegisteredCallbacks)
        {
            if (shouldRegisterCallbacks)
            {
                context.Unity.ProfilerCallbacks->RegisterCreateMarkerCallback(_handleCreateMarker, nullptr);
            }
            else
            {
                context.Unity.ProfilerCallbacks->UnregisterCreateMarkerCallback(_handleCreateMarker, nullptr);
                context.Unity.ProfilerCallbacks->UnregisterMarkerEventCallback(nullptr, _handleMarkerEvent, nullptr);
            }


            hasRegisteredCallbacks = shouldRegisterCallbacks;
        }


        // Tick C# trace (which is always available)
        context.Trace.CSharpTrace->Flip();


        // Tick extern trace if available
        if (context.Trace.ExternTrace)
        {
            context.Trace.ExternTrace->Flip();
        }
    }
}}}


// ------ //
// PLUGIN //
// ------ //

KLab_Profiling_ErrorCode UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API KLab_Profiling_Plugin_Initialize()
{
    // Assert plugin context to be created
    return (KLab::Profiling::Plugin::GetPluginContext() ? KLab_Profiling_ErrorCode_NoError : KLab_Profiling_ErrorCode_NotAvailable);
}


void KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_Plugin_FlipFrame()
{
    using namespace KLab::Profiling::Plugin;


    // Early out if context invailed
    if (!GetPluginContext())
    {
        return;
    }


    _flipFrame();
}


void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces *unity)
{
    auto &context = KLab::Profiling::Plugin::CreatePluginContext(unity);
    
    
    // Prevent potential 'unused' warnings
    (void)context;
}


void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    using namespace KLab::Profiling::Plugin;


    auto &context = GetPluginContext();


    // Early out if context invailed
    if (!context)
    {
        return;
    }


    // Unregister from Unity
    context.Unity.ProfilerCallbacks->UnregisterMarkerEventCallback(nullptr, _handleMarkerEvent, nullptr);
    context.Unity.ProfilerCallbacks->UnregisterCreateMarkerCallback(_handleCreateMarker, nullptr);
    context.Unity.ProfilerCallbacks->UnregisterCreateCategoryCallback(_handleCreateCategory, nullptr);


    // Release context
    context.Unload();
}
