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


#ifndef __IMPEG4VDEC_H__
#define __IMPEG4VDEC_H__

#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec3.h>

typedef struct IMPEG4VDEC_Status {
  IVIDDEC3_Status viddec3Status;
  XDAS_Int32 reserved[4];
} IMPEG4VDEC_Status;

typedef struct IMPEG4VDEC_Params {
  IVIDDEC3_Params viddec3Params;
  XDAS_Int32 outloopDeBlocking;
  XDAS_Int32 ErrorConcealmentON;
  XDAS_Int32 sorensonSparkStream;
  XDAS_Int32 reserved[3];
} IMPEG4VDEC_Params;

typedef struct IMPEG4VDEC_DynamicParams {
  IVIDDEC3_DynamicParams viddec3DynamicParams;
} IMPEG4VDEC_DynamicParams;

typedef struct IMPEG4VDEC_InArgs {
  IVIDDEC3_InArgs viddec3InArgs;
} IMPEG4VDEC_InArgs;

typedef struct IMPEG4VDEC_OutArgs {
  IVIDDEC3_OutArgs viddec3OutArgs;
  XDAS_Int32 vopTimeIncrementResolution;
  XDAS_Int32 vopTimeIncrement;
  XDAS_Int32 mp4ClosedGov;
  XDAS_Int32 mp4BrokenLink;
} IMPEG4VDEC_OutArgs;

#endif /*__IMPEG4VDEC_H__ */

