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

#include <lube.h>
#include <string.h>
#include <ctype.h>

static const char * s_elatypes[] = {
    "ECode",    "PVoid",
    "Int8",     "UInt8",    "Byte",     "PInt8",    "PByte",    "PUInt8",
    "Char8",    "Char16",   "Char32",   "PChar8",   "PChar16",  "PChar32",
    "Int16",    "UInt16",   "PInt16",   "PUInt16",
    "Int32",    "UInt32",   "PInt32",   "PUInt32",
    "Int64",    "UInt64",   "PInt64",   "PUInt64",
    "Float",    "Double",   "PFloat",   "PDouble",
    "Handle32", "Handle64",
    "PHandle32", "PHandle64",
    "Boolean",  "PBoolean",
    "String",

    "CarQuintet",  "ArrayOf",

    "PCarQuintet",

    "MemorySize",
    "Address",

    "EventHandler",

    "EGuid",       "EMuid",         "EIID",        "ClassID",      "InterfaceID",
};

const unsigned int c_elatypes = sizeof(s_elatypes) / sizeof(s_elatypes[0]);

const char * s_replstr = "_ELASTOS ";

static int IsWordBoundary(int c)
{
    return !isalnum(c) && (c != '_');
}

// Search 'substr' in 'instr' and add a prefix to the 'substr'.
// Returns TRUE on success, and the 'outstr' holds the result string.
// Returns FALSE if 'substr' does not appear in 'instr'.
//
static BOOL AddNamespacePrefix(
    /* [in] */ char *instr,
    /* [in] */ const char *substr,
    /* [out] */ char * outstr)
{
    assert(instr != NULL);
    assert(substr != NULL);
    assert(outstr != NULL);

    int nsubstr = strlen(substr);
    if (nsubstr == 0) {
        return FALSE;
    }

    char * p = strstr(instr, substr);
    if (p == NULL) {
        return FALSE;
    }

    int n = strlen(instr);
    char * instrend = instr + n;

    unsigned int neplstr = strlen(s_replstr);

    char * q;
    char * pbuf = outstr;
    pbuf[0] = 0;

    while (p != NULL) {
        if (p > instr) {
            n = p - instr;
            strncat(pbuf, instr, n);
            pbuf += n;
        }

        q = p + nsubstr;

        if (((p != instr && IsWordBoundary(*(p - 1))) || (p == instr))
            && (((q < instrend) && IsWordBoundary(*q)) || (q == instrend))) {
            strcat(pbuf, s_replstr);
            pbuf += neplstr;
        }

        strncat(pbuf, p, nsubstr);
        pbuf += nsubstr;
        instr = p + nsubstr;

        p = strstr(instr, substr);
    }

    if (instr != instrend) {
        strcat(pbuf, instr);
    }

    return TRUE;
}

char * ImplNamespaceType(const char * str)
{
    static char buf1[1024];
    static char buf2[1024];

    char * p = buf1;
    char * q = buf2;

    strcpy(p, str);
    char * ret = p;

    for (unsigned int i = 0; i < c_elatypes; i++) {
        if (AddNamespacePrefix(p, s_elatypes[i], q)) {
            ret = q;
            q = p;
            p = ret;
        }
    }
    return ret;
}
