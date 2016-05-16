//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CSTRUCTVARIABLE_H__
#define __CSTRUCTVARIABLE_H__

#include "refutil.h"

class CVariableOfStruct :
    public IVariableOfStruct,
    public IStructSetter,
    public IStructGetter
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetPayload(
        /* [out] */ PVoid * pPayload);

    CARAPI Rebox(
        /* [in] */ PVoid localVariablePtr);

    CARAPI GetSetter(
        /* [out] */ IStructSetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ IStructGetter ** ppGetter);

    CARAPI Init(
        /* [in] */ IStructInfo *pStructInfo,
        /* [in] */ PVoid pVarBuf);

//--------------Setter----------------------------------------------------------

    CARAPI ZeroAllFields();

    CARAPI SetInt16Field(
        /* [in] */ CString name,
        /* [in] */ Int16 value);

    CARAPI SetInt32Field(
        /* [in] */ CString name,
        /* [in] */ Int32 value);

    CARAPI SetInt64Field(
        /* [in] */ CString name,
        /* [in] */ Int64 value);

    CARAPI SetByteField(
        /* [in] */ CString name,
        /* [in] */ Byte value);

    CARAPI SetFloatField(
        /* [in] */ CString name,
        /* [in] */ Float value);

    CARAPI SetDoubleField(
        /* [in] */ CString name,
        /* [in] */ Double value);

    CARAPI SetChar8Field(
        /* [in] */ CString name,
        /* [in] */ Char8 value);

    CARAPI SetChar16Field(
        /* [in] */ CString name,
        /* [in] */ Char16 value);

    CARAPI SetChar32Field(
        /* [in] */ CString name,
        /* [in] */ Char32 value);

    CARAPI SetBooleanField(
        /* [in] */ CString name,
        /* [in] */ Boolean value);

    CARAPI SetEMuidField(
        /* [in] */ CString name,
        /* [in] */ EMuid * pValue);

    CARAPI SetEGuidField(
        /* [in] */ CString name,
        /* [in] */ EGuid * pValue);

    CARAPI SetECodeField(
        /* [in] */ CString name,
        /* [in] */ ECode value);

    CARAPI SetLocalPtrField(
        /* [in] */ CString name,
        /* [in] */ LocalPtr value);

    CARAPI SetLocalTypeField(
        /* [in] */ CString name,
        /* [in] */ PVoid value);

    CARAPI SetEnumField(
        /* [in] */ CString name,
        /* [in] */ Int32 value);

    CARAPI GetStructFieldSetter(
        /* [in] */ CString name,
        /* [out] */ IStructSetter ** ppSetter);

    CARAPI GetCppVectorFieldSetter(
        /* [in] */ CString name,
        /* [out] */ ICppVectorSetter ** ppSetter);

    CARAPI SetFieldValueByName(
        /* [in] */ CString name,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

//--------------Getter----------------------------------------------------------

    CARAPI GetInt16Field(
        /* [in] */ CString name,
        /* [out] */ Int16 * pValue);

    CARAPI GetInt32Field(
        /* [in] */ CString name,
        /* [out] */ Int32 * pValue);

    CARAPI GetInt64Field(
        /* [in] */ CString name,
        /* [out] */ Int64 * pValue);

    CARAPI GetByteField(
        /* [in] */ CString name,
        /* [out] */ Byte * pValue);

    CARAPI GetFloatField(
        /* [in] */ CString name,
        /* [out] */ Float * pValue);

    CARAPI GetDoubleField(
        /* [in] */ CString name,
        /* [out] */ Double * pValue);

    CARAPI GetChar8Field(
        /* [in] */ CString name,
        /* [out] */ Char8 * pValue);

    CARAPI GetChar16Field(
        /* [in] */ CString name,
        /* [out] */ Char16 * pValue);

    CARAPI GetChar32Field(
        /* [in] */ CString name,
        /* [out] */ Char32 * pValue);

    CARAPI GetBooleanField(
        /* [in] */ CString name,
        /* [out] */ Boolean * pValue);

    CARAPI GetEMuidField(
        /* [in] */ CString name,
        /* [out] */ EMuid * pValue);

    CARAPI GetEGuidField(
        /* [in] */ CString name,
        /* [out] */ EGuid * pValue);

    CARAPI GetECodeField(
        /* [in] */ CString name,
        /* [out] */ ECode * pValue);

    CARAPI GetLocalPtrField(
        /* [in] */ CString name,
        /* [out] */ LocalPtr * pValue);

    CARAPI GetLocalTypeField(
        /* [in] */ CString name,
        /* [out] */ PVoid value);

    CARAPI GetEnumField(
        /* [in] */ CString name,
        /* [out] */ Int32 * pValue);

    CARAPI GetStructFieldGetter(
        /* [in] */ CString name,
        /* [out] */ IStructGetter ** ppGetter);

    CARAPI GetCppVectorFieldGetter(
        /* [in] */ CString name,
        /* [out] */ ICppVectorGetter ** ppGetter);

    CARAPI GetFieldValueByName(
        /* [in] */ CString name,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

    CARAPI GetIndexByName(
        /* [in] */ CString name,
        /* [out] */ Int32 *pIndex);

    CARAPI AcquireCppVectorFieldSGetter(
        /* [in] */ CString name,
        /* [in] */ Boolean bSetter,
        /* [out] */ IInterface ** ppSGetter);

    CVariableOfStruct();

    virtual ~CVariableOfStruct();

private:
    IStructInfo     *m_pStructInfo;
    StructFieldDesc  *m_pStructFieldDesc;
    PByte   m_pVarBuf;
    Boolean m_bAlloc;
    Int32  m_iCount;
    UInt32  m_uVarSize;

    IInterface **m_pCppVectorSGetters;

    Int32 m_cRef;
};

#endif // __CSTRUCTVARIABLE_H__
