//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __DEBUGDEF_H__
#define __DEBUGDEF_H__

#include <elatypes.h>

_ELASTOS_NAMESPACE_USING

#define DBGSTUB_BUFFER_SIZE     1000

typedef enum _DBGSTUB_MGR_CODE
{
    DBGMANAGER_CODE_CREATE_DBGSTUB = 0,
    DBGMANAGER_CODE_DESTROY_DBGSTUB,
    DBGMANAGER_CODE_DUMPINFO,
} DBGSTUB_MGR_CODE;

typedef enum _DBGSTUB_CTRL_CODE
{
    DBGSTUB_CTRL_CODE_ATTACH_PROCESS = 0,
    DBGSTUB_CTRL_CODE_DETACH_PROCESS,
    DBGSTUB_CTRL_CODE_WAIT_FOR_DEBUGEVENT,
    DBGSTUB_CTRL_CODE_CONTINUE,
    DBGSTUB_CTRL_CODE_GET_THREAD_CONTEXT,
    DBGSTUB_CTRL_CODE_SET_THREAD_CONTEXT,
    DBGSTUB_CTRL_CODE_READ_PROCESS_MEMORY,
    DBGSTUB_CTRL_CODE_WRITE_PROCESS_MEMORY,
    DBGSTUB_CTRL_CODE_SET_BREAKPIONT,
    DBGSTUB_CTRL_CODE_GET_THREAD_STACK_BASE,
    DBGSTUB_CTRL_CODE_SUSPEND_PROCESS,
} DBGSTUB_CTRL_CODE;

#define FAMILY_DBGSTUB       0X76    // Debugger Error Family codes
#define FAMILY_DBGSTUB_ERROR(c) \
    MAKE_ECODE(SEVERITY_ERROR, FAMILY_DBGSTUB, c)

#define E_DBGSTUB_ALREADY_EXIST        FAMILY_DBGSTUB_ERROR(0x01)
#define E_DBGSTUB_NOT_EXIST            FAMILY_DBGSTUB_ERROR(0x02)
#define E_DBGSTUB_PROCESS_NOT_EXIST    FAMILY_DBGSTUB_ERROR(0x03)
#define E_DBGSTUB_NOT_DBGSTUB_OWNER    FAMILY_DBGSTUB_ERROR(0x04)
#define E_DBGSTUB_ALREADY_USED         FAMILY_DBGSTUB_ERROR(0x05)
#define E_DBGSTUB_ACCESS_DENIED        FAMILY_DBGSTUB_ERROR(0x06)
#define E_DBGSTUB_DEBUG_ITSELF         FAMILY_DBGSTUB_ERROR(0x07)
#define E_DBGSTUB_OWNER_EXIST          FAMILY_DBGSTUB_ERROR(0x08)
#define E_DBGSTUB_WAITER_NOT_EXIST     FAMILY_DBGSTUB_ERROR(0x09)
#define E_DBGSTUB_THREAD_NOT_EXIST     FAMILY_DBGSTUB_ERROR(0x0a)

#define DEBUGGEE_EXCEPTION          1
#define DEBUGGEE_THREAD_CREATE      2
#define DEBUGGEE_PROCESS_CREATE     3
#define DEBUGGEE_THREAD_EXIT        4
#define DEBUGGEE_PROCESS_EXIT       5
#define DEBUGGEE_LOAD_DLL           6
#define DEBUGGEE_UNLOAD_DLL         7
#define DEBUGGEE_OUTPUT_STRING      8
#define DEBUGGEE_RIP                9
#define DEBUGGEE_PROCESS_STOP       10

#if defined(_arm)
#define DBG_INS_LENGTH      4
#elif defined(_x86)
#define DBG_INS_LENGTH      1
#endif

#if defined(_arm) || (defined(_x86) && !defined(_win32))

typedef struct _EXCEPTION_RECORD
{
    _ELASTOS UInt32 ExceptionCode;
    _ELASTOS UInt32 ExceptionFlags;
    _ELASTOS PVoid  ExceptionAddress;
    _ELASTOS UInt32 NumberParameters;
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_DEBUG_INFO
{
    EXCEPTION_RECORD ExceptionRecord;
    _ELASTOS UInt32 dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO
{
    _ELASTOS UInt32 ThreadID;
    _ELASTOS UInt32 lpThreadLocalBase;
    _ELASTOS UInt32 lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO
{
    _ELASTOS UInt32 ThreadID;
    _ELASTOS UInt32 lpBaseOfImage;
    _ELASTOS UInt32 lpThreadLocalBase;
    _ELASTOS UInt32 lpStartAddress;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO
{
    _ELASTOS UInt32 dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO
{
    _ELASTOS UInt32 dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO
{
    _ELASTOS PVoid lpBaseOfDll;
    char lpImageName[260];
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO
{
    _ELASTOS PVoid lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO
{
    char * lpDebugStringData;
    _ELASTOS UInt16 fUnicode;
    _ELASTOS UInt16 nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _DEBUG_EVENT
{
    _ELASTOS UInt32 dwDebugEventCode;
    _ELASTOS UInt32 dwThreadId;
    union
    {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

#elif defined(_win32)

#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct _EXCEPTION_RECORD
{
    _ELASTOS UInt32 ExceptionCode;
    _ELASTOS UInt32 ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    _ELASTOS PVoid  ExceptionAddress;
    _ELASTOS UInt32 NumberParameters;
    Int32  ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_DEBUG_INFO
{
    EXCEPTION_RECORD ExceptionRecord;
    _ELASTOS UInt32 dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO
{
    _ELASTOS PVoid hThread;
    _ELASTOS PVoid lpThreadLocalBase;
//    LPTHREAD_START_ROUTINE lpStartAddress;
    _ELASTOS PVoid lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO
{
    _ELASTOS PVoid hFile;
    _ELASTOS PVoid hProcess;
    _ELASTOS PVoid hThread;
    _ELASTOS PVoid lpBaseOfImage;
    _ELASTOS UInt32 dwDebugInfoFileOffset;
    _ELASTOS UInt32 nDebugInfoSize;
    _ELASTOS PVoid lpThreadLocalBase;
//    LPTHREAD_START_ROUTINE lpStartAddress;
    _ELASTOS PVoid lpStartAddress;
    _ELASTOS PVoid lpImageName;
    _ELASTOS UInt16 fUnicode;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO
{
    _ELASTOS UInt32 dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO
{
    _ELASTOS UInt32 dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO
{
    _ELASTOS PVoid hFile;
    _ELASTOS PVoid lpBaseOfDll;
    _ELASTOS UInt32 dwDebugInfoFileOffset;
    _ELASTOS UInt32 nDebugInfoSize;
    _ELASTOS PVoid lpImageName;
    _ELASTOS UInt16 fUnicode;
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO
{
    _ELASTOS PVoid lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO
{
    char * lpDebugStringData;
    _ELASTOS UInt16 fUnicode;
    _ELASTOS UInt16 nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    _ELASTOS UInt32 dwError;
    _ELASTOS UInt32 dwType;
} RIP_INFO, *LPRIP_INFO;

typedef struct _DEBUG_EVENT
{
    _ELASTOS UInt32 dwDebugEventCode;
    _ELASTOS UInt32 dwProcessId;
    _ELASTOS UInt32 dwThreadId;
    union
    {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

#else

#error Unknown architecture / platform

#endif

#if defined(_arm)

typedef struct _Context
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
} Context;

#elif defined(_x86)

#if defined(_win32)

typedef ThreadContext Context;

#else

typedef struct _DebugRegistersInfo
{
    _ELASTOS UInt32  dr0;
    _ELASTOS UInt32  dr1;
    _ELASTOS UInt32  dr2;
    _ELASTOS UInt32  dr3;
    _ELASTOS UInt32  dr4;
    _ELASTOS UInt32  dr5;
    _ELASTOS UInt32  dr6;
    _ELASTOS UInt32  dr7;
} DebugRegisterInfo;

typedef struct _Context
{
    ThreadContext m_Context;
    DebugRegisterInfo m_DebugRegInfo;
} Context;

#endif // _win32

#endif // _arm

#endif // __DEBUGDEF_H__
