// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#include <KLab/Profiling/Internal.hpp>


// -------- //
// INCLUDES //
// -------- //

#if !(KLAB_PROFILING_HAS_EXTERN_UTILS)
#if (_WIN32)
#include <Windows.h>
#elif (KLAB_PROFILING_HAS_PTHREAD)
#include <pthread.h>
#endif
#endif


// ----- //
// UTILS //
// ----- //

namespace KLab { namespace Profiling { namespace Utils
{
    uint64_t IExternUtils::GetThreadID()
    {        
        #if (_WIN32)
        return (uint64_t)GetCurrentThreadId();
        #elif (KLAB_PROFILING_HAS_PTHREAD)
        return (uint64_t)pthread_self();
        #else
        return (uint64_t)0;
        #endif
    }
        
        
    Utf8Buffer IExternUtils::ConvertUtf16ToUtf8(const char16_t *utf16String, const uint32_t utf16Length)
    {
        Utf8Buffer utf8 = { { 0 } };


        #if (_WIN32)
        // Use Windows API if available
        if (!WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWCH>(utf16String), int(utf16Length), utf8.CString, sizeof(utf8.CString), nullptr, nullptr))
        {
            // Silently handle conversion error
            utf8.CString[0] = '\0';
        }
        #else
        // Memcopy
        auto storage = reinterpret_cast<const char *>(utf16String);
        auto end     = storage + (utf16Length * 2 ) + 1;


        if ((end - storage) > sizeof(utf8.CString))
        {
            end = ( storage + sizeof(utf8.CString));
        }


        for (size_t c = size_t(0); storage < end; ++storage, ++c)
        {
            utf8.CString[c] = *storage;
        }
        #endif


        return utf8;
    }


    IUtils &GetUtils()
    {
        #if (KLAB_PROFILING_HAS_EXTERN_UTILS)
        static IUtils *utils = LoadExternUtils();


        return &utils;
        #else
        static IUtils utils;


        return utils;
        #endif
    }
}}}
