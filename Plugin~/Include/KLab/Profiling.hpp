// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#pragma once


// -------- //
// INCLUDES //
// -------- //

#include <cstdint>


// ----- //
// TRACE //
// ----- //

namespace KLab { namespace Profiling { namespace Trace
{
    /// Info on trace section
    struct SectionInfo final
    {
        /// Group name as null-terminated UTF-8 string
        const char *GroupName;
        /// Section name as null-terminated UTF-8 string
        const char *Name;
        /// C-casted thread ID
        uint64_t ThreadID;
        /// Section color as 32-bit RGBA
        uint32_t Color;
        // [Unused] Padding
        uint32_t _padding;
    };


    // Trace interface
    struct IExternTrace
    {
        /// Flags whether should trace
        /// @return whether tracer is tracing 
        virtual bool IsTracing() = 0;
        /// [Optional] Called once at end of frame to flip trace frame
        virtual void Flip() {} 
        /// Handles entering section enter
        /// @param section - Info on section
        virtual void EnterSection(const SectionInfo &section) = 0;
        // Handles section leave
        /// @param section - Info on section
        virtual void LeaveSection(const SectionInfo &section) = 0;
        /// [Optional] Unloads interface
        virtual void Unload() {}

        // Allows virtual destruction
        virtual ~IExternTrace() {}
    };


    /// Loads extern trace interface
    /// @return the interface
    #if KLAB_PROFILING_HAS_EXTERN_TRACE
    extern IExternTrace &LoadExternTrace();
    #endif
}}}


// ----- //
// UTILS //
// ----- //

namespace KLab { namespace Profiling { namespace Utils
{
    /// Null-terminated UTF-8 string buffer
    struct Utf8Buffer final
    {
        /// C-string storage
        char CString[128];
    };


    /// Utility functions interface
    struct IUtils
    {
        /// [Optional] Gets C-cast thread ID of current thread
        /// @return the thread ID
        virtual uint64_t GetThreadID();
        /// [Optional] Converts UTF-16 string to UTF-8 string
        /// @param utf16String - String to convert
        /// @param utf16Length - Length of string (in characters)
        /// @return the string converted to UTF-8
        virtual Utf8Buffer ConvertUtf16ToUtf8(const char16_t *utf16String, const uint32_t utf16Length);
        /// [Optional] Unloads interface
        virtual void Unload() {}

        // Allows virtual destruction
        virtual ~IUtils() {}
    };


    /// Extern  Utility functions interface
    typedef IUtils IExternUtils;


    /// Loads extern utility interface
    /// @return the interface
    #if KLAB_PROFILING_HAS_EXTERN_UTILS
    extern IExternUtils &LoadExternUtils(IUtils &interface);
    #endif
}}}
