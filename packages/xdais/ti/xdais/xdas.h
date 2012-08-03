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
 *  @file       ti/xdais/xdas.h
 *
 *  @brief      This header defines all types and constants used in the
 *              XDAS interfaces.
 *
 *  @remarks    The types are mapped to the types defined in std.h.
 */
/**
 *  @addtogroup   ti_xdais_XDAS XDAIS Types and Constants
 */

#ifndef ti_xdais_XDAS_
#define ti_xdais_XDAS_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_xdais_XDAS_ */
/*@{*/


#define XDAS_TRUE       1
#define XDAS_FALSE      0


typedef Void            XDAS_Void;
typedef Uint8           XDAS_Bool;


typedef Int8            XDAS_Int8;      /**< Actual size chip dependent. */
typedef Uint8           XDAS_UInt8;     /**< Actual size chip dependent. */
typedef Int16           XDAS_Int16;     /**< Actual size of type is 16 bits. */
typedef Uint16          XDAS_UInt16;    /**< Actual size of type is 16 bits. */
typedef Int32           XDAS_Int32;     /**< Actual size of type is 32 bits. */
typedef Uint32          XDAS_UInt32;    /**< Actual size of type is 32 bits. */


/*@}*/


#ifdef __cplusplus
}
#endif

#endif  /* ti_xdais_XDAS_ */
/*
 *  @(#) ti.xdais; 1, 2.0, 1,1; 6-19-2012 17:57:46; /db/wtree/library/trees/dais/dais-w06/src/ xlibrary

 */

