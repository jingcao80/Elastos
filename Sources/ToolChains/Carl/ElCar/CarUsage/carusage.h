//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CARUSAGE_H__
#define __CARUSAGE_H__

#include <stdio.h>
#include <wtypes.h>
#include <assert.h>

#define USAGE_OK          0
#define USAGE_FALSE       1
#define USAGE_FAIL        -1

typedef struct CommandArgs {
    int        mAttribs;

    char        *pszInputFile;
    char        *pszOutputFile; /* output into one file! */
}   CommandArgs;

typedef enum CommandOptions {
    Command_r_ForceOverride     = 0x0001,
    Command_r_Interface         = 0x0002,
    Command_r_Class             = 0x0004,
    Command_r_Aspect            = 0x0008,
    Command_r_Generic           = 0x0010,
    Command_r_Enum              = 0x0020,
    Command_r_Const             = 0x0040,
    Command_r_Struct            = 0x0080,
    Command_r_Regime            = 0x0100,
    Command_r_Method            = 0x0200,
    Command_r_BaseClass         = 0x0400,
    Command_r_Callback          = 0x0800,
    Command_r_All               = 0x1000,
}   CommandOptions;

extern int ParseArgs(int, char **, CommandArgs *);

#endif // __CARUSAGE_H__
