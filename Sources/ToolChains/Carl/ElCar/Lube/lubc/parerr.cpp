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

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "token.h"
#include "parerr.h"

int g_nErrorNumber = 0;
int g_nWarningNumber = 0;

typedef struct ErrorMessage {
    LubeErrorNo      err;
    const char *    pszMessage;
} ErrorMessage;

static ErrorMessage s_errorMessages[] = {
    { Lube_E_UnexpectEOF, "Unexpected end of file." },
    { Lube_E_UnexpectSymbol, "Unexpected symbol '%s'." },
    { Lube_E_SymbolTooLong, "Symbol is too long." },
    { Lube_E_BufOverflow, "Source is too big to fit in memory." },
    { Lube_E_ExpectSymbol, "Symbol '%s' may be missing." },
    { Lube_E_OutOfMemory, "Compilation out of memory." },
    { Lube_E_NotFound, "Undefined %s '%s'." },
    { Lube_E_UndefinedSymbol, "Undefined symbol '%s'." },
    { Lube_E_OpenFile, "Can't open file '%s'." },
    { Lube_E_InvalidInput, "Invalid input object '%s'." },
    { Lube_E_InvalidWithObj, "'%s' is not a valid object in with statement." },
    { Lube_E_NoValidObject, "No valid %s's object existed here." },
    { Lube_E_IllegalAll, "Illegal using of 'all %s'." },
    { Lube_E_InvalidMember, "Invalid member '%s'." },
    { Lube_E_UndefFunc, "Undefined function '%s'." },
    { Lube_E_InvalidFuncArg, "The parameter type can't match "
        "function's prototype." },
    { Lube_E_MultiElse, "Multi-else." },
    { Lube_E_HasNoOrder, "The variable has no order." },
    { Lube_E_InvalidCondMem, "Do not support that attrib in condition." },
    { Lube_E_HasNoAttrib, "The '%s' has no attrib '%s'." },
    { Lube_E_HasNoType, "The '%s' has no type '%s'." },
    { Lube_E_InvalidEmbed, "Invalid embeded member in string." },
};

const int c_nErrorNumber = \
    sizeof(s_errorMessages) / sizeof(ErrorMessage);

static char s_szSourceFile[c_nMaxTokenSize] = "";
static const char *s_pszPrefix = NULL;
static const char *s_pszSuffix = NULL;

void ErrorReporterInit(const char *pszFile)
{
    strcpy(s_szSourceFile, pszFile);
}

void ErrorStringFix(const char *pszPrefix, const char *pszSuffix)
{
    s_pszPrefix = pszPrefix;
    s_pszSuffix = pszSuffix;
}

void *SaveErrorContext()
{
    char *pszSource;

    pszSource = new char[strlen(s_szSourceFile) + 1];
    if (!pszSource) return NULL;
    strcpy(pszSource, s_szSourceFile);

    return (void *)pszSource;
}

void RestoreErrorContext(void *pv)
{
    if (pv) {
        strcpy(s_szSourceFile, (char *)pv);
        delete [] (char *)pv;
    }
}

void ErrorReport(LubeErrorNo err, ...)
{
    int n;
    va_list vl;

    for (n = 0; n < c_nErrorNumber; n++) {
        if (err == s_errorMessages[n].err) {
            if (err > Lube_ErrorStart) {
                fprintf(stderr, "[ERROR] lubc (0x%04x)", err);
                g_nErrorNumber++;
            }
            else {
                fprintf(stderr, "[WARN] lubc (0x%04x)", err);
                g_nWarningNumber++;
            }

            if (s_szSourceFile[0]) {
                fprintf(stderr, " %s %d",
                        s_szSourceFile, g_nLineNumber);
            }
            fprintf(stderr, " : ");

            if (s_pszPrefix) fputs(s_pszPrefix, stderr);
            va_start(vl, err);
            vfprintf(stderr, s_errorMessages[n].pszMessage, vl);
            va_end(vl);

            if (s_pszSuffix) fputs(s_pszSuffix, stderr);
            fputs("\n", stderr);
            return;
        }
    }
    assert(true == false);
}
