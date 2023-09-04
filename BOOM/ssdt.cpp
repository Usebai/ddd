#include "undocumented.h"
#include "ssdt.h"
#include "pe.h"
#include "log.h"
#include "ntdll.h"

//structures
struct SSDTStruct
{
    LONG* pServiceTable;
    PVOID pCounterTable;
#ifdef _WIN64
    ULONGLONG NumberOfServices;
#else
    ULONG NumberOfServices;
#endif
    PCHAR pArgumentTable;
};





//Based on: https://github.com/hfiref0x/WinObjEx64
static SSDTStruct* SSDTfind()
{
    static SSDTStruct* SSDT = 0;
    if(!SSDT)
    {
#ifndef _WIN64
        //x86 code
        UNICODE_STRING routineName;
        RtlInitUnicodeString(&routineName, L"KeServiceDescriptorTable");
        SSDT = (SSDTStruct*)MmGetSystemRoutineAddress(&routineName);
#else
        //x64 code
        ULONG kernelSize;
        ULONG_PTR kernelBase = (ULONG_PTR)Undocumented::GetKernelBase(&kernelSize);
        if(kernelBase == 0 || kernelSize == 0)
            return NULL;

        // Find KiSystemServiceStart
        const unsigned char KiSystemServiceStartPattern[] = { 0x8B, 0xF8, 0xC1, 0xEF, 0x07, 0x83, 0xE7, 0x20, 0x25, 0xFF, 0x0F, 0x00, 0x00 };
        const ULONG signatureSize = sizeof(KiSystemServiceStartPattern);
        bool found = false;
        ULONG KiSSSOffset;
        for(KiSSSOffset = 0; KiSSSOffset < kernelSize - signatureSize; KiSSSOffset++)
        {
            if(RtlCompareMemory(((unsigned char*)kernelBase + KiSSSOffset), KiSystemServiceStartPattern, signatureSize) == signatureSize)
            {
                found = true;
                break;
            }
        }
        if(!found)
            return NULL;

        // lea r10, KeServiceDescriptorTable
        ULONG_PTR address = kernelBase + KiSSSOffset + signatureSize;
        LONG relativeOffset = 0;
        if((*(unsigned char*)address == 0x4c) &&
                (*(unsigned char*)(address + 1) == 0x8d) &&
                (*(unsigned char*)(address + 2) == 0x15))
        {
            relativeOffset = *(LONG*)(address + 3);
        }
        if(relativeOffset == 0)
            return NULL;

        SSDT = (SSDTStruct*)(address + relativeOffset + 7);
#endif
    }
    return SSDT;
}

PVOID SSDT::GetFunctionAddress(const char* apiname)
{
    //read address from SSDT
    SSDTStruct* SSDT = SSDTfind();
    if(!SSDT)
    {
        Log("[BOOM] SSDT not found...\r\n");
        return 0;
    }
    ULONG_PTR SSDTbase = (ULONG_PTR)SSDT->pServiceTable;
    if(!SSDTbase)
    {
        Log("[BOOM] ServiceTable not found...\r\n");
        return 0;
    }
    ULONG readOffset = NTDLL::GetExportSsdtIndex(apiname);
    if(readOffset == -1)
        return 0;
    if(readOffset >= SSDT->NumberOfServices)
    {
        Log("[BOOM] Invalid read offset...\r\n");
        return 0;
    }
#ifdef _WIN64
    return (PVOID)((SSDT->pServiceTable[readOffset] >> 4) + SSDTbase);
#else
    return (PVOID)SSDT->pServiceTable[readOffset];
#endif
}

