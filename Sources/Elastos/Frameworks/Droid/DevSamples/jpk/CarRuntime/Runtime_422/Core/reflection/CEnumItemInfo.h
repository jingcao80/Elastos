//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CENUMITEMINFO_H__
#define __CENUMITEMINFO_H__

#include "refutil.h"

class CEnumItemInfo : public IEnumItemInfo
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

    CARAPI GetEnumInfo(
        /* [out] */ IEnumInfo ** ppEnumInfo);

    CARAPI GetValue(
        /* [out] */ Int32 * pValue);

    CEnumItemInfo(
        /* [in] */ IEnumInfo  * pEnumInfo,
        /* [in] */ CString name,
        /* [in] */ Int32 value);

    virtual ~CEnumItemInfo();

private:
    IEnumInfo  *m_pEnumInfo;
    String     m_sName;
    Int32      m_iValue;

    Int32      m_cRef;
};

#endif // __CENUMITEMINFO_H__
