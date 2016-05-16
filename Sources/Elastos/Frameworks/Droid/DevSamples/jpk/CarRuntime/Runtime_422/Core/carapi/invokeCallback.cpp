//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <elatypes.h>
#include <elapi.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
__declspec(naked)
int invokeCallback(CallbackEventFlags cFlags, void* pSender, void* pThis, void* pFunc, void* pParam, int nSize)
{
    __asm {
		push    ebp
	    mov     ebp, esp

        mov     ecx, nSize
        mov     eax, pParam
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
        mov     edx, cFlags
        and     edx, CallbackEventFlag_DirectCall
        jne      next
        mov     edx, pSender
        push    edx
next:
        mov     edx, pThis
        push    edx
        mov     ecx, pFunc
        call    ecx

        mov     esp, ebp
        pop     ebp
        ret
    }
}

#else // __GNUC__
int invokeCallback(CallbackEventFlags cFlags, void* pSender, void* pThis, void* pFunc, void* pParam, int nSize)
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
        : "m" (cFlags)
        , "m" (pSender)
        , "m" (pThis)
        , "m" (pFunc)
        , "m" (pParam)
        , "m" (nSize)
        , "n" (CallbackEventFlag_DirectCall)
        : "eax"
        , "ecx"
        , "edx"
    );

    return rval;
}
#endif // _MSC_VER

#ifdef __cplusplus
}
#endif
