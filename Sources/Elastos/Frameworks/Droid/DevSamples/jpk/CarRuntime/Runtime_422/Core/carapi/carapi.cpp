//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <elasys_server.h>
#include <locmod.h>
#include <stdio.h>
#include <elapi.h>
#include <stdlib.h>

#include <_pubcrt.h>
#include <stdio.h>

#define STATUS_WIN32_ERROR(u) ((ECode)((u)|0x80070000))

#define E_ENTRY_NOT_FOUND         STATUS_WIN32_ERROR(1761)

#define ENABLE_DUMP_CLSID    0    // debug info switch
#if ENABLE_DUMP_CLSID
#define DUMP_CLSID(CLSID, info) \
    do { \
        ALOGD("> INSIDE %s\n", info); \
        ALOGD("======== DUMP_CLSID ========\n"); \
        ALOGD("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                CLSID.clsid.Data1, CLSID.clsid.Data2, CLSID.clsid.Data3, \
                CLSID.clsid.Data4[0], CLSID.clsid.Data4[1], \
                CLSID.clsid.Data4[2], CLSID.clsid.Data4[3], \
                CLSID.clsid.Data4[4], CLSID.clsid.Data4[5], \
                CLSID.clsid.Data4[6], CLSID.clsid.Data4[7]); \
        ALOGD("============================\n"); \
    } while(0);
#else
#define DUMP_CLSID(CLSID, info);
#endif

ELAPI _CObject_CreateInstance(
    /* [in] */ RClassID rclsid,
    /* [in] */ PRegime pRegime,
    /* [in] */ REIID riid,
    /* [out] */ PInterface *ppObj)
{
    DUMP_CLSID(rclsid, "_CObject_CreateInstance")
    if (NULL == ppObj || IS_INVALID_REGIME(pRegime)) return E_INVALID_ARGUMENT;

    EzMultiQI mq = { &riid, NULL, 0 };
    ECode ec;

    if (!IS_RGM_NUMBER(pRegime)) {
        return pRegime->CreateObject(rclsid, riid, ppObj);
    }
    else {
        ec = _CObject_CreateInstanceEx(rclsid, pRegime, 1, &mq);
    }

    if (SUCCEEDED(ec)) {
        *ppObj = mq.pObject;
    }

    return ec;
}

ELAPI _CObject_CreateInstanceEx(
    /* [in] */ RClassID rclsid,
    /* [in] */ PRegime pRegime,
    /* [in] */ UInt32 cmq,
    /* [out] */ PMULTIQI pResults)
{
    DUMP_CLSID(rclsid, "_CObject_CreateInstanceEx")
    assert(cmq > 0 && pResults != NULL);
    if (IS_INVALID_REGIME(pRegime)) return E_INVALID_ARGUMENT;

    ECode ec;
    PInterface pObject;
    PClassObject pClsObject = NULL;

    ec = _CObject_AcquireClassFactory(rclsid, pRegime, EIID_IClassObject, (PInterface*)&pClsObject);
    if (FAILED(ec)) return ec;

    pResults->ec = pClsObject->CreateObject(
            NULL, *(pResults->pIID), &pResults->pObject);
    pClsObject->Release();
    if (FAILED(pResults->ec)) return pResults->ec;

    pObject = pResults->pObject;
    for (UInt32 n = 1; n < cmq; n++) {
        pResults++;
        pResults->pObject = pObject->Probe(*(pResults->pIID));
        if (!pResults->pObject) ec = E_NO_INTERFACE;
        else pResults->pObject->AddRef();
        pResults->ec = ec;
    }

    return ec;
}

ELAPI _CObject_AcquireClassFactory(
    /* [in] */ RClassID rclsid,
    /* [in] */ PRegime pRegime,
    /* [in] */ REIID riid,
    /* [out] */ PInterface *ppObject)
{
    DUMP_CLSID(rclsid, "_CObject_AcquireClassFactory")
    if (NULL == ppObject || IS_INVALID_REGIME(pRegime)) return E_INVALID_ARGUMENT;

    if (pRegime == RGM_SAME_DOMAIN) {
        return AcquireClassObjectFromLocalModule(rclsid, riid, ppObject);
    }

    return E_NOT_IMPLEMENTED;
}

ELAPI_(Boolean) _CObject_Compare(
    /* [in] */ PInterface pObjectA,
    /* [in] */ PInterface pObjectB)
{
    Boolean bResult;

    pObjectA = pObjectA->Probe(EIID_IInterface);
    if (!pObjectA) {
        assert(0 && "pObjectA query IInterface failed!");
        return FALSE;
    }
    pObjectB = pObjectB->Probe(EIID_IInterface);
    if (!pObjectB) {
        assert(0 && "pObjectB query IInterface failed!");
        return FALSE;
    }

    bResult = (pObjectA == pObjectB);

    return bResult;
}

ELAPI _CObject_AttachAspect(
    /* [in] */ PInterface pAggregator,
    /* [in] */ RClassID rAspectClsid)
{
    ECode ec;
    IAspect* pAspect = NULL;
    IObject* pObject = NULL;

    pObject = (IObject*)pAggregator->Probe(EIID_IObject);
    assert(pObject);

    ec = _CObject_CreateInstance(rAspectClsid,
                  RGM_SAME_DOMAIN,
                  EIID_IAspect,
                  (PInterface*)&pAspect);
    if(FAILED(ec)) return ec;
    ec = pAspect->AspectAggregate(AggrType_Aggregate, pObject);
    return ec;
}

ELAPI _CObject_DetachAspect(
    /* [in] */ PInterface pAggregator,
    /* [in] */ RClassID rAspectClsid)
{
    IObject* pObject = NULL;

    pObject = (IObject*)pAggregator->Probe(EIID_IObject);
    if (pObject == NULL) return E_NO_INTERFACE;

#ifdef __cplusplus
    return pObject->Aggregate(AggrType_Unaggregate,
                    (PInterface)&rAspectClsid);
#else
    return pObject->Aggregate(AggrType_Unaggregate,
                    (PInterface)rAspectClsid);
#endif
}

ELAPI _CObject_AddCallback(
    PInterface pServerObj,
    Int32 dwEvent,
    EventHandler delegate)
{
    ICallbackSink *pSink;
    ECode ec;

    ec = _CObject_AcquireCallbackSink(pServerObj, &pSink);
    if (FAILED(ec)) return ec;

    ec = pSink->AddCallback(dwEvent, delegate);
    pSink->Release();
    return ec;
}

ELAPI _CObject_RemoveCallback(
    PInterface pServerObj,
    Int32 dwEvent,
    EventHandler delegate)
{
    ICallbackSink *pSink;
    ECode ec;

    ec = _CObject_AcquireCallbackSink(pServerObj, &pSink);
    if (FAILED(ec)) return ec;

    ec = pSink->RemoveCallback(dwEvent, delegate);
    pSink->Release();
    return ec;
}

ELAPI _CObject_RemoveAllCallbacks(PInterface pServerObj)
{
    ICallbackSink *pSink;
    ECode ec;

    do {
        ec = _CObject_AcquireCallbackSink(pServerObj, &pSink);
        if (FAILED(ec)) return ec;
        ec = pSink->RemoveAllCallbacks();
        pSink->Release();

        pServerObj = pServerObj->Probe(EIID_SUPER_OBJECT);
    } while (pServerObj);

    return ec;
}

ELAPI _CObject_AcquireCallbackRendezvous(
    PInterface pServerObj,
    Int32 dwEvent,
    PCallbackRendezvous* ppCallbackRendezvous)
{
    ICallbackSink *pSink;
    ICallbackRendezvous* pICallbackRendezvous = NULL;
    ECode ec;

    ec = _CObject_AcquireCallbackSink(pServerObj, &pSink);
    if (FAILED(ec)) return ec;

    ec = pSink->AcquireCallbackRendezvous(dwEvent, &pICallbackRendezvous);
    pSink->Release();

    *ppCallbackRendezvous = pICallbackRendezvous;

    return ec;
}

ELAPI _CObject_AcquireCallbackSink(
    /* [in] */ PInterface pObj,
    /* [out] */ PCALLBACKSINK *ppSink)
{
    ICallbackConnector *pConnector;

    // Query LOCAL ICallbackConnector interface
    pConnector = (ICallbackConnector *)pObj->Probe(EIID_CALLBACK_CONNECTOR);
    if (!pConnector) return E_NO_INTERFACE;

    return pConnector->AcquireCallbackSink(ppSink);
}

