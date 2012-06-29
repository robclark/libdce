/*
 * Monitor load and trace any change.
 * Author: Vincent Stehlé <v-stehle@ti.com>, copied from ping_tasks.c
 *
 * Copyright (c) 2011, Texas Instruments Incorporated
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
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/smp/Load.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Time to sleep between load reporting attempts, in ticks.
 * On TI platforms, 1 tick == 1 ms.
 */
#define SLEEP_TICKS 1000

/*
 * Load reporting "threshold". When the new load is within previous reported
 * load +- this value, we do not report it.
 */
#define THRESHOLD 1

/* Monitor load and trace any change. */
static Void loadTaskFxn(UArg arg0, UArg arg1)
{
    UInt32 prev_load = 0;

    /* Suppress warnings. */
    (void)arg0;
    (void)arg1;

    System_printf(
        "loadTask: started\n"
        "  SLEEP_TICKS: %u\n"
        "  Load_hwiEnabled: %d\n"
        "  Load_swiEnabled: %d\n"
        "  Load_taskEnabled: %d\n"
        "  Load_updateInIdle: %d\n"
        "  Load_windowInMs: %u\n"
        ,
        SLEEP_TICKS,
        Load_hwiEnabled,
        Load_swiEnabled,
        Load_taskEnabled,
        Load_updateInIdle,
        Load_windowInMs
    );

    /* Infinite loop to trace load. */
    for (;;) {
        UInt32 load;
        unsigned delta;

        /* Get load. */
        load = Load_getCPULoad();

        /* Trace if changed and delta above threshold. */
        delta = abs((int)load - (int)prev_load);

        if (delta > THRESHOLD) {
            System_printf("loadTask: cpu load = %u%%\n", load);
            prev_load = load;
        }

        /* Delay. */
        Task_sleep(SLEEP_TICKS);
    }
}

void start_load_task(void)
{
    Task_Params params;

    /* Monitor load and trace any change. */
    Task_Params_init(&params);
    params.instance->name = "loadtsk";
    params.priority = 1;

    if(!Task_create(loadTaskFxn, &params, NULL))
        System_printf("Could not create load task!\n");
}
