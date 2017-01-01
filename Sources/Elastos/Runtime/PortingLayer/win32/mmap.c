//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "sys\mman.h"
#include <windows.h>

typedef enum {
    MemoryProtection_Read    = 0x00000001,
    MemoryProtection_Write   = 0x00000002,
    MemoryProtection_Exec    = 0x00000004,
    MemoryProtection_IO      = 0x00000008,
    MemoryProtection_VMem    = 0x00000018,
    MemoryProtection_RW      = 0x00000003,
    MemoryProtection_RX      = 0x00000005,
    MemoryProtection_RWX     = 0x00000007,
    MemoryProtection_RWIO    = 0x0000000b,
    MemoryProtection_RWVM    = 0x0000001b,
} MemoryProtection;

DWORD ConvertToWinAllocateType(int flags)
{
    DWORD allocatetype = MEM_RESERVE;

    if ((flags & MAP_PRIVATE)
        || (flags & MAP_SHARED)) {
        allocatetype  = MEM_COMMIT;
    }

    return allocatetype;
}

DWORD ConvertToWinProectType(int protect)
{
    MemoryProtection mProtect = 0;
    DWORD dwProtect = 0;

    if (protect & PROT_READ)  mProtect |= MemoryProtection_Read;
    if (protect & PROT_WRITE) mProtect |= MemoryProtection_Write;
    if (protect & PROT_EXEC)  mProtect |= MemoryProtection_Exec;

    switch (mProtect) {
        case MemoryProtection_Read:
            dwProtect = PAGE_READONLY;
            break;

        case MemoryProtection_Write:
            dwProtect = PAGE_READWRITE;
            break;

        case MemoryProtection_Exec:
            dwProtect = PAGE_EXECUTE;
            break;

        case MemoryProtection_RW:
        case MemoryProtection_RWIO:
            dwProtect = PAGE_READWRITE;
            break;

        case MemoryProtection_RX:
            dwProtect = PAGE_EXECUTE_READ;
            break;

        case MemoryProtection_RWX:
            dwProtect = PAGE_EXECUTE_READWRITE;
            break;

        default:
            dwProtect = PAGE_READONLY;
    }

    return dwProtect;
}

void *mmap (void *__addr, int __len, int __prot,
		   int __flags, int __fd, int __offset)
{
    LPVOID addrmap = NULL;
    DWORD dwRes = 0;
    int error = 0;
    DWORD dwAllocateType, dwProtect;
    MEMORY_BASIC_INFORMATION memInfo;

    if (NULL != __addr) {
        memset((void *)&memInfo, 0 , sizeof(memInfo));

        dwRes = VirtualQuery(__addr, &memInfo, sizeof(memInfo));
        if (dwRes != sizeof(memInfo)) {
            //error = KD_EAGAIN;
            goto E_FAIL_EXIT;
        }

        if (MEM_FREE != memInfo.State) {
            //error = KD_EALREADY;
            goto E_FAIL_EXIT;
        }
    }

    dwAllocateType = ConvertToWinAllocateType(__flags);
    dwProtect = ConvertToWinProectType(__prot);

    addrmap = VirtualAlloc(__addr, __len, dwAllocateType, dwProtect);
    if (NULL == addrmap) {
        goto E_FAIL_EXIT;
    }

    return addrmap;

E_FAIL_EXIT:
    return NULL;
}

int munmap (void *__addr, int __len)
{
    VirtualFree(__addr, 0, MEM_RELEASE);

    return 0;
}

int getpagesize(void)
{
    // BUGBUG:
    return 4096;
}
