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

#include <assert.h>
#include <stdlib.h>

#include "carc.h"
//#include <chgpath.h>


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
    "Usage: CARC [-INPUT-] [-OUTPUT-] [-MISCELLANEOUS-] <*.car>\n" \
    "\n" \
    "                        -CAR COMPILER SWITCHES-\n" \
    "\n" \
    "                                -INPUT-\n" \
    "-F url              Specify URL of manufactory\n" \
    "-L dir-list         Specify directory list for library path\n" \
    "-I dir-list         Specify directory list for source path\n" \
    "-A                  The compiler will ignore all thread model keyword and use naked model\n" \
    "\n" \
    "                               -OUTPUT-\n" \
    "-c [filename]       Generate .cls format object file\n" \
    "-E [filename]       Generate extended .cls format object file\n" \
    "-a                  Output .cls file in compress format\n" \
    "-r filename         Compiling result revert to CAR source file\n" \
    "\n" \
    "                            -MISCELLANEOUS-\n" \
    "-i                  Lenient naming, don't check the first charactor of class name\n" \
    "-t                  Treat warnings as errors\n" \
    "-w                  Suppress compiler warning messages\n" \
    "-s                  Do not using system type library\n" \
    "-e                  Do not load elastos.dll by default\n" \
    "-k                  Specify this option when compiling in kernel\n" \
    "-d                  Create the dependencies when the current .car file merge .cls or .car\n" \
    "-u                  Support weak reference\n" \
    "-n                  Specify this option when using naked mode\n"
    "-?                  Display this list of CAR compiler switches\n";

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
    CommandError_NoUrlStr       = -8,
};

void CmdError(CommandError err, char c)
{
    switch (err) {
        case CommandError_NoFile:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Command arg /%c - file is not specified.\n", -err, c);
            break;
        case CommandError_NoPathStr:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Command arg /%c - no search path specified.\n", -err, c);
            break;
        case CommandError_UnknownArg:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Command arg /%c - unknown switch.\n", -err, c);
            break;
        case CommandError_DupSwitch:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Command arg /%c - duplication of switch.\n", -err, c);
            break;
        case CommandError_DupSource:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Source file specified more than once.\n", -err);
            break;
        case CommandError_NoSource:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : .car source file must be specified.\n", -err);
            break;
        case CommandError_OutOfMemory:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Out of memory.\n", -err);
            break;
        case CommandError_NoUrlStr:
            fprintf(stderr, "[ERROR] carc (0x2%03x) : Command arg /%c - no URL string specified.\n", -err, c);
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
        if (!strcasecmp(pszName + n, pszSuffix)) {
            *ppFile = pszName;
            return;
        }
        else if (!strcasecmp(pszName + n, ".car")) {
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

    if (strcasecmp(pszSource + n, ".car")) {
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
                case 'c':
                    if (pArgs->mAttribs & Command_c_GenCLS) {
                        CmdError(CommandError_DupSwitch, c);
                        return CommandError_DupSwitch;
                    }
                    pArgs->mAttribs |= Command_c_GenCLS;

                    if (!IsCommandSwitch(stream.PeekChar()))
                        GetFileName(&stream, ".cls", pArgs, &pArgs->pszCLS);
                    break;
                case 'E':
                    if (pArgs->mAttribs & Command_E_GenExCLS) {
                        CmdError(CommandError_DupSwitch, c);
                        return CommandError_DupSwitch;
                    }
                    pArgs->mAttribs |= Command_E_GenExCLS;

                    if (!IsCommandSwitch(stream.PeekChar()))
                        GetFileName(&stream, ".cls", pArgs, &pArgs->pszExCLS);
                    break;

                case 'r':
                    if (pArgs->mAttribs & Command_r_GenCAR) {
                        CmdError(CommandError_DupSwitch, c);
                        return CommandError_DupSwitch;
                    }
                    pArgs->mAttribs |= Command_r_GenCAR;

                    pArgs->pszCAR = stream.GetWord();
                    if (!pArgs->pszCAR) {
                        CmdError(CommandError_NoFile, c);
                        return CommandError_NoFile;
                    }
                    break;

                case 'a':
                    pArgs->mAttribs |= Command_a_Compress;
                    break;

                case 't':
                    pArgs->mAttribs |= Command_t_Warn2Err;
                    break;

                case 'w':
                    pArgs->mAttribs |= Command_w_SuppWarn;
                    break;

                case 's':
                    pArgs->mAttribs |= Command_s_NoSys;
                    break;

                case 'e':
                    pArgs->mAttribs |= Command_e_NoElastos;
                    break;

                case 'd':
                    pArgs->mAttribs |= Command_d_Depend;
                    break;

                case 'k':
                    pArgs->mAttribs |= Command_k_InKernel;
                    break;

                case 'L':
//                    if(HasCygpath()){
//                        pArgs->pszLibraryPath = Turn2WinPath(stream.GetWord());
//                    } else {
                        pArgs->pszLibraryPath = stream.GetWord();
//                    }
                    if (!pArgs->pszLibraryPath) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
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

                case 'F':
                    pArgs->pszFactoryUrl = stream.GetWord();
                    if (!pArgs->pszFactoryUrl) {
                        CmdError(CommandError_NoUrlStr, c);
                        return CommandError_NoUrlStr;
                    }
                    break;

                case 'A':
                    pArgs->mAttribs |= Command_A_FreeModel;

                    break;

                case 'i':
                    pArgs->mAttribs |= Command_i_IgrName;

                    break;

                case 'u':
                    pArgs->mAttribs |= Command_u_WeakRef;

                    break;

                case 'n':
                    pArgs->mAttribs |= Command_n_NakedMode;

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

    if (!pArgs->pszSource) {
        CmdError(CommandError_NoSource, c);
        return CommandError_NoSource;
    }
    if ((pArgs->mAttribs & Command_c_GenCLS) && NULL == pArgs->pszCLS) {
        pArgs->pszCLS = GenFileName(pArgs->pszSource, ".cls");
        if (NULL == pArgs->pszCLS) {
            CmdError(CommandError_OutOfMemory, 0);
            return CommandError_OutOfMemory;
        }
    }

    return 0;
}
