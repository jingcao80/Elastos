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
