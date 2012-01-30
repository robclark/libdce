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
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/pm/IpcPower.h>
#include <ti/pm/_IpcPower.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/utils/trace/gt.h>
#include <xdc/cfg/global.h>

#include "dce_priv.h"

#include <ti/xdais/ires.h>
#include <ti/sdo/fc/ires/hdvicp/iresman_hdvicp.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <ti/sdo/fc/ires/tiledmemory/iresman_tiledmemory.h>
#include <ti/sdo/fc/rman/rman.h>

#define MEMORYSTATS_DEBUG 1

// XXX different base addr's for OMAP5..
#ifdef OMAP5
#define PM_IVAHD_PWRSTCTRL        (*(volatile unsigned int *)0xAAE06F00)
#define RM_IVAHD_RSTCTRL          (*(volatile unsigned int *)0xAAE06F10)
#define RM_IVAHD_RSTST            (*(volatile unsigned int *)0xAAE06F14)
#else
#define PM_IVAHD_PWRSTCTRL        (*(volatile unsigned int *)0xAA306F00)
#define RM_IVAHD_RSTCTRL          (*(volatile unsigned int *)0xAA306F10)
#define RM_IVAHD_RSTST            (*(volatile unsigned int *)0xAA306F14)
#endif

#define CM_IVAHD_CLKSTCTRL        (*(volatile unsigned int *)0xAA008F00)
#define CM_IVAHD_CLKCTRL          (*(volatile unsigned int *)0xAA008F20)
#define CM_IVAHD_SL2_CLKCTRL      (*(volatile unsigned int *)0xAA008F28)

#define CM_DIV_M5_DPLL_IVA        (*(volatile unsigned int *)0xAA0041BC)

#define IVAHD_CONFIG_REG_BASE     (0xBA000000)
#define ICONT1_ITCM_BASE          (IVAHD_CONFIG_REG_BASE + 0x08000)
#define ICONT2_ITCM_BASE          (IVAHD_CONFIG_REG_BASE + 0x18000)


/*******************************************************************************
 *   Hex code to set for Stack setting, Interrupt vector setting
 *   and instruction to put ICONT in WFI mode.
 *   This shall be placed at TCM_BASE_ADDRESS of given IVAHD, which is
 *   0x0000 locally after reset.
 *******************************************************************************/

const unsigned int icont_boot[] = {
        0xEA000006,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xEAFFFFFE,
        0xE3A00000,
        0xEE070F9A,
        0xEE070F90,
        0xE3A00000,
        0xEAFFFFFE,
        0xEAFFFFF1
};


// XXX hacky stuff...  there should be a better way than sleep!
static inline void sleepms(int ms)
{
//    Task_sleep(((ms * 1000 + (Clock_tickPeriod - 1)) / Clock_tickPeriod));
}

static void ivahd_boot(void)
{
    int i;
    volatile unsigned int *icont1_itcm_base_addr =
            (unsigned int *)ICONT1_ITCM_BASE;
    volatile unsigned int *icont2_itcm_base_addr =
            (unsigned int *)ICONT2_ITCM_BASE;

    /*
     * Reset IVA HD, SL2 and ICONTs
     */

    DEBUG("Booting IVAHD...");

    /* Reset IVAHD sequencers and SL2 */
    RM_IVAHD_RSTCTRL = 0x00000007;
    sleepms(10);

    /* Turn IVA power state to on */
    PM_IVAHD_PWRSTCTRL = 0x00000003;
    sleepms(10);

    /* First put IVA into SW_WKUP mode */
    CM_IVAHD_CLKSTCTRL = 0x00000002;
    sleepms(10);

    /* Set IVA clock to 'auto' */
    CM_IVAHD_CLKCTRL = 0x00000001;
    sleepms(10);

    /* Set SL2 clock to 'auto' */
    CM_IVAHD_SL2_CLKCTRL = 0x00000001;
    sleepms(10);

    /* put ICONT1 & ICONT2 in reset and remove SL2 reset */
    RM_IVAHD_RSTCTRL = 0x00000003;
    sleepms(10);

    /* Copy boot code to ICONT1 & ICONT2 memory */
    for (i = 0; i < DIM(icont_boot); i++) {
        *icont1_itcm_base_addr++ = icont_boot[i];
        *icont2_itcm_base_addr++ = icont_boot[i];
    }
}

int ivahd_reset(void * handle, void * iresHandle)
{
    /*
     * Reset IVA HD, SL2 and ICONTs
     */

    DEBUG("Resetting IVAHD...");

    /* First put IVA into HW Auto mode */
    CM_IVAHD_CLKSTCTRL |= 0x00000003;

    /* Wait for IVA HD to standby */
    while (!((CM_IVAHD_CLKCTRL) & 0x00040000));

    /* Disable IVAHD and SL2 modules */
    CM_IVAHD_CLKCTRL = 0x00000000;
    CM_IVAHD_SL2_CLKCTRL = 0x00000000;

    /* Ensure that IVAHD and SL2 are disabled */
    while (!(CM_IVAHD_CLKCTRL & 0x00030000));
    while (!(CM_IVAHD_SL2_CLKCTRL & 0x00030000));

    /* Reset IVAHD sequencers and SL2 */
    RM_IVAHD_RSTCTRL |= 0x00000007;

    /*
     * Check if modules are reset
     */

    /* First clear the status bits */
    RM_IVAHD_RSTST |= 0x00000007;

    /* Wait for confirmation that the systems have been reset */
    /* THIS CHECK MAY NOT BE NECESSARY, AND MOST OF ALL GIVEN OUR STATE,
     * MAY NOT BE POSSIBLE
     */

    /* Ensure that the wake up mode is set to SW_WAKEUP */
    CM_IVAHD_CLKSTCTRL &= 0x00000002;

    /* Enable IVAHD and SL2 modules */
    CM_IVAHD_CLKCTRL = 0x00000001;
    CM_IVAHD_SL2_CLKCTRL = 0x00000001;

    /* Deassert the SL2 reset */
    RM_IVAHD_RSTCTRL &= 0xFFFFFFFB;

    /* Ensure that IVAHD and SL2 are enabled */
    while (CM_IVAHD_CLKCTRL & 0x00030000);
    while (CM_IVAHD_SL2_CLKCTRL & 0x00030000);

    return TRUE;
}

static int ivahd_use_cnt = 0;

static inline void set_ivahd_opp(int opp)
{
    unsigned int val;

    switch (opp) {
    case 0:    val = 0x010e;  break;
    case 50:   val = 0x000e;  break;
    case 100:  val = 0x0007;  break;
    default: ERROR("invalid opp"); return;
    }

    DEBUG("CM_DIV_M5_DPLL_IVA=%08x", CM_DIV_M5_DPLL_IVA);
    /* set HSDIVDER_CLKOUT2_DIV */
    CM_DIV_M5_DPLL_IVA = (CM_DIV_M5_DPLL_IVA & ~0x0000011f) | val;
    DEBUG("CM_DIV_M5_DPLL_IVA=%08x", CM_DIV_M5_DPLL_IVA);
}

void ivahd_acquire(void)
{
    UInt hwiKey = Hwi_disable();
    if (++ivahd_use_cnt == 1) {
        DEBUG("ivahd acquire");
        set_ivahd_opp(100);
    } else {
        DEBUG("ivahd already acquired");
    }
    Hwi_restore(hwiKey);
}

void ivahd_release(void)
{
    UInt hwiKey = Hwi_disable();
    if (ivahd_use_cnt-- == 1) {
        DEBUG("ivahd release");
        set_ivahd_opp(0);
    } else {
        DEBUG("ivahd still in use");
    }
    Hwi_restore(hwiKey);
}


#define REG32(A)   (*(volatile UInt32 *) (A))
#define WUGEN_IVAHD_MAILBOX_IRQ_2       (1 << 22)
#define WUGEN_IVAHD_IRQ2                (1 << 23)
#define WUGEN_IVAHD_IRQ1                (1 << 24)
void IpcPower_idle(void);

void platform_idle_processing()
{
    /* wrapper slpm_idle_processing to ensure all necessary wakeup events
     * are enabled
     */
    REG32(WUGEN_MEVT0) |= (WUGEN_IVAHD_MAILBOX_IRQ_2 | WUGEN_IVAHD_IRQ2 | WUGEN_IVAHD_IRQ1);
    IpcPower_idle();
}

unsigned int SyslinkMemUtils_VirtToPhys(Ptr vaddr);

void *MEMUTILS_getPhysicalAddr(Ptr vaddr)
{
    unsigned int paddr = SyslinkMemUtils_VirtToPhys(vaddr);
    DEBUG("virtual addr:%x\tphysical addr:%x", vaddr, paddr);
    return (void *)paddr;
}

static Bool allocFxn(IALG_MemRec *memTab, Int numRecs);
static void freeFxn(IALG_MemRec *memTab, Int numRecs);


void ivahd_init(void)
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

    ivahd_boot();

    /* clear HSDIVDER_CLKOUT2_DIV */
    set_ivahd_opp(0);

end:
    return;
}

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
        Memory_getStats(NULL, &stats);
        INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
                stats.totalFreeSize, stats.largestFreeSize);
#endif

        blk = Memory_alloc(NULL, size, memTab[i].alignment, &eb);

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
            Memory_free(NULL, hdr->ptr, hdr->size);
        }
#ifdef MEMORYSTATS_DEBUG
        Memory_getStats(NULL, &stats);
        INFO("Total: %d\tFree: %d\tLargest: %d", stats.totalSize,
                stats.totalFreeSize, stats.largestFreeSize);
#endif
    }
}
