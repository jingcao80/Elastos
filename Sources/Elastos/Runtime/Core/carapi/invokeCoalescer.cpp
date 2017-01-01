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

#ifdef _MSC_VER
__declspec(naked)
int invokeCoalescer(void* funcPtr, void* oldParam, void* newParam, int size)
{
    __asm {
        push    ebp
        mov     ebp, esp

        mov     ecx, size
        mov     eax, newParam
        add     eax, ecx
        sub     eax, 4
push_NewParam:
        test    ecx, ecx
        jz      next

        mov     edx, eax
        push    edx
        sub     eax, 4
        sub     ecx, 4
        jmp     push_NewParam

next:
        mov     ecx, size
        mov     eax, oldParam
        add     eax, ecx
        sub     eax, 4
push_OldParam:
        test    ecx, ecx
        jz      do_call

        mov     edx, dword ptr [eax]
        push    edx
        sub     eax, 4
        sub     ecx, 4
        jmp     push_OldParam

do_call:
        mov     ecx, funcPtr              // function pointer
        call    ecx

        mov     esp, ebp
        pop     ebp
        ret
    }
}
#else // __GNUC__
int invokeCoalescer(void* funcPtr, void* oldParam, void* newParam, int size)
{
    int rval;
    __asm__ (
        "movl   %4, %%ecx\n"
        "movl   %3, %%eax\n"
        "addl   %%ecx, %%eax\n"
        "subl   $4, %%eax\n"
        "push_NewParam:\n"
        "test   %%ecx, %%ecx\n"
        "jz     next\n"
        "movl   %%eax, %%edx\n"
        "pushl  %%edx\n"
        "subl   $4, %%eax\n"
        "subl   $4, %%ecx\n"
        "jmp    push_NewParam\n"
        "next:\n"
        "movl   %4, %%ecx\n"
        "movl   %2, %%eax\n"
        "addl   %%ecx, %%eax\n"
        "subl   $4, %%eax\n"
        "push_OldParam:\n"
        "test   %%ecx, %%ecx\n"
        "jz     do_call\n"
        "movl   (%%eax), %%edx\n"
        "pushl  %%edx\n"
        "subl   $4, %%eax\n"
        "subl   $4, %%ecx\n"
        "jmp    push_NewParam\n"
        "do_call:\n"
        "movl   %1, %%ecx\n"
        "calll  *%%ecx\n"
        "movl   %%ebp, %%esp\n"
        "movl   %%eax, %0"
        : "=r" (rval)
        : "m" (funcPtr)
        , "m" (oldParam)
        , "m" (newParam)
        , "m" (size)
        : "eax"
        , "ecx"
        , "edx"
    );

    return rval;
}
#endif // _MSC_VER

}
