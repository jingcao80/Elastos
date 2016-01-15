
#ifndef __ELASTOS_DROID_UTILITY_LRUCACHE_H__
#define __ELASTOS_DROID_UTILITY_LRUCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * A cache that holds strong references to a limited number of values. Each time
 * a value is accessed, it is moved to the head of a queue. When a value is
 * added to a full cache, the value at the end of that queue is evicted and may
 * become eligible for garbage collection.
 *
 * <p>If your cached values hold resources that need to be explicitly released,
 * override {@link #entryRemoved}.
 *
 * <p>If a cache miss should be computed on demand for the corresponding keys,
 * override {@link #create}. This simplifies the calling code, allowing it to
 * assume a value will always be returned, even when there's a cache miss.
 *
 * <p>By default, the cache size is measured in the number of entries. Override
 * {@link #sizeOf} to size the cache in different units. For example, this cache
 * is limited to 4MiB of bitmaps:
 * <pre>   {@code
 *   int cacheSize = 4 * 1024 * 1024; // 4MiB
 *   LruCache<String, Bitmap> bitmapCache = new LruCache<String, Bitmap>(cacheSize) {
 *       protected int sizeOf(String key, Bitmap value) {
 *           return value.getByteCount();
 *       }
 *   }}</pre>
 *
 * <p>This class is thread-safe. Perform multiple cache operations atomically by
 * synchronizing on the cache: <pre>   {@code
 *   synchronized(cache) {
 *     if (cache.get(key) == null) {
 *         cache.put(key, value);
 *     }
 *   }}</pre>
 *
 * <p>This class does not allow null to be used as a key or value. A return
 * value of null from {@link #get}, {@link #put} or {@link #remove} is
 * unambiguous: the key was not in the cache.
 *
 * <p>This class appeared in Android 3.1 (Honeycomb MR1); it's available as part
 * of <a href="http://developer.android.com/sdk/compatibility-library.html">Android's
 * Support Package</a> for earlier releases.
 */
template<typename K, typename V>
class LruCache
    : public Object
{
public:
    /**
     * @param maxSize for caches that do not override {@link #sizeOf}, this is
     *     the maximum number of entries in the cache. For all other caches,
     *     this is the maximum sum of the sizes of the entries in this cache.
     */
    LruCache(
        /* [in] */ Int32 maxSize);

    /**
     * Sets the size of the cache.
     *
     * @param maxSize The new maximum size.
     */
    CARAPI_(void) Resize(
        /* [in] */ Int32 maxSize);

    /**
     * Returns the value for {@code key} if it exists in the cache or can be
     * created by {@code #create}. If a value was returned, it is moved to the
     * head of the queue. This returns null if a value is not cached and cannot
     * be created.
     */
    CARAPI_(V) Get(
        /* [in] */ K key);

    /**
     * Caches {@code value} for {@code key}. The value is moved to the head of
     * the queue.
     *
     * @return the previous value mapped by {@code key}.
     */
    CARAPI_(V) Put(
        /* [in] */ K key,
        /* [in] */ V value);

    /**
     * Remove the eldest entries until the total of remaining entries is at or
     * below the requested size.
     *
     * @param maxSize the maximum size of the cache before returning. May be -1
     *            to evict even 0-sized elements.
     */
    CARAPI_(void) TrimToSize(
        /* [in] */ Int32 maxSize);

    /**
     * Removes the entry for {@code key} if it exists.
     *
     * @return the previous value mapped by {@code key}.
     */
    CARAPI_(V) Remove(
        /* [in] */ K key);

    /**
     * Clear the cache, calling {@link #entryRemoved} on each removed entry.
     */
    CARAPI_(void) EvictAll();

    /**
     * For caches that do not override {@link #sizeOf}, this returns the number
     * of entries in the cache. For all other caches, this returns the sum of
     * the sizes of the entries in this cache.
     */
    CARAPI_(Int32) Size();

    /**
     * For caches that do not override {@link #sizeOf}, this returns the maximum
     * number of entries in the cache. For all other caches, this returns the
     * maximum sum of the sizes of the entries in this cache.
     */
    CARAPI_(Int32) MaxSize();

    /**
     * Returns the number of times {@link #get} returned a value that was
     * already present in the cache.
     */
    CARAPI_(Int32) HitCount();

    /**
     * Returns the number of times {@link #get} returned null or required a new
     * value to be created.
     */
    CARAPI_(Int32) MissCount();

    /**
     * Returns the number of times {@link #create(Object)} returned a value.
     */
    CARAPI_(Int32) CreateCount();

    /**
     * Returns the number of times {@link #put} was called.
     */
    CARAPI_(Int32) PutCount();

    /**
     * Returns the number of values that have been evicted.
     */
    CARAPI_(Int32) EvictionCount();

    /**
     * Returns a copy of the current contents of the cache, ordered from least
     * recently accessed to most recently accessed.
     */
    AutoPtr<HashMap<K, V> > Snapshot();

    virtual CARAPI_(String) ToString();

protected:
    /**
     * Called for entries that have been evicted or removed. This method is
     * invoked when a value is evicted to make space, removed by a call to
     * {@link #remove}, or replaced by a call to {@link #put}. The default
     * implementation does nothing.
     *
     * <p>The method is called without synchronization: other threads may
     * access the cache while this method is executing.
     *
     * @param evicted true if the entry is being removed to make space, false
     *     if the removal was caused by a {@link #put} or {@link #remove}.
     * @param newValue the new value for {@code key}, if it exists. If non-null,
     *     this removal was caused by a {@link #put}. Otherwise it was caused by
     *     an eviction or a {@link #remove}.
     */
    virtual CARAPI_(void) EntryRemoved(
        /* [in] */ Boolean evicted,
        /* [in] */ K key,
        /* [in] */ V oldValue,
        /* [in] */ V newValue);

    /**
     * Called after a cache miss to compute a value for the corresponding key.
     * Returns the computed value or null if no value can be computed. The
     * default implementation returns null.
     *
     * <p>The method is called without synchronization: other threads may
     * access the cache while this method is executing.
     *
     * <p>If a value for {@code key} exists in the cache when this method
     * returns, the created value will be released with {@link #entryRemoved}
     * and discarded. This can occur when multiple threads request the same key
     * at the same time (causing multiple values to be created), or when one
     * thread calls {@link #put} while another is creating a value for the same
     * key.
     */
    virtual CARAPI_(V) Create(
        /* [in] */ K key);

    /**
     * Returns the size of the entry for {@code key} and {@code value} in
     * user-defined units.  The default implementation returns 1 so that size
     * is the number of entries and max size is the maximum number of entries.
     *
     * <p>An entry's size must not change while it is in the cache.
     */
    CARAPI_(Int32) SizeOf(
        /* [in] */ K key,
        /* [in] */ V value);

private:
    CARAPI_(Int32) SafeSizeOf(
        /* [in] */ K key,
        /* [in] */ V value);

private:
    HashMap<K, V> mMap;

    /** Size of this cache in units. Not necessarily the number of elements. */
    Int32 mSize;
    Int32 mMaxSize;

    Int32 mPutCount;
    Int32 mCreateCount;
    Int32 mEvictionCount;
    Int32 mHitCount;
    Int32 mMissCount;
};

template<typename K, typename V>
LruCache<K, V>::LruCache(
    /* [in] */ Int32 maxSize)
    : mSize(0)
    , mPutCount(0)
    , mCreateCount(0)
    , mEvictionCount(0)
    , mHitCount(0)
    , mMissCount(0)
{
    if (maxSize <= 0) {
        Slogger::E("LruCache", "maxSize <= 0");
        assert(0);
        // throw new IllegalArgumentException("maxSize <= 0");
    }
    mMaxSize = maxSize;
}

template<typename K, typename V>
void LruCache<K, V>::Resize(
    /* [in] */ Int32 maxSize)
{
    if (maxSize <= 0) {
        Slogger::E("LruCache", "maxSize <= 0");
        assert(0);
        // throw new IllegalArgumentException("maxSize <= 0");
    }

    synchronized(this) {
        mMaxSize = maxSize;
    }

    TrimToSize(maxSize);
}

template<typename K, typename V>
V LruCache<K, V>::Get(
    /* [in] */ K key)
{
    if (key == NULL) {
        Slogger::E("LruCache", "key == null");
        assert(0);
        // throw new NullPointerException("key == null");
    }

    V mapValue;
    synchronized(this) {
        typename HashMap<K, V>::Iterator it = mMap.Find(key);
        if (it != mMap.End()) {
            mapValue = it->mSecond;
        }
        if (mapValue != NULL) {
            mHitCount++;
            return mapValue;
        }
        mMissCount++;

    }

    /*
     * Attempt to create a value. This may take a long time, and the map
     * may be different when create() returns. If a conflicting value was
     * added to the map while create() was working, we leave that value in
     * the map and release the created value.
     */

    V createdValue = Create(key);
    if (createdValue == NULL) {
        return NULL;
    }

    synchronized(this) {
        mCreateCount++;
        mapValue = mMap[key];
        if (mapValue == NULL) {
            mMap[key] = createdValue;
            mSize += SafeSizeOf(key, createdValue);
        }
    }

    if (mapValue != NULL) {
        EntryRemoved(FALSE, key, createdValue, mapValue);
        return mapValue;
    }
    else {
        TrimToSize(mMaxSize);
        return createdValue;
    }
}

template<typename K, typename V>
V LruCache<K, V>::Put(
    /* [in] */ K key,
    /* [in] */ V value)
{
    if (key == NULL || value == NULL) {
        Slogger::E("LruCache", "key == null || value == null");
        assert(0);
        // throw new NullPointerException("key == null || value == null");
    }

    V previous;
    synchronized(this) {
        mPutCount++;
        mSize += SafeSizeOf(key, value);

        typename HashMap<K, V>::Iterator it = mMap.Find(key);
        if (it != mMap.End()) {
            previous = it->mSecond;
        }

        if (previous != NULL) {
            mSize -= SafeSizeOf(key, previous);
        }
        mMap[key] = value;
    }

    if (previous != NULL) {
        EntryRemoved(FALSE, key, previous, value);
    }

    TrimToSize(mMaxSize);
    return previous;
}

template<typename K, typename V>
void LruCache<K, V>::TrimToSize(
    /* [in] */ Int32 maxSize)
{
    while (TRUE) {
        K key;
        V value;
        synchronized(this) {
            if (mSize < 0 || (mMap.IsEmpty() && mSize != 0)) {
                Slogger::E("LruCache", ".sizeOf() is reporting inconsistent results!");
                assert(0);
                // throw new IllegalStateException(getClass().getName()
                //         + ".sizeOf() is reporting inconsistent results!");
            }

            if (mSize <= maxSize) {
                break;
            }

            Slogger::W("LruCache", " >> TODO find the eldest record!");
            //TODO Map.Entry<K, V> toEvict = map.eldest();
            typename HashMap<K, V>::Iterator toEvict = mMap.Begin();
            if (toEvict == mMap.End()) {
                break;
            }

            key = toEvict->mFirst;
            value = toEvict->mSecond;
            mMap.Erase(toEvict);

            mSize -= SafeSizeOf(key, value);
            mEvictionCount++;
        }

        EntryRemoved(TRUE, key, value, NULL);
    }
}

template<typename K, typename V>
V LruCache<K, V>::Remove(
    /* [in] */ K key)
{
    if (key == NULL) {
        Slogger::E("LruCache", "key == null");
        assert(0);
        // throw new NullPointerException("key == null");
    }

    V previous;
    synchronized(this) {
        typename HashMap<K, V>::Iterator it = mMap.Find(key);
        if (it != mMap.End()) {
            previous = it->mSecond;
            mMap.Erase(it);
        }
        if (previous != NULL) {
            mSize -= SafeSizeOf(key, previous);
        }
    }

    if (previous != NULL) {
        EntryRemoved(FALSE, key, previous, NULL);
    }
    return previous;
}

template<typename K, typename V>
void LruCache<K, V>::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ K key,
    /* [in] */ V oldValue,
    /* [in] */ V newValue)
{}

template<typename K, typename V>
V LruCache<K, V>::Create(
    /* [in] */ K key)
{
    return NULL;
}

template<typename K, typename V>
Int32 LruCache<K, V>::SafeSizeOf(
    /* [in] */ K key,
    /* [in] */ V value)
{
    Int32 result = SizeOf(key, value);
    if (result < 0) {
        assert(0);
        // throw new IllegalStateException("Negative size: " + key + "=" + value);
    }
    return result;
}

template<typename K, typename V>
Int32 LruCache<K, V>::SizeOf(
    /* [in] */ K key,
    /* [in] */ V value)
{
    return 1;
}

template<typename K, typename V>
void LruCache<K, V>::EvictAll()
{
    TrimToSize(-1); // -1 will evict 0-sized elements
}

template<typename K, typename V>
Int32 LruCache<K, V>::Size()
{
    return mSize;
}

template<typename K, typename V>
Int32 LruCache<K, V>::MaxSize()
{
    return mMaxSize;
}

template<typename K, typename V>
Int32 LruCache<K, V>::HitCount()
{
    return mHitCount;
}

template<typename K, typename V>
Int32 LruCache<K, V>::MissCount()
{
    return mMissCount;
}

template<typename K, typename V>
Int32 LruCache<K, V>::CreateCount()
{
    return mCreateCount;
}

template<typename K, typename V>
Int32 LruCache<K, V>::PutCount()
{
    return mPutCount;
}

template<typename K, typename V>
Int32 LruCache<K, V>::EvictionCount()
{
    return mEvictionCount;
}

template<typename K, typename V>
AutoPtr<HashMap<K, V> > LruCache<K, V>::Snapshot()
{
    AutoPtr<HashMap<K, V> > map = new HashMap<K, V>(mMap.Begin(), mMap.End());
    return map;
}

template<typename K, typename V>
String LruCache<K, V>::ToString()
{
    Int32 accesses = mHitCount + mMissCount;
    Int32 hitPercent = accesses != 0 ? (100 * mHitCount / accesses) : 0;
    StringBuilder sb("LruCache[maxSize=");
    sb += mMaxSize;
    sb += ",hits=";
    sb += mHitCount;
    sb += ",misses=";
    sb += mMissCount;
    sb += ",hitRate=";
    sb += hitPercent;
    sb += "]";
    return sb.ToString();
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_LRUCACHE_H__
