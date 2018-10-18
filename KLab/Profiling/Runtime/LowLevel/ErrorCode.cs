// -------------------------------------------------------------------------------------------- //
//  Copyright (c) KLab Inc.. All rights reserved.                                               //
//  Licensed under the MIT License. See 'LICENSE' in the project root for license information.  //
// -------------------------------------------------------------------------------------------- //

namespace KLab.Profiling.LowLevel
{
    /// <summary>
    /// Error code
    /// </summary>
    public enum ErrorCode : int
    {
        /// <summary>
        /// No error
        /// </summary>
        NoError = 0,

        /// <summary>
        /// Unavailable functionality requested
        /// </summary>
        NotAvailable = -1,

        /// <summary>
        /// One or more arguments have been invailed
        /// </summary>
        InvalidArgument = -2,

        /// <summary>
        /// One or more states have been invailed for operation
        /// </summary>
        InvalidState = -3
    }
}
