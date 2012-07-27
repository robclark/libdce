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


#ifndef __IH264VDEC_H__
#define __IH264VDEC_H__

#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec3.h>

typedef struct IH264VDEC_Status {
  IVIDDEC3_Status viddec3Status;
  XDAS_Int32 reserved[7];
  XDAS_UInt32 gapInFrameNum;
  XDAS_UInt32 spsMaxRefFrames;
  XDAS_Int32 reservedSomeMore[3];
} IH264VDEC_Status;

typedef struct IH264VDEC_Params {
  IVIDDEC3_Params viddec3Params;
  union {
	  XDAS_Int32 maxNumRefFrames;
	  XDAS_Int32 dpbSizeInFrames;      /* new name, same meaning */
  };
  XDAS_Int32 pConstantMemory;
  XDAS_Int32 bitStreamFormat;
  XDAS_UInt32 errConcealmentMode;
  XDAS_Int32 temporalDirModePred;
  XDAS_Int32 reserved_1[4];
  XDAS_Int32 presetLevelIdc;
  XDAS_Int32 presetProfileIdc;
  XDAS_UInt32 detectCabacAlignErr;
  XDAS_UInt32 detectIPCMAlignErr;
  XDAS_Int32 reserved_2[5];
} IH264VDEC_Params;

typedef struct IH264VDEC_DynamicParams {
  IVIDDEC3_DynamicParams viddec3DynamicParams;
  XDAS_Int32 deblockFilterMode;
  XDAS_Int32 reserved[7];
} IH264VDEC_DynamicParams;

typedef struct IH264VDEC_InArgs {
  IVIDDEC3_InArgs viddec3InArgs;
} IH264VDEC_InArgs;

typedef struct IH264VDEC_OutArgs {
  IVIDDEC3_OutArgs viddec3OutArgs;
} IH264VDEC_OutArgs;

/* old name */
typedef enum {
  IH264VDEC_NUM_REFFRAMES_AUTO = -1,
  IH264VDEC_NUM_REFFRAMES_0 = 0,
  IH264VDEC_NUM_REFFRAMES_1 = 1,
  IH264VDEC_NUM_REFFRAMES_2 = 2,
  IH264VDEC_NUM_REFFRAMES_3 = 3,
  IH264VDEC_NUM_REFFRAMES_4 = 4,
  IH264VDEC_NUM_REFFRAMES_5 = 5,
  IH264VDEC_NUM_REFFRAMES_6 = 6,
  IH264VDEC_NUM_REFFRAMES_7 = 7,
  IH264VDEC_NUM_REFFRAMES_8 = 8,
  IH264VDEC_NUM_REFFRAMES_9 = 9,
  IH264VDEC_NUM_REFFRAMES_10 = 10,
  IH264VDEC_NUM_REFFRAMES_11 = 11,
  IH264VDEC_NUM_REFFRAMES_12 = 12,
  IH264VDEC_NUM_REFFRAMES_13 = 13,
  IH264VDEC_NUM_REFFRAMES_14 = 14,
  IH264VDEC_NUM_REFFRAMES_15 = 15,
  IH264VDEC_NUM_REFFRAMES_16 = 16,
  IH264VDEC_NUM_REFFRAMES_DEFAULT = IH264VDEC_NUM_REFFRAMES_AUTO
} IH264VDEC_numRefFrames;

/* new name */
typedef enum {
  IH264VDEC_DPB_NUMFRAMES_AUTO = -1,
  IH264VDEC_DPB_NUMFRAMES_0 = 0,
  IH264VDEC_DPB_NUMFRAMES_1 = 1,
  IH264VDEC_DPB_NUMFRAMES_2 = 2,
  IH264VDEC_DPB_NUMFRAMES_3 = 3,
  IH264VDEC_DPB_NUMFRAMES_4 = 4,
  IH264VDEC_DPB_NUMFRAMES_5 = 5,
  IH264VDEC_DPB_NUMFRAMES_6 = 6,
  IH264VDEC_DPB_NUMFRAMES_7 = 7,
  IH264VDEC_DPB_NUMFRAMES_8 = 8,
  IH264VDEC_DPB_NUMFRAMES_9 = 9,
  IH264VDEC_DPB_NUMFRAMES_10 = 10,
  IH264VDEC_DPB_NUMFRAMES_11 = 11,
  IH264VDEC_DPB_NUMFRAMES_12 = 12,
  IH264VDEC_DPB_NUMFRAMES_13 = 13,
  IH264VDEC_DPB_NUMFRAMES_14 = 14,
  IH264VDEC_DPB_NUMFRAMES_15 = 15,
  IH264VDEC_DPB_NUMFRAMES_16 = 16,
  IH264VDEC_DPB_NUMFRAMES_DEFAULT = IH264VDEC_DPB_NUMFRAMES_AUTO
} IH264VDEC_dpbNumFrames;

typedef enum {
  IH264VDEC_NO_CONCEALMENT = 0,
  IH264VDEC_APPLY_CONCEALMENT
} IH264VDEC_errConcealmentMode;

typedef enum {
  IH264VDEC_DISABLE_CABACALIGNERR_DETECTION = 0,
  IH264VDEC_ENABLE_CABACALIGNERR_DETECTION
} IH264VDEC_detectCabacAlignErr;

typedef enum {
  IH264VDEC_DISABLE_IPCMALIGNERR_DETECTION = 0,
  IH264VDEC_ENABLE_IPCMALIGNERR_DETECTION
} IH264VDEC_detectIPCMAlignErr;

typedef enum {
  IH264VDEC_LEVEL1 = 0,
  IH264VDEC_LEVEL1B,
  IH264VDEC_LEVEL11,
  IH264VDEC_LEVEL12,
  IH264VDEC_LEVEL13,
  IH264VDEC_LEVEL2,
  IH264VDEC_LEVEL21,
  IH264VDEC_LEVEL22,
  IH264VDEC_LEVEL3,
  IH264VDEC_LEVEL31,
  IH264VDEC_LEVEL32,
  IH264VDEC_LEVEL4,
  IH264VDEC_LEVEL41,
  IH264VDEC_LEVEL42,
  IH264VDEC_LEVEL5,
  IH264VDEC_LEVEL51,
  IH264VDEC_MAXLEVELID = IH264VDEC_LEVEL51
} IH264VDEC_LevelId;


#endif /* __IH264VDEC_H__ */

