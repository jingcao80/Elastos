//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <asm.h>

    .text;
    .align 4;
    .global C_SYMBOL(invokeCoalescer);

C_SYMBOL(invokeCoalescer):
    mov     ip, sp
    stmdb   sp!, {r4, r5, r6, r7, fp, ip, lr} 
    sub     fp, ip, #4           // if defined(_arm && _GNUC__ && __GNUC__ >= 4)
    sub     sp, sp, #20          
    str     r0, [fp, #-28]       //pFunc
    str     r1, [fp, #-32]       //pOldParam
    str     r2, [fp, #-36]       //pNewParam
    str     r3, [fp, #-40]       //nSize
    cmp     r3, #0               
    ble     do_call
    cmp     r3, #4
    ble     one_param
    cmp     r3, #8
    ble     two_params
    cmp     r3, #12
    ble     three_params
    cmp     r3, #16
    ble     four_params
    mov     r6, r3
    sub     r6, r6, #16
    sub     sp, sp, r6
    mov     r7, #16

push_other_old_param:
    ldr     r4, [r1, r7]
    str     r4, [sp, #4]
    add     sp, sp, #4
    add     r7, r7, #4
    sub     r6, r6, #4
    cmp     r6, #0
    bgt     push_other_old_param
    mov     r7, #0

push_other_new_param:
    add     r4, r2, r7
    str     r4, [sp, #4]
    add     sp, sp, #4
    add     r7, r7, #4
    sub     r3, r3, #4 
    cmp     r3, #0
    bgt     push_other_new_param
    
    mov     r4, r1
    ldmia   r4, {r0, r1, r2, r3}
    ble     do_call    
        
one_param:                          
    ldr     r0, [r1]
    mov     r1, r2
    ble     do_call
    
two_params:                          
    mov     r4, r1
    mov     r5, r2
    ldr     r0, [r4]
    ldr     r1, [r4, #4]
    mov     r2, r5
    add     r3, r5, #4
    ble     do_call

three_params:
    mov     r4, r1
    mov     r5, r2
    ldr     r0, [r4]
    ldr     r1, [r4, #4]
    ldr     r2, [r4, #8]
    mov     r3, r5
    add     r4, r5, #8
    str     r4, [sp, #-4]
    add     r4, r5, #4
    str     r4, [sp, #-8]
    ble     do_call

four_params:
    mov     r4, r1
    mov     r5, r2
    ldr     r0, [r4]
    ldr     r1, [r4, #4]
    ldr     r2, [r4, #8]
    ldr     r3, [r4, #12]
    add     r4, r5, #12
    str     r4, [sp, #-16]
    add     r4, r5, #8
    str     r4, [sp, #-12]
    add     r4, r5, #4
    str     r4, [sp, #-8]
    mov     r4, r5
    str     r4, [sp, #-4]

do_call:
    ldr     r5, [fp, #-28]
    mov     lr, pc
    mov     pc, r5
    
    //return
    add     fp, fp, #4
    ldmdb   fp, {r4, r5, r6, r7, fp, sp, pc}
