//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CEnumItemInfo.h"
#include "CEnumInfo.h"

UInt32 CEnumItemInfo::AddRef()
{
    return m_pEnumInfo->AddRef();
}

UInt32 CEnumItemInfo::Release()
{
    return m_pEnumInfo->Release();
}

PInterface CEnumItemInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IEnumItemInfo) {
        return (IEnumItemInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CEnumItemInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CEnumItemInfo::CEnumItemInfo(
    /* [in] */ IEnumInfo * pEnumInfo,
    /* [in] */ CString name,
    /* [in] */ Int32 value)
{
    m_pEnumInfo = pEnumInfo;
    m_sName = name;
    m_iValue = value;
    m_cRef = 0;
}

CEnumItemInfo::~CEnumItemInfo()
{
}

ECode CEnumItemInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(m_sName);
    return NOERROR;
}

ECode CEnumItemInfo::GetEnumInfo(
    /* [out] */ IEnumInfo ** ppEnumInfo)
{
    if (!ppEnumInfo) {
        return E_INVALID_ARGUMENT;
    }

    m_pEnumInfo->AddRef();
    *ppEnumInfo = m_pEnumInfo;
    return NOERROR;
}

ECode CEnumItemInfo::GetValue(
    /* [out] */ Int32 * pValue)
{
    if (!pValue) {
        return E_INVALID_ARGUMENT;
    }

    *pValue = m_iValue;
    return NOERROR;
}
