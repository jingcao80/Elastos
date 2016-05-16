//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CVariableOfCppVector.h"
#include "CStructInfo.h"
#include "CObjInfoList.h"

UInt32 CVariableOfCppVector::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CVariableOfCppVector::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CVariableOfCppVector::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ICppVectorSetter *)this;
    }
    else if (riid == EIID_ICppVectorSetter) {
        return (ICppVectorSetter *)this;
    }
    else if (riid == EIID_ICppVectorGetter) {
        return (ICppVectorGetter *)this;
    }

    return NULL;
}

ECode CVariableOfCppVector::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CVariableOfCppVector::CVariableOfCppVector(
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ Int32 length,
    /* [in] */ PVoid pVarPtr)
{
    m_pElementTypeInfo = pElementTypeInfo;
    m_pElementTypeInfo->AddRef();

    m_pVarPtr = pVarPtr;
    m_iElementSize = 0;
    m_iLength = length;
    m_pElementTypeInfo->GetDataType(&m_dataType);
    m_pElementTypeInfo->GetSize(&m_iElementSize);
    m_pCppVectorSGetters = NULL;

    m_uSize = m_iLength * m_iElementSize;
    m_cRef = 0;
}

CVariableOfCppVector::~CVariableOfCppVector()
{
    if (m_pElementTypeInfo) m_pElementTypeInfo->Release();
    if (m_pCppVectorSGetters) {
        for (Int32 i = 0; i < m_iLength; i++) {
            if (m_pCppVectorSGetters[i]) m_pCppVectorSGetters[i]->Release();
        }
        delete [] m_pCppVectorSGetters;
    }
}

ECode CVariableOfCppVector::Init()
{
    m_pCppVectorSGetters = new PInterface[m_iLength];
    if (!m_pCppVectorSGetters) {
        return E_OUT_OF_MEMORY;
    }
    memset(m_pCppVectorSGetters, 0, m_iLength * sizeof(IInterface*));

    return NOERROR;
}
//--------------Setter----------------------------------------------------------

ECode CVariableOfCppVector::ZeroAllElements()
{
    assert(m_pVarPtr);
    memset(m_pVarPtr, 0, m_uSize);
    return NOERROR;
}

ECode CVariableOfCppVector::SetAllElements(
    /* [in] */ PVoid value,
    /* [in] */ MemorySize size)
{
    if (!value || size < (MemorySize)m_uSize) {
        return E_INVALID_ARGUMENT;
    }

    assert(m_pVarPtr);
    memcpy(m_pVarPtr, value, m_uSize);
    return NOERROR;
}

ECode CVariableOfCppVector::SetElementValue(
    /* [in] */ Int32 index,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
    if (index < 0 || index >= m_iLength || !pParam) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != type || !m_pVarPtr) {
        return E_INVALID_OPERATION;
    }

    memcpy((PByte)m_pVarPtr + m_iElementSize * index, pParam, m_iElementSize);

    return NOERROR;
}

ECode CVariableOfCppVector::SetInt16Element(
    /* [in] */ Int32 index,
    /* [in] */ Int16 value)
{
    return SetElementValue(index, &value, CarDataType_Int16);
}

ECode CVariableOfCppVector::SetInt32Element(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetElementValue(index, &value, CarDataType_Int32);
}

ECode CVariableOfCppVector::SetInt64Element(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return SetElementValue(index, &value, CarDataType_Int64);
}

ECode CVariableOfCppVector::SetByteElement(
    /* [in] */ Int32 index,
    /* [in] */ Byte value)
{
    return SetElementValue(index, &value, CarDataType_Byte);
}

ECode CVariableOfCppVector::SetFloatElement(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return SetElementValue(index, &value, CarDataType_Float);
}

ECode CVariableOfCppVector::SetDoubleElement(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return SetElementValue(index, &value, CarDataType_Double);
}

ECode CVariableOfCppVector::SetChar8Element(
    /* [in] */ Int32 index,
    /* [in] */ Char8 value)
{
    return SetElementValue(index, &value, CarDataType_Char8);
}

ECode CVariableOfCppVector::SetChar16Element(
    /* [in] */ Int32 index,
    /* [in] */ Char16 value)
{
    return SetElementValue(index, &value, CarDataType_Char16);
}

ECode CVariableOfCppVector::SetChar32Element(
    /* [in] */ Int32 index,
    /* [in] */ Char32 value)
{
    return SetElementValue(index, &value, CarDataType_Char32);
}

ECode CVariableOfCppVector::SetBooleanElement(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return SetElementValue(index, &value, CarDataType_Boolean);
}

ECode CVariableOfCppVector::SetEMuidElement(
    /* [in] */ Int32 index,
    /* [in] */ EMuid * pValue)
{
    return SetElementValue(index, (PVoid)pValue, CarDataType_EMuid);
}

ECode CVariableOfCppVector::SetEGuidElement(
    /* [in] */ Int32 index,
    /* [in] */ EGuid * pValue)
{
    return SetElementValue(index, (PVoid)pValue, CarDataType_EGuid);
}

ECode CVariableOfCppVector::SetECodeElement(
    /* [in] */ Int32 index,
    /* [in] */ ECode value)
{
    return SetElementValue(index, &value, CarDataType_ECode);
}

ECode CVariableOfCppVector::SetLocalPtrElement(
    /* [in] */ Int32 index,
    /* [in] */ LocalPtr value)
{
    return SetElementValue(index, &value, CarDataType_LocalPtr);
}

ECode CVariableOfCppVector::SetLocalTypeElement(
    /* [in] */ Int32 index,
    /* [in] */ PVoid value)
{
    return SetElementValue(index, value, CarDataType_LocalType);
}

ECode CVariableOfCppVector::SetEnumElement(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return SetElementValue(index, &value, CarDataType_Enum);
}

ECode CVariableOfCppVector::GetStructElementSetter(
    /* [in] */ Int32 index,
    /* [out] */ IStructSetter ** ppSetter)
{
    if (index < 0 || index >= m_iLength || !ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != CarDataType_Struct || !m_pVarPtr) {
        return E_INVALID_OPERATION;
    }

    CStructRefInfo *pStructInfo = (CStructRefInfo *)m_pElementTypeInfo;

    AutoPtr<IVariableOfStruct> pVariable;
    ECode ec = pStructInfo->CreateVariableBox(
        (PByte)m_pVarPtr + m_iElementSize * index, (IVariableOfStruct**)&pVariable);
    if (FAILED(ec)) {
        return ec;
    }

    return pVariable->GetSetter(ppSetter);
}

ECode CVariableOfCppVector::GetCppVectorElementSetter(
    /* [in] */ Int32 index,
    /* [out] */ ICppVectorSetter ** ppSetter)
{
    return AcquireCppVectorSGetter(index, TRUE, (IInterface**)ppSetter);
}

//--------------Getter----------------------------------------------------------

ECode CVariableOfCppVector::GetLength(
    /* [out] */ Int32 *pLength)
{
    if (!pLength) {
        return E_INVALID_ARGUMENT;
    }

    *pLength = m_iLength;

    return NOERROR;
}

ECode CVariableOfCppVector::GetRank(
    /* [out] */ Int32 *pRank)
{
    if (!pRank) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = NOERROR;
    Int32 rank = 1;
    CarDataType type = m_dataType;
    AutoPtr<IDataTypeInfo> pElementTypeInfo;
    AutoPtr<IDataTypeInfo> pPElementTypeInfo = m_pElementTypeInfo;

    while (type == CarDataType_CppVector) {
        rank++;
        pElementTypeInfo = NULL;
        ec = ((ICppVectorInfo *)pPElementTypeInfo.Get())->GetElementTypeInfo(
            (IDataTypeInfo **)&pElementTypeInfo);
        if (FAILED(ec)) return ec;

        ec = pElementTypeInfo->GetDataType(&type);
        if (FAILED(ec)) {
            return ec;
        }
        pPElementTypeInfo = pElementTypeInfo;
    }

    *pRank = rank;

    return NOERROR;
}

ECode CVariableOfCppVector::GetElementValue(
    /* [in] */ Int32 index,
    /* [in] */ void *pParam,
    /* [in] */ CarDataType type)
{
    if (index < 0 || index >= m_iLength || !pParam) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != type || !m_pVarPtr) {
        return E_INVALID_OPERATION;
    }

    memcpy(pParam, (PByte)m_pVarPtr + m_iElementSize * index, m_iElementSize);

    return NOERROR;
}

ECode CVariableOfCppVector::GetInt16Element(
    /* [in] */ Int32 index,
    /* [out] */ Int16 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int16);
}

ECode CVariableOfCppVector::GetInt32Element(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int32);
}

ECode CVariableOfCppVector::GetInt64Element(
    /* [in] */ Int32 index,
    /* [out] */ Int64 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Int64);
}

ECode CVariableOfCppVector::GetByteElement(
    /* [in] */ Int32 index,
    /* [out] */ Byte * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Byte);
}

ECode CVariableOfCppVector::GetFloatElement(
    /* [in] */ Int32 index,
    /* [out] */ Float * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Float);
}

ECode CVariableOfCppVector::GetDoubleElement(
    /* [in] */ Int32 index,
    /* [out] */ Double * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Double);
}

ECode CVariableOfCppVector::GetChar8Element(
    /* [in] */ Int32 index,
    /* [out] */ Char8 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char8);
}

ECode CVariableOfCppVector::GetChar16Element(
    /* [in] */ Int32 index,
    /* [out] */ Char16 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char16);
}

ECode CVariableOfCppVector::GetChar32Element(
    /* [in] */ Int32 index,
    /* [out] */ Char32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Char32);
}

ECode CVariableOfCppVector::GetBooleanElement(
    /* [in] */ Int32 index,
    /* [out] */ Boolean * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Boolean);
}

ECode CVariableOfCppVector::GetEMuidElement(
    /* [in] */ Int32 index,
    /* [out] */ EMuid * pValue)
{
    return GetElementValue(index, pValue, CarDataType_EMuid);
}

ECode CVariableOfCppVector::GetEGuidElement(
    /* [in] */ Int32 index,
    /* [out] */ EGuid * pValue)
{
    return GetElementValue(index, pValue, CarDataType_EGuid);
}

ECode CVariableOfCppVector::GetECodeElement(
    /* [in] */ Int32 index,
    /* [out] */ ECode * pValue)
{
    return GetElementValue(index, pValue, CarDataType_ECode);
}

ECode CVariableOfCppVector::GetLocalPtrElement(
    /* [in] */ Int32 index,
    /* [out] */ LocalPtr * pValue)
{
    return GetElementValue(index, pValue, CarDataType_LocalPtr);
}

ECode CVariableOfCppVector::GetLocalTypeElement(
    /* [in] */ Int32 index,
    /* [out] */ PVoid value)
{
    return GetElementValue(index, value, CarDataType_LocalType);
}

ECode CVariableOfCppVector::GetEnumElement(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * pValue)
{
    return GetElementValue(index, pValue, CarDataType_Enum);
}

ECode CVariableOfCppVector::GetStructElementGetter(
    /* [in] */ Int32 index,
    /* [out] */ IStructGetter ** ppGetter)
{
    if (index < 0 || index >= m_iLength || !ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != CarDataType_Struct || !m_pVarPtr) {
        return E_INVALID_OPERATION;
    }

    CStructRefInfo *pStructInfo = (CStructRefInfo *)m_pElementTypeInfo;

    AutoPtr<IVariableOfStruct> pVariable;
    ECode ec = pStructInfo->CreateVariableBox(
        (PByte)m_pVarPtr + m_iElementSize * index, (IVariableOfStruct**)&pVariable);
    if (FAILED(ec)) {
        return ec;
    }

    return pVariable->GetGetter(ppGetter);
}

ECode CVariableOfCppVector::GetCppVectorElementGetter(
    /* [in] */ Int32 index,
    /* [out] */ ICppVectorGetter ** ppGetter)
{
    return AcquireCppVectorSGetter(index, FALSE, (IInterface**)ppGetter);
}

ECode CVariableOfCppVector::Rebox(PVoid pVarPtr)
{
    if (!pVarPtr) {
        return E_INVALID_OPERATION;
    }

    m_pVarPtr = pVarPtr;
    return NOERROR;
}

ECode CVariableOfCppVector::GetSetter(
    /* [out] */ ICppVectorSetter ** ppSetter)
{
    if (!ppSetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppSetter = (ICppVectorSetter *)this;
    return NOERROR;
}

ECode CVariableOfCppVector::GetGetter(
    /* [out] */ ICppVectorGetter ** ppGetter)
{
    if (!ppGetter) {
        return E_INVALID_ARGUMENT;
    }

    this->AddRef();
    *ppGetter = (ICppVectorGetter *)this;
    return NOERROR;
}

ECode CVariableOfCppVector::AcquireCppVectorSGetter(
    /* [in] */ Int32 index,
    /* [in] */ Boolean bSetter,
    /* [out] */ IInterface ** ppSGetter)
{
    if (index < 0 || index >= m_iLength || !ppSGetter) {
        return E_INVALID_ARGUMENT;
    }

    if (m_dataType != CarDataType_CppVector || !m_pVarPtr) {
        return E_INVALID_OPERATION;
    }

    assert(m_pCppVectorSGetters);

    g_objInfoList.LockHashTable(EntryType_Struct);

    if (!m_pCppVectorSGetters[index]) {
        Int32 length = 0;
        ECode ec = ((ICppVectorInfo *)m_pElementTypeInfo)->GetLength(&length);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        AutoPtr<IDataTypeInfo> pElementTypeInfo;
        ec = ((ICppVectorInfo *)m_pElementTypeInfo)->GetElementTypeInfo(
            (IDataTypeInfo **)&pElementTypeInfo);
        if (FAILED(ec)) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        CVariableOfCppVector *pSGetter = new CVariableOfCppVector(
            pElementTypeInfo, length,
            (PByte)m_pVarPtr  + m_iElementSize * index);
        if (pSGetter == NULL) {
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return E_OUT_OF_MEMORY;
        }

        ec = pSGetter->Init();
        if (FAILED(ec)) {
            delete pSGetter;
            g_objInfoList.UnlockHashTable(EntryType_Struct);
            return ec;
        }

        if (bSetter) {
            pSGetter->GetSetter(
                (ICppVectorSetter**)&m_pCppVectorSGetters[index]);
        }
        else {
            pSGetter->GetGetter(
                (ICppVectorGetter**)&m_pCppVectorSGetters[index]);
        }
    }

    m_pCppVectorSGetters[index]->AddRef();
    *ppSGetter = m_pCppVectorSGetters[index];

    g_objInfoList.UnlockHashTable(EntryType_Struct);

    return NOERROR;
}
