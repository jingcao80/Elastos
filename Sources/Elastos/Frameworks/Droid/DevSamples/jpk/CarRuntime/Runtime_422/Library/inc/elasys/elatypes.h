//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELATYPES_H__
#define __ELATYPES_H__

#include <stddef.h>
#include <elans.h>

#ifndef _win32
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

#if defined(_MSC_VER) || defined(_EVC)
#define DECLSPEC_SELECTANY      __declspec(selectany)
#define DECLSPEC_NOVTABLE       __declspec(novtable)
#define DECLSPEC_UUID(x)        __declspec(uuid(x))
#define DECL_NAKED              __declspec(naked)
#define DECL_PACKED
#define DECL_SECTION(s)
#elif defined(_GNUC)
#define DECL_REGPARM(n)         __attribute__ ((regparm(n)))
#define DECL_SECTION(s)         __attribute__ ((section(s)))
#define DECL_PACKED             __attribute__ ((packed))
#define DECL_ALIGN(n)           __attribute__ ((aligned(n)))
#define DECL_NORETURN           __attribute__ ((noreturn))
#define DECL_NAKED
#endif // defined(_MSC_VER)

//typedef __int64 hyper;

#define INFINITE -1     // Infinite timeout

typedef unsigned char   byte;
typedef int             bool_t;
#ifndef _UINT_T_DEFINED_
#define _UINT_T_DEFINED_
typedef unsigned int    uint_t;
#endif
typedef void *          virtaddr_t;
typedef unsigned long   ulong_t;
typedef ulong_t         oid_t;
typedef unsigned char   uchar_t;
typedef uchar_t         byte_t;

_ELASTOS_NAMESPACE_BEGIN

/** @addtogroup BaseTypesRef
  *   @{
  */
typedef void Void;
/** @} */

/** @addtogroup CARTypesRef
  *   @{
  */
typedef void * PVoid;

typedef signed char Int8;
typedef unsigned char UInt8;
typedef UInt8 Byte;
typedef char Char8;
typedef unsigned short Char16;
typedef unsigned int Char32;
typedef signed short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
#ifdef __GNUC__
typedef long long Int64;
typedef unsigned long long UInt64;
#else
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#endif
typedef float Float;
typedef double Double;
typedef unsigned char Boolean;
typedef Int32 ECode;
/** @} */

typedef Char8 *PChar8;
typedef Char16 *PChar16;
typedef Char32 *PChar32;
typedef Int8 *PInt8;
typedef Byte *PByte;
typedef UInt8 *PUInt8;
typedef Int16 *PInt16;
typedef UInt16 *PUInt16;
typedef Int32 *PInt32;
typedef UInt32 *PUInt32;
typedef Int64 *PInt64;
typedef UInt64 *PUInt64;
typedef Float *PFloat;
typedef Double *PDouble;
typedef Boolean *PBoolean;

/** @addtogroup CARTypesRef
  *   @{
  */
typedef UInt8  Flags8;
typedef UInt16 Flags16;
typedef UInt32 Flags32;
typedef UInt64 Flags64;
/** @} */

typedef Flags8  *PFlag8;
typedef Flags16 *PFlag16;
typedef Flags32 *PFlag32;
typedef Flags64 *PFlag64;

/** @addtogroup CARTypesRef
  *   @{
  */
typedef UInt32 Handle32;
typedef UInt64 Handle64;
/** @} */

typedef Handle32 *PHandle32;
typedef Handle64 *PHandle64;

/** @addtogroup CARTypesRef
  *   @{
  */
typedef Int32 Second32;
typedef Int32 Millisecond32;
typedef Int32 Microsecond32;

typedef Int64 Second64;
typedef Int64 Millisecond64;
typedef Int64 Microsecond64;
/** @} */

typedef Second32*      PSecond32;
typedef Millisecond32* PMillisecond32;
typedef Microsecond32* PMicrosecond32;

typedef Second64*      PSecond64;
typedef Millisecond64* PMillisecond64;
typedef Microsecond64* PMicrosecond64;

/** @addtogroup CARTypesRef
  *   @{
  */
//System Type
typedef Int32 MemorySize;
typedef Int32 ThreadCreationFlags;
typedef Int32 ModuleLoadFlags;
typedef Int32 MemoryMapFlags;
typedef Int32 EventState;
typedef Int32 SystemEvents;
typedef PVoid TLValue;
typedef UInt32 Address;

typedef struct SystemTime
{
    Int32 seconds;
    Int32 microseconds;
} SystemTime, *PSystemTime;
/** @} */

/** @addtogroup SystemRef
  *   @{
  */
typedef struct CriticalSection
{
    Int32   cWaiters;
    Int32   cNested;
    Int32   nOwnerId;
    PVoid   event;
    Int32   bLocked;
    PVoid   reserved;
    PVoid   sequenced;
} CriticalSection, *PCriticalSection, CRITICAL_SECTION, *PCRITICAL_SECTION;
/** @} */

/** @addtogroup CARTypesRef
  *   @{
  */
typedef struct _EMuid
{
    UInt32 Data1;
    UInt16 Data2;
    UInt16 Data3;
    UInt8  Data4[8];
}EMuid, *PEMuid;

typedef struct _EGuid
{
    EMuid   clsid;
    char    *pUunm;
    UInt32 carcode;
}EGuid, *PEGuid;

typedef EGuid ClassID;
typedef EMuid InterfaceID;
typedef InterfaceID EIID;
/** @} */

typedef ClassID *PClassID;
typedef InterfaceID *PInterfaceID;
typedef EIID* PEIID;

#if defined (_arm)
typedef struct
{
    _ELASTOS UInt32    r0;
    _ELASTOS UInt32    r1;
    _ELASTOS UInt32    r2;
    _ELASTOS UInt32    r3;
    _ELASTOS UInt32    r4;
    _ELASTOS UInt32    r5;
    _ELASTOS UInt32    r6;
    _ELASTOS UInt32    r7;
    _ELASTOS UInt32    r8;
    _ELASTOS UInt32    r9;
    _ELASTOS UInt32    r10;
    _ELASTOS UInt32    r11;
    _ELASTOS UInt32    r12;
    _ELASTOS UInt32    sp;
    _ELASTOS UInt32    lr;
    _ELASTOS UInt32    pc;
    _ELASTOS UInt32    cpsr;
} ThreadContext;
#elif defined (_x86)
#if defined(_win32)

#define DBG_CONTINUE                     (0x00010002L)
#define DBG_EXCEPTION_NOT_HANDLED        (0x80010001L)

// this assumes that i386 has identical context records
#define CONTEXT_i386    0x00010000

// SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L)

// AX, BX, CX, DX, SI, DI
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L)

// DS, ES, FS, GS
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L)

// 387 state
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L)

// DB 0-3,6,7
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L)

// cpu specific extensions
#define CONTEXT_EXTENDED_REGISTERS  (CONTEXT_i386 | 0x00000020L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER | \
                      CONTEXT_SEGMENTS)

#define CONTEXT_ALL (CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS | \
                    CONTEXT_FLOATING_POINT | CONTEXT_DEBUG_REGISTERS | \
                    CONTEXT_EXTENDED_REGISTERS)

#define SIZE_OF_80387_REGISTERS         80

#define MAXIMUM_SUPPORTED_EXTENSION     512

typedef struct _FLOATING_SAVE_AREA
{
    _ELASTOS UInt32   ControlWord;
    _ELASTOS UInt32   StatusWord;
    _ELASTOS UInt32   TagWord;
    _ELASTOS UInt32   ErrorOffset;
    _ELASTOS UInt32   ErrorSelector;
    _ELASTOS UInt32   DataOffset;
    _ELASTOS UInt32   DataSelector;
    _ELASTOS Byte    RegisterArea[SIZE_OF_80387_REGISTERS];
    _ELASTOS UInt32   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _Context
{
    _ELASTOS UInt32   ContextFlags;
    _ELASTOS UInt32   Dr0;
    _ELASTOS UInt32   Dr1;
    _ELASTOS UInt32   Dr2;
    _ELASTOS UInt32   Dr3;
    _ELASTOS UInt32   Dr6;
    _ELASTOS UInt32   Dr7;
    FLOATING_SAVE_AREA FloatSave;
    _ELASTOS UInt32   SegGs;
    _ELASTOS UInt32   SegFs;
    _ELASTOS UInt32   SegEs;
    _ELASTOS UInt32   SegDs;
    _ELASTOS UInt32   Edi;
    _ELASTOS UInt32   Esi;
    _ELASTOS UInt32   Ebx;
    _ELASTOS UInt32   Edx;
    _ELASTOS UInt32   Ecx;
    _ELASTOS UInt32   Eax;
    _ELASTOS UInt32   Ebp;
    _ELASTOS UInt32   Eip;
    _ELASTOS UInt32   SegCs;
    _ELASTOS UInt32   EFlags;
    _ELASTOS UInt32   Esp;
    _ELASTOS UInt32   SegSs;
    _ELASTOS Byte    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
} ThreadContext;

#else

typedef struct
{
    _ELASTOS UInt32  ebx;
    _ELASTOS UInt32  ecx;
    _ELASTOS UInt32  edx;
    _ELASTOS UInt32  esi;
    _ELASTOS UInt32  edi;
    _ELASTOS UInt32  ebp;
    _ELASTOS UInt32  eax;
    _ELASTOS UInt32  ds;
    _ELASTOS UInt32  es;
    union
    {
        uint8_t   irq;
        _ELASTOS UInt32  errorcode;
        _ELASTOS UInt32  uSystemCallNo;
    } u;
    _ELASTOS UInt32  eip;
    _ELASTOS UInt32  cs;
    _ELASTOS UInt32  eflags;
    _ELASTOS UInt32  _esp;
    _ELASTOS UInt32  _ss;
} ThreadContext;

#endif // _win32
#endif // _arm

#if defined(__cplusplus)
typedef const EMuid&        REMuid;
typedef const ClassID&      RClassID;
typedef const InterfaceID&  REIID;
typedef const InterfaceID&  RInterfaceID;
#else // !__cplusplus
typedef const EMuid* const       REMuid;
typedef const ClassID * const    RClassID;
typedef const InterfaceID* const REIID;
typedef const InterfaceID* const RInterfaceID;
#endif // !__cplusplus

_ELASTOS_NAMESPACE_END

#if defined(_linux) || defined(_mips) && defined(_GNUC)
#define __cdecl
#define __stdcall
#endif

#define CDECL                   __cdecl
#define STDCALL                 __stdcall

#define EXTERN      extern
#define STATIC      static
#define CONST       const
#define VOLATILE    volatile

//---- EXTERN_C ----
#ifdef __cplusplus
#define EXTERN_C            extern "C"
#define EXTERN_C_BEGIN      EXTERN_C {
#define EXTERN_C_END        }
#else
#define EXTERN_C            extern
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

//---- CAR_INLINE ----
#ifdef __cplusplus
#define CAR_INLINE inline
#elif defined(_MSC_VER)
#define CAR_INLINE __inline
#elif defined(DIAB_COMPILER)
#define CAR_INLINE /* only pragmas supported, don't bother */
#else
#define CAR_INLINE static __inline__ /* GNU c style */
#endif

// ---- Boolean Value ----
#ifdef _GNUC
  #ifndef TRUE
  #define TRUE    ((_ELASTOS Boolean)1)
  #endif
  #ifndef FALSE
  #define FALSE   ((_ELASTOS Boolean)0)
  #endif
#else
  #ifndef TRUE
  #define TRUE    ((_ELASTOS Boolean)1)
  #endif
  #ifndef FALSE
  #define FALSE   ((_ELASTOS Boolean)0)
  #endif
#endif

//---- ASM ----
#if defined(_EVC) && defined(_mips)
EXTERN_C void __asm(char*, ...);
#endif

#ifdef _GNUC
#define __asm           __asm__
#define _asm            __asm
#define ASM_VOLATILE    __asm__ __volatile__
#define ASM             ASM_VOLATILE
#elif defined(_EVC) || defined(_MSVC)
#define __asm           __asm
#define _asm            __asm
#define ASM_VOLATILE    __asm
#define ASM             ASM_VOLATILE
#else
#error unknown compiler
#endif

#define interface           struct
#ifdef _GNUC
#define CAR_INTERFACE(x)    interface
#else
#define CAR_INTERFACE(x)    interface DECLSPEC_UUID(x) DECLSPEC_NOVTABLE
#endif

#ifdef _DEBUG
#define CarClass(name)      class name : public _##name name##_CallbackKeyword_Checking
#else
#define CarClass(name)      class name : public _##name
#endif

// ---- API Call ----
#define ELAPICALLTYPE       CDECL
#define CARAPICALLTYPE      STDCALL

#define ELAPI               EXTERN_C _ELASTOS ECode ELAPICALLTYPE
#define ELAPI_(type)        EXTERN_C type ELAPICALLTYPE

#define CARAPI              _ELASTOS ECode CARAPICALLTYPE
#define CARAPI_(type)       type CARAPICALLTYPE

#define ELFUNCCALLTYPE          CDECL
#define ELFUNC                  _ELASTOS ECode ELFUNCCALLTYPE
#define ELFUNC_(type)           type ELFUNCCALLTYPE

typedef _ELASTOS ECode (ELFUNCCALLTYPE *PThreadMain)(_ELASTOS PVoid);
typedef _ELASTOS Void (ELFUNCCALLTYPE *PThreadQuitRoutine)(_ELASTOS PVoid);
typedef _ELASTOS Void (ELFUNCCALLTYPE *PProcessExitRoutine)(_ELASTOS PVoid);

#ifdef __cplusplus
}
#endif

#endif // __ELATYPES_H__
