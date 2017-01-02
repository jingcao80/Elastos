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

#ifndef __LUBC_H__
#define __LUBC_H__

#include "parser.h"

typedef struct CommandArgs {
    DWORD       mAttribs;

    char        *pszSource;
    char        *pszLBO;
    char        *pszSourcePath;
}   CommandArgs;

typedef enum CommandOption {
    Command_o_GenLBO        = 0x0001,
    Command_p_Preview       = 0x0002,
    Command_I_SrcPath       = 0x0004,
    Command_u_ListUserFuncs = 0x0008,
}   CommandOption;

extern int ParseArgs(int, char **, CommandArgs *);
extern void PreviewResult(LubeHeader *);
extern void GenerateLBO(const char *, LubeHeader *);
extern void ListAllUserFuncs();

#endif // __LUBC_H__
