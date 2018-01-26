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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "lubedef.h"
#include "token.h"
#include "parerr.h"

#define _stricmp strcasecmp

int g_nLineNumber;
char g_szCurrentToken[c_nMaxTokenSize + 1];
int g_nCurrentTokenLen;

static LubeToken s_peekToken;
static char s_szPeekToken[c_nMaxTokenSize + 1];
static int s_nPeekTokenLen;

static bool s_bIn;
static bool s_bBrace;

void TokenInit()
{
    g_nLineNumber = 1;
    g_nCurrentTokenLen = 0;
    s_peekToken = Token_nothing;
    s_bIn = false;
    s_bBrace = false;
}

PVOID SaveTokenContext()
{
    DWORD dwContext = g_nLineNumber;

    if (s_bIn) dwContext |= 0x80000000;
    if (s_bBrace) dwContext |= 0x4000000;

    return (PVOID)dwContext;
}

void RestoreTokenContext(PVOID pvCtx)
{
    DWORD dwContext = (DWORD)(uintptr_t)pvCtx;

    if (dwContext & 0x40000000) s_bBrace = true;
    if (dwContext & 0x80000000) s_bIn = true;
    g_nLineNumber = dwContext & 0x00ffffff;

    s_peekToken = Token_nothing;
}

struct KeywordToken
{
    const char  *pszKeyword;
    LubeToken    token;
};
typedef struct KeywordToken KeywordToken;

#define DEFINE_TOKEN(token) { #token, Token_K_##token }

static const KeywordToken s_keywords_a[] = {
    DEFINE_TOKEN(all),
    DEFINE_TOKEN(and),
    DEFINE_TOKEN(attrib),
    DEFINE_TOKEN(aspect),
    DEFINE_TOKEN(aggregate),
    DEFINE_TOKEN(alias),
    DEFINE_TOKEN(aliases),
    DEFINE_TOKEN(attribute),
    DEFINE_TOKEN(aspects),
    DEFINE_TOKEN(aggregates),
    DEFINE_TOKEN(argument),
    DEFINE_TOKEN(arguments),
};

static const KeywordToken s_keywords_b[] = {
    DEFINE_TOKEN(builddate),
};

static const KeywordToken s_keywords_c[] = {
    DEFINE_TOKEN(child),
    DEFINE_TOKEN(class),
    DEFINE_TOKEN(clsid),
    DEFINE_TOKEN(classes),
    DEFINE_TOKEN(const),
    DEFINE_TOKEN(consts),
    DEFINE_TOKEN(classforaspect),
    DEFINE_TOKEN(classesforaspect),
    DEFINE_TOKEN(coalescence),
    DEFINE_TOKEN(coalescences),
    DEFINE_TOKEN(compilingenv),
    DEFINE_TOKEN(constructor),
    DEFINE_TOKEN(constructors),
    DEFINE_TOKEN(carcode),
};

static const KeywordToken s_keywords_d[] = {
    DEFINE_TOKEN(do),
    DEFINE_TOKEN(dimention),
};

static const KeywordToken s_keywords_e[] = {
    DEFINE_TOKEN(end),
    DEFINE_TOKEN(else),
    DEFINE_TOKEN(enum),
    DEFINE_TOKEN(enums),
    DEFINE_TOKEN(ezclsid),
    DEFINE_TOKEN(element),
    DEFINE_TOKEN(elements),
};

static const KeywordToken s_keywords_f[] = {
    DEFINE_TOKEN(first),
    DEFINE_TOKEN(false),
    DEFINE_TOKEN(fullname),
    DEFINE_TOKEN(function),
    DEFINE_TOKEN(functions),
};

static const KeywordToken s_keywords_g[] = {
    DEFINE_TOKEN(guid),
};

static const KeywordToken s_keywords_i[] = {
    DEFINE_TOKEN(if),
    DEFINE_TOKEN(is),
    DEFINE_TOKEN(interface),
    DEFINE_TOKEN(iid),
    DEFINE_TOKEN(input),
    DEFINE_TOKEN(interfaces),
    DEFINE_TOKEN(importlib),
    DEFINE_TOKEN(importlibs),
    DEFINE_TOKEN(inputtype),
    DEFINE_TOKEN(interfaceconst),
    DEFINE_TOKEN(interfaceconsts),
};

static const KeywordToken s_keywords_l[] = {
    DEFINE_TOKEN(last),
    DEFINE_TOKEN(library),
    DEFINE_TOKEN(libraries),
    DEFINE_TOKEN(lubeoption),
};

static const KeywordToken s_keywords_m[] = {
    DEFINE_TOKEN(method),
    DEFINE_TOKEN(member),
    DEFINE_TOKEN(module),
    DEFINE_TOKEN(methods),
    DEFINE_TOKEN(members),
    DEFINE_TOKEN(majorversion),
    DEFINE_TOKEN(minorversion),
};

static const KeywordToken s_keywords_n[] = {
    DEFINE_TOKEN(name),
    DEFINE_TOKEN(namespace),
    DEFINE_TOKEN(not),
};

static const KeywordToken s_keywords_o[] = {
    DEFINE_TOKEN(or),
    DEFINE_TOKEN(output),
};

static const KeywordToken s_keywords_p[] = {
    DEFINE_TOKEN(parent),
    DEFINE_TOKEN(parameter),
    DEFINE_TOKEN(parents),
    DEFINE_TOKEN(parents_and_me),
    DEFINE_TOKEN(parameters),
};

static const KeywordToken s_keywords_s[] = {
    DEFINE_TOKEN(struct),
    DEFINE_TOKEN(structs),
    DEFINE_TOKEN(super),
    DEFINE_TOKEN(supers),
};

static const KeywordToken s_keywords_t[] = {
    DEFINE_TOKEN(type),
    DEFINE_TOKEN(template),
    DEFINE_TOKEN(true),
    DEFINE_TOKEN(typedef),
    DEFINE_TOKEN(typedefs),
};

static const KeywordToken s_keywords_u[] = {
    DEFINE_TOKEN(uuid),
    DEFINE_TOKEN(uunm),
};

static const KeywordToken s_keywords_v[] = {
    DEFINE_TOKEN(value),
    DEFINE_TOKEN(version),
};

static const KeywordToken s_keywords_w[] = {
    DEFINE_TOKEN(with),
};

typedef struct KeywordIndex {
    int nNumber;
    const KeywordToken * pKeywords;
} KeywordIndex;

static const KeywordIndex s_indexTable[26] = {
    { sizeof(s_keywords_a) / sizeof(KeywordToken), s_keywords_a },  // a
    { sizeof(s_keywords_b) / sizeof(KeywordToken), s_keywords_b },  // b
    { sizeof(s_keywords_c) / sizeof(KeywordToken), s_keywords_c },  // c
    { sizeof(s_keywords_d) / sizeof(KeywordToken), s_keywords_d },  // d
    { sizeof(s_keywords_e) / sizeof(KeywordToken), s_keywords_e },  // e
    { sizeof(s_keywords_f) / sizeof(KeywordToken), s_keywords_f },  // f
    { sizeof(s_keywords_g) / sizeof(KeywordToken), s_keywords_g },  // g
    { 0, NULL },                                                    // h
    { sizeof(s_keywords_i) / sizeof(KeywordToken), s_keywords_i },  // i
    { 0, NULL },                                                    // j
    { 0, NULL },                                                    // k
    { sizeof(s_keywords_l) / sizeof(KeywordToken), s_keywords_l },  // l
    { sizeof(s_keywords_m) / sizeof(KeywordToken), s_keywords_m },  // m
    { sizeof(s_keywords_n) / sizeof(KeywordToken), s_keywords_n },  // n
    { sizeof(s_keywords_o) / sizeof(KeywordToken), s_keywords_o },  // o
    { sizeof(s_keywords_p) / sizeof(KeywordToken), s_keywords_p },  // p
    { 0, NULL },                                                    // q
    { 0, NULL },                                                    // r
    { sizeof(s_keywords_s) / sizeof(KeywordToken), s_keywords_s },  // s
    { sizeof(s_keywords_t) / sizeof(KeywordToken), s_keywords_t },  // t
    { sizeof(s_keywords_u) / sizeof(KeywordToken), s_keywords_u },  // u
    { sizeof(s_keywords_v) / sizeof(KeywordToken), s_keywords_v },  // v
    { sizeof(s_keywords_w) / sizeof(KeywordToken), s_keywords_w },  // w
    { 0, NULL },                                                    // x
    { 0, NULL },                                                    // y
    { 0, NULL },                                                    // z
};

inline void CurrentTokenBegin()
{
    g_nCurrentTokenLen = 0;
}

inline void CurrentTokenEnd()
{
    g_szCurrentToken[g_nCurrentTokenLen] = '\0';
}

inline void AddTokenChar(char c)
{
    if (g_nCurrentTokenLen < c_nMaxTokenSize) {
        g_szCurrentToken[g_nCurrentTokenLen] = c;
    }
    g_nCurrentTokenLen++;
}

inline void AddTokenInt(int n)
{
    if (g_nCurrentTokenLen + sizeof(int) < (UINT)c_nMaxTokenSize) {
        *(int *)&(g_szCurrentToken[g_nCurrentTokenLen]) = n;
    }
    g_nCurrentTokenLen += sizeof(int);
}

inline bool IsCurrentTokenOverflow()
{
    return g_nCurrentTokenLen > c_nMaxTokenSize;
}

inline int fgetcEx(FILE *pFile)
{
    int c = fgetc(pFile);

    if (c == '\n') g_nLineNumber++;
    return c;
}

inline int ungetcEx(int c, FILE *pFile)
{
    if ('\n' == c) g_nLineNumber--;
    return ungetc(c, pFile);
}

#define fgetc(pFile)       fgetcEx(pFile)
#define ungetc(c, pFile)   ungetcEx(c, pFile)

inline void SkipCurrentLine(FILE *pFile)
{
    while (!feof(pFile) && fgetc(pFile) != '\n');
}

LubeToken TryGetKeywordToken(const char *s)
{
    const KeywordIndex *pIndex;
    int n;

    if (isalpha(*s)) {
        pIndex = &s_indexTable[tolower(*s) - 'a'];
    }
    else {
        return Token_ident;
    }

    for (n = 0; n < pIndex->nNumber; n++) {
        if (!_stricmp(s, pIndex->pKeywords[n].pszKeyword)) {
            return pIndex->pKeywords[n].token;
        }
    }
    return Token_ident;
}

LubeToken GetIdentifyToken(FILE *pFile)
{
    char c;

    while (!feof(pFile)) {
        c = fgetc(pFile);
        if (isalnum(c) || '_' == c) {
            AddTokenChar(c);
        }
        else {
            ungetc(c, pFile);
            break;
        }
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(Lube_E_SymbolTooLong);
        return Token_error;
    }
    CurrentTokenEnd();

    return TryGetKeywordToken(g_szCurrentToken);
}

inline int GetValidChar(FILE *pFile)
{
    int c;
    do {c = fgetc(pFile);} while (' ' == c || '\t' == c || '\r' == c);
    return c;
}

int GetTokenInString(FILE *pFile)
{
    LubeToken token;
    int c, n = 0;
    char buf[c_nMaxTokenSize];

    while (!feof(pFile)) {
        c = GetValidChar(pFile);
        while (isalnum(c) || '_' == c) {
            buf[n++] = c;
            c = fgetc(pFile);
        }
        if (n > 0) {
            buf[n] = 0;
            token = TryGetKeywordToken(buf);
            if (Token_ident == token) {
                ErrorReport(Lube_E_UnexpectSymbol, buf);
                return LUBE_FAIL;
            }
            AddTokenChar(c_nEmbedPrefix);
            AddTokenInt(token);
            n = 0;
        }

        if ('.' == c) {
            AddTokenChar('.');
        }
        else if ('}' == c) {
            return 0;
        }
        else if (!isspace(c)) {
            break;
        }
    }
    ErrorReport(Lube_E_ExpectSymbol, "}");
    return LUBE_FAIL;
}

LubeToken GetStringToken(FILE *pFile)
{
    char c = 0;

    CurrentTokenBegin();
    while (!feof(pFile)) {
        c = fgetc(pFile);
        if ('$' == c) {
            c = fgetc(pFile);
            if ('{' == c) {
                if (GetTokenInString(pFile) < 0) return Token_error;
                c = fgetc(pFile);
            }
        }
        if ('\\' == c) {
            c = fgetc(pFile);
            if ('n' == c) {
                c = '\n';
            }
            else if ('\\' != c) {
                AddTokenChar('\\');
            }
        }
        else if ('"' == c) {
            break;
        }
        AddTokenChar(c);
    }
    if ('"' != c) {
        ErrorReport(Lube_E_UnexpectEOF);
        return Token_error;
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(Lube_E_SymbolTooLong);
        return Token_error;
    }
    CurrentTokenEnd();

    return Token_string;
}

int SearchBegin(FILE *pFile, char *pBuffer)
{
    int n = 0;
    char c;

    while (!feof(pFile)) {
        c = fgetc(pFile);
        if (EOF == c) break;

        if ('$' == c) {
            c = fgetc(pFile);

            if ('$' == c) {
                pBuffer[n++] = '$';
            }
            else if ('^' == c) {
                SkipCurrentLine(pFile);
            }
            else {
                ungetc(c, pFile);
                pBuffer[n] = '\0';
                return n;
            }
        }
        else if ('\r' == c) {
            c = fgetc(pFile);
            assert(c == '\n');
            pBuffer[n++] = c;
        }
        else {
            pBuffer[n++] = c;
        }

        if (n >= c_nMaxTextSize) {
            ErrorReport(Lube_E_BufOverflow);
            n = 0;
        }
    }

    pBuffer[n] = '\0';
    return LUBE_FAIL;
}

LubeToken GetTokenFromFile(FILE *pFile, char *pBuffer)
{
    int c;

    if (!s_bIn) {
        c = SearchBegin(pFile, pBuffer);
        if (c < 0) return Token_eof;
        s_bIn = true;
        if (c > 0) return Token_text;
    }

    switch (c = GetValidChar(pFile)) {
        case '{':
            if (s_bBrace) ErrorReport(Lube_E_UnexpectSymbol, "{");
            s_bBrace = true;
            return GetTokenFromFile(pFile, pBuffer);
        case '}':
            if (!s_bBrace) ErrorReport(Lube_E_ExpectSymbol, "{");
            break;
        case '\n':
            if (s_bBrace) ErrorReport(Lube_E_ExpectSymbol, "}");
            break;
        case ';':
            if (s_bBrace) ErrorReport(Lube_E_ExpectSymbol, "}");
            do {c = fgetc(pFile);} while ('\n' != c && EOF != c);
            break;
        default:
            break;
    }
    if (EOF == c) return Token_eof;

    if ('}' == c || '\n' == c) {
        s_bBrace = false;
        s_bIn = false;
        return GetTokenFromFile(pFile, pBuffer);
    }

    CurrentTokenBegin();
    AddTokenChar(c);

    if (isalpha(c) || '_' == c) {
        return GetIdentifyToken(pFile);
    }
    else if ('"' == c) {
        return GetStringToken(pFile);
    }

    CurrentTokenEnd();

    if ('(' == c) {
        return Token_S_lparen;
    }
    else if (')' == c) {
        return Token_S_rparen;
    }
    else if ('.' == c) {
        return Token_S_dot;
    }

    ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
    return Token_error;
}

LubeToken GetToken(FILE *pFile, char *pBuffer)
{
    LubeToken token;

    if (s_peekToken == Token_nothing) {
        token = GetTokenFromFile(pFile, pBuffer);
        if (token == Token_eof) {
            strcpy(g_szCurrentToken, "EOF");
        }
    }
    else {
        token = s_peekToken;
        strcpy(g_szCurrentToken, s_szPeekToken);
        g_nCurrentTokenLen = s_nPeekTokenLen;

        s_peekToken = Token_nothing;
    }

    return token;
}

LubeToken PeekToken(FILE *pFile, char *pBuffer)
{
    char szCurrentToken[c_nMaxTokenSize + 1];
    int nCurrentTokenLen;

    if (s_peekToken == Token_nothing) {
        //
        // Save current token context.
        //
        strcpy(szCurrentToken, g_szCurrentToken);
        nCurrentTokenLen = g_nCurrentTokenLen;

        s_peekToken = GetTokenFromFile(pFile, pBuffer);
        if (s_peekToken == Token_eof) {
            strcpy(s_szPeekToken, "EOF");
        }
        else {
            strcpy(s_szPeekToken, g_szCurrentToken);
        }
        s_nPeekTokenLen = g_nCurrentTokenLen;

        //
        // Restore current token context.
        //
        g_nCurrentTokenLen = nCurrentTokenLen;
        strcpy(g_szCurrentToken, szCurrentToken);
    }
    return s_peekToken;
}
