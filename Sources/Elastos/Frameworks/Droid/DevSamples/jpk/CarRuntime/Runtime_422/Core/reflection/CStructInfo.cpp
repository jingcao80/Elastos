//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CStructInfo.h"
#include "CFieldInfo.h"
#include "CVariableOfStruct.h"
#include "CCarArrayInfo.h"
#include "CCppVectorInfo.h"

UInt32 CStructRefInfo::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CStructRefInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Struct);
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        if (!m_pCClsModule) {
            g_objInfoList.DetachStructInfo(this);
        }
        else {
            g_objInfoList.RemoveStructInfo(m_pStructDirEntry);
        }
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Struct);
    assert(nRef >= 0);
    return nRef;
}

PInterface CStructRefInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IStructInfo) {
        return (IStructInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CStructRefInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CStructRefInfo::CStructRefInfo()
{
    m_pCClsModule = NULL;
    m_pName = NULL;
    m_pFieldNames = NULL;
    m_pBuf = NULL;
    m_pStructFieldDesc = NULL;
    m_uSize = 0;
    m_cRef = 0;
}

CStructRefInfo::~CStructRefInfo()
{
    if (m_pStructFieldDesc) delete [] m_pStructFieldDesc;
    if (m_pCClsModule) m_pCClsModule->Release();
    if (m_pBuf) free(m_pBuf);
    if (m_pFieldNames) BufferOf<CString>::Free(m_pFieldNames);

    Int32 i = 0;
    if (m_pFieldTypeInfos) {
        for (int i = 0; i < m_pFieldTypeInfos->GetUsed(); i++) {
            if ((*m_pFieldTypeInfos)[i]) (*m_pFieldTypeInfos)[i]->Release();
        }
        BufferOf<IDataTypeInfo *>::Free(m_pFieldTypeInfos);
    }

    if (m_pFieldInfos) {
        for (i = 0; i < m_pFieldInfos->GetUsed(); i++) {
            if ((*m_pFieldInfos)[i]) delete (*m_pFieldInfos)[i];
        }
        BufferOf<IFieldInfo *>::Free(m_pFieldInfos);
    }
}

ECode CStructRefInfo::InitStatic(
    /* [in] */ CClsModule * pCClsModule,
    /* [in] */ StructDirEntry *pStructDirEntry)
{
    m_pCClsModule = pCClsModule;
    m_pCClsModule->AddRef();

	Int32 nBase = m_pCClsModule->m_nBase;
    m_pStructDirEntry = pStructDirEntry;
    m_pName = adjustNameAddr(nBase, m_pStructDirEntry->pszName);
	StructDescriptor *pDesc = adjustStructDescAddr(nBase, pStructDirEntry->pDesc);
    StructElement *pElem = NULL;

    ECode ec = NOERROR;
    int i = 0;
    m_pFieldTypeInfos = BufferOf<IDataTypeInfo *>::Alloc(pDesc->cElems);
    if (!m_pFieldTypeInfos) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    m_pFieldNames = BufferOf<CString>::Alloc(pDesc->cElems);
    if (!m_pFieldNames) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    memset(m_pFieldTypeInfos->GetPayload(), 0, sizeof(PInterface) * pDesc->cElems);
    memset(m_pFieldNames->GetPayload(), 0, sizeof(CString) * pDesc->cElems);
    m_pFieldTypeInfos->SetUsed(pDesc->cElems);
    m_pFieldNames->SetUsed(pDesc->cElems);

    for (i = 0; i < pDesc->cElems; i++) {
        pElem = getStructElementAddr(nBase, pDesc->ppElems, i);
        (*m_pFieldNames)[i] = adjustNameAddr(nBase, pElem->pszName);

        ec = g_objInfoList.AcquireDataTypeInfo(m_pCClsModule,
            &pElem->type,
            (IDataTypeInfo**)&(*m_pFieldTypeInfos)[i], TRUE);
        if (FAILED(ec)) goto EExit;
    }

    ec = InitFieldElement();
    if (FAILED(ec)) goto EExit;

    ec = InitFieldInfos();
    if (FAILED(ec)) goto EExit;

    return NOERROR;

EExit:
    if (m_pFieldNames) {
        BufferOf<CString>::Free(m_pFieldNames);
        m_pFieldNames = NULL;
    }

    if (m_pFieldTypeInfos) {
        for (i = 0; i < pDesc->cElems; i++) {
            if ((*m_pFieldTypeInfos)[i]) (*m_pFieldTypeInfos)[i]->Release();
        }

        BufferOf<IDataTypeInfo *>::Free(m_pFieldTypeInfos);
        m_pFieldTypeInfos = NULL;
    }

    return ec;
}

ECode CStructRefInfo::InitDynamic(
    /* [in] */ CString name,
    /* [in] */ const BufferOf<CString>& fieldNames,
    /* [in] */ const BufferOf<IDataTypeInfo *>& fieldTypeInfos)
{
    ECode ec = NOERROR;

    //Get the totle size;
    int i, size = StringAlignSize(name);

    for (i = 0; i < fieldNames.GetUsed(); i++) {
        size += StringAlignSize(fieldNames[i]);
    }

    m_pBuf = (char *)malloc(size);
    if (!m_pBuf) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    //
    m_pFieldNames = BufferOf<CString>::Alloc(fieldNames.GetUsed());
    if (!m_pFieldNames) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    size = 0;
    m_pName = m_pBuf;
    strcpy(m_pName, name);
    size = StringAlignSize(name);

    m_pFieldNames->SetUsed(fieldNames.GetUsed());
    for (i = 0; i < fieldNames.GetUsed(); i++) {
        (*m_pFieldNames)[i] = m_pBuf + size;
        strcpy((char *)(const char *)(*m_pFieldNames)[i], fieldNames[i]);
        size += StringAlignSize(fieldNames[i]);
    }

    //
    m_pFieldTypeInfos = fieldTypeInfos.Clone();
    if (!m_pFieldTypeInfos) {
        ec = E_OUT_OF_MEMORY;
        goto EExit;
    }

    m_pFieldTypeInfos->SetUsed(m_pFieldTypeInfos->GetUsed());
    for (i = 0; i < m_pFieldTypeInfos->GetUsed(); i++) {
        (*m_pFieldTypeInfos)[i]->AddRef();
    }

    ec = InitFieldElement();
    if (FAILED(ec)) goto EExit;

    ec = InitFieldInfos();
    if (FAILED(ec)) goto EExit;

    return NOERROR;

EExit:
    if (m_pBuf) {
        delete m_pBuf;
        m_pBuf = NULL;
    }

    if (m_pFieldNames) {
        BufferOf<CString>::Free(m_pFieldNames);
        m_pFieldNames = NULL;
    }

    if (m_pFieldTypeInfos) {
		for (i = 0; i < m_pFieldTypeInfos->GetUsed(); i++) {
            if ((*m_pFieldTypeInfos)[i]) (*m_pFieldTypeInfos)[i]->Release();
        }

        BufferOf<IDataTypeInfo *>::Free(m_pFieldTypeInfos);
        m_pFieldTypeInfos = NULL;
    }

    return ec;
}

ECode CStructRefInfo::GetName(
    /* [out] */ StringBuf * pName)
{
    if (pName == NULL || !pName->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pName->Copy(m_pName);
    return NOERROR;
}

ECode CStructRefInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    if (!pSize) {
        return E_INVALID_ARGUMENT;
    }

    *pSize = m_uSize;
    return NOERROR;
}

ECode CStructRefInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    if (!pDataType) {
        return E_INVALID_ARGUMENT;
    }

    *pDataType = CarDataType_Struct;
    return NOERROR;
}

ECode CStructRefInfo::GetModuleInfo(
    /* [out] */ IModuleInfo ** ppModuleInfo)
{
    if (m_pCClsModule) {
        return m_pCClsModule->GetModuleInfo(ppModuleInfo);
    }
    else {
        return E_INVALID_OPERATION;
    }
}

ECode CStructRefInfo::GetFieldCount(
    /* [out] */ Int32 * pCount)
{
    if (!pCount) {
        return E_INVALID_ARGUMENT;
    }

    *pCount = m_pFieldNames->GetUsed();

    return NOERROR;
}

ECode CStructRefInfo::GetAllFieldInfos(
    /* [out] */ BufferOf<IFieldInfo *> * pFieldInfos)
{
    if (!pFieldInfos || !pFieldInfos->GetCapacity()) {
        return E_INVALID_ARGUMENT;
    }

    pFieldInfos->Copy(m_pFieldInfos);
    for (int i = 0; i < pFieldInfos->GetUsed(); i++) {
        (*pFieldInfos)[i]->AddRef();
    }

    return NOERROR;
}

ECode CStructRefInfo::GetFieldInfo(
    /* [in] */ CString name,
    /* [out] */ IFieldInfo ** ppFieldInfo)
{
    if (name.IsNull() || !ppFieldInfo) {
        return E_INVALID_ARGUMENT;
    }

    for (int i = 0; i < m_pFieldNames->GetUsed(); i++) {
        if ((*m_pFieldNames)[i].Compare(name) == 0) {
            *ppFieldInfo = (IFieldInfo *)(*m_pFieldInfos)[i];
            (*ppFieldInfo)->AddRef();
            return NOERROR;
        }
    }

    return E_DOES_NOT_EXIST;
}

ECode CStructRefInfo::GetMaxAlignSize(
    /* [out] */ MemorySize * pAlignSize)
{
    Int32 size = 0, align = 1;

    CarDataType dataType;
    for (int i = 0; i < m_pFieldTypeInfos->GetUsed(); i++) {
        ((IDataTypeInfo *)(*m_pFieldTypeInfos)[i])->GetDataType(&dataType);
        if (dataType == CarDataType_Struct) {
            ((CStructRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&size);
        }
        else if (dataType == CarDataType_ArrayOf
                || dataType == CarDataType_BufferOf
                || dataType == CarDataType_MemoryBuf) {
            ((CCarArrayRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&size);
        }
        else if (dataType == CarDataType_CppVector) {
            ((CCppVectorRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&size);
        }
        else {
            ((IDataTypeInfo *)(*m_pFieldTypeInfos)[i])->GetSize(&size);
        }

        if (size > align) align = size;
    }

    if (align > ALIGN_BOUND) align = ALIGN_BOUND;

    *pAlignSize = align;

    return NOERROR;
}

ECode CStructRefInfo::InitFieldInfos()
{
    Int32 i = 0, count = m_pFieldTypeInfos->GetUsed();
    m_pFieldInfos = BufferOf<IFieldInfo *>::Alloc(count);
    if (m_pFieldInfos == NULL) {
        return E_OUT_OF_MEMORY;
    }
    memset(m_pFieldInfos->GetPayload(), 0, count * sizeof(PInterface));
    m_pFieldInfos->SetUsed(count);

    for (i = 0; i < count; i++) {
        (*m_pFieldInfos)[i] = new CFieldInfo(this, (*m_pFieldNames)[i],
            (IDataTypeInfo*)(*m_pFieldTypeInfos)[i]);
        if (!(*m_pFieldInfos)[i]) goto EExit;
    }

    return NOERROR;

EExit:
    for (i = 0; i < count; i++) {
        if ((*m_pFieldInfos)[i]) delete (*m_pFieldInfos)[i];
    }
    BufferOf<IFieldInfo *>::Free(m_pFieldInfos);
    m_pFieldInfos = NULL;

    return NOERROR;
}

CAR_INLINE UInt32 AdjuctElemOffset(UInt32 uOffset, UInt32 uElemSize, UInt32 uAlign)
{
    if (uElemSize > uAlign) uElemSize = uAlign;
    UInt32 uMode = uOffset % uElemSize;
    if (uMode != 0) {
        uOffset = uOffset - uMode + uElemSize;
    }

    return uOffset;
}

ECode CStructRefInfo::InitFieldElement()
{
    Int32 count = m_pFieldTypeInfos->GetUsed();
    m_pStructFieldDesc = new StructFieldDesc[count];
    if (!m_pStructFieldDesc) {
        return E_OUT_OF_MEMORY;
    }

    UInt32 uOffset = 0;
    ECode ec = NOERROR;

    CarDataType dataType;
    Int32 align, elemtAlign, elemtSize;
    ec = GetMaxAlignSize(&align);
    if (FAILED(ec)) return ec;

    for (int i = 0; i < count; i++) {
        ((IDataTypeInfo *)(*m_pFieldTypeInfos)[i])->GetDataType(&dataType);
        if (dataType == CarDataType_Struct) {
            ((CStructRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&elemtAlign);
            ((CStructRefInfo *)(*m_pFieldTypeInfos)[i])->GetSize(&elemtSize);
        }
        else if (dataType == CarDataType_ArrayOf
                || dataType == CarDataType_BufferOf
                || dataType == CarDataType_MemoryBuf) {
            ((CCarArrayRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&elemtAlign);
            ((CCarArrayRefInfo *)(*m_pFieldTypeInfos)[i])->GetSize(&elemtSize);
        }
        else if (dataType == CarDataType_CppVector) {
            ((CCppVectorRefInfo *)(*m_pFieldTypeInfos)[i]) \
                ->GetMaxAlignSize(&elemtAlign);
            ((CCppVectorRefInfo *)(*m_pFieldTypeInfos)[i])->GetSize(&elemtSize);
        }
        else {
            ((IDataTypeInfo *)(*m_pFieldTypeInfos)[i])->GetSize(&elemtSize);
            elemtAlign = align;
        }

        m_pStructFieldDesc[i].type = dataType;
        m_pStructFieldDesc[i].size = elemtSize;
        //Adjuct the Element Size
        uOffset = AdjuctElemOffset(uOffset, elemtSize, elemtAlign);

        m_pStructFieldDesc[i].pos = uOffset;
        uOffset += m_pStructFieldDesc[i].size;

        m_pStructFieldDesc[i].pszName = (char *)(const char *)(*m_pFieldNames)[i];
    }

    m_uSize = AdjuctElemOffset(uOffset, align, align);

    return NOERROR;
}

ECode CStructRefInfo::CreateVarBox(
    /* [in] */ PVoid variableDescriptor,
    /* [out] */ IVariableOfStruct ** ppVariable)
{
    CVariableOfStruct *pStructBox = new CVariableOfStruct();
    if (pStructBox == NULL) {
        return E_OUT_OF_MEMORY;
    }

    ECode ec = pStructBox->Init(this, variableDescriptor);
    if (FAILED(ec)) {
        delete pStructBox;
        return ec;
    }

    *ppVariable = (IVariableOfStruct *)pStructBox;
    pStructBox->AddRef();

    return NOERROR;
}

ECode CStructRefInfo::CreateVariable(
    /* [out] */ IVariableOfStruct ** ppVariable)
{
    if (!ppVariable) {
        return E_INVALID_ARGUMENT;
    }

    return CreateVarBox(NULL, ppVariable);
}

ECode CStructRefInfo::CreateVariableBox(
    /* [in] */ PVoid variableDescriptor,
    /* [out] */ IVariableOfStruct ** ppVariable)
{
    if (!variableDescriptor || !ppVariable) {
        return E_INVALID_ARGUMENT;
    }

    return CreateVarBox(variableDescriptor, ppVariable);
}
