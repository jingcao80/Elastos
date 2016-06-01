
#include "elastos/droid/google/mms/utility/AbstractCache.h"

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
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Trying to put " + key + " into cache.");
    // }
    //
    // if (mCacheMap.size() >= MAX_CACHED_ITEMS) {
    //     // TODO Should remove the oldest or least hit cached entry
    //     // and then cache the new one.
    //     if (LOCAL_LOGV) {
    //         Log.v(TAG, "Failed! size limitation reached.");
    //     }
    //     return false;
    // }
    //
    // if (key != null) {
    //     CacheEntry<V> cacheEntry = new CacheEntry<V>();
    //     cacheEntry.value = value;
    //     mCacheMap.put(key, cacheEntry);
    //
    //     if (LOCAL_LOGV) {
    //         Log.v(TAG, key + " cached, " + mCacheMap.size() + " items total.");
    //     }
    //     return true;
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode AbstractCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Trying to get " + key + " from cache.");
    // }
    //
    // if (key != null) {
    //     CacheEntry<V> cacheEntry = mCacheMap.get(key);
    //     if (cacheEntry != null) {
    //         cacheEntry.hit++;
    //         if (LOCAL_LOGV) {
    //             Log.v(TAG, key + " hit " + cacheEntry.hit + " times.");
    //         }
    //         return cacheEntry.value;
    //     }
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode AbstractCache::Purge(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Trying to purge " + key);
    // }
    //
    // CacheEntry<V> v = mCacheMap.remove(key);
    //
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, mCacheMap.size() + " items cached.");
    // }
    //
    // return v != null ? v.value : null;
    assert(0);
    return NOERROR;
}

ECode AbstractCache::PurgeAll()
{
    // ==================before translated======================
    // if (LOCAL_LOGV) {
    //     Log.v(TAG, "Purging cache, " + mCacheMap.size()
    //             + " items dropped.");
    // }
    // mCacheMap.clear();
    assert(0);
    return NOERROR;
}

ECode AbstractCache::Size(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCacheMap.size();
    assert(0);
    return NOERROR;
}

AbstractCache::AbstractCache()
{
    // ==================before translated======================
    // mCacheMap = new HashMap<K, CacheEntry<V>>();
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos


