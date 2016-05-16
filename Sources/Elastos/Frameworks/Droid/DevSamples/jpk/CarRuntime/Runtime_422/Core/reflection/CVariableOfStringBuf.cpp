//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CVariableOfStringBuf.h"
#include "CObjInfoList.h"

UInt32 CVariableOfStringBuf::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CVariableOfStringBuf::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CVariableOfStringBuf::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IVariableOfStringBuf *)this;
    }
    else if (riid == EIID_IVariableOfStringBuf) {
        return (IVariableOfStringBuf *)this;
    }
    else if (riid == EIID_IStringBufSetter) {
        return (IStringBufSetter *)this;
    }
    else if (riid == EIID_IStringBufGetter) {
        return (IStringBufGetter *)this;
    }
    else {
        return NULL;
    }
}

ECode CVariableOfStringBuf::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CVariableOfStringBuf::CVariableOfStringBuf(
    /* [in] */ PStringBuf psb,
    /* [in] */ Boolean bAlloc)
{
    m_psb = psb;
    m_bAlloc = bAlloc;
    m_cRef = 0;
}

CVariableOfStringBuf::~CVariableOfStringBuf()
{
    if (m_bAlloc && m_psb) {
        StringBuf::Free(m_psb);
    }
}

ECode CVariableOfStringBuf::GetTypeInfo(
    /* [out] */ IDataTypeInfo ** ppTypeInfo)
{
    return g_objInfoList.AcquireIntrinsicInfo(CarDataType_Char8, ppTypeInfo);
}

ECode CVariableOfStringBuf::GetPayload(
    /* [out] */ PVoid * pPayload)
{
    if (!pPayload) {
        return E_INVALID_ARGUMENT;
    }

    *pPayload = m_psb;
    return NOERROR;
}

ECode CVariableOfStringBuf::Rebox(
    /* [in] */ PVoid localVariablePtr)
{
    PStringBuf psb = (PStringBuf)localVariablePtr;
    if (!psb || !(psb->m_flags & CarQuintetFlag_Type_Char8)
        || !psb->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    if (m_bAlloc && m_psb) {
        StringBuf::Free(m_psb);
        m_bAlloc = FALSE;
    }

    m_psb = psb;

    return NOERROR;
}

ECode CVariableOfStringBuf::GetSetter(
    /* [out] */ IStringBufSetter ** ppSetter)
{
    if (!ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppSetter = (IStringBufSetter *)this;
    return NOERROR;
}

ECode CVariableOfStringBuf::GetGetter(
    /* [out] */ IStringBufGetter ** ppGetter)
{
    if (!ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppGetter = (IStringBufGetter *)this;
    return NOERROR;
}

ECode CVariableOfStringBuf::SetElement(
    /* [in] */ Int32 index,
    /* [in] */ Char8 character)
{
    if (index < 0 || index >= m_psb->GetLength()) {
        return E_INVALID_ARGUMENT;
    }

    (*m_psb)[index] = character;
    return NOERROR;
}

ECode CVariableOfStringBuf::SetData(
    /* [in] */ CString str)
{
    if (str.IsNull()) {
        return E_INVALID_ARGUMENT;
    }

    m_psb->Copy(str);
    return NOERROR;
}

ECode CVariableOfStringBuf::GetCapacity(
    /* [out] */ Int32 * pCapacity)
{
    if (!pCapacity) {
        return E_INVALID_ARGUMENT;
    }

    *pCapacity = m_psb->GetCapacity();
    return NOERROR;
}

ECode CVariableOfStringBuf::GetLength(
    /* [out] */ Int32 * pLength)
{
    if (!pLength) {
        return E_INVALID_ARGUMENT;
    }

    *pLength = m_psb->GetLength();
    return NOERROR;
}

ECode CVariableOfStringBuf::GetElement(
    /* [in] */ Int32 index,
    /* [out] */ Char8 * character)
{
    if (index >= m_psb->GetLength()) {
        return E_INVALID_ARGUMENT;
    }

    *character = (*m_psb)[index];
    return NOERROR;
}

ECode CVariableOfStringBuf::GetData(
    /* [out] */ StringBuf * stringBuf)
{
    if (!stringBuf) {
        return E_INVALID_ARGUMENT;
    }

    stringBuf->Copy(m_psb->GetPayload());

    return NOERROR;
}
