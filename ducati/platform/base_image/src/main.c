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
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/ipc/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/ammu/AMMU.h>
#include <ti/sysbios/family/arm/ducati/CTM.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/omap/slpm/idle.h>
#include <ti/omap/mem/shim/MemMgr.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include <ti/sysbios/family/arm/ducati/GateDualCore.h>


#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/sdo/utils/MultiProc.h>
#include <xdc/runtime/Diags.h>
#include <ti/sdo/fc/global/FCSettings.h>
#include <ti/sdo/ce/global/CESettings.h>

#include "dce_priv.h"

#define PM_IVAHD_PWRSTCTRL        (*(volatile unsigned int *)0xAA306F00)
#define RM_IVAHD_RSTCTRL          (*(volatile unsigned int *)0xAA306F10)
#define RM_IVAHD_RSTST            (*(volatile unsigned int *)0xAA306F14)

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

UInt32 HDVICP_Reset(void * handle, void * iresHandle)
{
    int i;
    volatile unsigned int *icont1_itcm_base_addr =
            (unsigned int *)ICONT1_ITCM_BASE;
    volatile unsigned int *icont2_itcm_base_addr =
            (unsigned int *)ICONT2_ITCM_BASE;

    /*
     * Reset IVA HD, SL2 and ICONTs
     */

    DEBUG("HDVICP_Reset");

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

    /* Copy boot code to ICONT1 & ICONT2 memory */
    for (i = 0; i < DIM(icont_boot); i++) {
        *icont1_itcm_base_addr++ = icont_boot[i];
        *icont2_itcm_base_addr++ = icont_boot[i];
    }

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

void platform_idle_processing()
{
    /* wrapper slpm_idle_processing to ensure all necessary wakeup events
     * are enabled
     */
    REG32(WUGEN_MEVT0) |= (WUGEN_IVAHD_MAILBOX_IRQ_2 | WUGEN_IVAHD_IRQ2 | WUGEN_IVAHD_IRQ1);
    slpm_idle_processing();
}


void platform_init()
{
	Int status = 0;
	UInt16 hostId = 0, sysProcId = 0;

    /* clear HSDIVDER_CLKOUT2_DIV */
    set_ivahd_opp(0);

	/* Set up interprocessor notifications */
	DEBUG("Setting up IPC");
	status = Ipc_start();
	if (status < 0) {
	    ERROR("Ipc_start failed: %08x", status);
	    return;
	}

	/* attach to host */
	hostId = MultiProc_getId("MPU");
	DEBUG("APPM3: IPC attaching to MPU, hostId = %d", hostId);
	do {
	    status = Ipc_attach(hostId);
	    DEBUG("APPM3: IPC attaching... %08x", status);
	} while (status < 0);

	/* attach to other M3.. do we need this? */
	sysProcId = MultiProc_getId("SysM3");
	DEBUG("APPM3: IPC attaching to SysM3, sysProcId = %d", sysProcId);

	do {
	    status = Ipc_attach(sysProcId);
	    DEBUG("APPM3: IPC attaching... %08x", status);
	} while (status < 0);

	/* maybe above stuff should move into dce_init().. */
	dce_init();
	DEBUG("APPM3: Completed IPC setup and Server Bringup");

    return;
}


void platform_deinit()
{
    DEBUG("APPM3: Shutdown");
	dce_deinit();
}



unsigned int ducatiSysClock;

extern Void init_IVAHDFrwk();
extern Void exit_IVAHDFrwk();

Void Ducati_AppTask (UArg arg1, UArg arg2)
{
	FCSettings_init();
	Diags_setMask(FCSETTINGS_MODNAME"+12345678LEXAIZFS");
	CESettings_init();
	Diags_setMask(CESETTINGS_MODNAME"+12345678LEXAIZFS");
//	Diags_setMask("ti.sdo.rcm.RcmServer-12");
//	Diags_setMask("ti.sysbios.utils.Load-4");

    init_IVAHDFrwk();
    HDVICP_Reset(NULL, NULL);
    platform_init();

    /* how do we know when to unload.. */
    while (1) {
        Task_sleep(0x7fffffff);
    }

    /*The test cases will run in this task's context in the init call.
    Once init returns, testcases have exited so do deinit*/
    platform_deinit();

    exit_IVAHDFrwk();

    System_exit(0);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Main entry for Ducati subsystem
 * =====================================================================================
 */


int main(void)
{
#if defined(DUCATI_APP_M3)
    /* Hack to enable CMT for APP M3 */
    if ((CTM_ctm.CTGNBL[0] && 0x14) != 0x14) {
        CTM_ctm.CTCNTL |= 1;    /* enable the CTM */

        CTM_ctm.CTCR[2] = 0x4;  /* enable Chain mode, count cycles */
        CTM_ctm.CTCR[3] = 0x4;  /* enable Chain mode, count cycles */
        CTM_ctm.CTCR[4] = 0x4;  /* enable Chain mode, count cycles */
        CTM_ctm.CTCR[5] = 0x4;  /* enable Chain mode, count cycles */

        CTM_ctm.CTGRST[0] |= 0x3c; /* reset counters 2,3,4,5 syncronously */
        CTM_ctm.CTGNBL[0] |= 0x14; /* enable counters 2,4 syncronously */
    }
#endif

    BIOS_start();
}

