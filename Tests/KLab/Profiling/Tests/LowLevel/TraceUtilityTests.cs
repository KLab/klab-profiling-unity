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
    /// <see cref="TraceUtility"/> tests
    /// </summary>
    internal sealed class TraceUtilityTests
    {
        [UnityTest]
        public IEnumerator BeginTrace_EndTrace_Succeeds()
        {
            // Arrange
            var eventBuffer = AllocateEventBuffer(2048);
            var result      = new Profiling.LowLevel.Trace.TraceInfo();


            // Act
            {
                TraceUtility.BeginTrace(eventBuffer, 2048);


                // Trace some frames
                yield return new WaitForEndOfFrame();
                yield return new WaitForEndOfFrame();
                yield return new WaitForEndOfFrame();


                TraceUtility.EndTrace(ref result);
            }


            // Assert
            {
                Assert.Greater(result.EventCount, 0, "Expected trace events");
            }


            // Clean up
            FreeEventBuffer(eventBuffer);
        }

        #region Helpers

        /// <summary>
        /// Allocates event buffer
        /// </summary>
        /// <param name="capacity">Capacity</param>
        private static IntPtr AllocateEventBuffer(int capacity = 2048)
        {
            unsafe
            {
                var sizeof_ = UnsafeUtility.SizeOf<Profiling.LowLevel.Trace.EventInfo>();
                var alignof_ = UnsafeUtility.AlignOf<Profiling.LowLevel.Trace.EventInfo>();

                return (IntPtr)UnsafeUtility.Malloc((sizeof_ * capacity), alignof_, Allocator.Persistent);
            }
        }

        /// <summary>
        /// Frees event buffer
        /// </summary>
        /// <param name="eventBuffer">Event buffer</param>
        private static void FreeEventBuffer(IntPtr eventBuffer)
        {
            unsafe
            {
                UnsafeUtility.Free((void*)eventBuffer, Allocator.Persistent);
            }
        }

        #endregion
    }
}
