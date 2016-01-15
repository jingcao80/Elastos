//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

// Token processor, read characters from input file byte-by-byte and
// solved them as tokens which parser known.
// To avoid too complex program flow, the token 'uunm' is not analysed
// by main token processor. Whereas, there is a functions provided:
// GetUUNM(), it can be invoked by parser, for parser known when need to
// invoke it.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "carerr.h"

int g_nLineNumber;
char g_szCurrentToken[c_nMaxTokenSize + 1];
int g_nCurrentTokenLen;

static CARToken s_peekToken;
static char s_szPeekToken[c_nMaxTokenSize + 1];
static int s_nPeekTokenLen;

struct SymbolToken
{
    char        symbol;
    CARToken    token;
};
typedef struct SymbolToken SymbolToken;

static const SymbolToken s_symbolTable[] = {
    { '(', Token_S_lparen },
    { ')', Token_S_rparen },
    { '{', Token_S_lbrace },
    { '}', Token_S_rbrace },
    { '[', Token_S_lbracket },
    { ']', Token_S_rbracket },
    { ',', Token_S_comma },
    { ';', Token_S_semicolon },
    { '*', Token_S_star },
    { ':', Token_S_colon },
    { '=', Token_S_assign },
    { '<', Token_S_langle },
    { '>', Token_S_rangle },
    { '.', Token_S_dot },
    { '&', Token_S_and },
    { '^', Token_S_eor },
    { '|', Token_S_ior },
    { '~', Token_S_not },
    { '+', Token_S_add },
    { '@', Token_S_at },
    { EOF, Token_EOF },
};
const int c_symbolNumber = sizeof(s_symbolTable) / sizeof(SymbolToken);

struct KeywordToken
{
    const char  *pszKeyword;
    CARToken    token;
};
typedef struct KeywordToken KeywordToken;

static const KeywordToken s_keywords_a[] = {
    { "affiliates", Token_K_affiliates },
    { "aggregates", Token_K_aggregates },
    { "aspect", Token_K_aspect },
    { "asynchronous", Token_K_asynchronous },
};

static const KeywordToken s_keywords_c[] = {
    { "callback", Token_K_callback },
    { "callbacks", Token_K_callbacks },
    { "callbacksink", Token_K_callbacksink },
    { "callee", Token_K_callee},
    { "class", Token_K_class },
    { "const", Token_K_const },
    { "constructor", Token_K_constructor },
    { "coalesce", Token_K_coalesce },
    { "classlocal", Token_K_classlocal },
};

static const KeywordToken s_keywords_d[] = {
    { "delegate", Token_K_delegate },
    { "delegates", Token_K_delegates },
    { "delegatesink", Token_K_delegatesink },
    { "deprecated", Token_K_deprecated },
    { "disable", Token_K_disable },
    { "domain", Token_K_domain },
    { "dummytype", Token_K_dummytype },
};

static const KeywordToken s_keywords_e[] = {
    { "enum", Token_K_enum },
    { "enable", Token_K_enable },
    { "extends", Token_K_extends },
};

static const KeywordToken s_keywords_f[] = {
    { "filtering", Token_K_filtering },
    { "final", Token_K_final },
};

static const KeywordToken s_keywords_g[] = {
    { "generic", Token_K_generic },
};

static const KeywordToken s_keywords_h[] = {
    { "hidden", Token_K_hidden },
};

static const KeywordToken s_keywords_i[] = {
    { "import", Token_K_import },
    { "importlib", Token_K_importlib },
    { "in", Token_K_in },
    { "inherits", Token_K_inherits },
    { "interface", Token_K_interface },
};

static const KeywordToken s_keywords_l[] = {
    { "local", Token_K_local },
    { "library", Token_K_library },
};

static const KeywordToken s_keywords_m[] = {
    { "merge", Token_K_merge },
    { "mergelib", Token_K_mergelib },
    { "module", Token_K_module },
};

static const KeywordToken s_keywords_n[] = {
    { "namespace", Token_K_namespace },
};

static const KeywordToken s_keywords_o[] = {
    { "oneway", Token_K_oneway },
    { "out", Token_K_out },
};

static const KeywordToken s_keywords_p[] = {
    { "parcelable", Token_K_parcelable },
    { "pertainsto", Token_K_pertainsto },
    { "pragma", Token_K_pragma },
    { "private", Token_K_private },
    { "privileged", Token_K_privileged },
};

static const KeywordToken s_keywords_r[] = {
    { "regime", Token_K_regime },
};

static const KeywordToken s_keywords_s[] = {
    { "scriptable", Token_K_scriptable },
    { "singleton", Token_K_singleton },
    { "struct", Token_K_struct },
    { "submodule", Token_K_submodule },
    { "substitutes", Token_K_substitutes },
};

static const KeywordToken s_keywords_t[] = {
    { "typedef", Token_K_typedef },
};

static const KeywordToken s_keywords_u[] = {
    { "using", Token_K_using },
};

static const KeywordToken s_keywords_v[] = {
    { "version", Token_K_version },
    { "virtual", Token_K_virtual },
};

static const KeywordToken s_keywords_A[] = {
    { "ArrayOf", Token_K_ArrayOf },
};

static const KeywordToken s_keywords_B[] = {
    { "Boolean", Token_K_Boolean },
    { "Byte", Token_K_Byte},
};

static const KeywordToken s_keywords_C[] = {
    { "Char16", Token_K_Char16 },
    { "Char32", Token_K_Char32 },
};

static const KeywordToken s_keywords_D[] = {
    { "Double", Token_K_Double },
};

static const KeywordToken s_keywords_E[] = {
    { "EventHandler", Token_K_EventHandler },
    { "EMuid", Token_K_EMuid },
    { "ECode", Token_K_ECode },
    { "EGuid", Token_K_EGuid },
};

static const KeywordToken s_keywords_F[] = {
    { "FALSE", Token_K_false },
    { "Float", Token_K_Float },
};

static const KeywordToken s_keywords_G[] = {
};

static const KeywordToken s_keywords_I[] = {
    { "Int16", Token_K_Int16 },
    { "Int32", Token_K_Int32 },
    { "Int64", Token_K_Int64 },
    { "Int8", Token_K_Int8 },
};

static const KeywordToken s_keywords_O[] = {
};

static const KeywordToken s_keywords_P[] = {
    { "PVoid", Token_K_PVoid },
};

static const KeywordToken s_keywords_S[] = {
    { "String", Token_K_String },
};

static const KeywordToken s_keywords_T[] = {
    { "TRUE", Token_K_true },
};

static const KeywordToken s_keywords_U[] = {
    { "UInt16", Token_K_UInt16 },
    { "UInt32", Token_K_UInt32 },
    { "UInt64", Token_K_UInt64 },
};

static const KeywordToken s_keywords_W[] = {
};

typedef struct KeywordIndex {
    int nNumber;
    const KeywordToken * pKeywords;
} KeywordIndex;

static const KeywordIndex s_lowerIndexTable[26] = {
    { sizeof(s_keywords_a) / sizeof(KeywordToken), s_keywords_a },  // a
    { 0, NULL },                                                    // b
    { sizeof(s_keywords_c) / sizeof(KeywordToken), s_keywords_c },  // c
    { sizeof(s_keywords_d) / sizeof(KeywordToken), s_keywords_d },  // d
    { sizeof(s_keywords_e) / sizeof(KeywordToken), s_keywords_e },  // e
    { sizeof(s_keywords_f) / sizeof(KeywordToken), s_keywords_f },  // f
    { sizeof(s_keywords_g) / sizeof(KeywordToken), s_keywords_g },  // g
    { sizeof(s_keywords_h) / sizeof(KeywordToken), s_keywords_h },  // h
    { sizeof(s_keywords_i) / sizeof(KeywordToken), s_keywords_i },  // i
    { 0, NULL },                                                    // j
    { 0, NULL },                                                    // k
    { sizeof(s_keywords_l) / sizeof(KeywordToken), s_keywords_l },  // l
    { sizeof(s_keywords_m) / sizeof(KeywordToken), s_keywords_m },  // m
    { sizeof(s_keywords_n) / sizeof(KeywordToken), s_keywords_n },  // n
    { sizeof(s_keywords_o) / sizeof(KeywordToken), s_keywords_o },  // o
    { sizeof(s_keywords_p) / sizeof(KeywordToken), s_keywords_p },  // p
    { 0, NULL },                                                    // q
    { sizeof(s_keywords_r) / sizeof(KeywordToken), s_keywords_r },  // r
    { sizeof(s_keywords_s) / sizeof(KeywordToken), s_keywords_s },  // s
    { sizeof(s_keywords_t) / sizeof(KeywordToken), s_keywords_t },  // t
    { sizeof(s_keywords_u) / sizeof(KeywordToken), s_keywords_u },  // u
    { sizeof(s_keywords_v) / sizeof(KeywordToken), s_keywords_v },  // v
    { 0, NULL },                                                    // w
    { 0, NULL },                                                    // x
    { 0, NULL },                                                    // y
    { 0, NULL },                                                    // z
};

static const KeywordIndex s_upperIndexTable[26] = {
    { sizeof(s_keywords_A) / sizeof(KeywordToken), s_keywords_A },  // A
    { sizeof(s_keywords_B) / sizeof(KeywordToken), s_keywords_B },  // B
    { sizeof(s_keywords_C) / sizeof(KeywordToken), s_keywords_C },  // C
    { sizeof(s_keywords_D) / sizeof(KeywordToken), s_keywords_D },  // D
    { sizeof(s_keywords_E) / sizeof(KeywordToken), s_keywords_E },  // E
    { sizeof(s_keywords_F) / sizeof(KeywordToken), s_keywords_F },  // F
    { sizeof(s_keywords_G) / sizeof(KeywordToken), s_keywords_G },  // G
    { 0, NULL },                                                    // H
    { sizeof(s_keywords_I) / sizeof(KeywordToken), s_keywords_I },  // I
    { 0, NULL },                                                    // J
    { 0, NULL },                                                    // K
    { 0, NULL },                                                    // L
    { 0, NULL },                                                    // M
    { 0, NULL },                                                    // N
    { sizeof(s_keywords_O) / sizeof(KeywordToken), s_keywords_O },  // O
    { sizeof(s_keywords_P) / sizeof(KeywordToken), s_keywords_P },  // P                                                  // P
    { 0, NULL },                                                    // Q
    { 0, NULL },                                                    // R
    { sizeof(s_keywords_S) / sizeof(KeywordToken), s_keywords_S },  // S
    { sizeof(s_keywords_T) / sizeof(KeywordToken), s_keywords_T },  // T
    { sizeof(s_keywords_U) / sizeof(KeywordToken), s_keywords_U },  // U
    { 0, NULL },                                                    // V
    { sizeof(s_keywords_W) / sizeof(KeywordToken), s_keywords_W },  // W
    { 0, NULL },                                                    // X
    { 0, NULL },                                                    // Y
    { 0, NULL },                                                    // Z
};

inline void CurrentTokenBegin()
{
    g_nCurrentTokenLen = 0;
}

inline void CurrentTokenEnd()
{
    g_szCurrentToken[g_nCurrentTokenLen] = '\0';
}

inline void AddCurrentToken(char c)
{
    if (g_nCurrentTokenLen < c_nMaxTokenSize)
        g_szCurrentToken[g_nCurrentTokenLen++] = c;
    else {
        g_nCurrentTokenLen++;
    }
}

inline bool IsCurrentTokenOverflow()
{
    return g_nCurrentTokenLen > c_nMaxTokenSize;
}

inline int fgetcEx(FILE *fp)
{
    int c = fgetc(fp);

    if (c == '\n') g_nLineNumber++;
    return c;
}

inline int ungetcEx(int c, FILE *fp)
{
    if ('\n' == c) g_nLineNumber--;
    return ungetc(c, fp);
}

#define fgetc(fp)       fgetcEx(fp)
#define ungetc(c, fp)   ungetcEx(c, fp)

void TokenInit()
{
    g_nLineNumber = 1;
    g_nCurrentTokenLen = 0;
    s_peekToken = Token_Nothing;
    s_nPeekTokenLen = 0;
}

typedef struct TokenContext {
    int         nLineNumber;
    CARToken    peekToken;
    int         nPeekTokenLen;
    char        szPeekToken[c_nMaxTokenSize + 1];
}   TokenContext;

void *SaveTokenContext()
{
    TokenContext *pContext;

    pContext = new struct TokenContext;
    if (!pContext) return NULL;
    pContext->nLineNumber = g_nLineNumber;
    pContext->peekToken = s_peekToken;
    pContext->nPeekTokenLen = s_nPeekTokenLen;
    memcpy(pContext->szPeekToken, s_szPeekToken, s_nPeekTokenLen);

    return (void *)pContext;
}

void RestoreTokenContext(void *pv)
{
    TokenContext *pContext;

    if (!pv) return;

    pContext = (TokenContext *)pv;
    g_nLineNumber = pContext->nLineNumber;
    s_peekToken = pContext->peekToken;
    s_nPeekTokenLen = pContext->nPeekTokenLen;
    memcpy(s_szPeekToken, pContext->szPeekToken, s_nPeekTokenLen);
    delete pContext;
}

CARToken GetSymbolToken(char symbol)
{
    int n;

    for (n = 0; n < c_symbolNumber; n++) {
        if (symbol == s_symbolTable[n].symbol)
            return s_symbolTable[n].token;
    }

    ErrorReport(CAR_E_IllegalChar, symbol);
    return Token_Error;
}

CARToken TryGetKeywordToken(const char *s)
{
    const KeywordIndex *pIndex;
    int n;

    if (islower(*s)) {
        pIndex = &s_lowerIndexTable[*s - 'a'];
    }
    else if (isupper(*s)) {
        pIndex = &s_upperIndexTable[*s - 'A'];
    }
    else {
        return Token_ident;
    }

    for (n = 0; n < pIndex->nNumber; n++) {
        if (!strcmp(s, pIndex->pKeywords[n].pszKeyword)) {
            return pIndex->pKeywords[n].token;
        }
    }
    return Token_ident;
}

char GetNextChar(FILE *fp)
{
    char c;

    do {
        c = EOF;
        while (!feof(fp)) {
            c = fgetc(fp);
            if (!isspace(c)) break;
        }

        //
        // skip comments in CAR file
        //
        if ('/' == c) {
            c = fgetc(fp);
            if ('/' == c) {
                while (!feof(fp) && fgetc(fp) != '\n');
            }
            else if ('*' == c) {
                while (!feof(fp)) {
                    if (fgetc(fp) == '*') {
                        c = fgetc(fp);
                        if (c == '/') {
                            break;
                        }
                        else {
                            ungetc(c, fp);
                        }
                    }
                }
                if (feof(fp)) {
                    c = EOF;
                    ErrorReport(CAR_E_UnexpectEOF);
                }
            }
            else {
                ungetc(c, fp);
                c = '/';
                break;
            }
        }
    } while ('/' == c);

    return c;
}

CARToken GetIdentifyToken(FILE *fp)
{
    char c;

    while (!feof(fp)) {
        c = fgetc(fp);
        if (isalnum(c) || '_' == c || '.' == c) {
            AddCurrentToken(c);
        }
        else {
            ungetc(c, fp);
            break;
        }
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(CAR_E_SymbolTooLong);
        return Token_Error;
    }
    CurrentTokenEnd();

    return TryGetKeywordToken(g_szCurrentToken);
}

#define ishex(c) (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

CARToken GetNumberToken(FILE *fp, char c)
{
    bool bDecimal = false;

    if ('0' == c) {
        c = fgetc(fp);
        if ('x' == c || 'X' == c) {
            AddCurrentToken(c);

            while (!feof(fp)) {
                c = fgetc(fp);
                if (!ishex(c)) {
                    if (isalpha(c) || '_' == c || '.' == c) {
                        ErrorReport(CAR_E_UnexpectChar, c);
                        return Token_Error;
                    }
                    ungetc(c, fp);
                    break;
                }
                AddCurrentToken(c);
            }

            if (IsCurrentTokenOverflow()) {
                ErrorReport(CAR_E_SymbolTooLong);
                return Token_Error;
            }
            CurrentTokenEnd();

            if (g_nCurrentTokenLen > 2) {
                return Token_hinteger;
            }
            else {
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Token_Error;
            }
        }
        else {
            ungetc(c, fp);
        }
    }

    while (!feof(fp)) {
        c = fgetc(fp);
        if ('.' == c) {
            if (!bDecimal)
                bDecimal = true;
            else {
                ErrorReport(CAR_E_UnexpectChar, '.');
                return Token_Error;
            }
        }
        else if (!isdigit(c)) {
            if (isalpha(c) || '_' == c) {
                ErrorReport(CAR_E_UnexpectChar, c);
                return Token_Error;
            }
            ungetc(c, fp);
            break;
        }

        AddCurrentToken(c);
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(CAR_E_SymbolTooLong);
        return Token_Error;
    }
    CurrentTokenEnd();

    return bDecimal ? Token_decimal:Token_integer;
}

CARToken GetStringToken(FILE *fp)
{
    char c = 0;

    CurrentTokenBegin();
    while (!feof(fp)) {
        c = fgetc(fp);
        if ('\\' == c) {
            c = fgetc(fp);
            if ('\\' != c) AddCurrentToken('\\');
        }
        else if ('"' == c)
            break;
        AddCurrentToken(c);
    }
    if ('"' != c) {
        ErrorReport(CAR_E_UnexpectEOF);
        return Token_Error;
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(CAR_E_SymbolTooLong);
        return Token_Error;
    }
    CurrentTokenEnd();

    return Token_string;
}

CARToken GetCharacterToken(FILE *fp)
{
    char c = 0;

    CurrentTokenBegin();
    if (feof(fp)) {
        ErrorReport(CAR_E_UnexpectEOF);
        return Token_Error;
    }
    c = fgetc(fp);
    if ('\\' == c) {
        c = fgetc(fp);
        if ('\\' != c) AddCurrentToken('\\');
    }
    AddCurrentToken(c);
    c = fgetc(fp);
    if ('\'' != c) {
        ErrorReport(CAR_E_UnexpectEOF);
        return Token_Error;
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(CAR_E_SymbolTooLong);
        return Token_Error;
    }
    CurrentTokenEnd();

    return Token_character;
}

CARToken GetTokenFromFile(FILE *fp)
{
    char c;

    CurrentTokenBegin();
    c = GetNextChar(fp);
    AddCurrentToken(c);

    if (isalpha(c) || '_' == c) {
        return GetIdentifyToken(fp);
    }
    else if (isdigit(c) || '-' == c) {
        return GetNumberToken(fp, c);
    }
    else if ('"' == c) {
        return GetStringToken(fp);
    }
    else if ('\'' == c) {
        return GetCharacterToken(fp);
    }

    CurrentTokenEnd();
    return GetSymbolToken(c);
}

CARToken GetToken(FILE *fp)
{
    CARToken token;

    if (s_peekToken == Token_Nothing) {
        token = GetTokenFromFile(fp);
        if (token == Token_EOF)
            strcpy(g_szCurrentToken, "EOF");
    }
    else {
        token = s_peekToken;
        strcpy(g_szCurrentToken, s_szPeekToken);
        g_nCurrentTokenLen = s_nPeekTokenLen;

        s_peekToken = Token_Nothing;
    }

    return token;
}

CARToken PeekToken(FILE *fp)
{
    char szCurrentToken[c_nMaxTokenSize + 1];
    int nCurrentTokenLen;

    if (s_peekToken == Token_Nothing) {
        //
        // Save current token context.
        //
        strcpy(szCurrentToken, g_szCurrentToken);
        nCurrentTokenLen = g_nCurrentTokenLen;

        s_peekToken = GetTokenFromFile(fp);
        if (s_peekToken == Token_EOF)
            strcpy(s_szPeekToken, "EOF");
        else
            strcpy(s_szPeekToken, g_szCurrentToken);
        s_nPeekTokenLen = g_nCurrentTokenLen;

        //
        // Restore current token context.
        //
        g_nCurrentTokenLen = nCurrentTokenLen;
        strcpy(g_szCurrentToken, szCurrentToken);
    }
    return s_peekToken;
}

/*
 * <FirstName> ::= <InternetDomainName> | <PersonalEmailName>
 */
bool isValidFirstName(const char* pszName)
{
    return true;
}

/*
 * <MiddleName> ::= <Category> {"."<Category>}
 * <category>   ::= <ID>
 * <ID>         ::= {<let-num> | "_"}+
 * <let-num>    ::= <letter> | <number>
 * <letter>     ::= <upper> | <lower>
 * <upper>      ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" |
 *                  "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" |
 *                  "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" |
 *                  "Y" | "Z"
 * <lower>      ::= "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" |
 *                  "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" |
 *                  "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" |
 *                  "y" | "z"
 * <number>     ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" |
 *                  "8" | "9"
 */
bool isValidMiddleName(const char* pszName)
{
    enum {
        STAT_CAT_BEG,
        STAT_CAT_ID,
    } stat;
    int i;
    char c;

    i = 0; stat = STAT_CAT_BEG;
    while ((c = *(pszName + i)) != ':') {
        if (c == '.') {
            if (STAT_CAT_BEG == stat) return false;
            else if (STAT_CAT_ID == stat) stat = STAT_CAT_BEG;
        }
        else if (isalpha(c) || isdigit(c)) stat = STAT_CAT_ID;
		else if (c != '_') return false;

        i++;
    }

    if (STAT_CAT_ID == stat) return true;
    else return false;
}

bool isValidSuffix(const char* pszName)
{
    if (!strcmp("eco", pszName)) return true;
    return false;
}

/*
 * <LastName>   ::= <ModuleName> "." <Suffix>
 * <ModuleName> ::= <ID>
 * <Suffix>     ::= "eco"
 */
bool isValidLastName(const char* pszName)
{
    enum {
        STAT_MN_BEG,
        STAT_MN_ID,
    } stat;
    int i;
    char c;

    i = 0; stat = STAT_MN_BEG;
    while ((c = *(pszName + i)) != '.') {
        if (isalpha(c) || isdigit(c)) stat = STAT_MN_ID;
        else if (c != '_') return false;

        i++;
    }

    if (STAT_MN_BEG == stat) return false;

    return isValidSuffix(pszName + i + 1);
}

/*
 * The BNF grammar of Universal Unique CAR Module Name
 * <CarModuleName> ::= <FirstName>":"<MiddleName>":"<LastName>
 * <FirstName>     ::= <InternetDomainName> | <PersonalEmailName>
 * <MiddleName>    ::= <Category>{"."<Category>}
 * <LastName>      ::= <ModuleName>["."<Suffix>]
 */
bool isValidUUNM(const char* pszUUMN)
{
    char* pszName;

    if (!isValidFirstName(pszUUMN)) return false;

    pszName = strchr((char*)pszUUMN, ':');
    if ((pszName == NULL) || (!isValidMiddleName(pszName + 1))) return false;

    pszName = strchr(pszName + 1, ':');
    if ((pszName == NULL) || (!isValidLastName(pszName + 1))) return false;

    return true;
}

/*
 * Read http://192.168.0.8/web/bbs/viewthread.php?tid=3994&fpage=1 for more
 * information about uunm changes.
 *
 * For using URN as uunm, please read some RFC documents:
 * http://www.ietf.org/rfc/rfc2141.txt
 * http://www.ietf.org/rfc/rfc2168.txt
 * http://www.ietf.org/rfc/rfc2611.txt
 */
CARToken GetUunm(FILE *fp)
{
    int c;

    /*
     * PeekToken must be called already before calling GetUunm, see parser.cpp
     * #P_CAR. And s_peekToken should be Token_ident already. For example,
     * "module urn:elastos:demo:button.dll"
     * previous PeekToken call will already put "www" in the s_szPeekToken as
     * Token_ident. Following we will copy those token so the full uunm can be
     * retrieved correctly.
     */
    strcpy(g_szCurrentToken, s_szPeekToken);
    g_nCurrentTokenLen = s_nPeekTokenLen;
    s_peekToken = Token_Nothing;

    while(!feof(fp)) {
        c = fgetc(fp);
        if ('{' == c || '\t' == c || '\r' == c || '\n' == c || isspace(c)) {
            ungetc(c, fp);
            break;
        }
        AddCurrentToken(c);
    }

    if (feof(fp)) {
        ErrorReport(CAR_E_UnexpectEOF);
        return Token_Error;
    }
    if (IsCurrentTokenOverflow()) {
        ErrorReport(CAR_E_SymbolTooLong);
        return Token_Error;
    }

    CurrentTokenEnd();

    if (!isValidUUNM(g_szCurrentToken)) {
        ErrorReport(CAR_E_InvalidUUNM);
        return Token_Error;
    }

    return Token_uunm;
}
