//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __PARERR_H__
#define __PARERR_H__

typedef enum LubeErrorNo {
    Lube_NoError = 0,

    // Warnings
    Lube_WarningStart        = 0x0001,

    // Errors
    Lube_ErrorStart          = 0x0100,
    Lube_E_UnexpectEOF,
    Lube_E_UnexpectSymbol,
    Lube_E_SymbolTooLong,
    Lube_E_BufOverflow,
    Lube_E_ExpectSymbol,
    Lube_E_OutOfMemory,
    Lube_E_NotFound,
    Lube_E_UndefinedSymbol,
    Lube_E_OpenFile,
    Lube_E_InvalidInput,
    Lube_E_InvalidWithObj,
    Lube_E_NoValidObject,
    Lube_E_IllegalAll,
    Lube_E_InvalidMember,
    Lube_E_UndefFunc,
    Lube_E_InvalidFuncArg,
    Lube_E_MultiElse,
    Lube_E_HasNoOrder,
    Lube_E_InvalidCondMem,
    Lube_E_HasNoAttrib,
    Lube_E_HasNoType,
    Lube_E_InvalidEmbed,
} LubeErrorNo;

extern "C" void ErrorReport(LubeErrorNo, ...);
extern void ErrorReporterInit(const char *);
extern void ErrorStringFix(const char *, const char *);
extern void * SaveErrorContext();
extern void RestoreErrorContext(void *);

extern int g_nErrorNumber;
extern int g_nWarningNumber;

#endif // __PARERR_H__
