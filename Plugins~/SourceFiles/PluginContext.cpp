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


// -------------- //
// PLUGIN CONTEXT // 
// -------------- //

namespace KLab { namespace Profiling { namespace Plugin
{
    void PluginContext::Unload()
    {
        if (!*this)
        {
            return;
        }


        if (Trace.ATrace)
        {
            Trace.ATrace->Unload();
        }
        if (Trace.ExternTrace)
        {
            Trace.ExternTrace->Unload();
        }
        Utils->Unload();


        Unity.Interfaces = nullptr;
    }


    PluginContext &CreatePluginContext(IUnityInterfaces *unity)
    {
        auto &context = GetPluginContext();


        // Don't re-initialize
        if (context)
        {
            return context;
        }


        context.Unity.Interfaces        = unity;
        context.Unity.ProfilerCallbacks = unity->Get<IUnityProfilerCallbacks>();
        context.Trace.ATrace            = KLab::Profiling::Trace::TryGetATrace();
        context.Trace.CSharpTrace       = &KLab::Profiling::Trace::GetCSharpTrace();
        context.Trace.ExternTrace       = KLab::Profiling::Trace::TryGetExternTrace();
        context.Utils                   = &KLab::Profiling::Utils::GetUtils();


        return context;
    }


    PluginContext &GetPluginContext()
    {
        static PluginContext context;


        return context;
    }
}}}
