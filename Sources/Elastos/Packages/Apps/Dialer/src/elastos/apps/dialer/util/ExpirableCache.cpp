
#include "util/ExpirableCache.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

//=================================================================
// ExpirableCache::GenerationalCachedValues
//=================================================================
CAR_INTERFACE_IMPL(ExpirableCache::GenerationalCachedValues, Object, IExpirableCacheCachedValue);

ExpirableCache::GenerationalCachedValues::GenerationalCachedValue(
    /* [in] */ IInterface* value,
    /* [in] */ IAtomicInteger32* cacheGeneration)
    : mValue(value)
    , mCacheGeneration(cacheGeneration)
{
    // Snapshot the current generation.
    mCacheGeneration->Get(&mGeneration);
}

ECode ExpirableCache::GenerationalCachedValues::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode ExpirableCache::GenerationalCachedValues::IsExpired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 generation;
    mCacheGeneration->Get(&generation);
    *result = mGeneration != generation;
    return NOERROR;
}

//=================================================================
// ExpirableCache
//=================================================================
CAR_INTERFACE_IMPL(ExpirableCache, Object, IExpirableCache);

ExpirableCache::ExpirableCache(
    /* [in] */ IInterface* cache)
    : mCache(cache)
{
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mGeneration);
}

ECode ExpirableCache::GetCachedValue(
    /* [in] */ IInterface* key,
    /* [out] */ IExpirableCacheCachedValue** value)
{
    VALIDATE_NOT_NULL(value);
    return mCache->Get(key, value);
}

ECode ExpirableCache::GetPossiblyExpired(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IExpirableCacheCachedValue> cachedValue;
    GetCachedValue(key, (IExpirableCacheCachedValue**)&cachedValue);
    if (cachedValue == NULL) {
        *value = NULL;
    }
    else{
        cachedValue->GetValue(value);
    }

    return NOERROR;
}

ECode ExpirableCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    AutoPtr<IExpirableCacheCachedValue> cachedValue;
    GetCachedValue(key, (IExpirableCacheCachedValue**)&cachedValue);
    Boolean result;
    if (cachedValue == NULL || cachedValue->IsExpired(&result), result) {
        *value = NULL;
    }
    else{
        cachedValue->GetValue(value);
    }
    return NOERROR;
}

ECode ExpirableCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<IExpirableCacheCachedValue> cached;
    NewCachedValue(value, &cached);
    mCache->Put(key, cached);

    return NOERROR;
}

ECode ExpirableCache::ExpireAll()
{
    mGeneration->IncrementAndGet();
    return NOERROR;
}

ECode ExpirableCache::NewCachedValue(
    /* [in] */ IInterface* value,
    /* [out] */ IExpirableCacheCachedValue** cached)
{
    VALIDATE_NOT_NULL(cached);

    AutoPtr<GenerationalCachedValue> gcv = new GenerationalCachedValue(value, mGeneration);
    *cached = (IExpirableCacheCachedValue*)gcv;
    REFCOUNT_ADD(*cached);
    return NOERROR;
}

AutoPtr<IExpirableCache> ExpirableCache::Create(
    /* [in] */ IInterface* cache)
{
    AutoPtr<ExpirableCache> expireableCache = new ExpirableCache(cache);
    return (IExpirableCache*)expireableCache;
}

AutoPtr<IExpirableCache> ExpirableCache::Create(
    /* [in] */ Int32 maxSize)
{
    AutoPtr<LruCache<IInterface*, IInterface*> > lruCache =
            new LruCache<IInterface*, IInterface*>(maxSize);
    return Create((IInterface*)lruCache);
}

} // Util
} // Dialer
} // Apps
} // Elastos
