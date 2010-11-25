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

/**
 *  @file       ti/xdais/dm/xdm.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared across the various XDM classes of algorithms.
 */
/**
 *  @addtogroup   ti_xdais_dm_XDM       XDM - Shared XDM Definitions
 *
 *  This is the XDM interface.
 */

#ifndef ti_xdais_dm_XDM_
#define ti_xdais_dm_XDM_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_XDM */
/*@{*/

#define XDM_EOK                 IALG_EOK    /**< Success. */
#define XDM_EFAIL               IALG_EFAIL  /**< General failure. */
#define XDM_EUNSUPPORTED        -3          /**< Request is unsupported. */


#ifdef XDM_INCLUDE_DOT9_SUPPORT
/**
 *  @brief      General runtime failure.
 *
 *  @deprecated This is only supported on 0.9 XDM.  To use it, you must
 *              define "XDM_INCLUDE_DOT9_SUPPORT".
 *              In XDM 1.00+, it is required that codecs return "EFAIL", as
 *              "ERUNTIME" is not supported.
 */
#define XDM_ERUNTIME            -2
#endif

#define XDM_MAX_IO_BUFFERS      16          /**< Max I/O Buffers */

/**
 *  @brief      Buffer descriptor for multiple buffers.
 *
 *  @dot
 *  digraph example {
 *    rankdir=LR;
 *    node [shape=record];
 *    XDM_BufDesc [ style=filled, fillcolor=gray98, label="<bufs> XDAS_Int8 **bufs | <numBufs> XDAS_Int32 numBufs | <bufSizes> XDAIS_Int32 *bufSizes"];
 *    bufArray [ label="<f0> ptr to buf 0 |<f1> ptr to buf 1|<f2> ptr to buf 2|.\n.\n.\n" ];
 *    buf0 [ label="<f0> data buf 0" ];
 *    buf1 [ label="<f0> data buf 1" ];
 *    buf2 [ label="<f0> data buf 2" ];
 *    bufSizes [ label="<f0> size of data buf 0 | size of data buf 1 | size of data buf 2|.\n.\n.\n" ];
 *    XDM_BufDesc:bufs -> bufArray:f0;
 *    bufArray:f0 -> buf0:f0;
 *    bufArray:f1 -> buf1:f0;
 *    bufArray:f2 -> buf2:f0;
 *    XDM_BufDesc:bufSizes -> bufSizes:f0;
 *  }
 *  @enddot
 *
 *  @pre        @c numBufs can not be larger than #XDM_MAX_IO_BUFFERS.  Related,
 *              @c *bufs and @c *bufSizes will never be indexed beyond
 *              #XDM_MAX_IO_BUFFERS elements.
 *
 *  @remarks    This data type is commonly used to manage input and output
 *              buffers.
 *
 *  @remarks    If @c *bufs is a sparse array, @c *bufSizes will be a similar
 *              sparse array.  The @c NULL indexes in @c bufs will be ignored
 *              in @c bufSizes.
 *
 *  @remarks    @c numBufs describes the number of buffers in this descriptor.
 *              if @c *bufs is a sparse array, @c numBufs describes
 *              the number of non-NULL buffers in this descriptor;
 *              this is not necessarily the maximum index of the last
 *              buffer pointed to by @c *bufs.
 *
 *  @remarks    An example utilizing XDM_BufDesc as a sparse array would be
 *              the following:
 *  @code
 *              XDM_BufDesc outBufs;
 *              XDAS_Int32  bufSizeArray[XDM_MAX_IO_BUFFERS];
 *              XDAS_Int8  *pBuffers[XDM_MAX_IO_BUFFERS];
 *              XDAS_Int8   buffer1[4096];
 *              XDAS_Int8   buffer2[1024];
 *
 *              // ensure all pBuffers and bufSizeArray are initially NULL
 *              memset(pBuffers, 0, sizeof(pBuffers[0]) * XDM_MAX_IO_BUFFERS);
 *              memset(bufSizeArray, 0,
 *                  sizeof(bufSizeArray[0]) * XDM_MAX_IO_BUFFERS);
 *
 *              pBuffers[0] = buffer1;
 *              pBuffers[4] = buffer2;
 *
 *              bufSizeArray[0] = 4096;
 *              bufSizeArray[4] = 1024;
 *
 *              outBufs.bufs = pBuffers;
 *              outBufs.numBufs = 2;
 *              outBufs.bufSizes = bufSizeArray;
 *  @endcode
 *
 *  @remarks    The following diagram describes graphically the example above.
 *
 *  @dot
 *  digraph example {
 *    rankdir=LR;
 *    node [shape=record];
 *    XDM_BufDesc [ style=filled, fillcolor=gray98, label="<bufs> bufs = pBuffers | <numBufs> numBufs = 2 | <bufSizes> bufSizes = bufSizeArray"];
 *    bufArray [ label="<f0> pBuffers[0] |<f1> NULL |<f2> NULL|<f3> NULL|<f4> pBuffers[4]|NULL|NULL|.\n.\n.\n" ];
 *    buf0 [ label="<f0> buffer1" ];
 *    buf4 [ label="<f0> buffer2" ];
 *    bufSizes [ label="<f0> 4096|0|0|0|1024|0|0| .\n.\n.\n" ];
 *    XDM_BufDesc:bufs -> bufArray:f0;
 *    bufArray:f0 -> buf0:f0;
 *    bufArray:f4 -> buf4:f0;
 *    XDM_BufDesc:bufSizes -> bufSizes:f0;
 *  }
 *  @enddot
 *
 */
typedef struct XDM_BufDesc {
    XDAS_Int8   **bufs;     /**< Pointer to an array containing buffer
                             *   addresses.
                             */
    XDAS_Int32   numBufs;   /**< Number of buffers. */
    XDAS_Int32  *bufSizes;  /**< Size of each buffer in 8-bit bytes. */
} XDM_BufDesc;


/**
 *  @brief      Single buffer descriptor.
 */
typedef struct XDM_SingleBufDesc {
    XDAS_Int8   *buf;       /**< Pointer to a buffer address. */
    XDAS_Int32  bufSize;    /**< Size of @c buf in 8-bit bytes. */
} XDM_SingleBufDesc;



/**
 *  @brief      Single buffer descriptor.
 */
typedef struct XDM1_SingleBufDesc {
    XDAS_Int8   *buf;       /**< Pointer to a buffer address. */
    XDAS_Int32  bufSize;    /**< Size of @c buf in 8-bit bytes. */
    XDAS_Int32  accessMask; /**< Mask filled by the algorithm, declaring
                             *   how the buffer was accessed <b>by the
                             *   algorithm processor</b>.
                             *
                             *   @remarks  If the buffer was <b>not</b>
                             *             accessed by the algorithm
                             *             processor (e.g., it was filled
                             *             via DMA or other hardware
                             *             accelerator that <i>doesn't</i>
                             *             write through the algorithm's
                             *             CPU), then no bits in this mask
                             *             should be set.
                             *
                             *   @remarks  It is acceptible (and
                             *             appropriate!)to set several
                             *             bits in this mask if the
                             *             algorithm accessed the buffer
                             *             in several ways.
                             *
                             *   @remarks  This mask is often used by the
                             *             application and/or framework
                             *             to appropriately manage cache
                             *             on cache-based systems.
                             *
                             *   @sa XDM_AccessMode
                             */
} XDM1_SingleBufDesc;


/**
 *  @brief      Union describing a buffer size
 *
 *  @remarks    More advanced memory types (e.g. tiled memory) cannot
 *              indicate buffer size using a simple integer, but rather
 *              must indicate memory size using a width and height.
 */
typedef union  {
    struct {
        XDAS_Int32 width;   /**< Width of @c buf in 8-bit bytes. */
        XDAS_Int32 height;  /**< Height of @c buf in 8-bit bytes. */
    } tileMem;
    XDAS_Int32 bytes;       /**< Size of @c buf in 8-bit bytes. */
} XDM2_BufSize;

/**
 *  @brief      Single buffer descriptor
 *
 *  @remarks    This buffer descriptor contains a @c memType field, and uses
 *              the XDM_BufSize union to indicate the size of the buffer.
 *              As a result, this data type can be used to describe
 *              complex memory types (e.g. tiled memory).
 */
typedef struct XDM2_SingleBufDesc {
    XDAS_Int8   *buf;       /**< Pointer to a buffer address. */
    XDAS_Int16  memType;    /**< Memory type.
                             *
                             *   @sa XDM_MemoryType
                             */
    XDAS_Int16 usageMode;   /**< Memory usage descriptor.
                             *
                             *   @remarks   This field is set by the owner
                             *              of the buffer (typically the
                             *              application), and read by users
                             *              of the buffer (including the
                             *              algorithm).
                             *
                             *   @sa XDM_MemoryUsageMode
                             */
    XDM2_BufSize bufSize;   /**< Buffer size(for tile memory/row memory */
    XDAS_Int32  accessMask; /**< Mask filled by the algorithm, declaring
                             *   how the buffer was accessed <b>by the
                             *   algorithm processor</b>.
                             *
                             *   @remarks  If the buffer was <b>not</b>
                             *             accessed by the algorithm
                             *             processor (e.g., it was filled
                             *             via DMA or other hardware
                             *             accelerator that <i>doesn't</i>
                             *             write through the algorithm's
                             *             CPU), then no bits in this mask
                             *             should be set.
                             *
                             *   @remarks  It is acceptible (and
                             *             appropriate!)to set several
                             *             bits in this mask if the
                             *             algorithm accessed the buffer
                             *             in several ways.
                             *
                             *   @remarks  This mask is often used by the
                             *             application and/or framework
                             *             to appropriately manage cache
                             *             on cache-based systems.
                             *
                             *   @sa XDM_AccessMode
                             */
} XDM2_SingleBufDesc;


/**
 *  @brief      Buffer descriptor.
 */
typedef struct XDM1_BufDesc {
    XDAS_Int32   numBufs;   /**< Number of buffers in @c descs array.
                             *
                             *   @remarks  Must be less than
                             *             #XDM_MAX_IO_BUFFERS.
                             */
    XDM1_SingleBufDesc descs[XDM_MAX_IO_BUFFERS]; /** Array of buffer
                             * descriptors.
                             */
} XDM1_BufDesc;

/**
 *  @brief      Buffer descriptor
 *
 *  @remarks    This advanced buffer uses the XDM2_SingleBufDesc, and is
 *              typically used for codecs which must reflect <i>types</i>
 *              of memory.  For example, video codecs may need to indicate
 *              whether tiled memory is used.
 */
typedef struct XDM2_BufDesc {
    XDAS_Int32   numBufs;   /**< Number of buffers in @c descs array.
                             *
                             *   @remarks  Must be less than
                             *             #XDM_MAX_IO_BUFFERS.
                             */
    XDM2_SingleBufDesc descs[XDM_MAX_IO_BUFFERS]; /** Array of buffer
                             * descriptors.
                             */
} XDM2_BufDesc;


/**
 *  @brief      Access modes used to declare how the algorithm accessed buffers.
 *
 *  @remarks    This indicates how the algorithm's <b>CPU</b> accessed the
 *              buffer, independent of DMA or other hardware accellerators.
 *              For example, if the buffer was written to with DMA (as
 *              opposed to writing to the buffer with the CPU write
 *              instructions), the algorithm should <b>not</b> set the
 *              XDM_ACCESSMODE_WRITE bit.
 *
 *  @remarks    The value of the enum is the bit offset into a mask.  The value
 *              of the enum is not the value to assign the mask.
 *
 *
 *  @enumWarning
 *
 *  @sa         XDM1_SingleBufDesc
 *  @sa         XDM1_BufDesc
 */
typedef enum {
    XDM_ACCESSMODE_READ = 0,      /**< The algorithm <i>read</i> from the
                                   *   buffer using the CPU.
                                   *
                                   *   @sa      XDM_SETACCESSMODE_READ
                                   *   @sa      XDM_ISACCESSMODE_READ
                                   */
    XDM_ACCESSMODE_WRITE = 1      /**< The algorithm <i>wrote</i> to the
                                   *   buffer using the CPU.
                                   *
                                   *   @sa      XDM_SETACCESSMODE_WRITE
                                   *   @sa      XDM_ISACCESSMODE_WRITE
                                   */
} XDM_AccessMode;


/**
 *  @brief      Check an access mask for CPU read access.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an application.
 *
 *  @sa         XDM1_SingleBufDesc::accessMask
 *  @sa         XDM_ISACCESSMODE_WRITE
 */
#define XDM_ISACCESSMODE_READ(x)    (((x) >> XDM_ACCESSMODE_READ) & 0x1)


/**
 *  @brief      Check an access mask for CPU write access.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an application.
 *
 *  @sa         XDM1_SingleBufDesc::accessMask
 *  @sa         XDM_ISACCESSMODE_READ
 */
#define XDM_ISACCESSMODE_WRITE(x)   (((x) >> XDM_ACCESSMODE_WRITE) & 0x1)


/**
 *  @brief      Clear the "CPU read access" bit in an access mask.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an algorithm.
 *
 *  @sa         XDM_SETACCESSMODE_READ
 *  @sa         XDM1_SingleBufDesc::accessMask
 */
#define XDM_CLEARACCESSMODE_READ(x)   ((x) &= (~(0x1 << XDM_ACCESSMODE_READ)))


/**
 *  @brief      Clear the "CPU write access" bit in an access mask.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an algorithm.
 *
 *  @sa         XDM_SETACCESSMODE_WRITE
 *  @sa         XDM1_SingleBufDesc::accessMask
 */
#define XDM_CLEARACCESSMODE_WRITE(x)   ((x) &= (~(0x1 << XDM_ACCESSMODE_WRITE)))


/**
 *  @brief      Set the bit to indicate CPU read access in an access mask.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an algorithm.
 *
 *  @sa         XDM1_SingleBufDesc::accessMask
 */
#define XDM_SETACCESSMODE_READ(x)   ((x) |= (0x1 << XDM_ACCESSMODE_READ))


/**
 *  @brief      Set the bit to indicate CPU write access in an access mask.
 *
 *  @param      x       access mask.
 *
 *  @remarks    This is typically used by an algorithm.
 *
 *  @sa         XDM1_SingleBufDesc::accessMask
 */
#define XDM_SETACCESSMODE_WRITE(x)  ((x) |= (0x1 << XDM_ACCESSMODE_WRITE))


/**
 *  @brief      Buffer information descriptor for input and output buffers.
 */
typedef struct XDM1_AlgBufInfo {
    XDAS_Int32 minNumInBufs;       /**< Minimum number of input buffers. */
    XDAS_Int32 minNumOutBufs;      /**< Minimum number of output buffers. */
    XDM2_BufSize minInBufSize[XDM_MAX_IO_BUFFERS];  /**< Minimum size required
                                    * for each input buffer.
                                    */
    XDM2_BufSize minOutBufSize[XDM_MAX_IO_BUFFERS]; /**< Minimum size required
                                    * for each output buffer.
                                    */
    XDAS_Int32 inBufMemoryType[XDM_MAX_IO_BUFFERS]; /**< Required memory type
                                    * for each input buffer.
                                    *
                                    *   @sa XDM_MemoryType
                                    */
    XDAS_Int32 outBufMemoryType[XDM_MAX_IO_BUFFERS]; /**< Required memory type
                                    * for each output buffer.
                                    *
                                    *   @sa XDM_MemoryType
                                    */
    XDAS_Int32 minNumBufSets;      /**< Minimum number of buffer sets for
                                    *   buffer management.
                                    *
                                    *   @todo  need more details
                                    */
} XDM1_AlgBufInfo;


/**
 *  @brief      Buffer information descriptor for input and output buffers.
 */
typedef struct XDM_AlgBufInfo {
    XDAS_Int32 minNumInBufs;       /**< Minimum number of input buffers. */
    XDAS_Int32 minNumOutBufs;      /**< Minimum number of output buffers. */
    XDAS_Int32 minInBufSize[XDM_MAX_IO_BUFFERS];  /**< Minimum size, in 8-bit
                                    * bytes, required for each input buffer.
                                    */
    XDAS_Int32 minOutBufSize[XDM_MAX_IO_BUFFERS]; /**< Minimum size, in 8-bit
                                    * bytes, required for each output buffer.
                                    */
} XDM_AlgBufInfo;


/**
 *  @brief      Base of algorithm-specific enum values
 *
 *  @remarks    This is provided to ensure that future updates to XDM-defined
 *              enumerations don't conflict with algorithm-proprietary
 *              enumerations.
 *
 *  @remarks    Custom enumerations should be defined like the following
 *              (@c USERENUM0 and @c USERENUM1 are simply examples):
 *  @code
 *  #define MYMODULE_MYVENDOR_USERENUM0 (XDM_CUSTOMENUMBASE + 0)
 *  #define MYMODULE_MYVENDOR_USERENUM1 (XDM_CUSTOMENUMBASE + 1)
 *  @endcode
 */
#define XDM_CUSTOMENUMBASE 0x100


/**
 *  @brief      Base of algorithm-specific commands
 *
 *  @remarks    This is provided to ensure that future updates to XDM_CmdId's
 *              enumeration don't conflict with algorithm-proprietary
 *              command ID's.
 *
 *  @remarks    Custom command ID's should be defined like the following
 *              (@c USERCMD0 and @c USERCMD1 are simply examples):
 *  @code
 *  #define MYMODULE_MYVENDOR_USERCMD0 (XDM_CUSTOMCMDBASE + 0)
 *  #define MYMODULE_MYVENDOR_USERCMD1 (XDM_CUSTOMCMDBASE + 1)
 *  @endcode
 *
 *  @sa XDM_CmdId
 */
#define XDM_CUSTOMCMDBASE 0x100

/**
 *  @brief      Standard control commands that must be implemented by
 *              XDM compliant multimedia algorithms.
 *
 *  @remarks    If an algorithm receives a command it doesn't handle or
 *              understand, it must return EUNSUPPORTED.
 *
 *  @remarks    XDM_GETCONTEXTINFO need only be implemented by split codecs.
 *              Standard algorithms should return EUNSUPPORTED if they receive
 *              the XDM_GETCONTEXTINFO command.
 *
 *  @remarks    Any control ID extension in IMOD interface should start
 *              from XDM_CUSTOMCMDBASE onward.  The ID range from 0 to
 *              XDM_CUSTOMCMDBASE is reserved.
 *
 *  @enumWarning
 *
 *  @sa XDM_CUSTOMCMDBASE
 */
typedef enum {
    XDM_GETSTATUS = 0,      /**< Query algorithm to fill status structure.
                             *
                             *   @remarks   Some XDM interfaces provide an
                             *              embedded "DynamicParams" struct in
                             *              the base class "Status" struct in
                             *              which the current state of an
                             *              algorithm's dynamic params can be
                             *              returned (e.g.
                             *              #IVIDENC2_Status.encDynamicParams);
                             *              other codec classes do not (and
                             *              algs could provide this via
                             *              an extended Status struct).
                             */
    XDM_SETPARAMS = 1,      /**< Set run time dynamic parameters. */
    XDM_RESET = 2,          /**< Reset the algorithm.  All fields in the
                             *   internal data structures are reset and all
                             *   internal buffers are flushed.
                             */
    XDM_SETDEFAULT = 3,     /**< Restore the algorithm's internal state
                             *   to its original, default values.
                             *
                             *   @remarks       The application only needs
                             *                  to initialize the
                             *                  @c dynamicParams.size and
                             *                  @c status.size fields
                             *                  prior to calling control()
                             *                  with XDM_SETDEFAULT.
                             *
                             *   @remarks       The algorithm must only
                             *                  write to the
                             *                  @c status.extendedError field,
                             *                  and potentially algorithm
                             *                  specific, extended fields.
                             *
                             *   @remarks       XDM_SETDEFAULT differs from
                             *                  XDM_RESET.  In addition to
                             *                  restoring the algorithm's
                             *                  internal state, XDM_RESET
                             *                  additionally resets any
                             *                  channel related state.
                             */
    XDM_FLUSH = 4,          /**< Handle end of stream conditions.  This
                             *   command forces the algorithm to output
                             *   data without additional input.  The
                             *   recommended sequence is to call the
                             *   control() function (with XDM_FLUSH)
                             *   followed by repeated calls to the
                             *   process() function until it returns an
                             *   error.
                             *
                             *   @remarks    The algorithm should return
                             *               the appropriate, class-specific
                             *               "EFAIL" error (e.g.
                             *               ISPHDEC1_EFAIL, IVIDENC1_EFAIL,
                             *               etc), when flushing is
                             *               complete.
                             */
    XDM_GETBUFINFO = 5,     /**< Query algorithm instance regarding its
                             *   properties of input and output
                             *   buffers.
                             *
                             *   @remarks   The application only needs
                             *              to initialize the
                             *              @c dynamicParams.size, the
                             *              @c status.size, and set
                             *              any buffer descriptor fields
                             *              (e.g. @c status.data) to
                             *              @c NULL prior to calling
                             *              control() with XDM_GETBUFINFO.
                             *
                             */
    XDM_GETVERSION = 6,     /**< Query the algorithm's version.  The result
                             *   will be returned in the @c data field of the
                             *   respective _Status structure.
                             *
                             *   @remarks   There is no specific format
                             *              defined for version returned by
                             *              the algorithm.
                             */
    XDM_GETCONTEXTINFO = 7, /**< Query a split codec part for its context
                             *   needs.
                             *
                             *   @remarks   Only split codecs are required
                             *              to implement this command.
                             */
    XDM_GETDYNPARAMSDEFAULT = 8,  /**<  Query the algorithm to fill the
                             *   default values for the parameters which
                             *   can be configured dynamically.
                             *
                             *   @remarks   The algorithm provides the
                             *              default dynamic params by
                             *              writing into the @c dynamicParams
                             *              structure.  Note that this is
                             *              an exception to the general rule
                             *              that the algorithm should not
                             *              write into @c dynamicParams.
                             *
                             *   @remarks   The application only needs
                             *              to initialize the
                             *              @c dynamicParams.size and
                             *              @c status.size fields
                             *              prior to calling control()
                             *              with #XDM_GETDYNPARAMSDEFAULT.
                             *
                             *   @remarks   Other than the @c .size field,
                             *              values in the @c status struct
                             *              are undefined upon returning
                             *              from this call.
                             *
                             *   @remarks   To get the <i>current</i> value of
                             *              an algorithm instance's dynamic
                             *              parameters, it's recommended that
                             *              the alg provide them via the
                             *              #XDM_GETSTATUS call.
                             */
    XDM_SETLATEACQUIREARG = 9 /**< Set an algorithm's 'late acquire' argument.
                             *
                             *   @remarks   Handling this command is optional.
                             *
                             *   @remarks   Only algorithms that utilize the
                             *              late acquire IRES feature may
                             *              implement this command.
                             */
} XDM_CmdId;


/**
 *  @brief      Extended error information.
 *
 *  @remarks    When an internal error occurs, the algorithm will return
 *              an error return value (e.g. EFAIL, EUNSUPPORTED)
 *
 *  @remarks    The value of each enum is the bit which is set.
 *
 *  @remarks    Bits 31-16 are reserved.  Bits 7-0 are codec and
 *              implementation specific.
 *
 *  @remarks    The algorithm can set multiple bits to 1 based on conditions.
 *              e.g. it will set bits #XDM_FATALERROR (fatal) and
 *              #XDM_UNSUPPORTEDPARAM (unsupported params) in case
 *              of unsupported run time parameters.
 *
 *  @enumWarning
 */
typedef enum {
    XDM_PARAMSCHANGE = 8,       /**< Bit 8 - Sequence Parameters Change.
                                  *
                                  *   @remarks  This error is applicable
                                  *             for transcoders. It is
                                  *             set when some key parameter
                                  *             of the input sequence changes.
                                  *             The transcoder simply
                                  *             returns after setting this error
                                  *             field and the correct input
                                  *             sequence parameters are
                                  *             updated in outArgs.
                                  */
    XDM_APPLIEDCONCEALMENT = 9,  /**< Bit 9 - Applied concealment.
                                  *
                                  *   @remarks  This error is applicable
                                  *             for decoders.  It is
                                  *             set when the decoder
                                  *             was not able to able
                                  *             to decode the
                                  *             bitstream, and the
                                  *             decoder has concealed
                                  *             the bitstream error
                                  *             and produced the
                                  *             concealed output.
                                  */
    XDM_INSUFFICIENTDATA = 10,   /**< Bit 10 - Insufficient input data.
                                  *
                                  *   @remarks  This error is typically
                                  *             applicable for
                                  *             decoders. This is set
                                  *             when the input data
                                  *             provided is not
                                  *             sufficient to produce
                                  *             of one frame of data.
                                  *             This can be also be
                                  *             set for encoders when
                                  *             the number of valid
                                  *             samples in the input
                                  *             frame is not
                                  *             sufficient to process
                                  *             a frame.
                                  */
    XDM_CORRUPTEDDATA = 11,      /**< Bit 11 - Data problem/corruption.
                                  *
                                  *   @remarks  This error is typically
                                  *             applicable for
                                  *             decoders.  This is set
                                  *             when the bitstream has
                                  *             an error and not
                                  *             compliant to the
                                  *             standard syntax.
                                  */
    XDM_CORRUPTEDHEADER = 12,    /**< Bit 12 - Header problem/corruption.
                                  *
                                  *   @remarks  This error is typically
                                  *             applicable for
                                  *             decoders.  This is set
                                  *             when the header
                                  *             information in the
                                  *             bitstream is
                                  *             incorrect.  For example,
                                  *             it is set when
                                  *             Sequence/Picture/Slice
                                  *             etc. are incorrect in
                                  *             video decoders.
                                  */
    XDM_UNSUPPORTEDINPUT = 13,   /**< Bit 13 - Unsupported feature/parameter
                                  *   in input.
                                  *
                                  *   @remarks  This error is set when the
                                  *             algorithm is not able
                                  *             process a certain
                                  *             input data/bitstream
                                  *             format.  It can also be
                                  *             set when a subset of
                                  *             features in a standard
                                  *             are not supported by
                                  *             the algorithm.
                                  *
                                  *   @remarks  For example, if a video
                                  *             encoder only supports
                                  *             4:2:2 format, it can
                                  *             set this error for any
                                  *             other type of input
                                  *             video format.
                                  */
    XDM_UNSUPPORTEDPARAM = 14,   /**< Bit 14 - Unsupported input parameter or
                                  *   configuration.
                                  *
                                  *   @remarks  This error is set when the
                                  *             algorithm doesn't
                                  *             support certain
                                  *             configurable
                                  *             parameters.  For
                                  *             example, if the video
                                  *             decoder doesn't
                                  *             support the "display
                                  *             width" feature, it
                                  *             shall return
                                  *             XDM_UNSUPPORTEDPARAM
                                  *             when the control
                                  *             function is called for
                                  *             setting the
                                  *             @c displayWidth
                                  *             attribute.

                                  */
    XDM_FATALERROR = 15          /**< Bit 15 - Fatal error (stop the codec).
                                  *   If there is an error and this
                                  *   bit is not set, the error is a
                                  *   recoverable one.
                                  *
                                  *   @remarks  This error is set when the
                                  *             algorithm cannot
                                  *             recover from the
                                  *             current state.  It
                                  *             informs the system not
                                  *             to try the next frame
                                  *             and possibly delete
                                  *             the multimedia
                                  *             algorithm instance.  It
                                  *             implies the codec
                                  *             shall not work when
                                  *             reset.
                                  *
                                  *   @remarks  The user should delete the
                                  *             current instance of
                                  *             the codec.
                                  */
} XDM_ErrorBit;

/** Check for fatal error */
#define XDM_ISFATALERROR(x)         (((x) >> XDM_FATALERROR) & 0x1)
/** Check for unsupported parameter */
#define XDM_ISUNSUPPORTEDPARAM(x)   (((x) >> XDM_UNSUPPORTEDPARAM) & 0x1)
/** Check for unsupported input */
#define XDM_ISUNSUPPORTEDINPUT(x)   (((x) >> XDM_UNSUPPORTEDINPUT) & 0x1)
/** Check for corrupted header */
#define XDM_ISCORRUPTEDHEADER(x)    (((x) >> XDM_CORRUPTEDHEADER) & 0x1)
/** Check for corrupted data */
#define XDM_ISCORRUPTEDDATA(x)      (((x) >> XDM_CORRUPTEDDATA) & 0x1)
/** Check for insufficient data */
#define XDM_ISINSUFFICIENTDATA(x)   (((x) >> XDM_INSUFFICIENTDATA) & 0x1)
/** Check for applied concealment */
#define XDM_ISAPPLIEDCONCEALMENT(x) (((x) >> XDM_APPLIEDCONCEALMENT) & 0x1)

/** Set fatal error bit */
#define XDM_SETFATALERROR(x)         ((x) |= (0x1 << XDM_FATALERROR))
/** Set unsupported parameter bit */
#define XDM_SETUNSUPPORTEDPARAM(x)   ((x) |= (0x1 << XDM_UNSUPPORTEDPARAM))
/** Set unsupported input bit */
#define XDM_SETUNSUPPORTEDINPUT(x)   ((x) |= (0x1 << XDM_UNSUPPORTEDINPUT))
/** Set corrupted header bit */
#define XDM_SETCORRUPTEDHEADER(x)    ((x) |= (0x1 << XDM_CORRUPTEDHEADER))
/** Set corrupted data bit */
#define XDM_SETCORRUPTEDDATA(x)      ((x) |= (0x1 << XDM_CORRUPTEDDATA))
/** Set insufficient data bit */
#define XDM_SETINSUFFICIENTDATA(x)   ((x) |= (0x1 << XDM_INSUFFICIENTDATA))
/** Set applied concealment bit */
#define XDM_SETAPPLIEDCONCEALMENT(x) ((x) |= (0x1 << XDM_APPLIEDCONCEALMENT))


/**
 *  @brief      Endianness of data
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
typedef enum {
    XDM_BYTE = 1,           /**< Big endian stream. */
    XDM_LE_16 = 2,          /**< 16 bit little endian stream. */
    XDM_LE_32 = 3,          /**< 32 bit little endian stream. */
    XDM_LE_64 = 4,          /**< 64 bit little endian stream. */
    XDM_BE_16 = 5,          /**< 16 bit big endian stream. */
    XDM_BE_32 = 6,          /**< 32 bit big endian stream. */
    XDM_BE_64 = 7           /**< 64 bit big endian stream. */
} XDM_DataFormat;


/**
 *  @brief      Date and time
 */
typedef struct XDM_Date {
    XDAS_Int32 msecsOfDay;   /**< Milliseconds of the day */
    XDAS_Int32 month;        /**< Month (0 = January, 11 = December) */
    XDAS_Int32 dayOfMonth;   /**< Month (1 - 31) */
    XDAS_Int32 dayOfWeek;    /**< Day of week (0 = Sunday, 6 = Saturday) */
    XDAS_Int32 year;         /**< Year (since 0) */
} XDM_Date;


/**
 *  @brief      2-dimensional point
 */
typedef struct XDM_Point {
    XDAS_Int32 x;
    XDAS_Int32 y;
} XDM_Point;


/**
 *  @brief      Rectangle
 */
typedef struct XDM_Rect {
    XDM_Point topLeft;
    XDM_Point bottomRight;
} XDM_Rect;

/**
 *  @brief      Maximum number of context buffers.
 */
#define XDM_MAX_CONTEXT_BUFFERS   32


/**
 *  @brief      Buffer information descriptor for input and output buffers.
 */
typedef struct XDM_ContextInfo {
    XDAS_Int32 minContextSize;     /**< Minimum size, in 8-bit bytes,
                                    *   required for the alg context.
                                    */
    XDAS_Int32 minIntermediateBufSizes[XDM_MAX_CONTEXT_BUFFERS];  /**< Minimum
                                    *   size, in 8-bit bytes, required for each
                                    *   intermediate buffer.
                                    *
                                    *   @remarks   The codec indicates the
                                    *              number of intermediate
                                    *              buffers required by
                                    *              zero-terminating this array.
                                    */
} XDM_ContextInfo;


/**
 *  @brief      Context used by split codecs.
 */
typedef struct XDM_Context {
    XDM1_SingleBufDesc algContext;  /**< App allocated and provided.
                                     *
                                     *   @remarks  Split codecs can use this
                                     *             for passing scalar data to
                                     *             the next part.
                                     */

    XDAS_Int32 numInBufs;           /**< Number of input data buffers */
    XDAS_Int32 numOutBufs;          /**< Number of output data buffers */
    XDAS_Int32 numInOutBufs;        /**< Number of in/out data buffers */
    XDM1_SingleBufDesc inBufs[XDM_MAX_CONTEXT_BUFFERS];  /**< Input data
                                     *   Cbuffers.
                                     *
                                     *   @remarks  This is a sparse array.
                                     */
    XDM1_SingleBufDesc outBufs[XDM_MAX_CONTEXT_BUFFERS]; /**< Output data
                                     *   buffers.
                                     *
                                     *   @remarks  This is a sparse array.
                                     */
    XDM1_SingleBufDesc inOutBufs[XDM_MAX_CONTEXT_BUFFERS]; /**< Input/Output
                                     *   data buffers.
                                     *
                                     *   @remarks  This is a sparse array.
                                     */

    XDM1_SingleBufDesc intermediateBufs[XDM_MAX_CONTEXT_BUFFERS]; /**< Intermediate, working buffers.
                                     *
                                     *   @remarks   For FRONT codec parts,
                                     *              these buffers are
                                     *              treated as OUT
                                     *              buffers (i.e.,
                                     *              written to by the
                                     *              algorithm).  For
                                     *              BACK codec parts,
                                     *              these buffers are
                                     *              treated as IN
                                     *              buffers (i.e., read
                                     *              from by the
                                     *              algorithm).  For
                                     *              MIDDLE codec parts,
                                     *              these buffers are
                                     *              treated as IN/OUT
                                     *              buffers (i.e., the
                                     *              codec can read
                                     *              from, and write to
                                     *              them).
                                     *
                                     *   @remarks   This is a null-terminated
                                     *              array
                                     */
} XDM_Context;


/**
 *  @brief      Encoding presets.
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
typedef enum {
    XDM_DEFAULT = 0,                /**< Default setting of encoder.  See
                                     *   codec specific documentation for its
                                     *   encoding behaviour.
                                     */
    XDM_HIGH_QUALITY = 1,           /**< High quality encoding. */
    XDM_HIGH_SPEED = 2,             /**< High speed encoding. */
    XDM_USER_DEFINED = 3,           /**< User defined configuration, using
                                     *   advanced parameters.
                                     */
    XDM_HIGH_SPEED_MED_QUALITY = 4, /**< High speed, medium quality
                                     *   encoding.
                                     */
    XDM_MED_SPEED_MED_QUALITY = 5,  /**< Medium speed, medium quality
                                     *   encoding.
                                     */
    XDM_MED_SPEED_HIGH_QUALITY = 6, /**< Medium speed, high quality
                                     *   encoding.
                                     */

    XDM_ENCODING_PRESET_MAX  = 7,   /**< @todo need to add documentation */
    XDM_PRESET_DEFAULT = XDM_MED_SPEED_MED_QUALITY /**< Default setting of
                                     *   encoder.  See codec specific
                                     *   documentation for its encoding
                                     *   behaviour.
                                     */
} XDM_EncodingPreset;


/**
 *  @brief      Decode entire access unit or only header.
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
typedef enum {
    XDM_DECODE_AU = 0,      /**< Decode entire access unit, including all
                             *   the headers.
                             */
    XDM_PARSE_HEADER = 1    /**< Decode only header. */
} XDM_DecMode;


/**
 *  @brief      Encode entire access unit or only header.
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
typedef enum {
    XDM_ENCODE_AU = 0,      /**< Encode entire access unit, including the
                             *   headers.
                             */
    XDM_GENERATE_HEADER = 1 /**< Encode only header. */
} XDM_EncMode;


/**
 *  @brief      Chroma formats.
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
typedef enum {
    XDM_CHROMA_NA = -1,     /**< Chroma format not applicable. */
    XDM_YUV_420P = 1,       /**< YUV 4:2:0 planer. */
    XDM_YUV_422P = 2,       /**< YUV 4:2:2 planer. */
    XDM_YUV_422IBE = 3,     /**< YUV 4:2:2 interleaved (big endian). */
    XDM_YUV_422ILE = 4,     /**< YUV 4:2:2 interleaved (little endian). */
    XDM_YUV_444P = 5,       /**< YUV 4:4:4 planer. */
    XDM_YUV_411P = 6,       /**< YUV 4:1:1 planer. */
    XDM_GRAY = 7,           /**< Gray format. */
    XDM_RGB = 8,            /**< RGB color format. */
    XDM_YUV_420SP = 9,      /**< YUV 420 semi_planar format.(Luma 1st plane,
                             *   CbCr interleaved 2nd plane)
                             */
    XDM_ARGB8888 = 10,      /**< Alpha plane. */
    XDM_RGB555 = 11,        /**< RGB 555 color format. */
    XDM_RGB565 = 12,        /**< RGB 565 color format. */
    XDM_YUV_444ILE = 13,    /**< YUV 4:4:4 interleaved (little endian). */
    /** Default setting. */
    XDM_CHROMAFORMAT_DEFAULT = XDM_YUV_422ILE
} XDM_ChromaFormat;


/**
 *  @brief      Memory space attributes
 *
 *  @enumWarning
 *
 *  @extendedEnum
 */
 typedef enum {
    XDM_MEMTYPE_ROW = 0,        /**< Linear (standard) memory.
                                 *
                                 * @deprecated  The XDM_MEMTYPE_ROW value
                                 *        is deprecated.  Please use
                                 *        #XDM_MEMTYPE_RAW (which, by design,
                                 *        has the same underyling value)
                                 *        instead.
                                 *
                                 * @remarks     XDM_MEMTYPE_ROW may be
                                 *              removed in a future
                                 *              release.
                                 *
                                 * @sa XDM_MemoryType::XDM_MEMTYPE_RAW
                                 */
    XDM_MEMTYPE_RAW = 0,        /**< Linear (standard) memory.
                                 *
                                 * @todo  add documentation
                                 */
    XDM_MEMTYPE_TILED8 = 1,     /**< @todo  add documentation */
    XDM_MEMTYPE_TILED16 = 2,    /**< @todo  add documentation */
    XDM_MEMTYPE_TILED32 = 3,    /**< @todo  add documentation */
    XDM_MEMTYPE_TILEDPAGE = 4   /**< @todo  add documentation */
} XDM_MemoryType;


/**
 *  @brief      Memory space attributes
 *
 *  @enumWarning
 */
 typedef enum {
    XDM_MEMUSAGE_DATASYNC = 0     /**< Bit 0 - Data Sync mode
                                   *
                                   *  @remarks  If this bit is set,
                                   *            the memory will be
                                   *            used in data sync mode.
                                   *
                                   *  @remarks  When in data sync mode, the
                                   *            algorithm is responsible for
                                   *            ensuring any XDAIS rules are
                                   *            met for the buffer (e.g.
                                   *            managing cache coherency).
                                   *
                                   *  @remarks  If an algorithm is running in
                                   *            mode other than data sync, and
                                   *            receives a buffer with this
                                   *            bit set, it should return an
                                   *            error, likely
                                   *            #XDM_UNSUPPORTEDINPUT.
                                   */
} XDM_MemoryUsageMode;


/**
 *  @brief          Descriptor for the chunk of data being
 *                  transferred in one call to putData or getData
 */
typedef struct XDM_DataSyncDesc {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 scatteredBlocksFlag; /**< Flag indicating whether the
                                 *   individual data blocks may
                                 *   be scattered in memory.
                                 *
                                 *   @remarks   Note that each individual
                                 *              block must be physically
                                 *              contiguous.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 *
                                 *   @remarks   If set to XDAS_FALSE, the
                                 *              @c baseAddr field points
                                 *              directly to the start of the
                                 *              first block, and is not treated
                                 *              as a pointer to an array.
                                 *
                                 *   @remarks   If set to XDAS_TRUE, the
                                 *              @c baseAddr array must
                                 *              contain the base address of
                                 *              each individual block.
                                 */
    XDAS_Int32 *baseAddr;       /**< Base address of single data block or
                                 *              pointer to an array of
                                 *              data block addresses of
                                 *              size @c numBlocks.
                                 *
                                 *   @remarks   If @c scatteredBlocksFlag is
                                 *              set to XDAS_FALSE, this
                                 *              field points
                                 *              directly to the start of the
                                 *              first block, and is not treated
                                 *              as a pointer to an array.
                                 *
                                 *   @remarks   If @c scatteredBlocksFlag is
                                 *              set to XDAS_TRUE, this
                                 *              field points to an array of
                                 *              pointers to the data blocks.
                                 */
    XDAS_Int32 numBlocks;       /**< Number of blocks available */
    XDAS_Int32 varBlockSizesFlag; /**< Flag indicating whether any of the
                                 *   data blocks vary in size.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 */
    XDAS_Int32 *blockSizes;     /**< Variable block sizes array.
                                 *
                                 *  @remarks     If @c varBlockSizesFlag is
                                 *               XDAS_TRUE, this array
                                 *               contains the sizes of each
                                 *               block.  If @c varBlockSizesFlag
                                 *               is XDAS_FALSE, this contains
                                 *               the size of same-size blocks.
                                 *
                                 *   @remarks    Memory for this array
                                 *               (of size @c numBlocks) has
                                 *               to be allocated by the
                                 *               caller of the putData API.
                                 */
} XDM_DataSyncDesc;


/**
 *  @brief          Handle that identifies the DataSync FIFO.
 *
 *  @sa XDM_DataSyncPutFxn()
 *  @sa XDM_DataSyncGetFxn()
 *  @sa XDM_DataSyncGetBufferFxn()
 *  @sa XDM_DataSyncPutBufferFxn()
 */
typedef Void * XDM_DataSyncHandle;


/**
 *  @brief      Non-blocking API to signal "data ready" to one or more
 *              consumers.
 *
 *  @param[in]  dataSyncHandle  Handle to a data sync instance.
 *  @param[out] dataSyncDesc    Full data sync descriptor.  This includes one
 *                              or more filled data buffers.
 *
 *
 *  @todo       Needs review
 *
 *  @sa IVIDDEC3_DynamicParams::putDataFxn()
 *  @sa IVIDENC2_DynamicParams::putDataFxn()
 *  @sa IVIDENC2_DynamicParams::getBufferFxn()
 *  @sa XDM_DataSyncGetBufferFxn
 */
typedef Void (*XDM_DataSyncPutFxn)(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
 *  @brief          API to obtain data information from a consumer.
 *
 *  @param[in]  dataSyncHandle  Handle to a data sync instance.
 *  @param[out] dataSyncDesc    Empty data sync descriptor to be filled by
 *                              the producer.  Only the @c size field must
 *                              be initialized by the caller.
 *
 *  @post       Upon return, the @c dataSyncDesc will contain details about
 *              the provided data.
 *
 *  @remarks    Given that this is an input buffer, the implementation of this
 *              fxn must make the provided external or shared memory coherent
 *              with respect to the algorithm processor's cache.
 *
 *  @todo       Needs review
 *
 *  @sa IVIDDEC3_DynamicParams::getDataFxn()
 *  @sa IVIDDEC3_DynamicParams::putBufferFxn()
 *  @sa IVIDENC2_DynamicParams::getDataFxn()
 *  @sa XDM_DataSyncPutBufferFxn
 */
typedef XDAS_Int32 (*XDM_DataSyncGetFxn)(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
 *  @brief      API to obtain empty bitstream buffers from an allocator
 *              to be filled by the algorithm.
 *
 *  @param[in]  dataSyncHandle  Handle to a data sync instance.
 *  @param[out] dataSyncDesc    Empty data sync descriptor to be filled by
 *                              the allocator.  Only the @c size field must
 *                              be initialized by the caller.
 *
 *  @post       Upon return, the @c dataSyncDesc will contain details about
 *              the allocated buffer(s).
 *
 *  @remarks    Given that this is an output buffer, the implementation of this
 *              fxn (i.e., the allocator) must make the provided external or
 *              shared memory coherent with respect to the algorithm
 *              processor's cache.  This typically implies it must be cache
 *              invalidated since the alg may fill this buffer via DMA.
 *
 *  @remarks    The allocator may not zero-initialize this buffer.  If the
 *              allocator <i>does</i> initialize the buffer, it must ensure the
 *              cache coherency via writeback-invalidate.
 *
 *  @todo       Needs review
 *
 *  @sa IVIDENC2_DynamicParams::putDataFxn()
 *  @sa IVIDENC2_Fxns::process()
 */
typedef XDAS_Int32 (*XDM_DataSyncGetBufferFxn)(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);


/**
 *  @brief      API to return consumed bitstream buffers to the original
 *              provider.
 *
 *  @param[in]  dataSyncHandle  Handle to a data sync instance.
 *  @param[out] dataSyncDesc    Data sync descriptor.
 *
 *  @todo       What cache coherency responsibilities are placed on this
 *              buffer?
 *
 *  @todo       Needs review and further detail
 *
 *  @sa IVIDDEC3_DynamicParams::getDataFxn()
 *  @sa IVIDDEC3_Fxns::process()
 */
typedef XDAS_Int32 (*XDM_DataSyncPutBufferFxn)(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/*@}*/


#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_dm_XDM_ */
/*
 *  @(#) ti.xdais.dm; 1, 0, 7,14; 5-24-2010 11:19:27; /db/wtree/library/trees/dais/dais.git/src/
 */

