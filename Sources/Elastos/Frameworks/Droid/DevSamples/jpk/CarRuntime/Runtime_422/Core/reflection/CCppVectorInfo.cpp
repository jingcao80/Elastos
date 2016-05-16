//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCppVectorInfo.h"
#include "CObjInfoList.h"
#include "CStructInfo.h"

UInt32 CCppVectorRefInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CCppVectorRefInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_DataType);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.DetachCppVectorInfo(this);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_DataType);
    assert(nRef >= 0);
    return nRef;
}

PInterface CCppVectorRefInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ICppVectorInfo) {
        return (ICppVectorInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CCppVectorRefInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CCppVectorRefInfo::CCppVectorRefInfo(
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ Int32 length)
{
    m_pElementTypeInfo = pElementTypeInfo;
    m_pElementTypeInfo->AddRef();
    pElementTypeInfo->GetSize(&m_iSize);
    m_iLength = length;
    m_iSize *= m_iLength;
    m_cRef = 0;
}

CCppVectorRefInfo::~CCppVectorRefInfo()
{
    if (m_pElementTypeInfo) m_pElementTypeInfo->Release();
}

ECode CCppVectorRefInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = m_pElementTypeInfo->GetName(pName);
    if (FAILED(ec)) return ec;

    pName->Append("Vector");

    return NOERROR;
}

ECode CCppVectorRefInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    if (!pSize) {
        return E_INVALID_ARGUMENT;
    }

    *pSize = m_iSize;

    return NOERROR;
}

ECode CCppVectorRefInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = CarDataType_CppVector;

    return NOERROR;
}

ECode CCppVectorRefInfo::GetElementTypeInfo(
    /* [out] */ IDataTypeInfo ** ppElementTypeInfo)
{
    m_pElementTypeInfo->AddRef();
    *ppElementTypeInfo = m_pElementTypeInfo;
    return NOERROR;
}

ECode CCppVectorRefInfo::GetLength(
    /* [out] */ Int32 * pLength)
{
    if (!pLength) {
        return E_INVALID_ARGUMENT;
    }

    *pLength = m_iLength;

    return NOERROR;
}

ECode CCppVectorRefInfo::GetMaxAlignSize(
    /* [out] */ MemorySize * pAlignSize)
{
    Int32 size = 1;
    CarDataType dataType;
    m_pElementTypeInfo->GetDataType(&dataType);
    if (dataType == CarDataType_Struct) {
        ((CStructRefInfo *)m_pElementTypeInfo)->GetMaxAlignSize(&size);
    }
    else {
        m_pElementTypeInfo->GetSize(&size);
    }

    if (size > ALIGN_BOUND) size = ALIGN_BOUND;

    *pAlignSize = size;

    return NOERROR;
}
