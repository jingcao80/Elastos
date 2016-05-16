//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CIntrinsicInfo.h"
#include "CObjInfoList.h"
#include "refutil.h"

UInt32 CIntrinsicInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CIntrinsicInfo::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CIntrinsicInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CIntrinsicInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CIntrinsicInfo::CIntrinsicInfo(
    /* [in] */ CarDataType dataType,
    /* [in] */ UInt32 uSize)
{
    m_dataType = dataType;
    m_uSize = uSize;
    m_cRef = 0;
}

ECode CIntrinsicInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(g_cDataTypeList[m_dataType].name);
    return NOERROR;
}

ECode CIntrinsicInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    if (!m_uSize) {
        return E_INVALID_OPERATION;
    }

    if (!pSize) {
        return E_INVALID_ARGUMENT;
    }

    *pSize = m_uSize;
    return NOERROR;
}

ECode CIntrinsicInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = m_dataType;
    return NOERROR;
}
