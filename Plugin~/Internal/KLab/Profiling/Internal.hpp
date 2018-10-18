// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#pragma once


// -------- //
// INCLUDES //
// -------- //

#include <KLab/Profiling.hpp>
#include <KLab/Profiling/CSharpInterface.h>

#include <atomic>
#include <chrono>

struct IUnityInterfaces;
struct IUnityProfilerCallbacks;
struct UnityProfilerMarkerDesc;


// ------- //
// HELPERS //
// ------- //

namespace KLab { namespace Profiling
{
    /// Non-growing buffer with thread-safe allocate function
    template<typename T>
    struct AtomicBuffer final
    {
        /// Gets base address
        /// @return the base address
        T *GetBase()
        {
            return _base;
        }

        /// Gets number of data currently allocated
        /// @return the number of data allocated
        uint32_t GetLength() const
        {
            return _position;
        }

        /// Initializes buffer
        /// @param base - Buffer base address
        /// @param capacity - Buffer capacity in data
        void Initialize(T *base, const uint32_t capacity)
        {
            _base     = base;
            _capacity = capacity;
            _position = 0;
        }

        /// Allocates datum
        /// @return a valid pointer to allocated datum on success; null otherwise
        T *Allocate()
        {
            const uint32_t index = _position++;


            return ((index < _capacity) ? (_base + index) : nullptr);
        }

        /// Rebases and resets buffer (assuming same capacity)
        /// @param base - New base address
        void RebaseAndReset(T *base)
        {
            Initialize(base, _capacity);
        }

        // Base address
        T *_base = nullptr;
        // Capacity
        uint32_t _capacity = 0;
        // Position
        std::atomic<uint32_t> _position = { 0 };
    };


    /// Array-like list with fixed capacity
    template<typename T, uint32_t N>
    struct FixedCapacityList final
    {
        /// Tries to get datum at index
        /// @param index - Index of datum to get
        /// @return copy of datum at given index if available; default datum otherwise
        const T GetAtOrDefault(const uint32_t index) const
        {
            T default_;


            return (index < _position ? _base[index] : default_);
        }


        /// Adds datum
        /// @returns true on success; false on out-of-memory
        bool Add(const T &datum)
        {
            const uint32_t index = _position++;


            if (index >= _capacity)
            {
                return false;
            }
            

            _base[index] = datum;


            return true;
        }


        // Base address
        T _base[N];
        // Capacity
        static constexpr uint32_t _capacity = N;
        // Position
        uint32_t _position = 0;
    };


    /// Frame timing utility
    struct FrameTimer final
    {
        /// Gets offset since last frame flip
        /// @return the offset since last frame flip in nanoseconds
        inline uint64_t GetTimestampNs() const
        {
            // Be explicit about nanoseconds
            std::chrono::nanoseconds offset = (std::chrono::high_resolution_clock::now() - _lastFlip);


            return uint64_t(offset.count());
        }
        
        /// Flips frame
        /// @returns the offset since last flip in milliseconds
        inline uint32_t FlipMs()
        {
            auto thisFlip = std::chrono::high_resolution_clock::now();
            auto offset   = std::chrono::duration_cast<std::chrono::milliseconds>(thisFlip - _lastFlip);
            _lastFlip     = thisFlip;


            return uint32_t(offset.count());
        }

        /// Resets timer
        inline void Reset()
        {
            _lastFlip = std::chrono::high_resolution_clock::now();
        }

        // Time at last flip
        std::chrono::high_resolution_clock::time_point _lastFlip;
    };


    namespace Trace
    {
        /// Info on a group of sections
        struct SectionGroupInfo final
        {
            /// Group name
            const char *Name = "\0";
            /// Group RGBA color
            uint32_t Color = 0x2d89ef;
        };
    }
}}


// ------------------- //
// ANDOID NATIVE TRACE //
// ------------------- //

namespace KLab { namespace Profiling { namespace Trace
{
    /// Android native trace interface
    struct ATrace final
    {
        /// Flags whether should trace
        /// @return whether tracer is tracing 
        bool (*IsTracing)();
        /// Handles section enter
        /// @param name - Section name
        void (*EnterSection)(const char *name);
        /// Handles section leave
        void (*LeaveSection)();
        /// Unloads interface
        void Unload();

        // Tries initializing interface
        ATrace();

        // Converts to bool
        operator bool() const
        {
            return (IsTracing != nullptr);
        }

        // 'libandroid.so' handle
        void *_libAndroid;
    };


    /// Tries to get Android native trace interface
    /// @return the interface if available; null otherwise
    ATrace *TryGetATrace();
}}}


// -------- //
// C# TRACE //
// -------- //

namespace KLab { namespace Profiling { namespace Trace
{
    /// C# trace interface
    struct CSharpTrace final
    {
        /// Flags whether should trace
        /// @return whether tracer is tracing 
        bool IsTracing() const;
        /// Ticks interface
        void Flip();
        /// Handles entering section enter
        /// @param section - Info on section
        void EnterSection(const SectionInfo &section);
        // Handles section leave
        /// @param section - Info on section
        void LeaveSection(const SectionInfo &section);

        // Frame index
        uint64_t _frameIndex = 0;
        // Frame time
        FrameTimer _frameTimer;
        // Trace event buffer
        AtomicBuffer<KLab_Profiling_Trace_EventInfo> _eventBuffer;
        // Frame flip function
        KLab_Profiling_Trace_FlipFrameFunction _flipFrame = nullptr;
        // Rate to trace at in milliseconds
        uint32_t _rateMs = 0;
        // Offset since last trace in milliseconds
        uint32_t _tMs = 0;
        // Flag whether to trace current frame
        bool _traceFrame = false;
        // Flag whether event buffer ran full
        bool _didEventBufferRunOutOfMemory = false;

        // Flags whether tracing is enabled
        // @return true if enabled; false otherwise
        bool _isEnabled() const;
        // Enables tracing (expecting valid arguments)
        // @param flipFrame - Trace frame flip function
        // @param eventBufferCapacity - Event buffer capacity
        // @param rateMs - Rate to trace at in milliseconds
        void _enable(KLab_Profiling_Trace_FlipFrameFunction flipFrame, const uint32_t eventBufferCapacity, const uint32_t rateMs);
        // Disables tracing
        void _disable();

        // Defaults construction
        CSharpTrace() = default;
        // Prevents copy construction
        CSharpTrace(const CSharpTrace &) = delete;
        // Prevents move construction
        CSharpTrace(CSharpTrace &&) = delete;
    };


    /// Get C# trace interface
    /// @return the interface
    CSharpTrace &GetCSharpTrace();
}}}


// ------------ //
// EXTERN TRACE //
// ------------ //

namespace KLab { namespace Profiling { namespace Trace
{
    /// Tries to get extern trace interface
    /// @return the interface if available; null otherwise
    inline IExternTrace *TryGetExternTrace()
    {
        static IExternTrace *trace = nullptr;


        #if KLAB_PROFILING_HAS_EXTERN_TRACE
        if (!trace)
        {
            trace = LoadExternTrace();
        }
        #endif


        return trace;
    }
}}}


// ----- //
// UTILS //
// ----- //

namespace KLab { namespace Profiling { namespace Utils
{
    /// Utility functions interface
    typedef IExternUtils IUtils;


    /// Gets utility interface
    /// @return the singleton interface
    IUtils &GetUtils();
}}}


// -------------- //
// PLUGIN CONTEXT //
// -------------- //

namespace KLab { namespace Profiling { namespace Plugin
{
    /// Plugin context
    struct PluginContext final
    {



        /// Unity context
        struct
        {
            /// Unity interfaces
            IUnityInterfaces *Interfaces = nullptr;
            /// Profiler callbacks
            IUnityProfilerCallbacks *ProfilerCallbacks = nullptr;
        }
        Unity;
        // Trace context
        struct
        {
            /// Default marker descriptor
            const UnityProfilerMarkerDesc *DefaultMarkerDescriptor = nullptr;
            // [Optional] Android native trace interface
            Trace::ATrace *ATrace = nullptr;
            // C# trace interface
            Trace::CSharpTrace *CSharpTrace = nullptr;
            // [Optional] Extern trace interface
            Trace::IExternTrace *ExternTrace = nullptr;
            // Marker groups
            // INV How much capacity is needed?
            FixedCapacityList<KLab::Profiling::Trace::SectionGroupInfo, 64> SectionGroups;
        }
        Trace;
        // Utilities
        Utils::IUtils *Utils;

        /// Unloads context
        void Unload();

        // Defaults construction
        PluginContext() = default;
        // Prevents copy construction
        PluginContext(const PluginContext &) = delete;
        // Prevents move construction
        PluginContext(PluginContext &&) = delete;

        // Converts to bool
        // @return true if context valid; false otherwise
        operator bool() const
        {
            return (Unity.ProfilerCallbacks != nullptr);
        }
    };


    /// Creates plugin context (expecting valid arguments)
    /// @param unity - Unity interfaces
    /// @return the singleton instance
    PluginContext &CreatePluginContext(IUnityInterfaces *unity);

    /// Gets plugin context
    /// @return the singleton context
    PluginContext &GetPluginContext();
}}}
