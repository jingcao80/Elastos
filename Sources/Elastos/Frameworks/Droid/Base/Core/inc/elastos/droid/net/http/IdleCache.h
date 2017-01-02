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

#ifndef __ELASTOS_DROID_NET_HTTP_IDLECACHE_H__
#define __ELASTOS_DROID_NET_HTTP_IDLECACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;

using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
class IdleCache
    : public Object
{
public:
    class Entry
        : public Object
    {
    public:
        AutoPtr<IHttpHost> mHost;
        AutoPtr<IConnection> mConnection;
        Int64 mTimeout;
    };

private:
    class IdleReaper
        : public Thread
    {
    public:
        IdleReaper(
            /* [in] */ IdleCache* host);

        CARAPI Run();

    private:
        IdleCache* mHost;
    };

public:
    IdleCache();

    CARAPI constructor();

    /**
     * Caches connection, if there is room.
     * @return true if connection cached
     */
    CARAPI CacheConnection(
        /* [in] */ IHttpHost* host,
        /* [in] */ IConnection* connection,
        /* [out] */ Boolean* result);

    CARAPI GetConnection(
        /* [in] */ IHttpHost* host,
        /* [out] */ IConnection** result);

    CARAPI Clear();

private:
    CARAPI ClearIdle();

private:
    static const Int32 IDLE_CACHE_MAX;

    /* Allow five consecutive empty queue checks before shutdown */
    static const Int32 EMPTY_CHECK_MAX;

    /* six second timeout for connections */
    static const Int32 TIMEOUT;

    static const Int32 CHECK_INTERVAL;

    AutoPtr<ArrayOf<Entry*> > mEntries;

    Int32 mCount;

    AutoPtr<IdleReaper> mThread;

    /* stats */
    Int32 mCached;

    Int32 mReused;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Net::Http::IdleCache::Entry, IInterface)

#endif // __ELASTOS_DROID_NET_HTTP_IDLECACHE_H__
