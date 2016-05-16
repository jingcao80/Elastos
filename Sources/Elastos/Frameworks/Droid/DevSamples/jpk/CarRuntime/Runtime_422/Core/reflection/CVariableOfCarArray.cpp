//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CVariableOfCarArray.h"
#include "CStructInfo.h"
#include "CObjInfoList.h"

UInt32 CVariableOfCarArray::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CVariableOfCarArray::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CVariableOfCarArray::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IVariableOfCarArray *)this;
    }
    else if (riid == EIID_IVariableOfCarArray) {
        return (IVariableOfCarArray *)this;
    }
    else if (riid == EIID_ICarArraySetter) {
        return (ICarArraySetter *)this;
    }
    else if (riid == EIID_ICarArrayGetter) {
        return (ICarArrayGetter *)this;
    }
    else {
        return NULL;
    }
}

ECode CVariableOfCarArray::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CVariableOfCarArray::CVariableOfCarArray(
    /* [in] */ ICarArrayInfo *pTypeInfo,
    /* [in] */ PCarQuintet pCq,
    /* [in] */ Boolean bAlloc)
{
    m_pCarArrayInfo = pTypeInfo;
    m_pCarArrayInfo->AddRef();

    m_pCq = pCq;
    m_bAlloc = bAlloc;
    m_iElementSize = 0;
    m_iLength = 0;

    AutoPtr<IDataTypeInfo> pElementTypeInfo;
    m_pCarArrayInfo->GetElementTypeInfo((IDataTypeInfo**)&pElementTypeInfo);
    assert(pElementTypeInfo);
    pElementTypeInfo->GetDataType(&m_dataType);
    pElementTypeInfo->GetSize(&m_iElementSize);
    assert(m_iElementSize);
    m_iLength = pCq->m_size / m_iElementSize;
    m_cRef = 0;
}

CVariableOfCarArray::~CVariableOfCarArray()
{
    if (m_bAlloc && m_pCq) {
        free(m_pCq);
    }

    if (m_pCarArrayInfo) m_pCarArrayInfo->Release();
}

ECode CVariableOfCarArray::GetTypeInfo(
    /* [out] */ IDataTypeInfo ** ppTypeInfo)
{
    if (!ppTypeInfo) {
        return E_INVALID_ARGUMENT;
    }

    m_pCarArrayInfo->AddRef();
    *ppTypeInfo = m_pCarArrayInfo;
    return NOERROR;
}

ECode CVariableOfCarArray::GetPayload(
    /* [out] */ PVoid * pPayload)
{
    if (!pPayload) {
        return E_INVALID_ARGUMENT;
    }

    *pPayload = m_pCq;
    return NOERROR;
}

ECode CVariableOfCarArray::Rebox(
    /* [in] */ PVoid localVariablePtr)
{
    CarQuintetFlag flag = DataTypeToFlag(m_dataType);
    PCarQuintet pCq = (PCarQuintet)localVariablePtr;
    if (!pCq || !(pCq->m_flags & flag) || pCq->m_size < m_iElementSize) {
        return E_INVALID_ARGUMENT;
    }

    if (m_bAlloc && m_pCq) {
        free(m_pCq);
        m_bAlloc = FALSE;
    }

    m_pCq = pCq;

    return NOERROR;
}

ECode CVariableOfCarArray::GetSetter(
    /* [out] */ ICarArraySetter ** ppSetter)
{
    if (!ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppSetter = (ICarArraySetter *)this;
    return NOERROR;
}

ECode CVariableOfCarArray::GetGetter(
    /* [out] */ ICarArrayGetter ** ppGetter)
{
    if (!ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppGetter = (ICarArrayGetter *)this;

    return NOERROR;
}

//Setter

ECode CVariableOfCarArray::SetUsed(
    /* [in] */ Int32 used)
{
    if (used < 0 || used > m_iLength) {
        return E_INVALID_ARGUMENT;
    }

    m_pCq->m_used = used * m_iElementSize;

    return NOERROR;
}

ECode CVariableOfCarArray::SetElementValue(
    /* [in] */ Int32 index,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
    if (!pParam) {
        return E_INVALID_ARGUMENT;
    }

    if (index >= m_iLength
            || index >= (Int32)(m_pCq->m_used / m_iElementSize)) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != type || !m_pCq->m_pBuf) {
        return E_INVALID_OPERATION;
    }

    memcpy((PByte)m_pCq->m_pBuf + m_iElementSize * index,
        pParam, m_iElementSize);

    return NOERROR;
}

ECode CVariableOfCarArray::SetInt16Element(
    /* [in] */ Int32 index,
    /* [in] */ Int16 value)
{
    return SetElementValue(index, &value, CarDataType_Int16);
}

ECode CVariableOfCarArray::SetInt32Element(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetElementValue(index, &value, CarDataType_Int32);
}

ECode CVariableOfCarArray::SetInt64Element(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return SetElementValue(index, &value, CarDataType_Int64);
}

ECode CVariableOfCarArray::SetByteElement(
    /* [in] */ Int32 index,
    /* [in] */ Byte value)
{
    return SetElementValue(index, &value, CarDataType_Byte);
}

ECode CVariableOfCarArray::SetFloatElement(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return SetElementValue(index, &value, CarDataType_Float);
}

ECode CVariableOfCarArray::SetDoubleElement(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return SetElementValue(index, &value, CarDataType_Double);
}

ECode CVariableOfCarArray::SetEnumElement(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetElementValue(index, &value, CarDataType_Enum);
}

ECode CVariableOfCarArray::SetChar8Element(
    /* [in] */ Int32 index,
    /* [in] */ Char8 value)
{
    return SetElementValue(index, &value, CarDataType_Char8);
}

ECode CVariableOfCarArray::SetChar16Element(
    /* [in] */ Int32 index,
    /* [in] */ Char16 value)
{
    return SetElementValue(index, &value, CarDataType_Char16);
}

ECode CVariableOfCarArray::SetChar32Element(
    /* [in] */ Int32 index,
    /* [in] */ Char32 value)
{
    return SetElementValue(index, &value, CarDataType_Char32);
}

ECode CVariableOfCarArray::SetCStringElement(
    /* [in] */ Int32 index,
    /* [in] */ CString value)
{
    return SetElementValue(index, reinterpret_cast<void*>(const_cast<CString*>(&value)), CarDataType_CString);
}

ECode CVariableOfCarArray::SetStringElement(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    return SetElementValue(index, reinterpret_cast<void*>(const_cast<String*>(&value)), CarDataType_String);
}

ECode CVariableOfCarArray::SetBooleanElement(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return SetElementValue(index, &value, CarDataType_Boolean);
}

ECode CVariableOfCarArray::SetEMuidElement(
    /* [in] */ Int32 index,
    /* [in] */ EMuid * pValue)
{
    return SetElementValue(index, &pValue, CarDataType_EMuid);
}

ECode CVariableOfCarArray::SetEGuidElement(
    /* [in] */ Int32 index,
    /* [in] */ EGuid * pValue)
{
    return SetElementValue(index, &pValue, CarDataType_EGuid);
}

ECode CVariableOfCarArray::SetECodeElement(
    /* [in] */ Int32 index,
    /* [in] */ ECode value)
{
    return SetElementValue(index, &value, CarDataType_ECode);
}

ECode CVariableOfCarArray::SetLocalTypeElement(
    /* [in] */ Int32 index,
    /* [in] */ PVoid value)
{
    return SetElementValue(index, value, CarDataType_LocalType);
}

ECode CVariableOfCarArray::SetObjectPtrElement(
    /* [in] */ Int32 index,
    /* [in] */ PInterface value)
{
    return SetElementValue(index, &value, CarDataType_Interface);
}

ECode CVariableOfCarArray::GetStructElementSetter(
    /* [in] */ Int32 index,
    /* [out] */ IStructSetter ** ppSetter)
{
    if (!ppSetter || m_dataType != CarDataType_Struct) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<IDataTypeInfo> pElementTypeInfo;
    ECode ec = m_pCarArrayInfo->GetElementTypeInfo((IDataTypeInfo **)&pElementTypeInfo);
    if (FAILED(ec)) {
        return ec;
    }

    CStructRefInfo *pStructInfo = (CStructRefInfo *)pElementTypeInfo.Get();

    AutoPtr<IVariableOfStruct> pVariable;
    ec = pStructInfo->CreateVariableBox(
        (PByte)m_pCq->m_pBuf + m_iElementSize * index, (IVariableOfStruct **)&pVariable);
    if (FAILED(ec)) {
        return ec;
    }

    return pVariable->GetSetter(ppSetter);
}

//Getter

ECode CVariableOfCarArray::GetCapacity(
    /* [out] */ Int32 *pCapacity)
{
    if (!pCapacity) {
        return E_INVALID_ARGUMENT;
    }

    *pCapacity = m_iLength;

    return NOERROR;
}

ECode CVariableOfCarArray::GetUsed(
    /* [out] */ Int32 *pUsed)
{
    if (!pUsed) {
        return E_INVALID_ARGUMENT;
    }

    *pUsed = m_pCq->m_used / m_iElementSize;

    return NOERROR;
}

ECode CVariableOfCarArray::IsEmpty(
    /* [out] */ Boolean *pIsEmpty)
{
    if (!pIsEmpty) {
        return E_INVALID_ARGUMENT;
    }

    *pIsEmpty = (m_pCq->m_pBuf == NULL);

    return NOERROR;
}

ECode CVariableOfCarArray::GetElementValue(
    /* [in] */ Int32 index,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
    if (!pParam) {
        return E_INVALID_ARGUMENT;
    }

    if (index >= m_iLength
            || index >= (Int32)(m_pCq->m_used / m_iElementSize)) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != type || !m_pCq->m_pBuf) {
        return E_INVALID_OPERATION;
    }

    memcpy(pParam, (PByte)m_pCq->m_pBuf + m_iElementSize * index,
        m_iElementSize);

    return NOERROR;
}

ECode CVariableOfCarArray::GetInt16Element(
    /* [in] */ Int32 index,
    /* [out] */ Int16 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int16);
}

ECode CVariableOfCarArray::GetInt32Element(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int32);
}

ECode CVariableOfCarArray::GetInt64Element(
    /* [in] */ Int32 index,
    /* [out] */ Int64 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int64);
}

ECode CVariableOfCarArray::GetByteElement(
    /* [in] */ Int32 index,
    /* [out] */ Byte * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Byte);
}

ECode CVariableOfCarArray::GetFloatElement(
    /* [in] */ Int32 index,
    /* [out] */ Float * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Float);
}

ECode CVariableOfCarArray::GetDoubleElement(
    /* [in] */ Int32 index,
    /* [out] */ Double * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Double);
}

ECode CVariableOfCarArray::GetEnumElement(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Enum);
}

ECode CVariableOfCarArray::GetChar8Element(
    /* [in] */ Int32 index,
    /* [out] */ Char8 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char8);
}

ECode CVariableOfCarArray::GetChar16Element(
    /* [in] */ Int32 index,
    /* [out] */ Char16 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char16);
}

ECode CVariableOfCarArray::GetChar32Element(
    /* [in] */ Int32 index,
    /* [out] */ Char32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char32);
}

ECode CVariableOfCarArray::GetCStringElement(
    /* [in] */ Int32 index,
    /* [out] */ CString* pValue)
{
    return GetElementValue(index, pValue, CarDataType_CString);
}

ECode CVariableOfCarArray::GetStringElement(
    /* [in] */ Int32 index,
    /* [out] */ String* pValue)
{
    return GetElementValue(index, pValue, CarDataType_String);
}

ECode CVariableOfCarArray::GetBooleanElement(
    /* [in] */ Int32 index,
    /* [out] */ Boolean * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Boolean);
}

ECode CVariableOfCarArray::GetEMuidElement(
    /* [in] */ Int32 index,
    /* [out] */ EMuid * pValue)
{
    return GetElementValue(index, pValue, CarDataType_EMuid);
}

ECode CVariableOfCarArray::GetEGuidElement(
    /* [in] */ Int32 index,
    /* [out] */ EGuid * pValue)
{
    return GetElementValue(index, pValue, CarDataType_EGuid);
}

ECode CVariableOfCarArray::GetECodeElement(
    /* [in] */ Int32 index,
    /* [out] */ ECode * pValue)
{
    return GetElementValue(index, pValue, CarDataType_ECode);
}

ECode CVariableOfCarArray::GetLocalTypeElement(
    /* [in] */ Int32 index,
    /* [out] */ PVoid value)
{
    return GetElementValue(index, value, CarDataType_LocalType);
}

ECode CVariableOfCarArray::GetObjectPtrElement(
    /* [in] */ Int32 index,
    /* [out] */ PInterface * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Interface);
}

ECode CVariableOfCarArray::GetStructElementGetter(
    /* [in] */ Int32 index,
    /* [out] */ IStructGetter ** ppGetter)
{
    if (!ppGetter || m_dataType != CarDataType_Struct) {
        return E_INVALID_ARGUMENT;
    }

    AutoPtr<IDataTypeInfo> pElementTypeInfo;
    ECode ec = m_pCarArrayInfo->GetElementTypeInfo((IDataTypeInfo **)&pElementTypeInfo);
    if (FAILED(ec)) {
        return ec;
    }

    CStructRefInfo *pStructInfo = (CStructRefInfo *)pElementTypeInfo.Get();

    AutoPtr<IVariableOfStruct> pVariable;
    ec = pStructInfo->CreateVariableBox(
        (PByte)m_pCq->m_pBuf + m_iElementSize * index, (IVariableOfStruct **)&pVariable);
    if (FAILED(ec)) {
        return ec;
    }

    return pVariable->GetGetter(ppGetter);
}
