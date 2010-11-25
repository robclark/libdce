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
 * 
 */
/*
 *  ======== node.h ========
 *  NODE module definitions.
 */
/**
 *  @file       ti/sdo/ce/node/node.h
 *
 *  @brief      The Codec Engine Internal NODE API.
 */
/**
 *  @addtogroup ti_sdo_ce_NODE      CE NODE API
 */

#ifndef ti_sdo_ce_node_NODE_
#define ti_sdo_ce_node_NODE_

#include <ti/sdo/ce/ipc/Comm.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup    ti_sdo_ce_NODE */
/*@{*/

/** @cond INTERNAL */

#define NODE_FOREVER    (UInt)-1     /* infinite timeout (Comm_FOREVER) */
#define NODE_TOGPP 0                /* GPP is the destination for message */

typedef struct NODE_Obj *NODE_EnvPtr;   /* pointer to a node's environment */

typedef struct NODE_Config {
    Int OBJSEG;         /* allocate node instances from this MEM seg id */
    Int MSGQPOOLID;     /* Segment to allocate message frames */
    Int FIXEDMSGSIZE;   /* if >0, size NODE must use with Comm_alloc */
} NODE_Config;

extern NODE_Config *NODE;

/*
 *  ======== NODE_Cmd ========
 */
typedef struct NODE_Cmd {
    Int     id;
    UArg    arg1;
    UArg    arg2;
} NODE_Cmd;

#define NODE_CCALL  0   /* "normal" call command id */
#define NODE_CEXIT  1   /* exit command id */

/*
 *  ======== NODE_MsgHeader ========
 */
typedef struct NODE_MsgHeader {
    Comm_MsgHeader header;
    NODE_Cmd       cmd;
} NODE_MsgHeader;

/*
 *  ======== NODE_Msg ========
 */
typedef struct NODE_MsgHeader *NODE_Msg;

/**
 *  @brief      NODE error code
 */
typedef Int NODE_Status;

#define NODE_EOK            0   /**< Success. */
#define NODE_EOUTOFMEMORY   1   /**< Unable to allocate memory. */
#define NODE_ERESOURCE      2   /**< Unable to obtain a necessary resource. */
#define NODE_ETASK          3   /**< Unable to create a task. */
#define NODE_EFAIL          4   /**< General error. */

/* The Node UUID structure */
typedef struct NODE_Uuid {
    UInt32 data;
} NODE_Uuid;

/*
 *  ======== NODE_allocMsgBuf ========
 *  Allocate a data buffer whose descriptor will be passed to the GPP within
 *  a message.
 *
 *  Parameters:
 *    node: Node's environment.
 *    size:     Size of buffer, in DSP MAUs.
 *    align:    Buffer alignment.
 *
 *  Returns:
 *    On Success: Address of the allocated buffer.
 *    On Failure: NULL.
 *
 */
extern Ptr NODE_allocMsgBuf(NODE_EnvPtr node, UInt size, UInt align);


/*
 *  ======== NODE_freeMsgBuf ========
 *  Free a data buffer previously allocated with NODE_allocMsgBuf.
 *
 *  Parameters:
 *    node: Node's environment.
 *    addr:     Address of the data buffer.
 *    size:     Size of the allocated buffer, in DSP MAUs.
 *
 *  Returns:
 *    On Success: TRUE.
 *    On Failure: FALSE.  A failure occurs when the specified address does
 *                not reside in the memory segment used for allocations by
 *        NODE_allocMsgBuf().
 *
 */
extern Bool NODE_freeMsgBuf(NODE_EnvPtr node, Ptr addr, UInt size);


/*
 *  ======== NODE_getPri ========
 *  Retrieve the task priority of a node.
 *
 *  Parameters:
 *    node:     Node's environment.
 *
 *  Returns:
 *    taksPriority: the task priority set for the node
 *
 *  Constraints:
 *    none
 *
 *  Requires:
 *    node != NULL
 *
 */
extern UInt NODE_getPri(NODE_EnvPtr node);

/*
 *  ======== NODE_init ========
 */
extern Void NODE_init(Void);

/*
 *  ======== NODE_exit ========
 */
extern Void NODE_exit(Void);

/*
 *  ======== NODE_uuidMatch ========
 *
 *  Determine whether a uuid matches another.
 *
 */
static inline Bool NODE_uuidMatch(NODE_Uuid *uuid1, NODE_Uuid *uuid2)
{
    return (uuid1->data == uuid2->data);
}

/** @endcond */

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce.node; 1, 0, 0,395; 7-27-2010 22:15:26; /db/atree/library/trees/ce/ce-q08x/src/
 */

