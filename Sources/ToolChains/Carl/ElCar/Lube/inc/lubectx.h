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

#ifndef __LUBECTX_H__
#define __LUBECTX_H__

class LubeContext
{
public:
    LubeContext(LubeHeader *, CLSModule *, DWORD dwOptions);

public:
    int PutString(const char *);
    int PutData(PVOID, int);

    int ExecTemplate(const char *);
    int ExecTemplate(int nIndex);
    int ExecStatements(PSTATEDESC);

    int EnterFile(const char *, FILE **);
    void LeaveFile(FILE *);

    int ParseStringEmbed(const char *, char *);
    int GetMemberString(ObjectType, MemberType, char *);

    void ErrMsg(const char *, const char * = NULL, const char * = NULL);
    void WarnMsg(const char *, const char * = NULL, const char * = NULL);

public:
    LubeHeader *        m_pHeader;
    CLSModule *         m_pModule;
    DWORD               m_dwOptions;

    FILE *              m_pFile;
    char                m_szFileName[c_nStrBufSize];
    char                m_szOption[c_nStrBufSize];
    LubeTemplate *      m_pTemplate;

    char *              m_pLibrary;
    ClassDirEntry *     m_pClass;
    ClassDirEntry *     m_pClassParent;
    ClassDirEntry *     m_pOrigClass;
    ClassDirEntry *     m_pAspect;
    ClassDirEntry *     m_pAggregate;
    ClassDirEntry *     m_pForClass;
    InterfaceDirEntry * m_pInterface;
    InterfaceDirEntry * m_pIntfParent;
    InterfaceDirEntry * m_pIntfChild;
    ClassInterface *    m_pClsIntf;
    StructDirEntry *    m_pStruct;
    EnumDirEntry *      m_pEnum;
    ConstDirEntry *     m_pConst;
    AliasDirEntry *     m_pTypedef;
    InterfaceConstDescriptor * m_pInterfaceConst;
    MethodDescriptor *  m_pMethod;
    ParamDescriptor *   m_pParam;
    StructElement *     m_pStructMember;
    EnumElement *       m_pEnumMember;

    char *              m_pFirstLibrary;
    char *              m_pLastLibrary;
    ClassDirEntry *     m_pFirstClass;
    ClassDirEntry *     m_pLastClass;
    InterfaceDirEntry * m_pFirstInterface;
    InterfaceDirEntry * m_pLastInterface;
    StructDirEntry *    m_pFirstStruct;
    StructDirEntry *    m_pLastStruct;
    EnumDirEntry *      m_pFirstEnum;
    EnumDirEntry *      m_pLastEnum;
    ConstDirEntry *     m_pFirstConst;
    ConstDirEntry *     m_pLastConst;
    AliasDirEntry *     m_pFirstTypedef;
    AliasDirEntry *     m_pLastTypedef;

private:
    int IntfParentMember(MemberType, char *);
    int IntfParentParentMember(MemberType, char *);
    int IntfChildMember(MemberType, char *);
    int AggregateMember(MemberType, char *);
    int AspectMember(MemberType, char *);
    int ClassForAspectMember(MemberType member, char *pszBuffer);
    int ClassParentMember(MemberType, char *);
    int EnumMemMember(MemberType, char *);
    int StructMemMember(MemberType, char *);
    int InterfaceConstMember(MemberType, char *);
    int ParamMember(MemberType, char *);
    int MethodMember(MemberType, char *);
    int TypedefMember(MemberType, char *);
    int EnumMember(MemberType, char *);
    int ConstMember(MemberType, char *);
    int StructMember(MemberType, char *);
    int InterfaceMember(MemberType, char *);
    int ClassMember(MemberType, char *);
    int LibraryMember(MemberType, char *);
    int ModuleMember(MemberType, char *);
};
typedef LubeContext *PLUBECONTEXT, *PLUBECTX;

typedef enum _LubeOption {
    LubeOpt_Redirect    = 0x0001,
    LubeOpt_Overwrite   = 0x0002,
    LubeOpt_InKernel    = 0x0004,
    LubeOpt_InSDK       = 0x0008,
    LubeOpt_FromExCls   = 0x0010,
    LubeOpt_UseCustomClassObject = 0x0020,
    LubeOpt_HaveSandwichMethod   = 0x0040,
    LubeOpt_UseNakedMode         = 0x0080,
    LubeOpt_WeakRef     = 0x0100,
}   _LubeOption;

#endif // __LUBECTX_H__
