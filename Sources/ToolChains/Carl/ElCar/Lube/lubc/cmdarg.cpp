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
#include <chgpath.h>

const char *c_pszCopyright = \
    "Copyright (C) 2012 The Elastos Open Source Project\n" \
    "\n" \
    "Licensed under the Apache License, Version 2.0 (the \"License\");\n" \
    "you may not use this file except in compliance with the License.\n" \
    "You may obtain a copy of the License at\n" \
    "\n" \
    "     http://www.apache.org/licenses/LICENSE-2.0\n" \
    "\n" \
    "Unless required by applicable law or agreed to in writing, software\n" \
    "distributed under the License is distributed on an \"AS IS\" BASIS,\n" \
    "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n" \
    "See the License for the specific language governing permissions and\n" \
    "limitations under the License.\n\n";

const char *c_pszUsage = \
    "LUBC [-u] [-I dir-list] [-o[filename]] [-p] Source\n"
    "  -u    List all valid user functions.\n"
    "  -I    Specify directory list for source path.\n"
    "  -o    Generate .lbo format object file.\n"
    "  -p    Preview result using internal sample CLS.\n";

inline BOOL IsCommandSwitch(char c)
{
    return ('-' == c);
}

class ArgsStream
{
public:
    inline void Initialize(int nArgc, char *ppArgv[]);

    inline char PeekChar();

    inline char GetChar();
    inline char *GetWord();

private:
    int m_nArgc;
    char **m_ppArgv;

    int m_x, m_y;
};

void ArgsStream::Initialize(int nArgc, char **ppArgv)
{
    m_nArgc = nArgc;
    m_ppArgv = ppArgv;
    m_x = 1;
    m_y = 0;
}

char ArgsStream::PeekChar()
{
    return (m_x < m_nArgc ? m_ppArgv[m_x][m_y] : 0);
}

char ArgsStream::GetChar()
{
    char ch;

    if (m_x < m_nArgc) {
        ch = m_ppArgv[m_x][m_y];
        if (0 == m_ppArgv[m_x][++m_y]) {
            m_x++;
            m_y = 0;
        }
        return ch;
    }
    return 0;
}

char *ArgsStream::GetWord()
{
    char *pszWord;

    if (m_x < m_nArgc) {
        pszWord = &(m_ppArgv[m_x][m_y]);
        m_x++;
        m_y = 0;
        return pszWord;
    }
    return NULL;
}

enum CommandError
{
    CommandError_NoFile         = -1,
    CommandError_NoPathStr      = -2,
    CommandError_UnknownArg     = -3,
    CommandError_DupSwitch      = -4,
    CommandError_NoSource       = -5,
    CommandError_DupSource      = -6,
    CommandError_OutOfMemory    = -7,
};

void CmdError(CommandError err, char c)
{
    switch (err) {
        case CommandError_NoFile:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Command arg /%c - file is not specified.\n", -err, c);
            break;
        case CommandError_NoPathStr:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Command arg /%c - no search path specified.\n", -err, c);
            break;
        case CommandError_UnknownArg:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Command arg /%c - unknown switch.\n", -err, c);
            break;
        case CommandError_DupSwitch:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Command arg /%c - duplication of switch.\n", -err, c);
            break;
        case CommandError_DupSource:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Source file specified more than once.\n", -err);
            break;
        case CommandError_NoSource:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : .lub source file must be specified.\n", -err);
            break;
        case CommandError_OutOfMemory:
            fprintf(stderr, "[ERROR] lubc (0x2%03x) : Out of memory.\n", -err);
            break;
        default:
            assert(TRUE == FALSE);
            break;
    }
}

void GetFileName(
    ArgsStream *pStream, const char *pszSuffix,
    CommandArgs *pArgs, char **ppFile)
{
    int n;
    char *p, *pszName;

    assert(strlen(pszSuffix) == 4);

    pszName = pStream->GetWord();
    if (!pszName) return;

    n = strlen(pszName) - 4;

    if (n > 0) {
        if (!_stricmp(pszName + n, pszSuffix)) {
            *ppFile = pszName;
            return;
        }
        else if (!_stricmp(pszName + n, ".lub")) {
            pArgs->pszSource = pszName;
            return;
        }
    }

    p = new char[n + 4 + 4];    // that's a leak
    if (!p) return;

    strcpy(p, pszName);
    strcat(p, pszSuffix);
    *ppFile = p;
}

char *GenFileName(const char *pszSource, const char *pszSuffix)
{
    int n;
    char *pszName;

    n = strlen(pszSource) - 4;

    if (_stricmp(pszSource + n, ".lub")) {
        pszName = new char[n + 4 + 4 + 1]; // leak
        if (!pszName) return NULL;
        strcpy(pszName, pszSource);
        strcat(pszName, pszSuffix);
    }
    else {
        pszName = new char[n + 4 + 1]; // leak
        if (!pszName) return NULL;
        strcpy(pszName, pszSource);
        strcpy(pszName + n, pszSuffix);
    }
    return pszName;
}

int ParseArgs(int nArgc, char *ppArgv[], CommandArgs *pArgs)
{
    char c;
    ArgsStream stream;

    memset(pArgs, 0, sizeof(CommandArgs));
    stream.Initialize(nArgc, ppArgv);
//    GetCygpath();

    while ((c = stream.PeekChar()) != 0) {
        if (IsCommandSwitch(c)) {
            stream.GetChar(); // skip switch char: '-'

            switch (c = stream.GetChar()) {
                case 'o':
                    if (pArgs->mAttribs & Command_o_GenLBO) {
                        CmdError(CommandError_DupSwitch, c);
                        return CommandError_DupSwitch;
                    }
                    pArgs->mAttribs |= Command_o_GenLBO;

                    if (!IsCommandSwitch(stream.PeekChar())) {
                        GetFileName(&stream, ".lbo", pArgs, &pArgs->pszLBO);
                    }
                    break;

                case 'u':
                    pArgs->mAttribs |= Command_u_ListUserFuncs;
                    break;

                case 'p':
                    pArgs->mAttribs |= Command_p_Preview;
                    break;

                case 'I':
//                    if(HasCygpath()){
//                        pArgs->pszSourcePath = Turn2WinPath(stream.GetWord());
//                    } else {
                        pArgs->pszSourcePath = stream.GetWord();
//                    }
                    if (!pArgs->pszSourcePath) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
                    break;

                case '?':
                    fputs(c_pszCopyright, stdout);
                    fputs(c_pszUsage, stdout);
                    exit(0);

                default:
                    CmdError(CommandError_UnknownArg, c);
                    return CommandError_UnknownArg;
            }
        }
        else {
            if (pArgs->pszSource) {
                CmdError(CommandError_DupSource, c);
                return CommandError_DupSource;
            }
//            if(HasCygpath()){
//                pArgs->pszSource = Turn2WinPath(stream.GetWord());
//            } else {
                pArgs->pszSource = stream.GetWord();
//            }
        }
    }

    if (pArgs->mAttribs & Command_u_ListUserFuncs) {
        ListAllUserFuncs();
        if (!pArgs->pszSource) exit(0);
    }

    if (!pArgs->pszSource) {
        CmdError(CommandError_NoSource, c);
        return CommandError_NoSource;
    }
    if ((pArgs->mAttribs & Command_o_GenLBO) && NULL == pArgs->pszLBO) {
        pArgs->pszLBO = GenFileName(pArgs->pszSource, ".lbo");
        if (NULL == pArgs->pszLBO) {
            CmdError(CommandError_OutOfMemory, 0);
            return CommandError_OutOfMemory;
        }
    }

    return 0;
}

const char *UserFuncArgString(DWORD dwArg)
{
    static char s_szArgString[64];

    ObjectType object;
    MemberType member;
    const char *p;

    if ((DWORD)-1 == dwArg) return "string";

    object = (ObjectType)(dwArg >> 16);
    member = (MemberType)(dwArg & 0xffff);
    p = "";

    switch (object) {
        case Object_None:
            p = "";
            break;
        case Object_Module:
            p = "module";
            break;
        case Object_Class:
        case Object_Super:
            p = "class";
            break;
        case Object_Interface:
        case Object_Parent:
        case Object_IntfParentParent:
        case Object_Child:
            p = "interface";
            break;
        case Object_Struct:
            p = "struct";
            break;
        case Object_Enum:
            p = "enum";
            break;
        case Object_Typedef:
            p = "typedef";
            break;
        case Object_ClassMethod:
        case Object_IntfMethod:
        case Object_CParentMtd:
        case Object_IParentMtd:
        case Object_CbCoalescence:
            p = "method";
            break;
        case Object_Param:
            p = "parameter";
            break;
        case Object_StcMember:
            p = "struct.mem";
            break;
        case Object_EnumMember:
            p = "enum.mem";
            break;
        case Object_ClsIntf:
        case Object_ClsIntfAndParents:
            p = "c.interface";
            break;
        case Object_Aspect:
            p = "aspect";
            break;
        case Object_Aggregate:
            p = "aggregate";
            break;
        case Object_ClassForAspect:
            p = "classforaspect";
            break;
        case Object_Library:
            p = "library";
            break;
        case Object_Const:
            p = "const";
            break;
        case Object_LubeOption:
            p = "lubeoption";
            break;
        case Object_InuptType:
            p = "inputtype";
            break;
        case Object_CompilingEnv:
            p = "compilingenv";
            break;
        case Object_ClassConstructor:
            p = "constructor";
            break;
        case Object_IntfConst:
            p = "interfaceconst";
            break;
    }
    strcpy(s_szArgString, p);
    if (Member_None == member) return s_szArgString;

    if (Object_None != object) strcat(s_szArgString, ".");
    switch (member) {
        case Member_Type:
            p = "type";
            break;
        case Member_Name:
            p = "name";
            break;
        case Member_FullName:
            p = "fullname";
            break;
        case Member_Attrib:
            p = "attrib";
            break;
        case Member_Version:
            p = "version";
            break;
        case Member_Major_Version:
            p = "majorversion";
            break;
        case Member_Minor_Version:
            p = "minorversion";
            break;
        case Member_Uuid:
            p = "uuid";
            break;
        case Member_Uunm:
            p = "uunm";
            break;
        case Member_CarCode:
            p = "carcode";
            break;
        case Member_BuildDate:
            p = "builddate";
            break;
        case Member_Clsid:
            p = "clsid";
            break;
        case Member_Iid:
            p = "iid";
            break;
        case Member_Value:
            p = "value";
            break;
        case Member_Dimention:
            p = "dimention";
            break;
        case Member_NameSpace:
            p = "namespace";
            break;
        case Member_None:
            assert(TRUE == FALSE);
            break;
    }
    strcat(s_szArgString, p);

    return s_szArgString;
}

void ListAllUserFuncs()
{
    printf("%-4s%-18s%-18s%s\n", "NO", "NAME", "ARG TYPE", "DESCRIPTION");
    puts("---------------------------------------------------");
    for (int n = 0; n < c_cUserFuncs; n++) {
        printf("%-4d%-18s%-18s%s\n",
            n,
            g_userFuncs[n].mName,
            UserFuncArgString(g_userFuncs[n].dwArgType),
            g_userFuncs[n].pszDesc);
    }
}
