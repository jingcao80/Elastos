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

#include <asm.h>

    .text;
    .align 4;
    .global C_SYMBOL(invokeCallback);

C_SYMBOL(invokeCallback):
    mov     ip, sp
    stmdb   sp!, {r4, r5, r6, r7, fp, ip, lr} 
    sub     fp, ip, #4         //if defined(_arm && _GNUC__ && __GNUC__ >= 4)
    sub     sp, sp, #20        
    str     r0, [fp, #-28]     //cFlags
    str     r1, [fp, #-32]     //pSender
    str     r2, [fp, #-36]     //pThis
    str     r3, [fp, #-40]     //pFunc
    ldr     r4, [fp, #8]       //nSize
    ldr     r5, [fp, #4]       //pParam
    
    //if (cFlags & CallbackEventFlag_DirectCall) is true then do not need pSender
    and     r0, r0, #0x00000400
    cmp     r0, #0
    bne     is_direct_call
    ldr     r1, [fp, #-32]

    cmp     r4, #0
    bgt     set_param1
    b       do_call
    
set_param1:
    ldr     r2, [r5]
    add     r5, r5, #4
    sub     r4, #4
    cmp     r4, #0
    bgt     set_param2
    b       do_call

set_param2:
    ldr     r3, [r5]
    add     r5, r5, #4
    sub     r4, #4
    cmp     r4, #0
    bgt     set_other_params
    b       do_call

is_direct_call:        
    cmp     r4, #0
    bgt     set_param1_under_direct_call
    b       do_call
    
set_param1_under_direct_call:
    ldr     r1, [r5]
    add     r5, r5, #4
    sub     r4, #4
    cmp     r4, #0
    bgt     set_param2_under_direct_call
    b       do_call

set_param2_under_direct_call:
    ldr     r2, [r5]
    add     r5, r5, #4
    sub     r4, #4
    cmp     r4, #0
    bgt     set_param3_under_direct_call
    b       do_call

set_param3_under_direct_call:
    ldr     r3, [r5]
    add     r5, r5, #4
    sub     r4, #4
    cmp     r4, #0
    bgt     set_other_params
    b       do_call

set_other_params:
    mov     r7, #0
    sub     sp, sp, r4
push_param:
    ldr     r6, [r5, r7]
    str     r6, [sp, r7]
    add     r7, r7, #4
    sub     r4, r4, #4
    cmp     r4, #0
    bgt     push_param

do_call:
    ldr     r0, [fp, #-36]       //pThis
    ldr     r7, [fp, #-40]       //pFunc  
    mov     lr, pc
    mov     pc, r7               //pFunc    
    
    //return
    add     fp, fp, #4
    ldmdb   fp, {r4, r5, r6, r7, fp, sp, pc}
