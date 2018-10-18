// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

using System.Runtime.InteropServices;


namespace KLab.Profiling
{
    /// <summary>
    /// Info on plugin
    /// </summary>
    public static class PluginInfo
    {
        /// <summary>
        /// C functions
        /// </summary>
        private static class C
        {
            [DllImport(DllName, EntryPoint = "KLab_Profiling_PluginInfo_SupportsAndroidNativeTrace")]
            public static extern int SupportsAndroidNativeTrace();


            [DllImport(DllName, EntryPoint = "KLab_Profiling_PluginInfo_SupportsExternTrace")]
            public static extern int SupportsExternTrace();
        }


        /// <summary>
        /// Plugin DLL name
        /// </summary>
#if UNITY_IOS && !UNITY_EDITOR
        internal const string DllName = "__Internal";
#else
        internal const string DllName = "KLab_Profiling";
#endif


        /// <summary>
        /// Flag whether plugin is available
        /// </summary>
        public static bool IsPluginAvailable
        {
            get
            {
                // No driver, no plugin...
                return PluginDriver.IsInitialized;
            }
        }


        /// <summary>
        /// Flag whether native tracing on Android is supported
        /// </summary>
        public static bool SupportsAndroidNativeTrace
        {
            get
            {
                if (!IsPluginAvailable)
                {
                    return false;
                }


                return (C.SupportsAndroidNativeTrace() != 0);
            }
        }


        /// <summary>
        /// Flag whether extern tracing is supported
        /// </summary>
        public static bool SupportsExternTrace
        {
            get
            {
                if (!IsPluginAvailable)
                {
                    return false;
                }


                return (C.SupportsExternTrace() != 0);
            }
        }
    }
}
