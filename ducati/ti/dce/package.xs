/*
 * Copyright (c) 2011-2012, Texas Instruments Incorporated
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

/*
 *  ======== package.xs ========
 *
 */


/*
 *  ======== init ========
 */
function init()
{
    /*
     * install a SYS/BIOS startup function
     * it will be called during BIOS_start()
     */
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    BIOS.addUserStartupFunction('&dce_init');
}

/*
 *  ======== getLibs ========
 */
function getLibs(prog)
{
    var name = this.$name + ".a" + prog.build.target.suffix;
    var lib = "";

    lib= "lib/" + this.profile + "/" + name;

    if (java.io.File(this.packageBase + lib).exists()) {
        return lib;
    }

    /* all ti.targets return whole_program_debug library by default */
    if (prog.build.target.$name.match(/^ti\.targets\./)) {
        lib = "lib/" + "whole_program_debug/" + name;
        if (java.io.File(this.packageBase + lib).exists()) {
            return lib;
        }
    }

    /* all other targets, return release library by default */
    else {
        lib = "lib/" + "release/" + name;
        if (java.io.File(this.packageBase + lib).exists()) {
            return lib;
        }
    }

    /* could not find any library, throw exception */
    throw Error("Library not found: " + name);
}

