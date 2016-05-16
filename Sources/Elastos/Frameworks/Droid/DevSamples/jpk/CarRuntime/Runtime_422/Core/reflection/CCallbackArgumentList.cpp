//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCallbackArgumentList.h"

UInt32 CCallbackArgumentList::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CCallbackArgumentList::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface  CCallbackArgumentList::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ICallbackArgumentList) {
        return (ICallbackArgumentList *)this;
    }
    else {
        return NULL;
    }
}

ECode CCallbackArgumentList::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CCallbackArgumentList::CCallbackArgumentList()
{
    m_pParamElem = 0;
    m_uParamCount = 0;
    m_pParamBuf = NULL;
    m_pCallbackMethodInfo = NULL;
    m_cRef = 0;
}

CCallbackArgumentList::~CCallbackArgumentList()
{
    if (m_pParamBuf) free(m_pParamBuf);
    if (m_pCallbackMethodInfo) m_pCallbackMethodInfo->Release();
}

ECode CCallbackArgumentList::Init(
    /* [in] */ ICallbackMethodInfo *pCallbackMethodInfo,
    /* [in] */ ParmElement *pParamElem,
    /* [in] */ UInt32 uParamCount,
    /* [in] */ UInt32 uParamBufSize)
{
    m_pParamBuf = (PByte)malloc(uParamBufSize);
    if (m_pParamBuf == NULL) {
        return E_OUT_OF_MEMORY;
    }

    m_pParamElem = pParamElem;
    m_uParamCount = uParamCount;
    memset(m_pParamBuf, 0, uParamBufSize);
    m_uParamBufSize = uParamBufSize;
    m_pCallbackMethodInfo = pCallbackMethodInfo;
    m_pCallbackMethodInfo->AddRef();

    return NOERROR;
}

ECode CCallbackArgumentList::GetCallbackMethodInfo(
    /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo)
{
    if (!ppCallbackMethodInfo) {
        return E_INVALID_ARGUMENT;
    }
    m_pCallbackMethodInfo->AddRef();
    *ppCallbackMethodInfo = m_pCallbackMethodInfo;

    return NOERROR;
}

ECode CCallbackArgumentList::GetParamValue(
    /* [in] */ Int32 index,
    /* [in] */ PVoid pParam,
    /* [in] */ CarDataType type)
{
    if (type == CarDataType_CarArray
        && (m_pParamElem[index].type == CarDataType_ArrayOf
        || m_pParamElem[index].type == CarDataType_BufferOf
        || m_pParamElem[index].type == CarDataType_MemoryBuf)) {
        type = m_pParamElem[index].type;
    }

    if (index < 0 || index >= (Int32)m_uParamCount || !pParam
        || type != m_pParamElem[index].type) {
        return E_INVALID_ARGUMENT;
    }

    if (!m_pParamBuf) {
        return E_INVALID_OPERATION;
    }

    UInt32 *pParamValue = (UInt32 *)(m_pParamBuf + m_pParamElem[index].pos);

    if (m_pParamElem[index].size == 1) {
        *(Byte *)pParam = (Byte)*pParamValue;
    }
    else if (m_pParamElem[index].size == 2) {
        *(UInt16 *)pParam = (UInt16)*pParamValue;
    }
    else if (m_pParamElem[index].size == 4) {
        *(UInt32 *)pParam = (UInt32)*pParamValue;
    }
    else if (m_pParamElem[index].size == 8) {
        *(UInt64 *)pParam = *(UInt64 *)pParamValue;
    }
    else {
        return E_INVALID_OPERATION;
    }

    return NOERROR;
}

ECode CCallbackArgumentList::GetServerPtrArgument(
    /* [out] */ PInterface * pServer)
{
    if (!pServer) {
        return E_INVALID_ARGUMENT;
    }

    *pServer = *(PInterface *)m_pParamBuf;
    return NOERROR;
}

ECode CCallbackArgumentList::GetInt16Argument(
    /* [in] */ Int32 index,
    /* [out] */ Int16 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Int16);
}

ECode CCallbackArgumentList::GetInt32Argument(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Int32);
}

ECode CCallbackArgumentList::GetInt64Argument(
    /* [in] */ Int32 index,
    /* [out] */ Int64 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Int64);
}

ECode CCallbackArgumentList::GetByteArgument(
    /* [in] */ Int32 index,
    /* [out] */ Byte * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Byte);
}

ECode CCallbackArgumentList::GetFloatArgument(
    /* [in] */ Int32 index,
    /* [out] */ Float * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Float);
}

ECode CCallbackArgumentList::GetDoubleArgument(
    /* [in] */ Int32 index,
    /* [out] */ Double * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Double);
}

ECode CCallbackArgumentList::GetChar8Argument(
    /* [in] */ Int32 index,
    /* [out] */ Char8 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Char8);
}

ECode CCallbackArgumentList::GetChar16Argument(
    /* [in] */ Int32 index,
    /* [out] */ Char16 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Char16);
}

ECode CCallbackArgumentList::GetChar32Argument(
    /* [in] */ Int32 index,
    /* [out] */ Char32 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Char32);
}

ECode CCallbackArgumentList::GetCStringArgument(
    /* [in] */ Int32 index,
    /* [out] */ CString* value)
{
    return GetParamValue(index, value, CarDataType_CString);
}

ECode CCallbackArgumentList::GetStringArgument(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    return GetParamValue(index, value, CarDataType_String);
}

ECode CCallbackArgumentList::GetBooleanArgument(
    /* [in] */ Int32 index,
    /* [out] */ Boolean * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Boolean);
}

ECode CCallbackArgumentList::GetEMuidArgument(
    /* [in] */ Int32 index,
    /* [out] */ EMuid ** ppValue)
{
    return GetParamValue(index, ppValue, CarDataType_EMuid);
}

ECode CCallbackArgumentList::GetEGuidArgument(
    /* [in] */ Int32 index,
    /* [out] */ EGuid ** ppValue)
{
    return GetParamValue(index, ppValue, CarDataType_EGuid);
}

ECode CCallbackArgumentList::GetECodeArgument(
    /* [in] */ Int32 index,
    /* [out] */ ECode * pValue)
{
    return GetParamValue(index, pValue, CarDataType_ECode);
}

ECode CCallbackArgumentList::GetLocalPtrArgument(
    /* [in] */ Int32 index,
    /* [out] */ LocalPtr * pValue)
{
    return GetParamValue(index, pValue, CarDataType_LocalPtr);
}

ECode CCallbackArgumentList::GetEnumArgument(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Enum);
}

ECode CCallbackArgumentList::GetCarArrayArgument(
    /* [in] */ Int32 index,
    /* [out] */ PCarQuintet * pValue)
{
    return GetParamValue(index, pValue, CarDataType_CarArray);
}

ECode CCallbackArgumentList::GetStructPtrArgument(
    /* [in] */ Int32 index,
    /* [out] */ PVoid * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Struct);
}

ECode CCallbackArgumentList::GetObjectPtrArgument(
    /* [in] */ Int32 index,
    /* [out] */ PInterface * pValue)
{
    return GetParamValue(index, pValue, CarDataType_Interface);
}
