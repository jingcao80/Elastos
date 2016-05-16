//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCALLBACKINVOCATION_H__
#define __CCALLBACKINVOCATION_H__

#include <elastos.h>
#include <clsdef.h>

class CCallbackInvocation : public ICallbackInvocation
{
public:
    CCallbackInvocation(): m_cRef(0){}

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Invoke(
        /* [in] */ PVoid targetObject,
        /* [in] */ PVoid targetMethod,
        /* [in] */ ICallbackArgumentList * pCallbackArgumentList);

private:
    Int32 m_cRef;
};

#endif // __CCALLBACKINVOCATION_H__
