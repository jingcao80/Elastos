

#include "CLinkedHashMap.h"


namespace Elastos {
namespace Utility {


//==========================================================
//       CLinkedHashMap
//==========================================================
CAR_INTERFACE_IMPL(CLinkedHashMap, HashMap, ILinkedHashMap);

ECode CLinkedHashMap::constructor()
{
    HashMap::constructor();
    Init_();
    mAccessOrder = FALSE;
    return NOERROR;
}

ECode CLinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity)
{
    return constructor(initialCapacity, DEFAULT_LOAD_FACTOR);
}

ECode CLinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ Float loadFactor)
{
    return constructor(initialCapacity, loadFactor, FALSE);
}

ECode CLinkedHashMap::constructor(
    /* [in] */ Int32 initialCapacity,
    /* [in] */ Float loadFactor,
    /* [in] */ Boolean accessOrder)
{
    FAIL_RETURN(HashMap::constructor(initialCapacity, loadFactor));
    Init_();
    mAccessOrder = accessOrder;
    return NOERROR;
}

ECode CLinkedHashMap::constructor(
    /* [in] */ IMap* map)
{
    Int32 sizelen = 0;
    map->GetSize(&sizelen);
    FAIL_RETURN(constructor(CapacityForInitSize(sizelen)));
    return ConstructorPutAll(map);
}

ECode CLinkedHashMap::Init_()
{
    mHeader = new LinkedEntry();
    return NOERROR;
}

ECode CLinkedHashMap::Eldest(
    /* [out] */ IMapEntry** outent)
{
    VALIDATE_NOT_NULL(outent)

    AutoPtr<LinkedEntry> eldest = mHeader->mNxt;
    AutoPtr<IMapEntry> entry = eldest != mHeader ? IMapEntry::Probe(eldest) : NULL;
    *outent = entry;
    REFCOUNT_ADD(*outent)
    return NOERROR;
}

ECode CLinkedHashMap::AddNewEntry(
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

ECode CLinkedHashMap::AddNewEntryForNullKey(
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

AutoPtr<CLinkedHashMap::HashMapEntry> CLinkedHashMap::ConstructorNewEntry(
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

ECode CLinkedHashMap::Get(
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

void CLinkedHashMap::MakeTail(
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

ECode CLinkedHashMap::PreModify(
    /* [in] */ HashMapEntry* e)
{
    if (mAccessOrder) {
        MakeTail((LinkedEntry*)e);
    }
    return NOERROR;
}

ECode CLinkedHashMap::PostRemove(
    /* [in] */ HashMapEntry* e)
{
    AutoPtr<LinkedEntry> le = (LinkedEntry*) e;
    le->mPrv->mNxt = le->mNxt;
    le->mNxt->mPrv = le->mPrv;
    le->mNxt = le->mPrv = NULL; // Help the GC (for performance)
    return NOERROR;
}

ECode CLinkedHashMap::ContainsValue(
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

ECode CLinkedHashMap::Clear()
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

AutoPtr<IIterator> CLinkedHashMap::NewKeyIterator()
{
    AutoPtr<IIterator> res = new KeyIterator(this);
    return res;
}

AutoPtr<IIterator> CLinkedHashMap::NewValueIterator()
{
    AutoPtr<IIterator> res = new ValueIterator(this);
    return res;
}

AutoPtr<IIterator> CLinkedHashMap::NewEntryIterator()
{
    AutoPtr<IIterator> res = new EntryIterator(this);
    return res;
}

Boolean CLinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    return FALSE;
}

ECode CLinkedHashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return HashMap::ContainsKey(key, result);
}

ECode CLinkedHashMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    return HashMap::GetEntrySet(entries);
}

ECode CLinkedHashMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return HashMap::Equals(object, result);
}

ECode CLinkedHashMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return HashMap::GetHashCode(hashCode);
}

ECode CLinkedHashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return HashMap::IsEmpty(result);
}

ECode CLinkedHashMap::GetKeySet(
    /* [out] */ ISet** keySet)
{
    return HashMap::GetKeySet(keySet);
}

ECode CLinkedHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return HashMap::Put(key, value, oldValue);
}

ECode CLinkedHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return HashMap::Put(key, value);
}

ECode CLinkedHashMap::PutAll(
    /* [in] */ IMap* map)
{
    return HashMap::PutAll(map);
}

ECode CLinkedHashMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return HashMap::Remove(key, value);
}

ECode CLinkedHashMap::Remove(
    /* [in] */ PInterface key)
{
    return HashMap::Remove(key);
}

ECode CLinkedHashMap::GetSize(
    /* [out] */ Int32* size)
{
    return HashMap::GetSize(size);
}

ECode CLinkedHashMap::GetValues(
    /* [out] */ ICollection** value)
{
    return HashMap::GetValues(value);
}

ECode CLinkedHashMap::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    /*
     * This could be made more efficient. It unnecessarily hashes all of
     * the elements in the map.
     */
    AutoPtr<CLinkedHashMap> result;
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
//       CLinkedHashMap::LinkedHashIterator
//==========================================================
CAR_INTERFACE_IMPL(CLinkedHashMap::LinkedHashIterator, Object, IIterator);

CLinkedHashMap::LinkedHashIterator::LinkedHashIterator(
    /* [in] */ CLinkedHashMap* host)
{
    mHost = host;
    mNext = mHost->mHeader->mNxt;
    mLastReturned = NULL;
    mExpectedModCount = mHost->mModCount;
}

ECode CLinkedHashMap::LinkedHashIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mNext != mHost->mHeader;
    return NOERROR;
}

AutoPtr<CLinkedHashMap::LinkedEntry> CLinkedHashMap::LinkedHashIterator::NextEntry()
{
    if (mHost->mModCount != mExpectedModCount) {
        // throw new ConcurrentModificationException();
        return NULL;
    }
    AutoPtr<LinkedEntry> e = mNext;
    if (e == mHost->mHeader) {
        // throw new NoSuchElementException();
        return NULL;
    }
    mNext = e->mNxt;
    return mLastReturned = e;
}

ECode CLinkedHashMap::LinkedHashIterator::Remove()
{
    if (mHost->mModCount != mExpectedModCount) {
        // throw new ConcurrentModificationException();
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    if (mLastReturned == NULL) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> outface;
    mHost->Remove(mLastReturned->mKey, (IInterface**)&outface);
    mLastReturned = NULL;
    mExpectedModCount = mHost->mModCount;
    return NOERROR;
}


//==========================================================
//       CLinkedHashMap::KeyIterator
//==========================================================
CLinkedHashMap::KeyIterator::KeyIterator(
    /* [in] */ CLinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode CLinkedHashMap::KeyIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res = NextEntry();
    *outface = res->mKey;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}


//==========================================================
//       CLinkedHashMap::ValueIterator
//==========================================================
CLinkedHashMap::ValueIterator::ValueIterator(
    /* [in] */ CLinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode CLinkedHashMap::ValueIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res = NextEntry();
    *outface = res->mValue;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}


//==========================================================
//       CLinkedHashMap::EntryIterator
//==========================================================
CLinkedHashMap::EntryIterator::EntryIterator(
    /* [in] */ CLinkedHashMap* host)
    : LinkedHashIterator(host)
{
}

ECode CLinkedHashMap::EntryIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<LinkedEntry> res = NextEntry();
    *outface = IMapEntry::Probe(res);
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
