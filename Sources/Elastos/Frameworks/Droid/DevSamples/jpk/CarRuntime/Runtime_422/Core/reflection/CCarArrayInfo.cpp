//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCarArrayInfo.h"
#include "CVariableOfCarArray.h"
#include "CObjInfoList.h"
#include "CStructInfo.h"

UInt32 CCarArrayRefInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CCarArrayRefInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_DataType);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        g_objInfoList.DetachCarArrayInfo(this);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_DataType);
    assert(nRef >= 0);
    return nRef;
}

PInterface CCarArrayRefInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ICarArrayInfo) {
        return (ICarArrayInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CCarArrayRefInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

 CCarArrayRefInfo::CCarArrayRefInfo(
    /* [in] */ CarDataType quintetType,
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ CarDataType dataType)
{
    m_pElementTypeInfo = pElementTypeInfo;
    m_pElementTypeInfo->AddRef();

    m_elementDataType = dataType;
    m_quintetType = quintetType;
    m_cRef = 0;
}

CCarArrayRefInfo::~CCarArrayRefInfo()
{
    if (m_pElementTypeInfo) m_pElementTypeInfo->Release();
}

ECode CCarArrayRefInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

//    if (m_elementDataType == CarDataType_LocalType) {
//        pName->Copy(g_cDataTypeList[CarDataType_LocalType].name);
//    }


    pName->Copy(g_cDataTypeList[m_quintetType].name);

    if (m_quintetType != CarDataType_MemoryBuf) {
        StringBuf_<32> elementName;
        ECode ec = m_pElementTypeInfo->GetName(&elementName);
        if (FAILED(ec)) return ec;

        pName->Append("<");
        pName->Append((char*)elementName);
        if (m_elementDataType == CarDataType_Interface) {
            pName->Append("*");
        }
        pName->Append(">");
    }

    return NOERROR;
}

ECode CCarArrayRefInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return E_INVALID_OPERATION;
}

ECode CCarArrayRefInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = m_quintetType;
    return NOERROR;
}

ECode CCarArrayRefInfo::GetElementTypeInfo(
    /* [out] */ IDataTypeInfo ** ppElementTypeInfo)
{
    if (!ppElementTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    m_pElementTypeInfo->AddRef();
    *ppElementTypeInfo = m_pElementTypeInfo;
    return NOERROR;
}

ECode CCarArrayRefInfo::CreateVariable(
    /* [in] */ Int32 capacity,
    /* [out] */ IVariableOfCarArray ** ppVariable)
{
    if (capacity <= 0 || !ppVariable) {
        return E_INVALID_ARGUMENT;
    }

    Int32 size = 0;
    ECode ec = m_pElementTypeInfo->GetSize(&size);
    if (FAILED(ec)) return ec;

    Int32 bufSize = capacity * size;
    PCarQuintet pCarQuintet =
        (PCarQuintet)calloc(sizeof(CarQuintet) + bufSize, sizeof(Byte));
    if (!pCarQuintet) {
        return E_OUT_OF_MEMORY;
    }

    pCarQuintet->m_flags = DataTypeToFlag(m_elementDataType);
    if (m_quintetType == CarDataType_ArrayOf) {
        pCarQuintet->m_used = bufSize;
    }

    pCarQuintet->m_size = bufSize;
    pCarQuintet->m_pBuf = (Byte *)pCarQuintet + sizeof(CarQuintet);

    CVariableOfCarArray *pCarArrayBox = new CVariableOfCarArray(this,
        pCarQuintet, TRUE);
    if (pCarArrayBox == NULL) {
        free(pCarQuintet);
        return E_OUT_OF_MEMORY;
    }

    *ppVariable = (IVariableOfCarArray *)pCarArrayBox;
    pCarArrayBox->AddRef();

    return NOERROR;
}

ECode CCarArrayRefInfo::CreateVariableBox(
    /* [in] */ PCarQuintet variableDescriptor,
    /* [out] */ IVariableOfCarArray ** ppVariable)
{
    if (!variableDescriptor || !ppVariable) {
        return E_INVALID_ARGUMENT;
    }

    Int32 size = 0;
    ECode ec = m_pElementTypeInfo->GetSize(&size);
    if (FAILED(ec)) return ec;
    if (variableDescriptor->m_size < size) {
        return E_INVALID_ARGUMENT;
    }

    if (!(variableDescriptor->m_flags & DataTypeToFlag(m_elementDataType))) {
        return E_INVALID_ARGUMENT;
    }

    CVariableOfCarArray *pCarArrayBox = new CVariableOfCarArray(this,
        variableDescriptor, FALSE);
    if (pCarArrayBox == NULL) {
        return E_OUT_OF_MEMORY;
    }

    *ppVariable = (IVariableOfCarArray *)pCarArrayBox;
    pCarArrayBox->AddRef();

    return NOERROR;
}

ECode CCarArrayRefInfo::GetMaxAlignSize(
    /* [out] */ MemorySize * pAlignSize)
{
    Int32 size = 1;

    if (m_elementDataType == CarDataType_Struct) {
        ((CStructRefInfo *)m_pElementTypeInfo)->GetMaxAlignSize(&size);
    }
    else {
        m_pElementTypeInfo->GetSize(&size);
    }

    if (size > ALIGN_BOUND) size = ALIGN_BOUND;

    *pAlignSize = size;

    return NOERROR;
}
