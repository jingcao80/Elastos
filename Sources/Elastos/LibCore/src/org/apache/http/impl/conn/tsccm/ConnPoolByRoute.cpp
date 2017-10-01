//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "org/apache/http/impl/conn/tsccm/ConnPoolByRoute.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntryRef.h"
#include "org/apache/http/conn/params/ConnManagerParams.h"
#include "elastos/core/CSystem.h"
#include "elastos/utility/CLinkedList.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/CDate.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::IIterator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::Params::ConnManagerParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

//==============================================================================
// SingleClientConnManager::ConnAdapter
//==============================================================================
ConnPoolByRoute::MyPoolEntryRequest::MyPoolEntryRequest(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [in] */ WaitingThreadAborter* aborter,
    /* [in] */ ConnPoolByRoute* host)
    : mRoute(route)
    , mState(state)
    , mAborter(aborter)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(ConnPoolByRoute::MyPoolEntryRequest, Object, IPoolEntryRequest)

ECode ConnPoolByRoute::MyPoolEntryRequest::GetPoolEntry(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* tunit,
    /* [out] */ IInterface** basicPoolEntry)
{
    VALIDATE_NOT_NULL(basicPoolEntry)
    AutoPtr<BasicPoolEntry> entry;
    mHost->GetEntryBlocking(mRoute, mState, timeout, tunit, mAborter, (BasicPoolEntry**)&entry);
    *basicPoolEntry = entry->Probe(EIID_IInterface);
    REFCOUNT_ADD(*basicPoolEntry)
    return NOERROR;
}

ECode ConnPoolByRoute::MyPoolEntryRequest::AbortRequest()
{
    ILock::Probe(mHost->mPoolLock)->Lock();
    // try {
    mAborter->Abort();
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mHost->mPoolLock)->UnLock();
    return NOERROR;
}


ConnPoolByRoute::ConnPoolByRoute(
    /* [in] */ IClientConnectionOperator* oper,
    /* [in] */ IHttpParams* params)
    : AbstractConnPool()
    , mMaxTotalConnections(0)
{
    if (oper == NULL) {
        Logger::E("ConnPoolByRoute", "Connection operator may not be null");
        assert(0);
        // throw new IllegalArgumentException("Connection operator may not be null");
    }
    mOperator = oper;

    mFreeConnections = CreateFreeConnQueue();
    mWaitingThreads  = CreateWaitingThreadQueue();
    mRouteToPool     = CreateRouteToPoolMap();
    ConnManagerParams::GetMaxTotalConnections(params, &mMaxTotalConnections);
    ConnManagerParams::GetMaxConnectionsPerRoute(params, (IConnPerRoute**)&mConnPerRoute);
}

AutoPtr<IQueue> ConnPoolByRoute::CreateFreeConnQueue()
{
    AutoPtr<ILinkedList> list;
    CLinkedList::New((ILinkedList**)&list);
    return IQueue::Probe(list);
}

AutoPtr<IQueue> ConnPoolByRoute::CreateWaitingThreadQueue()
{
    AutoPtr<ILinkedList> list;
    CLinkedList::New((ILinkedList**)&list);
    return IQueue::Probe(list);
}

AutoPtr<IMap> ConnPoolByRoute::CreateRouteToPoolMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return IMap::Probe(map);
}

AutoPtr<RouteSpecificPool> ConnPoolByRoute::NewRouteSpecificPool(
    /* [in] */ IHttpRoute* route)
{
    Int32 value;
    mConnPerRoute->GetMaxForRoute(route, &value);
    AutoPtr<RouteSpecificPool> pool = new RouteSpecificPool(route, value);
    return pool;
}

AutoPtr<WaitingThread> ConnPoolByRoute::NewWaitingThread(
    /* [in] */ ICondition* cond,
    /* [in] */ RouteSpecificPool* rospl)
{
    AutoPtr<WaitingThread> t = new WaitingThread(cond, rospl);
    return t;
}

AutoPtr<RouteSpecificPool> ConnPoolByRoute::GetRoutePool(
    /* [in] */ IHttpRoute* route,
    /* [in] */ Boolean create)
{
    AutoPtr<RouteSpecificPool> rospl;
    ILock::Probe(mPoolLock)->Lock();
    // try {
    AutoPtr<IInterface> value;
    mRouteToPool->Get(route, (IInterface**)&value);
    if (value != NULL) {
        rospl = (RouteSpecificPool*)(Object*)(IObject*)value.Get();
    }
    if ((rospl == NULL) && create) {
        // no pool for this route yet (or anymore)
        rospl = NewRouteSpecificPool(route);
        mRouteToPool->Put(route, rospl->Probe(EIID_IInterface));
    }

    // } finally {
    //     ILock::Probe(mPoolLock)->UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return rospl;
}

Int32 ConnPoolByRoute::GetConnectionsInPool(
    /* [in] */ IHttpRoute* route)
{
    ILock::Probe(mPoolLock)->Lock();
    // try {
    // don't allow a pool to be created here!
    AutoPtr<RouteSpecificPool> rospl = GetRoutePool(route, FALSE);
    Int32 count = 0;
    if (rospl != NULL) {
        count = rospl->GetEntryCount();
    }

    // } finally {
    //     ILock::Probe(mPoolLock)->UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return count;
}

AutoPtr<IPoolEntryRequest> ConnPoolByRoute::RequestPoolEntry(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state)
{
    AutoPtr<WaitingThreadAborter> aborter = new WaitingThreadAborter();
    AutoPtr<IPoolEntryRequest> request = (IPoolEntryRequest*)new MyPoolEntryRequest(route, state, aborter, this);
    return request;
}

ECode ConnPoolByRoute::GetEntryBlocking(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IObject* state,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* tunit,
    /* [in] */ WaitingThreadAborter* aborter,
    /* [out] */ BasicPoolEntry** poolEntry)
{
    VALIDATE_NOT_NULL(poolEntry)
    *poolEntry = NULL;

    AutoPtr<IDate> deadline;
    if (timeout > 0) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 current, millis;
        system->GetCurrentTimeMillis(&current);
        tunit->ToMillis(timeout, &millis);
        CDate::New(current + millis, (IDate**)&deadline);
    }

    AutoPtr<BasicPoolEntry> entry;
    ILock::Probe(mPoolLock)->Lock();
    // try {

    AutoPtr<RouteSpecificPool> rospl = GetRoutePool(route, TRUE);
    AutoPtr<WaitingThread> waitingThread;

    while (entry == NULL) {
        if (mIsShutDown) {
            Logger::E("ConnPoolByRoute", "Connection pool shut down.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        // if (log.isDebugEnabled()) {
        //     log.debug("Total connections kept alive: " + freeConnections.size());
        //     log.debug("Total issued connections: " + issuedConnections.size());
        //     log.debug("Total allocated connection: " + numConnections + " out of " + maxTotalConnections);
        // }

        // the cases to check for:
        // - have a free connection for that route
        // - allowed to create a free connection for that route
        // - can delete and replace a free connection for another route
        // - need to wait for one of the things above to come true

        entry = GetFreeEntry(rospl, state);
        if (entry != NULL) {
            break;
        }

        Boolean hasCapacity = rospl->GetCapacity() > 0;

        // if (log.isDebugEnabled()) {
        //     log.debug("Available capacity: " + rospl.getCapacity()
        //             + " out of " + rospl.getMaxEntries()
        //             + " [" + route + "][" + state + "]");
        // }

        Boolean isEmpty;
        if (hasCapacity && mNumConnections < mMaxTotalConnections) {
            entry = CreateEntry(rospl, mOperator);
        }
        else if (hasCapacity && (mFreeConnections->IsEmpty(&isEmpty), !isEmpty)) {
            DeleteLeastUsedEntry();
            entry = CreateEntry(rospl, mOperator);
        }
        else {
            // if (log.isDebugEnabled()) {
            //     log.debug("Need to wait for connection" +
            //             " [" + route + "][" + state + "]");
            // }

            if (waitingThread == NULL) {
                AutoPtr<ICondition> condition;
                ILock::Probe(mPoolLock)->NewCondition((ICondition**)&condition);
                waitingThread = NewWaitingThread(condition, rospl);
                aborter->SetWaitingThread(waitingThread);
            }

            Boolean success = FALSE;
            // try {
            rospl->QueueThread(waitingThread);
            mWaitingThreads->Add(waitingThread->Probe(EIID_IInterface));
            waitingThread->Await(deadline, &success);
            // } finally {
            //     // In case of 'success', we were woken up by the
            //     // connection pool and should now have a connection
            //     // waiting for us, or else we're shutting down.
            //     // Just continue in the loop, both cases are checked.
            //     rospl.removeThread(waitingThread);
            //     waitingThreads.remove(waitingThread);
            // }
            rospl->RemoveThread(waitingThread);
            Boolean result;
            mWaitingThreads->Remove(waitingThread->Probe(EIID_IInterface), &result);

            // check for spurious wakeup vs. timeout
            if (!success && (deadline != NULL)) {
                Int64 time;
                deadline->GetTime(&time);
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 current;
                system->GetCurrentTimeMillis(&current);
                if (time <= current) {
                    Logger::E("ConnPoolByRoute", "Timeout waiting for connection");
                    return E_CONNECTION_POOL_TIMEOUT_EXCEPTION;
                }
            }
        }
    } // while no entry

    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    *poolEntry = entry;
    REFCOUNT_ADD(*poolEntry)
    return NOERROR;
}

void ConnPoolByRoute::FreeEntry(
    /* [in] */ BasicPoolEntry* entry,
    /* [in] */ Boolean reusable,
    /* [in] */ Int64 validDuration,
    /* [in] */ ITimeUnit* timeUnit)
{
    AutoPtr<IHttpRoute> route = entry->GetPlannedRoute();
    // if (log.isDebugEnabled()) {
    //     log.debug("Freeing connection" +
    //             " [" + route + "][" + entry.getState() + "]");
    // }

    ILock::Probe(mPoolLock)->Lock();
    // try {
    if (mIsShutDown) {
        // the pool is shut down, release the
        // connection's resources and get out of here
        AutoPtr<IOperatedClientConnection> conn = entry->GetConnection();
        CloseConnection(conn);
        return;
    }

    // no longer issued, we keep a hard reference now
    AutoPtr<IInterface> ref = entry->GetWeakRef()->Probe(EIID_IInterface);
    mIssuedConnections->Remove(ref);

    AutoPtr<RouteSpecificPool> rospl = GetRoutePool(route, TRUE);

    if (reusable) {
        rospl->FreeEntry(entry);
        mFreeConnections->Add(entry->Probe(EIID_IInterface));
        AutoPtr<IOperatedClientConnection> oper = entry->GetConnection();
        AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(oper);
        mIdleConnHandler->Add(conn, validDuration, timeUnit);
    }
    else {
        rospl->DropEntry();
        mNumConnections--;
    }

    NotifyWaitingThread(rospl);
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

AutoPtr<BasicPoolEntry> ConnPoolByRoute::GetFreeEntry(
    /* [in] */ RouteSpecificPool* rospl,
    /* [in] */ IObject* state)
{
    AutoPtr<BasicPoolEntry> entry;
    ILock::Probe(mPoolLock)->Lock();
    // try {
    Boolean done = FALSE;
    while(!done) {
        entry = rospl->AllocEntry(state);
        if (entry != NULL) {
            // if (log.isDebugEnabled()) {
            //     log.debug("Getting free connection"
            //             + " [" + rospl.getRoute() + "][" + state + "]");

            // }
            Boolean result;
            mFreeConnections->Remove(entry->Probe(EIID_IInterface), &result);
            AutoPtr<IOperatedClientConnection> oper = entry->GetConnection();
            AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(oper);
            Boolean valid = mIdleConnHandler->Remove(conn);
            if(!valid) {
                // If the free entry isn't valid anymore, get rid of it
                // and loop to find another one that might be valid.
                // if(log.isDebugEnabled())
                //     log.debug("Closing expired free connection"
                //             + " [" + rospl.getRoute() + "][" + state + "]");
                CloseConnection(oper);
                // We use dropEntry instead of deleteEntry because the entry
                // is no longer "free" (we just allocated it), and deleteEntry
                // can only be used to delete free entries.
                rospl->DropEntry();
                mNumConnections--;
            }
            else {
                AutoPtr<IInterface> ref = entry->GetWeakRef()->Probe(EIID_IInterface);
                mIssuedConnections->Add(ref);
                done = TRUE;
            }

        }
        else {
            done = TRUE;
            // if (log.isDebugEnabled()) {
            //     log.debug("No free connections"
            //             + " [" + rospl.getRoute() + "][" + state + "]");
            // }
        }
    }
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return entry;
}

AutoPtr<BasicPoolEntry> ConnPoolByRoute::CreateEntry(
    /* [in] */ RouteSpecificPool* rospl,
    /* [in] */ IClientConnectionOperator* op)
{
    // if (log.isDebugEnabled()) {
    //     log.debug("Creating new connection [" + rospl.getRoute() + "]");
    // }

    // the entry will create the connection when needed
    AutoPtr<IHttpRoute> route = rospl->GetRoute();
    AutoPtr<BasicPoolEntry> entry = new BasicPoolEntry(op, route, mRefQueue);

    ILock::Probe(mPoolLock)->Lock();
    // try {
    rospl->CreatedEntry(entry);
    mNumConnections++;

    AutoPtr<IInterface> ref = entry->GetWeakRef()->Probe(EIID_IInterface);
    mIssuedConnections->Add(ref);
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
    return entry;
}

void ConnPoolByRoute::DeleteEntry(
    /* [in] */ BasicPoolEntry* entry)
{
    AutoPtr<IHttpRoute> route = entry->GetPlannedRoute();

    // if (log.isDebugEnabled()) {
    //     log.debug("Deleting connection"
    //             + " [" + route + "][" + entry.getState() + "]");
    // }

    ILock::Probe(mPoolLock)->Lock();
    // try {
    AutoPtr<IOperatedClientConnection> operconn = entry->GetConnection();
    CloseConnection(operconn);

    AutoPtr<RouteSpecificPool> rospl = GetRoutePool(route, TRUE);
    rospl->DeleteEntry(entry);
    mNumConnections--;
    if (rospl->IsUnused()) {
        mRouteToPool->Remove((IInterface*)route);
    }

    AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(operconn);
    mIdleConnHandler->Remove(conn);// not idle, but dead
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreinterpret-base-class"
#endif

void ConnPoolByRoute::DeleteLeastUsedEntry()
{
    // try {
    ILock::Probe(mPoolLock)->Lock();

    //@@@ with get() instead of remove, we could
    //@@@ leave the removing to deleteEntry()
    AutoPtr<IInterface> value;
    mFreeConnections->Remove((IInterface**)&value);
    AutoPtr<BasicPoolEntry> entry;
    if (value != NULL) {
        entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
    }

    if (entry != NULL) {
        DeleteEntry(entry);
    }
    // else if (log.isDebugEnabled()) {
    //     log.debug("No free connection to delete.");
    // }
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

void ConnPoolByRoute::HandleLostEntry(
    /* [in] */ IHttpRoute* route)
{
    ILock::Probe(mPoolLock)->Lock();
    // try {
    AutoPtr<RouteSpecificPool> rospl = GetRoutePool(route, TRUE);
    rospl->DropEntry();
    if (rospl->IsUnused()) {
        mRouteToPool->Remove((IInterface*)route);
    }

    mNumConnections--;
    NotifyWaitingThread(rospl);
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

void ConnPoolByRoute::NotifyWaitingThread(
    /* [in] */ RouteSpecificPool* rospl)
{
    //@@@ while this strategy provides for best connection re-use,
    //@@@ is it fair? only do this if the connection is open?
    // Find the thread we are going to notify. We want to ensure that
    // each waiting thread is only interrupted once, so we will remove
    // it from all wait queues before interrupting.
    AutoPtr<WaitingThread> waitingThread;

    ILock::Probe(mPoolLock)->Lock();
    // try {
    if ((rospl != NULL) && rospl->HasThread()) {
        // if (log.isDebugEnabled()) {
        //     log.debug("Notifying thread waiting on pool" +
        //             " [" + rospl.getRoute() + "]");
        // }
        waitingThread = rospl->NextThread();
    }
    else {
        Boolean isEmpty;
        if (mWaitingThreads->IsEmpty(&isEmpty), !isEmpty) {
            // if (log.isDebugEnabled()) {
            //     log.debug("Notifying thread waiting on any pool");
            // }
            AutoPtr<IInterface> value;
            mWaitingThreads->Remove((IInterface**)&value);
            waitingThread = (WaitingThread*)(Object*)(IObject*)value.Get();
        }
    }
    // else if (log.isDebugEnabled()) {
    //     log.debug("Notifying no-one, there are no waiting threads");
    // }

    if (waitingThread != NULL) {
        waitingThread->Wakeup();
    }
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreinterpret-base-class"
#endif

void ConnPoolByRoute::DeleteClosedConnections()
{
    ILock::Probe(mPoolLock)->Lock();
    // try {
    AutoPtr<IIterator> iter;
    mFreeConnections->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while(iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
        Boolean isOpen;
        if (IHttpConnection::Probe(entry->GetConnection())->IsOpen(&isOpen), !isOpen) {
            iter->Remove();
            DeleteEntry(entry);
        }
    }
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

void ConnPoolByRoute::Shutdown()
{
    ILock::Probe(mPoolLock)->Lock();
    // try {
    AbstractConnPool::Shutdown();

    // close all free connections
    //@@@ move this to base class?
    AutoPtr<IIterator> iter;
    mFreeConnections->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while(iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
        iter->Remove();
        AutoPtr<IOperatedClientConnection> oper = entry->GetConnection();
        CloseConnection(oper);
    }

    // wake up all waiting threads
    AutoPtr<IIterator> iwth;
    mWaitingThreads->GetIterator((IIterator**)&iwth);
    while(iwth->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iwth->GetNext((IInterface**)&value);
        AutoPtr<WaitingThread> waiter = (WaitingThread*)(Object*)(IObject*)value.Get();
        iwth->Remove();
        waiter->Wakeup();
    }

    mRouteToPool->Clear();
    // } finally {
    //     poolLock.UnLock();
    // }
    ILock::Probe(mPoolLock)->UnLock();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org