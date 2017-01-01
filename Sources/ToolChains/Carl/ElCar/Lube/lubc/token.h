
#ifndef __TOKEN_H__
#define __TOKEN_H__

typedef void* PVOID;

typedef enum LubeToken {
    Token_error     = -1,
    Token_eof       = -2,

    Token_nothing   = 0,    Token_text,

    // Symbols --> Token_S_xxx
    Token_symbols   = 100,
    Token_S_lparen,         Token_S_rparen,         Token_S_dot,

    // Keywords --> Token_K__xxx
    Token_keywords  = 1000,
    Token_K_input,          Token_K_output,         Token_K_template,

    Token_K_with,           Token_K_all,            Token_K_do,
    Token_K_end,            Token_K_if,             Token_K_else,
    Token_K_is,             Token_K_and,            Token_K_or,
    Token_K_not,            Token_K_first,          Token_K_last,
    Token_K_true,           Token_K_false,

    Token_objects = 1100,
    Token_K_module,         Token_K_class,          Token_K_interface,
    Token_K_struct,         Token_K_enum,           Token_K_typedef,
    Token_K_parent,         Token_K_aspect,         Token_K_aggregate,
    Token_K_method,         Token_K_constructor,     Token_K_coalescence,
    Token_K_parameter,      Token_K_member,         Token_K_delegate,
    Token_K_library,        Token_K_const,          Token_K_parents_and_me,
    Token_K_super,          Token_K_classforaspect,  Token_K_interfaceconst,
    Token_K_lubeoption,     Token_K_compilingenv,   Token_K_inputtype,
    Token_K_child,


    // Redundant definitions of objects
    Token_K_classes     = Token_K_class,
    Token_K_interfaces  = Token_K_interface,
    Token_K_structs     = Token_K_struct,
    Token_K_enums       = Token_K_enum,
    Token_K_typedefs    = Token_K_typedef,
    Token_K_alias       = Token_K_typedef,
    Token_K_aliases     = Token_K_typedef,
    Token_K_parents     = Token_K_parent,
    Token_K_supers      = Token_K_super,
    Token_K_aspects     = Token_K_aspect,
    Token_K_aggregates  = Token_K_aggregate,
    Token_K_classesforaspect = Token_K_classforaspect,
    Token_K_interfaceconsts = Token_K_interfaceconst,
    Token_K_methods     = Token_K_method,
    Token_K_constructors = Token_K_constructor,
    Token_K_coalescences = Token_K_coalescence,
    Token_K_function    = Token_K_method,
    Token_K_functions   = Token_K_method,
    Token_K_parameters  = Token_K_parameter,
    Token_K_argument    = Token_K_parameter,
    Token_K_arguments   = Token_K_parameter,
    Token_K_members     = Token_K_member,
    Token_K_element     = Token_K_member,
    Token_K_elements    = Token_K_member,
    Token_K_consts      = Token_K_const,
    Token_K_libraries   = Token_K_library,
    Token_K_importlib   = Token_K_library,
    Token_K_importlibs  = Token_K_library,

    Token_members = 1200,
    Token_K_name,           Token_K_attrib,         Token_K_version,
    Token_K_uunm,           Token_K_clsid,          Token_K_iid,
    Token_K_uuid,           Token_K_type,           Token_K_value,
    Token_K_dimention,      Token_K_majorversion,   Token_K_minorversion,
    Token_K_carcode,        Token_K_builddate,      Token_K_namespace,
    Token_K_fullname,

    // Redundant definitions of members
    Token_K_attribute   = Token_K_attrib,
    Token_K_guid        = Token_K_uuid,
    Token_K_ezclsid     = Token_K_clsid,

    // Others --> Token_xxx
    Token_others    = 10000,
    Token_ident,            Token_string,
} LubeToken;

inline bool IsKeywordToken(LubeToken token)
{
    return (token > Token_keywords && token < Token_others);
}

inline bool IsSymbolToken(LubeToken token)
{
    return (token > Token_symbols && token < Token_keywords);
}

inline bool IsObjectToken(LubeToken token)
{
    return (token > Token_objects && token < Token_members);
}

inline bool IsMemberToken(LubeToken token)
{
    return (token > Token_members && token < Token_others);
}

extern LubeToken GetToken(FILE *, char *);
extern LubeToken PeekToken(FILE *, char *);

extern void TokenInit();
extern PVOID SaveTokenContext();
extern void RestoreTokenContext(PVOID);

const int c_nMaxTokenSize = 511;
const int c_nMaxTextSize = 0x8000;

extern int g_nLineNumber;
extern char g_szCurrentToken[c_nMaxTokenSize + 1];
extern int g_nCurrentTokenLen;

#endif // __TOKEN_H__
