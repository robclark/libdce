/* 
 * Copyright (c) 2012, Texas Instruments Incorporated
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

/**
 *  @file       ti/xdais/ialg.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              defined by XDAIS for algorithms.
 */
/**
 *  @defgroup   ti_xdais_IALG   IALG - XDAIS Algorithm Interface
 *
 *  This is the XDAIS IALG interface.
 */

#ifndef ti_xdais_IALG_
#define ti_xdais_IALG_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_IALG */
/*@{*/

/*---------------------------*/
/*    TYPES AND CONSTANTS    */
/*---------------------------*/

#define IALG_DEFMEMRECS 4   /**< Default number of memory records. */
#define IALG_OBJMEMREC  0   /**< Memory record index of instance object. */
#define IALG_SYSCMD     256 /**< Minimum "system" IALG_Cmd value. */

#define IALG_EOK        0   /**< Successful return status code. */
#define IALG_EFAIL      (-1)  /**< Unspecified error return status code. */

#define IALG_CUSTOMFAILBASE  (-2048)/**< Algorithm-specific failure code end.
                                   *
                                   *   @remarks This is 0xfffff800.
                                   */
#define IALG_CUSTOMFAILEND (-256) /**< Algorithm-specific failure code base.
                                   *
                                   *   @remarks This is 0xffffff00.
                                   */

/**
 *  @brief      Memory attributes.
 */
typedef enum IALG_MemAttrs {
    IALG_SCRATCH,           /**< Scratch memory. */
    IALG_PERSIST,           /**< Persistent memory. */
    IALG_WRITEONCE          /**< Write-once persistent memory. */
} IALG_MemAttrs;

#define IALG_MPROG  0x0008  /**< Program memory space bit. */
#define IALG_MXTRN  0x0010  /**< External memory space bit. */

/**
 *  @brief      Defined memory spaces.
 */
/*
 *  ======== IALG_MemSpace ========
 */
typedef enum IALG_MemSpace {
    IALG_EPROG =            /**< External program memory */
        IALG_MPROG | IALG_MXTRN,

    IALG_IPROG =            /**< Internal program memory */
        IALG_MPROG,

    IALG_ESDATA =           /**< Off-chip data memory (accessed sequentially) */
        IALG_MXTRN + 0,

    IALG_EXTERNAL =         /**< Off-chip data memory (accessed randomly) */
        IALG_MXTRN + 1,

    IALG_DARAM0 = 0,        /**< Dual access on-chip data memory */
    IALG_DARAM1 = 1,        /**< Block 1, if independant blocks required */

    IALG_SARAM  = 2,        /**< Single access on-chip data memory */
    IALG_SARAM0 = 2,        /**< Block 0, equivalent to IALG_SARAM */
    IALG_SARAM1 = 3,        /**< Block 1, if independant blocks required */

    IALG_DARAM2 = 4,        /**< Block 2, if a 3rd independent block required */
    IALG_SARAM2 = 5         /**< Block 2, if a 3rd independent block required */
} IALG_MemSpace;

/*
 *  ======== IALG_isProg ========
 */
#define IALG_isProg(s) (        \
    (((int)(s)) & IALG_MPROG)   \
)

/*
 *  ======== IALG_isOffChip ========
 */
#define IALG_isOffChip(s) (     \
    (((int)(s)) & IALG_MXTRN)   \
)


/**
 *  @brief      Memory records.
 */
typedef struct IALG_MemRec {
    Uns             size;       /**< Size in MAU of allocation */
    Int             alignment;  /**< Alignment requirement (MAU) */
    IALG_MemSpace   space;      /**< Allocation space */
    IALG_MemAttrs   attrs;      /**< Memory attributes */
    Void            *base;      /**< Base address of allocated buf */
} IALG_MemRec;


/**
 *  @brief      Algorithm instance object definition.
 *
 *  @remarks    All XDAIS algorithm instance objects <b>must</b> have this
 *              structure as their first element.  However, they do not
 *              need to initialize it; initialization of this sub-structure
 *              is done by the "framework".
 */
typedef struct IALG_Obj {
    struct IALG_Fxns *fxns;     /**< Pointer to IALG function table. */
} IALG_Obj;


/**
 *  @brief      Handle to an algorithm instance object.
 */
typedef struct IALG_Obj *IALG_Handle;


/**
 *  @brief      Algorithm instance creation parameters.
 *
 *  @remarks    All XDAS algorithm parameter structures <b>must</b> have this
 *              as their first element.
 */
typedef struct IALG_Params {
    Int size;       /**< Number of MAU in the structure */
} IALG_Params;


/**
 *  @brief      Pointer to algorithm specific status structure.
 *
 *  @remarks    All XDAIS algorithm parameter structures <b>must</b> have this
 *              as their first element.
 */
typedef struct IALG_Status {
    Int size;       /**< Number of MAU in the structure */
} IALG_Status;


/**
 *  @brief      Algorithm specific command.
 *
 *  @remarks    This command is used in conjunction with IALG_Status to get
 *              and set algorithm specific attributes via the algControl()
 *               method.
 */
typedef unsigned int IALG_Cmd;


/**
 *  @brief      Defines the fields and methods that must be supplied by all
 *              XDAIS algorithms.
 */
/*
 *      algAlloc()        - apps call this to query the algorithm about
 *                          its memory requirements. Must be non-NULL.
 *      algControl()      - algorithm specific control operations.  May be
 *                          NULL; NULL => no operations supported.
 *      algDeactivate()   - notification that current instance is about to
 *                          be "deactivated".  May be NULL; NULL => do nothing.
 *      algFree()         - query algorithm for memory to free when removing
 *                          an instance.  Must be non-NULL.
 *      algInit()         - apps call this to allow the algorithm to
 *                          initialize memory requested via algAlloc().  Must
 *                          be non-NULL.
 *      algMoved()        - apps call this whenever an algorithms object or
 *                          any pointer parameters are moved in real-time.
 *                          May be NULL; NULL => object can not be moved.
 *      algNumAlloc()     - query algorithm for number of memory requests.
 *                          May be NULL; NULL => number of mem recs is less
 *                          then #IALG_DEFMEMRECS.
 */
typedef struct IALG_Fxns {
/**
 *  @brief      Unique pointer that identifies the module
 *              implementing this interface.
 */
    Void    *implementationId;

/**
 *  @brief      Notification to the algorithm that its memory
 *              is "active" and algorithm processing methods
 *              may be called.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *
 *  @remarks    algActivate() initializes any of the instance's scratch
 *              buffers using the persistent memory that is part of the
 *              algorithm's instance object.
 *
 *  @remarks    The implementation of algActivate() (and algDeactivate())
 *              is <b>required</b> if the algorithm requests memory of
 *              type #IALG_SCRATCH.
 *
 *  @remarks    The algActivate() method should only be implemented if
 *              a module wants to factor out initialization code that
 *              can be executed once prior to processing multiple
 *              consecutive frames of data.
 *
 *  @remarks    If a module does not implement this method, the algActivate()
 *              field in the module's static function table (of type
 *              IALG_Fxns) must be set to @c NULL.  This is equivalent to
 *              the following implementation:
 *  @code
 *      Void algActivate(IALG_Handle handle)
 *      {
 *      }
 *  @endcode
 *
 *  @pre        algActivate() can only be called after a successful return
 *              from algInit().
 *
 *  @pre        @c handle must be a valid handle for the algorithm's
 *              instance object.
 *
 *  @pre        No other algorithm method is currently being run on this
 *              instance. This method never preempts any other method on
 *              the same instance.
 *
 *  @pre        If the algorithm has implemented the IDMA2 interface,
 *              algActivate() can only be called after a successful return
 *              from dmaInit().
 *
 *  @post       All methods related to the algorithm may now be executed
 *              by client (subject to algorithm specific restrictions).
 *
 *  @sa         algDeactivate().
 */
    Void    (*algActivate)(IALG_Handle handle);

/**
 *  @brief      Apps call this to query the algorithm about
 *              its memory requirements.  Must be non-NULL.
 *
 *  @param[in]  params          Algorithm specific attributes.
 *  @param[out] parentFxns      Parent algorithm functions.
 *  @param[out] memTab          array of memory records.
 *
 *  @remarks    algAlloc() returns a table of memory records that
 *              describe the size, alignment, type and memory space of
 *              all buffers required by an algorithm (including the
 *              algorithm's instance object itself).  If successful,
 *              this function returns a positive non-zero value
 *              indicating the number of records initialized.  This
 *              function can never initialize more memory records than
 *              the number returned by algNumAlloc().
 *
 *  @remarks    If algNumAlloc() is not implemented, the maximum number
 *              of initialized memory records is #IALG_DEFMEMRECS.
 *
 *  @remarks    The first argument to algAlloc() is a pointer to the creation
 *              parameters for the instance of the algorithm object to be
 *              created.  This pointer is algorithm-specific; i.e., it points
 *              to a structure that is defined by each particular algorithm.
 *              This pointer may be @c NULL; however, in this case, algAlloc()
 *              must assume default creation parameters and must not fail.
 *
 *  @remarks    The second argument to algAlloc() is an optional parameter.
 *              algAlloc() may return a pointer to its parent's IALG functions.
 *              If this output value is assigned a non-NULL value, the client
 *              must create the parent instance object using the designated
 *              IALG functions pointer.  The parent instance object must then
 *              be passed to algInit().
 *
 *  @remarks    algAlloc() may be called at any time and it must be idempotent;
 *              i.e., it can be called repeatedly without any side effects,
 *              and always returns the same result.
 *
 *  @remarks    Algorithms are encouraged to return (and document!) clear,
 *              algorithm-specific error codes from algAlloc().  Create-time
 *              failures are a common integration issue, and the clearer a
 *              return value, the sooner the algorithm integrator can identify
 *              and resolve the issue.
 *
 *  @pre        The number of memory records in the array @c memTab[] is no
 *              less than the number returned by algNumAlloc().
 *
 *  @pre        @c *parentFxns is a valid pointer to an IALG_Fxns pointer
 *              variable.
 *
 *  @post       If the algorithm needs a parent object to be created, the
 *              pointer @c *parentFxns is set to a non-NULL value that points
 *              to a valid IALG_Fxns structure, the parent's IALG
 *              implementation.  Otherwise, this pointer is not set.  algAlloc()
 *              may elect to ignore the @c parentFxns pointer altogether.
 *
 *  @post       For each memory descriptor in memTab with an #IALG_WRITEONCE
 *              attribute, the algorithm has either set the base field to a
 *              non-NULL value, which is the address of a statically
 *              allocated and initialized memory buffer of the indicated
 *              'size', or has set the base field to @c NULL, thereby
 *              requiring the memory for the buffer to be provided by the
 *              client.
 *
 *  @post       Exactly @c n elements of the @c memTab[] array are initialized,
 *              where @c n is the return value from this operation.
 *
 *  @post       For each memory descriptor in @c memTab with an #IALG_PERSIST or
 *              #IALG_SCRATCH attribute, the algorithm does not set its base
 *              field.
 *
 *  @post       @c memTab[0] defines the memory required for the instance's
 *              object and this object's first field is an IALG_Obj structure.
 *
 *  @post       @c memTab[0] is requested as persistent memory.
 *
 *  @sa         algFree()
 */
    Int     (*algAlloc)(const IALG_Params *params,
        struct IALG_Fxns **parentFxns, IALG_MemRec *memTab);

/**
 *  @brief      Algorithm specific control and status.
 *
 *  @param[in]  handle          Algorithm instance handle.
 *  @param[in]  cmd             Algorithm specific command.
 *  @param[out] status          Algorithm specific status.
 *
 *  @remarks    algControl() sends an algorithm specific command, @c cmd, and
 *              an input/output status buffer pointer to an algorithm's
 *              instance object.
 *
 *  @remarks    In preemptive execution environments, algControl() may preempt
 *              a module's other metods (for example, its processing methods).
 *
 *  @remarks    The implementation of algControl() is optional.  If a module
 *              does not implement this method, the algControl() field in
 *              the module's static function table (of type IALG_Fxns) must
 *              be set to @c NULL.  This is equivalent to the following
 *              implementation:
 *  @code
 *      Void algControl(IALG_Handle handle, IALG_Cmd cmd, IALG_Status *status)
 *      {
 *          return (IALG_EFAIL);
 *      }
 *  @endcode
 *
 *  @pre        algControl() can only be called after a successful return
 *              from algInit().
 *
 *  @pre        @c handle must be a valid handle for the algorithm's instance
 *              object.
 *
 *  @pre        Algorithm specific @c cmd values are always less than
 *              #IALG_SYSCMD.
 *
 *  @post       If the @c cmd value is not recognized by the algorithm, the
 *              return value is not equal to IALG_EOK.
 *
 *  @sa         algInit()
 */
    Int     (*algControl)(IALG_Handle handle, IALG_Cmd cmd,
        IALG_Status *status);

/**
 *  @brief      Save all persistent data to non-scratch memory.
 *
 *  @param[in]  handle          Algorithm instance handle.
 *
 *  @remarks    algDeactivate() saves any persistent information to non-scratch
 *              buffers using the persistent memory that is part of the
 *              algorithm's instance object.
 *
 *  @remarks    @c handle is used by the algorithm to identify the various
 *              buffers that must be saved prior to the next cycle of
 *              algActivate() and processing.
 *
 *  @remarks    The implementation of algDeactivate() (and algActivate())
 *              is <b>required</b> if the algorithm requests memory of
 *              type #IALG_SCRATCH.
 *
 *  @remarks    The algDeactivate() method is only implemented if a
 *              module wants to factor out initialization code that
 *              can be executed once prior to processing multiple
 *              consecutive frames of data.
 *
 *  @remarks    If a module does not implement this method, the
 *              algDeactivate() field in the module's static function table
 *              (of type IALG_Fxns) must be set to @c NULL.  This is
 *              equivalent to the following implementation:
 *  @code
 *      Void algDeactivate(IALG_Handle handle)
 *      {
 *      }
 *  @endcode
 *
 *  @pre        algDeactivate() can only be called after a successful return
 *              from algInit().
 *
 *  @pre        The instance object is currently "active"; i.e., all instance
 *              memory is active and if an algActivate() method is defined,
 *              it has been called.
 *
 *  @pre        @c handle must be a valid handle for the algorithm's instance
 *              object.
 *
 *  @pre        No other algorithm method is currently being run on this
 *              instance.  This method never preempts any other method on the
 *              same instance.
 *
 *  @post       No methods related to the algorithm may now be executed by
 *              the client; only algActivate() or algFree() may be called.
 *
 *  @post       All instance scratch memory may be safely overwritten.
 *
 *  @sa         algActivate()
 */
    Void    (*algDeactivate)(IALG_Handle handle);

/**
 *  @brief      Apps call this to allow the algorithm to initialize memory
 *              requested via algAlloc().  Must be non-NULL.
 *
 *  @param[in]  handle          Algorithm instance handle.
 *  @param[out] memTab          Output array of memory records.
 *
 *  @remarks    algFree() returns a table of memory records that describe
 *              the base address, size, alignment, type and memory space
 *              of all buffers previously allocated for the algorithm's
 *              instance (including the algorithm's instance object itself)
 *              specified by @c handle.  This function always returns a
 *              positive non-zero value indicating the number of records
 *              initialized.  This function can never initialize more memory
 *              records than the value returned by algNumAlloc().
 *
 *  @pre        The @c memTab[] array contains at least algNumAlloc() records.
 *
 *  @pre        @c handle must be a valid handle for the algorithm's instance
 *              object.
 *
 *  @pre        If the prior call to algAlloc() returned a non-NULL parent
 *              functions pointer, then the parent instance must be an active
 *              instance object created via that function pointer.
 *
 *  @pre        No other agorithm method is currently being run on this
 *              instance.  This method never preempts any other method on the
 *              same instance.
 *
 *  @post       @c memTab[] contains pointers to all of the memory passed to
 *              the algorithm via algInit().
 *
 *  @post       The size and alignment fields contain the same values passed
 *              to the client via algAlloc(); i.e., if the client makes changes
 *              to the values returned via algAlloc() and passes these new
 *              values to algInit(), the algorithm is not responsible for
 *              retaining any such changes.
 *
 *  @sa         algAlloc()
 */
    Int     (*algFree)(IALG_Handle handle, IALG_MemRec *memTab);

/**
 *  @brief      Initialize an algorithm's instance object.  Must be non-NULL.
 *
 *  @param[in]  handle          Algorithm instance handle.  This is a pointer
 *                              to an initialized IALG_Obj structure.  Its
 *                              value is identical to the
 *                              <tt>memTab[0].base</tt>.
 *  @param[in]  memTab          Array of allocated buffers.
 *  @param[in]  parent          Handle of algorithm's parent instance.
 *  @param[in]  params          Pointer to algorithm's instance parameters.
 *
 *  @remarks    algInit() performs all initialization necessary to complete the
 *              run-time creation of an algorithm's instance object.  After a
 *              successful return from algInit(), the algorithm's instance
 *              object is ready to be used to process data.
 *
 *  @remarks    @c handle is a pointer to an initialized IALG_Obj structure.
 *              Its value is identical to the <tt>memTab[0].base</tt>.
 *
 *  @remarks    @c memTab is a table of memory records that describe the
 *              base address, size, alignment, type, and memory space
 *              of all buffers allocated for an algorithm instance
 *              (including the algorithm's instance object itself).  The
 *              number of initialized records is identical to the
 *              number returned by a prior call to algAlloc().
 *
 *  @remarks    @c parent is a handle to another algorithm instance object.
 *              This parameter is often NULL; indicating that no
 *              parent object exists.  This parameter allows clients
 *              to create a shared algorithm instance object and pass
 *              it to other algorithm instances.  For example, a
 *              parent instance object might contain global read-only
 *              tables that are used by several instances of a
 *              vocoder.
 *
 *  @remarks    @c params is a pointer to algorithm-specific parameters that
 *              are necessary for the creation and initialization of
 *              the instance object.  This pointer points to the same
 *              parameters passed to the algAlloc() operation.
 *              However, this pointer may be NULL.  In this case,
 *              algInit(), must assume default creation parameters.
 *
 *  @remarks    Algorithms are encouraged to return (and document!) clear,
 *              algorithm-specific error codes from algInit().  Create-time
 *              failures are a common integration issue, and the clearer a
 *              return value, the sooner the algorithm integrator can identify
 *              and resolve the issue.
 *
 *  @remarks    The algorithm <b>must not</b> access any scratch resources,
 *              memory or otherwise, until it has been activated (see
 *              algActivate()).  A common error is to initialize
 *              scratch resources during algInit().  This is an error
 *              as <i>another</i> algorithm, or instance of the same
 *              algorithm, may be active when this instance's
 *              algInit() is called.  This algorithm must not access
 *              scratch resources until it is active.
 *
 *  @remarks    The client is not required to satisfy the IALG_MemSpace
 *              attribute of the requested memory.  Note however that
 *              C6000 algorithms that use DMA may strictly require the
 *              client to satisfy its on-chip memory requirements and
 *              may not function correctly otherwise.
 *
 *  @remarks    The client may allocate the requested #IALG_WRITEONCE
 *              buffers once (or never, if the algorithm has assigned
 *              a base address in the prior call to algAlloc) and use
 *              the same buffers to initialize multiple instances of
 *              the same algorithm that are created with identical
 *              parameters.
 *
 *  @par Example:
 *
 *  @code
 *      typedef struct EncoderObj {
 *          IALG_Obj ialgObj;
 *          int workBuf;
 *          Int workBufLen;
 *          ... ;
 *      } EncoderObj;
 *
 *      Int algInit(IALG_Handle handle, IALG_MemRec memTab[],
 *              IALG_Handle parent, IALG_Params *params)
 *      {
 *          EncoderObj *inst = (EncoderObj *)handle;
 *          EncoderParams *encParams = (EncoderParams *)params;
 *
 *          if (encParams == NULL) {
 *              encParams = MYDEFAULTPARAMS;
 *          }
 *
 *          if (IALG_isOffChip(memTab[1].space)) {
 *              // unsupported off-chip memory, return documented error code
 *              return (MYMOD_MYVENDOR_EUNSUPPORTEDOFFCHIPWORKBUF);
 *          }
 *
 *          inst->workBuf = memTab[1].base;
 *          inst->workBufLen = encParams->frameDuration * 8;
 *          ...
 *
 *          return (IALG_EOK);
 *      }
 *  @endcode
 *
 *  @pre        @c memTab contains pointers to non-overlapping buffers with
 *              the size and alignment requested via a prior call to
 *              algAlloc().  In addition, the algorithm parameters,
 *              params, passed to algAlloc() are identical to those
 *              passed to this operation.
 *
 *  @pre        @c handle must be a valid handle for the algorithm's
 *              instance object; i.e., <tt>handle == memTab[0].base</tt> and
 *              @c handle->fxns is initialized to point to the
 *              appropriate IALG_Fxns structure.
 *
 *  @pre        The prior call to algAlloc() using same creation params must
 *              have returned value > 1.  This pre-condition ensures that any
 *              create-time parameter validation can be done once (in
 *              algAlloc()), and is not required to be done in algInit().
 *              Note that algInit() can, and should, validate any algorithm-
 *              specific requirements on resources received (e.g. internal
 *              memory, etc).
 *
 *  @pre        If the prior call to algAlloc() has not assigned a non-NULL
 *              base address to an #IALG_WRITEONCE @c memTab entry,
 *              the client must provide the memory resource with the
 *              requested size and alignment.
 *
 *  @pre        If the prior call to algAlloc() returned a non-NULL parent
 *              functions pointer, then the parent handle, @c parent,
 *              must be a valid handle to an instance object created
 *              via that function pointer.
 *
 *  @pre        No other algorithm method is currently being run on this
 *              instance.  This method never preempts any other method
 *              on the same instance.
 *
 *  @pre        If @c parent is non-NULL, no other method is currently being
 *              run on the parent instance; i.e., this method never
 *              preempts any other method on the parent instance.
 *
 *  @pre        If the prior call to algAlloc() assigned a non-NULL base
 *              address to an #IALG_WRITEONCE @c memTab[] entry, the
 *              client may pass back the same address in the base
 *              field without allocating additional memory for that
 *              persistent write-once buffer. Alternatively, the
 *              client may treat #IALG_WRITEONCE buffers in the same
 *              way as #IALG_PERSIST buffers; by allocating new memory
 *              and granting it to the algorithm using the base field.
 *
 *  @pre        The buffer pointed to in @c memTab[0] is initialized
 *              with a pointer to the appropriate IALG_Fxns structure
 *              followed by all 0s.
 *
 *  @post       With the exception of any initialization performed
 *              by algActivate() and any optional auxilary interface
 *              initialization functions (such as IRES and IDMA3),
 *              all of the instance's persistent and write-once memory is
 *              initialized and the object is ready to be used.
 *
 *  @post       All subsequent memory accesses to the #IALG_WRITEONCE
 *              buffers by this algorithm instance will be read-only.
 *
 *  @post       If the algorithm has implemented the IDMA2 interface,
 *              the dmaGetChannels() operation can be called.
 *
 *  @retval     #IALG_EOK       @copydoc IALG_EOK
 *  @retval     #IALG_EFAIL     @copydoc IALG_EFAIL
 *  @retval     "Custom error"  Algorithm-specific error - see
 *                              algorithm's documentation.
 *
 *  @sa         algAlloc()
 *  @sa         algMoved()
 */
    Int     (*algInit)(IALG_Handle handle, const IALG_MemRec *memTab,
        IALG_Handle parent, const IALG_Params *params);

/**
 *  @brief      Notify algorithm instance that instance memory has been
 *              relocated.
 *
 *  @param[in]  handle          Algorithm instance handle.
 *  @param[in]  memTab          Array of allocated buffers.
 *  @param[in]  parent          Handle of algorithm's parent instance.
 *  @param[in]  params          Pointer to algorithm's instance parameters.
 *
 *  @remarks    algMoved() performs any reinitialization necessary to insure
 *              that, if an algorithm's instance object has been moved by the
 *              client, all internal data references are recomputed.
 *
 *  @remarks    The implementation of algMoved() is optional.  However, it is
 *              highly recommended that this method be implemented.  If a
 *              module does not implement this method, the algMoved() field
 *              in the module's static function table (of type IALG_Fxns) must
 *              be set to @c NULL.  This is equivalent to asserting that the
 *              algorithm's instance objects cannot be moved.
 */
    Void    (*algMoved)(IALG_Handle handle, const IALG_MemRec *memTab,
        IALG_Handle parent, const IALG_Params *params);

/**
 *  @brief      Number of memory allocation requests required.
 *
 *  @remarks    algNumAlloc() returns the maximum number of memory allocation
 *              requests that the algAlloc() method requires.  This operation
 *              allows clients to allocate sufficient space to call the
 *              algAlloc() method or fail because insufficient space exists
 *              to support the creation of the algorithm's instance object.
 *              algNumAlloc() may be called at any time, and it must be
 *              idempotent; i.e., it can be called repeatedly without any
 *              side effects, and always returns the same result.
 *
 *  @remarks    algNumAlloc() is optional; if it is not implemented, the
 *              maximum number of memory records for algAlloc() is assumed
 *              to be #IALG_DEFMEMRECS.  This is equivalent to the following
 *              implementation:
 *  @code
 *      Void algNumAlloc(Void)
 *      {
 *          return (IALG_DEFNUMRECS);
 *      }
 *  @endcode
 *
 *  @remarks    If a module does not implement this method, the algNumAlloc()
 *              field in the module's static function table (of type IALG_Fxns)
 *              must be set to @c NULL.
 *
 *  @post       The return value from algNumAlloc() is always greater than
 *              or equal to one and always equals or exceeds the value
 *              returned by algAlloc().
 *
 *  @par Example:
 *
 *  @code
 *      #define NUMBUF 3
 *      extern IALG_Fxns *subAlg;
 *
 *      Int algNumAlloc(Void)
 *      {
 *          return (NUMBUF + subAlg->algNumAlloc());
 *      }
 *
 *      Int algAlloc(const IALG_Params *p, struct IALG_Fxns **pFxns,
 *              IALG_MemRec memTab)
 *      {
 *          Int n;
 *
 *          ...
 *
 *          n = subAlg->algAlloc(0, memTab);
 *          return (n + NUMBUF);
 *      }
 *  @endcode
 *
 *  @sa algAlloc()
 */
    Int     (*algNumAlloc)(Void);
} IALG_Fxns;

/*@}*/

#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_IALG_ */
/*
 *  @(#) ti.xdais; 1, 2.0, 1,1; 6-19-2012 17:57:45; /db/wtree/library/trees/dais/dais-w06/src/ xlibrary

 */

