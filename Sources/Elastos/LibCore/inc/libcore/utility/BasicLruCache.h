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

#ifndef __LIBCORE_UTILITY_BASICLRUCACHE_H__
#define __LIBCORE_UTILITY_BASICLRUCACHE_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Libcore::Utility::IBasicLruCache;

namespace Libcore {
namespace Utility {

class BasicLruCache
    : public Object
    , public IBasicLruCache
{
public:
    CAR_INTERFACE_DECL()

    BasicLruCache();

    BasicLruCache(
        /* [in] */ Int32 maxSize);

    virtual ~BasicLruCache();

    CARAPI constructor(
        /* [in] */ Int32 maxSize);

    /**
     * Returns the value for {@code key} if it exists in the cache or can be
     * created by {@code #create}. If a value was returned, it is moved to the
     * head of the queue. This returns null if a value is not cached and cannot
     * be created.
     */
    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** ouface);

    /**
     * Caches {@code value} for {@code key}. The value is moved to the head of
     * the queue.
     *
     * @return the previous value mapped by {@code key}. Although that entry is
     *     no longer cached, it has not been passed to {@link #entryEvicted}.
     */
    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** outface);

    /**
     * Returns a copy of the current contents of the cache, ordered from least
     * recently accessed to most recently accessed.
     */
    CARAPI Snapshot(
        /* [out] */ IMap** outmap);

    /**
     * Clear the cache, calling {@link #entryEvicted} on each removed entry.
     */
    CARAPI EvictAll();

protected:
    /**
     * Called for entries that have reached the tail of the least recently used
     * queue and are be removed. The default implementation does nothing.
     */
    virtual CARAPI_(void) EntryEvicted(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value);

    /**
     * Called after a cache miss to compute a value for the corresponding key.
     * Returns the computed value or null if no value can be computed. The
     * default implementation returns null.
     */
    virtual CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ IInterface* key);

private:
    CARAPI_(void) TrimToSize(
        /* [in] */ Int32 maxSize);

private:
    AutoPtr<IMap> mMap;
    Int32 mMaxSize;
};

} // namespace Utility
} // namespace Libcore

#endif //__LIBCORE_UTILITY_BASICLRUCACHE_H__
