#include "elastos/droid/utility/CArraySet.h"
#include "elastos/droid/utility/ContainerHelpers.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Core::StringBuilder;
using Libcore::Utility::EmptyArray;
using Elastos::Utility::EIID_IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

const Boolean CArraySet::DEBUG = FALSE;
const String CArraySet::TAG("CArraySet");
const Int32 CArraySet::BASE_SIZE = 4;
const Int32 CArraySet::CACHE_SIZE = 10;

AutoPtr<ArrayOf<IInterface*> > CArraySet::mBaseCache;
Int32 CArraySet::mBaseCacheSize = 0;
AutoPtr<ArrayOf<IInterface*> > CArraySet::mTwiceBaseCache;
Int32 CArraySet::mTwiceBaseCacheSize = 0;
Object CArraySet::sLock;

//======================================================================
// CArraySet::InnerMapCollections
//======================================================================
CArraySet::InnerMapCollections::InnerMapCollections(
    /* [in] */ CArraySet* host)
    : mHost(host)
{
}

Int32 CArraySet::InnerMapCollections::ColGetSize()
{
    return mHost->mSize;
}

AutoPtr<IInterface> CArraySet::InnerMapCollections::ColGetEntry(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset)
{
    return (*mHost->mArray)[index];
}

Int32 CArraySet::InnerMapCollections::ColIndexOfKey(
    /* [in] */ IInterface* key)
{
    Int32 index;
    mHost->GetIndexOf(key, &index);
    return index;
}

Int32 CArraySet::InnerMapCollections::ColIndexOfValue(
    /* [in] */ IInterface* value)
{
    Int32 index;
    mHost->GetIndexOf(value, &index);
    return index;
}

AutoPtr<IMap> CArraySet::InnerMapCollections::ColGetMap()
{
    Logger::E("CArraySet::InnerMapCollections", "throw new UnsupportedOperationException: not a map");
    assert(0);
    return NULL;
}

void CArraySet::InnerMapCollections::ColPut(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    mHost->Add(key);
}

AutoPtr<IInterface> CArraySet::InnerMapCollections::ColSetValue(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    Logger::E("CArraySet::InnerMapCollections", "throw new UnsupportedOperationException: not a map");
    assert(0);
    return NULL;
}

void CArraySet::InnerMapCollections::ColRemoveAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> old;
    mHost->RemoveAt(index, (IInterface**)&old);
}

void CArraySet::InnerMapCollections::ColClear()
{
    mHost->Clear();
}

//======================================================================
//
//======================================================================
CAR_INTERFACE_IMPL_2(CArraySet, Object, IArrayMap, IMap)

CAR_OBJECT_IMPL(CArraySet)

CArraySet::CArraySet()
    : mSize(0)
{
}

ECode CArraySet::constructor()
{
    mHashes = EmptyArray::INT32;
    mArray = EmptyArray::OBJECT;
    mSize = 0;
    return NOERROR;
}

ECode CArraySet::constructor(
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

ECode CArraySet::constructor(
    /* [in] */ IArraySet* set)
{
    constructor();
    if (set != NULL) {
        AddAll(set);
    }
    return NOERROR;
}

Int32 CArraySet::GetIndexOf(
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
    if (Object::Equals(key, (*mArray)[index])) {
        return index;
    }

    // Search for a matching key after the index.
    Int32 end;
    for (end = index + 1; end < N && (*mHashes)[end] == hash; end++) {
        if (Object::Equals(key, (*mArray)[end])) return end;
    }

    // Search for a matching key before the index.
    for (Int32 i = index - 1; i >= 0 && (*mHashes)[i] == hash; i--) {
        if (Object::Equals(key, (*mArray)[i])) return i;
    }

    // Key not found -- return negative value indicating where a
    // new entry for this key should go.  We use the end of the
    // hash chain to reduce the number of array entries that will
    // need to be copied when inserting.
    return ~end;
}

Int32 CArraySet::GetIndexOfNull()
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
    if (NULL == (*mArray)[index]) {
        return index;
    }

    // Search for a matching key after the index.
    Int32 end;
    for (end = index + 1; end < N && (*mHashes)[end] == 0; end++) {
        if (NULL == (*mArray)[end]) return end;
    }

    // Search for a matching key before the index.
    for (Int32 i = index - 1; i >= 0 && (*mHashes)[i] == 0; i--) {
        if (NULL == (*mArray)[i]) return i;
    }

    // Key not found -- return negative value indicating where a
    // new entry for this key should go.  We use the end of the
    // hash chain to reduce the number of array entries that will
    // need to be copied when inserting.
    return ~end;
}

ECode CArraySet::AllocArrays(
    /* [in] */ Int32 size)
{
    if (size == (BASE_SIZE * 2)) {
        synchronized(sLock) {
            if (mTwiceBaseCache != NULL) {
                AutoPtr<ArrayOf<IInterface*> > array = mTwiceBaseCache;
                mArray = array;
                AutoPtr<ObjectsEntry> oe = (ObjectsEntry*)(IObject*)(*array)[0];
                AutoPtr<HashesEntry> he = (HashesEntry*)(IObject*)(*array)[1];
                mTwiceBaseCache = oe->mObjects;
                mHashes = he->mHashes;

                array->Set(0, NULL);
                array->Set(1, NULL);
                mTwiceBaseCacheSize--;
                if (DEBUG) Logger::D(TAG, "Retrieving 2x cache %p now have %d entries", mHashes.Get(), mTwiceBaseCacheSize);
                return NOERROR;
            }
        }
    }
    else if (size == BASE_SIZE) {
        synchronized(sLock) {
            if (mBaseCache != NULL) {
                AutoPtr<ArrayOf<IInterface*> > array = mBaseCache;
                mArray = array;
                AutoPtr<ObjectsEntry> oe = (ObjectsEntry*)(IObject*)(*array)[0];
                AutoPtr<HashesEntry> he = (HashesEntry*)(IObject*)(*array)[1];
                mBaseCache = oe->mObjects;
                mHashes = he->mHashes;

                array->Set(0, NULL);
                array->Set(1, NULL);
                mBaseCacheSize--;
                if (DEBUG) Logger::D(TAG, "Retrieving 1x cache %p now have %d entries", mHashes.Get(), mBaseCacheSize);
                return NOERROR;
            }
        }
    }

    mHashes = ArrayOf<Int32>::Alloc(size);
    mArray = ArrayOf<IInterface*>::Alloc(size);
    return NOERROR;
}

ECode CArraySet::FreeArrays(
    /* [in] */ ArrayOf<Int32>* hashes,
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 size)
{
    if (hashes->GetLength() == (BASE_SIZE * 2)) {
        synchronized(sLock) {
            if (mTwiceBaseCacheSize < CACHE_SIZE) {
                AutoPtr<IInterface> oe = (IObject*)new ObjectsEntry(mTwiceBaseCache.Get());
                AutoPtr<IInterface> he = (IObject*)new HashesEntry(hashes);
                array->Set(0, oe);
                array->Set(1, he);

                for (Int32 i = size - 1; i >= 2; i--) {
                    array->Set(i, NULL);
                }
                mTwiceBaseCache = array;
                mTwiceBaseCacheSize++;
                if (DEBUG) Logger::D(TAG, "Storing 2x cache %p now have %d entries", array, mTwiceBaseCacheSize);
            }
        }
    }
    else if (hashes->GetLength() == BASE_SIZE) {
        synchronized(sLock) {
            if (mBaseCacheSize < CACHE_SIZE) {
                AutoPtr<IInterface> oe = (IObject*)new ObjectsEntry(mBaseCache.Get());
                AutoPtr<IInterface> he = (IObject*)new HashesEntry(hashes);
                array->Set(0, oe);
                array->Set(1, he);

                for (Int32 i= size - 1; i >= 2; i--) {
                    array->Set(i, NULL);
                }

                mBaseCache = array;
                mBaseCacheSize++;
                if (DEBUG) Logger::D(TAG, "Storing 1x cache %p now have %d entries", array, mBaseCacheSize);
            }
        }
    }
    return NOERROR;
}

ECode CArraySet::Clear()
{
    if (mSize != 0) {
        FreeArrays(mHashes, mArray, mSize);
        mHashes = EmptyArray::INT32;
        mArray = EmptyArray::OBJECT;
        mSize = 0;
    }
    return NOERROR;
}

ECode CArraySet::EnsureCapacity(
    /* [in] */ Int32 minimumCapacity)
{
    if (mHashes->GetLength() < minimumCapacity) {
        AutoPtr<ArrayOf<Int32> > ohashes = mHashes;
        AutoPtr<ArrayOf<IInterface*> > oarray = mArray;
        AllocArrays(minimumCapacity);
        if (mSize > 0) {
            mHashes->Copy(ohashes, 0, mSize);
            mArray->Copy(oarray, 0, mSize);
        }
        FreeArrays(ohashes, oarray, mSize);
    }
    return NOERROR;
}

ECode CArraySet::Contains(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index;
    GetIndexOf(key, &index);
    *result = index >= 0;
    return NOERROR;
}

ECode CArraySet::GetIndexOf(
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

ECode CArraySet::GetValueAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = (*mArray)[index];
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CArraySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize <= 0;
    return NOERROR;
}

ECode CArraySet::Add(
    /* [in] */ IInterface* value)
{
    Boolean result;
    return Add(value, &result);
}

ECode CArraySet::Add(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 hash;
    Int32 index;
    if (value == NULL) {
        hash = 0;
        index = GetIndexOfNull();
    }
    else {
        hash = Object::GetHashCode(value);
        index = GetIndexOf(value, hash);
    }
    if (index >= 0) {
        return NOERROR;
    }

    index = ~index;
    if (mSize >= mHashes->GetLength()) {
        Int32 n = mSize >= (BASE_SIZE * 2) ? (mSize + (mSize >> 1))
                : (mSize >= BASE_SIZE ? (BASE_SIZE * 2) : BASE_SIZE);

        if (DEBUG) Logger::D(TAG, "add: grow from %d to n", mHashes->GetLength(), n);

        AutoPtr<ArrayOf<Int32> > ohashes = mHashes;
        AutoPtr<ArrayOf<IInterface*> > oarray = mArray;
        AllocArrays(n);

        if (mHashes->GetLength() > 0) {
            if (DEBUG) Logger::D(TAG, "add: copy 0- to 0", mSize);
            mHashes->Copy(ohashes);
            mArray->Copy(oarray);
        }

        FreeArrays(ohashes, oarray, mSize);
    }

    if (index < mSize) {
        if (DEBUG) Logger::D(TAG, "add: move %d-%d to %d", index, (mSize-index), (index+1));
        mHashes->Copy(index + 1, mHashes, index, mSize - index);
        mArray->Copy((index + 1), mArray, index, mSize - index);
    }

    mHashes->Set(index, hash);
    mArray->Set(index, value);
    mSize++;
    *result = TRUE;
    return NOERROR;
}

ECode CArraySet::AddAll(
    /* [in] */ IArraySet* set)
{
    CArraySet* array = (CArraySet*)set;
    Int32 N = array->mSize;
    EnsureCapacity(mSize + N);
    if (mSize == 0) {
        if (N > 0) {
            mHashes->Copy(array->mHashes, 0, N);
            mArray->Copy(mArray, 0, N);
            mSize = N;
        }
    }
    else {
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> key, value;
            array->GetValueAt(i, (IInterface**)&value);
            Add(value);
        }
    }
    return NOERROR;
}

ECode CArraySet::Remove(
    /* [in] */ IInterface* value)
{
    Boolean result;
    return Remove(value, &result);
}

ECode CArraySet::Remove(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 index;
    GetIndexOf(value, &index);
    if (index >= 0) {
        AutoPtr<IInterface> old;
        RemoveAt(index, (IInterface**)&old);
        *result = TRUE;
    }

    return NOERROR;
}

ECode CArraySet::RemoveAt(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> old = (*mArray)[index];
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
                mArray->Copy(oarray, 0, index);
            }
            if (index < mSize) {
                if (DEBUG) Logger::D(TAG, "remove: copy from %d - %d to %d", (index+1), mSize, index);
                mHashes->Copy(index, ohashes, index + 1, mSize - index);
                mArray->Copy(index, oarray, index + 1, mSize - index);
            }
        }
        else {
            mSize--;
            if (index < mSize) {
                if (DEBUG) Logger::D(TAG, "remove: move %d - %d to %d", (index+1), mSize, index);
                mHashes->Copy(index, mHashes, index + 1, mSize - index);
                mArray->Copy(index, mArray, index + 1, mSize - index);
            }
            mArray->Set(mSize, NULL);
            mArray->Set(mSize + 1,NULL);
        }
    }

    *result = old;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CArraySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}


ECode CArraySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)
    AutoPtr<ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(mSize);
    result->Copy(mArray, mSize);
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CArraySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    *outArray = NULL;
    VALIDATE_NOT_NULL(inArray)
    AutoPtr<ArrayOf<IInterface*> > array = inArray;
    if (array->GetLength() < mSize) {
        array = ArrayOf<IInterface*>::Alloc(mSize);
    }
    array->Copy(mArray, mSize);

    if (array->GetLength() > mSize) {
        array->Set(mSize, NULL);
    }
    *outArray = array;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode CArraySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (TO_IINTERFACE(this) == object) {
        *result = TRUE;
        return NOERROR;
    }

    if (ISet::Probe(object)) {
        ISet* set = ISet::Probe(object);
        Int32 size, osize;
        GetSize(&size);
        set->GetSize(&osize);
        if (size != osize) {
            return NOERROR;
        }

        //try {
        Boolean bval;
        for (Int32 i = 0; i < mSize; i++) {
            AutoPtr<IInterface> mine;
            GetValueAt(i, (IInterface**)&mine);
            if ((set->Contains(mine, &bval), !bval)) {
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

ECode CArraySet::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    AutoPtr<ArrayOf<Int32> > hashes = mHashes;
    Int32 result = 0;
    for (Int32 i = 0, v = 1, s = mSize; i < s; i++, v+=2) {
        result += (*hashes)[i];
    }
    *hash = result;
    return NOERROR;
}

ECode CArraySet::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean empty;
    IsEmpty(&empty);
    if (empty) {
        *result = String("{}");
        return NOERROR;
    }

    StringBuilder buffer(mSize * 14);
    buffer.AppendChar('{');
    for (Int32 i=0; i<mSize; i++) {
        if (i > 0) {
            buffer.Append(", ");
        }

        AutoPtr<IInterface> value;
        GetValueAt(i, (IInterface**)&value);
        if (value.Get() != TO_IINTERFACE(this)) {
            buffer.Append(value);
        }
        else {
            buffer.Append("(this Set)");
        }
    }
    buffer.AppendChar('}');
    *result = buffer.ToString();
    return NOERROR;
}

AutoPtr<MapCollections> CArraySet::GetCollection()
{
    if (mCollections == NULL) {
        mCollections = new InnerMapCollections(this);
    }
    return NULL;
}

ECode CArraySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    AutoPtr<ISet> keySet = GetCollection()->GetKeySet();
    return keySet->GetIterator((IIterator**)&it);
}

ECode CArraySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext, contains;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (Contains(obj, &contains), !contains) {
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CArraySet::AddAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return AddAll(collection, &result);
}

ECode CArraySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(collection)

    Int32 size;
    collection->GetSize(&size);
    EnsureCapacity(mSize + size);
    Boolean added = FALSE;

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext, bval;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Add(value, &bval);
        added |= bval;
    }

    *result = added;
    return NOERROR;
}

ECode CArraySet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return RemoveAll(collection, &result);
}

ECode CArraySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(collection)

    Boolean removed = FALSE;

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext, bval;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Remove(value, &bval);
        removed |= bval;
    }

    *result = removed;
    return NOERROR;
}

ECode CArraySet::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return RetainAll(collection, &result);
}

ECode CArraySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(collection)

    Boolean removed = FALSE, contains;

    for (Int32 i = mSize - 1; i >= 0; i--) {
        if (collection->Contains((*mArray)[i], &contains), !contains) {
            AutoPtr<IInterface> value;
            RemoveAt(i, (IInterface**)&value);
            removed = TRUE;
        }
    }

    *result = removed;
    return NOERROR;
}


} // Utility
} // Droid
} // Elastos