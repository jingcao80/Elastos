;;=========================================================================
;; Copyright (C) 2012 The Elastos Open Source Project
;;
;; Licensed under the Apache License, Version 2.0 (the "License");
;; you may not use this file except in compliance with the License.
;; You may obtain a copy of the License at
;;
;;      http://www.apache.org/licenses/LICENSE-2.0
;;
;; Unless required by applicable law or agreed to in writing, software
;; distributed under the License is distributed on an "AS IS" BASIS,
;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; See the License for the specific language governing permissions and
;; limitations under the License.
;;=========================================================================

    AREA |.text|,ALIGN=2,CODE, READONLY

    EXPORT  invoke

invoke
    mov     ip, sp
    stmdb   sp!, {r4, r5, fp, ip, lr}
    mov     fp, ip
    mov     r5, r0
    cmp     r2, #0
    beq     call_func
    mov     r4, r1
    sub     r2, r2, #16     ; 0x10
    cmp     r2, #0  ; 0x0
    ble     setreg
    add     r1, r1, #16     ; 0x10
    mov     r0, #0  ; 0x0
    sub     sp, sp, r2

setsp
    ldr     r3, [r1, r0]
    str     r3, [sp, r0]
    add     r0, r0, #4      ; 0x4
    sub     r2, r2, #4      ; 0x4
    cmp     r2, #0  ; 0x0
    bgt     setsp

setreg
    ldmia   r4!, {r0, r1, r2, r3}

call_func
    mov     lr, pc
    mov     pc, r5

    ldmdb   fp, {r4, r5, fp, sp, pc}

    END