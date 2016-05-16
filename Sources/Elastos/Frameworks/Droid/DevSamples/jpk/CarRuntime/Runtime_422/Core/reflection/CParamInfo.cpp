//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CParamInfo.h"
#include "CMethodInfo.h"
#include "CLocalPtrInfo.h"
#include "CLocalTypeInfo.h"

UInt32 CParamInfo::AddRef()
{
    return m_pMethodInfo->AddRef();
}

UInt32 CParamInfo::Release()
{
    return m_pMethodInfo->Release();
}

PInterface CParamInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IParamInfo) {
        return (IParamInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CParamInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CParamInfo::CParamInfo(
    /* [in] */ CClsModule *pCClsModule,
    /* [in] */ IMethodInfo *pMethodInfo,
    /* [in] */ ParmElement *pParmElement,
    /* [in] */ ParamDescriptor *pParamDescriptor,
    /* [in] */ Int32 iIndex)
{
    m_pCClsModule = pCClsModule;
    m_pMethodInfo = pMethodInfo;
    m_pParmElement = pParmElement;
    m_pParamDescriptor = pParamDescriptor;
    m_iIndex = iIndex;
    m_cRef = 0;
}

CParamInfo::~CParamInfo()
{
}

ECode CParamInfo::GetMethodInfo(
    /* [out] */ IMethodInfo ** ppMethodInfo)
{
    if (!ppMethodInfo) {
        return E_INVALID_ARGUMENT;
    }

    m_pMethodInfo->AddRef();
    *ppMethodInfo = m_pMethodInfo;

    return NOERROR;
}

ECode CParamInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(adjustNameAddr(m_pCClsModule->m_nBase, m_pParamDescriptor->pszName));
    return NOERROR;
}

ECode CParamInfo::GetIndex(
    /* [out] */ Int32 * pIndex)
{
    if (!pIndex) {
        return E_INVALID_ARGUMENT;
    }

    *pIndex = m_iIndex;
    return NOERROR;
}

ECode CParamInfo::GetIOAttribute(
    /* [out] */ ParamIOAttribute * pIOAttrib)
{
    if (!pIOAttrib) {
        return E_INVALID_ARGUMENT;
    }

    *pIOAttrib = m_pParmElement->attrib;

    return NOERROR;
}

ECode CParamInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CParamInfo::GetTypeInfo(
    /* [out] */ IDataTypeInfo ** ppTypeInfo)
{
    if (!ppTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pParmElement->type == CarDataType_LocalPtr) {
        return g_objInfoList.AcquireLocalPtrInfo(m_pCClsModule,
                        &m_pParamDescriptor->type,
                        m_pParmElement->pointer, (ILocalPtrInfo **)ppTypeInfo);
    }
    else {
        return g_objInfoList.AcquireDataTypeInfo(m_pCClsModule,
                        &m_pParamDescriptor->type, ppTypeInfo);
    }
}

ECode CParamInfo::GetAdvisedCapacity(
    /* [out] */ Int32 * pAdvisedCapacity)
{
    if (!pAdvisedCapacity) {
        return E_INVALID_ARGUMENT;
    }

    if (m_pParmElement->type != CarDataType_ArrayOf
        && m_pParmElement->type != CarDataType_BufferOf
        && m_pParmElement->type != CarDataType_MemoryBuf
        && m_pParmElement->type != CarDataType_StringBuf) {
        return E_INVALID_OPERATION;
    }

    if (m_pParamDescriptor->type.nSize <= 0) {
        //if the size of carquient isn't assigned, then it's -1;
        return E_INVALID_OPERATION;
    }

    *pAdvisedCapacity = m_pParamDescriptor->type.nSize;

    return NOERROR;
}

ECode CParamInfo::IsUsingTypeAlias(
    /* [out] */ Boolean * pUsingTypeAlias)
{
    if (!pUsingTypeAlias) {
        return E_INVALID_ARGUMENT;
    }

    UInt32 uIndex = m_pParamDescriptor->type.sIndex;
    if ((m_pParamDescriptor->type.type == Type_alias)
        && !IsSysAlaisType(m_pCClsModule, uIndex)) {
        *pUsingTypeAlias = TRUE;
    }
    else {
        *pUsingTypeAlias = FALSE;
    }

    return NOERROR;
}

ECode CParamInfo::GetUsedTypeAliasInfo(
    /* [out] */ ITypeAliasInfo ** ppUsedTypeAliasInfo)
{
    if (!ppUsedTypeAliasInfo) {
        return E_INVALID_ARGUMENT;
    }

    UInt32 uIndex = m_pParamDescriptor->type.sIndex;
    if ((m_pParamDescriptor->type.type != Type_alias) ||
        IsSysAlaisType(m_pCClsModule, uIndex)) {
        return E_DOES_NOT_EXIST;
    }

    *ppUsedTypeAliasInfo = NULL;
    return g_objInfoList.AcquireTypeAliasInfo(m_pCClsModule,
                getAliasDirAddr(m_pCClsModule->m_nBase,
                        m_pCClsModule->m_pClsMod->ppAliasDir, uIndex),
                (IInterface **)ppUsedTypeAliasInfo);
}
