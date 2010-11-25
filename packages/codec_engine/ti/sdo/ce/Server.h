/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== Server.h ========
 *  DSP Server module
 *
 *  APIs for accessing information from remote server.
 */

/**
 *  @file       ti/sdo/ce/Server.h
 *
 *  @brief      The Codec Engine Server Interface.  Provides the user an
 *              inteface to open and manipulate a Server which contains
 *              remote algorithms.
 */
/**
 *  @addtogroup   ti_sdo_ce_Server     Codec Engine Server Interface
 */
#ifndef ti_sdo_ce_Server_
#define ti_sdo_ce_Server_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>  /* def of FILE * */

/** @ingroup    ti_sdo_ce_Server */
/*@{*/

/**
 * @brief       Name to pass to Diags_setMask() to enable logging for Server
 *              functions. For example,
 *                  Diags_setMask(Server_MODNAME"+EX1234567");
 *              turns on all Log statements in this module.
 *              Diags_setMask() must be called after initialization to take
 *              effect.
 */
#define Server_MODNAME "ti.sdo.ce.Server"

/**
 *  @brief      Maximum number of characters used in memory segment names.
 */
#define Server_MAXSEGNAMELENGTH 32


/**
 *  @brief      Opaque handle to the server for an engine.
 */
typedef struct Server_Obj *Server_Handle;

/**
 *  @brief      Server error code
 */
typedef enum Server_Status {
    Server_EOK       = 0,      /**< Success. */
    Server_ENOSERVER = 1,      /**< Engine has no server. */
    Server_ENOMEM    = 2,      /**< Unable to allocate memory. */
    Server_ERUNTIME  = 3,      /**< Internal runtime failure. */
    Server_EINVAL    = 4,      /**< Bad value passed to function. */
    Server_EWRONGSTATE =5,     /**< Server is not in the correct state to
                                *   execute the requested function. */
    Server_EINUSE   = 6,       /**< Server call did not succeed because a
                                *   because a required resource is in use. */
    Server_ENOTFOUND = 7,      /**< An entity was not found */
    Server_EFAIL = 8           /**< Unknown failure */
} Server_Status;

/**
 *  @brief      Information for a memory heap of a remote DSP server.
 *
 *  @remarks    Sizes are given in DSP data MAUs.
 *  @sa         Server_getMemStat().
 *
 */
typedef struct Server_MemStat {
    Char   name[Server_MAXSEGNAMELENGTH + 1]; /**< Name of memory heap. */
    Uint32 base;           /**< Base address of the memory segment. */
    Uint32 size;           /**< Original size of the memory segment. */
    Uint32 used;           /**< Number of bytes used. */
    Uint32 maxBlockLen;    /**< Length of the largest contiguous free block. */
} Server_MemStat;

/*
 *  ======== Server_connectTrace ========
 */
/**
 *  @brief                      Connect to server for purposes of collecting
 *                              trace and/or LOG data.
 *
 *  @param[in]  server          Server handle obtained from Engine_getServer().
 *
 *  @param[in]  token           Address to store connection token.  This token
 *                              should be specified in the companion call to
 *                              Server_disconnectTrace().
 *
 *  @retval     Server_EOK       Success, trace token was acquired.
 *  @retval     Server_EINUSE    A connection for server trace is already
 *                               established.
 *  @retval     Server_ERUNTIME  An internal runtime error occurred.
 *
 *  @pre        @c server is non-NULL.
 *  @pre        @c token is non-NULL.
 *
 *  @sa         Server_disconnectTrace().
 *
 */
extern Server_Status Server_connectTrace(Server_Handle server, Int * token);

/*
 *  ======== Server_disconnectTrace ========
 */
/**
 *  @brief                      Disconnect from server when finished collecting
 *                              trace and/or LOG data.
 *
 *  @param[in]  server          Server handle obtained from Engine_getServer().
 *
 *  @param[in]  token           Connection token (as obtained from earlier,
 *                              companion call to Server_connectTrace()).
 *
 *  @retval     Server_EOK       Success.
 *  @retval     Server_ERUNTIME  An internal runtime error occurred.
 *
 *  @pre        @c server is non-NULL.
 *
 *  @sa         Server_connectTrace().
 *
 */
extern Server_Status Server_disconnectTrace(Server_Handle server, Int token);

/*
 *  ======== Server_fwriteTrace ========
 */
/**
 *  @brief              Write Server's trace buffer to specifed file stream
 *
 *  @param[in]  server  Server handle, obtained from Engine_getServer().
 *
 *  @param[in]  prefix  A string to prepend to each line output; this
 *                      allows one to easily identify trace from the
 *                      server from the application's trace, for
 *                      example.
 *  @param[in]  out     An open FILE stream used to output the
 *                      Server's trace characters.
 *
 *  @retval             Integer number of characters copied to the specified
 *                      FILE stream.
 *
 *  @pre             @c server is non-NULL.
 *  @pre             @c Corresponding engine is in the open state.
 *
 *  @post       In the event a negative value is returned,
 *              Engine_getLastError() will return the value:
 *                - #Engine_ERUNTIME    Either an internal runtime error
 *                                      occured or the underlying server
 *                                      error occured.
 */
extern Int Server_fwriteTrace(Server_Handle server, String prefix, FILE *out);

/*
 *  ======== Server_getCpuLoad ========
 */
/**
 *  @brief                      Get Server's CPU usage in percent.
 *
 *  @param[in]  server          Server handle, obtained from Engine_getServer().
 *
 *  @retval                     Integer between 0-100 indicating percentage
 *                              of time the Server is processing measured
 *                              over a period of approximately 1 second.  If
 *                              the load is unavailable, a negative value is
 *                              returned.
 *
 *  @pre        @c server is non-NULL.
 *
 *  @post       In the event a negative value is returned,
 *              Engine_getLastError() will return one of the following values:
 *                - #Engine_ERUNTIME    Either an internal runtime error
 *                                      occured or an underlying server
 *                                      error occured.
 *                - #Engine_EINVAL      The Server handle is not valid.
 *
 */
extern Int Server_getCpuLoad(Server_Handle server);

/*
 *  ======== Server_getMemStat ========
 */
/**
 *  @brief Get information on a memory heap segment of a remote DSP server.
 *
 *  @param[in]  server          Server handle obtained from Engine_getServer().
 *  @param[in]  segNum          The heap number of a segment on the DSP.
 *  @param[out] memStat         Structure to store memory segment information.
 *
 *  @retval     Server_EOK       Success.
 *  @retval     Server_ENOTFOUND @c segNum is out of range.
 *  @retval     Server_ERUNTIME  Internal runtime error occurred.
 *
 *  @pre        @c server is non-NULL.
 *  @pre        @c memStat is non-NULL.
 *
 *  @post       On success, memStat will contain information about the memory
 *              heap @c segNum on the DSP.
 *
 *  @sa         Server_getNumMemSegs().
 */
extern Server_Status Server_getMemStat(Server_Handle server, Int segNum,
        Server_MemStat *memStat);

/*
 *  ======== Server_getNumMemSegs ========
 */
/**
 *  @brief Get the number of memory heap segments of a remote DSP server.
 *
 *  @param[in]  server          Server handle obtained from Engine_getServer().
 *  @param[out] numSegs         The number of heap segments of the DSP server.
 *
 *  @retval     Server_EOK      Success.
 *  @retval     Server_ERUNTIME Internal runtime error occurred.
 *
 *  @pre        @c server is non-NULL.
 *  @pre        @c numSegs is non-NULL.
 *
 *  @post       On success, numSegs will contain the number of memory heaps
 *              on the DSP.
 *
 *  @sa         Server_getMemStat().
 */
extern Server_Status Server_getNumMemSegs(Server_Handle server, Int *numSegs);

/** @cond INTERNAL */
/*
 *  ======== Server_init ========
 */
extern Void Server_init(Void);
/** @endcond */

/*
 *  ======== Server_redefineHeap ========
 */
/**
 *  @brief Set the base address and size of a remote DSP server heap.
 *
 *  This API is used to move and/or resize a named heap of the remote DSP
 *  server.  The address passed to this API is a DSP address and the
 *  memory from @c base to @c base + @c size must be contiguous in physical
 *  memory. The size of the heap should be given in DSP MADUs (minimum
 *  addressable data units).
 *  The name of the heap can be at most #Server_MAXSEGNAMELENGTH
 *  characters long.
 *
 *  For example, in the case of DM644x, suppose that an application wants
 *  to allocate a block of memory on the GPP to be used by the DSP server
 *  for the memory segment named "DDRALGHEAP".  A block of physically
 *  contiguous memory could be obtained by Memory_alloc() and the
 *  corresponding DSP address obtained with
 *  Memory_getBufferPhysicalAddress().  This DSP address and the size
 *  of the block could then be passed to Server_redefineHeap().
 *  For example:
 *
 *  @code
 *      Server_redefineHeap(server, "DDRALGHEAP", base, size);
 *  @endcode
 *
 *  This function can only be called when there is no memory currently
 *  allocated in the heap (since the heap cannot be changed if it is being
 *  used).
 *
 *  @param[in]  server          Server handle obtained from Engine_getServer().
 *  @param[in]  name            Name of heap to be redefined.
 *  @param[in]  base            Base address for algorithm heap.
 *  @param[in]  size            Size (DSP MADUs) of new heap.
 *
 *  @retval     Server_EOK         Success.
 *  @retval     Server_ERUNTIME    Internal runtime error occurred.
 *  @retval     Server_ENOTFOUND   No heap with the specified name was found.
 *  @retval     Server_EINVAL      Changing to the new base and size would
 *                                 cause an overlap with another heap.
 *  @retval     Server_EINUSE      Memory is currently allocated in the heap.
 *
 *  @pre        @c server is non-NULL.
 *  @pre        @c base is a DSP address of a physically contiguous
 *              block of memory.
 *  @pre        @c base is aligned on an 8-byte boundary.
 *
 *  @post       On success, the server's algorithm heap base will have been
 *              set to @c base, and the size will have been set to @c size.
 *
 *  @sa         Server_restoreHeap().
 */
extern Server_Status Server_redefineHeap(Server_Handle server, String name,
        Uint32 base, Uint32 size);

/*
 *  ======== Server_restoreHeap ========
 */
/**
 *  @brief Set the base address and size of a remote DSP server heap
 *         back to their original values.
 *
 *  This function resets the base address and size of a named heap of
 *  the remote server, back to their values before the first call to
 *  Server_redefineHeap() was made. The name of the heap can be at most
 *  #Server_MAXSEGNAMELENGTH characters long, otherwise this function
 *  will return #Server_ENOTFOUND.
 *
 *  As with Server_redefineHeap(), this function can only be called when
 *  no memory is currently allocated from the heap (as the heap cannot be
 *  changed if it is being used).
 *
 *  @param[in]  server      Server handle obtained through Engine_getServer().
 *  @param[in]  name        Name of the heap to be restored.
 *
 *  @retval     Server_EOK         Success.
 *  @retval     Server_ERUNTIME    Internal runtime error occurred.
 *  @retval     Server_ENOTFOUND   No heap with the specified name was found.
 *  @retval     Server_EINVAL      Changing back to the original base and size
 *                                 would cause an overlap with another heap.
 *  @retval     Server_EINUSE      Memory is currently allocated in the heap.
 *
 *  @pre        @c server is non-NULL.
 *
 *  @post       On success, the server's algorithm heap base and size will
 *              have been reset to their original value.
 *
 *  @sa         Server_redefineHeap().
 */
extern Server_Status Server_restoreHeap(Server_Handle server, String name);

/*
 *  ======== Server_setTrace ========
 */
/**
 *  @brief              Set Server's trace mask
 *
 *  @param[in]  server  Server handle obtained through Engine_getServer().
 *  @param[in]  mask    Trace mask, e.g. "*=01234567"
 *
 *  @retval     Server_EOK         Success.
 *  @retval     Server_EINUSE      A connection for server trace is already
 *                                 established.
 *  @retval     Server_ENOSERVER   No server for the engine.
 *  @retval     Server_ERUNTIME    Internal runtime error occurred.
 *
 *  @pre        @c server is non-NULL.
 *  @pre        @c Corresponding engine is in the open state.
 *
 *  @remarks    This only sets the trace for a remote server.  To change
 *              the trace mask for the application-side of the framework,
 *              use Diags_setMask or Diags_setMaskMeta.
 *
 *  @sa         xdc.runtime.Diags
 */
extern Int Server_setTrace(Server_Handle server, String mask);

/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce; 1, 0, 6,403; 7-27-2010 22:02:16; /db/atree/library/trees/ce/ce-q08x/src/
 */

