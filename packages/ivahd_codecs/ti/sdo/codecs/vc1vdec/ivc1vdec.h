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
 */


#ifndef __IVC1VDEC_H__
#define __IVC1VDEC_H__

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/ividdec3.h>

typedef struct IVC1VDEC_Status {
  IVIDDEC3_Status viddecStatus;
  XDAS_Int32 eRes;
  XDAS_UInt32 extendedErrorCode0;
  XDAS_UInt32 extendedErrorCode1;
  XDAS_UInt32 extendedErrorCode2;
  XDAS_UInt32 extendedErrorCode3;
} IVC1VDEC_Status;

typedef struct IVC1VDEC_Params {
  IVIDDEC3_Params viddecParams;
  XDAS_Int32 ErrorConcealmentON;
  XDAS_Int32 FrameLayerDataPresentFlag;
} IVC1VDEC_Params;

typedef struct IVC1VDEC_DynamicParams {
  IVIDDEC3_DynamicParams viddecDynamicParams;
  XDAS_Int32 outloopDeblocking;
  XDAS_Int32 reserved;
} IVC1VDEC_DynamicParams;

typedef struct IVC1VDEC_InArgs {
  IVIDDEC3_InArgs viddecInArgs;
}IVC1VDEC_InArgs;

typedef struct IVC1VDEC_OutArgs {
  IVIDDEC3_OutArgs viddecOutArgs;
  XDAS_Int32 *mv_table_ptr;
  XDAS_UInt8 pixelRange;
  XDAS_UInt16 parWidth;
  XDAS_UInt16 parHeight;
  XDAS_UInt16 numErrMbs;
#ifndef NO_RCV_HEADER_PARSING
  XDAS_UInt32 frameSize;
#endif
} IVC1VDEC_OutArgs;

#endif  /* __IVC1VDEC_H__ */
