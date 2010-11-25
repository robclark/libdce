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
 *  ======== skel.h ========
 */
/**
 *  @file       ti/sdo/ce/skel.h
 *
 *  @brief      The Codec Engine System Programming Interface (SPI) for
 *              skeleton developers.
 */
/**
 *  @addtogroup ti_sdo_ce_SKEL      CE Skeleton SPI
 */

#ifndef ti_sdo_ce_SKEL_
#define ti_sdo_ce_SKEL_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/sdo/ce/visa.h>

/** @ingroup    ti_sdo_ce_SKEL */
/*@{*/

/**
 *  @brief  Prototype for a skeleton's call() implementation
 *
 *  @param[in]      handle  A handle to the current skeleton.
 *  @param[in]      msg     A message sent by the algorithm's stub
 */
typedef VISA_Status (*SKEL_CALLFXN)(VISA_Handle handle, VISA_Msg msg);

/**
 *  @brief  Prototype for a skeleton's create() API.
 *
 *  @param[in]      reserved    Reserved.
 *  @param[in]      name        Name of the algorithm to create.
 *  @param[in]      params      Creation parameters for the algorithm.
 *
 *  @retval         NULL        Error, unable to create the algorithm.
 *  @retval         non-NULL    Handle to the successfully created algorithm.
 */
typedef VISA_Handle (*SKEL_CREATEFXN)(Void *reserved, String name,
    Void *params);

/**
 *  @brief  Prototype for a skeleton's destroy() API.
 *
 *  @param[in]      handle  A handle to the current skeleton.
 *  @param[in]      msg     A message sent by the algorithm's stub
 */
typedef Void (*SKEL_DESTROYFXN)(VISA_Handle handle);


/*
 *  ======== SKEL_Fxns ========
 */
/**
 *  @brief      Table of functions defining the interface of a skeleton.
 */
typedef struct SKEL_Fxns {
    SKEL_CALLFXN    call;       /**< A skeleton's "call" implementation. */
    SKEL_CREATEFXN  apiCreate;  /**< A skeleton's "create" API. */
    SKEL_DESTROYFXN apiDestroy; /**< A skeleton's "destroy" API. */
} SKEL_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _SKEL_ */
/*
 *  @(#) ti.sdo.ce; 1, 0, 6,403; 7-27-2010 22:02:17; /db/atree/library/trees/ce/ce-q08x/src/
 */

