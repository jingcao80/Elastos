//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELADEF_H__
#define __ELADEF_H__

#include <elatypes.h>
#include <elaerror.h>

#ifndef _devtools
#include <pthread.h>
#endif

#if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
#include <assert.h>
#endif

#define DECL_ASMLINKAGE         EXTERN_C
#define DECL_LINKERSYMBOL       EXTERN_C

// Version parsed out into numeric values
//
#define ELASTOS_MAJOR_VERSION   2
#define ELASTOS_MINOR_VERSION   1
#define ELASTOS_BUILD_NUMBER    0

// Version as a string
//
#define ELASTOS_VERSION         "2.1"

// Version as a single 4-byte hex number, which can be used for numeric
// comparisons, e.g. #if ELASTOS_VERSION_HEX >= ...
//
#define ELASTOS_VERSION_HEX  ((ELASTOS_MAJOR_VERSION << 24)      \
    | (ELASTOS_MINOR_VERSION << 16) | ELASTOS_BUILD_NUMBER)

//path separator
#define CH_PATH_SEPARATOR '/'
#define WC_PATH_SEPARATOR L'/'
#define IS_PATH_SEPARATOR(x) ((x) == '/' || (x) == '\\')
#define STR_PATH_SEPARATOR "/"
#define WCS_PATH_SEPARATOR L"/"

#if defined(_RELEASE) && !defined(_PRERELEASE) && !defined(_TEST_TYPE)
//#define ELASTOS_RC
#endif

#ifndef _RELEASE

// macros for mutex & crital section deadlock detection in test
#if (_TEST_TYPE == 9) /* Test for Deadlock Detection */
#define MUTEX_DEADLOCK_DETECT
#define CRITICAL_SECTION_DEADLOCK_DETECT
#endif // _TEST_TYPE == 9

#ifndef CRITICAL_SECTION_DEADLOCK_DETECT
#if !defined(_RELEASE) || defined(_PRERELEASE)
#define MULTI_RESOURCE_DEADLOCK_DETECT
#endif //MULTI_RESOURCE_DEADLOCK_DETECT
#endif

// macro for kernel memory spy
#define KMEM_CHECKPOINT_MAX_SUM 8
#define KMEM_CURRENT_POINT KMEM_CHECKPOINT_MAX_SUM + 1
#if defined(_x86) || defined(_arm)
//#define KERNEL_MEMORY_DETECT
#endif // _x86 || _arm
#endif

// macro for process heap memory spy
#if defined(_x86) || defined(_arm)
#if !defined(_RELEASE) || defined(_PRERELEASE)
#define MEMORYSPY
#endif
#endif // _x86 _arm

// arm debugger for msvc

#ifdef _arm
#if !defined(_RELEASE) || defined(_PRERELEASE)
#define _ELADBGAGENT
#endif //_RELEASE
#endif //_arm

#if defined(_x86) || defined(_arm)
#if !defined(_RELEASE) || defined(_PRERELEASE)
#define _KDBAGENT
#define _UDBAGENT
#endif
#endif // _x86 || _arm

// If define this then use Cache Manager
//#define CACHEMANAGER

#if defined(_GNUC)
#if defined(_mips) || (defined(__GNUC__) && (__GNUC__ >= 4))
#define DECL_ASMENTRY(lable) \
        ASM(".global "#lable";" \
            ".align;" \
            #lable":");

#else
#define DECL_ASMENTRY(lable) \
        ASM(".global _"#lable";" \
            ".align;" \
            "_"#lable":");
#endif
#elif defined(_EVC)
#define DECL_ASMENTRY(lable)
#elif defined(_MSVC)
#define DECL_ASMENTRY(lable) \
        ASM(".global _"#lable";" \
            ".align;" \
            "_"#lable":");
#else
#error unknown compiler
#endif

//
// WinNT compatible dll entry Op values
//
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0

#ifndef _devtools

//
// System definitions
//
#define THREAD_MINIMUM_AVAILABLE        127

#define TLS_MINIMUM_AVAILABLE           52
#define TLS_TOTAL_SLOTS                 64
#define TLS_INVALID_INDEX               ((int)0xffffffff)

EXTERN_C pthread_key_t *getTlSystemSlotBase();

#define TL_SYSTEM_SLOT_BASE             (getTlSystemSlotBase())
#define TL_CALLBACK_SLOT                (*(TL_SYSTEM_SLOT_BASE + 3))
#define TL_HELPER_INFO_SLOT             (*(TL_SYSTEM_SLOT_BASE + 5))
#define TL_ORG_CALLBACK_SLOT            (*(TL_SYSTEM_SLOT_BASE + 7))

#endif // _devtools

#define MAXIMUM_WAIT_EVENTS             64     // Maximum number of wait events

#define MAXIMUM_SERVICE_NAME_LENGTH     256

#define MAXIMUM_OWNED_READER_LOCKS      12

#define MAXIMUM_FULL_PATH_LENGTH        260    // Contains the L'\0'

#define MAXIMUM_ARGC                    128
#define MAXIMUM_ARGS_LENGTH             1024   // Contains the L'\0'

#define MAXIMUM_THREAD_NAME_LENGTH      64     // Contains the L'\0'


//
// Helper Info Flag
//
#define HELPER_CHILD_CALL_PARENT          0x00000001
#define HELPER_ENTERED_PROTECTED_ZONE     0x00000002
#define HELPER_ASYNC_CALLING              0x00000004
#define HELPER_CARHOST_CALLING            0x00000008
#define HELPER_PROBE_CALLBACK_INTERFACE   0x00000010
#define HELPER_CALLING_CALLBACK_FILTER    0x00000020


//
// Error Info
//
#define ERROR_INFO_SIZE         512
#define MAX_ERROR_STRING        ((ERROR_INFO_SIZE - sizeof(WORD)) / 2)
#define MAX_ERROR_STRING_A      (ERROR_INFO_SIZE - sizeof(WORD))
#define MAX_ERROR_URL           (ERROR_INFO_SIZE - sizeof(WORD))

//
// Debug out
//
#define ELADBG_NORMAL           1
#define ELADBG_CREF             2
#define ELADBG_WARNING          3
#define ELADBG_ERROR            4
#define ELADBG_FATAL            5
#define ELADBG_NONE             100

#define ELADBG_LEVEL            ELADBG_WARNING

#if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
#define ELA_DBGOUT(level, exp)  \
    ((level) >= ELADBG_LEVEL ? (void)(exp) : (void)0)
#else
#define ELA_DBGOUT(level, exp)
#endif // ELA_DBGOUT

#if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
#define ELA_ASSERT_WITH_BLOCK(cond) for ( ; !(cond) ; assert(cond) )
#else
#define ELA_ASSERT_WITH_BLOCK(cond) if (0)
#endif // ELA_ASSERT_WITH_BLOCK

//
//  Profiling
//
// When this value is defined, measures will be read in key points in the code
// to be used as a reference to analyze or check the performance among
// software releases.
#define PROFILING_TRACES

#ifdef PROFILING_TRACES
#define PROFILING(x) {x;}
#else
#define PROFILING(x)
#endif

//
//  System macros
//
#define SIGNALED     1
#define UNSIGNALED   0

//
//  Utilities
//
#define HINIBBLE(u8)            ((uint8_t)(u8)  >> 4)
#define LONIBBLE(u8)            ((uint8_t)(u8)  &  0xf)

#define HIBYTE(u16)             ((uint8_t)((uint16_t)(u16) >> 8))
#define LOBYTE(u16)             ((uint8_t)((uint16_t)(u16) &  0xff))

#define HIWORD(u32)             ((uint16_t)((uint32_t)(u32) >> 16))
#define LOWORD(u32)             ((uint16_t)((uint32_t)(u32) &  0xffff))

#define HIDWORD(u64)            ((uint32_t)((uint64_t)(u64) >> 32))
#define LODWORD(u64)            ((uint32_t)((uint64_t)(u64) &  0xffffffff))

#define __BYTE0(u32)            ((uint8_t)((uint32_t)(u32) &  0xff))
#define __BYTE1(u32)            ((uint8_t)((uint32_t)(u32) >> 8))
#define __BYTE2(u32)            ((uint8_t)((uint32_t)(u32) >> 16))
#define __BYTE3(u32)            ((uint8_t)((uint32_t)(u32) >> 24))

#define MKWORD(u8low, u8high) \
        ((uint16_t)(((uint8_t)(u8low)) \
            | ((uint16_t)(uint8_t)(u8high) << 8)))

#define MKDWORD(u16low, u16high) \
        ((uint32_t)(((uint16_t)(u16low)) \
            | ((uint32_t)(uint16_t)(u16high) << 16)))

#define LENGTHOF(array)         (sizeof(array) / sizeof((array)[0]))
#define lengthof(array)         LENGTHOF(array)

#define THIS(objptr, type, member) \
        ((type *)((char *)(objptr) - offsetof(type, member)))

#define __8BIT(n)               ((uint8_t)1u  << (n))
#define __16BIT(n)              ((uint16_t)1u << (n))
#define __32BIT(n)              (1ul << (n))
#define __64BIT(n)              (1ull << (n))

#define MAX(a, b)               (((a) > (b))? (a) : (b))
#define MIN(a, b)               (((a) < (b))? (a) : (b))

#define ROUNDUP(n, size)        ((((n) + (size) - 1) / (size)) * (size))
#define ROUNDUP2(n, size)       (((n) + (size) - 1) & ~((size) - 1))

#define ROUNDDOWN(n, size)      (((n) / (size)) * (size))
#define ROUNDDOWN2(n, size)     ((n) & ~((size) - 1))

#define IS_ALIGNMENT(n, size)   (0 == ((size_t)(n) % (size)))
#define IS_ALIGNMENT2(n, size)  (0 == ((size_t)(n) & ((size) - 1)))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifdef __cplusplus

template <class type>
CAR_INLINE const type &Max(const type &a, const type &b)
{
    if (a > b)
        return a;
    else
        return b;
}

template <class type>
CAR_INLINE const type &Min(const type &a, const type &b)
{
    if (a < b)
        return a;
    else
        return b;
}

template <class type>
CAR_INLINE void Swap(type &a, type &b)
{
    type temp = a;
    a = b;
    b = temp;
}

template <class type>
CAR_INLINE type RoundUp(type n, size_t size)
{
    return ((n + size - 1) / size) * size;
}

template <class type>
CAR_INLINE type RoundUp2(type n, size_t size)
{
    return (n + size - 1) & ~(size - 1);
}

template <class type>
CAR_INLINE type RoundDown(type n, size_t size)
{
    return (n / size) * size;
}

template <class type>
CAR_INLINE type RoundDown2(type n, size_t size)
{
    return n & ~(size - 1);
}

template <class type>
CAR_INLINE bool_t IsAlignment(type n, size_t size)
{
    return 0 == ((size_t)n % size);
}

template <class type>
CAR_INLINE bool_t IsAlignment2(type n, size_t size)
{
    return 0 == ((size_t)n & (size - 1));
}

#endif // __cplusplus

#endif //__ELADEF_H__
