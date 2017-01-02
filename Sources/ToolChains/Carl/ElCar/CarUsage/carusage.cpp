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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <windows.h>

#include <carusage.h>

#define APPEND_TEMPLATE(opt, tmplt) \
        if (args.mAttribs & Command_r_ ##opt) {\
            strcat(szBuf, tmplt);\
            bExisted = TRUE;\
        }\

int main(int argc, char *argv[])
{
    CommandArgs args;

    if (ParseArgs(argc, argv, &args) < 0) exit(1);

    char szBuf[_MAX_PATH];
    BOOL bToAppendDLL = TRUE;
    if (strchr(args.pszInputFile, '.') != NULL) {
        bToAppendDLL = FALSE;
    }
    if (bToAppendDLL) {
        sprintf(szBuf, "lube -C \"%s.dll\"", args.pszInputFile);
    }
    else {
        sprintf(szBuf, "lube -C \"%s\"", args.pszInputFile);
    }
    if (args.pszOutputFile) {
        strcat(szBuf, " -o ");
        strcat(szBuf, args.pszOutputFile);
    }
    else {
        strcat(szBuf, " -v");
    }
    if (args.mAttribs & Command_r_ForceOverride) {
        strcat(szBuf, " -f");
    }
    if (args.mAttribs & Command_r_All) {
        strcat(szBuf, " -T carusage");
    }
    else if (args.mAttribs & Command_r_Method) {
        strcat(szBuf, " -T methodusage");
    }
    else {
        BOOL bExisted = FALSE; // template existed?
        APPEND_TEMPLATE(Const, " -T constusage")
        APPEND_TEMPLATE(Struct, " -T structusage")
        APPEND_TEMPLATE(Enum, " -T enumusage")
        APPEND_TEMPLATE(Aspect, " -T aspectusage")
        APPEND_TEMPLATE(Regime, " -T regimeusage")
        APPEND_TEMPLATE(Class, " -T classusage")
        APPEND_TEMPLATE(Generic, " -T genericusage")
        APPEND_TEMPLATE(Interface, " -T interfaceusage")
        if (!bExisted) {
            strcat(szBuf, " -T carusage");
        }
    }
    if (0 != system(szBuf)) return USAGE_FAIL;
    return 0;
}
