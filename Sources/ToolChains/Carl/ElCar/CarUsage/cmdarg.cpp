//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "carusage.h"

const char *c_pszCopyright = \
    "CarUsage Version 1.0, Revision 1\n" \
    "Copyright (c) 2000-2007, Elastos, Inc. All rights reserved.\n\n";

const char *c_pszUsage = \
    "usage: CARUSAGE [-OPTION-] <*.dll | *.cls>\n" \
    "\n" \
    "                              -OPTION-\n" \
    "-i/-interface       list interfaces\n" \
    "-c/-class           list classes\n" \
    "-b/-baseclass       list base-classes\n" \
    "-g/-generic         list generic-classes\n" \
    "-a/-aspect          list aspects\n" \
    "-r/-regime          list regimes\n" \
    "-e/-enum            list enumerations\n" \
    "-n/-const           list constants\n" \
    "-s/-struct          list structures\n" \
    "-m/-method          list methods, same as \"-i -c -g -a -t\"\n" \
    "-h/-callback        list callback-handlers\n" \
    "-all                list all of the above\n" \
    "-o/-output file     designate destination file\n" \
    "-f                  force-override existing file\n" \
    "-?/-help            print this usage prompt\n" \
    "\n" \
    "If option -a/-all is given, -i/-c/-b/-g/-a/-k/-h/-e/-c/-s/-m will be ignored.\n" \
    "If option -m/-method is given, -i/-c/-b/-g/-a/-k/-h/-e/-c/-s will be ignored.\n";

inline BOOL IsCommandSwitch(char c)
{
    return ('-' == c);
}

inline BOOL IsBlankChar(char c)
{
    return (' ' == c || '\r' == c || '\n' == c || '\f' == c || '\t' == c);
}

class ArgsStream
{
public:
    inline void Initialize(int nArgc, char *ppArgv[]);

    inline char PeekChar();
    inline char PeekCharAt(int i);

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

char ArgsStream::PeekCharAt(int i)
{
    if (strlen(m_ppArgv[m_x]) - i <= 0) {
        return 0;
    }
    char c = (m_x > 0 && m_x <= m_nArgc ? m_ppArgv[m_x][i] : 0);
    return c;
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
    fputs("Command: ", stderr);

    switch (err) {
        case CommandError_NoFile:
            fprintf(stderr, "arg /%c - file is not specified.", c);
            break;
        case CommandError_NoPathStr:
            fprintf(stderr, "arg /%c - no path specified.", c);
            break;
        case CommandError_UnknownArg:
            fprintf(stderr, "arg /%c - unknown switch.", c);
            break;
        case CommandError_DupSwitch:
            fprintf(stderr, "arg /%c - duplication of switch.", c);
            break;
        case CommandError_DupSource:
            fputs("template name specified more than once.", stderr);
            break;
        case CommandError_NoSource:
            fputs("template and CLS file must be specified.", stderr);
            break;
        case CommandError_OutOfMemory:
            fputs("Out of memory.", stderr);
            break;
        default:
            assert(TRUE == FALSE);
            break;
    }

    fputs("\n", stderr);
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
    BOOL bCharSkipped;

    memset(pArgs, 0, sizeof(CommandArgs));
    stream.Initialize(nArgc, ppArgv);

    if (stream.PeekChar() == 0) {
        fputs(c_pszCopyright, stdout);
        fputs(c_pszUsage, stdout);
        exit(0);
    }

    while ((c = stream.PeekChar()) != 0) {
        if (IsCommandSwitch(c)) {
            stream.GetChar(); // skip switch char: '-'

            bCharSkipped = FALSE;
            c = stream.PeekChar();
            if (IsCommandSwitch(c)) { // another '-'
                stream.GetChar(); // skip switch char: '-'
                c = stream.PeekChar();
            }
            switch (c) {
                case 'o': // output
                    stream.GetWord(); // skip char "c" or the rest of "output"
                    bCharSkipped = TRUE;
                    pArgs->pszOutputFile = stream.GetWord();
                    if (!pArgs->pszOutputFile) {
                        CmdError(CommandError_NoPathStr, c);
                        return CommandError_NoPathStr;
                    }
                    break;

                case 'i': // interface
                    pArgs->mAttribs |= Command_r_Interface;
                    break;

                case 'c':
                    if (stream.PeekCharAt(2) == 'a') { // callback
                        pArgs->mAttribs |= Command_r_Callback;
                    }
                    else if (stream.PeekCharAt(2) == 'o') { // const
                        pArgs->mAttribs |= Command_r_Const;
                    }
                    else {
                        pArgs->mAttribs |= Command_r_Class;
                    }
                    break;

                case 'b':
                    pArgs->mAttribs |= Command_r_BaseClass;
                    break;

                case 'a':
                    if (stream.PeekCharAt(2) == 'l') { // all
                        pArgs->mAttribs |= Command_r_All;
                    }
                    else { // aspect
                        pArgs->mAttribs |= Command_r_Aspect;
                    }
                    break;

                case 'g': // generic
                    pArgs->mAttribs |= Command_r_Generic;
                    break;

                case 'h':
                    if (stream.PeekCharAt(2) == 'e') { // help
                        fputs(c_pszCopyright, stdout);
                        fputs(c_pszUsage, stdout);
                        exit(0);
                    }
                    else { // callback handler
                        pArgs->mAttribs |= Command_r_Callback;
                    }
                    break;

                case 'n':
                    pArgs->mAttribs |= Command_r_Const;
                    break;

                case 'e': // enum
                    pArgs->mAttribs |= Command_r_Enum;
                    break;

                case 'r': // regime
                    pArgs->mAttribs |= Command_r_Regime;
                    break;

                case 's': // struct
                    pArgs->mAttribs |= Command_r_Struct;
                    break;

                case 'm': // method
                    pArgs->mAttribs |= Command_r_Method;
                    break;

                case 'f':
                    pArgs->mAttribs |= Command_r_ForceOverride;
                    break;

                case '?':
                    fputs(c_pszCopyright, stdout);
                    fputs(c_pszUsage, stdout);
                    exit(0);

                default:
                    CmdError(CommandError_UnknownArg, c);
                    return CommandError_UnknownArg;
            }
            if (!bCharSkipped) {
                stream.GetWord(); //skip the char and the rest
            }
        }
        else {
            pArgs->pszInputFile = stream.GetWord();
        }
    }

    if (!pArgs->pszInputFile) {
        CmdError(CommandError_NoSource, c);
        return CommandError_NoSource;
    }
    return 0;
}
