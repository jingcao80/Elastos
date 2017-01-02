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
#include <ctype.h>

inline BOOL LibraryCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pLibrary == pCtx->m_pFirstLibrary;
    }
    return pCtx->m_pLibrary == pCtx->m_pLastLibrary;
}

inline BOOL ClassCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pClass == pCtx->m_pFirstClass;
    }
    return pCtx->m_pClass == pCtx->m_pLastClass;
}

inline BOOL SuperCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pClassParent== pCtx->m_pFirstClass;
    }
    return pCtx->m_pClassParent == pCtx->m_pLastClass;
}

inline BOOL InterfaceCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pInterface == pCtx->m_pFirstInterface;
    }
    return pCtx->m_pInterface == pCtx->m_pLastInterface;
}

inline BOOL StructCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pStruct == pCtx->m_pFirstStruct;
    }
    return pCtx->m_pStruct == pCtx->m_pLastStruct;
}

inline BOOL EnumCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pEnum == pCtx->m_pFirstEnum;
    }
    return pCtx->m_pEnum == pCtx->m_pLastEnum;
}

inline BOOL TypedefCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pTypedef == pCtx->m_pFirstTypedef;
    }
    return pCtx->m_pTypedef == pCtx->m_pLastTypedef;
}

inline BOOL InterfaceConstCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pInterfaceConst == pCtx->m_pInterface->mDesc->mConsts[0];
    }
    return pCtx->m_pInterfaceConst == pCtx->m_pInterface->mDesc->mConsts
                [pCtx->m_pInterface->mDesc->mConstCount - 1];
}

inline BOOL MethodCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pMethod == pCtx->m_pInterface->mDesc->mMethods[0];
    }
    return pCtx->m_pMethod == pCtx->m_pInterface->mDesc->mMethods
                [pCtx->m_pInterface->mDesc->mMethodCount - 1];
}

inline BOOL ParamCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pParam == pCtx->m_pMethod->mParams[0];
    }
    return pCtx->m_pParam == pCtx->m_pMethod->mParams
                [pCtx->m_pMethod->mParamCount - 1];
}

void FirstLastClsIntf(
    CLSModule *pModule, ClassDescriptor *pClass,
    ClassInterface **ppFirst, ClassInterface **ppLast)
{
    int n;
    ClassInterface *pFirst, *pLast;

    pFirst = pLast = NULL;

    for (n = 0; n < pClass->mInterfaceCount; n++) {
        if (!(pClass->mInterfaces[n]->mAttribs
            & ClassInterfaceAttrib_callback)) {
            pLast = pClass->mInterfaces[n];
            if (NULL == pFirst) pFirst = pLast;
        }
    }
    *ppLast = pLast;
    *ppFirst = pFirst;
}

inline BOOL ClsIntfCondition(PLUBECTX pCtx, BOOL bFirst)
{
    ClassInterface *pFirst, *pLast;

    FirstLastClsIntf(pCtx->m_pModule, pCtx->m_pClass->mDesc, &pFirst, &pLast);

    if (bFirst) {
        return pCtx->m_pClsIntf == pFirst;
    }
    return pCtx->m_pClsIntf == pLast;
}

inline BOOL AspectCondition(PLUBECTX pCtx, BOOL bFirst)
{
    ClassDescriptor *pDesc;

    pDesc = pCtx->m_pClass->mDesc;
    if (bFirst) {
        return pCtx->m_pAspect == pCtx->m_pModule->mClassDirs
                [pDesc->mAspectIndexes[0]];
    }
    return pCtx->m_pAspect == pCtx->m_pModule->mClassDirs
                [pDesc->mAspectIndexes[pDesc->mAspectCount - 1]];
}

inline BOOL AggregateCondition(PLUBECTX pCtx, BOOL bFirst)
{
    ClassDescriptor *pDesc;

    pDesc = pCtx->m_pClass->mDesc;
    if (bFirst) {
        return pCtx->m_pAggregate == pCtx->m_pModule->mClassDirs
                    [pDesc->mAggrIndexes[0]];
    }
    return pCtx->m_pAggregate == pCtx->m_pModule->mClassDirs
                    [pDesc->mAggrIndexes[pDesc->mAggregateCount - 1]];
}

inline BOOL ClassForAspectCondition(PLUBECTX pCtx, BOOL bFirst)
{
    ClassDescriptor *pDesc;

    pDesc = pCtx->m_pClass->mDesc;
    if (bFirst) {
        return pCtx->m_pForClass== pCtx->m_pModule->mClassDirs
                    [pDesc->mClassIndexes[0]];
    }
    return pCtx->m_pForClass == pCtx->m_pModule->mClassDirs
                    [pDesc->mClassIndexes[pDesc->mClassCount - 1]];
}

inline BOOL StcMemberCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pStructMember == pCtx->m_pStruct->mDesc->mElements[0];
    }
    return pCtx->m_pStructMember == pCtx->m_pStruct-> \
                mDesc->mElements[pCtx->m_pStruct->mDesc->mElementCount - 1];
}

inline BOOL EnumMemberCondition(PLUBECTX pCtx, BOOL bFirst)
{
    if (bFirst) {
        return pCtx->m_pEnumMember == pCtx->m_pEnum->mDesc->mElements[0];
    }
    return pCtx->m_pEnumMember == pCtx->m_pEnum-> \
                mDesc->mElements[pCtx->m_pEnum->mDesc->mElementCount - 1];
}

BOOL ObjectConditionValue(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    assert((pDesc->dwCondition & Condition_First)
            || (pDesc->dwCondition & Condition_Last));

    BOOL bValue;
    BOOL bFirst = (pDesc->dwCondition & Condition_First);

    switch (pDesc->object) {
        case Object_Library:
            bValue = LibraryCondition(pCtx, bFirst);
            break;
        case Object_Class:
            bValue = ClassCondition(pCtx, bFirst);
            break;
        case Object_Super:
            bValue = SuperCondition(pCtx, bFirst);
            break;
        case Object_Aspect:
            bValue = AspectCondition(pCtx, bFirst);
            break;
        case Object_Aggregate:
            bValue = AggregateCondition(pCtx, bFirst);
            break;
        case Object_ClassForAspect:
            bValue = ClassForAspectCondition(pCtx, bFirst);
            break;
        case Object_Interface:
            bValue = InterfaceCondition(pCtx, bFirst);
            break;
        case Object_Struct:
            bValue = StructCondition(pCtx, bFirst);
            break;
        case Object_Enum:
            bValue = EnumCondition(pCtx, bFirst);
            break;
        case Object_Typedef:
            bValue = TypedefCondition(pCtx, bFirst);
            break;
        case Object_ClassMethod:
            bValue = FALSE;
            break;
        case Object_IntfConst:
            bValue = InterfaceConstCondition(pCtx, bFirst);
            break;
        case Object_IntfMethod:
            bValue = MethodCondition(pCtx, bFirst);
            break;
        case Object_Param:
            bValue = ParamCondition(pCtx, bFirst);
            break;
        case Object_ClsIntf:
            bValue = ClsIntfCondition(pCtx, bFirst);
            break;
        case Object_StcMember:
            bValue = StcMemberCondition(pCtx, bFirst);
            break;
        case Object_EnumMember:
            bValue = EnumMemberCondition(pCtx, bFirst);
            break;
        default:
            assert(TRUE == FALSE);
            return FALSE;
    }
    return bValue;
}

BOOL NameConditionValue(PLUBECTX pCtx, ObjectType object, const char *pszDest)
{
    const char *pszSource;
    BOOL retVal;

    switch (object) {
        case Object_Module:
            pszSource = pCtx->m_pModule->mName;
            break;
        case Object_Library:
            pszSource = pCtx->m_pLibrary;
            while (*pszSource) {
                if (toupper(*pszSource) != toupper(*pszDest)) break;
                pszSource++;
                pszDest++;
            }
            return ((0 == *pszSource || '.' == *pszSource) && 0 == *pszDest);
        case Object_Class:
            pszSource = pCtx->m_pClass->mName;
            break;
        case Object_Aspect:
            pszSource = pCtx->m_pAspect->mName;
            break;
        case Object_Aggregate:
            pszSource = pCtx->m_pAggregate->mName;
            break;
        case Object_ClassForAspect:
            pszSource = pCtx->m_pForClass->mName;
            break;
        case Object_ClsIntf:
        case Object_Interface:
            pszSource = pCtx->m_pInterface->mName;
            break;
        case Object_Struct:
            pszSource = pCtx->m_pStruct->mName;
            break;
        case Object_Enum:
            pszSource = pCtx->m_pEnum->mName;
            break;
        case Object_Typedef:
            pszSource = pCtx->m_pTypedef->mName;
            break;
        case Object_IntfConst:
            pszSource = pCtx->m_pInterfaceConst->mName;
            break;
        case Object_ClassMethod:
        case Object_IntfMethod:
            pszSource = pCtx->m_pMethod->mName;
            break;
        case Object_Param:
            pszSource = pCtx->m_pParam->mName;
            break;
        case Object_StcMember:
            pszSource = pCtx->m_pStructMember->mName;
            break;
        case Object_EnumMember:
            pszSource = pCtx->m_pEnumMember->mName;
            break;
        case Object_Parent:
            pszSource = pCtx->m_pIntfParent->mName;
            break;
        case Object_Super:
            pszSource = pCtx->m_pClassParent->mName;
            break;
        case Object_LubeOption:
            retVal = false;
            if (!retVal && (pCtx->m_dwOptions & LubeOpt_InKernel)) {
                retVal = !_stricmp("InKernel", pszDest);
            }
            if (!retVal && (pCtx->m_dwOptions & LubeOpt_UseCustomClassObject)) {
                retVal = !_stricmp("UseCustomClassObject", pszDest);
            }
            if (!retVal && (pCtx->m_dwOptions & LubeOpt_HaveSandwichMethod)) {
                retVal = !_stricmp("HaveSandwichMethod", pszDest);
            }
            if (!retVal && (pCtx->m_dwOptions & LubeOpt_UseNakedMode)) {
                retVal = !_stricmp("UseNakedMode", pszDest);
            }
            if (!retVal && (pCtx->m_dwOptions & LubeOpt_WeakRef)) {
                retVal = !_stricmp("WeakReference", pszDest);
            }
            if (!retVal) {
                retVal = !_stricmp("", pszDest);
            }
            return retVal;
        case Object_InuptType:
            if (pCtx->m_dwOptions & LubeOpt_FromExCls) {
                pszSource = "ExCls";
            }
            else pszSource = "Dll";
            break;
        case Object_CompilingEnv:
            if (pCtx->m_dwOptions & LubeOpt_InSDK) {
                pszSource = "InSDK";
            }
            else pszSource = "";
            break;
        default:
            assert(TRUE == FALSE);
            return FALSE;
    }
    return !_stricmp(pszSource, pszDest);
}

BOOL TypeConditionValue(PLUBECTX pCtx, ObjectType object, const char *pszDest)
{
    TypeDescriptor *pType, type;
    const char *pszSource;

    switch (object) {
        case Object_Library:
            pszSource = pCtx->m_pLibrary + strlen(pCtx->m_pLibrary);
            while (pszSource != pCtx->m_pLibrary && *(pszSource - 1) != '.') {
                pszSource--;
            }
            return !_stricmp(pszSource, pszDest);
        case Object_Typedef:
            pType = &pCtx->m_pTypedef->mType;
            break;
        case Object_IntfConst:
        {
            if (pCtx->m_pInterfaceConst->mType == TYPE_BOOLEAN)
                return !_stricmp("Boolean", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_CHAR32)
                return !_stricmp("Char32", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_BYTE)
                return !_stricmp("Byte", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_INTEGER16)
                return !_stricmp("Int16", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_INTEGER32)
                return !_stricmp("Int32", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_INTEGER64)
                return !_stricmp("Int64", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_FLOAT)
                return !_stricmp("Float", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_DOUBLE)
                return !_stricmp("Double", pszDest);
            else if (pCtx->m_pInterfaceConst->mType == TYPE_STRING)
                return !_stricmp("String", pszDest);
            assert(TRUE == FALSE);
            return FALSE;
        }
        case Object_ClassMethod:
        case Object_IntfMethod:
            pType = &pCtx->m_pMethod->mType;
            break;
        case Object_Param:
            pType = &pCtx->m_pParam->mType;
            break;
        case Object_StcMember:
            pType = &pCtx->m_pStructMember->mType;
            break;
        default:
            assert(TRUE == FALSE);
            return FALSE;
    }

    GetOriginalType(pCtx->m_pModule, pType, &type);
    pszSource = Type2CString(pCtx->m_pModule, &type);

    return !_stricmp(pszSource, pszDest);
}

BOOL StringConditionValue(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    char szBuf[c_nStrBufSize];

    if (pCtx->ParseStringEmbed((char *)pDesc->pvData, szBuf) < 0) {
        return FALSE;
    }

    switch (pDesc->member) {
        case Member_Name:
            return NameConditionValue(pCtx, pDesc->object, szBuf);
        case Member_Type:
            return TypeConditionValue(pCtx, pDesc->object, szBuf);
        default:
            assert(TRUE == FALSE);
            return FALSE;
    }
}

BOOL AttribConditionValue(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    assert(pDesc->dwExtra != 0);
    char* pszName;
    int nLength;
    switch (pDesc->object) {
        case Object_Module:
            return pCtx->m_pModule->mAttribs & pDesc->dwExtra;
        case Object_Class:
            return pCtx->m_pClass->mDesc->mAttribs & pDesc->dwExtra;
        case Object_Aspect:
            return pCtx->m_pAspect->mDesc->mAttribs & pDesc->dwExtra;
        case Object_Aggregate:
            return pCtx->m_pAggregate->mDesc->mAttribs & pDesc->dwExtra;
        case Object_ClassForAspect:
            return pCtx->m_pClass->mDesc->mAttribs & pDesc->dwExtra;
        case Object_ClsIntf:
        case Object_ClsIntfAndParents:
            return pCtx->m_pClsIntf->mAttribs & pDesc->dwExtra;
        case Object_Interface:
            return pCtx->m_pInterface->mDesc->mAttribs & pDesc->dwExtra;
        case Object_Param:
            return pCtx->m_pParam->mAttribs & pDesc->dwExtra;
        case Object_Super:
            return pCtx->m_pClassParent->mDesc->mAttribs & pDesc->dwExtra;
        case Object_Parent:
            return pCtx->m_pIntfParent->mDesc->mAttribs & pDesc->dwExtra;
        case Object_ClassMethod:
        case Object_IntfMethod:
            return pCtx->m_pMethod->mAttribs & pDesc->dwExtra;
        case Object_Typedef:
            return !pCtx->m_pTypedef->mIsDummyType == !pDesc->dwExtra;
        case Object_Enum: // callback enums
            pszName = pCtx->m_pEnum->mDesc->mElements[pCtx->m_pEnum->mDesc->mElementCount - 1]->mName;
            nLength = strlen(pszName);
            if (nLength > 14 && !strcmp(pszName + (nLength - 14), "Sink_MaxEvents")) {
                return TRUE;
            }
            return FALSE;
        default:
            assert(TRUE == FALSE);
            return FALSE;
    }
}

inline BOOL FuncConditionValue(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    BOOL bRet;

    DECL_STATEMENT(Func);
    bRet = (0 != CALL_STATEMENT(Func)(pCtx, pDesc));

    return (pDesc->dwCondition & Condition_True) ? bRet:!bRet;
}

BOOL ConditionValue(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    BOOL bValue;

    if (Condition_Function & pDesc->dwCondition) {
        bValue = FuncConditionValue(pCtx, pDesc);
    }
    else if (Member_None == pDesc->member) {
        bValue = ObjectConditionValue(pCtx, pDesc);
    }
    else if (pDesc->pvData) {
        bValue = StringConditionValue(pCtx, pDesc);
    }
    else {
        bValue = AttribConditionValue(pCtx, pDesc);
    }
    return (pDesc->dwCondition & Condition_Not) ? !bValue:bValue;
}

BOOL DecideCondition(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    assert(NULL != pDesc && State_Condition == pDesc->type);

    BOOL bTrue;

    bTrue = ConditionValue(pCtx, pDesc);
    if (bTrue) {
        if (pDesc->dwCondition & Condition_And) {
            bTrue = DecideCondition(pCtx, pDesc->pNext);
        }
    }
    else {
        while (pDesc->dwCondition & Condition_And) {
            pDesc = pDesc->pNext;
        }
        if (pDesc->dwCondition & Condition_Or) {
            bTrue = DecideCondition(pCtx, pDesc->pNext);
        }
    }
    return bTrue;
}

IMPL_STATEMENT(If)(PLUBECTX pCtx, PSTATEDESC pDesc)
{
    BOOL bTrue;
    StatementFn_t fn;

    pDesc = pDesc->pBlockette;
    bTrue = DecideCondition(pCtx, pDesc);

    // Skip conditions.
    //
    while (pDesc && State_Condition == pDesc->type) {
        pDesc = pDesc->pNext;
    }

    if (bTrue) {
        //
        // Exec to 'ELSE' or 'END IF' for true condition.
        //
        while (pDesc && State_Else != pDesc->type) {
            fn = GetStatementFunc(pDesc->uStateIndex);
            if ((*fn)(pCtx, pDesc) < 0) return LUBE_FAIL;
            pDesc = pDesc->pNext;
        }
    }
    else {
        //
        // Skip to 'ELSE' or 'END IF' for false condition.
        //
        while (pDesc && State_Else != pDesc->type) {
            pDesc = pDesc->pNext;
        }
        if (pDesc) pDesc = pDesc->pNext; // skip 'ELSE'

        while (pDesc) {
            fn = GetStatementFunc(pDesc->uStateIndex);
            if ((*fn)(pCtx, pDesc) < 0) return LUBE_FAIL;
            pDesc = pDesc->pNext;
        }
    }
    return LUBE_OK;
}
