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

#ifndef __CARC_H__
#define __CARC_H__

#include <stdio.h>
//#include <wtypes.h>

#include <elatypes.h>
#include "clsdef.h"
#include "token.h"
#include "carerr.h"

typedef struct CommandArgs {
    DWORD       mAttribs;

    char        *pszSource;
    char        *pszCLS;
    char        *pszExCLS;
    char        *pszCAR;

    char        *pszLibraryPath;
    char        *pszSourcePath;
    char        *pszFactoryUrl;
}   CommandArgs;

typedef enum CommandOption {
    Command_s_NoSys     = 0x00001,
    Command_e_NoElastos = 0x00002,
    Command_a_Compress  = 0x00004,
    Command_t_Warn2Err  = 0x00008,
    Command_w_SuppWarn  = 0x00010,
    Command_c_GenCLS    = 0x00020,
    Command_r_GenCAR    = 0x00040,
    Command_d_Depend    = 0x00080,
    Command_A_FreeModel = 0x00100,
    Command_E_GenExCLS  = 0x00200,
    Command_i_IgrName   = 0x00400,
    Command_k_InKernel  = 0x00800,
    Command_u_WeakRef   = 0x01000,
    Command_n_NakedMode = 0x02000,
}   CommandOption;

extern int ParseArgs(int, char **, CommandArgs *);
extern int LoadSystemLibrary(BOOL);
extern int DoCompiling(const char *, CLSModule *);
extern void SetSourcePath(const char *);
extern void SetFactoryUrl(const char *);

extern int AddCLSLibrary(CLSModule *);
extern int RetrieveClass(const char *, const char *, CLSModule *, BOOL);
extern int RetrieveInterface(const char *, const char *, CLSModule *, BOOL);
extern int RetrieveStruct(const char *, CLSModule *, BOOL);
extern int RetrieveEnum(const char *, const char *, CLSModule *, BOOL);
extern int RetrieveConst(const char *, CLSModule *, BOOL);
extern int RetrieveAlias(const char *, CLSModule *, BOOL);
extern int RetrieveMethod(const char *pszName, CLSModule *pModule,
                                        const ClassInterface *pDesc);
extern int MergeCLS(const CLSModule *, CLSModule *);
extern int RetrieveIdentifyType(
                const char *, const char *, CLSModule *, TypeDescriptor *, BOOL);
extern void DestroyAllLibraries();

extern void GenCarDependences(const char *pszden, int nRes);

#endif // __CARC_H__
