//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCALLBACKARGLIST_H__
#define __CCALLBACKARGLIST_H__

#include "refutil.h"

class CCallbackArgumentList : public ICallbackArgumentList
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetCallbackMethodInfo(
        /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI GetServerPtrArgument(
        /* [out] */ PInterface * pServer);

    CARAPI GetParamValue(
        /* [in] */ Int32 index,
        /* [in] */ PVoid pParam,
        /* [in] */ CarDataType type);

    CARAPI GetInt16Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI GetInt32Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetInt64Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI GetByteArgument(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI GetFloatArgument(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI GetDoubleArgument(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI GetChar8Argument(
        /* [in] */ Int32 index,
        /* [out] */ Char8 * pValue);

    CARAPI GetChar16Argument(
        /* [in] */ Int32 index,
        /* [out] */ Char16 * pValue);

    CARAPI GetChar32Argument(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI GetCStringArgument(
        /* [in] */ Int32 index,
        /* [out] */ CString* value);

    CARAPI GetStringArgument(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI GetBooleanArgument(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI GetEMuidArgument(
        /* [in] */ Int32 index,
        /* [out] */ EMuid ** ppValue);

    CARAPI GetEGuidArgument(
        /* [in] */ Int32 index,
        /* [out] */ EGuid ** ppValue);

    CARAPI GetECodeArgument(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI GetLocalPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ LocalPtr * pValue);

    CARAPI GetEnumArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetCarArrayArgument(
        /* [in] */ Int32 index,
        /* [out] */ PCarQuintet * pValue);

    CARAPI GetStructPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ PVoid * pValue);

    CARAPI GetObjectPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * pValue);

    CARAPI Init(
        /* [in] */ ICallbackMethodInfo *pCallbackMethodInfo,
        /* [in] */ ParmElement *pParamElem,
        /* [in] */ UInt32 uParamCount,
        /* [in] */ UInt32 uParamBufSize);

    CCallbackArgumentList();
    virtual ~CCallbackArgumentList();

    Byte  *m_pParamBuf;
    UInt32 m_uParamBufSize;

private:
    ParmElement          *m_pParamElem;
    UInt32                m_uParamCount;
    ICallbackMethodInfo  *m_pCallbackMethodInfo;

    Int32                m_cRef;
};

#endif // __CCALLBACKARGLIST_H__
