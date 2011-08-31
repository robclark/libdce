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


#ifndef __IVP6VDEC_H__
#define __IVP6VDEC_H__

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/ividdec3.h>

typedef struct Ivp6VDEC_Status {
  IVIDDEC3_Status viddecStatus;
  XDAS_UInt32 extendedErrorCode0;
  XDAS_UInt32 extendedErrorCode1;
  XDAS_UInt32 extendedErrorCode2;
  XDAS_UInt32 extendedErrorCode3;
  XDAS_UInt32 reserved[4];
} Ivp6VDEC_Status;

typedef struct Ivp6VDEC_Params {
  IVIDDEC3_Params viddecParams;
  XDAS_UInt32 ivfFormat;
  XDAS_UInt32 reserved[2];
  XDAS_UInt32 payloadHeaderPresent;
} Ivp6VDEC_Params;

typedef struct Ivp6VDEC_DynamicParams {
  IVIDDEC3_DynamicParams viddecDynamicParams;
} Ivp6VDEC_DynamicParams;

typedef struct Ivp6VDEC_InArgs {
  IVIDDEC3_InArgs viddecInArgs;
} Ivp6VDEC_InArgs;

typedef struct Ivp6VDEC_OutArgs {
  IVIDDEC3_OutArgs viddecOutArgs;
} Ivp6VDEC_OutArgs;

#endif  /* __IVP6VDEC_H__ */
