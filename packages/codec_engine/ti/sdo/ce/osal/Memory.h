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
 *  ======== Memory.h ========
 */
/**
 *  @file       ti/sdo/ce/osal/Memory.h
 *
 *  @brief      The Codec Engine OSAL Memory interface.  Provides the user
 *              an OS-independent Memory abstraction.
 */
/**
 *  @defgroup   ti_sdo_ce_osal_Memory Memory - OS Independent Memory Interface
 *
 *  This Memory module provides services to manage memory in OS independent
 *  environments.
 *
 *  @hasPackagePrefix{ti_sdo_ce_osal,Memory,alloc,ti/sdo/ce/osal}
 */

#ifndef ti_sdo_ce_osal_Memory_
#define ti_sdo_ce_osal_Memory_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_sdo_ce_osal_Memory */
/*@{*/

/**
 *  @brief Default Alignment
 *
 *  @hasShortName{Memory_DEFAULTALIGNMENT}
 *
 *  @sa ti_sdo_ce_osal_Memory_AllocParams.align
 */
#define ti_sdo_ce_osal_Memory_DEFAULTALIGNMENT ((UInt)(-1))

/**
 * @brief       Name to pass to Diags_setMask() to enable logging for Memory
 *              functions. For example,
 *                  Diags_setMask(Memory_MODNAME"+EX1234567");
 *              turns on all Log statements in this module.
 *              Diags_setMask() must be called after initialization to take
 *              effect.
 */
#define Memory_MODNAME "ti.sdo.ce.osal.Memory"

/**
 *  @brief GT name containing the trace mask for this module
 *
 *  @hasShortName{Memory_GTNAME}
 */
#define ti_sdo_ce_osal_Memory_GTNAME "OM"

/** @cond INTERNAL */

/**
 *  @brief      Statistics structure for a memory section.
 *
 *  @hasShortName{Memory_Stat}
 *
 *  @remarks    Applicable to BIOS only.
 */
typedef struct ti_sdo_ce_osal_Memory_Stat {
    String name;     /**< Name of memory segment. */
    Uint32 base;     /**< Base of memory segment. */
    UInt size;       /**< Original size of segment. */
    UInt used;       /**< Number of bytes used in segment. */
    UInt length;     /**< Length of largest contiguous block. */
} ti_sdo_ce_osal_Memory_Stat;

/** @endcond */

/**
 *  @brief      Enum values for ti_sdo_ce_osal_Memory_AllocParams.type
 *
 *  @hasShortName{Memory_type}
 *
 *  @sa ti_sdo_ce_osal_Memory_AllocParams
 */
typedef enum {
    ti_sdo_ce_osal_Memory_MALLOC = 0,   /**< malloc()-based allocation
                                         *
                                         *  @hasShortName{Memory_MALLOC}
                                         */
    ti_sdo_ce_osal_Memory_SEG = 1,      /**< DSP/BIOS segment-based allocation
                                         *
                                         *  @hasShortName{Memory_SEG}
                                         */
    ti_sdo_ce_osal_Memory_CONTIGPOOL = 2, /**< Contiguous, pool-based allocation
                                         *
                                         *  @hasShortName{Memory_CONTIGPOOL}
                                         */
    ti_sdo_ce_osal_Memory_CONTIGHEAP =3 /**< Contiguous, heap-based allocation
                                         *
                                         *  @hasShortName{Memory_CONTIGHEAP}
                                         */
} ti_sdo_ce_osal_Memory_type;

/**
 * @brief       Parameters for ti_sdo_ce_osal_Memory_alloc() &
 *              ti_sdo_ce_osal_Memory_free()
 *
 * @hasShortName{Memory_AllocParams}
 *
 * @sa ti_sdo_ce_osal_Memory_DEFAULTPARAMS
 */
typedef struct ti_sdo_ce_osal_Memory_AllocParams {
    ti_sdo_ce_osal_Memory_type type;    /**< Type of allocation.
                         *
                         *   @sa ti_sdo_ce_osal_Memory_type
                         */
    UInt flags;         /**< Flags affecting allocation.
                         *
                         *   @sa ti_sdo_ce_osal_Memory_CACHED
                         *   @sa ti_sdo_ce_osal_Memory_NONCACHED
                         */
    UInt align;         /**< Alignment of allocation. */
    UInt seg;           /**< Segment for allocation. */
} ti_sdo_ce_osal_Memory_AllocParams;


/**
 *  @brief      Cached allocation
 *
 *  @hasShortName{Memory_CACHED}
 *
 *  @sa ti_sdo_ce_osal_Memory_AllocParams.flags
 */
#define ti_sdo_ce_osal_Memory_CACHED    0x00000000

/**
 *  @brief      Non-cached allocation
 *
 *  @hasShortName{Memory_NONCACHED}
 *
 *  @sa ti_sdo_ce_osal_Memory_AllocParams.flags
 */
#define ti_sdo_ce_osal_Memory_NONCACHED 0x00000001

/**
 *  @brief      Mask to isolate cache flag
 *
 *  @hasShortName{Memory_NONCACHEDMASK}
 *
 *  @sa ti_sdo_ce_osal_Memory_AllocParams.flags
 */
#define ti_sdo_ce_osal_Memory_CACHEDMASK 0x00000001

/**
 *  @brief   Default parameters for ti_sdo_ce_osal_Memory_alloc() &
 *           ti_sdo_ce_osal_Memory_free()
 *
 *  @hasShortName{Memory_DEFAULTPARAMS}
 *
 *  @remarks    ti_sdo_ce_osal_Memory_DEFAULTPARAMS will be used when NULL is
 *              passed in the ti_sdo_ce_osal_Memory_AllocParams parameter for
 *              ti_sdo_ce_osal_Memory_alloc() & ti_sdo_ce_osal_Memory_free().
 *              When passing a non-NULL ti_sdo_ce_osal_Memory_AllocParams
 *              parameter, you should first copy
 *              ti_sdo_ce_osal_Memory_DEFAULTPARAMS to your structure and then
 *              modify individual elements as needed:
 *
 * @code
 *      Memory_AllocParams  myParams;
 *
 *      myParams = Memory_DEFAULTPARAMS;
 *      myParams.type = Memory_CONTIGHEAP;
 * @endcode
 *
 * @sa      ti_sdo_ce_osal_Memory_alloc()
 * @sa      ti_sdo_ce_osal_Memory_free()
 */
extern ti_sdo_ce_osal_Memory_AllocParams ti_sdo_ce_osal_Memory_DEFAULTPARAMS;

/*
 *  ======== Memory_alloc ========
 */
/**
 *  @brief      General memory allocation.
 *
 *  @param[in]  size    Number of bytes to allocate.
 *  @param[in]  params  Parameters controlling the allocation.
 *
 *  @hasShortName{Memory_alloc}
 *
 *  @retval     NULL     The memory request failed.
 *  @retval     non-NULL The pointer to a buffer containing the requested
 *                       memory.
 *
 *  @sa         ti_sdo_ce_osal_Memory_free()
 *  @sa         ti_sdo_ce_osal_Memory_contigAlloc()
 */
extern Ptr ti_sdo_ce_osal_Memory_alloc(UInt size,
        ti_sdo_ce_osal_Memory_AllocParams *params);

/*
 *  ======== Memory_cacheInv ========
 */
/**
 *  @brief      Invalidate a range of cache.
 *
 *  @param[in]  addr            Address of the beginning of the buffer
 *                               to invalidate.
 *  @param[in]  sizeInBytes     Size of the buffer to invalidate.
 *
 *  @remarks    In addition to application usage, this is often called by
 *              a skeleton implementor after it has received a shared memory
 *              buffer from another processor, and before it invokes an
 *              algorithm's interface which will operate on that shared
 *              memory buffer.
 *
 *  @hasShortName{Memory_cacheInv}
 *
 *  @sa         ti_sdo_ce_osal_Memory_cacheWb()
 *  @sa         ti_sdo_ce_osal_Memory_cacheWbInv()
 */
extern Void ti_sdo_ce_osal_Memory_cacheInv(Ptr addr, Int sizeInBytes);


/*
 *  ======== Memory_cacheWb ========
 */
/**
 *  @brief      Write back cache.
 *
 *  @param[in]  addr            Address of the beginning of the buffer
 *                              to writeback.
 *  @param[in]  sizeInBytes     Size of the buffer to writeback.
 *
 *  @hasShortName{Memory_cacheWb}
 *
 *  @sa         ti_sdo_ce_osal_Memory_cacheInv()
 *  @sa         ti_sdo_ce_osal_Memory_cacheWbInv()
 */
extern Void ti_sdo_ce_osal_Memory_cacheWb(Ptr addr, Int sizeInBytes);


/*
 *  ======== Memory_cacheWbInv ========
 */
/**
 *  @brief      Write back and invalidate cache.
 *
 *  @param[in]  addr            Address of the beginning of the buffer
 *                              to writeback and invalidate.
 *  @param[in]  sizeInBytes     Size of the buffer to writeback invalidate.
 *
 *  @hasShortName{Memory_cacheWbInv}
 *
 *  @sa         ti_sdo_ce_osal_Memory_cacheInv()
 *  @sa         ti_sdo_ce_osal_Memory_cacheWb()
 */
extern Void ti_sdo_ce_osal_Memory_cacheWbInv(Ptr addr, Int sizeInBytes);


/*
 *  ======== Memory_contigAlloc ========
 */
/**
 *  @brief      Allocate physically contiguous blocks of memory.
 *
 *  @deprecated This function has been superceded by
 *              ti_sdo_ce_osal_Memory_alloc() and will be removed in a future
 *              release.  Users are recommended to modify their applications
 *              to use the new, more flexible
 *              ti_sdo_ce_osal_Memory_alloc() instead.
 *
 *  @param[in]  size    Size of the buffer to allocate.
 *  @param[in]  align   Alignment of the buffer; must be divisible by a power
 *                      of two.
 *
 *  @pre        @c align must be divisible by a power of two.
 *
 *  @hasShortName{Memory_contigAlloc}
 *
 *  @retval     non-NULL Address of a physically contiguous buffer.
 *  @retval     NULL    Unable to obtain the requested buffer.
 *
 *  @remarks    This is commonly called by an application wishing to obtain
 *              a physically contiguous buffer which it must share with
 *              another processor.
 *
 *  @remarks    For portability reasons, applications commonly use
 *              this API even in environments which do not have virtual memory
 *              or an MMU (e.g. a single processor, DSP/BIOS based system).
 *              Doing so makes them portable to environments which <i>do</i>
 *              have to provide physically contiguous memory to remote
 *              processors.
 *
 *  @sa         ti_sdo_ce_osal_Memory_alloc()
 *  @sa         ti_sdo_ce_osal_Memory_contigFree()
 */
extern Ptr ti_sdo_ce_osal_Memory_contigAlloc(UInt size, UInt align);


/*
 *  ======== Memory_contigFree ========
 */
/**
 *  @brief      Free memory allocated by ti_sdo_ce_osal_Memory_contigAlloc()
 *
 *  @param[in]  addr    Address of a buffer allocated by
 *                      ti_sdo_ce_osal_Memory_contigAlloc()
 *  @param[in]  size    Size of the buffer to free.
 *
 *  @hasShortName{Memory_contigFree}
 *
 *  @retval     TRUE    The buffer was freed and the memory pointed to by
 *                      @c addr is no longer valid.
 *  @retval     FALSE   The buffer couldn't be freed.
 *
 *  @pre        @c addr must be a valid address returned by
 *              ti_sdo_ce_osal_Memory_contigAlloc()
 *
 *  @pre        @c size must be equivalent to the @c size passed in during the
 *              ti_sdo_ce_osal_Memory_contigAlloc() allocation.
 *
 *  @sa         ti_sdo_ce_osal_Memory_contigAlloc()
 */
extern Bool ti_sdo_ce_osal_Memory_contigFree(Ptr addr, UInt size);



/*
 *  ======== Memory_free ========
 */
/**
 *  @brief      Free memory allocated with ti_sdo_ce_osal_Memory_alloc().
 *
 *  @param[in]  addr    Address of a buffer allocated by
 *                      ti_sdo_ce_osal_Memory_alloc().
 *  @param[in]  size    Size of the buffer to free.
 *  @param[in]  params  Parameters controlling the free operation.
 *
 *  @hasShortName{Memory_free}
 *
 *  @retval     TRUE    The buffer was freed and the memory pointed to by
 *                      @c addr is no longer valid.
 *  @retval     FALSE   The buffer couldn't be freed.
 *
 *  @pre        @c addr must be a valid address returned by
 *              ti_sdo_ce_osal_Memory_alloc().
 *
 *  @pre        @c size must be equivalent to the @c size passed in during the
 *              ti_sdo_ce_osal_Memory_alloc() allocation.
 *
 *  @pre        @c params must be equivalent to the @c params passed in during
 *              the ti_sdo_ce_osal_Memory_alloc() allocation.
 *
 *  @sa         ti_sdo_ce_osal_Memory_alloc()
 */
extern Bool ti_sdo_ce_osal_Memory_free(Ptr addr, UInt size,
        ti_sdo_ce_osal_Memory_AllocParams *params);


/*
 *  ======== Memory_getBufferPhysicalAddress ========
 */
/**
 *  @brief      Converts application virtual address to a physical address.
 *
 *  This API also checks verifies that the buffer is really contiguous.
 *
 *  @param[in]  virtualAddress  Address of a buffer.
 *  @param[in]  sizeInBytes     Size of the buffer.
 *  @param[out] isContiguous    Optional flag indicating whether the buffer
 *                              was physically contiguous or not.
 *
 *  @retval     0               Failure, the physical address could not
 *                              be obtained.
 *  @retval     non-zero        The physical address of the buffer.
 *
 *  @remarks    @c sizeInBytes must be non-zero.
 *
 *  @remarks    @c isContiguous is an optional parameter, and can be NULL if
 *              the caller doesn't want the results of this check.
 *              If @c isContiguous is NULL but the buffer is not contiguous,
 *              error trace will be generated.
 *
 *  @remarks    If a physically discontiguous buffer is provided in
 *              @c virtualAddress, zero will be returned.  And if
 *              @c isContiguous is non-null, it will be set to FALSE.
 *
 *  @remarks    This is typically called by a stub implementation to
 *              translate an application-side virtual address into a physical
 *              address usable by a remote skeleton and algorithm.
 *
 *  @remarks    To enable portable code, this is safe to call on systems
 *              without virtual memory.  In those cases, the returned
 *              physical buffer will be the same as the provided
 *              "virtual" buffer.
 *
 *  @hasShortName{Memory_getBufferPhysicalAddress}
 *
 *  @sa         ti_sdo_ce_osal_Memory_getBufferVirtualAddress()
 */
extern UInt32 ti_sdo_ce_osal_Memory_getBufferPhysicalAddress(Ptr virtualAddress,
    Int sizeInBytes, Bool *isContiguous);

/*
 *  ======== Memory_getBufferVirtualAddress ========
 */
/**
 *  @brief      Obtains the virtual address of a physically contiguous
 *              buffer.
 *
 *  @param[in]  physicalAddress Physical address of a buffer.
 *  @param[in]  sizeInBytes     Size of the buffer.
 *
 *  @retval     NULL            Failure, the virtual address could not
 *                              be obtained.
 *  @retval     non-zero        The virtual address of the buffer.
 *
 *  @remarks    @c sizeInBytes must be non-zero.
 *
 *  @remarks    If @c virtualAddress, was not acquired by
 *              ti_sdo_ce_osal_Memory_getBufferPhysicalAddress(), no attempt is
 *              made to map the physically contiguous buffer into the
 *              application's virtual memory space.
 *
 *  @remarks    This is typically called by a stub implementation to
 *              translate a buffer provided by a remote skeleton and/or
 *              algorithm into an application-usable, virtual address.
 *
 *  @remarks    On some homogenous multicore devices (e.g. C6472), this
 *              service translates a local address to a global address
 *              that's visible by a different processor.
 *
 *  @remarks    To enable portable code, this is safe to call on systems
 *              without virtual memory.  In those cases, the returned
 *              "virtual" buffer will be the same as the provided
 *              physical buffer.
 *
 *  @hasShortName{Memory_getBufferVirtualAddress}
 *
 *  @sa         ti_sdo_ce_osal_Memory_getBufferPhysicalAddress()
 */
extern Ptr ti_sdo_ce_osal_Memory_getBufferVirtualAddress(UInt32 physicalAddress,
    Int sizeInBytes);

/** @cond INTERNAL */

/*
 *  ======== Memory_init ========
 */
extern Bool ti_sdo_ce_osal_Memory_init(Void);


/*
 *  ======== Memory_exit ========
 */
extern Void ti_sdo_ce_osal_Memory_exit(Void);

/** @endcond */


/** @cond INTERNAL */

/*
 *  ======== Memory_segAlloc ========
 */
/**
 *  @brief      Allocate memory from a specific segment.
 *
 *  @privateFxn
 *
 *  @param[in]  segId   Segment ID to allocate the buffer from.
 *  @param[in]  size    Size of the buffer to allocate.
 *  @param[in]  align   Alignment of the buffer, must be divisible by a power
 *                      of two.
 *
 *  @retval     non-NULL Address of a physically contiguous buffer.
 *  @retval     NULL    Unable to obtain the requested buffer.
 *
 *  @hasShortName{Memory_segAlloc}
 *
 *  @sa         ti_sdo_ce_osal_Memory_segFree()
 *  @sa         ti_sdo_ce_osal_Memory_segStat()
 *  @sa         ti_sdo_ce_osal_Memory_alloc()
 *  @sa         ti_sdo_ce_osal_Memory_contigAlloc()
 */
extern Ptr ti_sdo_ce_osal_Memory_segAlloc(Int segId, UInt size, UInt align);


/*
 *  ======== Memory_segFree ========
 */
/**
 *  @brief      Free memory allocated with ti_sdo_ce_osal_Memory_segAlloc().
 *
 *  @privateFxn
 *
 *  @param[in]  segId   Segment the buffer to free was allocated from.
 *  @param[in]  addr    Address of a buffer allocated by
 *                      ti_sdo_ce_osal_Memory_segAlloc().
 *  @param[in]  size    Size of the buffer to free.
 *
 *  @retval     TRUE    The buffer was freed and the memory pointed to by
 *                      @c addr is no longer valid.
 *  @retval     FALSE   The buffer couldn't be freed.
 *
 *  @pre        @c segId must be equivalent to the @c segid passed in during the
 *              ti_sdo_ce_osal_Memory_segAlloc() allocation.
 *
 *  @pre        @c addr must be a valid address returned by
 *              ti_sdo_ce_osal_Memory_segAlloc().
 *
 *  @pre        @c size must be equivalent to the @c size passed in during the
 *              ti_sdo_ce_osal_Memory_segAlloc() allocation.
 *
 *  @hasShortName{Memory_segFree}
 *
 *  @sa         ti_sdo_ce_osal_Memory_segAlloc()
 *  @sa         ti_sdo_ce_osal_Memory_segStat()
 */
extern Bool ti_sdo_ce_osal_Memory_segFree(Int segId, Ptr addr, UInt size);


/*
 *  ======== Memory_segStat ========
 */
/**
 *  @brief      Obtain the memory usage statistics of a given memory segment
 *              allocator.
 *
 *  @privateFxn
 *
 *  @param[in]  segId   Segment ID to obtain statistics information for.
 *  @param[out] statbuf Buffer to fill with statistics data.
 *
 *  @retval     TRUE    The statistics were captured, and @c statbuf has been
 *                      filled.
 *  @retval     FALSE   The statistics could not be obtained.
 *
 *  @pre        @c statbuf must be a pointer to memory of size
 *              <tt>sizeof(#ti_sdo_ce_osal_Memory_Stat)</tt>.
 *
 *  @hasShortName{Memory_setStat}
 *
 *  @sa         ti_sdo_ce_osal_Memory_segAlloc()
 *  @sa         ti_sdo_ce_osal_Memory_segFree()
 */
extern Bool ti_sdo_ce_osal_Memory_segStat(Int segId,
        ti_sdo_ce_osal_Memory_Stat *statbuf);

/** @endcond */

/** @cond INTERNAL */

/*
 * ======== PREFIX ALIASES ========
 * Same strategy as XDC uses
 */
#if !defined(__nested__) && !defined(ti_sdo_ce_osal_Memory__nolocalnames)

/* module prefix */
#define Memory_DEFAULTALIGNMENT ti_sdo_ce_osal_Memory_DEFAULTALIGNMENT
#define Memory_GTNAME ti_sdo_ce_osal_Memory_GTNAME
#define Memory_Stat ti_sdo_ce_osal_Memory_Stat
#define Memory_type ti_sdo_ce_osal_Memory_type
#define Memory_MALLOC ti_sdo_ce_osal_Memory_MALLOC
#define Memory_SEG ti_sdo_ce_osal_Memory_SEG
#define Memory_CONTIGPOOL ti_sdo_ce_osal_Memory_CONTIGPOOL
#define Memory_CONTIGHEAP ti_sdo_ce_osal_Memory_CONTIGHEAP
#define Memory_AllocParams ti_sdo_ce_osal_Memory_AllocParams
#define Memory_CACHED ti_sdo_ce_osal_Memory_CACHED
#define Memory_NONCACHED ti_sdo_ce_osal_Memory_NONCACHED
#define Memory_CACHEDMASK ti_sdo_ce_osal_Memory_CACHEDMASK
#define Memory_DEFAULTPARAMS ti_sdo_ce_osal_Memory_DEFAULTPARAMS

#define Memory_alloc ti_sdo_ce_osal_Memory_alloc
#define Memory_cacheInv ti_sdo_ce_osal_Memory_cacheInv
#define Memory_cacheWb ti_sdo_ce_osal_Memory_cacheWb
#define Memory_cacheWbInv ti_sdo_ce_osal_Memory_cacheWbInv
#define Memory_contigAlloc ti_sdo_ce_osal_Memory_contigAlloc
#define Memory_contigFree ti_sdo_ce_osal_Memory_contigFree
#define Memory_free ti_sdo_ce_osal_Memory_free
#define Memory_getBufferPhysicalAddress ti_sdo_ce_osal_Memory_getBufferPhysicalAddress
#define Memory_getBufferVirtualAddress ti_sdo_ce_osal_Memory_getBufferVirtualAddress
#define Memory_init ti_sdo_ce_osal_Memory_init
#define Memory_exit ti_sdo_ce_osal_Memory_exit
#define Memory_segAlloc ti_sdo_ce_osal_Memory_segAlloc
#define Memory_segFree ti_sdo_ce_osal_Memory_segFree
#define Memory_segStat ti_sdo_ce_osal_Memory_segStat
#endif

/** @endcond */

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce.osal; 2, 0, 2,395; 7-27-2010 22:15:24; /db/atree/library/trees/ce/ce-q08x/src/
 */

