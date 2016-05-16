//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __AOBJECTEX_H__
#define __AOBJECTEX_H__

#include "_AObjectEx.h"

class AObjectEx : public _AObjectEx
{
public:
    CARAPI Equals(
        /* [in] */ IInterface *pObject);

    CARAPI GetHashCode(
        /* [out] */ int *pHashCode);

    CARAPI GetClassID(
        /* [out] */ ClassID *pClsid);

    CARAPI ToString(
        /* [out] */ StringBuf * pString);

    CARAPI OnAggregated(PInterface pOuter);

    CARAPI OnUnaggregated(PInterface pOuter);

    AObjectEx() : m_nHashCode(0), m_pString(NULL) {}

private:
    Int32           m_nHashCode;
    ClassID         m_clsid;
    StringBuf *     m_pString;
};

#endif // __AOBJECTEX_H__
