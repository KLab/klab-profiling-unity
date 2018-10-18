// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#include <KLab/Profiling/Internal.hpp>


// -------- //
// INCLUDES //
// -------- //

#if (KLAB_PROFILING_HAS_ATRACE)
#include <dlfcn.h>
#endif


// ------------------- //
// ANDOID NATIVE TRACE //
// ------------------- //

namespace KLab { namespace Profiling { namespace Trace
{
    ATrace::ATrace() : IsTracing(nullptr)
    {
        #if (KLAB_PROFILING_HAS_ATRACE)
        // Try load symbols
        _libAndroid = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);


        if (_libAndroid)
        {
            IsTracing    = reinterpret_cast<bool (*)()>(dlsym(_libAndroid, "ATrace_isEnabled"));
            EnterSection = reinterpret_cast<void (*)(const char *)>(dlsym(_libAndroid, "ATrace_beginSection"));
            LeaveSection = reinterpret_cast<void (*)()>(dlsym(_libAndroid, "ATrace_endSection"));


            if (!IsTracing)
            {
                dlclose(_libAndroid);


                _libAndroid = nullptr;
            }
        }
        #endif
    }


    void ATrace::Unload()
    {
        #if (KLAB_PROFILING_HAS_ATRACE)
        if (IsTracing)
        {
            dlclose(_libAndroid);


            _libAndroid = nullptr;
            IsTracing   = nullptr;
        }
        #endif
    }


    ATrace *TryGetATrace()
    {
        static ATrace interface;


        return (interface ? &interface : nullptr);
    }
}}}


// ----------- //
// PLUGIN INFO //
// ----------- //

int32_t KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_PluginInfo_SupportsAndroidNativeTrace()
{
    return (KLab::Profiling::Trace::TryGetATrace() != nullptr);
}
