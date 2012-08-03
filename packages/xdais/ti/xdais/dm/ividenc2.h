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
 *  @file       ti/xdais/dm/ividenc2.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video encoder
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDENC2   IVIDENC2 - XDM Video Encoder Interface
 *
 *  This is the XDM IVIDENC2 Video Encoder Interface.
 */

#ifndef ti_xdais_dm_IVIDENC2_
#define ti_xdais_dm_IVIDENC2_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_dm_IVIDENC2 */
/*@{*/

#define IVIDENC2_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define IVIDENC2_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define IVIDENC2_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */


/**
 *  @brief  Enumeration of possible motion vector (MV) accuracy
 *
 *  @enumWarning
 */
typedef enum {
    IVIDENC2_MOTIONVECTOR_PIXEL = 0,     /**< Motion vectors accuracy is only integer pel. */
    IVIDENC2_MOTIONVECTOR_HALFPEL = 1,   /**< Motion vectors accuracy is half pel. */
    IVIDENC2_MOTIONVECTOR_QUARTERPEL = 2,/**< Motion vectors accuracy is quarter pel. */
    IVIDENC2_MOTIONVECTOR_EIGHTHPEL = 3, /**< Motion vectors accuracy is one-eighth pel. */
    IVIDENC2_MOTIONVECTOR_MAX = 4        /**< Motion vectors accuracy is not defined */
} IVIDENC2_MotionVectorAccuracy;

/**
 *  @brief      Video frame control
 *
 *  @enumWarning
 *
 *  @extendedEnum
 *
 *  @remarks    This enumeration provides the application with some frame
 *              level control of the video encoder.
 *
 *  @sa IVIDENC2_InArgs.control
 */
typedef enum {
    IVIDENC2_CTRL_NONE = 0,        /**< No control operations. */
    IVIDENC2_CTRL_FORCESKIP = 1,   /**< Skip frame if it is not IDR/I frame. */
    /** Default settings. */
    IVIDENC2_CTRL_DEFAULT = IVIDENC2_CTRL_NONE
} IVIDENC2_Control;


/**
 *  @brief      This must be the first field of all IVIDENC2
 *              instance objects
 */
typedef struct IVIDENC2_Obj {
    struct IVIDENC2_Fxns *fxns;
} IVIDENC2_Obj;


/**
 *  @brief      Opaque handle to an IVIDENC2 objects.
 */
typedef struct IVIDENC2_Obj *IVIDENC2_Handle;

/**
 *  @brief      Default codec profile
 *
 *  @remarks    This definition is often used when the a particular codec
 *              doesn't have a profile, or the application doesn't
 *              know which profile the codec should use.
 *
 *  @sa IVIDENC2_Params.profile
 */
#define IVIDENC2_DEFAULTPROFILE (-1)

/**
 *  @brief      Default codec level
 *
 *  @remarks    This definition is often used when the a particular codec
 *              doesn't have a level, or the application doesn't
 *              know which profile the codec should use.
 *
 *  @sa IVIDENC2_Params.level
 */
#define IVIDENC2_DEFAULTLEVEL (-1)

/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDENC2 instance objects
 *
 *  @extensibleStruct
 */
typedef struct IVIDENC2_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 encodingPreset;  /**< Encoding preset. */
    XDAS_Int32 rateControlPreset;/**< @copydoc IVIDEO_RateControlPreset
                                 *
                                 *   @sa IVIDEO_RateControlPreset
                                 */
    XDAS_Int32 maxHeight;       /**< Maximum video height in pixels. */
    XDAS_Int32 maxWidth;        /**< Maximum video width in pixels. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 maxInterFrameInterval;/**< I to P frame distance. e.g. = 1 if
                                 *   no B frames, 2 to insert one B frame.
                                 *
                                 *   @remarks   This is used for setting the
                                 *              maximum number of B frames
                                 *              between two refererence frames.
                                 */
    XDAS_Int32 maxBitRate;      /**< Maximum Bit-rate for encoding in bits
                                 *   per second
                                 */
    XDAS_Int32 minBitRate;      /**< Minumum Bit-rate for encoding in bits
                                 *   per second
                                 */
    XDAS_Int32 inputChromaFormat;/**< Chroma format for the input buffer.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 inputContentType;/**< Video content type of the buffer being
                                 *   encoded.
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 operatingMode;   /**< Video coding mode of operation.
                                 *
                                 *   @sa IVIDEO_OperatingMode
                                 */
    XDAS_Int32 profile;         /**< Profile indicator of video codec
                                 *
                                 *   @remarks   Only one default value
                                 *              (#IVIDENC2_DEFAULTPROFILE) is
                                 *              defined by XDM for this field.
                                 *              The reason for not
                                 *              defining further values
                                 *              is to keep profile values as
                                 *              defined by video codec
                                 *              standards.
                                 *
                                 *   @sa IVIDENC2_Status.profile
                                 */
    XDAS_Int32 level;           /**< Level indicator of video codec
                                 *
                                 *   @remarks   Only one default value
                                 *              (#IVIDENC2_DEFAULTLEVEL) is
                                 *              defined by XDM for this field.
                                 *              The reason for not
                                 *              defining further values
                                 *              is to keep profile values as
                                 *              defined by video codec
                                 *              standards.
                                 *
                                 *   @sa IVIDENC2_Status.level
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDENC2_Fxns::control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              #IVIDENC2_Fxns::process() to
                                 *              set the
                                 *              IVIDENC2_DynamicParams::getDataFxn
                                 *              and
                                 *              IVIDENC2_DynamicParams::getDataHandle.
                                 *              Else, the alg can return
                                 *              error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDENC2_Fxns::control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              #IVIDENC2_Fxns::process() to
                                 *              set the
                                 *              IVIDENC2_DynamicParams::putDataFxn,
                                 *              IVIDENC2_DynamicParams::putDataHandle
                                 *              (and optionally
                                 *              IVIDENC2_DynamicParams::getBufferFxn,
                                 *              and
                                 *              IVIDENC2_DynamicParams::getBufferHandle).
                                 *              Else, the alg can return
                                 *              error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 numInputDataUnits; /**< Number of input slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c inputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks  Ignored if @c inputDataMode
                                 *             is set to full frame mode.
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c outputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks  Ignored if @c outputDataMode
                                 *             is set to full frame mode.
                                 */
    XDAS_Int32 metadataType[IVIDEO_MAX_NUM_METADATA_PLANES];/**< Type of
                                 *   each metadata plane.
                                 *
                                 *   @sa IVIDEO_MetadataType
                                 */
} IVIDENC2_Params;


/**
 *  @brief      This structure defines the algorithm parameters that can be
 *              modified after creation via control() calls
 *
 *  @remarks    It is not necessary that a given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC2_Fxns::control()
 */
typedef struct IVIDENC2_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 inputHeight;     /**< Input frame height. */
    XDAS_Int32 inputWidth;      /**< Input frame width. */
    XDAS_Int32 refFrameRate;    /**< Reference, or input, frame rate in
                                 *   fps * 1000.
                                 *
                                 *   @remarks   For example, if ref frame
                                 *              rate is 30 frames per second,
                                 *              this field will be 30000.
                                 */
    XDAS_Int32 targetFrameRate; /**< Target frame rate in
                                 *   fps * 1000.
                                 *
                                 *   @remarks   For example, if target frame
                                 *              rate is 30 frames per second,
                                 *              this field will be 30000.
                                 */
    XDAS_Int32 targetBitRate;   /**< Target bit rate in bits per second. */
    XDAS_Int32 intraFrameInterval;/**< The number of frames between two I
                                 *    frames.  For example, 30.
                                 *
                                 *    @remarks  For example, this field will be:
                                 *      - 0 - Only first frame to be intra
                                 *        coded.  e.g. IPPPPPP...
                                 *      - 1 - No inter frames (all intra
                                 *        frames).
                                 *      - 2 - Consecutive IPIPIP... sequence (if
                                 *        no B frames).
                                 *      - 3 - IPPIPP... or IPBIPBI... and so on.
                                 */
    XDAS_Int32 generateHeader;  /**< @copydoc XDM_EncMode
                                 *
                                 *   @sa XDM_EncMode
                                 */
    XDAS_Int32 captureWidth;    /**< DEFAULT(0): use imagewidth as
                                 *   pitch else use given capture
                                 *   width for pitch provided it
                                 *   is greater than image width.
                                 */
    XDAS_Int32 forceFrame;      /**< Force the current (immediate) frame to be
                                 *   encoded as a specific frame type.
                                 *
                                 *   @remarks   For example, this field will be:
                                 *     - IVIDEO_NA_FRAME - No forcing of any
                                 *       specific frame type for the frame.
                                 *     - IVIDEO_I_FRAME - Force the frame to be
                                 *       encoded as I frame.
                                 *     - IVIDEO_IDR_FRAME - Force the frame to
                                 *       be encoded as an IDR frame (specific
                                 *       to H.264 codecs).
                                 *     - IVIDEO_P_FRAME - Force the frame to be
                                 *       encoded as a P frame.
                                 *     - IVIDEO_B_FRAME - Force the frame to be
                                 *       encoded as a B frame.
                                 *
                                 *   @sa IVIDEO_FrameType.
                                 */
    XDAS_Int32 interFrameInterval;/**< Number of B frames between two reference
                                 *   frames; that is, the number of B frames
                                 *   between two P frames or I/P frames.
                                 *   DEFAULT(0).
                                 *
                                 *   @remarks   For example, this field will be:
                                 *     - 0 - to use maxInterFrameInterval.
                                 *     - 1 - 0 B frames between two reference
                                 *       frames.
                                 *     - 2 - 1 B frame between two reference
                                 *       frames.
                                 *     - 3 - 2 B frames between two reference
                                 *       frames.
                                 *     - and so on...
                                 *
                                 *   @sa IVIDENC2_Params.maxInterFrameInterval.
                                 */
    XDAS_Int32 mvAccuracy;      /**< Pixel Accuracy of the motion vector
                                 *
                                 *   @remarks   This parameter allows the user
                                 *              to tune performance by
                                 *              controlling the complexity of
                                 *              motion estimation and
                                 *              compensation within the video
                                 *              encoder.
                                 *
                                 *    @sa IVIDENC2_MotionVectorAccuracy
                                 */
    XDAS_Int32 sampleAspectRatioHeight; /**< Sample aspect ratio: Height
                                 *
                                 *   @remarks   This parameter is used to
                                 *              describe the desired aspect
                                 *              ratio in the bitstream.
                                 */
    XDAS_Int32 sampleAspectRatioWidth; /**< Sample aspect ratio: Width
                                 *
                                 *   @remarks   This parameter is used to
                                 *              describe the desired aspect
                                 *              ratio in the bitstream.
                                 */
    XDAS_Int32 ignoreOutbufSizeFlag; /**< Flag to indicate that the application
                                 *   has ignored the output buffer size
                                 *   requirement.
                                 *
                                 *   @remarks   Typically video encoders
                                 *              ask for large output buffers
                                 *              (compressed bit-streams)
                                 *              assuming theoretical worst
                                 *              case.  But on memory
                                 *              constrained systems, the
                                 *              application may want to
                                 *              allocate less than this
                                 *              worst-case size, depending
                                 *              upon the use case.
                                 *              If the application provides a
                                 *              buffer that is smaller than
                                 *              the worst-case size, the
                                 *              encoder will return an error.
                                 *              To prevent the encoder from
                                 *              returning an error, the
                                 *              application can set this
                                 *              @c ignoreOutbufSizeFlag field to
                                 *              #XDAS_TRUE.  When this flag
                                 *              is set to #XDAS_TRUE, the
                                 *              encoder shouldn't return an
                                 *              error even if the output
                                 *              buffer size is less than
                                 *              requested by the codec.
                                 *
                                 *    @remarks  Valid values are #XDAS_TRUE
                                 *              and #XDAS_FALSE.
                                 */
    XDM_DataSyncPutFxn putDataFxn; /**< Optional datasync "put data" function.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              video encoder.  The encoder
                                 *              calls this function when data
                                 *              has been put into an output
                                 *              buffer.
                                 */
    XDM_DataSyncHandle putDataHandle;/**< Datasync "put data" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              codec must provide when
                                 *              calling the app-registered
                                 *              IVIDENC2_DynamicParams.putDataFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDENC2_DynamicParams.putDataFxn().
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
                                 *              video encoder.  The encoder
                                 *              calls this function to get
                                 *              partial video buffer(s)
                                 *              from the app/framework.
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
                                 *              IVIDENC2_DynamicParams.getDataFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDM_DataSyncGetBufferFxn getBufferFxn;/**< Datasync "get buffer" function.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              video encoder.  The encoder
                                 *              calls this function to obtain
                                 *              partial compressed bit-stream
                                 *              data buffers from the
                                 *              app/framework.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 */
    XDM_DataSyncHandle getBufferHandle;/**< Datasync "get buffer" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              codec must provide when
                                 *              calling the app-registered
                                 *              IVIDENC2_DynamicParam.getBufferFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDENC2_DynamicParams.getBufferFxn().
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
                                 *              video encoders.  If the
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
} IVIDENC2_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IVIDENC2 instance
 *              process function
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC2_Fxns::process()
 */
typedef struct IVIDENC2_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 inputID;         /**< Identifier to attach with the corresponding
                                 *   input frames to be encoded.
                                 *
                                 *   @remarks   This is useful when frames
                                 *              require buffering (e.g.
                                 *              B frames), and to support
                                 *              buffer management.
                                 *
                                 *   @remarks   When there is no re-ordering,
                                 *              IVIDENC2_OutArgs::outputID will
                                 *              be the same as this inputID
                                 *              field.
                                 *
                                 *   @remarks   Zero (0) is <b>not</b> a
                                 *              supported inputID.  This value
                                 *              is reserved for cases when
                                 *              there is no input buffer is
                                 *              provided.
                                 *
                                 *   @sa IVIDENC2_OutArgs::outputID.
                                 */
   XDAS_Int32 control;          /**< Encoder control operations
                                 *
                                 *   @sa IVIDENC2_Control
                                 */
} IVIDENC2_InArgs;


/**
 *  @brief      Defines instance status parameters
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC2_Fxns::control()
 */
typedef struct IVIDENC2_Status {
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
                                 *              IVIDENC2_Fxns.control()
                                 *              call.  The algorithm must
                                 *              not retain a pointer to this
                                 *              data.
                                 *
                                 *   @sa #XDM_GETVERSION
                                 */

    XDAS_Int32  encodingPreset; /**< Encoding preset. */
    XDAS_Int32  rateControlPreset;/**< @copydoc IVIDEO_RateControlPreset
                                 *
                                 *   @sa IVIDEO_RateControlPreset
                                 */
    XDAS_Int32  maxInterFrameInterval; /**< I to P frame distance. e.g. = 1 if
                                 *   no B frames, 2 to insert one B frame.
                                 *
                                 *   @remarks   This is used for setting the
                                 *              maximum number of B frames
                                 *              between two refererence frames.
                                 */
    XDAS_Int32 inputChromaFormat;/**< Chroma format for the input buffer.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 inputContentType; /**< Video content type of the buffer being
                                 *   encoded.
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 operatingMode;   /**< Video encoding mode of operation.
                                 *
                                 *   @sa IVIDEO_OperatingMode
                                 */
    XDAS_Int32 profile;         /**< Profile indicator of video codec.
                                 *
                                 *   @sa IVIDENC2_DEFAULTPROFILE
                                 *   @sa IVIDENC2_Params.profile
                                 */
    XDAS_Int32 level;           /**< Level indicator of video codec.
                                 *
                                 *   @sa IVIDENC2_DEFAULTLEVEL
                                 *   @sa IVIDENC2_Params.level
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @sa IVIDENC2_Params.inputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @sa IVIDENC2_Params.outputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 numInputDataUnits; /**< Number of input slices/rows.
                                 *
                                 *   @remarks   Units depend on the
                                 *              IVIDENC2_Params.inputDataMode,
                                 *              like number of
                                 *              slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDENC2_Params.inputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDENC2_Params.inputDataMode
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c outputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDENC2_Params.outputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDENC2_Params.outputDataMode
                                 */
   XDAS_Int32 configurationID;  /**< Configuration ID of given codec.
                                 *
                                 *   @remarks   This is used to differentiate
                                 *              multiple images of a vendor.
                                 *              It can be used by the
                                 *              framework to optimize the
                                 *              save/restore overhead of any
                                 *              resources used.
                                 *
                                 *   @remarks   This can be useful in
                                 *              multichannel use case
                                 *              scenarios.
                                 *
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
                                 *   @sa IVIDENC2_Params
                                 *   @sa XDM1_AlgBufInfo
                                 *   @sa IVIDENC2_Fxns.process()
                                 */
    XDAS_Int32 metadataType[IVIDEO_MAX_NUM_METADATA_PLANES];/**< Type of
                                 *   each metadata plane.
                                 *
                                 *   @sa IVIDEO_MetadataType
                                 */
    IVIDENC2_DynamicParams encDynamicParams;  /**< Video encoder dynamic
                                 *   parameters.
                                 *
                                 *   @todo      Need to better wordsmith this
                                 *              section.
                                 *
                                 *   @remarks   In case of extened dynamic
                                 *              Params, alg can check the
                                 *              size of status or
                                 *              DynamicParams and return
                                 *              the parameters accordingly.
                                 */
} IVIDENC2_Status;


/**
 *  @brief      Defines the run time output arguments for all IVIDENC2
 *              instance objects
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDENC2_Fxns::process()
 */
typedef struct IVIDENC2_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDAS_Int32 bytesGenerated;  /**< Number of bytes generated during the
                                 *   IVIDENC2_Fxns::process() call.
                                 */
    XDAS_Int32 encodedFrameType;/**< @copydoc IVIDEO_FrameType
                                 *
                                 *   @sa IVIDEO_FrameType
                                 */
    XDAS_Int32 inputFrameSkip;  /**< @copydoc IVIDEO_SkipMode
                                 *
                                 *   @sa IVIDEO_SkipMode
                                 */
   XDAS_Int32 freeBufID[IVIDEO2_MAX_IO_BUFFERS]; /**< This is an
                                 *   array of inputID's corresponding to the
                                 *   buffers that have been unlocked in the
                                 *   current process call.
                                 *
                                 *   @remarks   The buffers released by the
                                 *              algorithm are indicated by
                                 *              their non-zero ID (previously
                                 *              provided via
                                 *              IVIDENC2_InArgs#inputID).
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
                                 *   @sa IVIDENC2_InArgs#inputID
                                 */

    IVIDEO2_BufDesc reconBufs;  /**< Reconstruction frames. */
} IVIDENC2_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDENC2 module
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IVIDENC2_Fxns::control()
 */
typedef  IALG_Cmd IVIDENC2_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDENC2 objects
 */
typedef struct IVIDENC2_Fxns {
    IALG_Fxns   ialg;             /**< XDAIS algorithm interface.
                                   *
                                   *   @sa      IALG_Fxns
                                   */

/**
 *  @brief      Basic video encoding call
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in,out] inBufs       Input video buffer descriptors.
 *  @param[in,out] outBufs      Output buffer descriptors.  The algorithm
 *                              may modify the output buffer pointers.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @remarks    process() is a blocking call.  When process() returns, the
 *              algorithm's processing is complete.
 *
 *  @pre        @c inBufs must not be NULL, and must point to a valid
 *              IVIDEO2_BufDesc structure.
 *
 *  @pre        @c inBufs->numPlanes will indicate the total number of input
 *              buffers supplied for input frame in the @c inBufs->planeDesc[]
 *              array.
 *
 *  @pre        @c inBufs->numMetaPlanes will indicate the total number of input
 *              buffers supplied for meta data planes in the
 *              @c inBufs->metadataPlaneDesc[] array.
 *
 *  @pre        @c outBufs must not be NULL, and must point to a valid
 *              XDM2_BufDesc structure.
 *
 *  @pre        @c outBufs->buf[0] must not be NULL, and must point to
 *              a valid buffer of data that is at least
 *              @c outBufs->bufSizes[0] bytes in length.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDENC2_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDENC2_OutArgs structure.
 *
 *  @pre        The buffers in @c inBuf and @c outBuf are physically
 *              contiguous and owned by the calling application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c inArgs.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c inBufs, with the exception of @c inBufs.bufDesc[].accessMask.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @post       The algorithm <b>must</b> appropriately set/clear the
 *              IVIDEO2_BufDesc.planeDesc[].accessMask and
 *              IVIDEO2_BufDesc.metadataPlaneDesc[].accessMask fields in
 *              @c inBufs to indicate the mode in which each of the respective
 *              buffers were read.
 *              For example, if the algorithm only read from
 *              @c inBufs.planeDesc[0].buf using the algorithm processor, it
 *              could utilize #XDM_SETACCESSMODE_READ to update the appropriate
 *              @c accessMask fields.
 *              The application <i>may</i> utilize these
 *              returned values to appropriately manage cache.
 *
 *  @post       The buffers in @c inBufs are
 *              owned by the calling application.
 *
 *  @retval     #IVIDENC2_EOK           @copydoc IVIDENC2_EOK
 *  @retval     #IVIDENC2_EFAIL         @copydoc IVIDENC2_EFAIL
 *                                      See IVIDENC2_Status.extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     #IVIDENC2_EUNSUPPORTED  @copydoc IVIDENC2_EUNSUPPORTED
 *
 *  @todo       Need to review these comments.  Not sure @c inBufs and
 *              @c outBufs are correctly described.
 */
    XDAS_Int32 (*process)(IVIDENC2_Handle handle, IVIDEO2_BufDesc *inBufs,
            XDM2_BufDesc *outBufs, IVIDENC2_InArgs *inArgs,
            IVIDENC2_OutArgs *outArgs);


/**
 *  @brief      Control behavior of an algorithm
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
 *              IVIDENC2_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDENC2_Status structure.
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
 *  @retval     #IVIDENC2_EOK           @copydoc IVIDENC2_EOK
 *  @retval     #IVIDENC2_EFAIL         @copydoc IVIDENC2_EFAIL
 *                                      See IVIDENC2_Status.extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     #IVIDENC2_EUNSUPPORTED  @copydoc IVIDENC2_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(IVIDENC2_Handle handle, IVIDENC2_Cmd id,
            IVIDENC2_DynamicParams *params, IVIDENC2_Status *status);

} IVIDENC2_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.xdais.dm; 1, 0, 7,1; 6-19-2012 17:57:46; /db/wtree/library/trees/dais/dais-w06/src/ xlibrary

 */

