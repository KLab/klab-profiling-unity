// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

#include <KLab/Profiling/Internal.hpp>


// ----------- //
// PLUGIN INFO //
// ----------- //

int32_t KLAB_PROFILING_CSHARP_INTERFACE KLab_Profiling_PluginInfo_SupportsExternTrace()
{
    return (KLab::Profiling::Trace::TryGetExternTrace() != nullptr );
}
