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
 *  ======== viddec3.h ========
 */
/**
 *  @file       ti/sdo/ce/video3/viddec3.h
 *
 *  @brief      The VIDDEC3 video decoder interface.  Provides the user an
 *              interface to create and interact with XDAIS algorithms that are
 *              compliant with the XDM-defined IVIDDEC3 video decoder
 *              interface.
 */
/**
 *  @defgroup   ti_sdo_ce_video3_VIDDEC3 VIDDEC3 - Video Decoder Interface
 *
 *  This is the VIDDEC3 video decoder interface.  Several of the data
 *  types in this API are specified by the XDM IVIDDEC3 interface; please see
 *  the XDM documentation for those details.
 */

#ifndef ti_sdo_ce_video3_VIDDEC3_
#define ti_sdo_ce_video3_VIDDEC3_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ividdec3.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/visa.h>
#include <ti/sdo/ce/skel.h>

/** @ingroup    ti_sdo_ce_video3_VIDDEC3 */
/*@{*/

#define VIDDEC3_EOK      IVIDDEC3_EOK         /**< @copydoc IVIDDEC3_EOK */
#define VIDDEC3_EFAIL    IVIDDEC3_EFAIL       /**< @copydoc IVIDDEC3_EFAIL */

/**< @copydoc IVIDDEC3_EUNSUPPORTED */
#define VIDDEC3_EUNSUPPORTED IVIDDEC3_EUNSUPPORTED

#define VIDDEC3_ETIMEOUT VISA_ETIMEOUT        /**< @copydoc VISA_ETIMEOUT */
#define VIDDEC3_FOREVER  VISA_FOREVER         /**< @copydoc VISA_FOREVER */


/**
 *  @brief      Opaque handle to a VIDDEC3 codec.
 */
typedef VISA_Handle VIDDEC3_Handle;

/** @copydoc IVIDDEC3_Params */
typedef struct IVIDDEC3_Params VIDDEC3_Params;

/** @copydoc IVIDDEC3_InArgs */
typedef IVIDDEC3_InArgs VIDDEC3_InArgs;

/** @copydoc IVIDDEC3_OutArgs */
typedef IVIDDEC3_OutArgs VIDDEC3_OutArgs;

/** @copydoc IVIDDEC3_Cmd */
typedef IVIDDEC3_Cmd VIDDEC3_Cmd;

/** @copydoc IVIDDEC3_DynamicParams */
typedef IVIDDEC3_DynamicParams   VIDDEC3_DynamicParams;

/** @copydoc IVIDDEC3_Status */
typedef IVIDDEC3_Status VIDDEC3_Status;


/** @cond INTERNAL */

/**
 *  @brief      An implementation of the skel interface; the skeleton side
 *              of the stubs.
 */
extern SKEL_Fxns VIDDEC3_SKEL;

/**
 *  @brief      Implementation of the IVIDDEC3 interface that is run remotely.
 */
extern IVIDDEC3_Fxns VIDDEC3_STUBS;

/** @endcond */

/**
 *  @brief      Definition of IVIDDEC3 codec class configurable parameters
 *
 *  @sa         VISA_getCodecClassConfig()
 */
typedef struct IVIDDEC3_CodecClassConfig {
    Bool manageInBufsCache     [ XDM_MAX_IO_BUFFERS ];
    Bool manageOutBufsCache    [ XDM_MAX_IO_BUFFERS ];
} IVIDDEC3_CodecClassConfig;


/*
 *  ======== VIDDEC3_control ========
 */
/**
 *  @brief      Execute the control() method in this instance of a video
 *              decoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video decoder instance.
 *  @param[in]  id      Command id for XDM control operation.
 *  @param[in]  params  Runtime control parameters used for decoding.
 *  @param[out] status  Status info upon completion of decode operation.
 *
 *  @pre        @c handle is a valid (non-NULL) video decoder handle
 *              and the video decoder is in the created state.
 *
 *  @retval     #VIDDEC3_EOK         Success.
 *  @retval     #VIDDEC3_EFAIL       Failure.
 *  @retval     #VIDDEC3_EUNSUPPORTED Unsupported request.
 *
 *  @remark     This is a blocking call, and will return after the control
 *              command has been executed.
 *
 *  @remark     If an error is returned, @c status->extendedError may
 *              indicate further details about the error.  See #XDM_ErrorBit
 *              for details.
 *
 *  @sa         VIDDEC3_create()
 *  @sa         VIDDEC3_delete()
 *  @sa         IVIDDEC3_Fxns::process()
 */
extern Int32 VIDDEC3_control(VIDDEC3_Handle handle, VIDDEC3_Cmd id,
    VIDDEC3_DynamicParams *params, VIDDEC3_Status *status);


/*
 *  ======== VIDDEC3_create ========
 */
/**
 *  @brief      Create an instance of a video decoder algorithm.
 *
 *  Instance handles must not be concurrently accessed by multiple threads;
 *  each thread must either obtain its own handle (via VIDDEC3_create) or
 *  explicitly serialize access to a shared handle.
 *
 *  @param[in]  e       Handle to an opened engine.
 *  @param[in]  name    String identifier of the type of video decoder
 *                      to create.
 *  @param[in]  params  Creation parameters.
 *
 *  @retval     NULL            An error has occurred.
 *  @retval     non-NULL        The handle to the newly created video decoder
 *                              instance.
 *
 *  @remark     @c params is optional.  If it's not supplied, codec-specific
 *              default params will be used.
 *
 *  @remark     Depending on the configuration of the engine opened, this
 *              call may create a local or remote instance of the video
 *              decoder.
 *
 *  @codecNameRemark
 *
 *  @sa         Engine_open()
 *  @sa         VIDENC3_delete()
 */
extern VIDDEC3_Handle VIDDEC3_create(Engine_Handle e, String name,
    VIDDEC3_Params *params);


/*
 *  ======== VIDDEC3_delete ========
 */
/**
 *  @brief      Delete the instance of a video decoder algorithm.
 *
 *  @param[in]  handle  Handle to a created video decoder instance.
 *
 *  @remark     Depending on the configuration of the engine opened, this
 *              call may delete a local or remote instance of the video
 *              decoder.
 *
 *  @pre        @c handle is a valid (non-NULL) handle which is
 *              in the created state.
 *
 *  @post       All resources allocated as part of the VIDDEC3_create()
 *              operation (memory, DMA channels, etc.) are freed.
 *
 *  @sa         VIDDEC3_create()
 */
extern Void VIDDEC3_delete(VIDDEC3_Handle handle);


/*
 *  ======== VIDDEC3_process ========
 */
/**
 *  @brief      Execute the process() method in this instance of a video
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
 *  @retval     #VIDDEC3_EOK         Success.
 *  @retval     #VIDDEC3_EFAIL       Failure.
 *  @retval     #VIDDEC3_EUNSUPPORTED Unsupported request.
 *
 *  @remark     Since the VIDDEC3 decoder contains support for asynchronous
 *              buffer submission and retrieval, this API becomes known as
 *              synchronous in nature.
 *
 *  @remark     This is a blocking call, and will return after the data
 *              has been decoded.
 *
 *  @remark     The buffers supplied to VIDDEC3_process() may have constraints
 *              put on them.  For example, in dual-processor, shared memory
 *              architectures, where the codec is running on a remote
 *              processor, the buffers may need to be physically contiguous.
 *              Additionally, the remote processor may place restrictions on
 *              buffer alignment.
 *
 *  @remark     If an error is returned, @c outArgs->extendedError may
 *              indicate further details about the error.  See #XDM_ErrorBit
 *              for details.
 *
 *  @sa         VIDDEC3_create()
 *  @sa         VIDDEC3_delete()
 *  @sa         VIDDEC3_control()
 *  @sa         VIDDEC3_processAsync()
 *  @sa         VIDDEC3_processWait()
 *  @sa         IVIDDEC3_Fxns::process()
 */
extern Int32 VIDDEC3_process(VIDDEC3_Handle handle, XDM2_BufDesc *inBufs,
    XDM2_BufDesc *outBufs, VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs);


#if 0 /* async not yet supported */

/*
 *  ======== VIDDEC3_processAsync ========
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
 *  @retval     #VIDDEC3_EOK         Success.
 *  @retval     #VIDDEC3_EFAIL       Failure.
 *  @retval     #VIDDEC3_EUNSUPPORTED Unsupported request.
 *
 *  @remark     This API is the asynchronous counterpart to the process()
 *              method.  It allows for buffer and argument submission without
 *              waiting for retrieval.  A response is retrieved using the
 *              VIDDEC3_processWait() API.
 *
 *  @remark     The buffers supplied to VIDDEC3_processAsync() may have
 *              constraints put on them.  For example, in dual-processor,
 *              shared memory architectures, where the codec is running on a
 *              remote processor, the buffers may need to be physically
 *              contiguous.  Additionally, the remote processor may place
 *              restrictions on buffer alignment.
 *
 *  @sa         VIDDEC3_create()
 *  @sa         VIDDEC3_delete()
 *  @sa         VIDDEC3_control()
 *  @sa         VIDDEC3_process()
 *  @sa         VIDDEC3_processWait()
 *  @sa         IVIDDEC3_Fxns::process()
 */
extern XDAS_Int32 VIDDEC3_processAsync(VIDDEC3_Handle handle,
    XDM1_BufDesc *inBufs, XDM_BufDesc *outBufs,
    VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs);

/*
 *  ======== VIDDEC3_processWait ========
 */
/**
 *  @brief      Wait for a return message from a previous invocation of
 *              VIDDEC3_processAsync() in this instance of an video decoder
 *              algorithm.
 *
 *  @param[in]  handle  Handle to a created video decoder instance.
 *  @param[in]  inBufs  A buffer descriptor containing input buffers.
 *  @param[out] outBufs A buffer descriptor containing output buffers.
 *  @param[in]  inArgs  Input Arguments.
 *  @param[out] outArgs Output Arguments.
 *  @param[in]  timeout Amount of "time" to wait (from 0 -> VIDDEC3_FOREVER)
 *
 *  @pre        @c handle is a valid (non-NULL) video decoder handle
 *              and the video decoder is in the created state.
 *
 *  @retval     #VIDDEC3_EOK         Success.
 *  @retval     #VIDDEC3_EFAIL       Failure.
 *  @retval     #VIDDEC3_EUNSUPPORTED Unsupported request.
 *  @retval     #VIDDEC3_ETIMEOUT    Operation timed out.
 *
 *  @remark     This is a blocking call, and will return after the data
 *              has been decoded.
 *
 *  @remark     "Polling" is supported by using a timeout of 0.  Waiting
 *              forever is supported by using a timeout of VIDDEC3_EFOREVER.
 *
 *  @remark     There must have previously been an invocation of the
 *              VIDDEC3_processAsync() API.
 *
 *  @remark     The buffers supplied to VIDDEC3_processAsync() may have
 *              constraints put on them.  For example, in dual-processor,
 *              shared memory architectures, where the codec is running on a
 *              remote processor, the buffers may need to be physically
 *              contiguous.  Additionally, the remote processor may place
 *              restrictions on buffer alignment.
 *
 *  @sa         VIDDEC3_create()
 *  @sa         VIDDEC3_delete()
 *  @sa         VIDDEC3_control()
 *  @sa         VIDDEC3_process()
 *  @sa         VIDDEC3_processAsync()
 */
extern XDAS_Int32 VIDDEC3_processWait(VIDDEC3_Handle handle,
    XDM1_BufDesc *inBufs, XDM_BufDesc *outBufs,
    VIDDEC3_InArgs *inArgs, VIDDEC3_OutArgs *outArgs, UInt timeout);

#endif


/*@}*/  /* ingroup */

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce.video3; 1, 0, 0,93; 7-27-2010 22:27:25; /db/atree/library/trees/ce/ce-q08x/src/
 */

