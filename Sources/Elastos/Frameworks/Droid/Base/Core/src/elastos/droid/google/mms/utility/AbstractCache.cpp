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

#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/utility/AbstractCache.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                           AbstractCache
//=====================================================================
CAR_INTERFACE_IMPL(AbstractCache, Object, IAbstractCache);

const String AbstractCache::TAG("AbstractCache");
const Boolean AbstractCache::DEBUG = FALSE;
const Boolean AbstractCache::LOCAL_LOGV = FALSE;
const Int32 AbstractCache::MAX_CACHED_ITEMS;

ECode AbstractCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (LOCAL_LOGV) {
        Logger::V(TAG, String("Trying to put ") + TO_CSTR(key) + String(" into cache."));
    }

    Int32 size = 0;
    mCacheMap->GetSize(&size);
    if (size >= MAX_CACHED_ITEMS) {
        // TODO Should remove the oldest or least hit cached entry
        // and then cache the new one.
        if (LOCAL_LOGV) {
            Logger::V(TAG, String("Failed! size limitation reached."));
        }
        *result = FALSE;
        return NOERROR;
    }

    if (key != NULL) {
        AutoPtr<CacheEntry> cacheEntry = new CacheEntry();
        cacheEntry->mValue = value;
        mCacheMap->Put(key, (IObject*)cacheEntry.Get());

        if (LOCAL_LOGV) {
            Int32 size = 0;
            mCacheMap->GetSize(&size);
            Logger::V(TAG, String(TO_CSTR(key)) + String(" cached, ") + StringUtils::ToString(size) + String(" items total."));
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode AbstractCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (LOCAL_LOGV) {
        Logger::V(TAG, String("Trying to get ") + TO_CSTR(key) + String(" from cache."));
    }

    if (key != NULL) {
        AutoPtr<IInterface> p;
        mCacheMap->Get(key, (IInterface**)&p);
        AutoPtr<CacheEntry> cacheEntry = (CacheEntry*)(IObject*)p.Get();
        if (cacheEntry != NULL) {
            cacheEntry->mHit++;
            if (LOCAL_LOGV) {
                Logger::V(TAG, String(TO_CSTR(key)) + String(" hit ") + StringUtils::ToString(cacheEntry->mHit) + String(" times."));
            }
            *result = cacheEntry->mValue;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode AbstractCache::Purge(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (LOCAL_LOGV) {
        Logger::V(TAG, String("Trying to purge ") + TO_CSTR(key));
    }

    AutoPtr<IInterface> p;
    mCacheMap->Remove(key, (IInterface**)&p);
    AutoPtr<CacheEntry> v = (CacheEntry*)(IObject*)p.Get();

    if (LOCAL_LOGV) {
        Int32 size = 0;
        mCacheMap->GetSize(&size);
        Logger::V(TAG, StringUtils::ToString(size) + String(" items cached."));
    }

    if (v != NULL) {
        *result = v->mValue;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode AbstractCache::PurgeAll()
{
    if (LOCAL_LOGV) {
        Int32 size = 0;
        mCacheMap->GetSize(&size);
        Logger::V(TAG, String("Purging cache, ") + StringUtils::ToString(size)
                + String(" items dropped."));
    }
    mCacheMap->Clear();
    return NOERROR;
}

ECode AbstractCache::Size(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mCacheMap->GetSize(result);
}

AbstractCache::AbstractCache()
{
    CHashMap::New((IHashMap**)&mCacheMap);
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos


