var memory = [];


//0.984275MB  memory for sysM3 code in I/D bus cached region
memory[0] = ["MEM_CODE_SYSM3",
{
        name: "MEM_CODE_SYSM3",
        base: 0x4000,
        len:  0x000FC000,
        space: "code"
}];

//4MB memory for appM3 code section in I/D bus cached region
memory[1] = ["MEM_CODE_APPM3",
{
        name: "MEM_CODE_APPM3",
        base: 0x00100000,
        len:  0x00400000,
        space: "code"
}];

//11MB memory section for appM3 heap in I/D bus cached region
memory[2] = ["MEM_HEAP1_APPM3",
{
        name: "MEM_HEAP1_APPM3",
        base: 0x00500000,
        len:  0x00B00000,
        space: "data"
}];


//0.25MB memory for sysM3 const section in S bus cached region
memory[3] = ["MEM_CONST_SYSM3",
{
        name: "MEM_CONST_SYSM3",
        base: 0x80000000,
        len:  0x00040000,
        space: "data"
}];

//0.75MB memory section for sysM3 heap in S bus cached region
memory[4] = ["MEM_HEAP_SYSM3",
{
        name: "MEM_HEAP_SYSM3",
        base: 0x80040000,
        len:  0x000C0000,
        space: "data"
}];

//3.5MB memory for appM3 const section in S bus cached region
//out of 3.5 MB, now 1KB is taken out hence total = 3.44MB.
//1KB is used for runtime trace control+MMS trace dump+codec Trace
memory[5] = ["MEM_CONST_APPM3",
{
        name: "MEM_CONST_APPM3",
        base: 0x80100000,
        len:  0x0037FC00,
        space: "data"
}];

//(~27.375MB) memory section for appM3 heap in S bus cached region
memory[6] = ["MEM_HEAP2_APPM3",
{
        name: "MEM_HEAP2_APPM3",
        base: 0x80480000,
        len:  0x01AE0000,
        space: "data"
}];

//128KB For Trace Buffer
memory[7] = ["DUCATI_TRACE_BUFFER",
{
        name: "DUCATI_TRACE_BUFFER",
        base: 0x81FE0000,
        len:  0x20000,
        space: "data"
}];

//336KB memory for IPC memory in S bus non cached region
memory[8] = ["MEM_IPC_HEAP0",
{
        name: "MEM_IPC_HEAP0",
        base: 0xA0000000,
        len:  0x54000,
        space: "data"
}];

//688KB memory for IPC memory in S bus non cached region
memory[9] = ["MEM_IPC_HEAP1",
{
        name: "MEM_IPC_HEAP1",
        base: 0xA0054000,
        len:  0xAC000,
        space: "data"
}];

memory[10] = ["L2_ROM",
{
        name: "L2_ROM",
        base: 0x0,
        len:  0x4000,
        space: "code/data"
}];

//1KB reserved for ducati trace control from A9+MMS trace dump+codec Trace dump
memory[11] = ["MEM_TRACE_CTRL_APPM3",
{
        name: "MEM_TRACE_CTRL_APPM3",
        base: 0x8047FC00,
        len:  0x00000400,
        space: "data"
}];

//500KB memory for new appM3 const section in S bus cached region
memory[12] = ["MEM_CONST_APPM3_1",
{
        name: "MEM_CONST_APPM3_1",
        base: 0x81F60000,
        len:  0x00080000,
        space: "data"
}];

Build.platformTable["ti.platforms.generic:DucatiPlatform"] =
{
    clockRate: 200.0,
    catalogName: "ti.catalog.arm.cortexm3",
    deviceName: "OMAP4430",
    mem: memory,
    customMemoryMap: memory
};

