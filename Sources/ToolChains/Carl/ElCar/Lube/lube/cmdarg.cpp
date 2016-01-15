//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdint.h>
#include <stdlib.h>
#include "_lube.h"
#include <chgpath.h>

const char *c_pszCopyright = \
    "Lube Version 1.0, Revision 11\n" \
    "Copyright (c) 2000-2005, Elastos, Inc. All rights reserved.\n\n";

const char *c_pszUsage = \
    "                          -LUBE SWITCHES-\n" \
    "\n" \
    "                                  -INPUT-\n" \
    "-C file [-C file [...]]     Specify input .cls format file list\n" \
    "-T name [-T name [...]]     Specify name list of input templates\n" \
    "-I dir  [-I dir  [...]]     Specify directory list for input path\n" \
    "-B file                     Specify input .lbo format file\n" \
    "\n" \
    "                                 -OUTPUT-\n" \
    "-f                          Force-override existing destination file\n" \
    "-w                          Specify this option when needing sandwich methods.\n"
    "-p path                     Specify output path of template result\n" \
    "-k                          Specify this option when compiling in kernel.\n" \
    "-s                          Specify this option when compiling in SDK.\n" \
    "-z                          Specify this option when using custom class objects.\n"\
    "-n                          Specify this option when using naked mode.\n" \
    "-u                          Specify this option when supporting weak reference.\n" \
    "\n" \
    "                              -MISCELLANEOUS-\n" \
    "-l                          List all valid lube templates\n" \
    "-e                          Do not load elastos.dll by default\n" \
    "-?                          Display this list of lube switches\n";

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
    CommandError_NoFile             = -1,
    CommandError_NoTemplate         = -2,
    CommandError_NoPathStr          = -3,
    CommandError_UnknownArg         = -4,
    CommandError_InvalidSemicolon   = -5,
    CommandError_NoSource           = -6,
    CommandError_DupSource          = -7,
    CommandError_OutOfMemory        = -8,
};

void CmdError(CommandError err, char c)
{
    switch (err) {
        case CommandError_NoFile:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Command arg /%c - file is not specified.\n", -err, c);
            break;
        case CommandError_NoTemplate:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Command arg /%c - template is not specified.\n", -err, c);
            break;
        case CommandError_NoPathStr:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Command arg /%c - no path specified.\n", -err, c);
            break;
        case CommandError_UnknownArg:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Command arg /%c - unknown switch.\n", -err, c);
            break;
        case CommandError_InvalidSemicolon:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Command arg /%c - contains character of \";\".\n", -err, c);
            break;
        case CommandError_DupSource:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Template name specified more than once.\n", -err);
            break;
        case CommandError_NoSource:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Template and CLS file must be specified.\n", -err);
            break;
        case CommandError_OutOfMemory:
            fprintf(stderr, "[ERROR] lube (0x2%03x) : Out of memory.\n", -err);
            break;
        default:
            assert(TRUE == FALSE);
            break;
    }
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

bool ContainsSemicolon(char* pszStr)
{
    int nLength = strlen(pszStr);
    for (int i = 0; i < nLength; i++) {
        if (pszStr[i] == ';') {
            return true;
        }
    }
    return false;
}

void ConcatCommasStr(char** pszBuf, char* pszStr)
{
    if (pszStr == NULL) {
        return;
    }
    if (*pszBuf != NULL) {
        int nLength = strlen(*pszBuf) + 1 + strlen(pszStr) + 1;
        char* pszNew = new char[nLength];
        pszNew[0] = '\0';
        strcat(pszNew, *pszBuf);
        strcat(pszNew, ";");
        strcat(pszNew, pszStr);
        if (ContainsSemicolon(*pszBuf)) {
            delete *pszBuf; // still leak the last string
        }
        *pszBuf = pszNew;
    }
    else {
        *pszBuf = pszStr;
    }
}

int ParseArgs(int nArgc, char *ppArgv[], CommandArgs *pArgs)
{
    char c;
    ArgsStream stream;
    char *pszStr;

    memset(pArgs, 0, sizeof(CommandArgs));
    stream.Initialize(nArgc, ppArgv);
#ifdef _win32
    GetCygpath();
#endif
    while ((c = stream.PeekChar()) != 0) {
        if (IsCommandSwitch(c)) {
            stream.GetChar(); // skip switch char: '-'

            switch (c = stream.GetChar()) {
                case 'B':
#ifdef _win32
                    if(HasCygpath()){
                        pArgs->pszInputLBO = Turn2WinPath(stream.GetWord());
                    } else {
#endif
                        pArgs->pszInputLBO = stream.GetWord();
#ifdef _win32
                    }
#endif

                    if (!pArgs->pszInputLBO) {
                        CmdError(CommandError_NoFile, c);
                        return CommandError_NoFile;
                    }
                    break;

                case 'C':
                    pszStr = stream.GetWord();
                    if (!pszStr) {
                        CmdError(CommandError_NoFile, c);
                        return CommandError_NoFile;
                    }
                    if (ContainsSemicolon(pszStr)) {
                        CmdError(CommandError_InvalidSemicolon, c);
                        return CommandError_InvalidSemicolon;
                    }
                    ConcatCommasStr(&(pArgs->pszInputCLS), pszStr);
                    break;

                case 'e':
                    pArgs->mAttribs |= Command_e_NoElastos;
                    break;
                case 'k':
                    pArgs->mAttribs |= Command_k_InKernel;
                    break;
                case 's':
                    pArgs->mAttribs |= Command_k_InSDK;
                    break;

                case 'T':
                    pszStr = stream.GetWord();
                    if (strcmp(pszStr, "foreground") == 0) {
                        pArgs->mAttribs |= Command_r_Foreground;
                    }
                    if (!pszStr) {
                        CmdError(CommandError_NoTemplate, c);
                        return CommandError_NoTemplate;
                    }
                    if (ContainsSemicolon(pszStr)) {
                        CmdError(CommandError_InvalidSemicolon, c);
                        return CommandError_InvalidSemicolon;
                    }
                    ConcatCommasStr(&(pArgs->pszTemplates), pszStr);
                    break;

                case 'I':
                    pszStr = stream.GetWord();
                    if (!pszStr) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
                    if (ContainsSemicolon(pszStr)) {
                        CmdError(CommandError_InvalidSemicolon, c);
                        return CommandError_InvalidSemicolon;
                    }
#ifdef _win32
                    if(HasCygpath()){
                        pszStr = Turn2WinPath(pszStr);
                    }
#endif
                    ConcatCommasStr(&(pArgs->pszInputPath), pszStr);
#ifdef _win32
                    if(HasCygpath()){
                        delete pszStr;
                    }
#endif
                    break;

                case 'p':
#ifdef _win32
                    if(HasCygpath()){
                        pArgs->pszOutputPath = Turn2WinPath(stream.GetWord());
                    } else {
#endif
                        pArgs->pszOutputPath = stream.GetWord();
#ifdef _win32
                    }
#endif
                    if (!pArgs->pszOutputPath) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
                    break;

                case 'o':
                    pArgs->pszOutputFile = stream.GetWord();
                    if (!pArgs->pszOutputFile) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
                    break;

                case 'l':
                    pArgs->mAttribs |= Command_l_ListTemplates;
                    break;

                case 'f':
                    pArgs->mAttribs |= Command_r_Overwrite;
                    break;

                case 'v':
                    pArgs->mAttribs |= Command_r_Redirect;
                    break;

                case 'z':
                    pArgs->mAttribs |= Command_z_UseCustomClassObject;
                    break;

                case 'w':
                    pArgs->mAttribs |= Command_w_HaveSandwichMethod;
                    break;

                case 'n':
                    pArgs->mAttribs |= Command_n_UseNakedMode;
                    break;

                case 'u':
                    pArgs->mAttribs |= Command_u_WeakRef;
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
            CmdError(CommandError_UnknownArg, c);
            return CommandError_UnknownArg;
        }
    }

    if (!(pArgs->mAttribs & Command_l_ListTemplates)
        && (!pArgs->pszTemplates || !pArgs->pszInputCLS)) {
        CmdError(CommandError_NoSource, c);
        return CommandError_NoSource;
    }
    return 0;
}
