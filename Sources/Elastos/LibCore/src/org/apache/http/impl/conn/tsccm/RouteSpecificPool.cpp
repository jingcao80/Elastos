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

#include "org/apache/http/impl/conn/tsccm/RouteSpecificPool.h"
#include "org/apache/http/impl/conn/tsccm/WaitingThread.h"
#include "elastos/utility/CLinkedList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::CLinkedList;
using Elastos::Utility::IListIterator;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::IOperatedClientConnection;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

RouteSpecificPool::RouteSpecificPool(
    /* [in] */ IHttpRoute* route,
    /* [in] */ Int32 maxEntries)
    : mRoute(route)
    , mMaxEntries(maxEntries)
    , mNumEntries(0)
{
    CLinkedList::New((ILinkedList**)&mfreeEntries);
    AutoPtr<ILinkedList> list;
    CLinkedList::New((ILinkedList**)&list);
    mWaitingThreads = IQueue::Probe(list);
}

AutoPtr<IHttpRoute> RouteSpecificPool::GetRoute()
{
    return mRoute;
}

Int32 RouteSpecificPool::GetMaxEntries()
{
    return mMaxEntries;
}

Boolean RouteSpecificPool::IsUnused()
{
    Boolean isEmpty;
    return (mNumEntries < 1) && (mWaitingThreads->IsEmpty(&isEmpty), isEmpty);
}

Int32 RouteSpecificPool::GetCapacity()
{
    return mMaxEntries - mNumEntries;
}

Int32 RouteSpecificPool::GetEntryCount()
{
    return mNumEntries;
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreinterpret-base-class"
#endif

AutoPtr<BasicPoolEntry> RouteSpecificPool::AllocEntry(
    /* [in] */ IObject* state)
{
    Boolean isEmpty;
    if (mfreeEntries->IsEmpty(&isEmpty), !isEmpty) {
        Int32 size;
        mfreeEntries->GetSize(&size);
        AutoPtr<IListIterator> it;
        mfreeEntries->GetListIterator(size, (IListIterator**)&it);
        Boolean hasPrevious;
        while(it->HasPrevious(&hasPrevious), hasPrevious) {
            AutoPtr<IInterface> value;
            it->GetPrevious((IInterface**)&value);
            AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
            if (state != NULL) {
                AutoPtr<IObject> entryState = entry->GetState();
                Boolean equals;
                if (state->Equals(entryState, &equals), equals) {
                    IIterator::Probe(it)->Remove();
                    return entry;
                }
            }
        }
    }
    if (mfreeEntries->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IInterface> value;
        mfreeEntries->Remove((IInterface**)&value);
        AutoPtr<BasicPoolEntry> entry = reinterpret_cast<BasicPoolEntry*>(value->Probe(EIID_BasicPoolEntry));
        entry->SetState(NULL);
        AutoPtr<IOperatedClientConnection> conn = entry->GetConnection();
        // try {
        IHttpConnection::Probe(conn)->Close();
        // } catch (IOException ex) {
        //     log.debug("I/O error closing connection", ex);
        // }
        return entry;
    }
    return NULL;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

ECode RouteSpecificPool::FreeEntry(
    /* [in] */ BasicPoolEntry* entry)
{
    if (mNumEntries < 1) {
        Logger::E("RouteSpecificPool", "No entry created for this pool. %p", mRoute.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 size;
    if (mfreeEntries->GetSize(&size), mNumEntries <= size) {
        Logger::E("RouteSpecificPool", "No entry allocated for this pool. %p", mRoute.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mfreeEntries->Add(entry->Probe(EIID_IInterface));
    return NOERROR;
}

ECode RouteSpecificPool::CreatedEntry(
    /* [in] */ BasicPoolEntry* entry)
{
    AutoPtr<IHttpRoute> plannedRoute = entry->GetPlannedRoute();
    Boolean equals;
    if (IObject::Probe(mRoute)->Equals(plannedRoute, &equals), !equals) {
        Logger::E("RouteSpecificPool", "Entry not planned for this pool.\npool: %p\nplan: %p", mRoute.Get(), plannedRoute.Get());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mNumEntries++;
    return NOERROR;
}

Boolean RouteSpecificPool::DeleteEntry(
    /* [in] */ BasicPoolEntry* entry)
{
    Boolean found;
    mfreeEntries->Remove(entry->Probe(EIID_IInterface), &found);
    if (found)
        mNumEntries--;
    return found;
}

ECode RouteSpecificPool::DropEntry()
{
    if (mNumEntries < 1) {
        Logger::E("RouteSpecificPool", "There is no entry that could be dropped.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mNumEntries--;
    return NOERROR;
}

ECode RouteSpecificPool::QueueThread(
    /* [in] */ WaitingThread* wt)
{
    if (wt == NULL) {
        Logger::E("RouteSpecificPool", "Waiting thread must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mWaitingThreads->Add(wt->Probe(EIID_IInterface));
    return NOERROR;
}

Boolean RouteSpecificPool::HasThread()
{
    Boolean isEmpty;
    mWaitingThreads->IsEmpty(&isEmpty);
    return !isEmpty;
}

AutoPtr<WaitingThread> RouteSpecificPool::NextThread()
{
    AutoPtr<IInterface> value;
    mWaitingThreads->Peek((IInterface**)&value);
    return (WaitingThread*)(Object*)(IObject*)value.Get();
}

void RouteSpecificPool::RemoveThread(
    /* [in] */ WaitingThread* wt)
{
    if (wt == NULL) return;

    Boolean result;
    mWaitingThreads->Remove(wt->Probe(EIID_IInterface), &result);
}


} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org