//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCALLBACKCONTEXT_H__
#define __CCALLBACKCONTEXT_H__

#include <elastos.h>
#include <clsdef.h>
#include <semaphore.h>
#include <sys/time.h>

_ELASTOS_NAMESPACE_USING

EXTERN void _Impl_CallbackSink_FreeCallbackEvent(PCallbackEvent pCallbackEvent);

class CCallbackContext : public IInterface
{
public:
    CARAPI_(PInterface) Probe(REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(IInterface *pObject, InterfaceID *pIID);

public:
    CCallbackContext()
    {
        m_bExitRequested = FALSE;
        m_bRequestToQuit = FALSE;

        m_eventQueue.Initialize();
        m_currentEvents.Initialize();
        m_Status = CallbackContextStatus_Created;
        m_nEventsCount = 0;
        m_cRef = 0;
    }

    ECode Initialize()
    {
        pthread_mutexattr_t recursiveAttr;

        pthread_mutexattr_init(&recursiveAttr);
        pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
        m_ppThreadEvent = &m_pThreadEvent;
        ECode ec = sem_init(&m_pThreadEvent, 0, 0);

        if (FAILED(ec)) return ec;
        if (pthread_mutex_init(&m_workingLock, &recursiveAttr)) goto Exit;
        if (pthread_mutex_init(&m_queueLock, &recursiveAttr)) goto Exit;
    Exit:
        pthread_mutexattr_destroy(&recursiveAttr);
        return ec;
    }

    virtual ~CCallbackContext()
    {
        CancelAllCallbackEvents();

        pthread_mutex_destroy(&m_workingLock);
        pthread_mutex_destroy(&m_queueLock);

        sem_destroy(&m_pThreadEvent);
    }

    ECode PostCallbackEvent(
            PCallbackEvent pCallbackEvent);
            
    ECode SendCallbackEvent(
            PCallbackEvent pCallbackEvent,
            Millisecond32 timeOut);

    ECode RequestToFinish(Int32 flag);

    ECode CancelAllPendingCallbacks(
        PInterface pSender);

    ECode CancelPendingCallback(
        PInterface pSender,
        CallbackEventId id);

    ECode CancelCallbackEvents(
        PInterface pSender,
        CallbackEventId id,
        PVoid pHandlerThis,
        PVoid pHandlerFunc);

    Int32 HandleCallbackEvents(
        Millisecond32 msTimeOut = INFINITE,
        WaitResult *pResult = NULL,
        Boolean * pbOccured = NULL,
        Flags32 fPriority = 0);

    void CancelAllCallbackEvents();

private:

    PCallbackEvent PopEvent();

    void PushEvent(PCallbackEvent pEvent);

    Boolean IsExistEvent(PCallbackEvent pEvent);

    Boolean IsExistEvent(
        PInterface pSender,
        CallbackEventId id,
        PVoid pHandlerThis,
        PVoid pHandlerFunc);

    PCallbackEvent GetEvent(Flags32 fPriority);

public:
    pthread_mutex_t             m_workingLock;
    pthread_mutex_t             m_queueLock;
    Boolean                     m_bExitRequested;
    Boolean                     m_bRequestToQuit;
    _EzCallbackEvent            m_eventQueue;
    _EzCallbackEvent            m_currentEvents;
    sem_t                       m_pThreadEvent;
    sem_t                       *m_ppThreadEvent;
    Int32                       m_Status;
    Int32                       m_nEventsCount;

private:
    Int32                       m_cRef;
};

#endif // __CCALLBACKCONTEXT_H__
