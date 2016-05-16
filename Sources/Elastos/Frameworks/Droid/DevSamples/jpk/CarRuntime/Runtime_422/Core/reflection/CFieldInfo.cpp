//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CFieldInfo.h"
#include "CObjInfoList.h"

UInt32 CFieldInfo::AddRef()
{
    return m_pStructInfo->AddRef();
}

UInt32 CFieldInfo::Release()
{
    return m_pStructInfo->Release();
}

PInterface CFieldInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IFieldInfo) {
        return (IFieldInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CFieldInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CFieldInfo::CFieldInfo(
    /* [in] */ IStructInfo * pStructInfo,
    /* [in] */ CString name,
    /* [in] */ IDataTypeInfo *pTypeInfo)
{
    m_pStructInfo = pStructInfo;
    m_sName = name;
    m_pTypeInfo = pTypeInfo;
    m_cRef = 0;
}

CFieldInfo::~CFieldInfo()
{
}

ECode CFieldInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(m_sName);
    return NOERROR;
}

ECode CFieldInfo::GetTypeInfo(
    /* [out] */ IDataTypeInfo ** ppTypeInfo)
{
    if (!ppTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    *ppTypeInfo = m_pTypeInfo;
    m_pTypeInfo->AddRef();
    return NOERROR;
}
