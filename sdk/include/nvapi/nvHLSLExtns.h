﻿ /************************************************************************************************************************************\
|*                                                                                                                                    *|
|*     Copyright © 2012 NVIDIA Corporation.  All rights reserved.                                                                     *|
|*                                                                                                                                    *|
|*  NOTICE TO USER:                                                                                                                   *|
|*                                                                                                                                    *|
|*  This software is subject to NVIDIA ownership rights under U.S. and international Copyright laws.                                  *|
|*                                                                                                                                    *|
|*  This software and the information contained herein are PROPRIETARY and CONFIDENTIAL to NVIDIA                                     *|
|*  and are being provided solely under the terms and conditions of an NVIDIA software license agreement.                             *|
|*  Otherwise, you have no rights to use or access this software in any manner.                                                       *|
|*                                                                                                                                    *|
|*  If not covered by the applicable NVIDIA software license agreement:                                                               *|
|*  NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.                                            *|
|*  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.                                                           *|
|*  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                                                                     *|
|*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.                       *|
|*  IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,                               *|
|*  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT,                         *|
|*  NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.            *|
|*                                                                                                                                    *|
|*  U.S. Government End Users.                                                                                                        *|
|*  This software is a "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT 1995),                                       *|
|*  consisting  of "commercial computer  software"  and "commercial computer software documentation"                                  *|
|*  as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government only as a commercial end item.     *|
|*  Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995),                                          *|
|*  all U.S. Government End Users acquire the software with only those rights set forth herein.                                       *|
|*                                                                                                                                    *|
|*  Any use of this software in individual and commercial software must include,                                                      *|
|*  in the user documentation and internal comments to the code,                                                                      *|
|*  the above Disclaimer (as applicable) and U.S. Government End Users Notice.                                                        *|
|*                                                                                                                                    *|
 \************************************************************************************************************************************/

////////////////////////// NVIDIA SHADER EXTENSIONS /////////////////

// this file is to be #included in the app HLSL shader code to make
// use of nvidia shader extensions


#include "nvHLSLExtnsInternal.h"

//----------------------------------------------------------------------------//
//------------------------- Warp Shuffle Functions ---------------------------//
//----------------------------------------------------------------------------//

// all functions have variants with width parameter which permits sub-division 
// of the warp into segments - for example to exchange data between 4 groups of 
// 8 lanes in a SIMD manner. If width is less than warpSize then each subsection 
// of the warp behaves as a separate entity with a starting logical lane ID of 0. 
// A thread may only exchange data with others in its own subsection. Width must 
// have a value which is a power of 2 so that the warp can be subdivided equally; 
// results are undefined if width is not a power of 2, or is a number greater 
// than warpSize.

//
// simple variant of SHFL instruction
// returns val from the specified lane
// optional width parameter must be a power of two and width <= 32
// 
int NvShfl(int val, uint srcLane, int width = NV_WARP_SIZE)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  val;                             // variable to be shuffled
    g_NvidiaExt[index].src0u.y  =  srcLane;                         // source lane
    g_NvidiaExt[index].src0u.z  =  __NvGetShflMaskFromWidth(width);
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_SHFL;
    
    // result is returned as the return value of IncrementCounter on fake UAV slot
    return g_NvidiaExt.IncrementCounter();
}

//
// Copy from a lane with lower ID relative to caller
//
int NvShflUp(int val, uint delta, int width = NV_WARP_SIZE)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  val;                        // variable to be shuffled
    g_NvidiaExt[index].src0u.y  =  delta;                      // relative lane offset
    g_NvidiaExt[index].src0u.z  =  (NV_WARP_SIZE - width) << 8;   // minIndex = maxIndex for shfl_up (src2[4:0] is expected to be 0)
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_SHFL_UP;
    return g_NvidiaExt.IncrementCounter();
}

//
// Copy from a lane with higher ID relative to caller
//
int NvShflDown(int val, uint delta, int width = NV_WARP_SIZE)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  val;           // variable to be shuffled
    g_NvidiaExt[index].src0u.y  =  delta;         // relative lane offset
    g_NvidiaExt[index].src0u.z  =  __NvGetShflMaskFromWidth(width);
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_SHFL_DOWN;
    return g_NvidiaExt.IncrementCounter();
}

//
// Copy from a lane based on bitwise XOR of own lane ID
//
int NvShflXor(int val, uint laneMask, int width = NV_WARP_SIZE)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  val;           // variable to be shuffled
    g_NvidiaExt[index].src0u.y  =  laneMask;      // laneMask to be XOR'ed with current laneId to get the source lane id
    g_NvidiaExt[index].src0u.z  =  __NvGetShflMaskFromWidth(width); 
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_SHFL_XOR;
    return g_NvidiaExt.IncrementCounter();
}


//----------------------------------------------------------------------------//
//----------------------------- Warp Vote Functions---------------------------//
//----------------------------------------------------------------------------//

// returns 0xFFFFFFFF if the predicate is true for any thread in the warp, returns 0 otherwise
uint NvAny(int predicate)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  predicate;
    g_NvidiaExt[index].opcode   = NV_EXTN_OP_VOTE_ANY;
    return g_NvidiaExt.IncrementCounter();
}

// returns 0xFFFFFFFF if the predicate is true for ALL threads in the warp, returns 0 otherwise
uint NvAll(int predicate)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  predicate;
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_VOTE_ALL;
    return g_NvidiaExt.IncrementCounter();
}

// returns a mask of all threads in the warp with bits set for threads that have predicate true
uint NvBallot(int predicate)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].src0u.x  =  predicate;
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_VOTE_BALLOT;
    return g_NvidiaExt.IncrementCounter();
}


//----------------------------------------------------------------------------//
//----------------------------- Utility Functions ----------------------------//
//----------------------------------------------------------------------------//

// returns the lane index of the current thread (thread index in warp)
int NvGetLaneId()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode   =  NV_EXTN_OP_GET_LANE_ID;
    return g_NvidiaExt.IncrementCounter();
}

