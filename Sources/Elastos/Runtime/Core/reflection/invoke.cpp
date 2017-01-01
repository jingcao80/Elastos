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

#include <elatypes.h>

extern "C" {

#ifdef _mips
__declspec(naked)
int invoke(void* func, int* param, int size)
{
    ASM("break 0;");
}
#elif _MSC_VER
__declspec(naked)
int invoke(void* func, int* param, int size)
{
    __asm {
        push    ebp
        mov     ebp, esp

        mov     ecx, size
        mov     eax, param
        add     eax, ecx
        sub     eax, 4
push_param:
        test    ecx, ecx
        jz      do_call

        mov     edx, dword ptr [eax]
        push    edx
        sub     eax, 4
        sub     ecx, 4
        jmp     push_param

do_call:
        mov     ecx, func              // function pointer
        call    ecx

        mov     esp, ebp
        pop     ebp
        ret
    }
}
#else // __GNUC__
int invoke(void* func, int* param, int size)
{
    int rval;
    __asm__ (
        "movl   %2, %%eax\n"
        "movl   %3, %%ecx\n"
        "addl   %%ecx, %%eax\n"
        "subl   $4, %%eax\n"
        "push_param:\n"
        "test   %%ecx, %%ecx\n"
        "jz     do_call\n"
        "movl   (%%eax), %%edx\n"
        "pushl  %%edx\n"
        "subl   $4, %%eax\n"
        "subl   $4, %%ecx\n"
        "jmp    push_param\n"
        "do_call:\n"
        "movl   %1, %%ecx\n"
        "calll  *%%ecx\n"
        "movl   %%ebp, %%esp\n"
        "movl   %%eax, %0"
        : "=r" (rval)
        : "m" (func)
        , "m" (param)
        , "d" (size)
        : "eax"
        , "ecx"
    );

    return rval;
}

#endif // _MSC_VER

}
