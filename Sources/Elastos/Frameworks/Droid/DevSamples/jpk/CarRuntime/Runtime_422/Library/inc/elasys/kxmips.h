//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/*++ BUILD Version: 0003    // Increment this if a change has global effects
*/
//*++
//
//
// Module Name:
//
//  kxmips.h
//
// Abstract:
//
//  This module contains the nongenerated part of the MIPS assembler
//  header file. In general, it contains processor architecture constant
//  information, however some assembler macros are also included.
//
//
// Revision History:
//
//--*/

//
// Define load linked and store conditional opcodes if system is not an r4000
//
#define ASM_ONLY    // allow assembler only conditionals

#ifdef  _MIPS64
#define REG_SIZE    8
#define L_REG       ld
#define L_FREG      ldc1
#define MFC_REG     dmfc0
#define MTC_REG     dmtc0
#define S_REG       sd
#define S_FREG      sdc1
#else   //  _MIPS64
#define REG_SIZE    4
#define L_REG       lw
#define L_FREG      lwc1
#define MFC_REG     mfc0
#define MTC_REG     mtc0
#define S_REG       sw
#define S_FREG      swc1
#endif  //  _MIPS64

//
// Define soft reset vector address for nonhandled cache parity errors.
//

#if defined(R4000)

#define SOFT_RESET_VECTOR 0xbfc00300    // default parity error routine address

#endif

//
// Define low memory transfer vector address and TB index address (temporary).
//

#define TRANSFER_VECTOR (KSEG1_BASE + 0x400) // exception handler address

//
// Define standard integer registers.
//

#define zero $0                         // read zero, writes ignored
#define AT $1                           // assembler temporary
#define v0 $2                           // return value
#define v1 $3                           //
#define a0 $4                           // argument registers
#define a1 $5                           //
#define a2 $6                           //
#define a3 $7                           //
#define t0 $8                           // caller saved registers
#define t1 $9                           //
#define t2 $10                          //
#define t3 $11                          //
#define t4 $12                          //
#define t5 $13                          //
#define t6 $14                          //
#define t7 $15                          //
#define s0 $16                          // callee saved registers
#define s1 $17                          //
#define s2 $18                          //
#define s3 $19                          //
#define s4 $20                          //
#define s5 $21                          //
#define s6 $22                          //
#define s7 $23                          //
#define t8 $24                          // caller saved registers
#define t9 $25                          //
#define k0 $26                          // kernel reserved registers
#define k1 $27                          //
#define gp $28                          // global pointer
#define sp $29                          // stack pointer
#define s8 $30                          // callee saved register
#define ra $31                          // return address register

//
// Define standard floating point registers.
//

#define f0 $f0                          // caller saved registers
#define f1 $f1                          //
#define f2 $f2                          //
#define f3 $f3                          //
#define f4 $f4                          //
#define f5 $f5                          //
#define f6 $f6                          //
#define f7 $f7                          //
#define f8 $f8                          //
#define f9 $f9                          //
#define f10 $f10                        //
#define f11 $f11                        //
#define f12 $f12                        //
#define f13 $f13                        //
#define f14 $f14                        //
#define f15 $f15                        //
#define f16 $f16                        //
#define f17 $f17                        //
#define f18 $f18                        //
#define f19 $f19                        //
#define f20 $f20                        // callee saved registers
#define f21 $f21                        //
#define f22 $f22                        //
#define f23 $f23                        //
#define f24 $f24                        //
#define f25 $f25                        //
#define f26 $f26                        //
#define f27 $f27                        //
#define f28 $f28                        //
#define f29 $f29                        //
#define f30 $f30                        //
#define f31 $f31                        //

//
// Define R4000 system coprocessor registers.
//

#if defined(R4000)

#define CP0_INDEX $0
#define CP0_RANDOM $1
#define CP0_ENTRYLO0 $2
#define CP0_ENTRYLO1 $3
#define CP0_CONF $3
#define CP0_CONTEXT $4
#define CP0_PAGEMASK $5
#define CP0_WIRED $6
#define CP0_INFO $7
#define CP0_BADVADDR $8
#define CP0_COUNT $9
#define CP0_ENTRYHI $10
#define CP0_COMPARE $11
#define CP0_STATUS $12
#define CP0_CAUSE $13
#define CP0_EPC $14
#define CP0_PRID $15
#define CP0_CONFIG $16
#define CP0_LLADDR $17
#define CP0_WATCHLO $18
#define CP0_WATCHHI $19
#define CP0_XCONTEXT $20
#define CP0_FRAMEMASK $21
#define CP0_DIAGNOSTIC $22
#define CP0_PERFORMANCE $25
#define CP0_ECC $26
#define CP0_CACHEERR $27
#define CP0_TAGLO $28
#define CP0_TAGHI $29
#define CP0_ERROREPC $30

//
// Define R4000 system coprocessor register bit field offsets.
//
#define KUBASE          0x00000000
#define K0BASE          0x80000000
#define K0SIZE          0x20000000
#define K1BASE          0xa0000000
#define K1SIZE          0x20000000
#define K2BASE          0xc0000000

#define K3BASE          0xe0000000

#define INDEX_INDEX     0x0             // TLB specified index <5:0>
#define INDEX_PROBE     0x1f            // TLB probe failure <31>

#define RANDOM_INDEX    0x0             // TLB random index <5:0>

#define ENTRYLO_G       0x0             // Global <0>
#define ENTRYLO_V       0x1             // Valid <1>
#define ENTRYLO_D       0x2             // Dirty <2>
#define ENTRYLO_C       0x3             // Cache control <5:3>
#define ENTRYLO_PFN     0x6             // Page Frame <29:6>

#define WIRED_NUMBER    0x0             // Wired entries <5:0>

#define ENTRYHI_PID     0x0             // Process id <7:0>
#define ENTRYHI_VPN2    0xd             // Virtual page <31:13>

#define PID_MASK        0xfff           // allow for ample expansion

#define SR_IE           0x00000001      // interrupt enable
#define SR_EXL          0x00000002      // exception level
#define SR_ERL          0x00000004      // error level
#define SR_KSU          0x00000010      // Kernel/supervisor/user <4:3>
#define SR_PMODE        (PSR_KSU + 1)   // Previous mode bit <4>
#define SR_UX           0x5             // User extended <5>
#define SR_SX           0x6             // Supervior extended <6>
#define SR_KX           0x7             // Kernel extended <7>
#define SR_INTMASK      0x8             // Interrupt mask <15:8>
#define SR_DE           0x00010000      // Disable cache errors
#define SR_CE           0x00020000      // Cache ECC register modifies check bits
#define SR_CH           0x00040000      // Cache Hit
#define SR_SR           0x00100000      // Soft Reset
#define SR_TS           0x00200000      // TLB shutdown
#define SR_BEV          0x00400000      // Bootstrap Exception Vector
#define SR_CU0          0x10000000      // Coprocessor 0 usable
#define SR_CU3          0x80000000      // Coprocessor 3 usable
#define SR_CU2          0x40000000      // coprocessor 2 usable
#define SR_CU1          0x20000000      // Coprocessor 1 usable
#define SR_CUMASK       0xf0000000      // Coprocessor usable bits
#define SR_IM           0xff00          //

#define KU_MASK         0x18
#define KU_USER         0x10
#define KU_KERN         0x00

//  Define the additional PSL bits.
#if defined(_MIPS64)
#define PSR_XX_C        (1<<31)         //  Enable MIPS IV instructions in user mode.
#ifdef  MIPS_HAS_FPU
#define PSR_FR_C        (1<<26)         //  Enable 32 64-bit floating point registers.
#else   //  MIPS_HAS_FPU
#define PSR_FR_C        0               //  Default to 32 32-bit floating point registers.
#endif  //  MIPS_HAS_FPU
#define PSR_UX_C        (1<<5)          //  Enable 64-bit addressing in user mode.
#else   //  _MIPS64
#define PSR_XX_C        0               //  Disable MIPS IV instructures in user mode.
#define PSR_FR_C        0               //  Default to 32 32-bit floating point registers.
#define PSR_UX_C        0               //  Disable 64-bit addressing in user mode.
#endif  //  _MIPS64

#define CONFIG_K0       0x0             // Kseg0 cache algorithm <2:0>
#define CONFIG_CU       0x3             // Update on store conditional <3>
#define CONFIG_DB       0x4             // Data cache block size <4>
#define CONFIG_IB       0x5             // Instruction cache block size <5>
#define CONFIG_DC       0x6             // Data cache size <8:6>
#define CONFIG_IC       0x9             // Instruction cache size <11:9>
#define CONFIG_EB       0xd             // Block ordering <13>
#define CONFIG_EM       0xe             // ECC mode enable <14>
#define CONFIG_BE       0xf             // Big endian memory <15>
#define CONFIG_SM       0x10            // Use dirty shared state <16>
#define CONFIG_SC       0x11            // Secondary cache present <17>
#define CONFIG_EW       0x12            // System address port width <19:18>
#define CONFIG_SW       0x14            // Secondary cache port width <20>
#define CONFIG_SS       0x15            // Split secondary cache mode <21>
#define CONFIG_SB       0x16            // Secondary cache block size <23:22>
#define CONFIG_EP       0x18            // Pattern for write back data <27:24>
#define CONFIG_EC       0x1c            // System address port clock ratio <30:28>
#define CONFIG_CM       0x1f            // Master checker mode <31>


#ifndef CFG_ECMASK
#define CFG_CM          0x80000000      // Master-Checker mode
#define CFG_ECMASK      0x70000000      // System Clock Ratio
#define CFG_ECSHIFT     28
#define CFG_ECBY2       0x00000000      // divide by 2
#define CFG_ECBY3       0x00000000      // divide by 3
#define CFG_ECBY4       0x00000000      // divide by 4
#define CFG_EPMASK      0x0f000000      // Transmit data pattern
#define CFG_EPD         0x00000000      // D
#define CFG_EPDDX       0x01000000      // DDX
#define CFG_EPDDXX      0x02000000      // DDXX
#define CFG_EPDXDX      0x03000000      // DXDX
#define CFG_EPDDXXX     0x04000000      // DDXXX
#define CFG_EPDDXXXX    0x05000000      // DDXXXX
#define CFG_EPDXXDXX    0x06000000      // DXXDXX
#define CFG_EPDDXXXXX   0x07000000      // DDXXXXX
#define CFG_EPDXXXDXXX  0x08000000      // DXXXDXXX
#define CFG_SBMASK      0x00c00000      // Secondary cache block size
#define CFG_SBSHIFT     22
#define CFG_SB4         0x00000000      // 4 words
#define CFG_SB8         0x00400000      // 8 words
#define CFG_SB16        0x00800000      // 16 words
#define CFG_SB32        0x00c00000      // 32 words
#define CFG_EMMASK      0x00c00000      // Vr54xx: SysAD mode
#define CFG_EMSHIFT     22
#define CFG_EM_R4K      0x00000000      // Vr54xx: R4x000 compatible
#define CFG_EM_SPLITRD  0x00400000      // Vr54xx: Multiple split reads
#define CFG_EM_PIPEWR   0x00800000      // Vr54xx: Pipeline writes
#define CFG_EM_WRREISSU 0x00c00000      // Vr54xx: Write-reissue
#define CFG_AD          0x00800000      // Accelerated data (R4100)
#define CFG_SS          0x00200000      // Split secondary cache
#define CFG_SW          0x00100000      // Secondary cache port width
#define CFG_EWMASK      0x000c0000      // System port width
#define CFG_EWSHIFT     18
#define CFG_EW64        0x00000000      // 64 bit
#define CFG_EW32        0x00040000      // 32 bit
#define CFG_SC          0x00020000      // Secondary cache absent
#define CFG_SM          0x00010000      // Dirty Shared mode disabled
#define CFG_BE          0x00008000      // Big Endian
#define CFG_EM          0x00004000      // ECC mode enable
#define CFG_EB          0x00002000      // Block ordering
#define CFG_ICMASK      0x00000e00      // Instruction cache size
#define CFG_ICSHIFT     9
#define CFG_DCMASK      0x000001c0      // Data cache size
#define CFG_DCSHIFT     6
#define CFG_IB          0x00000020      // Instruction cache block size
#define CFG_DB          0x00000010      // Data cache block size
#define CFG_CU          0x00000008      // Update on Store Conditional
#define CFG_K0MASK      0x00000007      // KSEG0 coherency algorithm
#endif //CFG_ECMASK


//
// R4000 Cache operations
//
#define Index_Invalidate_I              0x0         /* 0       0 */
#define Index_Writeback_Inv_D           0x1         /* 0       1 */
#define Index_Invalidate_SI             0x2         /* 0       2 */
#define Index_Writeback_Inv_SD          0x3         /* 0       3 */
#define Index_Load_Tag_I                0x4         /* 1       0 */
#define Index_Load_Tag_D                0x5         /* 1       1 */
#define Index_Load_Tag_SI               0x6         /* 1       2 */
#define Index_Load_Tag_SD               0x7         /* 1       3 */
#define Index_Store_Tag_I               0x8         /* 2       0 */
#define Index_Store_Tag_D               0x9         /* 2       1 */
#define Index_Store_Tag_SI              0xA         /* 2       2 */
#define Index_Store_Tag_SD              0xB         /* 2       3 */
#define Create_Dirty_Exc_D              0xD         /* 3       1 */
#define Create_Dirty_Exc_SD             0xF         /* 3       3 */
#define Hit_Invalidate_I                0x10        /* 4       0 */
#define Hit_Invalidate_D                0x11        /* 4       1 */
#define Hit_Invalidate_SI               0x12        /* 4       2 */
#define Hit_Invalidate_SD               0x13        /* 4       3 */
#define Fill_I                          0x14        /* 5       0 */
#define Hit_Writeback_Inv_D             0x15        /* 5       1 */
#define Hit_Writeback_Inv_SD            0x17        /* 5       3 */
#define Hit_Writeback_I                 0x18        /* 6       0 */
#define Hit_Writeback_D                 0x19        /* 6       1 */
#define Hit_Writeback_SD                0x1B        /* 6       3 */
#define Hit_Set_Virtual_SI              0x1E        /* 7       2 */
#define Hit_Set_Virtual_SD              0x1F        /* 7       3 */
//#endif

#define CFG_C_WTHRU_NOALLOC             0   /* r4600 only */
#define CFG_C_WTHRU_ALLOC               1   /* r4600 only */
#define CFG_C_UNCACHED                  2
#define CFG_C_NONCOHERENT               3
#define CFG_C_WBACK                     3
#define CFG_C_COHERENTXCL               4
#define CFG_C_COHERENTXCLW              5
#define CFG_C_COHERENTUPD               6   /* r4000/r4400 only */
#define CFG_C_UNCACHED_ACCEL            7   /* t5 only */


#define ECC_ECC 0x0                     // Secondary cache ECC <7:0>

#define CACHEERR_PIDX 0x0               // Primary cache index <2:0>
#define CACHEERR_SIDX 0x3               // Secondary cache index <21:3>
#define CACHEERR_EI 0x18                // ECC error on pimary store miss <24>
#define CACHEERR_EB 0x19                // Data error on instrruction error <25>
#define CACHEERR_EE 0x1a                // Error on system address bus <26>
#define CACHEERR_ES 0x1b                // Error accessing processor state <27>
#define CACHEERR_ET 0x1c                // Error on tag field <28>
#define CACHEERR_ED 0x1d                // Error on data field <29>
#define CACHEERR_EC 0x1e                // Cache level of error <30>
#define CACHEERR_ER 0x1f                // Type of reference <31>

#define TAGLO_P 0x0                     // primary tag even parity <0>
#define TAGLO_PSTATE 0x6                // primary cache state <7:6>
#define TAGLO_PTAGLO 0x8                // primary tag low physical address <31:8>

/*
 * Bits in the coprozessor 0 config register.
 */
#define CONF_CM_CACHABLE_NO_WA          0
#define CONF_CM_CACHABLE_WA             1
#define CONF_CM_UNCACHED                2
#define CONF_CM_CACHABLE_NONCOHERENT    3
#define CONF_CM_CACHABLE_CE             4
#define CONF_CM_CACHABLE_COW            5
#define CONF_CM_CACHABLE_CUW            6
#define CONF_CM_CACHABLE_ACCELERATED    7
#define CONF_CM_CMASK                   7
#define CONF_DB                         (1 <<  4)
#define CONF_IB                         (1 <<  5)
#define CONF_SC                         (1 << 17)
#define CONF_AC                         (1 << 23)
#define CONF_HALT                       (1 << 25)

#endif
//
// Define R3000 and R4000 cause register bit offsets.
//
#define CAUSE_XCODE             0x2     // Exception code <6:2>
#define CAUSE_INTPEND           0x8     // Interrupt pending <15:8>
#define CAUSE_CE                0x1c    // Coprocessor unusable <29:28>
#define CAUSE_BD                0x1f    // Branch delay slot <31>

//
// Define R3000 and R4000 processor id register field offsets.
//

#define PRID_REV                0x0     // Revision level <7:0>
#define PRID_IMP                0x8     // Implementation type <15:8>

//
// Define R3000 and R4000 exception codes.
//

#define XCODE_INTERRUPT 0x0             // Interrupt
#define XCODE_MODIFY 0x4                // TLB modify
#define XCODE_READ_MISS 0x8             // TLB read miss
#define XCODE_WRITE_MISS 0xc            // TLB write miss
#define XCODE_READ_ADDRESS_ERROR 0x10   // Read alignment error
#define XCODE_WRITE_ADDRESS_ERROR 0x14  // Write alignment error
#define XCODE_INSTRUCTION_BUS_ERROR 0x18 // Instruction bus error
#define XCODE_DATA_BUS_ERROR 0x1c       // Data bus error
#define XCODE_SYSTEM_CALL 0x20          // System call
#define XCODE_BREAKPOINT 0x24           // Breakpoint
#define XCODE_ILLEGAL_INSTRUCTION 0x28  // Illegal instruction
#define XCODE_COPROCESSOR_UNUSABLE 0x2c // Coprocessor unusable
#define XCODE_INTEGER_OVERFLOW 0x30     // Arithmetic overflow
#define XCODE_

#if defined(R4000)

#define XCODE_TRAP 0x34                 // Trap instruction
#define XCODE_VIRTUAL_INSTRUCTION 0x38  // Virtual instruction coherency
#define XCODE_FLOATING_EXCEPTION 0x3c   // Floating point exception
#define XCODE_WATCHPOINT 0x5c           // Watch point
#define XCODE_PANIC 0x78                // Stack overflow (software)
#define XCODE_VIRTUAL_DATA 0x7c         // Virtual data coherency

#define XCODE_MASK (0x1f << CAUSE_XCODE) // R4000 exception code mask

#define MISS_MASK (XCODE_MASK & \
                        (~(XCODE_READ_MISS ^ XCODE_WRITE_MISS))) //

#endif

//
// Define R4000 page mask values.
//

#if defined(R4000)

#define PAGEMASK_4KB                    0x0

#endif

//
// Define R4000 primary cache states.
//

#if defined(R4000)

#define PRIMARY_CACHE_INVALID           0x0 // primary cache invalid
#define PRIMARY_CACHE_SHARED            0x1 // primary cache shared
                                            //(clean or dirty)
#define PRIMARY_CACHE_CLEAN_EXCLUSIVE   0x2 // primary cache clean exclusive
#define PRIMARY_CACHE_DIRTY_EXCLUSIVE   0x3 // primary cache dirty exclusive

#endif

//
// Define R4000 cache instruction operation codes.
//

#if defined(R4000)

#define INDEX_INVALIDATE_I 0x0          // invalidate primary instruction cache
#define INDEX_WRITEBACK_INVALIDATE_D 0x1 // writeback/invalidate primary data cache
#define INDEX_INVALIDATE_SI 0x2         // invalidate secondary instruction cache
#define INDEX_WRITEBACK_INVALIDATE_SD 0x3 // writeback/invalidate secondary data cache

#define INDEX_LOAD_TAG_I 0x4            // load primary instruction tag indexed
#define INDEX_LOAD_TAG_D 0x5            // load primary data tag indexed
#define INDEX_LOAD_TAG_SI 0x6           // load secondary instruction tag indexed
#define INDEX_LOAD_TAG_SD 0x7           // load secondary data tag indexed

#define INDEX_STORE_TAG_I 0x8           // store primary instruction tag indexed
#define INDEX_STORE_TAG_D 0x9           // store primary data tag indexed
#define INDEX_STORE_TAG_SI 0xa          // store secondary instruction tag indexed
#define INDEX_STORE_TAG_SD 0xb          // store secondary data tag indexed

#define CREATE_DIRTY_EXCLUSIVE_D 0xd    // create dirty exclusive primary data cache
#define CREATE_DIRTY_EXCLUSIVE_SD 0xf   // create dirty exclusive secondary data cache

#define HIT_INVALIDATE_I 0x10           // invalidate primary instruction cache
#define HIT_INVALIDATE_D 0x11           // invalidate primary data cache
#define HIT_INVALIDATE_SI 0x12          // invalidate secondary instruction cache
#define HIT_INVALIDATE_SD 0x13          // invalidate secondary data cache

#define HIT_WRITEBACK_INVALIDATE_D 0x15 // writeback/invalidate primary data cache
#define HIT_WRITEBACK_INVALIDATE_SD 0x17 // writeback/invalidate secondary data cache

#define HIT_WRITEBACK_D 0x19            // writeback primary data cache
#define HIT_WRITEBACK_SD 0x1b           // writeback secondary data cache

#define HIT_SET_VIRTUAL_SI 0x1e         // hit set virtual secondary instruction cache
#define HIT_SET_VIRTUAL_SD 0x1f         // hit set virtual secondary data cache

#endif

//
// Define floating coprocessor registers
//

#define fsrid   $0                      // floating identification register
#define fsr     $31                     // floating status register

//
// Define floating status register bit offsets.
//

#define FSR_RM 0x0
#define FSR_SI 0x2
#define FSR_SU 0x3
#define FSR_SO 0x4
#define FSR_SZ 0x5
#define FSR_SV 0x6
#define FSR_EI 0x7
#define FSR_EU 0x8
#define FSR_EO 0x9
#define FSR_EZ 0xa
#define FSR_EV 0xb
#define FSR_XI 0xc
#define FSR_XU 0xd
#define FSR_XO 0xe
#define FSR_XZ 0xf
#define FSR_XV 0x10
#define FSR_XE 0x11
#define FSR_CC 0x17
#define FSR_FS 0x18

//
// Define TB and cache parameters.
//

#if defined(R4000)

#define PCR_ENTRY 0                     // TB entry numbers (2) for the PCR
#define PDR_ENTRY 2                     // TB entry number (1) for the PDR
#define LARGE_ENTRY 3                   // TB entry number (1) for large entry
#define DMA_ENTRY 4                     // TB entry number (1) for DMA/InterruptSource

#define TB_ENTRY_SIZE (3 * 4)           // size of TB entry
#define FIXED_BASE 0                    // base index of fixed TB entries
#define FIXED_ENTRIES (DMA_ENTRY + 1)   // number of fixed TB entries

#endif

//
// Define cache parameters
//

#define DCACHE_SIZE 4 * 1024            // size of data cache in bytes
#define ICACHE_SIZE 4 * 1024            // size of instruction cache in bytes
#define MINIMUM_CACHE_SIZE 4 * 1024     // minimum size of cache
#define MAXIMUM_CACHE_SIZE 128 * 1024   // maximum size fo cache


#define PHYS_TO_K0(pa)  ((pa)|K0BASE)
#define PHYS_TO_K1(pa)  ((pa)|K1BASE)
#define K0_TO_PHYS(va)  ((va)&(K0SIZE-1))
#define K1_TO_PHYS(va)  ((va)&(K1SIZE-1))
#define K0_TO_K1(va)    ((va)|K1SIZE)
#define K1_TO_K0(va)    ((va)&~K1SIZE)

#define IS_K0SEG(x) ((unsigned)(x) >= K0BASE && (unsigned)(x) < K1BASE)
#define IS_K1SEG(x) ((unsigned)(x) >= K1BASE && (unsigned)(x) < K2BASE)

/* SDE compatibility */
#ifndef KSEG0_BASE
#define KSEG0_BASE  K0BASE
#define KSEG1_BASE  K1BASE
#define PA_TO_KVA0(pa)  PHYS_TO_K0(pa)
#define PA_TO_KVA1(pa)  PHYS_TO_K1(pa)
#define KVA_TO_PA(pa)   K1_TO_PHYS(pa)
#define KVA0_TO_KVA1(v) (void *)K0_TO_K1(v)
#define KVA1_TO_KVA0(v) (void *)K1_TO_K0(v)
#define IS_KVA(v)   ((int)(v) < 0)
#define IS_KVA0(v)  (((unsigned)(v) >> 29) == 0x4)
#define IS_KVA1(v)  (((unsigned)(v) >> 29) == 0x5)
#define IS_KVA01(v) (((unsigned)(v) >> 30) == 0x2)
#endif /* KSEG0_BASE */


// Returns the kernel segment base of a given address
#define KSEGX(a)        ((a) & 0xe0000000)
//#define PHYSADDR(a)       ((a) & 0x1fffffff)
#define KSEG0ADDR(a)        (((a) & 0x1fffffff) | K0BASE)
#define KSEG1ADDR(a)        (((a) & 0x1fffffff) | K1BASE)
#define KSEG2ADDR(a)        (((a) & 0x1fffffff) | K2BASE)
#define KSEG3ADDR(a)        (((a) & 0x1fffffff) | K3BASE
//
// Define subtitle macro
//

#define ENTRYLO_CACHE   0x18
#define ENTRYLO_UNCACHE 0x10

#define ENTRYLO_DIRTY   0x4
#define ENTRYLO_VALID   0x2
#define ENTRYLO_GLOBLE  0x1


#define SBTTL(x)

//
// Define global definition macros.
//

#define END_REGION(Name)               \
        .globl  Name;                  \
Name:;

#define START_REGION(Name)             \
        .globl  Name;                  \
Name:;


//
// Define procedure entry macros
//
#define INT_ENTRY(Name)                     \
    .text;                              \
    .globl  Name;                       \
    .ent Name;                          \
    .set noreorder;                     \
    .set noat;                           \
Name:

#define INT_END(Name)                       \
    .set noat;                          \
    .set reorder;                       \
    .end Name


#define ALTERNATE_ENTRY(Name)           \
        .globl  Name;                   \
Name:;

#define LEAF_ENTRY(Name)                \
        .text;                          \
        .globl  Name;                   \
        .ent    Name, 0;                \
Name:;                                  \
        .frame  sp, 0, ra;              \
        .prologue 0;

#define NESTED_ENTRY(Name, fsize, retrg) \
        .text;                          \
        .globl  Name;                   \
        .ent    Name, 0;                \
Name:;                                  \
        .frame  sp, fsize, retrg;

#define ALTERNATE_ENTRY_S(Name)         \
        .globl  Name;                   \
Name:;

#define SYSTEM_ENTRY(Name)              \
        .text;                          \
        .globl  Name;                   \
        .ent    Name, 0;                \
Name:;                                  \
        .frame  sp, 0, ra;              \
        .prologue 0;

#define LEAF_ENTRY_S(Name, Section)     \
        .text   Section;                \
        .globl  Name;                   \
        .ent    Name, 0;                \
Name:;                                  \
        .frame  sp, 0, ra;              \
        .prologue 0;

#define NESTED_ENTRY_S(Name, fsize, retrg, Section) \
        .text   Section;                \
        .globl  Name;                   \
        .ent    Name, 0;                \
Name:;                                  \
        .frame  sp, fsize, retrg;

// this part is copy form gnu linux mips.h
#define LEAF(name) \
    .text; \
    .globl  name; \
    .ent    name; \
name:

#define XLEAF(name) \
    .text; \
    .globl  name; \
    .aent   name; \
name:

#define SLEAF(name) \
    .text; \
    .ent    name; \
name:

#define END(name) \
    .end    name

#define SEND(name) END(name)

#define EXPORT(name) \
    .globl name; \
name:

#define EXPORTS(name,sz) \
    .globl name; \
name:

#define IMPORT(name, size) \
    .extern name,size

#define BSS(name,size) \
    .comm   name,size

#define LBSS(name,size) \
    .lcomm  name,size

//
// Define exception handling macros.
//

#define EXCEPTION_HANDLER(Handler)      \
        .edata  1, Handler;

#define PROLOGUE_END .prologue 1;

//
// Define exception data section and align.
//

#ifndef HEADER_FILE

//        .edata  0
//        .text

#endif
