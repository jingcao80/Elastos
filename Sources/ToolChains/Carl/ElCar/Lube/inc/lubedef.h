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

#ifndef __LUBEDEF_H__
#define __LUBEDEF_H__

typedef unsigned char byte;
#include <clstype.h>

typedef void* PVOID;
typedef unsigned int UINT;

typedef enum MemberType {
    Member_None = 0,
    Member_Type,        Member_Name,        Member_Attrib,
    Member_Version,     Member_Uuid,        Member_Uunm,
    Member_Clsid,       Member_Iid,         Member_Value,
    Member_Dimention,   Member_Major_Version,   Member_Minor_Version,
    Member_CarCode,     Member_BuildDate,  Member_NameSpace,
    Member_FullName,
}   MemberType;

typedef enum ObjectType {
    Object_None = 0,
    Object_Module,      Object_Class,       Object_Interface,
    Object_Struct,      Object_Enum,        Object_Typedef,
    Object_Parent,      Object_Super,       Object_ClassMethod,
    Object_IntfConst,
    Object_IntfMethod,  Object_Param,       Object_StcMember,
    Object_EnumMember,  Object_ClsIntf,     Object_Aspect,
    Object_Aggregate,   Object_CParentMtd,  Object_IParentMtd,
    Object_Library,     Object_Const,       Object_IntfParentParent,
    Object_ClsIntfAndParents,               Object_ClassForAspect,
    Object_CbCoalescence,
    Object_LubeOption,  Object_CompilingEnv,Object_InuptType,
    Object_ClassConstructor,
    Object_Child,
}   ObjectType;

typedef enum StatementType {
    State_NoMean = 0,   State_Text = 1,
    State_Output,       State_Template,     State_With,
    State_WithAll,      State_If,           State_Else,
    State_Condition,    State_Func,         State_Member,
}   StatementType;

typedef struct StateDescriptor *PSTATEDESC;

typedef struct StateDescriptor {
    StatementType       type;
    UINT                uStateIndex;
    PSTATEDESC          pNext;
    PSTATEDESC          pBlockette;
    PSTATEDESC          pOwner;
    ObjectType          object;
    MemberType          member;
    UINT                uDataSize;
    PVOID               pvData;
    DWORD               dwCondition;
    DWORD               dwExtra;
}   StateDesc;

typedef enum ConditionAttrib {
    Condition_Not       = 0x00001,
    Condition_And       = 0x00002,
    Condition_Or        = 0x00004,

    Condition_First     = 0x01000,
    Condition_Last      = 0x02000,
    Condition_True      = Condition_First,
    Condition_False     = Condition_Last,

    Condition_Function  = 0x80000,
}   ConditionAttrib;

const int c_nEmbedPrefix = 0x1f;
const int c_nStrBufSize = 1024;

typedef struct LubeTemplate {
    char *              mName;
    StateDesc           tRoot;
}   LubeTemplate;

const int c_cMaxTemplates = 256;
const DWORD c_dwLubeMagic = 0x6562754c;

typedef struct LubeHeader {
    DWORD               dwMagic;
    int                 cTemplates;
    LubeTemplate **     ppTemplates;
}   LubeHeader;
typedef LubeHeader *PLUBEHEADER;

extern PLUBEHEADER CreateLube();
extern void DestroyLube(PLUBEHEADER);

extern void SetLubePath(const char *);
extern void SetOutputPath(const char *);
extern int SaveLube(PLUBEHEADER, const char *);
extern int LoadLube(const char *, PLUBEHEADER *);

extern int CreateTemplate(PLUBEHEADER, const char *);
extern int FindTemplate(PLUBEHEADER, const char *);

#define LUBE_OK         0
#define LUBE_FAIL       (-1)
#define LUBE_FALSE      1
#endif // __LUBEDEF_H__
