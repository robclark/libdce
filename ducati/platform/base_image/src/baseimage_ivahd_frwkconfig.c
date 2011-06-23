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


#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/utils/trace/gt.h>
#include <xdc/cfg/global.h>

#include "dce_priv.h"

//#include <ti/omap/mem/shim/MemMgr.h>
#include <ti/omap/mem/SyslinkMemUtils.h>

#include <ti/xdais/ires.h>
#include <ti/sdo/fc/ires/hdvicp/iresman_hdvicp.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <ti/sdo/fc/ires/tiledmemory/iresman_tiledmemory.h>


#define MEMORYSTATS_DEBUG 1

void *MEMUTILS_getPhysicalAddr(Ptr vaddr)
{
	unsigned int paddr = SyslinkMemUtils_VirtToPhys(vaddr);
	DEBUG("virtual addr:%x\tphysical addr:%x", vaddr, paddr);
	return (void *)paddr;
}

static Bool allocFxn(IALG_MemRec *memTab, Int numRecs);
static void freeFxn(IALG_MemRec *memTab, Int numRecs);


Void init_IVAHDFrwk(void)
{
	IRES_Status ret;
	IRESMAN_Params rman_params = {
			.size = sizeof(IRESMAN_Params),
			.allocFxn = allocFxn,
			.freeFxn = freeFxn,
	};

	CERuntime_init();

	ret = RMAN_init();
	if (ret != IRES_OK) {
		goto end;
	}

	/* Register HDVICP with RMAN if not already registered */
	ret = RMAN_register(&IRESMAN_HDVICP, &rman_params);
	if ((ret != IRES_OK) && (ret != IRES_EEXISTS)) {
		DEBUG("could not register IRESMAN_HDVICP: %d", ret);
		goto end;
	}

	/* NOTE: this might try MemMgr allocations over RCM remote
	 * call back to host side.  Which will fail if we don't
	 * have memsrv.  But will eventually fall back to allocFxn
	 * which will allocate from the local heap.
	 */
	ret = RMAN_register(&IRESMAN_TILEDMEMORY, &rman_params);
	if ((ret != IRES_OK) && (ret != IRES_EEXISTS)) {
		DEBUG("could not register IRESMAN_TILEDMEMORY: %d", ret);
		goto end;
	}

	DEBUG("RMAN_register() for HDVICP is successful");

end:
	return;
}

Void exit_IVAHDFrwk()
{
	/* CERuntime_exit() will call global exit that deinitialize all
	 * (RMAN,Algorithm...etc)
	 */
	DEBUG("call to CERuntime_exit()");
	CERuntime_exit();
}

/* we need to track the block size ourselves, to ensure that it is
 * properly deallocated
 */
typedef struct {
	Uns size;
	void *ptr;
} MemHeader;

#define P2H(p) (&(((MemHeader *)(p))[-1]))

static Bool allocFxn(IALG_MemRec memTab[], Int n)
{
	Int i;
#ifdef MEMORYSTATS_DEBUG
	Memory_Stats stats;
#endif

	for (i = 0; i < n; i++) {
		Error_Block eb;
		Uns pad, size;
		void *blk;
		MemHeader *hdr;

		if (memTab[i].alignment > sizeof(MemHeader)) {
			pad = memTab[i].alignment;
		} else {
			pad = sizeof(MemHeader);
		}

		size = memTab[i].size + pad;

#ifdef MEMORYSTATS_DEBUG
		Memory_getStats(heap1, &stats);
		INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
				stats.totalFreeSize, stats.largestFreeSize);
#endif

		blk = Memory_alloc(heap1, size, memTab[i].alignment, &eb);

		if (!blk) {
			ERROR("MemTab Allocation failed at %d", i);
			freeFxn(memTab, i);
			return FALSE;
		} else {
			memTab[i].base = (void *)((char *)blk + pad);
			hdr = P2H(memTab[i].base);
			hdr->size = size;
			hdr->ptr  = blk;
			DEBUG("%d: alloc: %p/%p (%d)", i, hdr->ptr,
					memTab[i].base, hdr->size);
		}
	}
	DEBUG("MemTab Allocation is Successful");
	return TRUE;
}

static void freeFxn(IALG_MemRec memTab[], Int n)
{
	Int i;
#ifdef MEMORYSTATS_DEBUG
	Memory_Stats stats;
#endif

	for (i = 0; i < n; i++) {
		if (memTab[i].base != NULL) {
			MemHeader *hdr = P2H(memTab[i].base);

#ifdef MEMORYSTATS_DEBUG
			DEBUG("%d: free: %p/%p (%d)", n, hdr->ptr,
					memTab[i].base, hdr->size);
#endif
			Memory_free(heap1, hdr->ptr, hdr->size);
		}
#ifdef MEMORYSTATS_DEBUG
		Memory_getStats(heap1, &stats);
		INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
				stats.totalFreeSize, stats.largestFreeSize);
#endif
	}
}
