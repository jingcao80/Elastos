//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdio.h>

typedef enum CARToken {
    Token_Error     = -1,
    Token_EOF       = -2,

    Token_Nothing   = 0,

    // Symbols --> Token_S_xxx
    Token_symbols   = 100,
    Token_S_lbracket,       Token_S_rbracket,       Token_S_comma,
    Token_S_lparen,         Token_S_rparen,         Token_S_lbrace,
    Token_S_rbrace,         Token_S_colon,          Token_S_semicolon,
    Token_S_star,           Token_S_langle,         Token_S_rangle,
    Token_S_assign,         Token_S_dot,            Token_S_and,
    Token_S_eor,            Token_S_ior,            Token_S_not,
    Token_S_add,            Token_S_at,

    // Keywords --> Token_K_xxx
    Token_keywords  = 1000,
    Token_K_library,        Token_K_module,         Token_K_version,

    Token_K_interface,      Token_K_in,             Token_K_out,
    Token_K_local,          Token_K_callee,         Token_K_deprecated,

    Token_K_Float,          Token_K_Boolean,
    Token_K_Double,         Token_K_Byte,           Token_K_PVoid,
    Token_K_Int8,           Token_K_Int16,
    Token_K_Int32,          Token_K_Int64,          Token_K_UInt16,
    Token_K_UInt32,         Token_K_UInt64,         Token_K_Char16,
    Token_K_String,         Token_K_Char32,

    Token_K_ArrayOf,

    Token_K_EMuid,          Token_K_EGuid,         Token_K_ECode,
    Token_K_delegate,       Token_K_EventHandler,

    Token_K_class,          Token_K_scriptable,
    Token_K_singleton,      Token_K_aggregates,
    Token_K_virtual,        Token_K_generic,        Token_K_aspect,
    Token_K_regime,         Token_K_domain,         Token_K_hidden,
    Token_K_callback,
    Token_K_freethreaded,   Token_K_naked,          Token_K_constructor,
    Token_K_private,        Token_K_classlocal,     Token_K_protect,
    Token_K_coalesce,       Token_K_asynchronous,
    Token_K_filtering,      Token_K_parcelable,    Token_K_privileged,

    Token_K_struct,         Token_K_enum,           Token_K_typedef,
    Token_K_const,          Token_K_dummytype,

    Token_K_pragma,         Token_K_disable,        Token_K_enable,
    Token_K_importlib,      Token_K_merge,          Token_K_import,
    Token_K_mergelib,       Token_K_substitutes,    Token_K_inherits,
    Token_K_pertainsto,     Token_K_extends,        Token_K_final,
    Token_K_affiliates,     Token_K_callbacks,      Token_K_delegates,
    Token_K_callbacksink,   Token_K_delegatesink,   Token_K_namespace,
    Token_K_using,          Token_K_oneway,         Token_K_submodule,

    Token_K_true,           Token_K_false,          Token_K_null,

    // Others --> Token_xxx
    Token_others    = 10000,
    Token_uunm,             Token_ident,            Token_integer,
    Token_decimal,          Token_hinteger,         Token_string,
    Token_character,
} CARToken;

inline bool IsKeywords(CARToken token)
{
    return (token > Token_keywords && token < Token_others);
}

inline bool IsSymbols(CARToken token)
{
    return (token > Token_symbols && token < Token_keywords);
}

extern void TokenInit();

extern CARToken GetToken(FILE *);
extern char GetNextChar(FILE *);
extern CARToken PeekToken(FILE *);
extern CARToken GetUunm(FILE *);
extern void * SaveTokenContext();
extern void RestoreTokenContext(void *);

const int c_nMaxTokenSize = 255;
extern int g_nLineNumber;
extern char g_szCurrentToken[c_nMaxTokenSize + 1];
extern int g_nCurrentTokenLen;

#endif // __TOKEN_H__
