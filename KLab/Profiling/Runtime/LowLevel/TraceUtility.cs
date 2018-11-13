// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

using System;
using System.Runtime.InteropServices;


namespace KLab.Profiling.LowLevel
{
    namespace Trace
    {
        /// <summary>
        /// Trace event type
        /// </summary>
        public enum EventType : int
        {
            /// <summary>
            /// Enter section event
            /// </summary>
            EnterSection = 0,

            /// <summary>
            /// Leave section event
            /// </summary>
            LeaveSection = 1
        }


        /// <summary>
        /// Trace event info
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct EventInfo
        {
            /// <summary>
            /// Thread name as null-terminated UTF-8 string
            /// </summary>
            public fixed byte Name[32];

            /// <summary>
            /// Thread name as null-terminated UTF-8 string
            /// </summary>
            public fixed byte GroupName[16];

            /// <summary>
            /// Offset since frame flip in nanoseconds
            /// </summary>
            public ulong TimestampNs;

            /// <summary>
            /// Thread name as null-terminated UTF-8 string
            /// </summary>
            public ulong ThreadID;

            /// <summary>
            /// Thread name as null-terminated UTF-8 string
            /// </summary>
            public EventType Type;

            /// <summary>
            /// RGBA marker color
            /// </summary>
            public uint Color;
        }


        /// <summary>
        /// Info on trace frame flip
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct TraceInfo
        {
            /// <summary>
            /// Duration of traced frame in nanoseconds 
            /// </summary>
            public ulong DurationNs;

            /// <summary>
            /// Number of trace events
            /// </summary>
            public uint EventCount;

            /// <summary>
            /// Flag whether event buffer ran out of memory
            /// </summary>
            public uint DidRunOutOfEventMemory;
        }
    }


    /// <summary>
    /// Utilities for C# callback-driven tracing
    /// </summary>
    public static class TraceUtility
    {
        /// <summary>
        /// C functions
        /// </summary>
        private static class C
        {
            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_TraceUtility_BeginTrace")]
            public static extern ErrorCode BeginTrace(IntPtr eventBufferFrame, int eventBufferCapacity);


            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_TraceUtility_EndTrace")]
            public static extern ErrorCode EndTrace(ref Trace.TraceInfo info);
        }


        /// <summary>
        /// Begins tracing
        /// </summary>
        /// <param name="eventBuffer"><see cref="TraceEventInfo"/> array buffer</param>
        /// <param name="eventBufferCapacity">Capacity of buffer for <see cref="TraceEventInfo"/></param>
        /// <returns><see cref="ErrorCode.NoError"/> on success; an error otherwise</returns>
        public static ErrorCode BeginTrace(IntPtr eventBuffer, int eventBufferCapacity)
        {
            // Validate availability
            if (!PluginInfo.IsPluginAvailable)
            {
                return ErrorCode.NotAvailable;
            }

            
            // Validate arguments
            if ((eventBuffer == IntPtr.Zero) || (eventBufferCapacity <= 0))
            {
                return ErrorCode.InvalidArgument;
            }


            return C.BeginTrace(eventBuffer, eventBufferCapacity);
        }


        /// <summary>
        /// Ends tracing
        /// </summary>
        public static ErrorCode EndTrace(ref Trace.TraceInfo info)
        {
            // Validate availability
            if (!PluginInfo.IsPluginAvailable)
            {
                return ErrorCode.NotAvailable;
            }


            return C.EndTrace(ref info);
        }
    }
}
