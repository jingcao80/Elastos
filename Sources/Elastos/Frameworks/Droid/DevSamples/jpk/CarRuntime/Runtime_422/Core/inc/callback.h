
#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#ifndef INVALID_ADDRVALUE
#define INVALID_ADDRVALUE           ((virtaddr_t)(0xcccccccc))
#endif

ELAPI_(void) _Impl_CallbackSink_FreeCallbackEvent(
            _ELASTOS PVoid pCallbackEvent);

#define THREAD_STACK_LIMIT  (0x80000000 - 0x400000)
#ifdef _neptune
#define THREAD_STACK_BASE   0x77D00000
#else
#define THREAD_STACK_BASE   0x60000000
#endif

#include <semaphore.h>
#include <time.h>
#include <elaatomics.h>

typedef _ELASTOS Int32 CallbackEventId;
typedef _ELASTOS Int32 CallbackEventFlags;

#define CallbackEventFlag_PriorityMask  0x000000FF
#define CallbackEventFlag_Normal        0x00000000
#define CallbackEventFlag_Duplicated    0x00000100
#define CallbackEventFlag_SyncCall      0x00000200
#define CallbackEventFlag_DirectCall    0x00000400

#define CallingStatus_Created       0x01
#define CallingStatus_Running       0x02
#define CallingStatus_Completed     0x03
#define CallingStatus_Cancelled     0x04

typedef _ELASTOS ECode (ELFUNCCALLTYPE *PCallbackEventHandleRoutine)(_ELASTOS PVoid pvThis);

typedef _ELASTOS ECode (ELFUNCCALLTYPE *PCoalesceEventFunc)(_ELASTOS PVoid* ppOld, _ELASTOS PVoid* ppNew);

#ifdef __cplusplus

class _EzCallbackEvent
{
public:
    _EzCallbackEvent()
        : m_id(0)
        , m_flags(0)
        , m_pSender(NULL)
        , m_pReceiver(NULL)
        , m_pCoalesceFunc(NULL)
        , m_pHandlerThis(NULL)
        , m_pHandlerFunc(NULL)
        , m_typeOfFunc(0)
        , m_ecRet(NOERROR)
        , m_Status(CallingStatus_Created)
        , m_bCompleted(0)
        , m_pParameters(NULL)
        , m_pPrev(NULL)
        , m_pNext(NULL)
        , m_when(0)
        , m_cRef(0)
    {
        sem_init(&m_pSyncEvent, 0, 0);
        AddRef();
    }

    _EzCallbackEvent(
        CallbackEventId id,
        CallbackEventFlags flags,
        PInterface pSender,
        PInterface pReceiver,
        _ELASTOS PVoid pCoalesceFunc,
        _ELASTOS PVoid pHandlerThis,
        _ELASTOS PVoid pHandlerFunc,
        _ELASTOS Int32 typeOfFunc,
        IParcel* pParameters)
        : m_id(id)
        , m_flags(flags)
        , m_pSender(pSender)
        , m_pReceiver(pReceiver)
        , m_pCoalesceFunc(pCoalesceFunc)
        , m_pHandlerThis(pHandlerThis)
        , m_pHandlerFunc(pHandlerFunc)
        , m_typeOfFunc(typeOfFunc)
        , m_ecRet(NOERROR)
        , m_Status(CallingStatus_Created)
        , m_bCompleted(0)
        , m_pParameters(pParameters)
        , m_pPrev(NULL)
        , m_pNext(NULL)
        , m_when(0)
        , m_cRef(0)
    {
        if (m_pSender) m_pSender->AddRef();
        if (m_pParameters) m_pParameters->AddRef();
        sem_init(&m_pSyncEvent, 0, 0);
        AddRef();
    }

    virtual ~_EzCallbackEvent()
    {
        if (m_pSender) m_pSender->Release();
        if (m_pParameters) m_pParameters->Release();

        sem_post(&m_pSyncEvent);
        sem_destroy(&m_pSyncEvent);
    }

    _ELASTOS UInt32 AddRef()
    {
        _ELASTOS Int32 ref = atomic_inc(&m_cRef);
        return (_ELASTOS UInt32)ref;
    }

    _ELASTOS UInt32 Release()
    {
        _ELASTOS Int32 nRef = atomic_dec(&m_cRef);
        if (nRef == 0) _Impl_CallbackSink_FreeCallbackEvent(this);
        return nRef;
    }

    _EzCallbackEvent *Prev() const { return m_pPrev; }

    // Return the next node.
    _EzCallbackEvent *Next() const { return m_pNext; }

    _EzCallbackEvent *Detach()
    {
        m_pPrev->m_pNext = m_pNext;
        m_pNext->m_pPrev = m_pPrev;
    #ifdef _DEBUG
        m_pPrev = m_pNext = (_EzCallbackEvent *)INVALID_ADDRVALUE;
    #endif

        return this;
    }

    void Initialize() { m_pPrev = m_pNext = this; }

    _ELASTOS Boolean IsEmpty() const { return this == m_pNext; }

    _EzCallbackEvent *First() const { return Next(); }

    _EzCallbackEvent *InsertNext(_EzCallbackEvent *pNextNode)
    {
        assert(pNextNode);

        pNextNode->m_pPrev  = this;
        pNextNode->m_pNext  = m_pNext;
        m_pNext->m_pPrev    = pNextNode;
        m_pNext             = pNextNode;

        return pNextNode;
    }

    _EzCallbackEvent *InsertFirst(_EzCallbackEvent *pFirstNode)
    {
        return InsertNext(pFirstNode);
    }

public:
    CallbackEventId             m_id;
    CallbackEventFlags          m_flags;
    PInterface                  m_pSender;
    PInterface                  m_pReceiver;
    _ELASTOS PVoid              m_pCoalesceFunc;
    _ELASTOS PVoid              m_pHandlerThis;
    _ELASTOS PVoid              m_pHandlerFunc;
    _ELASTOS Int32              m_typeOfFunc;
    _ELASTOS ECode              m_ecRet;
    sem_t                       m_pSyncEvent;
    _ELASTOS Int16              m_Status;
    _ELASTOS Int16              m_bCompleted;
    IParcel*                    m_pParameters;
    _EzCallbackEvent            *m_pPrev;
    _EzCallbackEvent            *m_pNext;
    _ELASTOS Millisecond64      m_when;
private:
    _ELASTOS Int32              m_cRef;
};

typedef class _EzCallbackEvent  *PCallbackEvent;

#else

typedef _ELASTOS PVoid PCallbackEvent;

#endif // __cplusplus

ELAPI _Impl_CallbackSink_InitCallbackContext(
            PInterface *ppCallbackContext);

ELAPI _Impl_CallbackSink_GetCallbackContext(
            PInterface *ppCallbackContext);

ELAPI _Impl_CallbackSink_AcquireCallbackContext(
            PInterface *ppCallbackContext);

ELAPI_(PCallbackEvent) _Impl_CallbackSink_AllocCallbackEvent(
            _ELASTOS MemorySize size);


ELAPI _Impl_CallbackSink_GetThreadEvent(
            PInterface pCallbackContext, sem_t* ppEvent);

ELAPI _Impl_CallbackSink_PostCallbackEvent(
            PInterface pCallbackContext,
            PCallbackEvent pCallbackEvent);

ELAPI _Impl_CallbackSink_PostCallbackEventAtTime(
            PInterface pCallbackContext,
            PCallbackEvent pCallbackEvent,
            _ELASTOS Millisecond64 uptimeMillis);

ELAPI _Impl_CallbackSink_SendCallbackEvent(
            PInterface pCallbackContext,
            PCallbackEvent pCallbackEvent,
            _ELASTOS Millisecond32 timeOut);

ELAPI _Impl_CallbackSink_WaitForCallbackEvent(
            PInterface pCallbackContext,
            _ELASTOS Millisecond32 msTimeOut,
            WaitResult* pResult,
            _ELASTOS Boolean *pbEventOccured,
            _ELASTOS Flags32 fPriority);

ELAPI _Impl_CallbackSink_CleanupAllCallbacks(
    IInterface *pCallbackContext);

ELAPI _Impl_CallbackSink_CancelAllPendingCallbacks(
            PInterface pCallbackContext,
            PInterface pSender);

ELAPI _Impl_CallbackSink_CancelPendingCallback(
            PInterface pCallbackContext,
            PInterface pSender,
            CallbackEventId id);

ELAPI _Impl_CallbackSink_CancelCallbackEvents(
            PInterface pCallbackContext,
            PInterface pSender,
            CallbackEventId id,
            _ELASTOS PVoid pHandlerThis,
            _ELASTOS PVoid pHandlerFunc);

ELAPI _Impl_CallbackSink_RequestToFinish(
    PInterface pCallbackContext, _ELASTOS Int32 Flag);

ELAPI_(_ELASTOS Int32) _Impl_CallbackSink_GetStatus(
    PInterface pCallbackContext);

ELAPI_(_ELASTOS Int32) _Impl_CallbackSink_TryToHandleEvents(IInterface* pCallbackContext);

ELAPI _Impl_CCallbackRendezvous_New(
        PInterface pCallbackContext,
        ICallbackSink* pCallbackSink,
        CallbackEventId eventId,
        _ELASTOS Boolean* pbEventFlag,
        _ELASTOS Boolean bNewCallback,
        ICallbackRendezvous** ppICallbackRendezvous);

#ifdef __cplusplus

//
//  struct DelegateNode
//
struct DelegateNode
{
    DelegateNode() : m_pCallbackContext(NULL) {}

    DelegateNode(PInterface pCallbackContext, _ELASTOS EventHandler delegate)
        : m_pCallbackContext(pCallbackContext), m_delegate(delegate)
    {
        assert(pCallbackContext);
    }

    _ELASTOS ECode CancelCallbackEvents(PInterface pSender, CallbackEventId id)
    {
        return _Impl_CallbackSink_CancelCallbackEvents(
                    m_pCallbackContext, pSender, id,
                    m_delegate.GetThisPtr(),
                    m_delegate.GetFuncPtr());
    }

    DelegateNode *Prev()
    {
        return m_pPrev;
    }

    DelegateNode* Next()
    {
        return m_pNext;
    }

    DelegateNode *InsertPrev(DelegateNode *pPrevNode)
    {
        assert(pPrevNode);

        pPrevNode->m_pPrev  = m_pPrev;
        pPrevNode->m_pNext  = this;
        m_pPrev->m_pNext    = pPrevNode;
        m_pPrev             = pPrevNode;

        return pPrevNode;
    }

    DelegateNode* Detach()
    {
        m_pPrev->m_pNext = m_pNext;
        m_pNext->m_pPrev = m_pPrev;
    #ifdef _DEBUG
        m_pPrev = m_pNext = (DelegateNode *)INVALID_ADDRVALUE;
    #endif

        return this;
    }

    void Initialize()
    {
        m_pPrev = m_pNext = this;
    }

    _ELASTOS Boolean IsEmpty()
    {
        return this == m_pNext;
    }

    DelegateNode *First()
    {
        return Next();
    }

    IInterface     *m_pCallbackContext;
    _ELASTOS EventHandler  m_delegate;
    DelegateNode      *m_pPrev;
    DelegateNode      *m_pNext;
};

class DelegateContainer
{
public:
    DelegateContainer();

    _ELASTOS ECode Current(
            DelegateNode **ppNode);

    _ELASTOS ECode MoveNext();

    _ELASTOS ECode Reset();

    _ELASTOS ECode Add(PInterface pCallbackContext, const _ELASTOS EventHandler & delegate);

    _ELASTOS ECode Remove(
            _ELASTOS EventHandler & delegate, PInterface pSender, CallbackEventId id);

    _ELASTOS ECode Dispose(
            PInterface pSender, CallbackEventId id);

    ~DelegateContainer();

public:
    DelegateNode  m_head;
    DelegateNode  *m_pCurrent;
};

CAR_INLINE DelegateContainer::DelegateContainer()
{
    m_head.Initialize();
    m_pCurrent = &m_head;
}

CAR_INLINE DelegateContainer::~DelegateContainer()
{
    DelegateNode *pNode;

    while (!m_head.IsEmpty()) {
        pNode = (DelegateNode *)m_head.First();
        pNode->Detach();
        delete pNode;
    }
    m_pCurrent = NULL;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::Dispose(PInterface pSender,
    CallbackEventId id)
{
    DelegateNode *pNode;

    while (!m_head.IsEmpty()) {
        pNode = (DelegateNode *)m_head.First();
        pNode->Detach();
        pNode->CancelCallbackEvents(pSender, id);
        delete pNode;
    }
    m_pCurrent = NULL;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::Current(
    DelegateNode **ppNode)
{
    assert(NULL != ppNode);

    if (NULL == m_pCurrent || &m_head == m_pCurrent) {
        return E_INVALID_OPERATION;
    }
    *ppNode = m_pCurrent;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::MoveNext()
{
    if (NULL == m_pCurrent) {
        return E_INVALID_OPERATION;
    }
    if (m_pCurrent == m_head.Prev()) {
        m_pCurrent = NULL;
        return S_FALSE;
    }
    m_pCurrent = m_pCurrent->Next();
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::Reset()
{
    m_pCurrent = &m_head;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::Add(
    PInterface pCallbackContext,
    const _ELASTOS EventHandler & delegate)
{
    DelegateNode *pNode;

    assert(pCallbackContext);

    pNode = new DelegateNode(pCallbackContext, delegate);
    if (NULL == pNode) {
        return E_OUT_OF_MEMORY;
    }

    m_head.InsertPrev(pNode);
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode DelegateContainer::Remove(
    _ELASTOS EventHandler & delegate, PInterface pSender, CallbackEventId id)
{
    DelegateNode *pNode;
    _ELASTOS ECode ec = NOERROR;

    for (pNode = m_head.m_pNext; pNode != &m_head; pNode = pNode->m_pNext) {
        if (pNode->m_delegate == delegate) {
            if (m_pCurrent == pNode) {
                m_pCurrent = m_pCurrent->Prev();
            }
            pNode->Detach();
            ec = pNode->CancelCallbackEvents(pSender, id);
            delete pNode;
            break;
        }
    }

    return ec;
}

//
//  struct CallbackContextNode
//
struct CallbackContextNode
{
    CallbackContextNode() : m_pCallbackContext(NULL){}

    CallbackContextNode(PInterface pCallbackContext, _ELASTOS Boolean bOccured)
        : m_pCallbackContext(pCallbackContext), m_bEventOccured(bOccured),
        m_bEmpty(TRUE)
    {
        assert(m_pCallbackContext);

        m_pCallbackContext->AddRef();
    }

    ~CallbackContextNode()
    {
        if (m_pCallbackContext) m_pCallbackContext->Release();
    }

    void Initialize();

    CallbackContextNode *Prev();

    CallbackContextNode *Next();

    CallbackContextNode *InsertPrev(CallbackContextNode *pPrevNode);

    CallbackContextNode *Detach();

    _ELASTOS Boolean IsEmpty();

    _ELASTOS ECode MoveNext();

    _ELASTOS ECode Reset();

    _ELASTOS ECode Current(
            _ELASTOS EventHandler **ppDelegate);

    _ELASTOS ECode Add(const _ELASTOS EventHandler & delegate);

    _ELASTOS ECode Remove(
            _ELASTOS EventHandler & delegate, PInterface pSender, CallbackEventId id);

    _ELASTOS ECode Dispose(
            PInterface pSender, CallbackEventId id);

    IInterface     *m_pCallbackContext;
    DelegateContainer m_delegateContainer;
    _ELASTOS Boolean m_bEventOccured;
    _ELASTOS Boolean m_bEmpty;
    CallbackContextNode      *m_pPrev;
    CallbackContextNode      *m_pNext;
};

CAR_INLINE void CallbackContextNode::Initialize()
{
    m_pPrev = m_pNext = this;
}

CAR_INLINE CallbackContextNode *CallbackContextNode::Prev()
{
    return m_pPrev;
}

CAR_INLINE CallbackContextNode *CallbackContextNode::Next()
{
    return m_pNext;
}

CAR_INLINE CallbackContextNode *CallbackContextNode::InsertPrev(CallbackContextNode *pPrevNode)
{
    assert(pPrevNode);

    pPrevNode->m_pPrev  = m_pPrev;
    pPrevNode->m_pNext  = this;
    m_pPrev->m_pNext    = pPrevNode;
    m_pPrev             = pPrevNode;

    return pPrevNode;
}

CAR_INLINE CallbackContextNode *CallbackContextNode::Detach()
{
    m_pPrev->m_pNext = m_pNext;
    m_pNext->m_pPrev = m_pPrev;
#ifdef _DEBUG
    m_pPrev = m_pNext = (CallbackContextNode *)INVALID_ADDRVALUE;
#endif

    return this;
}

CAR_INLINE _ELASTOS Boolean CallbackContextNode::IsEmpty()
{
    return this == m_pNext;
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::MoveNext()
{
    return m_delegateContainer.MoveNext();
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::Reset()
{
    return m_delegateContainer.Reset();
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::Dispose(PInterface pSender,
    CallbackEventId id)
{
    _ELASTOS ECode ec = m_delegateContainer.Dispose(pSender, id);
    if (FAILED(ec)) return ec;
    Detach();
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::Current(
    _ELASTOS EventHandler **ppDelegate)
{
    _ELASTOS ECode ec;
    DelegateNode *pNode;

    ec = m_delegateContainer.Current(&pNode);
    if (NOERROR == ec) {
        *ppDelegate = &pNode->m_delegate;
    }
    return ec;
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::Add(const _ELASTOS EventHandler & delegate)
{
    m_bEmpty = FALSE;
    return m_delegateContainer.Add(m_pCallbackContext, delegate);
}

CAR_INLINE _ELASTOS ECode CallbackContextNode::Remove(
    _ELASTOS EventHandler & delegate, PInterface pSender, CallbackEventId id)
{
    _ELASTOS ECode ec = m_delegateContainer.Remove(delegate, pSender, id);
    if (FAILED(ec)) return ec;

    if (m_delegateContainer.m_head.IsEmpty()) {
        m_bEmpty = TRUE;
        Detach();
    }
    return NOERROR;
}

//
//  class CallbackContextContainer
//
class CallbackContextContainer
{
public:
    _ELASTOS ECode Find(
            PInterface pCallbackContext, CallbackContextNode **ppNode);

    _ELASTOS ECode Current(
            CallbackContextNode **ppNode);

    _ELASTOS ECode MoveNext();

    _ELASTOS ECode Add(PInterface pCallbackContext, const _ELASTOS EventHandler & delegate);

    _ELASTOS ECode Remove(CallbackContextNode *pNode);

    _ELASTOS ECode Remove(
            PInterface pCallbackContext, _ELASTOS EventHandler & delegate,
            PInterface pSender, CallbackEventId id);

    _ELASTOS ECode Dispose(
            PInterface pCallbackContext, PInterface pSender, CallbackEventId id);

    _ELASTOS ECode Reset();

    CallbackContextContainer():m_bEventOccured(FALSE)
    {
        m_head.Initialize();
        m_pCurrent = &m_head;
    }

    ~CallbackContextContainer();

public:
    CallbackContextNode  m_head;
    CallbackContextNode  *m_pCurrent;
    _ELASTOS Boolean  m_bEventOccured;
};

CAR_INLINE CallbackContextContainer::~CallbackContextContainer()
{
    CallbackContextNode *pNode;

    Reset();
    assert(m_head.IsEmpty() && "You should call CFoo::RemoveAllCallbacks first.");

    while (MoveNext() == NOERROR) {
        if (Current(&pNode) != NOERROR) {
            break;
        }
        Remove(pNode);
        delete pNode;
    }
    assert(m_head.IsEmpty());
    m_pCurrent = NULL;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Find(
    PInterface pCallbackContext, CallbackContextNode **ppNode)
{
    CallbackContextNode *pCurrNode;

    for (pCurrNode = m_head.m_pNext; pCurrNode != &m_head; pCurrNode = pCurrNode->m_pNext) {
        if (pCurrNode->m_pCallbackContext == pCallbackContext) {
            *ppNode = pCurrNode;
            return NOERROR;
        }
    }
    *ppNode = NULL;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Dispose(
    PInterface pCallbackContext, PInterface pSender, CallbackEventId id)
{
    CallbackContextNode *pNode;

    pNode = m_head.Next();
    while (&m_head != pNode) {
        if (pNode->m_pCallbackContext == pCallbackContext) {
            pNode->Dispose(pSender, id);
            delete pNode;
            break;
        }
        pNode = pNode->Next();
    }
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Current(
    CallbackContextNode **ppNode)
{
    assert(NULL != ppNode);

    if (NULL == m_pCurrent || &m_head == m_pCurrent) {
        return E_INVALID_OPERATION;
    }
    *ppNode = m_pCurrent;
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::MoveNext()
{
    if (NULL == m_pCurrent) {
        return E_INVALID_OPERATION;
    }
    if (m_pCurrent == m_head.Prev()) {
        m_pCurrent = NULL;
        return S_FALSE;
    }
    m_pCurrent = m_pCurrent->Next();
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Add(
    PInterface pCallbackContext,
    const _ELASTOS EventHandler & delegate)
{
    _ELASTOS Boolean bFound = FALSE;
    CallbackContextNode *pNode;
    _ELASTOS ECode ec;

    if (NULL == pCallbackContext) {
        ec = _Impl_CallbackSink_GetCallbackContext(&pCallbackContext);
        if (FAILED(ec)) return ec;
    }
    else {
        pCallbackContext->AddRef();
    }

    for (pNode = m_head.m_pNext; pNode != &m_head; pNode = pNode->m_pNext) {
        if (pNode->m_pCallbackContext == pCallbackContext) {
            bFound = TRUE;
            break;
        }
    }

    if (!bFound) {
        pNode = new CallbackContextNode(pCallbackContext, FALSE);
        if (NULL == pNode) {
            pCallbackContext->Release();
            return E_OUT_OF_MEMORY;
        }
        m_head.InsertPrev(pNode);
    }

    ec = pNode->Add(delegate);
    pCallbackContext->Release();

    return ec;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Remove(CallbackContextNode *pNode)
{
    if (m_pCurrent == pNode) {
        m_pCurrent = m_pCurrent->Prev();
    }
    pNode->Detach();
    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Remove(
    PInterface pCallbackContext, _ELASTOS EventHandler & delegate,
    PInterface pSender, CallbackEventId id)
{
    assert(pCallbackContext);

    CallbackContextNode *pNode;

    for (pNode = m_head.m_pNext; pNode != &m_head; pNode = pNode->m_pNext) {
        if (pNode->m_pCallbackContext == pCallbackContext) {
            _ELASTOS ECode ec = pNode->Remove(delegate, pSender, id);
            if (FAILED(ec)) return ec;
            if (pNode->m_bEmpty) delete pNode;
            return NOERROR;
        }
    }

    return NOERROR;
}

CAR_INLINE _ELASTOS ECode CallbackContextContainer::Reset()
{
    m_pCurrent = &m_head;
    return NOERROR;
}

#endif // __cplusplus

#endif //__CALLBACK_H__
