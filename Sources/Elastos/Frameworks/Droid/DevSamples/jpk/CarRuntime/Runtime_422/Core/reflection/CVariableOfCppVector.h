//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CVARIABLEOFCPPVECTOR_H__
#define __CVARIABLEOFCPPVECTOR_H__

#include "CClsModule.h"

class CVariableOfCppVector :
    public ICppVectorSetter,
    public ICppVectorGetter
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

//--------------Setter----------------------------------------------------------
    CARAPI ZeroAllElements();

    CARAPI SetAllElements(
        /* [in] */ PVoid value,
        /* [in] */ MemorySize size);

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

    CARAPI SetChar8Element(
        /* [in] */ Int32 index,
        /* [in] */ Char8 value);

    CARAPI SetChar16Element(
        /* [in] */ Int32 index,
        /* [in] */ Char16 value);

    CARAPI SetChar32Element(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value);

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

    CARAPI SetLocalPtrElement(
        /* [in] */ Int32 index,
        /* [in] */ LocalPtr value);

    CARAPI SetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [in] */ PVoid value);

    CARAPI SetEnumElement(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetStructElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ IStructSetter ** ppSetter);

    CARAPI GetCppVectorElementSetter(
        /* [in] */ Int32 index,
        /* [out] */ ICppVectorSetter ** ppSetter);

    CARAPI SetElementValue(
        /* [in] */ Int32 index,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

//--------------Getter----------------------------------------------------------

    CARAPI GetLength(
        /* [out] */ Int32 *pLength);

    CARAPI GetRank(
        /* [out] */ Int32 *pRank);

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

    CARAPI GetChar8Element(
        /* [in] */ Int32 index,
        /* [out] */ Char8 * pValue);

    CARAPI GetChar16Element(
        /* [in] */ Int32 index,
        /* [out] */ Char16 * pValue);

    CARAPI GetChar32Element(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

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

    CARAPI GetLocalPtrElement(
        /* [in] */ Int32 index,
        /* [out] */ LocalPtr * pValue);

    CARAPI GetLocalTypeElement(
        /* [in] */ Int32 index,
        /* [out] */ PVoid value);

    CARAPI GetEnumElement(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetStructElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ IStructGetter ** ppGetter);

    CARAPI GetCppVectorElementGetter(
        /* [in] */ Int32 index,
        /* [out] */ ICppVectorGetter ** ppGetter);

    CARAPI GetElementValue(
        /* [in] */ Int32 index,
        /* [in] */ void *pParam,
        /* [in] */ CarDataType type);

    CARAPI GetSetter(
        /* [out] */ ICppVectorSetter ** ppSetter);

    CARAPI GetGetter(
        /* [out] */ ICppVectorGetter ** ppGetter);

    CARAPI Rebox(PVoid pVarPtr);

    CARAPI AcquireCppVectorSGetter(
        /* [in] */ Int32 index,
        /* [in] */ Boolean bSetter,
        /* [out] */ IInterface ** ppSGetter);

    CARAPI Init();

    CVariableOfCppVector(
        /* [in] */ IDataTypeInfo  *pElementTypeInfo,
        /* [in] */ Int32 length,
        /* [in] */ PVoid pVarPtr);

    virtual ~CVariableOfCppVector();

private:
    IDataTypeInfo  *m_pElementTypeInfo;
    CarDataType     m_dataType;
    PVoid           m_pVarPtr;
    UInt32          m_uSize;
    Int32           m_iElementSize;
    Int32           m_iLength;

    IInterface **m_pCppVectorSGetters;

    Int32           m_cRef;
};

#endif // __CVARIABLEOFCPPVECTOR_H__
