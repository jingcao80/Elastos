//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CSTRUCTINFO_H__
#define __CSTRUCTINFO_H__

#include "CEntryList.h"

class CStructRefInfo : public IStructInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetSize(
        /* [out] */ MemorySize * pSize);

    CARAPI GetDataType(
        /* [out] */ CarDataType * pDataType);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI GetFieldCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllFieldInfos(
        /* [out] */ BufferOf<IFieldInfo *> * pFieldInfos);

    CARAPI GetFieldInfo(
        /* [in] */ CString name,
        /* [out] */ IFieldInfo ** ppFieldInfo);

    CARAPI CreateVariable(
        /* [out] */ IVariableOfStruct ** ppVariable);

    CARAPI CreateVariableBox(
        /* [in] */ PVoid variableDescriptor,
        /* [out] */ IVariableOfStruct ** ppVariable);

    CARAPI InitFieldInfos();

    CARAPI InitFieldElement();

    CARAPI CreateVarBox(
        /* [in] */ PVoid pVarPtr,
        /* [out] */ IVariableOfStruct ** ppVariable);

    CARAPI GetMaxAlignSize(
        /* [out] */ MemorySize * pAlignSize);

    CARAPI InitStatic(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ StructDirEntry *pStructDirEntry);

    CARAPI InitDynamic(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& fieldNames,
        /* [in] */ const BufferOf<IDataTypeInfo *>& fieldTypeInfos);

    CStructRefInfo();

    virtual ~CStructRefInfo();

    BufferOf<CString>    *m_pFieldNames;
    BufferOf<IDataTypeInfo *>     *m_pFieldTypeInfos;
    StructFieldDesc *m_pStructFieldDesc;
    UInt32           m_uSize;

private:
    CClsModule      *m_pCClsModule;
    StructDirEntry  *m_pStructDirEntry;

    BufferOf<IFieldInfo *>     *m_pFieldInfos;
    char            *m_pName;
    char            *m_pBuf;

    Int32 m_cRef;
};

#endif // __CSTRUCTINFO_H__
