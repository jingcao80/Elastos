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

# EXTERN_C int invoke2(uintptr_t func, uintptr_t params, int paramCount, struct ParmElement* paramsSize, int totalSize);

#include <asm.h>

    .text
    .align 8;
    .global C_SYMBOL(invoke2);

C_SYMBOL(invoke2):

# x9: subroutine return address
# x10: hold "func" value
# x11: hold "params" value
# w12: hold "paramCount" value
# x13: hold "paramsSize" value
# x14: hold "totalSize" value

    sub     sp, sp, #16
    stp     fp, lr, [sp]
    mov     fp, sp
    b       main

get_next_param:
    add     x13, x13, #20            ; /* jump to next parameter size, sizeof(struct ParmElement) is #20. */
    ldr     w15, [x13, #4]
    cmp     w15, #8
    beq     need_eight_bytes_alignment
    br      x9
need_eight_bytes_alignment:
    and     x15, x11, #0x7
    cmp     x15, #0
    beq     do_not_adjust
    add     x11, x11, #4
do_not_adjust:
    br     x9

main:
    mov     x10, x0                  ; /* x0 holds parameter "func" */
    mov     x11, x1                  ; /* x1 holds parameter "params" */
    mov     x12, #0
    mov     w12, w2                  ; /* w2 holds parameter "paramCount" */
    mov     x13, x3                  ; /* x3 holds parameter "paramsSize" */
set_this:
    ldr     x0, [x11]
    add     x11, x11, #8
    sub     w12, w12, #1
    cmp     w12, #0
    ble     call_func

set_reg_x1:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w1
    ldr     x1, [x11]
    add     x11, x11, #8
    b       set_ret_x1_end
set_reg_w1:
    ldr     w1, [x11]
    add     x11, x11, #4
set_ret_x1_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x2
    b       get_next_param

set_reg_x2:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w2
    ldr     x2, [x11]
    add     x11, x11, #8
    b       set_ret_x2_end
set_reg_w2:
    ldr     w2, [x11]
    add     x11, x11, #4
set_ret_x2_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x3
    b       get_next_param

set_reg_x3:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w3
    ldr     x3, [x11]
    add     x11, x11, #8
    b       set_ret_x3_end
set_reg_w3:
    ldr     w3, [x11]
    add     x11, x11, #4
set_ret_x3_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x4
    b       get_next_param

set_reg_x4:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w4
    ldr     x4, [x11]
    add     x11, x11, #8
    b       set_ret_x4_end
set_reg_w4:
    ldr     w4, [x11]
    add     x11, x11, #4
set_ret_x4_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x5
    b       get_next_param

set_reg_x5:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w5
    ldr     x5, [x11]
    add     x11, x11, #8
    b       set_ret_x5_end
set_reg_w5:
    ldr     w5, [x11]
    add     x11, x11, #4
set_ret_x5_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x6
    b       get_next_param

set_reg_x6:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w6
    ldr     x6, [x11]
    add     x11, x11, #8
    b       set_ret_x6_end
set_reg_w6:
    ldr     w6, [x11]
    add     x11, x11, #4
set_ret_x6_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, set_reg_x7
    b       get_next_param

set_reg_x7:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     set_reg_w7
    ldr     x7, [x11]
    add     x11, x11, #8
    b       set_ret_x7_end
set_reg_w7:
    ldr     w7, [x11]
    add     x11, x11, #4
set_ret_x7_end:
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, save_param_to_sp
    b       get_next_param

save_param_to_sp:
    mov     x14, #16                  ; /* sp should be 16-byte aligned */
    mul     x14, x12, x14
    sub     sp, sp, x14
    mov     x8, sp
save_next_param:
    ldr     w9, [x13, #4]
    cmp     w9, #4
    ble     size_four
x8_need_eight_bytes_alignment:
    and     x15, x8, #0x7
    cmp     x15, #0
    beq     do_not_adjust_x8
    add     x8, x8, #4
do_not_adjust_x8:
    ldr     x14, [x11]
    str     x14, [x8]
    add     x11, x11, #8
    add     x8, x8, #8
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, save_next_param
    b       get_next_param
size_four:
    ldr     w14, [x11]
    str     w14, [x8]
    add     x11, x11, #4
    add     x8, x8, #4
    sub     w12, w12, #1
    cmp     w12, #0
    beq     call_func
    adr     x9, save_next_param
    b       get_next_param

call_func:
    adr     lr, return_from_func
    br      x10

return_from_func:
    mov     sp, fp
    ldp     fp, lr, [sp]
    add     sp, sp, #16
    ret
