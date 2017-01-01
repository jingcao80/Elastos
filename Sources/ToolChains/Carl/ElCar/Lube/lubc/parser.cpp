
#include <assert.h>

#include "parser.h"

#define _MAX_PATH 256

static ParserContext *s_pContext = NULL;
static const char *s_pszSourceDirList = "";
static char s_szCurrentPath[_MAX_PATH] = "";

int P_Statement();

ObjectType GetConditionObjectType(LubeToken token)
{
    if (!IsObjectToken(token)) {
        ErrorReport(Lube_E_InvalidWithObj, g_szCurrentToken);
        return s_pContext->m_curObject;
    }
    switch (token) {
        case Token_K_library:
            return Object_Library;
        case Token_K_class:
            return Object_Class;
        case Token_K_super:
            return Object_Super;
        case Token_K_interface:
            if (0 != s_pContext->m_nClass) return Object_ClsIntf;
            return Object_Interface;
        case Token_K_struct:
            return Object_Struct;
        case Token_K_enum:
            return Object_Enum;
        case Token_K_const:
            return Object_Const;
        case Token_K_typedef:
            return Object_Typedef;
        case Token_K_interfaceconst:
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "interfaceconst");
                return Object_None;
            }
            return Object_IntfConst;
        case Token_K_method:
            if (Object_Class == s_pContext->m_curObject) {
                return Object_ClassMethod;
            }
            else if (0 != s_pContext->m_nInterface) {
                return Object_IntfMethod;
            }
            ErrorReport(Lube_E_NoValidObject, "interface");
            return Object_None;
        case Token_K_constructor:
            return Object_ClassConstructor;
        case Token_K_coalescence:
            return Object_CbCoalescence;
        case Token_K_parameter:
            if (0 == s_pContext->m_nClassMethod
                && 0 == s_pContext->m_nIntfMethod) {
                ErrorReport(Lube_E_NoValidObject, "method");
                return Object_None;
            }
            return Object_Param;
        case Token_K_member:
            if (Object_Struct == s_pContext->m_curObject) {
                return Object_StcMember;
            }
            else if (Object_Enum == s_pContext->m_curObject) {
                return Object_EnumMember;
            }
            ErrorReport(Lube_E_NoValidObject, "member");
            return Object_None;
        case Token_K_aspect:
            if (0 == s_pContext->m_nClass) {
                ErrorReport(Lube_E_NoValidObject, "class");
                return Object_None;
            }
            return Object_Aspect;
        case Token_K_aggregate:
            if (0 == s_pContext->m_nClass) {
                ErrorReport(Lube_E_NoValidObject, "class");
                return Object_None;
            }
            return Object_Aggregate;
        case Token_K_classforaspect:
            if (0 == s_pContext->m_nClass) {
                ErrorReport(Lube_E_NoValidObject, "classforaspect");
                return Object_None;
            }
            return Object_ClassForAspect;
        case Token_K_parent:
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "parent");
                return Object_None;
            }
            return Object_Parent;
        case Token_K_child:
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "child");
                return Object_None;
            }
            return Object_Child;
        case Token_K_parents_and_me:
            if (Object_ClsIntf == s_pContext->m_curObject) {
                return Object_ClsIntfAndParents;
            }
            ErrorReport(Lube_E_NoValidObject, "parents_and_me");
            return Object_None;
        default:
            ErrorReport(Lube_E_InvalidWithObj, g_szCurrentToken);
            return Object_None;
    }
}

ObjectType GetObjectType(LubeToken token)
{
    if (!IsObjectToken(token)) {
        ErrorReport(Lube_E_InvalidWithObj, g_szCurrentToken);
        return s_pContext->m_curObject;
    }
    switch (token) {
        case Token_K_module:
            return Object_Module;
        case Token_K_library:
            if (0 == s_pContext->m_nLibrary) {
                ErrorReport(Lube_E_NoValidObject, "library");
                return Object_None;
            }
            return Object_Library;
        case Token_K_class:
            if (0 == s_pContext->m_nClass) {
                ErrorReport(Lube_E_NoValidObject, "class");
                return Object_None;
            }
            return Object_Class;
        case Token_K_interface:
            if (0 != s_pContext->m_nClass) {
                return Object_ClsIntf;
            }
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "interface");
                return Object_None;
            }
            return Object_Interface;
        case Token_K_struct:
            if (0 == s_pContext->m_nStruct) {
                ErrorReport(Lube_E_NoValidObject, "struct");
                return Object_None;
            }
            return Object_Struct;
        case Token_K_enum:
            if (0 == s_pContext->m_nEnum) {
                ErrorReport(Lube_E_NoValidObject, "enum");
                return Object_None;
            }
            return Object_Enum;
        case Token_K_const:
            if (0 == s_pContext->m_nConst) {
                ErrorReport(Lube_E_NoValidObject, "const");
                return Object_None;
            }
            return Object_Const;
        case Token_K_typedef:
            if (0 == s_pContext->m_nTypedef) {
                ErrorReport(Lube_E_NoValidObject, "typedef");
                return Object_None;
            }
            return Object_Typedef;
        case Token_K_interfaceconst:
            if (0 == s_pContext->m_nIntfConst) {
                ErrorReport(Lube_E_NoValidObject, "interfaceconst");
                return Object_None;
            }
            return Object_IntfConst;
        case Token_K_method:
            if (0 != s_pContext->m_nIntfMethod) {
                return Object_IntfMethod;
            }
            else if (0 != s_pContext->m_nClassMethod) {
                return Object_ClassMethod;
            }
            ErrorReport(Lube_E_NoValidObject, "method");
            return Object_None;
        case Token_K_coalescence:
            return Object_CbCoalescence;
        case Token_K_parameter:
            if (0 == s_pContext->m_nParameter) {
                ErrorReport(Lube_E_NoValidObject, "parameter");
                return Object_None;
            }
            return Object_Param;
        case Token_K_aspect:
            if (0 == s_pContext->m_nAspect) {
                ErrorReport(Lube_E_NoValidObject, "aspect");
                return Object_None;
            }
            return Object_Aspect;
        case Token_K_aggregate:
            if (0 == s_pContext->m_nAggregate) {
                ErrorReport(Lube_E_NoValidObject, "aggregate");
                return Object_None;
            }
            return Object_Aggregate;
        case Token_K_classforaspect:
            if (0 == s_pContext->m_nForClass) {
                ErrorReport(Lube_E_NoValidObject, "classforaspect");
                return Object_None;
            }
            return Object_ClassForAspect;
        case Token_K_member:
            if (Object_Struct == s_pContext->m_curObject) {
                if (0 != s_pContext->m_nStructMember) {
                    return Object_StcMember;
                }
            }
            else if (Object_Enum == s_pContext->m_curObject) {
                if (0 != s_pContext->m_nEnumMember) {
                    return Object_EnumMember;
                }
            }
            ErrorReport(Lube_E_NoValidObject, "struct or enum member");
            return Object_None;
        case Token_K_super:
            if (0 == s_pContext->m_nClass) {
                ErrorReport(Lube_E_NoValidObject, "super");
                return Object_None;
            }
            return Object_Super;
        case Token_K_parent:
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "parent");
                return Object_None;
            }
            return Object_Parent;
        case Token_K_child:
            if (0 == s_pContext->m_nInterface) {
                ErrorReport(Lube_E_NoValidObject, "child");
                return Object_None;
            }
            return Object_Child;
        case Token_K_parents_and_me:
            if (Object_ClsIntf == s_pContext->m_curObject) {
                return Object_ClsIntfAndParents;
            }
            ErrorReport(Lube_E_NoValidObject, "parents_and_me");
            return Object_None;
        case Token_K_lubeoption:
            return Object_LubeOption;
        case Token_K_compilingenv:
            return Object_CompilingEnv;
        case Token_K_inputtype:
            return Object_InuptType;
        default:
            ErrorReport(Lube_E_InvalidWithObj, g_szCurrentToken);
            return Object_None;
    }
}

MemberType GetMemberType(ObjectType obj, LubeToken token)
{
    if (!IsMemberToken(token)) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
        return Member_None;
    }

    switch (token) {
        case Token_K_name:
            return Member_Name;
        case Token_K_namespace:
            return Member_NameSpace;
        case Token_K_fullname:
            return Member_FullName;
        case Token_K_type:
            return Member_Type;
        case Token_K_attrib:
            return Member_Attrib;
        case Token_K_version:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "version");
                return Member_None;
            }
            return Member_Version;
        case Token_K_majorversion:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "majorversion");
                return Member_None;
            }
            return Member_Major_Version;
        case Token_K_minorversion:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "minorversion");
                return Member_None;
            }
            return Member_Minor_Version;
        case Token_K_uuid:
            if (Object_Module != obj && Object_Interface != obj
                && Object_Parent != obj && Object_ClsIntf != obj
                && Object_ClsIntfAndParents != obj
                && Object_Class != obj && Object_Super != obj
                && Object_Aspect != obj && Object_Aggregate != obj) {
                ErrorReport(Lube_E_InvalidMember, "uuid");
                return Member_None;
            }
            return Member_Uuid;
        case Token_K_uunm:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "uunm");
                return Member_None;
            }
            return Member_Uunm;
        case Token_K_clsid:
            if (Object_Class != obj && Object_Super != obj
                && Object_Aspect != obj && Object_Aggregate != obj) {
                ErrorReport(Lube_E_InvalidMember, "clsid");
                return Member_None;
            }
            return Member_Clsid;
        case Token_K_carcode:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "carcode");
                return Member_None;
            }
            return Member_CarCode;
        case Token_K_builddate:
            if (Object_Module != obj) {
                ErrorReport(Lube_E_InvalidMember, "builddate");
                return Member_None;
            }
            return Member_BuildDate;
        case Token_K_iid:
            if (Object_Interface != obj && Object_Parent != obj
                && Object_ClsIntf != obj && Object_ClsIntfAndParents != obj) {
                ErrorReport(Lube_E_InvalidMember, "iid");
                return Member_None;
            }
            return Member_Iid;
        case Token_K_value:
            if (Object_EnumMember != obj && Object_Const != obj && Object_IntfConst != obj) {
                ErrorReport(Lube_E_InvalidMember, "value");
                return Member_None;
            }
            return Member_Value;
        case Token_K_dimention:
            return Member_Dimention;
        default:
            assert(TRUE == FALSE);
            return Member_None;
    }
}

void ParseStringToken()
{
    int n = 0;
    char *s, buf[c_nStrBufSize];
    LubeToken token;
    ObjectType object;
    MemberType member;

    s = g_szCurrentToken;
    while (*s) {
        if (c_nEmbedPrefix != *s) {
            buf[n++] = *s++;
        }
        else {
            s++; // skip prefix char
            token = *(LubeToken *)s;
            s += sizeof(LubeToken); // skip token

            if (IsObjectToken(token)) {
                if (*s != '.' || *(s + 1) != c_nEmbedPrefix) {
                    ErrorReport(Lube_E_InvalidEmbed);
                    return;
                }
                s += 2;

                object = GetObjectType(token);
                if (Object_None == object) return;

                token = *(LubeToken *)s;
                s += sizeof(LubeToken); // skip token
            }
            else {
                object = s_pContext->m_curObject;
            }

            if (!IsMemberToken(token)) {
                ErrorReport(Lube_E_InvalidEmbed);
                return;
            }
            member = GetMemberType(object, token);
            if (Member_None == member) return;

            buf[n++] = c_nEmbedPrefix;
            buf[n++] = (char)object;
            buf[n++] = (char)member;
        }
    }
    buf[n] = 0;
    strcpy(g_szCurrentToken, buf);
}

inline LubeToken _GetToken()
{
    LubeToken token;

    token = s_pContext->GetToken();
    if (Token_string != token) return token;

    ParseStringToken();
    return Token_string;
}

inline LubeToken _PeekToken()
{
    return s_pContext->PeekToken();
}

// MEMBER      -> [ OBJECT s_dot ] <
//                k_type | k_name | k_attrib | k_version | k_uuid | k_uunm |
//                k_clsid | k_iid | k_value >
//
int P_Member(LubeToken token)
{
    ObjectType object;
    MemberType member;
    PSTATEDESC pDesc;

    if (IsObjectToken(token)) {
        object = GetObjectType(token);
        if (Object_None == object) object = s_pContext->m_curObject;

        if (_PeekToken() != Token_S_dot) {
            ErrorReport(Lube_E_InvalidMember, g_szCurrentToken);
            return Ret_Continue;
        }
        _GetToken(); // skip s_dot
        token = _GetToken();
    }
    else {
        object = s_pContext->m_curObject;
    }

    member = GetMemberType(object, token);
    if (Member_None == member) return Ret_Continue;

    pDesc = s_pContext->CreateBlockette(State_Member);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    pDesc->object = object;
    pDesc->member = member;

    return Ret_Continue;
}

int GetMemberObject(LubeToken token, PSTATEDESC pDesc)
{
    if (IsObjectToken(token)) {
        pDesc->object = GetObjectType(token);
        if (Object_None == pDesc->object) {
            pDesc->object = s_pContext->m_curObject;
        }
        if (Object_LubeOption == pDesc->object) {
            pDesc->member = Member_Name;
            return Ret_Continue;
        }
        if (Object_CompilingEnv== pDesc->object) {
            pDesc->member = Member_Name;
            return Ret_Continue;
        }
        if (Object_InuptType== pDesc->object) {
            pDesc->member = Member_Name;
            return Ret_Continue;
        }

        if (_PeekToken() != Token_S_dot) return Ret_Continue;
        _GetToken(); // skip s_dot
        token = _GetToken();
    }
    else {
        pDesc->object = s_pContext->m_curObject;
    }

    if (IsMemberToken(token)) {
        pDesc->member = GetMemberType(pDesc->object, token);
    }
    else {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
    }
    return Ret_Continue;
}

// OUTPUT      -> k_output string { STATEMENT } k_end k_output
//
int P_Output()
{
    int nRet;
    PSTATEDESC pDesc;

    pDesc = s_pContext->CreateStatement(State_Output);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    if (_GetToken() != Token_string) {
        ErrorReport(Lube_E_ExpectSymbol, "output file name");
        return Ret_AbortOnError;
    }

    pDesc->uDataSize = strlen(g_szCurrentToken) + 1;
    pDesc->pvData = new char[pDesc->uDataSize];
    if (!pDesc->pvData) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    strcpy((char *)pDesc->pvData, g_szCurrentToken);

    while (_PeekToken() != Token_K_end) {
        nRet = P_Statement();
        if (Ret_EndOfFile == nRet) {
            ErrorReport(Lube_E_UnexpectEOF);
            return Ret_AbortOnError;
        }
        if (Ret_Continue != nRet) return nRet;
    }
    _GetToken(); // skip k_end

    if (_GetToken() != Token_K_output) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
    }
    s_pContext->LeaveStatement();

    return Ret_Continue;
}

// TEMPLATE    -> k_template string
//
int P_Template()
{
    int nRet;
    PSTATEDESC pDesc;
    ObjectType object;

    pDesc = s_pContext->CreateStatement(State_Template);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    if (_GetToken() != Token_string) {
        ErrorReport(Lube_E_ExpectSymbol, "template name");
        return Ret_AbortOnError;
    }

    nRet = FindTemplate(s_pContext->m_pLube, g_szCurrentToken);
    if (nRet < 0) {
        nRet = DoCompiling(g_szCurrentToken);
    }
    if (nRet >= 0) {
        pDesc->dwExtra = nRet;
        object = s_pContext->m_pLube->ppTemplates[nRet]->tRoot.object;
        if (Object_Class == object) {
            if (s_pContext->m_nClass > 0) {
                pDesc->object = Object_Class;
            }
            else {
                ErrorReport(Lube_E_NoValidObject, "class");
            }
        }
        else if (Object_Interface == object) {
            if (s_pContext->m_nInterface > 0) {
                pDesc->object = Object_Interface;
            }
            else {
                ErrorReport(Lube_E_NoValidObject, "interface");
            }
        }
    }

    s_pContext->LeaveStatement();
    return Ret_Continue;
}

// FUNC        -> ident s_lparen [ MEMBER | OBJECT | string ] s_rparen
//
int P_Func(const char *pszFunc, PSTATEDESC pDesc)
{
    int nIndex, nRet;
    LubeToken token;

    if (_PeekToken() != Token_S_lparen) {
        ErrorReport(Lube_E_UnexpectSymbol, pszFunc);
        return Ret_Continue;
    }

    nIndex = GetUserFuncIndex(pszFunc);
    if (nIndex < 0) {
        ErrorReport(Lube_E_UndefFunc, pszFunc);
        return Ret_AbortOnError;
    }
    _GetToken(); // skip s_lparen

    if (NULL == pDesc) {
        pDesc = s_pContext->CreateBlockette(State_Func);
        if (!pDesc) {
            ErrorReport(Lube_E_OutOfMemory);
            return Ret_AbortOnError;
        }
    }
    pDesc->dwExtra = (DWORD)nIndex;

    token = _GetToken();
    if (Token_string == token) {
        pDesc->uDataSize = strlen(g_szCurrentToken) + 1;
        pDesc->pvData = new char[pDesc->uDataSize];
        if (!pDesc->pvData) {
            ErrorReport(Lube_E_OutOfMemory);
            return Ret_AbortOnError;
        }
        strcpy((char *)pDesc->pvData, g_szCurrentToken);
        token = _GetToken();
    }
    else if (IsKeywordToken(token)) {
        nRet = GetMemberObject(token, pDesc);
        if (Ret_Continue != nRet) return nRet;
        token = _GetToken();
    }

    if (CheckUserFuncArg(nIndex, pDesc) < 0) {
        ErrorReport(Lube_E_InvalidFuncArg);
    }
    if (Token_S_rparen != token) {
        ErrorReport(Lube_E_ExpectSymbol, ")");
    }
    return Ret_Continue;
}

// WITH        -> k_with [ k_all | k_first | k_last ]
//                  OBJECT k_do { STATEMENT } k_end k_do
//
int P_With()
{
    StatementType type;
    LubeToken token;
    PSTATEDESC pDesc;
    ObjectType origObject = Object_None;
    DWORD dwCondition = 0;
    int nRet;

    token = _GetToken();
    if (Token_K_all == token) {
        type = State_WithAll;
        token = _GetToken();
    }
    else {
        if (Token_K_first == token) {
            dwCondition = Condition_First;
            token = _GetToken();
        }
        else if (Token_K_last == token) {
            dwCondition = Condition_Last;
            token = _GetToken();
        }
        type = State_With;
    }

    pDesc = s_pContext->CreateStatement(type);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory, g_szCurrentToken);
        return Ret_AbortOnError;
    }
    pDesc->dwCondition = dwCondition;

    if (type == State_WithAll || 0 != dwCondition) {
        pDesc->object = GetConditionObjectType(token);
    }
    else {
        pDesc->object = GetObjectType(token);
    }
    if (Object_None != pDesc->object) {
        origObject = s_pContext->EnterObject(pDesc->object);
    }

    if (_GetToken() != Token_K_do) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }
    while (_PeekToken() != Token_K_end) {
        nRet = P_Statement();
        if (Ret_EndOfFile == nRet) {
            ErrorReport(Lube_E_UnexpectEOF);
            return Ret_AbortOnError;
        }
        if (Ret_Continue != nRet) return nRet;
    }
    _GetToken(); // skip k_end

    if (_GetToken() != Token_K_do) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
    }
    s_pContext->LeaveStatement(); // leave with statement

    if (pDesc->object != Object_None) {
        s_pContext->LeaveObject(origObject);
    }
    return Ret_Continue;
}

// CONDITION   -> OBJCOND | MEMCOND | FUNCOND
// MEMCOND     -> MEMBER k_is [ k_not ] string
// OBJCOND     -> OBJECT k_is [ k_not ] < k_first | k_last >
// FUNCOND     -> FUNC k_is [ k_not ] < k_true | k_false >
//
int P_Condition(PSTATEDESC pDesc)
{
    LubeToken token;
    int nRet;

    token = _GetToken();
    if (Token_ident == token) {
        nRet = P_Func(g_szCurrentToken, pDesc);
        if (Ret_Continue != nRet) return nRet;
        pDesc->dwCondition |= Condition_Function;
    }
    else {
        nRet = GetMemberObject(token, pDesc);
        if (Ret_Continue != nRet) return nRet;
    }

    if (Token_K_is != _GetToken()) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }
    token = _GetToken();
    if (Token_K_not == token) {
        pDesc->dwCondition |= Condition_Not;
        token = _GetToken();
    }

    if (Condition_Function & pDesc->dwCondition) {
        switch (token) {
            case Token_K_true:
                pDesc->dwCondition |= Condition_True;
                break;
            case Token_K_false:
                pDesc->dwCondition |= Condition_False;
                break;
            default:
                ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
            case Token_error:
                return Ret_AbortOnError;
        }
        return Ret_Continue;
    }
    else if (Member_None == pDesc->member) {
        switch (token) {
            case Token_K_first:
                pDesc->dwCondition |= Condition_First;
                break;
            case Token_K_last:
                pDesc->dwCondition |= Condition_Last;
                break;
            default:
                ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
            case Token_error:
                return Ret_AbortOnError;
        }
        if (Object_Module == pDesc->object
            || Object_Parent == pDesc->object) {
            ErrorReport(Lube_E_HasNoOrder);
        }
        return Ret_Continue;
    }
    else if (Token_string != token) {
        if (Token_error != token) {
            ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
        }
        return Ret_AbortOnError;
    }

    return CheckMemberCondition(pDesc, g_szCurrentToken);
}

// IF          -> k_if CONDITION { < k_and | k_or > CONDITION } { STATEMENT }
//                [ k_else { STATEMENT } ] k_end k_if
//
int P_If()
{
    BOOL bElse = FALSE;
    int nRet;
    LubeToken token;
    PSTATEDESC pDesc, pNextDesc;

    pDesc = s_pContext->CreateStatement(State_If);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    pDesc = s_pContext->CreateBlockette(State_Condition);
    if (!pDesc) {
        ErrorReport(Lube_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    nRet = P_Condition(pDesc);
    if (nRet != Ret_Continue) return nRet;

    token = _PeekToken();
    while (Token_K_and == token || Token_K_or == token) {
        token = _GetToken(); // skip k_and or k_or
        pNextDesc = s_pContext->CreateBlockette(State_Condition);
        if (!pDesc) {
            ErrorReport(Lube_E_OutOfMemory);
            return Ret_AbortOnError;
        }
        nRet = P_Condition(pNextDesc);
        if (nRet != Ret_Continue) return nRet;

        pDesc->dwCondition |= (Token_K_and == token ?
                            Condition_And:Condition_Or);
        pDesc = pNextDesc;
        token = _PeekToken();
    }

    while (token != Token_K_end) {
        if (Token_K_else == token) {
            if (bElse) {
                ErrorReport(Lube_E_MultiElse);
            }
            else {
                pDesc = s_pContext->CreateBlockette(State_Else);
                if (!pDesc) {
                    ErrorReport(Lube_E_OutOfMemory);
                    return Ret_AbortOnError;
                }
                bElse = TRUE;
            }
            _GetToken(); // skip k_else
        }
        else {
            nRet = P_Statement();
            if (Ret_EndOfFile == nRet) {
                ErrorReport(Lube_E_UnexpectEOF);
                return Ret_AbortOnError;
            }
            if (Ret_Continue != nRet) return nRet;
        }
        token = _PeekToken();
    }
    _GetToken(); // skip k_end

    if (_GetToken() != Token_K_if) {
        ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
    }
    s_pContext->LeaveStatement(); // leave if statement

    return Ret_Continue;
}

int CreateTextBlockette()
{
    int size;
    char *pszText = NULL;
    PSTATEDESC pDesc = NULL;

    size = strlen(s_pContext->m_pTextBuffer) + 1;
    pszText = new char[size];
    if (!pszText) goto ErrorExit;
    pDesc = s_pContext->CreateBlockette(State_Text);
    if (!pDesc) goto ErrorExit;

    strcpy(pszText, s_pContext->m_pTextBuffer);
    *(s_pContext->m_pTextBuffer) = 0;
    pDesc->uDataSize = size;
    pDesc->pvData = (PVOID)pszText;
    return Ret_Continue;

ErrorExit:
    if (pszText) delete [] pszText;
    ErrorReport(Lube_E_OutOfMemory);
    return Ret_AbortOnError;
}

// STATEMENT   -> TEMPLATE | WITH | IF | FUNC | MEMBER | TEXT
//
int P_Statement()
{
    LubeToken token;
    token = _GetToken();

    switch (token) {
        case Token_text:
            return CreateTextBlockette();
        case Token_K_output:
            return P_Output();
        case Token_K_template:
            return P_Template();
        case Token_K_with:
            return P_With();
        case Token_K_if:
            return P_If();
        case Token_ident:
            return P_Func(g_szCurrentToken, NULL);
        case Token_eof:
            if (0 != *s_pContext->m_pTextBuffer) CreateTextBlockette();
            return Ret_EndOfFile;
        case Token_string:
            ErrorReport(Lube_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_Continue;
        case Token_error:
            return Ret_AbortOnError;
        default:
            return P_Member(token);
    }
}

// LUBE        -> INPUT { STATMENT }
// INPUT       -> k_input < k_module | k_class | k_interface >
//
int P_Lube()
{
    int nRet;
    LubeToken token;

    if (_GetToken() != Token_K_input) {
        ErrorReport(Lube_E_ExpectSymbol, "input");
        return Ret_AbortOnError;
    }
    switch (token = _GetToken()) {
        case Token_K_module:
            s_pContext->m_curObject = Object_Module;
            break;
        case Token_K_class:
            s_pContext->m_curObject = Object_Class;
            s_pContext->m_nClass++;
            break;
        case Token_K_interface:
            s_pContext->m_curObject = Object_Interface;
            s_pContext->m_nInterface++;
            break;
        default:
            ErrorReport(Lube_E_InvalidInput, g_szCurrentToken);
        case Token_error:
            return Ret_AbortOnError;
    }
    s_pContext->m_pCurrentState->object = s_pContext->m_curObject;

    while ((nRet = P_Statement()) == Ret_Continue);

    if (Token_K_class == token) {
        s_pContext->m_nClass--;
    }
    else if (Token_K_interface == token) {
        s_pContext->m_nInterface--;
    }
    return nRet;
}

void CurrentTemplatePath(const char *pszPath, char *pszDest)
{
    int n = strlen(pszPath) - 1;

    while (n >= 0) {
        if (IS_PATH_SEPARATOR(*(pszPath + n))) {
            memcpy(pszDest, pszPath, n);
            pszDest[n] = 0;
            break;
        }
        n--;
    }
}

int DoCompiling(const char *pFileName)
{
    int nRet;
    char szPath[_MAX_PATH];
    FILE *pFile, *pOrigFile;
    PSTATEDESC pOrigState;
    ObjectType origObject;
    PVOID pvTokenContext, pvErrorContext;

    if (!s_pContext->m_pLube) {
        s_pContext->m_pLube = CreateLube();
        if (!s_pContext->m_pLube) {
            ErrorReport(Lube_E_OutOfMemory);
            return LUBE_FAIL;
        }
    }
    nRet = CreateTemplate(s_pContext->m_pLube, pFileName);
    if (nRet < 0) {
        ErrorReport(Lube_E_OutOfMemory);
        return nRet;
    }

    if (SearchFileFromPath(s_szCurrentPath, pFileName, szPath) < 0) {
        if (SearchFileFromPath(s_pszSourceDirList, pFileName, szPath) < 0) {
            ErrorReport(Lube_E_OpenFile, pFileName);
            return LUBE_FAIL;
        }
    }
    pFileName = szPath;
    CurrentTemplatePath(szPath, s_szCurrentPath);

    pFile = fopen(pFileName, "rt");
    if (!pFile) {
        ErrorReport(Lube_E_OpenFile, pFileName);
        return LUBE_FAIL;
    }

    pvTokenContext = SaveTokenContext();
    pvErrorContext = SaveErrorContext();
    pOrigState = s_pContext->m_pCurrentState;
    origObject = s_pContext->m_curObject;
    pOrigFile = s_pContext->m_pFile;

    TokenInit();
    ErrorReporterInit(pFileName);
    s_pContext->m_pCurrentState = \
                &s_pContext->m_pLube->ppTemplates[nRet]->tRoot;
    s_pContext->m_pFile = pFile;

    P_Lube();

    s_pContext->m_pFile = pOrigFile;
    s_pContext->m_curObject = origObject;
    s_pContext->m_pCurrentState = pOrigState;
    RestoreErrorContext(pvErrorContext);
    RestoreTokenContext(pvTokenContext);

    fclose(pFile);
    return nRet;
}

void InitParser(ParserContext *pContext, const char *pszSourcePath)
{
    if (pszSourcePath) {
        s_pszSourceDirList = pszSourcePath;
    }
    s_pContext = pContext;
}
