var memory = [];


//0.984275MB  memory for sysM3 code in I/D bus cached region
memory[0] = ["MEM_CODE_SYSM3",
{
        name: "MEM_CODE_SYSM3",
        base: 0x4000,
        len:  0x000FC000,
        space: "code"
}];

//3MB memory for appM3 code section in I/D bus cached region
memory[1] = ["MEM_CODE_APPM3",
{
        name: "MEM_CODE_APPM3",
        base: 0x00100000,
        len:  0x00300000,
        space: "code"
}];

//12MB memory section for appM3 heap in I/D bus cached region
memory[2] = ["MEM_HEAP1_APPM3",
{
        name: "MEM_HEAP1_APPM3",
        base: 0x00400000,
        len:  0x00C00000,
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

//1.5MB memory for appM3 const section in S bus cached region
memory[5] = ["MEM_CONST_APPM3",
{
        name: "MEM_CONST_APPM3",
        base: 0x80100000,
        len:  0x00200000,
        space: "data"
}];

//(~29.375MB) memory section for appM3 heap in S bus cached region
memory[6] = ["MEM_HEAP2_APPM3",
{
        name: "MEM_HEAP2_APPM3",
        base: 0x80300000,
        len:  0x01CE0000,
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

Build.platformTable["ti.platforms.generic:DucatiPlatform"] =
{
    clockRate: 200.0,
    catalogName: "ti.catalog.arm.cortexm3",
    deviceName: "OMAP4430",
    mem: memory,
    customMemoryMap: memory
};

