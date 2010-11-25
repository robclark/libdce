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
 *  @file       ti/xdais/dm/ividdec3.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video decoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDDEC3   IVIDDEC3 - XDM Video Decoder Interface (Beta)
 *
 *  This is the XDM IVIDDEC3 Video Decoder Interface (Beta).
 */

#ifndef ti_xdais_dm_IVIDDEC3_
#define ti_xdais_dm_IVIDDEC3_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @ingroup    ti_xdais_dm_IVIDDEC3 */
/*@{*/

#define IVIDDEC3_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define IVIDDEC3_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define IVIDDEC3_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */

/**
 *  @brief      This must be the first field of all IVIDDEC3
 *              instance objects.
 */
typedef struct IVIDDEC3_Obj {
    struct IVIDDEC3_Fxns *fxns;
} IVIDDEC3_Obj;


/**
 *  @brief      Opaque handle to an IVIDDEC3 objects.
 */
typedef struct IVIDDEC3_Obj  *IVIDDEC3_Handle;


/**
 *  @brief      Video decoder output frame order.
 *
 *  @enumWarning
 *
 *  @sa IVIDDEC3_Params.displayDelay
 *
 */
typedef enum {
    IVIDDEC3_DISPLAY_DELAY_AUTO = -1,  /**< Decoder decides the display delay */
    IVIDDEC3_DECODE_ORDER = 0,        /**< Display frames are in decoded order without delay */
    IVIDDEC3_DISPLAY_DELAY_1 = 1,     /**< Display the frames with 1 frame delay  */
    IVIDDEC3_DISPLAY_DELAY_2 = 2,     /**< Display the frames with 2 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_3 = 3,     /**< Display the frames with 3 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_4 = 4,     /**< Display the frames with 4 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_5 = 5,     /**< Display the frames with 5 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_6 = 6,     /**< Display the frames with 6 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_7 = 7,     /**< Display the frames with 7 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_8 = 8,     /**< Display the frames with 8 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_9 = 9,     /**< Display the frames with 9 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_10 = 10,   /**< Display the frames with 10 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_11 = 11,   /**< Display the frames with 11 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_12 = 12,   /**< Display the frames with 12 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_13 = 13,   /**< Display the frames with 13 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_14 = 14,   /**< Display the frames with 14 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_15 = 15,   /**< Display the frames with 15 frames delay  */
    IVIDDEC3_DISPLAY_DELAY_16 = 16,   /**< Display the frames with 16 frames delay  */
    IVIDDEC3_DISPLAYDELAY_DEFAULT = IVIDDEC3_DISPLAY_DELAY_AUTO
} IVIDDEC3_displayDelay;

/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDDEC3 instance objects.
 *
 *  @extensibleStruct
 */
typedef struct IVIDDEC3_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 maxHeight;       /**< Maximum video height in pixels. */
    XDAS_Int32 maxWidth;        /**< Maximum video width in pixels. */
    XDAS_Int32 maxFrameRate;    /**< Maximum frame rate in fps * 1000.
                                 *   For example, if max frame rate is 30
                                 *   frames per second, set this field
                                 *   to 30000.
                                 */
    XDAS_Int32 maxBitRate;      /**< Maximum bit rate, bits per second.
                                 *   For example, if bit rate is 10 Mbps, set
                                 *   this field to 10000000
                                 */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa XDM_DataFormat
                                 */
    XDAS_Int32 forceChromaFormat;/**< @copydoc XDM_ChromaFormat
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 operatingMode;   /**< Video coding mode of operation.
                                 *
                                 *   @sa IVIDEO_OperatingMode
                                 */
    XDAS_Int32 displayDelay;    /**< @copydoc IVIDDEC3_displayDelay
                                 *
                                 *   @sa IVIDDEC3_displayDelay
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDDEC3_Fxns.control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              IVIDDEC3_Fxns.process() to
                                 *              set
                                 *              IVIDDEC3_DynamicParams.getDataFxn()
                                 *              and
                                 *              IVIDDEC3_DynamicParams.getDataHandle
                                 *              (and optionally
                                 *              IVIDDEC3_DynamicParams.putBufferFxn(),
                                 *              and
                                 *              IVIDDEC3_DynamicParams.putBufferHandle).
                                 *              Else, the alg can return error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDDEC3_Fxns.control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              #IVIDDEC3_Fxns.process() to
                                 *              set
                                 *              IVIDDEC3_DynamicParams.putDataFxn(),
                                 *              and
                                 *              IVIDDEC3_DynamicParams.putDataHandle.
                                 *              Else, the alg can return error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 numInputDataUnits;/**< Number of input slices/rows.
                                 *
                                 *   @remarks   Units depend on the
                                 *              IVIDDEC3_Params.inputDataMode,
                                 *              like number of
                                 *              slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDDEC3_Params.inputDataMode
                                 *              is set to full frame mode.
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks   Units depend on the
                                 *              IVIDDEC3_Params.outputDataMode,
                                 *              like number of
                                 *              slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDDEC3_Params.outputDataMode
                                 *              is set to full frame mode.
                                 */
    XDAS_Int32 errorInfoMode;   /**< Enable/disable packet error information
                                 *   for input and/or output.
                                 *
                                 *   @sa IVIDEO_ErrorInfoMode
                                 */
    XDAS_Int32 displayBufsMode; /**< Indicates which mode the displayBufs are
                                 *   presented in.
                                 *
                                 *   @remarks   See the
                                 *              IVIDDEC3_DisplayBufsMode enum
                                 *              for the values this field may
                                 *              contain.
                                 *
                                 *   @sa IVIDDEC3_OutArgs.displayBufsMode
                                 *   @sa IVIDDEC3_DisplayBufsMode
                                 */
    XDAS_Int32 metadataType[IVIDEO_MAX_NUM_METADATA_PLANES];/**< Type of
                                 *   each metadata plane.
                                 *
                                 *   @sa IVIDEO_MetadataType
                                 */
 } IVIDDEC3_Params;


/**
 *  @brief      This structure defines the algorithm parameters that can be
 *              modified after creation via IVIDDEC3_Fxns.control() calls.
 *
 *  @remarks    It is not necessary that a given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC3_Fxns::control()
 */
typedef struct IVIDDEC3_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 decodeHeader;    /**< @copydoc XDM_DecMode
                                 *
                                 *   @sa XDM_DecMode
                                 */
    XDAS_Int32 displayWidth;    /**< Pitch.  If set to zero, use the decoded
                                 *   image width.  Else, use given display
                                 *   width in pixels.  Display width has to be
                                 *   greater than or equal to image width.
                                 */
    XDAS_Int32 frameSkipMode;   /**< @copydoc IVIDEO_FrameSkip
                                 *
                                 *   @sa IVIDEO_FrameSkip
                                 */
    XDAS_Int32 newFrameFlag;    /**< Flag to indicate that the algorithm should
                                 *   start a new frame.
                                 *
                                 *   @remarks   Valid values are XDAS_TRUE
                                 *              and XDAS_FALSE.
                                 *
                                 *   @remarks   This is useful for error
                                 *              recovery, for example when the
                                 *              end of frame cannot be detected
                                 *              by the codec but is known to the
                                 *              application.
                                 */
    XDM_DataSyncPutFxn putDataFxn; /**< Optional datasync "put data" function.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              video decoder.  The decoder
                                 *              calls this function when
                                 *              sub-frame data has been put
                                 *              into an output buffer and is
                                 *              available.
                                 */
    XDM_DataSyncHandle putDataHandle;/**< Datasync "put data" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              codec must provide when
                                 *              calling the app-registered
                                 *              IVIDDEC3_DynamicParams.putDataFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDDEC3_DynamicParams.putDataFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDM_DataSyncGetFxn getDataFxn;/**< Datasync "get data" function.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              video decoder.  The decoder
                                 *              calls this function to get
                                 *              partial compressed bit-stream
                                 *              data from the app/framework.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 */
    XDM_DataSyncHandle getDataHandle;/**< Datasync "get data" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              codec must provide when
                                 *              calling @c getDataFxn.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDDEC3_DynamicParams.getDataFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDM_DataSyncPutBufferFxn putBufferFxn;/**< Datasync "put buffer" function.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              video decoder.  The decoder
                                 *              calls this function to release
                                 *              consumed, partial compressed
                                 *              bit-stream data buffers to the
                                 *              app/framework.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 */
    XDM_DataSyncHandle putBufferHandle;/**< Datasync "put buffer" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              codec must provide when
                                 *              calling the app-registered
                                 *              IVIDDEC3_DynamicParam.putBufferFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDDEC3_DynamicParams.putBufferFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDAS_Int32 lateAcquireArg;  /**< Argument used during late acquire.
                                 *
                                 *   @remarks   For all control() commands
                                 *              other than
                                 *              #XDM_SETLATEACQUIREARG, this
                                 *              field is ignored and can
                                 *              therefore be set by the
                                 *              caller to any value.
                                 *
                                 *   @remarks   This field is used to
                                 *              provide the
                                 *              'late acquire' arg required by
                                 *              #XDM_SETLATEACQUIREARG.
                                 *
                                 *   @remarks   Late acquire support is
                                 *              an optional feature for
                                 *              video decoders.  If the
                                 *              codec supports late
                                 *              acquisition of resources,
                                 *              and the application has supplied
                                 *              a lateAcquireArg value (via
                                 *              #XDM_SETLATEACQUIREARG), then the
                                 *              codec must also provide this
                                 *              @c lateAcquireArg value when
                                 *              requesting resources (i.e.
                                 *              during their call to
                                 *              acquire() when requesting
                                 *              the resource).
                                 */
} IVIDDEC3_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IVIDDEC3 instance
 *              process function.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC3_Fxns::process()
 */
typedef struct IVIDDEC3_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 numBytes;        /**< Size of input data in bytes, provided
                                 *   to the algorithm for decoding.
                                 */
    XDAS_Int32 inputID;         /**< The decoder will attach
                                 *   this ID with the corresponding output
                                 *   frames.
                                 *
                                 *   @remarks   This is useful when frames
                                 *   require re-ordering (e.g. B frames).
                                 *
                                 *   @remarks   When there is no re-ordering,
                                 *   IVIDDEC3_OutArgs#outputID will be same
                                 *   as this inputID field.
                                 *
                                 *   @remarks   Zero (0) is not a supported
                                 *              inputID.  This value is
                                 *              reserved for cases when there
                                 *              is no output buffer provided in
                                 *              IVIDDEC3_OutArgs::displayBufs.
                                 *
                                 *   @sa IVIDDEC3_OutArgs::outputID.
                                 */
} IVIDDEC3_InArgs;


/**
 *  @brief      Defines instance status parameters.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDDEC3_Fxns::control()
 */
typedef struct IVIDDEC3_Status {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDM1_SingleBufDesc data;    /**< Buffer descriptor for data passing.
                                 *
                                 *   @remarks   If this field is not used,
                                 *              the application <b>must</b>
                                 *              set @c data.buf to NULL.
                                 *
                                 *   @remarks   This buffer can be used as
                                 *              either input or output,
                                 *              depending on the command.
                                 *
                                 *   @remarks   The buffer will be provided
                                 *              by the application, and
                                 *              returned to the application
                                 *              upon return of the
                                 *              IVIDDEC3_Fxns.control()
                                 *              call.  The algorithm must
                                 *              not retain a pointer to this
                                 *              data.
                                 *
                                 *   @sa #XDM_GETVERSION
                                 */
    XDAS_Int32 maxNumDisplayBufs;/**< The maximum number of buffers that will
                                 *   be required by the codec.
                                 *
                                 *   @remarks   The maximum number of buffers
                                 *              can be IVIDEO2_MAX_IO_BUFFERS.
                                 */
    XDAS_Int32 maxOutArgsDisplayBufs;/**< The maximum number of display
                                 *   buffers that can be returned via
                                 *   IVIDDEC3_OutArgs.displayBufs.
                                 *
                                 *   @remarks   If returning display buffers
                                 *              embedded into the OutArgs
                                 *              struct, this field provides
                                 *              the size of the
                                 *              OutArgs.displayBufs.bufDesc[]
                                 *              array.
                                 */
    XDAS_Int32 outputHeight;    /**< Output height in pixels. */
    XDAS_Int32 outputWidth;     /**< Output width in pixels. */
    XDAS_Int32 frameRate;       /**< Average frame rate in fps * 1000.
                                 *   For example, if average frame rate is 30
                                 *   frames per second, this field should be
                                 *   30000.
                                 */
    XDAS_Int32 bitRate;         /**< Average bit rate, in bits per second. */
    XDAS_Int32 contentType;     /**< @copydoc IVIDEO_ContentType
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 sampleAspectRatioHeight;/**< Sample aspect ratio height. */
    XDAS_Int32 sampleAspectRatioWidth;/**< Sample aspect ratio width. */
    XDAS_Int32 bitRange;        /**< Full 8 bit, CCIR 601 */
    XDAS_Int32 forceChromaFormat;/**< Output chroma format.
                                 *
                                 *   @sa    XDM_ChromaFormat
                                 */
    XDAS_Int32 operatingMode;   /**< Video decoding mode of operation.
                                 *
                                 *   @sa IVIDEO_OperatingMode
                                 */
    XDAS_Int32 frameOrder;      /**< Frame Order
                                 *
                                 *   @remarks   This field reflects the value
                                 *              provided during creation in
                                 *              IVIDDEC3_Params.displayDelay
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @sa IVIDDEC3_Params.inputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @sa IVIDDEC3_Params.outputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDM1_AlgBufInfo bufInfo;    /**< Input and output buffer information.
                                 *
                                 *   @remarks   This field provides the
                                 *              application with the algorithm's
                                 *              buffer requirements.  The
                                 *              requirements may vary depending
                                 *              on the current configuration
                                 *              of the algorithm instance.
                                 *
                                 *   @sa XDM1_AlgBufInfo
                                 */
    XDAS_Int32 numInputDataUnits;/**< Number of input slices/rows.
                                 *
                                 *   @remarks   Units depend on the
                                 *              IVIDDEC3_Params.inputDataMode,
                                 *              like number of
                                 *              slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDDEC3_Params.inputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDDEC3_Params.inputDataMode
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c outputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDDEC3_Params.outputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDDEC3_Params.outputDataMode
                                 */
    XDAS_Int32 configurationID; /**< Configuration ID of given codec.
                                 *
                                 *   @remarks   This is based on the input
                                 *              stream & can be used by the
                                 *              framework to optimize the
                                 *              save/restore overhead of any
                                 *              resources used.
                                 *
                                 *   @remarks   This can be useful in
                                 *              multichannel use case
                                 *              scenarios.
                                 */
    XDAS_Int32 metadataType[IVIDEO_MAX_NUM_METADATA_PLANES];/**< Type of
                                 *   each metadata plane.
                                 *
                                 *   @sa IVIDEO_MetadataType
                                 */
    IVIDDEC3_DynamicParams decDynamicParams;/**< Current values of the
                                 *   decoder's dynamic parameters.
                                 *
                                 *   @remarks   This is the last field in
                                 *              the base struct as it can
                                 *              be extended.
                                 */
} IVIDDEC3_Status;


/**
 *  @brief      Mode in which display buffers will be returned in
 *              IVIDDEC3_OutArgs.
 *
 *  @remarks    Note that we start this enum at '1' to help catch inadvertently
 *              uninitialized fields.
 *
 *  @enumWarning
 */
typedef enum {
    IVIDDEC3_DISPLAYBUFS_EMBEDDED = 1, /**< @c displayBufs details are embedded
                                        *   into the struct.
                                        *
                                        *   @remarks    This mode causes the
                                        *               size of the
                                        *               IVIDDEC3_OutArgs
                                        *               struct to be larger,
                                        *               but typically easier to
                                        *               manage by both the app
                                        *               and the codec.
                                        */
    IVIDDEC3_DISPLAYBUFS_PTRS = 2      /**< @c displayBufs details are returned
                                        *   via the provided pointers.
                                        *
                                        *   @remarks    This mode causes the
                                        *               size of the
                                        *               IVIDDEC3_OutArgs
                                        *               struct to be smaller
                                        *               and predictable,
                                        *               but makes the usage of
                                        *               the struct more
                                        *               difficult as the
                                        *               app/framework has to
                                        *               manage (sometimes many)
                                        *               small buffers.  On
                                        *               multiprocessor systems,
                                        *               these extra buffers can
                                        *               introduce extra
                                        *               overhead related to
                                        *               address translation and
                                        *               cache maintenance.
                                        */
} IVIDDEC3_DisplayBufsMode;


/**
 *  @brief      Defines the run time output arguments for
 *              all IVIDDEC3 instance objects.
 *
 *  @extensibleStruct
 *
 *  @remarks    The size of this struct may vary when
 *              IVIDDEC3_OutArgs.displayBufsMode is set to
 *              #IVIDDEC3_DISPLAYBUFS_EMBEDDED (see details in
 *              IVIDDEC3_OutArgs.displayBufs.bufDesc).
 *
 *  @remarks    When IVIDDEC3_OutArgs.displayBufsMode is set to
 *              #IVIDDEC3_DISPLAYBUFS_EMBEDDED, the number of elements in the
 *              IVIDDEC3_OutArgs.displayBufs.bufDesc array is a constant (and
 *              can be acquired by calling IVIDDEC3_Fxns.control() and looking
 *              in the IVIDDEC3_Status.maxOutArgsDisplayBufs field.  Note that
 *              any extended fields follow the
 *              IVIDDEC3_OutArgs.displayBufs.bufDesc array.
 *
 *  @sa         IVIDDEC3_Fxns.process()
 */
typedef struct IVIDDEC3_OutArgs {
    XDAS_Int32 size;            /**< @sizeField
                                 *
                                 *   @remarks   Extra care must be taken when
                                 *              setting this field as the
                                 *              size of even the base data
                                 *              can vary because of the
                                 *              #IVIDDEC3_OutArgs.displayBufs
                                 *              field.
                                 *
                                 *   @sa IVIDDEC3_OutArgs.displayBufs
                                 */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesConsumed;   /**< Number of bytes consumed. */
    XDAS_Int32 outputID[IVIDEO2_MAX_IO_BUFFERS]; /**< Output ID corresponding
                                 *   to @c displayBufs[].
                                 *
                                 *   @remarks   A value of zero (0) indicates
                                 *              an invalid ID.  The first zero
                                 *              entry in array will indicate
                                 *              end of valid outputIDs within
                                 *              the array.  Hence the
                                 *              application can stop reading the
                                 *              array when it encounters the
                                 *              first zero entry.
                                 *
                                 *   @sa IVIDDEC3_OutArgs.displayBufs
                                 *   @sa IVIDDEC3_InArgs.inputID
                                 */
    IVIDEO2_BufDesc decodedBufs; /**< The decoder fills this structure with
                                 *   buffer pointers to the decoded frame.
                                 *   Related information fields for the
                                 *   decoded frame are also populated.
                                 *
                                 *   When frame decoding is not complete, as
                                 *   indicated by
                                 *   IVIDDEC3_OutArgs.outBufsInUseFlag,
                                 *   the frame data in this structure will be
                                 *   incomplete.  However, the algorithm will
                                 *   provide incomplete decoded frame data
                                 *   in case application wants to use
                                 *   it for error recovery purposes.
                                 *
                                 *   @sa IVIDDEC3_OutArgs.outBufsInUseFlag
                                 */
    XDAS_Int32 freeBufID[IVIDEO2_MAX_IO_BUFFERS]; /**< This is an
                                 *   array of inputID's corresponding to the
                                 *   buffers that have been unlocked in the
                                 *   current process call.
                                 *
                                 *   @remarks   Buffers returned to the
                                 *              application for display (via
                                 *              IVIDDEC3_OutArgs.displayBufs)
                                 *              continue to be owned by the
                                 *              algorithm until they are
                                 *              released - indicated by
                                 *              the ID being returned in this
                                 *              @c freeBuf array.
                                 *
                                 *   @remarks   The buffers released by the
                                 *              algorithm are indicated by
                                 *              their non-zero ID (previously
                                 *              provided via
                                 *              IVIDDEC3_InArgs.inputID).
                                 *
                                 *   @remarks   A value of zero (0) indicates
                                 *              an invalid ID.  The first zero
                                 *              entry in array will indicate
                                 *              end of valid freeBufIDs within
                                 *              the array.  Hence the
                                 *              application can stop searching
                                 *              the array when it encounters the
                                 *              first zero entry.
                                 *
                                 *   @remarks   If no buffer was unlocked in
                                 *              the process call,
                                 *              @c freeBufID[0] will
                                 *              have a value of zero.
                                 *
                                 *   @sa IVIDDEC3_InArgs.inputID
                                 *   @sa IVIDDEC3_OutArgs.displayBufs
                                 */
    XDAS_Int32 outBufsInUseFlag; /**< Flag to indicate that the @c outBufs
                                 *   provided with the IVIDDEC3_Fxns.process()
                                 *   call are in use.  No @c outBufs are
                                 *   required to be supplied with the next
                                 *   IVIDDEC3_Fxns.process() call.
                                 *
                                 *   @remarks   Valid values are #XDAS_TRUE
                                 *              and #XDAS_FALSE.
                                 */
    XDAS_Int32 displayBufsMode; /**< Indicates which mode the
                                 *   #IVIDDEC3_OutArgs.displayBufs are
                                 *   presented in.
                                 *
                                 *   @remarks   See the
                                 *              IVIDDEC3_DisplayBufsMode enum
                                 *              for the values this field may
                                 *              contain.
                                 *
                                 *   @remarks   This will be set to the same
                                 *              value the application provided
                                 *              at creation time via
                                 *              #IVIDDEC3_Params.displayBufsMode.
                                 *
                                 *   @sa IVIDDEC3_Params.displayBufsMode
                                 *   @sa IVIDDEC3_DisplayBufsMode
                                 */
    union {
        IVIDEO2_BufDesc bufDesc[1];/**< Array containing display frames
                                 *   corresponding to valid ID entries
                                 *   in the @c outputID[] array.
                                 *
                                 *   @remarks   The number of elements in this
                                 *              array is not necessarily 1, and
                                 *              should be acquired by the app
                                 *              via the
                                 *              IVIDDEC3_Status.maxNumDisplayBufs
                                 *              field - acquired by calling
                                 *              IVIDDEC3_Fxns.control() with
                                 *              #XDM_GETSTATUS.
                                 *              The application should acquire
                                 *              this prior to calling
                                 *              IVIDDEC3_Fxns.process().
                                 *
                                 *   @remarks   Because of the variable size
                                 *              of this array, care must be
                                 *              taken when setting the
                                 *              IVIDDEC3_OutArgs.size field
                                 *              to include the complete size of
                                 *              this struct.
                                 *
                                 *   @remarks   Entries in the array
                                 *              corresponding to invalid
                                 *              ID values (zero) in
                                 *              IVIDDEC3_OutArgs.outputID[] will
                                 *              set zero value for the following
                                 *              fields in the IVIDEO2_BufDesc
                                 *              structure:  @c numPlanes,
                                 *              @c numMetaPlanes.
                                 *
                                 *   @remarks   Implied by the previous remark,
                                 *              as this array corresponds to
                                 *              buffer IDs indicated by
                                 *              @c outputID[], elements of
                                 *              this array are undefined if
                                 *              the corresponding @c outputID[]
                                 *              element is zero (0).
                                 *
                                 */
        IVIDEO2_BufDesc *pBufDesc[IVIDEO2_MAX_IO_BUFFERS]; /**< Array containing
                                 *   pointers to display frames corresponding
                                 *   to valid ID entries in the @c outputID[]
                                 *   array.
                                 *
                                 *   @remarks   These buffers must be allocated
                                 *              by the application, and provided
                                 *              <i>into</i> this "outArgs"
                                 *              structure by the app.
                                 */
    } displayBufs;              /**< Display Buffers union.
                                 *
                                 *   @remarks   This field is complex.  The
                                 *              value in
                                 *              #IVIDDEC3_OutArgs.displayBufsMode
                                 *              indicates how the user should
                                 *              interact with this union field.
                                 *              If #IVIDDEC3_OutArgs.displayBufsMode
                                 *              is
                                 *              #IVIDDEC3_DISPLAYBUFS_EMBEDDED,
                                 *              this field should be referenced
                                 *              via the
                                 *              IVIDDEC3_OutArgs.bufDesc[] array
                                 *              who's number of elements is
                                 *              determined via the
                                 *              #IVIDDEC3_OutArgs.outputID[]
                                 *              array.  If this field is
                                 *              #IVIDDEC3_DISPLAYBUFS_PTRS,
                                 *              this field should be referenced
                                 *              via the
                                 *              IVIDDEC3_OutArgs.pBufDesc[]
                                 *              array.
                                 *
                                 *   @sa IVIDDEC3_OutArgs.bufDesc
                                 *   @sa IVIDDEC3_OutArgs.pBufDesc
                                 */
} IVIDDEC3_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDDEC3 module.
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IVIDDEC3_Fxns::control()
 */
typedef  IALG_Cmd IVIDDEC3_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDDEC3 objects.
 */
typedef struct IVIDDEC3_Fxns {
    IALG_Fxns ialg;               /**< XDAIS algorithm interface.
                                   *
                                   *   @sa IALG_Fxns
                                   */

/**
 *  @brief      Basic video decoding call.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in,out] inBufs       Input buffer descriptors.
 *  @param[in,out] outBufs      Output buffer descriptors.  The algorithm
 *                              may modify the output buffer pointers.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @remarks    process() is a blocking call.  When process() returns, the
 *              algorithm's processing is complete.
 *
 *  @remarks    process() enables codecs to support error resiliency and
 *              error concealment.  As a result, even if #IVIDDEC3_EFAIL
 *              is returned from process() because the encoded buffer has
 *              an error, it's possible that decoded buffers
 *              (@c outArgs->decodedBufs) and display buffers
 *              (@c outArgs->displayBufs) could still be returned.
 *              The codec can indicate that buffers are available by
 *              <i>not</i> setting the #XDM_ISFATALERROR bit
 *              in the respective @c displayBufs and @c decodedBufs
 *              @c extendedError field if the buffers contain valid data.
 *
 *  @remarks    By extension then, if the @c outArgs->decodedBufs and
 *              @c outArgs->displayBufs buffers are <i>not</i> valid, even
 *              if the codec's process() call returns IVIDDEC3_EFAIL, it must
 *              also be sure to set the #XDM_ISFATALERROR bit in the
 *              respective @c extendedError fields.  Failure to do so may
 *              result in applications accessing these buffers and causing
 *              system instability.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDDEC3_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDDEC3_OutArgs structure.
 *
 *  @pre        @c inBufs must not be NULL, and must point to a valid
 *              XDM2_BufDesc structure.
 *
 *  @pre        When operating in "fullframe mode" (e.g. not in data sync
 *              mode), @c inBufs->descs[0].buf must not be NULL, and must
 *              point to a valid buffer of data that is at least
 *              @c inBufs->descs[0].bufSize bytes in length.
 *
 *  @pre        When created with .inputDataMode (for @c inBufs) or
 *              .outputDataMode (for @c outBufs) set to operate in
 *              "subframe mode" (e.g. data sync mode), the appropriate
 *              .descs[].usageMode field(s)'s XDM_MEMUSAGE_DATASYNC
 *              bit must be set.  See #XDM_MemoryUsageMode for more details.
 *
 *  @pre        @c outBufs must not be NULL, and must point to a valid
 *              XDM2_BufDesc structure.
 *
 *  @pre        When operating in "fullframe mode" (e.g. not in data sync
 *              mode), @c outBufs->buf[0] must not be NULL, and must point to
 *              a valid buffer of data that is at least
 *              @c outBufs->bufSizes[0] bytes in length.
 *
 *  @pre        Unless used in data sync mode, the buffers in @c inBufs
 *              and @c outBufs are physically
 *              contiguous and owned by the calling application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c inArgs.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c inBufs, with the exception of @c inBufs.bufDesc[].accessMask.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @post       The algorithm <b>must</b> modify the contents of
 *              @c inBufs->descs[].accessMask and appropriately indicate the
 *              mode in which each of the buffers in @c inBufs were read.
 *              For example, if the algorithm only read from
 *              @c inBufs.descs[0].buf using the algorithm processor, it
 *              could utilize #XDM_SETACCESSMODE_READ to update the appropriate
 *              @c accessMask fields.
 *              The application <i>may</i> utilize these
 *              returned values to appropriately manage cache.
 *
 *  @post       The buffers in @c inBufs are
 *              owned by the calling application.
 *
 *  @retval     IVIDDEC3_EOK            @copydoc IVIDDEC3_EOK
 *  @retval     IVIDDEC3_EFAIL          @copydoc IVIDDEC3_EFAIL
 *                                      See IVIDDEC3_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC3_EUNSUPPORTED   @copydoc IVIDDEC3_EUNSUPPORTED
 */
    XDAS_Int32 (*process)(IVIDDEC3_Handle handle, XDM2_BufDesc *inBufs,
        XDM2_BufDesc *outBufs, IVIDDEC3_InArgs *inArgs,
        IVIDDEC3_OutArgs *outArgs);


/**
 *  @brief      Control behavior of an algorithm.
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c params must not be NULL, and must point to a valid
 *              IVIDDEC3_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDDEC3_Status structure.
 *
 *  @pre        If a buffer is provided in the @c status->data field,
 *              it must be physically contiguous and owned by the calling
 *              application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @post       If a buffer was provided in the @c status->data field,
 *              it is owned by the calling application.
 *
 *  @retval     IVIDDEC3_EOK            @copydoc IVIDDEC3_EOK
 *  @retval     IVIDDEC3_EFAIL          @copydoc IVIDDEC3_EFAIL
 *                                      See IVIDDEC3_Status#extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     IVIDDEC3_EUNSUPPORTED   @copydoc IVIDDEC3_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(IVIDDEC3_Handle handle, IVIDDEC3_Cmd id,
        IVIDDEC3_DynamicParams *params, IVIDDEC3_Status *status);

} IVIDDEC3_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.xdais.dm; 1, 0, 7,14; 5-24-2010 11:19:26; /db/wtree/library/trees/dais/dais.git/src/
 */

