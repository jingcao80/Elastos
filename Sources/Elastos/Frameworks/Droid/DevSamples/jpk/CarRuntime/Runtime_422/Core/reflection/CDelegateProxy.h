//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CDELEGATEPROXY_H__
#define __CDELEGATEPROXY_H__

#include "refutil.h"

class CDelegateProxy : public IDelegateProxy
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

    CARAPI GetTargetObject(
        /* [out] */ PVoid * pTargetObject);

    CARAPI GetTargetMethod(
        /* [out] */ PVoid * pTargetMethod);

    CARAPI GetCallbackInvocation(
        /* [out] */ ICallbackInvocation ** ppCallbackInvocation);

#ifndef _arm
    CARAPI OnEvent(
        /* [in] */ PInterface pServer);
#else
    CARAPI OnEvent(
        /* [in] */ PInterface pServer, ...);
#endif

    CARAPI EventHander(
        /* [in] */ PVoid pParamBuf,
        /* [out] */ UInt32 *pParamBufSize);

    CARAPI GetDelegate(
        /* [out] */ EventHandler * pHandler);

    CDelegateProxy(
        /* [in] */ ICallbackMethodInfo * pCallbackMethodInfo,
        /* [in] */ ICallbackInvocation * pCallbackInvocation,
        /* [in] */ PVoid targetObject,
        /* [in] */ PVoid targetMethod);

    virtual ~CDelegateProxy();

private:
    ICallbackMethodInfo * m_pCallbackMethodInfo;
    ICallbackInvocation * m_pCallbackInvocation;
    PVoid                 m_pTargetObject;
    PVoid                 m_pTargetMethod;

    Int32                 m_cRef;
};

#endif // __CDELEGATEPROXY_H__
