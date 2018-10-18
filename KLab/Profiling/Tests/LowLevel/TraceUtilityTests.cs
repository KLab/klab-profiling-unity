using KLab.Profiling.LowLevel;
using NUnit.Framework;
using System;
using System.Collections;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.TestTools;


namespace KLab.Profiling.Tests.LowLevel
{
    /// <summary>
    /// Mock for tracing
    /// </summary>
    internal sealed class TraceMock
    {
        /// <summary>
        /// <see cref="EventBuffer"/> capacity
        /// </summary>
        public int EventBufferCapacity { get; private set; }

        /// <summary>
        /// Event buffer
        /// </summary>
        public IntPtr EventBuffer { get; private set; }
        
        /// <summary>
        /// Number of total events traced
        /// </summary>
        public int TracedEventCount { get; private set; }

        #region Ctors

        /// <summary>
        /// Initializes instance
        /// </summary>
        /// <param name="capacity"><see cref="EventBuffer"/> capacity</param>
        public TraceMock(int capacity = 2048)
        {
            EventBufferCapacity = capacity;


            unsafe
            {
                var sizeof_  = UnsafeUtility.SizeOf<Profiling.LowLevel.Trace.EventInfo>();
                var alignof_ = UnsafeUtility.AlignOf<Profiling.LowLevel.Trace.EventInfo>();

                EventBuffer  = (IntPtr)UnsafeUtility.Malloc((sizeof_ * capacity), alignof_, Allocator.Persistent);
            }


            TracedEventCount = 0;
        }

        #endregion

        /// <summary>
        /// Handles trace frame flip
        /// </summary>
        /// <remarks>
        /// In real-world situations, try to flip as fast as possible
        /// </remarks>
        /// <param name="info">Frame info</param>
        /// <param name="events">Events</param>
        /// <returns>Event buffer for next trace frame</returns>
        private unsafe Profiling.LowLevel.Trace.EventInfo* FlipFrame(Profiling.LowLevel.Trace.FlipFrameInfo info, Profiling.LowLevel.Trace.EventInfo* events)
        {
            if ((IntPtr)events != IntPtr.Zero)
            {
                TracedEventCount += info.EventCount;
            }


            return (Profiling.LowLevel.Trace.EventInfo*)EventBuffer;
        }


        /// <summary>
        /// Begins tracing
        /// </summary>
        /// <param name="rate">Rate to trace frames at in seconds</param>
        public void BeginTrace(float rate = 0f)
        {
            unsafe
            {
                TraceUtility.BeginTrace(FlipFrame, EventBufferCapacity, rate);
            }
        }

        /// <summary>
        /// Ends tracing
        /// </summary>
        /// <returns>Total number of events traced</returns>
        public int EndTrace()
        {
            TraceUtility.EndTrace();


            return TracedEventCount;
        }


        /// <summary>
        /// Disposes unmanaged resources
        /// </summary>
        public void Dispose()
        {
            if (EventBuffer != IntPtr.Zero)
            {
                unsafe
                {
                    UnsafeUtility.Free((void *)EventBuffer, Allocator.Persistent);
                }
            }


            EventBufferCapacity = 0;
            EventBuffer         = IntPtr.Zero;
        }
    }


    /// <summary>
    /// <see cref="TraceUtility"/> tests
    /// </summary>
    internal sealed class TraceUtilityTests
    {
        [UnityTest]
        public IEnumerator BeginTrace_WhenTracing_Succeeds()
        {
            // Arrange
            var trace = new TraceMock();


            // Act
            {
                trace.BeginTrace();


                // Trace some frames
                yield return new WaitForEndOfFrame();
                yield return new WaitForEndOfFrame();
                yield return new WaitForEndOfFrame();
            }


            // Assert
            {
                var result = trace.EndTrace();


                Assert.Greater(result, 0, "Expected trace events");
            }


            // Clean up
            trace.Dispose();
        }
    }
}
