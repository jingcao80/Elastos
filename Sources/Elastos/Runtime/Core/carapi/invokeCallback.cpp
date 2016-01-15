//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <elapi.h>

extern "C" {

#ifdef _MSC_VER
__declspec(naked)
int invokeCallback(CallbackEventFlags flags, void* sender, void* thisPtr, void* funcPtr, void* param, int size)
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
        mov     edx, flags
        and     edx, CallbackEventFlag_DirectCall
        jne      next
        mov     edx, sender
        push    edx
next:
        mov     edx, thisPtr
        push    edx
        mov     ecx, funcPtr
        call    ecx

        mov     esp, ebp
        pop     ebp
        ret
    }
}

#else // __GNUC__
int invokeCallback(CallbackEventFlags flags, void* sender, void* thisPtr, void* funcPtr, void* param, int size)
{
    int rval;
    __asm__ (
        "movl   %6, %%ecx\n"
        "movl   %5, %%eax\n"
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
        "movl   %1, %%edx\n"
        "andl   %7, %%edx\n"
        "jne    next\n"
        "movl   %2, %%edx\n"
        "pushl  %%edx\n"
        "next:\n"
        "movl   %3, %%edx\n"
        "pushl  %%edx\n"
        "movl   %4, %%ecx\n"
        "calll  *%%ecx\n"
        "movl   %%ebp, %%esp\n"
        "popl   %%ebp\n"
        "ret\n"
        : "=r" (rval)
        : "m" (flags)
        , "m" (sender)
        , "m" (thisPtr)
        , "m" (funcPtr)
        , "m" (param)
        , "m" (size)
        , "n" (CallbackEventFlag_DirectCall)
        : "eax"
        , "ecx"
        , "edx"
    );

    return rval;
}
#endif // _MSC_VER

}
