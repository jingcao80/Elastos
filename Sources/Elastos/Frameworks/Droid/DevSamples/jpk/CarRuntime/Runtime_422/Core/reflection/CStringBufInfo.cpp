//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "refutil.h"
#include "CStringBufInfo.h"
#include "CVariableOfStringBuf.h"

UInt32 CStringBufInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CStringBufInfo::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface  CStringBufInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IStringBufInfo) {
        return (IStringBufInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CStringBufInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CStringBufInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(g_cDataTypeList[CarDataType_StringBuf].name);
    return NOERROR;
}

ECode CStringBufInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return E_INVALID_OPERATION;
}

ECode CStringBufInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = CarDataType_StringBuf;
    return NOERROR;
}

ECode CStringBufInfo::CreateVariable(
    /* [in] */ Int32 capacity,
    /* [out] */ IVariableOfStringBuf ** ppVariableBox)
{
    if (capacity <= 0 || !ppVariableBox) {
        return E_INVALID_ARGUMENT;
    }

    StringBuf *psb =  StringBuf::Alloc(capacity);
    if (!psb) {
        return E_OUT_OF_MEMORY;
    }

    CVariableOfStringBuf *pVarBox = new CVariableOfStringBuf(psb, TRUE);
    if (pVarBox == NULL) {
        StringBuf::Free(psb);
        return E_OUT_OF_MEMORY;
    }

    *ppVariableBox = (IVariableOfStringBuf *)pVarBox;
    pVarBox->AddRef();

    return NOERROR;
}

ECode CStringBufInfo::CreateVariableBox(
    /* [in] */ PCarQuintet variableDescriptor,
    /* [out] */ IVariableOfStringBuf ** ppVariableBox)
{
    if (!variableDescriptor || !ppVariableBox) {
        return E_INVALID_ARGUMENT;
    }

    PStringBuf psb = (PStringBuf)variableDescriptor;
    if (!(psb->m_flags & CarQuintetFlag_Type_Char8) || !psb->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    CVariableOfStringBuf *pVarBox = new CVariableOfStringBuf(psb, FALSE);
    if (pVarBox == NULL) {
        return E_OUT_OF_MEMORY;
    }

    *ppVariableBox = (IVariableOfStringBuf *)pVarBox;
    pVarBox->AddRef();

    return NOERROR;
}
