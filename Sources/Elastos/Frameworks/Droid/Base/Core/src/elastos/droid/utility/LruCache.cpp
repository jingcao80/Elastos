#include "elastos/droid/utility/LruCache.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(LruCache, Object, ILruCache)

LruCache::LruCache()
    : mSize(0)
    , mMaxSize(0)
    , mPutCount(0)
    , mCreateCount(0)
    , mEvictionCount(0)
    , mHitCount(0)
    , mMissCount(0)
{
}

ECode LruCache::constructor(
    /* [in] */ Int32 maxSize)
{
    if (maxSize <= 0) {
        Logger::E("LruCache", "maxSize <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mMaxSize = maxSize;
    return CLinkedHashMap::New(0, 0.75f, TRUE, (ILinkedHashMap**)&mMap);
}

ECode LruCache::Resize(
    /* [in] */ Int32 maxSize)
{
    if (maxSize <= 0) {
        Logger::E("LruCache", "maxSize <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }

    {
        AutoLock syncLock(this);
        mMaxSize = maxSize;
    }

    return TrimToSize(maxSize);
}

ECode LruCache::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    if (key == NULL) {
        Logger::E("LruCache", "key == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInterface> mapValue;
    {
        AutoLock syncLock(this);
        mMap->Get(key, (IInterface**)&mapValue);
        if (mapValue != NULL) {
            mHitCount++;
            *value = mapValue;
            REFCOUNT_ADD(*value)
            return NOERROR;
        }
        mMissCount++;
    }

    /*
     * Attempt to create a value. This may take a long time, and the map
     * may be different when create() returns. If a conflicting value was
     * added to the map while create() was working, we leave that value in
     * the map and release the created value.
     */

    AutoPtr<IInterface> createdValue = Create(key);
    if (createdValue == NULL) {
        return NOERROR;
    }

    {
        AutoLock syncLock(this);
        mCreateCount++;
        mMap->Put(key, createdValue, (IInterface**)&mapValue);

        if (mapValue != NULL) {
            // There was a conflict so undo that last put
            AutoPtr<IInterface> tmp;
            mMap->Put(key, mapValue, (IInterface**)&tmp);
        }
        else {
            mSize += SafeSizeOf(key, createdValue);
        }
    }

    if (mapValue != NULL) {
        EntryRemoved(FALSE, key, createdValue, mapValue);
        *value = mapValue;
        REFCOUNT_ADD(*value)
    }
    else {
        TrimToSize(mMaxSize);
        *value = createdValue;
        REFCOUNT_ADD(*value)
    }
    return NOERROR;
}

ECode LruCache::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    if (key == NULL || value == NULL) {
        if (oldValue != NULL) *oldValue = NULL;
        Logger::E("LruCache", "key == null || value == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInterface> previous;
    {
        AutoLock syncLock(this);
        mPutCount++;
        mSize += SafeSizeOf(key, value);
        mMap->Put(key, value, (IInterface**)&previous);
        if (previous != NULL) {
            mSize -= SafeSizeOf(key, previous);
        }
    }

    if (previous != NULL) {
        EntryRemoved(FALSE, key, previous, value);
    }

    TrimToSize(mMaxSize);
    if (oldValue != NULL) {
        *oldValue = previous;
        REFCOUNT_ADD(*oldValue)
    }
    return NOERROR;
}

ECode LruCache::TrimToSize(
    /* [in] */ Int32 maxSize)
{
    while (TRUE) {
        AutoPtr<IInterface> key;
        AutoPtr<IInterface> value;
        {
            AutoLock lock(this);
            Boolean bval;
            if (mSize < 0 || ((mMap->IsEmpty(&bval), bval) && mSize != 0)) {
                Logger::E("LruCache", ".sizeOf() is reporting inconsistent results!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            if (mSize <= maxSize) {
                break;
            }

            AutoPtr<IMapEntry> toEvict;
            mMap->GetEldest((IMapEntry**)&toEvict);
            if (toEvict == NULL) {
                break;
            }

            toEvict->GetKey((IInterface**)&key);
            toEvict->GetValue((IInterface**)&value);
            mMap->Remove(key);
            mSize -= SafeSizeOf(key, value);
            mEvictionCount++;
        }

        EntryRemoved(TRUE, key, value, NULL);
    }
    return NOERROR;
}

ECode LruCache::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** oldValue)
{
    if (key == NULL) {
        if (oldValue) *oldValue = NULL;
        Logger::E("LruCache", "key == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInterface> previous;
    {
        mMap->Remove(key, (IInterface**)&previous);
        if (previous != NULL) {
            mSize -= SafeSizeOf(key, previous);
        }
    }

    if (previous != NULL) {
        EntryRemoved(FALSE, key, previous, NULL);
    }
    if (oldValue) {
        *oldValue = previous;
        REFCOUNT_ADD(*oldValue)
    }
    return NOERROR;
}

ECode LruCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue)
{
    Logger::W("LruCache", "LruCache::EntryRemoved called. please check Subclass's implement.");
    return NOERROR;
}

AutoPtr<IInterface> LruCache::Create(
    /* [in] */ IInterface* key)
{
    return NULL;
}

Int32 LruCache::SafeSizeOf(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    Int32 result = SizeOf(key, value);
    if (result < 0) {
        Logger::E("LruCache", "egative size: %s = %s", TO_CSTR(key), TO_CSTR(value));
        assert(0);
    }
    return result;
}

Int32 LruCache::SizeOf(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    return 1;
}

ECode LruCache::EvictAll()
{
    return TrimToSize(-1); // -1 will evict 0-sized elements
}

ECode LruCache::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    AutoLock syncLock(this);
    *size = mSize;
    return NOERROR;
}

ECode LruCache::GetMaxSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    AutoLock syncLock(this);
    *size = mMaxSize;
    return NOERROR;
}

ECode LruCache::GetHitCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoLock syncLock(this);
    *count = mHitCount;
    return NOERROR;
}

ECode LruCache::GetMissCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoLock syncLock(this);
    *count = mMissCount;
    return NOERROR;
}

ECode LruCache::GetCreateCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoLock syncLock(this);
    *count = mCreateCount;
    return NOERROR;
}

ECode LruCache::GetPutCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoLock syncLock(this);
    *count = mPutCount;
    return NOERROR;
}

ECode LruCache::GetEvictionCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoLock syncLock(this);
    *count = mEvictionCount;
    return NOERROR;
}

ECode LruCache::Snapshot(
    /* [out] */ IMap** map)
{
    return CLinkedHashMap::New(IMap::Probe(mMap), (IMap**)&map);
}

ECode LruCache::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 accesses = mHitCount + mMissCount;
    Int32 hitPercent = accesses != 0 ? (100 * mHitCount / accesses) : 0;
    StringBuilder sb("LruCache[maxSize=");
    sb += mMaxSize;
    sb += ", hits=";
    sb += mHitCount;
    sb += ", misses=";
    sb += mMissCount;
    sb += ", hitRate=";
    sb += hitPercent;
    sb += "]";
    *str = sb.ToString();
    return NOERROR;
}


} // namespace Utility
} // namespace Droid
} // namespace Elastos