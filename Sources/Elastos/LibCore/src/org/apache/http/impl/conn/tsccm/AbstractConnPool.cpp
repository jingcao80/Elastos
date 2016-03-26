
#include "org/apache/http/impl/conn/tsccm/AbstractConnPool.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntryRef.h"
#include "elastos/core/CThread.h"
#include "elastos/utility/CHashSet.h"
#include "elastos/utility/CLinkedList.h"
#include "elastos/utility/concurrent/locks/CReentrantLock.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

AbstractConnPool::AbstractConnPool()
    : mNumConnections(0)
    , mIsShutDown(FALSE)
{
    AutoPtr<IHashSet> hs;
    CHashSet::New((IHashSet**)&hs);
    mIssuedConnections = ISet::Probe(hs);
    mIdleConnHandler = new IdleConnectionHandler();

    // boolean fair = false; //@@@ check parameters to decide
    CReentrantLock::New(FALSE, (IReentrantLock**)&mPoolLock);
}

CAR_INTERFACE_IMPL(AbstractConnPool, Object, IRefQueueHandler)

ECode AbstractConnPool::EnableConnectionGC()
{
    if (mRefQueue != NULL) {
        Logger::E("AbstractConnPool", "Connection GC already enabled.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    ILock::Probe(mPoolLock)->Lock();
    // try {
    if (mNumConnections > 0) { //@@@ is this check sufficient?
        Logger::E("AbstractConnPool", "Pool already in use.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // } finally {
    //     poolLock.unlock();
    // }
    ILock::Probe(mPoolLock)->UnLock();

    AutoPtr<ILinkedList> list;
    CLinkedList::New((ILinkedList**)&list); // new ReferenceQueue<Object>();
    mRefQueue = IQueue::Probe(list);
    mRefWorker = new RefQueueWorker(mRefQueue, this);
    AutoPtr<IThread> t;
    CThread::New((IRunnable*)mRefWorker, (IThread**)&t); //@@@ use a thread factory
    t->SetDaemon(TRUE);
    String s;
    ToString(&s);
    t->SetName(String("RefQueueWorker@") + s);
    t->Start();
    return NOERROR;
}

ECode AbstractConnPool::GetEntry(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* tunit,
    /* [out] */ BasicPoolEntry** entry)
{
    VALIDATE_NOT_NULL(entry)

    AutoPtr<IInterface> value;
    RequestPoolEntry(route, state)->GetPoolEntry(timeout, tunit, (IInterface**)&value);
    *entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
    REFCOUNT_ADD(*entry)
    return NOERROR;
}

ECode AbstractConnPool::HandleReference(
    /* [in] */ IInterface* ref)
{
    ILock::Probe(mPoolLock)->Lock();
    // try {

    AutoPtr<BasicPoolEntryRef> entryRef = reinterpret_cast<BasicPoolEntryRef*>(ref->Probe(EIID_BasicPoolEntryRef));
    if (entryRef != NULL) {
        // check if the GCed pool entry was still in use
        //@@@ find a way to detect this without lookup
        //@@@ flag in the BasicPoolEntryRef, to be reset when freed?
        Boolean lost;
        mIssuedConnections->Remove(ref, &lost);
        if (lost) {
            AutoPtr<IHttpRoute> route = entryRef->GetRoute();
            // if (log.isDebugEnabled()) {
            //     log.debug("Connection garbage collected. " + route);
            // }
            HandleLostEntry(route);
        }
    }

    // } finally {
    //     poolLock.unlock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return NOERROR;
}

ECode AbstractConnPool::CloseIdleConnections(
    /* [in] */ Int64 idletime,
    /* [in] */ ITimeUnit* tunit)
{
    // idletime can be 0 or negative, no problem there
    if (tunit == NULL) {
        Logger::E("AbstractConnPool", "Time unit must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ILock::Probe(mPoolLock)->Lock();
    // try {
    Int64 millis;
    tunit->ToMillis(idletime, &millis);
    mIdleConnHandler->CloseIdleConnections(millis);
    // } finally {
    //     poolLock.unlock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return NOERROR;
}

ECode AbstractConnPool::CloseExpiredConnections()
{
    ILock::Probe(mPoolLock)->Lock();
    // try {
    mIdleConnHandler->CloseExpiredConnections();
    // } finally {
    //     poolLock.unlock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return NOERROR;
}

void AbstractConnPool::Shutdown()
{
    ILock::Probe(mPoolLock)->Lock();
    // try {

    if (mIsShutDown) {
        ILock::Probe(mPoolLock)->UnLock();
        return;
    }

    // no point in monitoring GC anymore
    if (mRefWorker != NULL)
        mRefWorker->Shutdown();

    // close all connections that are issued to an application
    AutoPtr<IIterator> iter;
    mIssuedConnections->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        AutoPtr<IWeakReference> per;
        if (IWeakReferenceSource::Probe(value)) {
            IWeakReferenceSource::Probe(value)->GetWeakReference((IWeakReference**)&per);
        }
        iter->Remove();
        AutoPtr<IInterface> entryInt;
        per->Resolve(EIID_BasicPoolEntry,  (IInterface**)&entryInt);
        AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(entryInt->Probe(EIID_BasicPoolEntry));
        if (entry != NULL) {
            AutoPtr<IOperatedClientConnection> conn = entry->GetConnection();
            CloseConnection(conn);
        }
    }

    // remove all references to connections
    //@@@ use this for shutting them down instead?
    mIdleConnHandler->RemoveAll();

    mIsShutDown = TRUE;

    // } finally {
    //     poolLock.unlock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

ECode AbstractConnPool::CloseConnection(
    /* [in] */ IOperatedClientConnection* conn)
{
    if (conn != NULL) {
        // try {
        IHttpConnection::Probe(conn)->Close();
        // } catch (IOException ex) {
        //     log.debug("I/O error closing connection", ex);
        // }
    }
    return NOERROR;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org