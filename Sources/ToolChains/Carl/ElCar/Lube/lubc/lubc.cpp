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

#include <stdint.h>
#include <stdlib.h>
#include "lubc.h"


int main(int argc, char **argv)
{
    char buffer[c_nMaxTextSize];
    ParserContext context(buffer);
    CommandArgs args;

    if (ParseArgs(argc, argv, &args) < 0) return LUBE_FAIL;

    InitParser(&context, args.pszSourcePath);
    DoCompiling(args.pszSource);

    if (0 != g_nErrorNumber) {
        fprintf(stderr, "[INFO] lubc (0x1001) : Aborting compilation.\n");
        exit(g_nErrorNumber);
    }

    if (args.mAttribs & Command_p_Preview) {
        PreviewResult(context.m_pLube);
    }
    if (args.mAttribs & Command_o_GenLBO) {
        SaveLube(context.m_pLube, args.pszLBO);
    }
    return LUBE_OK;
}

void PreviewResult(PLUBEHEADER pLube)
{
    CLSModule *pModule;

    if (LoadCLSFromDll(NULL, &pModule) >= 0) {
        LubeContext lubeCtx(pLube, pModule, LubeOpt_Redirect);
        lubeCtx.ExecTemplate(0);
    }
    else {
        fprintf(stderr, "[ERROR] lubc (0x1002) : Can't load sample CLS resource.\n");
        fprintf(stderr, "[ERROR] lubc (0x1003) : %s\n", CLSLastErrorMessage());
    }
}
