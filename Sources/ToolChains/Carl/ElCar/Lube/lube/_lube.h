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

#ifndef ___LUBE_H__
#define ___LUBE_H__

#include <lube.h>

typedef struct CommandArgs {
    DWORD       mAttribs;

    char        *pszTemplates;
    char        *pszInputLBO;
    char        *pszInputCLS;
    char        *pszInputPath;
    char        *pszOutputPath;
    char        *pszOutputFile; /* output into one file! */
}   CommandArgs;

typedef enum CommandOptions {
    Command_r_Overwrite         = 0x0001,
    Command_l_ListTemplates     = 0x0002,
    Command_r_Redirect          = 0x0004,
    Command_r_Foreground        = 0x0008,
    Command_e_NoElastos         = 0x0010,
    Command_k_InKernel          = 0x0020,
    Command_k_InSDK             = 0x0040,
    Command_z_UseCustomClassObject     = 0x0080,
    Command_w_HaveSandwichMethod       = 0x0100,
    Command_n_UseNakedMode             = 0x0200,
    Command_u_WeakRef           = 0x0400,
}   CommandOptions;

extern int ParseArgs(int, char **, CommandArgs *);
extern void ListAllTemplates(PLUBEHEADER);

#endif // ___LUBE_H__
