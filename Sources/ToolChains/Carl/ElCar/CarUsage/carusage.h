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
