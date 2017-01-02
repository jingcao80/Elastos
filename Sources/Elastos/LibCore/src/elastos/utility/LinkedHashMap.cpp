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

#include "LinkedHashMap.h"
#include "CLinkedHashMap.h"

namespace Elastos {
namespace Utility {


//==========================================================
//       LinkedHashMap
//==========================================================
CAR_INTERFACE_IMPL(LinkedHashMap, HashMap, ILinkedHashMap)

LinkedHashMap::~LinkedHashMap()
{
    if (mHeader) {
        Clear();
        // mHeader adds 2 refcount in LinkedEntry()
        mHeader->mNxt = NULL;
        mHeader->mPrv = NULL;
    }
}

ECode LinkedHashMap::constructor()
{
    HashMap::constructor();
    Init_();
    mAccessOrder = FALSE;
    return NOERROR;
}

ECode LinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity)
{
    return constructor(initialCapacity, DEFAULT_LOAD_FACTOR);
}

ECode LinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ Float loadFactor)
{
    return constructor(initialCapacity, loadFactor, FALSE);
}

ECode LinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ Float loadFactor,
    /* [in] */ Boolean accessOrder)
{
    FAIL_RETURN(HashMap::constructor(initialCapacity, loadFactor));
    Init_();
    mAccessOrder = accessOrder;
    return NOERROR;
}

ECode LinkedHashMap::constructor(
    /* [in] */ IMap* map)
{
    Int32 sizelen = 0;
    map->GetSize(&sizelen);
    FAIL_RETURN(constructor(CapacityForInitSize(sizelen)));
    return ConstructorPutAll(map);
}

ECode LinkedHashMap::Init_()
{
    mHeader = new LinkedEntry();
    return NOERROR;
}

ECode LinkedHashMap::GetEldest(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent)

    AutoPtr<LinkedEntry> eldest = mHeader->mNxt;
    AutoPtr<IMapEntry> entry = eldest != mHeader ? IMapEntry::Probe(eldest) : NULL;
    *outent = entry;
    REFCOUNT_ADD(*outent)
    return NOERROR;
}

ECode LinkedHashMap::AddNewEntry(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Int32 hash,
    /* [in] */ Int32 index)
{
    AutoPtr<LinkedEntry> header = mHeader;

    // Remove eldest entry if instructed to do so.
    AutoPtr<LinkedEntry> eldest = header->mNxt;
    if (eldest != header && RemoveEldestEntry(eldest)) {
        AutoPtr<IInterface> outface;
        Remove(eldest->mKey, (IInterface**)&outface);
    }

    // Create new entry, link it on to list, and put it into table
    AutoPtr<LinkedEntry> oldTail = header->mPrv;
    AutoPtr<LinkedEntry> newTail = new LinkedEntry(key, value, hash, (*mTable)[index], header, oldTail);
    oldTail->mNxt = newTail;
    header->mPrv = newTail;
    mTable->Set(index, newTail);
    return NOERROR;
}

ECode LinkedHashMap::AddNewEntryForNullKey(
    /* [in] */ IInterface* value)
{
    AutoPtr<LinkedEntry> header = mHeader;

    // Remove eldest entry if instructed to do so.
    AutoPtr<LinkedEntry> eldest = header->mNxt;
    if (eldest != header && RemoveEldestEntry(eldest)) {
        AutoPtr<IInterface> outface;
        Remove(eldest->mKey, (IInterface**)&outface);
    }

    // Create new entry, link it on to list, and put it into table
    AutoPtr<LinkedEntry> oldTail = header->mPrv;
    AutoPtr<LinkedEntry> newTail = new LinkedEntry(
            NULL, value, 0, NULL, header, oldTail);
    mEntryForNullKey = oldTail->mNxt = header->mPrv = newTail;
    return NOERROR;
}

AutoPtr<LinkedHashMap::HashMapEntry> LinkedHashMap::ConstructorNewEntry(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [in] */ Int32 hash,
    /* [in] */ HashMapEntry* first)
{
    AutoPtr<LinkedEntry> header = mHeader;
    AutoPtr<LinkedEntry> oldTail = header->mPrv;
    AutoPtr<LinkedEntry> newTail
            = new LinkedEntry(key, value, hash, first, header, oldTail);
    return oldTail->mNxt = header->mPrv = newTail;
}

ECode LinkedHashMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    /*
     * This method is overridden to eliminate the need for a polymorphic
     * invocation in superclass at the expense of code duplication.
     */
    if (key == NULL) {
        AutoPtr<HashMapEntry> e = mEntryForNullKey;
        if (e == NULL) {
            *value = NULL;
            return NOERROR;
        }
        if (mAccessOrder)
            MakeTail((LinkedEntry*)e.Get());
        *value = e->mValue;
        REFCOUNT_ADD(*value)
        return NOERROR;
    }

    // Doug Lea's supplemental secondaryHash function (inlined)
    Int32 hash = Object::GetHashCode(key);
    hash ^= ((UInt32)hash >> 20) ^ ((UInt32)hash >> 12);
    hash ^= ((UInt32)hash >> 7) ^ ((UInt32)hash >> 4);

    AutoPtr< ArrayOf<HashMapEntry*> > tab = mTable;
    for (HashMapEntry * e = (*tab)[hash & (tab->GetLength() - 1)];
            e != NULL; e = e->mNext) {
        AutoPtr<IInterface> eKey = e->mKey;
        if (eKey.Get() == key || (e->mHash == hash && Object::Equals(key, eKey))) {
            if (mAccessOrder) {
                MakeTail((LinkedEntry*)e);
            }
            *value = e->mValue;
            REFCOUNT_ADD(*value)
            return NOERROR;
        }
    }
    *value = NULL;
    return NOERROR;
}

void LinkedHashMap::MakeTail(
    /* [in] */ LinkedEntry* e)
{
    // Unlink e
    e->mPrv->mNxt = e->mNxt;
    e->mNxt->mPrv = e->mPrv;

    // Relink e as tail
    AutoPtr<LinkedEntry> header = mHeader;
    AutoPtr<LinkedEntry> oldTail = header->mPrv;
    e->mNxt = header;
    e->mPrv = oldTail;
    oldTail->mNxt = header->mPrv = e;
    mModCount++;
}

ECode LinkedHashMap::PreModify(
    /* [in] */ HashMapEntry* e)
{
    if (mAccessOrder) {
        MakeTail((LinkedEntry*)e);
    }
    return NOERROR;
}

ECode LinkedHashMap::PostRemove(
    /* [in] */ HashMapEntry* e)
{
    AutoPtr<LinkedEntry> le = (LinkedEntry*) e;
    le->mPrv->mNxt = le->mNxt;
    le->mNxt->mPrv = le->mPrv;
    le->mNxt = le->mPrv = NULL; // Help the GC (for performance)
    return NOERROR;
}

ECode LinkedHashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<LinkedEntry> header = mHeader;
    AutoPtr<LinkedEntry> e;
    if (value == NULL) {
        for (e = header->mNxt; e != header; e = e->mNxt) {
            if (e->mValue == NULL) {
                *result = TRUE;
                return NOERROR;
            }
        }
        *result = FALSE;
        return NOERROR;
    }

    // value is non-NULL
    for (e = header->mNxt; e != header; e = e->mNxt) {
        if (Object::Equals(value, e->mValue)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkedHashMap::Clear()
{
    HashMap::Clear();

    // Clear all links to help GC
    AutoPtr<LinkedEntry> header = mHeader;
    for (AutoPtr<LinkedEntry> e = header->mNxt; e != header; ) {
        AutoPtr<LinkedEntry> nxt = e->mNxt;
        e->mNxt = e->mPrv = NULL;
        e = nxt;
    }

    header->mNxt = header->mPrv = header;
    return NOERROR;
}

AutoPtr<IIterator> LinkedHashMap::NewKeyIterator()
{
    AutoPtr<IIterator> res = new KeyIterator(this);
    return res;
}

AutoPtr<IIterator> LinkedHashMap::NewValueIterator()
{
    AutoPtr<IIterator> res = new ValueIterator(this);
    return res;
}

AutoPtr<IIterator> LinkedHashMap::NewEntryIterator()
{
    AutoPtr<IIterator> res = new EntryIterator(this);
    return res;
}

Boolean LinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    return FALSE;
}

ECode LinkedHashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return HashMap::ContainsKey(key, result);
}

ECode LinkedHashMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    return HashMap::GetEntrySet(entries);
}

ECode LinkedHashMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return HashMap::Equals(object, result);
}

ECode LinkedHashMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return HashMap::GetHashCode(hashCode);
}

ECode LinkedHashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return HashMap::IsEmpty(result);
}

ECode LinkedHashMap::GetKeySet(
    /* [out] */ ISet** keySet)
{
    return HashMap::GetKeySet(keySet);
}

ECode LinkedHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return HashMap::Put(key, value, oldValue);
}

ECode LinkedHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return HashMap::Put(key, value);
}

ECode LinkedHashMap::PutAll(
    /* [in] */ IMap* map)
{
    return HashMap::PutAll(map);
}

ECode LinkedHashMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return HashMap::Remove(key, value);
}

ECode LinkedHashMap::Remove(
    /* [in] */ PInterface key)
{
    return HashMap::Remove(key);
}

ECode LinkedHashMap::GetSize(
    /* [out] */ Int32* size)
{
    return HashMap::GetSize(size);
}

ECode LinkedHashMap::GetValues(
    /* [out] */ ICollection** value)
{
    return HashMap::GetValues(value);
}

ECode LinkedHashMap::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    /*
     * This could be made more efficient. It unnecessarily hashes all of
     * the elements in the map.
     */
    AutoPtr<LinkedHashMap> result;
    // try {
    // result = (HashMap<K, V>) super.clone();
    CLinkedHashMap::NewByFriend((CLinkedHashMap**)&result);
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }

    // Restore clone to empty state, retaining our capacity and threshold
    // result->MakeTable(mTable->GetLength());
    // result->mEntryForNullKey = NULL;
    // result->mSize = 0;
    // result->mKeySet = NULL;
    // result->mEntrySet = NULL;
    // result->mValues = NULL;

    result->Init_(); // Give subclass a chance to initialize itself
    result->ConstructorPutAll((IMap*)this->Probe(EIID_IMap)); // Calls method overridden in subclass!!
    AutoPtr<IMap> outmap = (IMap*) result->Probe(EIID_IMap);
    *object = outmap;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

//==========================================================
//       LinkedHashMap::LinkedHashIterator
//==========================================================
CAR_INTERFACE_IMPL(LinkedHashMap::LinkedHashIterator, Object, IIterator)

LinkedHashMap::LinkedHashIterator::LinkedHashIterator(
    /* [in] */ LinkedHashMap* host)
{
    mHost = host;
    mNext = mHost->mHeader->mNxt;
    mLastReturned = NULL;
    mExpectedModCount = mHost->mModCount;
}

ECode LinkedHashMap::LinkedHashIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mNext != mHost->mHeader;
    return NOERROR;
}

ECode LinkedHashMap::LinkedHashIterator::NextEntry(LinkedEntry** outface)
{
    VALIDATE_NOT_NULL(outface)
    *outface = NULL;

    if (mHost->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    AutoPtr<LinkedEntry> e = mNext;
    if (e == mHost->mHeader) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mNext = e->mNxt;
    mLastReturned = e;

    *outface = mLastReturned;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode LinkedHashMap::LinkedHashIterator::Remove()
{
    if (mHost->mModCount != mExpectedModCount) {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (mLastReturned == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> outface;
    mHost->Remove(mLastReturned->mKey, (IInterface**)&outface);
    mLastReturned = NULL;
    mExpectedModCount = mHost->mModCount;
    return NOERROR;
}


//==========================================================
//       LinkedHashMap::KeyIterator
//==========================================================
LinkedHashMap::KeyIterator::KeyIterator(
    /* [in] */ LinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode LinkedHashMap::KeyIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res;
    FAIL_RETURN(NextEntry((LinkedEntry**)&res));
    *outface = res->mKey;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}


//==========================================================
//       LinkedHashMap::ValueIterator
//==========================================================
LinkedHashMap::ValueIterator::ValueIterator(
    /* [in] */ LinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode LinkedHashMap::ValueIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res;
    FAIL_RETURN(NextEntry((LinkedEntry**)&res));
    *outface = res->mValue;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}


//==========================================================
//       LinkedHashMap::EntryIterator
//==========================================================
LinkedHashMap::EntryIterator::EntryIterator(
    /* [in] */ LinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode LinkedHashMap::EntryIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res;
    FAIL_RETURN(NextEntry((LinkedEntry**)&res));
    *outface = IMapEntry::Probe(res);
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
