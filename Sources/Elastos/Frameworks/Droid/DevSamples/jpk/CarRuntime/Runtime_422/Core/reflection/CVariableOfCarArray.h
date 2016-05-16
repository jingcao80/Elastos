//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCARARRAYVARIABLE_H__
#define __CCARARRAYVARIABLE_H__

#include "CClsModule.h"

class CVariableOfCarArray :
    public IVariableOfCarArray,
    public ICarArraySetter,
    public ICarArrayGetter
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
        /* [out] */ ICarArraySetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ ICarArrayGetter ** ppGetter);

    CARAPI SetUsed(
        /* [in] */ Int32 used);

    CARAPI SetInt16Element(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value);

    CARAPI SetInt32Element(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetInt64Element(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI SetByteElement(
        /* [in] */ Int32 index,
        /* [in] */ Byte value);

    CARAPI SetFloatElement(
        /* [in] */ Int32 index,
        /* [in] */ Float value);

    CARAPI SetDoubleElement(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI SetEnumElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI SetChar8Element(
        /* [in] */ Int32 index,
        /* [in] */ Char8 value);

    CARAPI SetChar16Element(
        /* [in] */ Int32 index,
        /* [in] */ Char16 value);

    CARAPI SetChar32Element(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

    CARAPI SetCStringElement(
        /* [in] */ Int32 index,
        /* [in] */ CString value);

    CARAPI SetStringElement(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI SetBooleanElement(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    CARAPI SetEMuidElement(
        /* [in] */ Int32 index,
        /* [in] */ EMuid * pValue);

    CARAPI SetEGuidElement(
        /* [in] */ Int32 index,
        /* [in] */ EGuid * pValue);

    CARAPI SetECodeElement(
        /* [in] */ Int32 index,
        /* [in] */ ECode value);

    CARAPI SetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI SetObjectPtrElement(
        /* [in] */ Int32 index,
        /* [in] */ PInterface value);

    CARAPI GetStructElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ IStructSetter ** ppSetter);

    CARAPI GetCapacity(
        /* [out] */ Int32 *pCapacity);

    CARAPI GetUsed(
        /* [out] */ Int32 *pUsed);

    CARAPI IsEmpty(
        /* [out] */ Boolean *pIsEmpty);

    CARAPI GetInt16Element(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI GetInt32Element(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetInt64Element(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI GetByteElement(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI GetFloatElement(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI GetDoubleElement(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI GetEnumElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32* pValue);

    CARAPI GetChar8Element(
        /* [in] */ Int32 index,
        /* [out] */ Char8 * pValue);

    CARAPI GetChar16Element(
        /* [in] */ Int32 index,
        /* [out] */ Char16 * pValue);

    CARAPI GetChar32Element(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI GetCStringElement(
        /* [in] */ Int32 index,
        /* [out] */ CString* pValue);

    CARAPI GetStringElement(
        /* [in] */ Int32 index,
        /* [out] */ String* pValue);

    CARAPI GetBooleanElement(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI GetEMuidElement(
        /* [in] */ Int32 index,
        /* [out] */ EMuid * pValue);

    CARAPI GetEGuidElement(
        /* [in] */ Int32 index,
        /* [out] */ EGuid * pValue);

    CARAPI GetECodeElement(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI GetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI GetObjectPtrElement(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * pValue);

    CARAPI GetStructElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ IStructGetter ** ppGetter);

    CARAPI SetElementValue(
        /* [in] */ Int32 index,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

    CARAPI GetElementValue(
        /* [in] */ Int32 index,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

    CVariableOfCarArray(
        /* [in] */ ICarArrayInfo *pTypeInfo,
        /* [in] */ PCarQuintet pVarPtr,
        /* [in] */ Boolean bAlloc);

    virtual ~CVariableOfCarArray();

private:
    ICarArrayInfo   *m_pCarArrayInfo;
    PCarQuintet     m_pCq;
    Boolean         m_bAlloc;
    Int32           m_iElementSize;
    Int32           m_iLength;

    CarDataType     m_dataType;

    Int32           m_cRef;
};

#endif // __CCARARRAYVARIABLE_H__
