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
 *  ======== videnc2.h ========
 */
/**
 *  @file       ti/sdo/ce/video2/videnc2.h
 *
 *  @brief      The VIDENC2 video encoder interface.  Provides the user an
 *              interface to create and interact with XDAIS algorithms that are
 *              compliant with the XDM-defined IVIDENC2 video encoder
 *              interface.
 */
/**
 *  @defgroup   ti_sdo_ce_video2_VIDENC2    VIDENC2 - Video Encoder Interface
 *
 *  This is the VIDENC2 video encoder interface.  Several of the data
 *  types in this API are specified by the XDM IVIDENC2 interface; please see
 *  the XDM documentation for those details.
 */

#ifndef ti_sdo_ce_video2_VIDENC2_
#define ti_sdo_ce_video2_VIDENC2_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ividenc2.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/visa.h>
#include <ti/sdo/ce/skel.h>

/** @ingroup    ti_sdo_ce_video2_VIDENC2 */
/*@{*/

#define VIDENC2_EOK      IVIDENC2_EOK         /**< @copydoc IVIDENC2_EOK */
#define VIDENC2_EFAIL    IVIDENC2_EFAIL       /**< @copydoc IVIDENC2_EFAIL */

/** @copydoc IVIDENC2_EUNSUPPORTED */
#define VIDENC2_EUNSUPPORTED IVIDENC2_EUNSUPPORTED

#define VIDENC2_ETIMEOUT VISA_ETIMEOUT       /**< @copydoc VISA_ETIMEOUT */
#define VIDENC2_FOREVER  VISA_FOREVER        /**< @copydoc VISA_FOREVER */


/**
 *  @brief      Opaque handle to a VIDENC2 codec.
 */
typedef VISA_Handle VIDENC2_Handle;

/* The following are just wrapper typedefs */

/** @copydoc IVIDENC2_Params */
typedef struct IVIDENC2_Params VIDENC2_Params;

/** @copydoc IVIDENC2_InArgs */
typedef IVIDENC2_InArgs VIDENC2_InArgs;

/** @copydoc IVIDENC2_OutArgs */
typedef IVIDENC2_OutArgs VIDENC2_OutArgs;

/** @copydoc IVIDENC2_Cmd */
typedef IVIDENC2_Cmd VIDENC2_Cmd;

/** @copydoc IVIDENC2_DynamicParams */
typedef IVIDENC2_DynamicParams VIDENC2_DynamicParams;

/** @copydoc IVIDENC2_Status */
typedef IVIDENC2_Status VIDENC2_Status;

/** @cond INTERNAL */

/**
 *  @brief      An implementation of the skel interface; the skeleton side
 *              of the stubs.
 */
extern SKEL_Fxns VIDENC2_SKEL;

/**
 *  @brief      Implementation of the IVIDENC interface that is run remotely.
 */
extern IVIDENC2_Fxns VIDENC2_STUBS;

/** @endcond */

/**
 *  @brief      Definition of IVIDENC2 codec class configurable parameters
 *
 *  @sa         VISA_getCodecClassConfig()
 */
typedef struct IVIDENC2_CodecClassConfig {
    Bool manageInBufsPlaneDescCache[IVIDEO_MAX_NUM_PLANES];
    Bool manageInBufsMetaPlaneDescCache[IVIDEO_MAX_NUM_METADATA_PLANES];
    Bool manageOutBufsCache[XDM_MAX_IO_BUFFERS];
} IVIDENC2_CodecClassConfig;


/*
 *  ======== VIDENC2_control ========
 */
/**
 *  @brief      Execute the control() method in this instance of a video
 *              encoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video encoder instance.
 *  @param[in]  id      Command id for XDM control operation.
 *  @param[in]  params  Runtime control parameters used for encoding.
 *  @param[out] status  Status info upon completion of encode operation.
 *
 *  @pre        @c handle is a valid (non-NULL) video encoder handle
 *              and the video encoder is in the created state.
 *
 *  @retval     #VIDENC2_EOK         Success.
 *  @retval     #VIDENC2_EFAIL       Failure.
 *  @retval     #VIDENC2_EUNSUPPORTED    The requested operation
 *                                       is not supported.
 *
 *  @remark     This is a blocking call, and will return after the control
 *              command has been executed.
 *
 *  @remark     If an error is returned, @c status->extendedError may
 *              indicate further details about the error.  See
 *              #VIDENC2_Status::extendedError for details.
 *
 *  @sa         VIDENC2_create()
 *  @sa         VIDENC2_delete()
 *  @sa         IVIDENC2_Fxns::process()
 */
extern Int32 VIDENC2_control(VIDENC2_Handle handle, VIDENC2_Cmd id,
        VIDENC2_DynamicParams *params, VIDENC2_Status *status);


/*
 *  ======== VIDENC2_create ========
 */
/**
 *  @brief      Create an instance of a video encoder algorithm.
 *
 *  Instance handles must not be concurrently accessed by multiple threads;
 *  each thread must either obtain its own handle (via VIDENC2_create()) or
 *  explicitly serialize access to a shared handle.
 *
 *  @param[in]  e       Handle to an opened engine.
 *  @param[in]  name    String identifier of the type of video encoder
 *                      to create.
 *  @param[in]  params  Creation parameters.
 *
 *  @retval     NULL            An error has occurred.
 *  @retval     non-NULL        The handle to the newly created video encoder
 *                              instance.
 *
 *  @remarks    @c params is optional.  If it's not supplied, codec-specific
 *              default params will be used.
 *
 *  @remark     Depending on the configuration of the engine opened, this
 *              call may create a local or remote instance of the video
 *              encoder.
 *
 *  @codecNameRemark
 *
 *  @sa         Engine_open()
 *  @sa         VIDENC2_delete()
 */
extern VIDENC2_Handle VIDENC2_create(Engine_Handle e, String name,
    VIDENC2_Params *params);


/*
 *  ======== VIDENC2_delete ========
 */
/**
 *  @brief      Delete the instance of a video encoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video encoder instance.
 *
 *  @remark     Depending on the configuration of the engine opened, this
 *              call may delete a local or remote instance of the video
 *              encoder.
 *
 *  @pre        @c handle is a valid (non-NULL) handle which is
 *              in the created state.
 *
 *  @post       All resources allocated as part of the VIDENC2_create()
 *              operation (memory, DMA channels, etc.) are freed.
 *
 *  @sa         VIDENC2_create()
 */
extern Void VIDENC2_delete(VIDENC2_Handle handle);


/*
 *  ======== VIDENC2_process ========
 */
/**
 *  @brief      Execute the process() method in this instance of a video
 *              encoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video encoder instance.
 *  @param[in]  inBufs  A buffer descriptor containing input buffers.
 *  @param[out] outBufs A buffer descriptor containing output buffers.
 *  @param[in]  inArgs  Input Arguments.
 *  @param[out] outArgs Output Arguments.
 *
 *  @pre        @c handle is a valid (non-NULL) video encoder handle
 *              and the video encoder is in the created state.
 *
 *  @retval     #VIDENC2_EOK         Success.
 *  @retval     #VIDENC2_EFAIL       Failure.
 *  @retval     #VIDENC2_EUNSUPPORTED    The requested operation
 *                                       is not supported.
 *
 *  @remark     Since the VIDENC2 decoder contains support for asynchronous
 *              buffer submission and retrieval, this API becomes known as
 *              synchronous in nature.
 *
 *  @remark     This is a blocking call, and will return after the data
 *              has been encoded.
 *
 *  @remark     The buffers supplied to VIDENC2_process() may have constraints
 *              put on them.  For example, in dual-processor, shared memory
 *              architectures, where the codec is running on a remote
 *              processor, the buffers may need to be physically contiguous.
 *              Additionally, the remote processor may place restrictions on
 *              buffer alignment.
 *
 *  @remark     If an error is returned, @c outArgs->extendedError may
 *              indicate further details about the error.  See
 *              #VIDENC2_OutArgs::extendedError for details.
 *
 *  @sa         VIDENC2_create()
 *  @sa         VIDENC2_delete()
 *  @sa         VIDENC2_control()
 *  @sa         VIDENC2_processAsync()
 *  @sa         VIDENC2_processWait()
 *  @sa         IVIDENC2_Fxns::process() - the reflected algorithm interface,
 *                                         which may contain further usage
 *                                         details.
 */
extern Int32 VIDENC2_process(VIDENC2_Handle handle, IVIDEO2_BufDesc *inBufs,
        XDM2_BufDesc *outBufs, VIDENC2_InArgs *inArgs,
        VIDENC2_OutArgs *outArgs);


/*
 *  ======== VIDENC2_processAsync ========
 */
/**
 *  @brief      Perform asynchronous submission to this instance of a video
 *              decoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video decoder instance.
 *  @param[in]  inBufs  A buffer descriptor containing input buffers.
 *  @param[out] outBufs A buffer descriptor containing output buffers.
 *  @param[in]  inArgs  Input Arguments.
 *  @param[out] outArgs Output Arguments.
 *
 *  @pre        @c handle is a valid (non-NULL) video decoder handle
 *              and the video decoder is in the created state.
 *
 *  @retval     #VIDENC2_EOK         Success.
 *  @retval     #VIDENC2_EFAIL       Failure.
 *  @retval     #VIDENC2_EUNSUPPORTED Unsupported request.
 *
 *  @remark     This API is the asynchronous counterpart to the process()
 *              method.  It allows for buffer and argument submission without
 *              waiting for retrieval.  A response is retrieved using the
 *              VIDENC2_processWait() API.
 *
 *  @remark     The buffers supplied to VIDENC2_processAsync() may have
 *              constraints put on them.  For example, in dual-processor,
 *              shared memory architectures, where the codec is running on a
 *              remote processor, the buffers may need to be physically
 *              contiguous.  Additionally, the remote processor may place
 *              restrictions on buffer alignment.
 *
 *  @sa         VIDENC2_create()
 *  @sa         VIDENC2_delete()
 *  @sa         VIDENC2_control()
 *  @sa         VIDENC2_process()
 *  @sa         VIDENC2_processWait()
 *  @sa         IVIDENC2_Fxns::process()
 */
extern XDAS_Int32 VIDENC2_processAsync(VIDENC2_Handle handle,
        IVIDEO2_BufDesc *inBufs, XDM2_BufDesc *outBufs,
        IVIDENC2_InArgs *inArgs, IVIDENC2_OutArgs *outArgs);


/*
 *  ======== VIDENC2_processWait ========
 */
/**
 *  @brief      Wait for a return message from a previous invocation of
 *              VIDENC2_processAsync() in this instance of an video decoder
 *              algorithm.
 *
 *  @param[in]  handle  Handle to a created video decoder instance.
 *  @param[in]  inBufs  A buffer descriptor containing input buffers.
 *  @param[out] outBufs A buffer descriptor containing output buffers.
 *  @param[in]  inArgs  Input Arguments.
 *  @param[out] outArgs Output Arguments.
 *  @param[in]  timeout Amount of "time" to wait (from 0 -> #VIDENC2_FOREVER)
 *
 *  @pre        @c handle is a valid (non-NULL) video decoder handle
 *              and the video decoder is in the created state.
 *
 *  @retval     #VIDENC2_EOK         Success.
 *  @retval     #VIDENC2_EFAIL       Failure.
 *  @retval     #VIDENC2_EUNSUPPORTED Unsupported request.
 *  @retval     #VIDENC2_ETIMEOUT    Operation timed out.
 *
 *  @remark     This is a blocking call, and will return after the data
 *              has been decoded.
 *
 *  @remark     "Polling" is supported by using a timeout of 0.  Waiting
 *              forever is supported by using a timeout of #VIDENC2_FOREVER.
 *
 *  @remark     There must have previously been an invocation of the
 *              VIDENC2_processAsync() API.
 *
 *  @remark     The buffers supplied to VIDENC2_processAsync() may have
 *              constraints put on them.  For example, in dual-processor,
 *              shared memory architectures, where the codec is running on a
 *              remote processor, the buffers may need to be physically
 *              contiguous.  Additionally, the remote processor may place
 *              restrictions on buffer alignment.
 *
 *  @sa         VIDENC2_create()
 *  @sa         VIDENC2_delete()
 *  @sa         VIDENC2_control()
 *  @sa         VIDENC2_process()
 *  @sa         VIDENC2_processAsync()
 */
extern XDAS_Int32 VIDENC2_processWait(VIDENC2_Handle handle,
        IVIDEO2_BufDesc *inBufs, XDM2_BufDesc *outBufs, IVIDENC2_InArgs *inArgs,
        IVIDENC2_OutArgs *outArgs, UInt timeout);


/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce.video2; 1, 0, 2,146; 7-27-2010 22:26:14; /db/atree/library/trees/ce/ce-q08x/src/
 */

