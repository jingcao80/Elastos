//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __PARSER_H__
#define __PARSER_H__

#include <lube.h>

#include "token.h"
#include "parerr.h"

class ParserContext
{
public:
    ParserContext(char *pBuffer);
    ~ParserContext();

    LubeToken GetToken();
    LubeToken PeekToken();

    PSTATEDESC CreateBlockette(StatementType type);
    PSTATEDESC CreateStatement(StatementType type);
    void LeaveStatement();

    ObjectType EnterObject(ObjectType obj);
    void LeaveObject(ObjectType obj);

public:
    PLUBEHEADER     m_pLube;

    PSTATEDESC      m_pCurrentState;
    FILE *          m_pFile;
    ObjectType      m_curObject;

    int             m_nLibrary;
    int             m_nClass;
    int             m_nClsIntf;
    int             m_nAspect;
    int             m_nAggregate;
    int             m_nForClass;
    int             m_nInterface;
    int             m_nClassMethod;
    int             m_nIntfConst;
    int             m_nIntfMethod;
    int             m_nParameter;
    int             m_nStruct;
    int             m_nStructMember;
    int             m_nEnum;
    int             m_nConst;
    int             m_nEnumMember;
    int             m_nTypedef;

    char *          m_pTextBuffer;
};

inline LubeToken ParserContext::PeekToken()
{
    return ::PeekToken(m_pFile, m_pTextBuffer);
}

inline LubeToken ParserContext::GetToken()
{
    return ::GetToken(m_pFile, m_pTextBuffer);
}

extern int CheckUserFuncArg(int nFuncIndex, PSTATEDESC pDesc);
extern int CheckMemberCondition(PSTATEDESC pDesc, const char *pszConst);

extern void InitParser(ParserContext *pContext, const char *pszSourcePath);
extern int DoCompiling(const char *);

enum ParserReturnValue
{
    Ret_Continue        = 0,
    Ret_EndOfFile       = -1,
    Ret_AbortOnError    = -2,
};

#endif // __PARSER_H__
