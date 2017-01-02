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

#include <lube.h>

PVOID ModuleArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pModule;
        case Member_Type:
        case Member_Attrib:
            return (PVOID)&pCtx->m_pModule->mAttribs;
        case Member_Name:
            return (PVOID)pCtx->m_pModule->mName;
        case Member_Major_Version:
            return (PVOID)&pCtx->m_pModule->mMajorVersion;
        case Member_Minor_Version:
            return (PVOID)&pCtx->m_pModule->mMinorVersion;
        case Member_Version:
            return NULL; // version is not a true member, so it should be used as argument
        case Member_Uuid:
            return (PVOID)&pCtx->m_pModule->mUuid;
        case Member_Uunm:
            return (PVOID)pCtx->m_pModule->mUunm;
        case Member_CarCode:
            return (PVOID)pCtx->m_pModule->mChecksum;
        case Member_BuildDate:
            return (PVOID)pCtx->m_pModule->mBarcode;
        default:
            return NULL;
    }
}

PVOID LibraryArg(PLUBECTX pCtx, MemberType member)
{
    char *p;

    p = pCtx->m_pLibrary + strlen(pCtx->m_pLibrary);
    while (p != pCtx->m_pLibrary && !IS_PATH_SEPARATOR(*(p - 1)))
        p--;

    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pLibrary;
        case Member_Name:
            return p;
        case Member_Type:
            p = pCtx->m_pLibrary;
            while (0 != *p) {
                if ('.' == *p) return (PVOID)(p + 1);
                p++;
            }
        default:
            return NULL;
    }
}

PVOID ClassArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pClass;
        case Member_Type:
        case Member_Attrib:
            return (PVOID)&pCtx->m_pClass->mDesc->mAttribs;
        case Member_Name:
            return (PVOID)pCtx->m_pClass->mName;
        case Member_Uuid:
        case Member_Clsid:
            return (PVOID)&pCtx->m_pClass->mDesc->mClsid;
        default:
            return NULL;
    }
}

PVOID ClsIntfArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pClsIntf;
        case Member_Type:
            return (PVOID)&pCtx->m_pInterface->mDesc->mAttribs;
        case Member_Attrib:
            return (PVOID)&pCtx->m_pClsIntf->mAttribs;
        case Member_Name:
            return (PVOID)pCtx->m_pInterface->mName;
        case Member_Uuid:
        case Member_Iid:
            return (PVOID)&pCtx->m_pInterface->mDesc->mIID;
        default:
            return NULL;
    }
}

PVOID InterfaceArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pInterface;
        case Member_Type:
        case Member_Attrib:
            return (PVOID)&pCtx->m_pInterface->mDesc->mAttribs;
        case Member_Name:
            return (PVOID)pCtx->m_pInterface->mName;
        case Member_Uuid:
        case Member_Iid:
            return (PVOID)&pCtx->m_pInterface->mDesc->mIID;
        default:
            return NULL;
    }
}

PVOID StructArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pStruct;
        case Member_Name:
            return (PVOID)pCtx->m_pStruct->mName;
        default:
            return NULL;
    }
}

PVOID EnumArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pEnum;
        case Member_Name:
            return (PVOID)pCtx->m_pEnum->mName;
        default:
            return NULL;
    }
}

PVOID TypedefArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pTypedef;
        case Member_Name:
            return (PVOID)pCtx->m_pTypedef->mName;
        case Member_Type:
            return (PVOID)&pCtx->m_pTypedef->mType;
        case Member_Attrib:
            return (PVOID)&pCtx->m_pTypedef->mIsDummyType;
        default:
            return NULL;
    }
}

PVOID MethodArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pMethod;
        case Member_Name:
            return (PVOID)pCtx->m_pMethod->mName;
        case Member_Type:
            return (PVOID)&pCtx->m_pMethod->mType;
        case Member_Attrib:
            return (PVOID)&pCtx->m_pMethod->mAttribs;
        default:
            return NULL;
    }
}

PVOID ParamArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pParam;
        case Member_Name:
            return (PVOID)pCtx->m_pParam->mName;
        case Member_Type:
            return (PVOID)&pCtx->m_pParam->mType;
        case Member_Attrib:
            return (PVOID)&pCtx->m_pParam->mAttribs;
        default:
            return NULL;
    }
}

PVOID StructMemberArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pStructMember;
        case Member_Name:
            return (PVOID)pCtx->m_pStructMember->mName;
        case Member_Type:
            return (PVOID)&pCtx->m_pStructMember->mType;
        default:
            return NULL;
    }
}

PVOID EnumMemberArg(PLUBECTX pCtx, MemberType member)
{
    switch (member) {
        case Member_None:
            return (PVOID)pCtx->m_pEnumMember;
        case Member_Name:
            return (PVOID)pCtx->m_pEnumMember->mName;
        case Member_Value:
            return (PVOID)&pCtx->m_pEnumMember->mValue;
        default:
            return NULL;
    }
}

PVOID ClassParentArg(PLUBECTX pCtx, MemberType member)
{
    ClassDirEntry *pOrigClass;
    PVOID pvArg;

    if (!(pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_hasparent)) {
        return NULL;
    }

    pOrigClass = pCtx->m_pClass;
    pCtx->m_pClass = pCtx->m_pClassParent;

    pvArg = ClassArg(pCtx, member);

    pCtx->m_pClass = pOrigClass;
    return pvArg;
}

PVOID AspectArg(PLUBECTX pCtx, MemberType member)
{
    ClassDirEntry *pOrigClass;
    PVOID pvArg;

    pOrigClass = pCtx->m_pClass;
    pCtx->m_pClass = pCtx->m_pAspect;

    pvArg = ClassArg(pCtx, member);

    pCtx->m_pClass = pOrigClass;
    return pvArg;
}

PVOID AggregateArg(PLUBECTX pCtx, MemberType member)
{
    ClassDirEntry *pOrigClass;
    PVOID pvArg;

    pOrigClass = pCtx->m_pClass;
    pCtx->m_pClass = pCtx->m_pAggregate;

    pvArg = ClassArg(pCtx, member);

    pCtx->m_pClass = pOrigClass;
    return pvArg;
}

PVOID ClassForAspectArg(PLUBECTX pCtx, MemberType member)
{
    ClassDirEntry *pOrigClass;
    PVOID pvArg;

    pOrigClass = pCtx->m_pClass;
    pCtx->m_pClass = pCtx->m_pForClass;

    pvArg = ClassArg(pCtx, member);

    pCtx->m_pClass = pOrigClass;
    return pvArg;
}

PVOID IntfParentArg(PLUBECTX pCtx, MemberType member)
{
    InterfaceDirEntry *pOrigInterface;
    PVOID pvArg;

    pOrigInterface = pCtx->m_pInterface;
    pCtx->m_pInterface = pCtx->m_pIntfParent;

    pvArg = InterfaceArg(pCtx, member);

    pCtx->m_pInterface = pOrigInterface;
    return pvArg;
}

PVOID GetFunctionArg(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    switch (pDesc->object) {
        case Object_Module:
            return ModuleArg(pCtx, pDesc->member);
        case Object_Library:
            return LibraryArg(pCtx, pDesc->member);
        case Object_Class:
            return ClassArg(pCtx, pDesc->member);
        case Object_ClsIntf:
        case Object_ClsIntfAndParents:
            return ClsIntfArg(pCtx, pDesc->member);
        case Object_Aspect:
            return AspectArg(pCtx, pDesc->member);
        case Object_Aggregate:
            return AggregateArg(pCtx, pDesc->member);
        case Object_ClassForAspect:
            return ClassForAspectArg(pCtx, pDesc->member);
        case Object_Interface:
            return InterfaceArg(pCtx, pDesc->member);
        case Object_Struct:
            return StructArg(pCtx, pDesc->member);
        case Object_Enum:
            return EnumArg(pCtx, pDesc->member);
        case Object_Typedef:
            return TypedefArg(pCtx, pDesc->member);
        case Object_IntfMethod:
        case Object_ClassMethod:
            return MethodArg(pCtx, pDesc->member);
        case Object_Param:
            return ParamArg(pCtx, pDesc->member);
        case Object_StcMember:
            return StructMemberArg(pCtx, pDesc->member);
        case Object_EnumMember:
            return EnumMemberArg(pCtx, pDesc->member);
        case Object_Super:
            return ClassParentArg(pCtx, pDesc->member);
        case Object_Parent:
            return IntfParentArg(pCtx, pDesc->member);
        default:
            assert(TRUE == FALSE);
            return NULL;
    }
}

IMPL_STATEMENT(Func)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    char szBuf[c_nStrBufSize];
    PVOID pvArg;
    UserFn_t fn;

    if (pDesc->pvData) {
        if (pCtx->ParseStringEmbed((char *)pDesc->pvData, szBuf) < 0) {
            return LUBE_OK;
        }
        pvArg = (PVOID)szBuf;
    }
    else {
        pvArg = GetFunctionArg(pCtx, pDesc);
        if (!pvArg) return LUBE_OK;
    }

    fn = GetUserFunction(pDesc->dwExtra);
    return (*fn)(pCtx, pDesc, pvArg);
}
