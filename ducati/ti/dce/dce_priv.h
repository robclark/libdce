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

#ifndef __DCE_PRIV_H__
#define __DCE_PRIV_H__

#if defined(CORE0) || defined(CORE1)
#  define SERVER
#else
#  define CLIENT
#endif

Bool dce_init(void);

#ifdef SERVER
/* these acquire/release functions should be implemented by the platform,
 * ie. to use OMX RM if integrated with OMX build, or use directly slpm
 * and/or register programming otherwise.  These are called from dce.c
 * before/after the process() call
 */
void ivahd_acquire(void);
void ivahd_release(void);
void ivahd_init(void);
#endif

#ifndef   DIM
#  define DIM(a) (sizeof((a)) / sizeof((a)[0]))
#endif

/* set desired trace level:
 *   3 - error
 *   2 - error, info
 *   1 - error, info, debug  (very verbose)
 */
#ifndef TRACE_LEVEL
#  define TRACE_LEVEL 1
#endif

#ifndef SERVER
#  define System_printf      printf
#  define System_flush()     do { } while (0)
#endif

#define TRACE(lvl, FMT,...)  do if ((lvl) >= TRACE_LEVEL) { \
        System_printf("%s:%d:\t%s\t" FMT "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        System_flush(); \
    } while (0)

#define ERROR(FMT,...)   TRACE(3, "error: " FMT, ##__VA_ARGS__)
#define INFO(FMT,...)    TRACE(2, "info: " FMT, ##__VA_ARGS__)
#define DEBUG(FMT,...)   TRACE(1, "debug: " FMT, ##__VA_ARGS__)

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif
#ifndef NULL
#  define NULL ((void *)0)
#endif

typedef struct {
    Uint32  size;
#ifdef SERVER
    void *ptr;		/* when used for BIOS heap blocks, just a raw ptr */
#else
    Uint32 bo;		/* GEM buffer object handle */
#endif
} MemHeader;

#define P2H(p) (&(((MemHeader *)(p))[-1]))
#define H2P(h) ((void *)&(h)[1])

#ifndef __packed
#  define __packed __attribute__((packed))
#endif

#endif /* __DCE_PRIV_H__ */
