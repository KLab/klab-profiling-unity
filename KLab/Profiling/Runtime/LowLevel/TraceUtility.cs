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
        public struct FlipFrameInfo
        {
            /// <summary>
            /// Index of traced frame
            /// </summary>
            public ulong FrameIndex;

            /// <summary>
            /// Duration of traced frame in milliseconds 
            /// </summary>
            public uint FrameDurationMs;

            /// <summary>
            /// Number of trace events
            /// </summary>
            public ushort EventCount;

            /// <summary>
            /// Flag whether event buffer ran out of memory
            /// </summary>
            public ushort DidRunOutOfEventMemory;
        }


        /// <summary>
        /// Flips trace frame
        /// </summary>
        /// <param name="info">Info on traced frame</param>
        /// <param name="events">Events buffer (which is <see langword="null"/> on initial flip)</param>
        /// <returns>Next event buffer</returns>
        public unsafe delegate EventInfo* FlipFrameFunction(FlipFrameInfo info, EventInfo* events);
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
            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_Trace_Enable")]
            public static extern ErrorCode Enable(IntPtr flipFrame, int eventBufferCapacity, float rateS);


            [DllImport(PluginInfo.DllName, EntryPoint = "KLab_Profiling_Trace_Disable")]
            public static extern void Disable();
        }


        /// <summary>
        /// Begins tracing
        /// </summary>
        /// <param name="flip">Delegate for flipping trace frame that must be valid until at least <see cref="EndTracing"/> called</param>
        /// <param name="eventBufferCapacity">Capacity of buffer for <see cref="TraceEventInfo"/></param>
        /// <param name="rate">Rate at which to trace in seconds</param>
        /// <returns><see cref="ErrorCode.NoError"/> on success; an error otherwise</returns>
        public static ErrorCode BeginTrace(Trace.FlipFrameFunction flip, int eventBufferCapacity, float rate)
        {
            // Validate availability
            if (!PluginInfo.IsPluginAvailable)
            {
                return ErrorCode.NotAvailable;
            }

            
            // Validate arguments
            if ((flip == null) || (eventBufferCapacity <= 0))
            {
                return ErrorCode.InvalidArgument;
            }
            if (rate < 0.0f)
            {
                rate = 0.0f;
            }


            var flipFunctionPointer = Marshal.GetFunctionPointerForDelegate(flip);


            return C.Enable(flipFunctionPointer, eventBufferCapacity, rate);
        }


        /// <summary>
        /// Ends tracing
        /// </summary>
        public static void EndTrace()
        {
            if (!PluginInfo.IsPluginAvailable)
            {
                return;
            }


            C.Disable();
        }
    }
}
