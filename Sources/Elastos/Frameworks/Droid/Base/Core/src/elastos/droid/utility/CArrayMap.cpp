#include "elastos/droid/utility/CArrayMap.h"
#include <libcore/utility/EmptyArray.h>
#include "elastos/droid/utility/ContainerHelpers.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::StringBuilder;
using Libcore::Utility::EmptyArray;
using Elastos::Utility::EIID_IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

static AutoPtr<IArrayMap> InitEMPTY()
{
    AutoPtr<IArrayMap> map;
    CArrayMap::New(TRUE, (IArrayMap**)&map);
    return map;
}

INIT_PROI_3 const AutoPtr<IArrayMap> CArrayMap::EMPTY = InitEMPTY();
const Boolean CArrayMap::DEBUG = FALSE;
const String CArrayMap::TAG("CArrayMap");
const Int32 CArrayMap::BASE_SIZE = 4;
const Int32 CArrayMap::CACHE_SIZE = 10;

INIT_PROI_3 AutoPtr<ArrayOf<IInterface*> > CArrayMap::sBaseCache;
Int32 CArrayMap::sBaseCacheSize = 0;
INIT_PROI_3 AutoPtr<ArrayOf<IInterface*> > CArrayMap::sTwiceBaseCache;
Int32 CArrayMap::sTwiceBaseCacheSize = 0;
INIT_PROI_3 Object CArrayMap::sLock;

INIT_PROI_3 const AutoPtr<ArrayOf<Int32> > CArrayMap::EMPTY_IMMUTABLE_INTS = ArrayOf<Int32>::Alloc(0);

//======================================================================
// CArrayMap::InnerMapCollections
//======================================================================
CArrayMap::InnerMapCollections::InnerMapCollections(
    /* [in] */ CArrayMap* host)
    : mHost(host)
{
}

Int32 CArrayMap::InnerMapCollections::ColGetSize()
{
    return mHost->mSize;
}

AutoPtr<IInterface> CArrayMap::InnerMapCollections::ColGetEntry(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset)
{
    return (*mHost->mArray)[(index<<1) + offset];
}

Int32 CArrayMap::InnerMapCollections::ColIndexOfKey(
    /* [in] */ IInterface* key)
{
    Int32 index;
    mHost->GetIndexOfKey(key, &index);
    return index;
}

Int32 CArrayMap::InnerMapCollections::ColIndexOfValue(
    /* [in] */ IInterface* value)
{
    Int32 index;
    mHost->GetIndexOfValue(value, &index);
    return index;
}

AutoPtr<IMap> CArrayMap::InnerMapCollections::ColGetMap()
{
    return (IMap*)mHost->Probe(EIID_IMap);
}

void CArrayMap::InnerMapCollections::ColPut(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    mHost->Put(key, value);
}

AutoPtr<IInterface> CArrayMap::InnerMapCollections::ColSetValue(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    AutoPtr<IInterface> old;
    mHost->SetValueAt(index, value, (IInterface**)&old);
    return old;
}

void CArrayMap::InnerMapCollections::ColRemoveAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> old;
    mHost->RemoveAt(index, (IInterface**)&old);
}

void CArrayMap::InnerMapCollections::ColClear()
{
    mHost->Clear();
}

//======================================================================
//
//======================================================================
CAR_INTERFACE_IMPL_2(CArrayMap, Object, IArrayMap, IMap)

CAR_OBJECT_IMPL(CArrayMap)

CArrayMap::CArrayMap()
    : mSize(0)
{
}

ECode CArrayMap::constructor()
{
    mHashes = EmptyArray::INT32;
    mArray = EmptyArray::OBJECT;
    mSize = 0;
    return NOERROR;
}

ECode CArrayMap::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity == 0) {
        mHashes = EmptyArray::INT32;
        mArray = EmptyArray::OBJECT;
    }
    else {
        AllocArrays(capacity);
    }
    mSize = 0;
    return NOERROR;
}

ECode CArrayMap::constructor(
    /* [in] */ Boolean immutable)
{
    mHashes = EmptyArray::INT32;
    mArray = EmptyArray::OBJECT;
    mSize = 0;
    return NOERROR;
}

ECode CArrayMap::constructor(
    /* [in] */ IArrayMap* map)
{
    constructor();
    if (map != NULL) {
        PutAll(map);
    }
    return NOERROR;
}

Int32 CArrayMap::GetIndexOf(
    /* [in] */ IInterface* key,
    /* [in] */ Int32 hash)
{
    Int32 N = mSize;

    // Important fast case: if nothing is in here, nothing to look for.
    if (N == 0) {
        return ~0;
    }

    Int32 index = ContainerHelpers::BinarySearch(mHashes, N, hash);

    // If the hash code wasn't found, then we have no entry for this key.
    if (index < 0) {
        return index;
    }

    // If the key at the returned index matches, that's what we want.
    if (Object::Equals(key, (*mArray)[index<<1])) {
        return index;
    }

    // Search for a matching key after the index.
    Int32 end;
    for (end = index + 1; end < N && (*mHashes)[end] == hash; end++) {
        if (Object::Equals(key, (*mArray)[end << 1])) return end;
    }

    // Search for a matching key before the index.
    for (Int32 i = index - 1; i >= 0 && (*mHashes)[i] == hash; i--) {
        if (Object::Equals(key, (*mArray)[i << 1])) return i;
    }

    // Key not found -- return negative value indicating where a
    // new entry for this key should go.  We use the end of the
    // hash chain to reduce the number of array entries that will
    // need to be copied when inserting.
    return ~end;
}

Int32 CArrayMap::GetIndexOfNull()
{
    Int32 N = mSize;

    // Important fast case: if nothing is in here, nothing to look for.
    if (N == 0) {
        return ~0;
    }

    Int32 index = ContainerHelpers::BinarySearch(mHashes, N, 0);

    // If the hash code wasn't found, then we have no entry for this key.
    if (index < 0) {
        return index;
    }

    // If the key at the returned index matches, that's what we want.
    if (NULL == (*mArray)[index<<1]) {
        return index;
    }

    // Search for a matching key after the index.
    Int32 end;
    for (end = index + 1; end < N && (*mHashes)[end] == 0; end++) {
        if (NULL == (*mArray)[end << 1]) return end;
    }

    // Search for a matching key before the index.
    for (Int32 i = index - 1; i >= 0 && (*mHashes)[i] == 0; i--) {
        if (NULL == (*mArray)[i << 1]) return i;
    }

    // Key not found -- return negative value indicating where a
    // new entry for this key should go.  We use the end of the
    // hash chain to reduce the number of array entries that will
    // need to be copied when inserting.
    return ~end;
}

ECode CArrayMap::AllocArrays(
    /* [in] */ Int32 size)
{
    if (mHashes == EMPTY_IMMUTABLE_INTS) {
        //throw new UnsupportedOperationException("ArrayMap is immutable");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    if (size == (BASE_SIZE * 2)) {
        synchronized(sLock) {
            if (sTwiceBaseCache != NULL) {
                AutoPtr<ArrayOf<IInterface*> > array = sTwiceBaseCache;
                mArray = array;
                AutoPtr<ObjectsEntry> oe = (ObjectsEntry*)(IObject*)(*array)[0];
                AutoPtr<HashesEntry> he = (HashesEntry*)(IObject*)(*array)[1];
                sTwiceBaseCache = oe->mObjects;
                mHashes = he->mHashes;

                array->Set(0, NULL);
                array->Set(1, NULL);
                sTwiceBaseCacheSize--;
                if (DEBUG) Logger::D(TAG, "Retrieving 2x cache %p now have %d entries", mHashes.Get(), sTwiceBaseCacheSize);
                return NOERROR;
            }
        }
    }
    else if (size == BASE_SIZE) {
        synchronized(sLock) {
            if (sBaseCache != NULL) {
                AutoPtr<ArrayOf<IInterface*> > array = sBaseCache;
                mArray = array;
                AutoPtr<ObjectsEntry> oe = (ObjectsEntry*)(IObject*)(*array)[0];
                AutoPtr<HashesEntry> he = (HashesEntry*)(IObject*)(*array)[1];
                sBaseCache = oe->mObjects;
                mHashes = he->mHashes;

                array->Set(0, NULL);
                array->Set(1, NULL);
                sBaseCacheSize--;
                if (DEBUG) Logger::D(TAG, "Retrieving 1x cache %p now have %d entries", mHashes.Get(), sBaseCacheSize);
                return NOERROR;
            }
        }
    }

    mHashes = ArrayOf<Int32>::Alloc(size);
    mArray = ArrayOf<IInterface*>::Alloc(size << 1);
    return NOERROR;
}

ECode CArrayMap::FreeArrays(
    /* [in] */ ArrayOf<Int32>* hashes,
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 size)
{
    if (hashes->GetLength() == (BASE_SIZE * 2)) {
        synchronized(sLock) {
            if (sTwiceBaseCacheSize < CACHE_SIZE) {
                AutoPtr<IInterface> oe = (IObject*)new ObjectsEntry(sTwiceBaseCache.Get());
                AutoPtr<IInterface> he = (IObject*)new HashesEntry(hashes);
                array->Set(0, oe);
                array->Set(1, he);

                for (Int32 i = (size << 1) - 1; i >= 2; i--) {
                    array->Set(i, NULL);
                }
                sTwiceBaseCache = array;
                sTwiceBaseCacheSize++;
                if (DEBUG) Logger::D(TAG, "Storing 2x cache %p now have %d entries", array, sTwiceBaseCacheSize);
            }
        }
    }
    else if (hashes->GetLength() == BASE_SIZE) {
        synchronized(sLock) {
            if (sBaseCacheSize < CACHE_SIZE) {
                AutoPtr<IInterface> oe = (IObject*)new ObjectsEntry(sBaseCache.Get());
                AutoPtr<IInterface> he = (IObject*)new HashesEntry(hashes);
                array->Set(0, oe);
                array->Set(1, he);

                for (Int32 i=(size<<1)-1; i>=2; i--) {
                    array->Set(i, NULL);
                }

                sBaseCache = array;
                sBaseCacheSize++;
                if (DEBUG) Logger::D(TAG, "Storing 1x cache %p now have %d entries", array, sBaseCacheSize);
            }
        }
    }
    return NOERROR;
}

ECode CArrayMap::Clear()
{
    if (mSize > 0) {
        FreeArrays(mHashes, mArray, mSize);
        mHashes = EmptyArray::INT32;
        mArray = EmptyArray::OBJECT;
        mSize = 0;
    }
    return NOERROR;
}

ECode CArrayMap::Erase()
{
    if (mSize > 0) {
        Int32 N = mSize << 1;
        AutoPtr<ArrayOf<IInterface*> > array = mArray;
        for (Int32 i = 0; i < N; i++) {
            array->Set(i, NULL);
        }
        mSize = 0;
    }
    return NOERROR;
}

ECode CArrayMap::EnsureCapacity(
    /* [in] */ Int32 minimumCapacity)
{
    if (mHashes->GetLength() < minimumCapacity) {
        AutoPtr<ArrayOf<Int32> > ohashes = mHashes;
        AutoPtr<ArrayOf<IInterface*> > oarray = mArray;
        AllocArrays(minimumCapacity);
        if (mSize > 0) {
            mHashes->Copy(ohashes, 0, mSize);
            mArray->Copy(oarray, 0, mSize << 1);
        }
        FreeArrays(ohashes, oarray, mSize);
    }
    return NOERROR;
}

ECode CArrayMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index;
    GetIndexOfKey(key, &index);
    *result = index >= 0;
    return NOERROR;
}

ECode CArrayMap::GetIndexOfKey(
    /* [in] */ IInterface* key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (key == NULL) {
        *result = GetIndexOfNull();
    }
    else {
        *result = GetIndexOf(key, Object::GetHashCode(key));
    }
    return NOERROR;
}

ECode CArrayMap::GetIndexOfValue(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 N = mSize*2;
    AutoPtr<ArrayOf<IInterface*> > array = mArray;
    if (value == NULL) {
        for (Int32 i = 1; i < N; i += 2) {
            if ((*array)[i] == NULL) {
                *result = i>>1;
                return NOERROR;
            }
        }
    } else {
        for (Int32 i = 1; i < N; i += 2) {
            if (Object::Equals(value, (*array)[i])) {
                *result = i>>1;
                return NOERROR;
            }
        }
    }

    *result = -1;
    return NOERROR;
}

ECode CArrayMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index;
    GetIndexOfValue(value, &index);
    *result = index >= 0;
    return NOERROR;
}

ECode CArrayMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 index;
    GetIndexOfKey(key, &index);
    if (index >= 0) {
        *result = (*mArray)[(index << 1) + 1];
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CArrayMap::GetKeyAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key)
    *key = (*mArray)[index << 1];
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CArrayMap::GetValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = (*mArray)[(index << 1) + 1];
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CArrayMap::SetValueAt(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    return SetValueAt(index, value, NULL);
}

ECode CArrayMap::SetValueAt(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    index = (index << 1) + 1;
    AutoPtr<IInterface> old = (*mArray)[index];
    mArray->Set(index, value);
    if (oldValue) {
        *oldValue = old;
        REFCOUNT_ADD(*oldValue)
    }
    return NOERROR;
}

ECode CArrayMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize <= 0;
    return NOERROR;
}

ECode CArrayMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<IInterface> old;
    return Put(key, value, (IInterface**)&old);
}

ECode CArrayMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    VALIDATE_NOT_NULL(oldValue)
    *oldValue = NULL;

    Int32 hash;
    Int32 index;
    if (key == NULL) {
        hash = 0;
        index = GetIndexOfNull();
    }
    else {
        hash = Object::GetHashCode(key);
        index = GetIndexOf(key, hash);
    }
    if (index >= 0) {
        index = (index << 1) + 1;
        AutoPtr<IInterface> old = (*mArray)[index];
        mArray->Set(index, value);
        *oldValue = old;
        REFCOUNT_ADD(*oldValue)
        return NOERROR;
    }

    index = ~index;
    if (mSize >= mHashes->GetLength()) {
        Int32 n = mSize >= (BASE_SIZE * 2) ? (mSize + (mSize >> 1))
                : (mSize >= BASE_SIZE ? (BASE_SIZE * 2) : BASE_SIZE);

        if (DEBUG) Logger::D(TAG, "put: grow from %d to n", mHashes->GetLength(), n);

        AutoPtr<ArrayOf<Int32> > ohashes = mHashes;
        AutoPtr<ArrayOf<IInterface*> > oarray = mArray;
        AllocArrays(n);

        if (mHashes->GetLength() > 0) {
            if (DEBUG) Logger::D(TAG, "put: copy 0- to 0", mSize);
            mHashes->Copy(ohashes);
            mArray->Copy(oarray);
        }

        FreeArrays(ohashes, oarray, mSize);
    }

    if (index < mSize) {
        if (DEBUG) Logger::D(TAG, "put: move %d-%d to %d", index, (mSize-index), (index+1));
        mHashes->Copy(index + 1, mHashes, index, mSize - index);
        mArray->Copy((index + 1) << 1, mArray, index << 1, (mSize - index) << 1);
    }

    mHashes->Set(index, hash);
    mArray->Set(index << 1, key);
    mArray->Set((index<<1)+1, value);
    mSize++;
    return NOERROR;
}

ECode CArrayMap::Append(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    Int32 index = mSize;
    Int32 hash = key == NULL ? 0 : Object::GetHashCode(key);
    if (index >= mHashes->GetLength()) {
        //throw new IllegalStateException("Array is full");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (index > 0 && (*mHashes)[index-1] > hash) {
        // RuntimeException e = new RuntimeException("here");
        // e.fillInStackTrace();
        Logger::W(TAG, "New hash %d is before end of array hash %d at index %d key %p",
            hash, (*mHashes)[index-1], index, key);
        return Put(key, value);
    }

    mSize = index+1;
    mHashes->Set(index, hash);
    index <<= 1;
    mArray->Set(index, key);
    mArray->Set(index+1, value);
    return NOERROR;
}

ECode CArrayMap::Validate()
{
    Int32 N = mSize;
    if (N <= 1) {
        // There can't be dups.
        return NOERROR;
    }
    Int32 basehash = (*mHashes)[0];
    Int32 basei = 0;
    for (Int32 i = 1; i < N; i++) {
        Int32 hash = (*mHashes)[i];
        if (hash != basehash) {
            basehash = hash;
            basei = i;
            continue;
        }
        // We are in a run of entries with the same hash code.  Go backwards through
        // the array to see if any keys are the same.
        AutoPtr<IInterface> cur = (*mArray)[i<<1];
        for (Int32 j=i-1; j>=basei; j--) {
            AutoPtr<IInterface> prev = (*mArray)[j<<1];
            if (cur == prev) {
                Logger::E(TAG, "Duplicate key in ArrayMap: %p", cur.Get());
                // throw new IllegalArgumentException("Duplicate key in ArrayMap: " + cur);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (cur != NULL && prev != NULL && Object::Equals(cur, prev)) {
                Logger::E(TAG, "Duplicate key in ArrayMap: %p", cur.Get());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
                //throw new IllegalArgumentException("Duplicate key in ArrayMap: " + cur);
            }
        }
    }
    return NOERROR;
}

ECode CArrayMap::PutAll(
    /* [in] */ IArrayMap* map)
{
    CArrayMap* array = (CArrayMap*)map;
    Int32 N = array->mSize;
    EnsureCapacity(mSize + N);
    if (mSize == 0) {
        if (N > 0) {
            mHashes->Copy(array->mHashes, 0, N);
            mArray->Copy(array->mArray, 0, N << 1);
            mSize = N;
        }
    }
    else {
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> key, value;
            array->GetKeyAt(i, (IInterface**)&key);
            array->GetValueAt(i, (IInterface**)&value);
            Put(key, value);
        }
    }
    return NOERROR;
}

ECode CArrayMap::Remove(
    /* [in] */ IInterface* key)
{
    return Remove(key, NULL);
}

ECode CArrayMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** result)
{
    Int32 index;
    GetIndexOfKey(key, &index);
    if (index >= 0) {
        return RemoveAt(index, result);
    }

    if (result) {
        *result = NULL;
    }
    return NOERROR;
}

ECode CArrayMap::RemoveAt(
    /* [in] */ Int32 index)
{
    return RemoveAt(index, NULL);
}

ECode CArrayMap::RemoveAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    AutoPtr<IInterface> old = (*mArray)[(index << 1) + 1];
    if (mSize <= 1) {
        // Now empty.
        if (DEBUG) Logger::D(TAG, "remove: shrink from %d to 0", mHashes->GetLength());
        FreeArrays(mHashes, mArray, mSize);
        mHashes = EmptyArray::INT32;
        mArray = EmptyArray::OBJECT;
        mSize = 0;
    }
    else {
        if (mHashes->GetLength() > (BASE_SIZE * 2) && mSize < mHashes->GetLength()/3) {
            // Shrunk enough to reduce size of arrays.  We don't allow it to
            // shrink smaller than (BASE_SIZE*2) to avoid flapping between
            // that and BASE_SIZE.
            Int32 n = mSize > (BASE_SIZE*2) ? (mSize + (mSize>>1)) : (BASE_SIZE*2);

            if (DEBUG) Logger::D(TAG, "remove: shrink from %d to %d", mHashes->GetLength(), n);

            AutoPtr<ArrayOf<Int32> > ohashes = mHashes;
            AutoPtr<ArrayOf<IInterface*> > oarray = mArray;
            AllocArrays(n);

            mSize--;
            if (index > 0) {
                if (DEBUG) Logger::D(TAG, "remove: copy from 0-%d to 0", index);
                mHashes->Copy(ohashes, 0, index);
                mArray->Copy(oarray, 0, index << 1);
            }
            if (index < mSize) {
                if (DEBUG) Logger::D(TAG, "remove: copy from %d - %d to %d", (index+1), mSize, index);
                mHashes->Copy(index, ohashes, index + 1, mSize - index);
                mArray->Copy(index << 1, oarray, (index + 1) << 1, (mSize - index) << 1);
            }
        }
        else {
            mSize--;
            if (index < mSize) {
                if (DEBUG) Logger::D(TAG, "remove: move %d - %d to %d", (index+1), mSize, index);
                mHashes->Copy(index, mHashes, index + 1, mSize - index);
                mArray->Copy(index << 1, mArray, (index + 1) << 1, (mSize - index) << 1);
            }
            mArray->Set(mSize << 1, NULL);
            mArray->Set((mSize << 1) + 1,NULL);
        }
    }

    if (result) {
        *result = old;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CArrayMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode CArrayMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (TO_IINTERFACE(this) == IInterface::Probe(object)) {
        *result = TRUE;
        return NOERROR;
    }

    if (IMap::Probe(object)) {
        IMap* map = IMap::Probe(object);
        Int32 size, osize;
        GetSize(&size);
        map->GetSize(&osize);
        if (size != osize) {
            return NOERROR;
        }

        //try {
        Boolean bval;
        for (Int32 i = 0; i < mSize; i++) {
            AutoPtr<IInterface> key, mine;
            GetKeyAt(i, (IInterface**)&key);
            GetValueAt(i, (IInterface**)&mine);
            AutoPtr<IInterface> theirs;
            map->Get(key, (IInterface**)&theirs);
            if (mine == NULL) {
                if (theirs != NULL || (map->ContainsKey(key, &bval), !bval)) {
                    return NOERROR;
                }
            }
            else if (!Object::Equals(mine, theirs)) {
                return NOERROR;
            }
        }
        // } catch (NullPointerException ignored) {
        //     return false;
        // } catch (ClassCastException ignored) {
        //     return false;
        // }
        *result = TRUE;
    }

    return NOERROR;
}

ECode CArrayMap::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    AutoPtr<ArrayOf<Int32> > hashes = mHashes;
    AutoPtr<ArrayOf<IInterface*> > array = mArray;
    Int32 result = 0;
    for (Int32 i = 0, v = 1, s = mSize; i < s; i++, v+=2) {
        AutoPtr<IInterface> value = (*array)[v];
        result += (*hashes)[i] ^ (value == NULL ? 0 : Object::GetHashCode(value));
    }
    *hash = result;
    return NOERROR;
}

ECode CArrayMap::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean empty;
    IsEmpty(&empty);
    if (empty) {
        *result = String("{}");
        return NOERROR;
    }

    StringBuilder buffer(mSize * 28);
    buffer.AppendChar('{');
    for (Int32 i=0; i<mSize; i++) {
        if (i > 0) {
            buffer.Append(", ");
        }
        AutoPtr<IInterface> key;
        GetKeyAt(i, (IInterface**)&key);
        if (key.Get() != TO_IINTERFACE(this)) {
            buffer.Append(key);
        }
        else {
            buffer.Append("(this Map)");
        }
        buffer.AppendChar('=');
        AutoPtr<IInterface> value;
        GetValueAt(i, (IInterface**)&value);
        if (value.Get() != TO_IINTERFACE(this)) {
            buffer.Append(value);
        }
        else {
            buffer.Append("(this Map)");
        }
    }
    buffer.AppendChar('}');
    *result = buffer.ToString();
    return NOERROR;
}

AutoPtr<MapCollections> CArrayMap::GetCollection()
{
    if (mCollections == NULL) {
        mCollections = new InnerMapCollections(this);
    }
    return mCollections;
}

ECode CArrayMap::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MapCollections::ContainsAllHelper(THIS_PROBE(IMap), collection);
    return NOERROR;
}

ECode CArrayMap::PutAll(
    /* [in] */ IMap* map)
{
    VALIDATE_NOT_NULL(map)
    Int32 size;
    map->GetSize(&size);
    EnsureCapacity(mSize + size);
    AutoPtr<ISet> entrySet;
    map->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IMapEntry* entry;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        entry = IMapEntry::Probe(next);
        AutoPtr<IInterface> key, value;
        entry->GetKey((IInterface**)&key);
        entry->GetValue((IInterface**)&value);
        Put(key, value);
    }
    return NOERROR;
}

ECode CArrayMap::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return RemoveAll(collection, &result);
}

ECode CArrayMap::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MapCollections::RemoveAllHelper(THIS_PROBE(IMap), collection);
    return NOERROR;
}

ECode CArrayMap::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return RetainAll(collection, &result);
}

ECode CArrayMap::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MapCollections::RetainAllHelper(THIS_PROBE(IMap), collection);
    return NOERROR;
}

ECode CArrayMap::GetEntrySet(
    /* [out] */ ISet** set)
{
    VALIDATE_NOT_NULL(set)
    AutoPtr<ISet> r = GetCollection()->GetEntrySet();
    *set = r;
    REFCOUNT_ADD(*set)
    return NOERROR;
}

ECode CArrayMap::GetKeySet(
    /* [out] */ ISet** set)
{
    VALIDATE_NOT_NULL(set)
    AutoPtr<ISet> r = GetCollection()->GetKeySet();
    *set = r;
    REFCOUNT_ADD(*set)
    return NOERROR;
}

ECode CArrayMap::GetValues(
    /* [out] */ ICollection** set)
{
    VALIDATE_NOT_NULL(set)
    AutoPtr<ICollection> r = GetCollection()->GetValues();
    *set = r;
    REFCOUNT_ADD(*set)
    return NOERROR;
}

} // Utility
} // Droid
} // Elastos