// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;


namespace KLab.Profiling
{
    /// <summary>
    /// Plugin initializer
    /// </summary>
    internal sealed class PluginDriver : MonoBehaviour
    {
        /// <summary>
        /// C functions
        /// </summary>
        private static class C
        {
            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_Plugin_Initialize")]
            public static extern LowLevel.ErrorCode Initialize();


            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_Plugin_Update")]
            public static extern void Update();
        }


        /// <summary>
        /// Driver singleton instance
        /// </summary>
        public static PluginDriver Singleton { get; private set; }

        /// <summary>
        /// Flag whether driver is initialized
        /// </summary>
        internal static bool IsInitialized
        {
            get
            {
                return Singleton != null;
            }
        }


        /// <summary>
        /// Initializes plugin
        /// </summary>
        [RuntimeInitializeOnLoadMethod]
        private static void Initialize ()
        {
            // INV How to handle platform support detection more gracefully
            var isPlatformSupported = Application.isEditor;
            isPlatformSupported     = isPlatformSupported
                || (Application.platform == RuntimePlatform.Android)
                || (Application.platform == RuntimePlatform.IPhonePlayer)
                || (Application.platform == RuntimePlatform.LinuxPlayer)
                || (Application.platform == RuntimePlatform.OSXPlayer);


            if (!isPlatformSupported)
            {
                return;
            }


            // Initialize plugin
            var cInitializeResult = C.Initialize();


            if (cInitializeResult != LowLevel.ErrorCode.NoError)
            {
                return;
            }


            // Create singleton
            {
                Debug.Assert(Singleton == null, "Expected singleton not to be created");


                var owner = new GameObject(typeof(PluginDriver).FullName);
                Singleton = owner.AddComponent<PluginDriver>();


                DontDestroyOnLoad(owner);
            }
        }


        /// <summary>
        /// Flips frame
        /// </summary>
        /// <returns>Scheduling timer</returns>
        private IEnumerator CUpdate()
        {
            for (;;)
            {
                yield return new WaitForEndOfFrame();


                C.Update();
            }
        }

        #region Unity Event Handling

        /// <summary>
        /// Schedules <see cref="CUpdate"/>
        /// </summary>
        private void Awake()
        {
            StartCoroutine(CUpdate());
        }

        #endregion
    }
}
