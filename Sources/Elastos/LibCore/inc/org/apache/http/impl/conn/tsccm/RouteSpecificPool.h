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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ROUTESPECIFICPOOL_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ROUTESPECIFICPOOL_H__

#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"

using Elastos::Core::IRunnable;
using Elastos::Utility::IQueue;
using Elastos::Utility::ILinkedList;
using Org::Apache::Http::Conn::Routing::IHttpRoute;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

class WaitingThread;

/**
 * A connection sub-pool for a specific route, used by {@link ConnPoolByRoute}.
 * The methods in this class are unsynchronized. It is expected that the
 * containing pool takes care of synchronization.
 */
class RouteSpecificPool : public Object
{
public:
    /**
     * Instantiates a new worker to listen for lost connections.
     *
     * @param queue     the queue on which to wait for references
     * @param handler   the handler to pass the references to
     */
    RouteSpecificPool(
        /* [in] */ IHttpRoute* route,
        /* [in] */ Int32 maxEntries);

    /**
     * Obtains the route for which this pool is specific.
     *
     * @return  the route
     */
    CARAPI_(AutoPtr<IHttpRoute>) GetRoute();

    /**
     * Obtains the maximum number of entries allowed for this pool.
     *
     * @return  the max entry number
     */
    CARAPI_(Int32) GetMaxEntries();

    /**
     * Indicates whether this pool is unused.
     * A pool is unused if there is neither an entry nor a waiting thread.
     * All entries count, not only the free but also the allocated ones.
     *
     * @return  <code>true</code> if this pool is unused,
     *          <code>false</code> otherwise
     */
    CARAPI_(Boolean) IsUnused();

    /**
     * Return remaining capacity of this pool
     *
     * @return capacity
     */
    CARAPI_(Int32) GetCapacity();

    /**
     * Obtains the number of entries.
     * This includes not only the free entries, but also those that
     * have been created and are currently issued to an application.
     *
     * @return  the number of entries for the route of this pool
     */
    CARAPI_(Int32) GetEntryCount();

    /**
     * Obtains a free entry from this pool, if one is available.
     *
     * @return an available pool entry, or <code>null</code> if there is none
     */
    CARAPI_(AutoPtr<BasicPoolEntry>) AllocEntry(
        /* [in] */ IObject* state);

    /**
     * Returns an allocated entry to this pool.
     *
     * @param entry     the entry obtained from {@link #allocEntry allocEntry}
     *                  or presented to {@link #createdEntry createdEntry}
     */
    CARAPI FreeEntry(
        /* [in] */ BasicPoolEntry* entry);

    /**
     * Indicates creation of an entry for this pool.
     * The entry will <i>not</i> be added to the list of free entries,
     * it is only recognized as belonging to this pool now. It can then
     * be passed to {@link #freeEntry freeEntry}.
     *
     * @param entry     the entry that was created for this pool
     */
    CARAPI CreatedEntry(
        /* [in] */ BasicPoolEntry* entry);

    /**
     * Deletes an entry from this pool.
     * Only entries that are currently free in this pool can be deleted.
     * Allocated entries can not be deleted.
     *
     * @param entry     the entry to delete from this pool
     *
     * @return  <code>true</code> if the entry was found and deleted, or
     *          <code>false</code> if the entry was not found
     */
    CARAPI_(Boolean) DeleteEntry(
        /* [in] */ BasicPoolEntry* entry);

    /**
     * Forgets about an entry from this pool.
     * This method is used to indicate that an entry
     * {@link #allocEntry allocated}
     * from this pool has been lost and will not be returned.
     */
    CARAPI DropEntry();

    /**
     * Adds a waiting thread.
     * This pool makes no attempt to match waiting threads with pool entries.
     * It is the caller's responsibility to check that there is no entry
     * before adding a waiting thread.
     *
     * @param wt        the waiting thread
     */
    CARAPI QueueThread(
        /* [in] */ WaitingThread* wt);

    /**
     * Checks whether there is a waiting thread in this pool.
     *
     * @return  <code>true</code> if there is a waiting thread,
     *          <code>false</code> otherwise
     */
    CARAPI_(Boolean) HasThread();

    /**
     * Returns the next thread in the queue.
     *
     * @return  a waiting thread, or <code>null</code> if there is none
     */
    CARAPI_(AutoPtr<WaitingThread>) NextThread();

    /**
     * Removes a waiting thread, if it is queued.
     *
     * @param wt        the waiting thread
     */
    CARAPI_(void) RemoveThread(
        /* [in] */ WaitingThread* wt);

protected:
    /** The route this pool is for. */
    AutoPtr<IHttpRoute> mRoute;

    /** the maximum number of entries allowed for this pool */
    Int32 mMaxEntries;

    /**
     * The list of free entries.
     * This list is managed LIFO, to increase idle times and
     * allow for closing connections that are not really needed.
     */
    AutoPtr<ILinkedList> mfreeEntries;

    /** The list of threads waiting for this pool. */
    AutoPtr<IQueue> mWaitingThreads;

    /** The number of created entries. */
    Int32 mNumEntries;

private:
    // private final Log log = LogFactory.getLog(getClass());
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ROUTESPECIFICPOOL_H__
