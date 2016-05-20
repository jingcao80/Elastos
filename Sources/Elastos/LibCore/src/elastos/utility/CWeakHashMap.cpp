#include "CWeakHashMap.h"
#include "CArrayList.h"


namespace Elastos {
namespace Utility {

//==========================================================
//       CWeakHashMap::Entry
//==========================================================
CWeakHashMap::Entry::Entry(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* object)
{
    if (IWeakReferenceSource::Probe(key)) {
        IWeakReferenceSource::Probe(key)->GetWeakReference((IWeakReference**)&mKey);
    }
    mIsNull = key == NULL;
    mHash = mIsNull ? 0 : Object::GetHashCode(key);
    mValue = object;
}

CAR_INTERFACE_IMPL(CWeakHashMap::Entry, Object, IMapEntry)

ECode CWeakHashMap::Entry::GetKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key);

    return mKey->Resolve(EIID_IInterface, key);
}

ECode CWeakHashMap::Entry::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CWeakHashMap::Entry::SetValue(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** oldObject)
{
    VALIDATE_NOT_NULL(oldObject);

    AutoPtr<IInterface> result = mValue;
    mValue = object;
    *oldObject = result;
    REFCOUNT_ADD(*oldObject)
    return NOERROR;
}

ECode CWeakHashMap::Entry::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!IMapEntry::Probe(other)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IMapEntry> entry = IMapEntry::Probe(other);
    AutoPtr<IInterface> key;
    mKey->Resolve(EIID_IInterface, (IInterface**)&key);
    AutoPtr<IInterface> entryKey;
    entry->GetKey((IInterface**)&entryKey);
    AutoPtr<IInterface> entryValue;
    entry->GetValue((IInterface**)&entryValue);
    *result = (key == NULL ? entryKey == NULL : Object::Equals(key, entryKey))
            && (mValue == NULL ? entryValue == NULL : Object::Equals(mValue, entryValue));
    return NOERROR;
}

ECode CWeakHashMap::Entry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    *hashCode = mHash + (mValue == NULL ? 0 : Object::GetHashCode(mValue));
    return NOERROR;
}

ECode CWeakHashMap::Entry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IInterface> key;
    mKey->Resolve(EIID_IInterface, (IInterface**)&key);
    *str = Object::ToString(key) + String("=") + Object::ToString(mValue);
    return NOERROR;
}


//==========================================================
//       CWeakHashMap::HashIterator
//==========================================================
CAR_INTERFACE_IMPL(CWeakHashMap::HashIterator, Object, IIterator)

ECode CWeakHashMap::HashIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNextEntry != NULL && (mNextKey != NULL || mNextEntry->mIsNull)) {
        *result = TRUE;
        return NOERROR;
    }
    while (TRUE) {
        if (mNextEntry == NULL) {
            while (mPosition < mOwner->mElementData->GetLength()) {
                if ((mNextEntry = (*mOwner->mElementData)[mPosition++]) != NULL) {
                    break;
                }
            }
            if (mNextEntry == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        // ensure key of next entry is not gc'ed
        mNextKey = NULL;
        mNextEntry->GetKey((IInterface**)&mNextKey);
        if (mNextKey != NULL || mNextEntry->mIsNull) {
            *result = TRUE;
            return NOERROR;
        }
        mNextEntry = mNextEntry->mNext;
    }
}

ECode CWeakHashMap::HashIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (mExpectedModCount == mOwner->mModCount) {
        Boolean hasNext = FALSE;
        if ((HasNext(&hasNext), hasNext)) {
            mCurrentEntry = mNextEntry;
            mNextEntry = mCurrentEntry->mNext;
            mType->Get(mCurrentEntry, object);
            mNextKey = NULL;
            return NOERROR;
        }
        return  E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    return E_CONCURRENT_MODIFICATION_EXCEPTION;
}

ECode CWeakHashMap::HashIterator::Remove()
{
    if (mExpectedModCount == mOwner->mModCount) {
        if (mCurrentEntry != NULL) {
            mOwner->RemoveEntry(mCurrentEntry);
            mCurrentEntry = NULL;
            mExpectedModCount++;
            return NOERROR;
        }
        else {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
}


//==========================================================
//       CWeakHashMap::_EntrySet::Type
//==========================================================

ECode CWeakHashMap::_EntrySet::Type::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)

    *value = entry;
    REFCOUNT_ADD(*value)
    return NOERROR;
}


//==========================================================
//       CWeakHashMap::_EntrySet
//==========================================================

ECode CWeakHashMap::_EntrySet::GetSize(
    /* [out] */ Int32* size)
{
    return mOwner->GetSize(size);
}

ECode CWeakHashMap::_EntrySet::Clear()
{
    return mOwner->Clear();
}

ECode CWeakHashMap::_EntrySet::Remove(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isContains = FALSE;
    if (Contains(o, &isContains), isContains) {
        AutoPtr<IInterface> key;
        IMapEntry::Probe(o)->GetKey((IInterface**)&key);
        AutoPtr<IInterface> value;
        mOwner->Remove(key, (IInterface**)&value);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CWeakHashMap::_EntrySet::Contains(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IMapEntry::Probe(o)) {
        AutoPtr<IInterface> key;
        IMapEntry::Probe(o)->GetKey((IInterface**)&key);
        AutoPtr<Entry> entry = mOwner->GetEntry(key);
        if (entry != NULL) {
            AutoPtr<IInterface> key1;
            entry->GetKey((IInterface**)&key1);
            if (key != NULL || entry->mIsNull) {
                return entry->Equals(o, result);
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CWeakHashMap::_EntrySet::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<MapEntry::Type> type = new _EntrySet::Type();
    AutoPtr<IIterator> iterator = new HashIterator(type, mOwner);
    *result = iterator;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CWeakHashMap::_EntrySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode CWeakHashMap::_EntrySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode CWeakHashMap::_EntrySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode CWeakHashMap::_EntrySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CWeakHashMap::_EntrySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RemoveAll(collection, result);
}

ECode CWeakHashMap::_EntrySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RetainAll(collection, result);
}

ECode CWeakHashMap::_EntrySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode CWeakHashMap::_EntrySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(contents, outArray);
}

ECode CWeakHashMap::_EntrySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode CWeakHashMap::_EntrySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}


//==========================================================
//       CWeakHashMap::_KeySet::Type
//==========================================================
ECode CWeakHashMap::_KeySet::Type::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** value)
{
    return entry->GetKey(value);
}


//==========================================================
//       CWeakHashMap::_KeySet
//==========================================================

ECode CWeakHashMap::_KeySet::Contains(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsKey(o, result);
}

ECode CWeakHashMap::_KeySet::GetSize(
    /* [out] */ Int32* size)
{
    return mOwner->GetSize(size);
}

ECode CWeakHashMap::_KeySet::Clear()
{
    return mOwner->Clear();
}

ECode CWeakHashMap::_KeySet::Remove(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isContains = FALSE;
    if (mOwner->ContainsKey(o, &isContains), isContains) {
        AutoPtr<IInterface> value;
        mOwner->Remove(o, (IInterface**)&value);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CWeakHashMap::_KeySet::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<MapEntry::Type> type = new _KeySet::Type();
    AutoPtr<IIterator> iterator = new HashIterator(type, mOwner);
    *result = iterator;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CWeakHashMap::_KeySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    Int32 size = 0;
    GetSize(&size);
    AutoPtr<IArrayList> l;
    CArrayList::New(size, (IArrayList**)&l);
    AutoPtr<ICollection> coll = ICollection::Probe(l);

    AutoPtr<IIterator> iter;
    GetIterator((IIterator**)&iter);
    Boolean hasNext = FALSE;
    for (; iter->HasNext(&hasNext), hasNext;) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        Boolean result;
        coll->Add(value, &result);
    }
    return coll->ToArray(array);
}

ECode CWeakHashMap::_KeySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    Int32 size = 0;
    GetSize(&size);
    AutoPtr<IArrayList> l;
    CArrayList::New(size, (IArrayList**)&l);
    AutoPtr<ICollection> coll = ICollection::Probe(l);

    AutoPtr<IIterator> iter;
    GetIterator((IIterator**)&iter);
    Boolean hasNext = FALSE;
    for (; iter->HasNext(&hasNext), hasNext;) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        Boolean result;
        coll->Add(value, &result);
    }
    return coll->ToArray(contents, outArray);
}

ECode CWeakHashMap::_KeySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode CWeakHashMap::_KeySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode CWeakHashMap::_KeySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode CWeakHashMap::_KeySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CWeakHashMap::_KeySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RemoveAll(collection, result);
}

ECode CWeakHashMap::_KeySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RetainAll(collection, result);
}

ECode CWeakHashMap::_KeySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode CWeakHashMap::_KeySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}


//==========================================================
//       CWeakHashMap::_Values::Type
//==========================================================

ECode CWeakHashMap::_Values::Type::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** value)
{
    return entry->GetValue(value);
}


//==========================================================
//       CWeakHashMap::_Values
//==========================================================

ECode CWeakHashMap::_Values::GetSize(
    /* [out] */ Int32* size)
{
    return mOwner->GetSize(size);
}

ECode CWeakHashMap::_Values::Clear()
{
    return mOwner->Clear();
}

ECode CWeakHashMap::_Values::Contains(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return mOwner->ContainsValue(o, result);
}

ECode CWeakHashMap::_Values::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<MapEntry::Type> type = new _Values::Type();
    AutoPtr<IIterator> iterator = (IIterator*) new HashIterator(type, mOwner);
    *result = iterator;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CWeakHashMap::_Values::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

ECode CWeakHashMap::_Values::Remove(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Remove(o, result);
}

ECode CWeakHashMap::_Values::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Add(object, modified);
}

ECode CWeakHashMap::_Values::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

ECode CWeakHashMap::_Values::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

ECode CWeakHashMap::_Values::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RemoveAll(collection, result);
}

ECode CWeakHashMap::_Values::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::RetainAll(collection, result);
}

ECode CWeakHashMap::_Values::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

ECode CWeakHashMap::_Values::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(contents, outArray);
}

ECode CWeakHashMap::_Values::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Object::Equals(this, object);
    return NOERROR;
}

ECode CWeakHashMap::_Values::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = Object::GetHashCode(TO_IINTERFACE(this));
    return NOERROR;
}



//==========================================================
//       CWeakHashMap
//==========================================================
CAR_INTERFACE_IMPL(CWeakHashMap, AbstractMap, IWeakHashMap)

const Int32 CWeakHashMap::DEFAULT_SIZE;

AutoPtr< ArrayOf<CWeakHashMap::Entry*> > CWeakHashMap::NewEntryArray(
    /* [in] */ Int32 size)
{
    AutoPtr< ArrayOf<Entry*> > result = ArrayOf<Entry*>::Alloc(size);
    return result;
}

ECode CWeakHashMap::constructor()
{
    return constructor(DEFAULT_SIZE);
}

ECode CWeakHashMap::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity >= 0) {
        mElementCount = 0;
        mElementData = NewEntryArray(capacity == 0 ? 1 : capacity);
        mLoadFactor = 7500; // Default load factor of 0.75
        ComputeMaxSize();
        return NOERROR;
    }
    else {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CWeakHashMap::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Float loadFactor)
{
    if (capacity >= 0 && loadFactor > 0) {
        mElementCount = 0;
        mElementData = NewEntryArray(capacity == 0 ? 1 : capacity);
        mLoadFactor = (Int32)(loadFactor * 10000);
        ComputeMaxSize();
        return NOERROR;
    }
    else {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CWeakHashMap::constructor(
    /* [in] */ IMap* map)
{
    Int32 size;
    map->GetSize(&size);
    Int32 capacity = (size < 6 ? 11 : (size * 2));
    FAIL_RETURN(constructor(capacity))
    PutAllImpl(map);
    return NOERROR;
}

ECode CWeakHashMap::Clear()
{
    if (mElementCount > 0) {
        mElementCount = 0;
        Int32 length = mElementData->GetLength();
        for (Int32 i = 0; i < length; ++i) {
            mElementData->Set(i, NULL);
        }
        mModCount++;
    }
    return NOERROR;
}

void CWeakHashMap::ComputeMaxSize()
{
    Int32 length = mElementData->GetLength();
    mThreshold = (Int32)((Int64)length * mLoadFactor / 10000);
}

ECode CWeakHashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = GetEntry(key) != NULL;
    return NOERROR;
}

ECode CWeakHashMap::GetEntrySet(
    /* [out] */ ISet** ret)
{
    VALIDATE_NOT_NULL(ret);

    Poll();
    AutoPtr<ISet> result = (ISet*)new _EntrySet(this);
    *ret = result;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CWeakHashMap::GetKeySet(
    /* [out] */ ISet** ret)
{
    VALIDATE_NOT_NULL(ret);

    Poll();
    if (mKeySet == NULL) {
        mKeySet = (ISet*)new _KeySet(this);
    }
    *ret = mKeySet;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CWeakHashMap::GetValues(
    /* [out] */ ICollection** collection)
{
    VALIDATE_NOT_NULL(collection);

    Poll();
    if (mValuesCollection == NULL) {
        mValuesCollection = (ICollection*)new _Values(this);
    }
    *collection = mValuesCollection;
    REFCOUNT_ADD(*collection)
    return NOERROR;
}

ECode CWeakHashMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    Poll();
    if (key != NULL) {
        Int32 index = (Object::GetHashCode(key) & 0x7FFFFFFF) % mElementData->GetLength();
        AutoPtr<Entry> entry = (*mElementData)[index];
        while (entry != NULL) {
            AutoPtr<IInterface> entryKey;
            entry->GetKey((IInterface**)&entryKey);
            if (Object::Equals(key, entryKey)) {
                return entry->GetValue(value);
            }
            entry = entry->mNext;
        }
        *value = NULL;
        return NOERROR;
    }
    AutoPtr<Entry> entry = (*mElementData)[0];
    while (entry != NULL) {
        if (entry->mIsNull) {
            return entry->GetValue(value);
        }
        entry = entry->mNext;
    }
    *value = NULL;
    return NOERROR;
}

AutoPtr<CWeakHashMap::Entry> CWeakHashMap::GetEntry(
    /* [in] */ IInterface* key)
{
    Poll();
    if (key != NULL) {
        Int32 index = (Object::GetHashCode(key) & 0x7FFFFFFF) % mElementData->GetLength();
        AutoPtr<Entry> entry = (*mElementData)[index];
        while (entry != NULL) {
            AutoPtr<IInterface> entryKey;
            entry->GetKey((IInterface**)&entryKey);
            if (Object::Equals(key, entryKey)) {
                return entry;
            }
            entry = entry->mNext;
        }
        return NULL;
    }
    AutoPtr<Entry> entry = (*mElementData)[0];
    while (entry != NULL) {
        if (entry->mIsNull) {
            return entry;
        }
        entry = entry->mNext;
    }
    return NULL;
}

ECode CWeakHashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Poll();
    if (value != NULL) {
        for (Int32 i = mElementData->GetLength(); --i >= 0;) {
            AutoPtr<Entry> entry = (*mElementData)[i];
            while (entry != NULL) {
                AutoPtr<IInterface> key;
                entry->GetKey((IInterface**)&key);
                if ((key != NULL || entry->mIsNull)
                        && Object::Equals(value, entry->mValue)) {
                    *result = TRUE;
                    return NOERROR;
                }
                entry = entry->mNext;
            }
        }
    }
    else {
        for (Int32 i = mElementData->GetLength(); --i >= 0;) {
            AutoPtr<Entry> entry = (*mElementData)[i];
            while (entry != NULL) {
                AutoPtr<IInterface> key;
                entry->GetKey((IInterface**)&key);
                if ((key != NULL || entry->mIsNull) && entry->mValue == NULL) {
                    *result = TRUE;
                    return NOERROR;
                }
                entry = entry->mNext;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CWeakHashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    GetSize(&size);
    *result = size == 0;
    return NOERROR;
}

void CWeakHashMap::Poll()
{
    for (Int32 i = 0; i < mElementData->GetLength(); ++i) {
        AutoPtr<Entry> entry, last;
        entry = (*mElementData)[i];
        last = entry;
        while (entry != NULL) {
            AutoPtr<IInterface> key;
            entry->GetKey((IInterface**)&key);
            if (key == NULL) {
                mModCount++;
                if (last == entry) {
                    mElementData->Set(i, entry->mNext);
                    entry = entry->mNext;
                    last = entry;
                }
                else {
                    last->mNext = entry->mNext;
                    entry = entry->mNext;
                }
                mElementCount--;
            }
            else {
                last = entry;
                entry = entry->mNext;
            }
        }
    }
}

void CWeakHashMap::RemoveEntry(
    /* [in] */ Entry* toRemove)
{
    AutoPtr<Entry> entry;
    AutoPtr<Entry> last;
    Int32 hashCode;
    toRemove->GetHashCode(&hashCode);
    Int32 index = (hashCode & 0x7FFFFFFF) % mElementData->GetLength();
    entry = (*mElementData)[index];
    // Ignore queued entries which cannot be found, the user could
    // have removed them before they were queued, i.e. using clear()
    while (entry != NULL) {
        if (toRemove == entry) {
            mModCount++;
            if (last == NULL) {
                mElementData->Set(index, entry->mNext);
            }
            else {
                last->mNext = entry->mNext;
            }
            mElementCount--;
            break;
        }
        last = entry;
        entry = entry->mNext;
    }
}

ECode CWeakHashMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    Poll();
    Int32 index = 0;
    AutoPtr<Entry> entry;
    if (key != NULL) {
        index = (Object::GetHashCode(key) & 0x7FFFFFFF) % mElementData->GetLength();
        entry = (*mElementData)[index];
        AutoPtr<IInterface> entryKey;
        while (entry != NULL &&
                !Object::Equals(key, (entryKey = NULL, entry->GetKey((IInterface**)&entryKey), entryKey))) {
            entry = entry->mNext;
        }
    }
    else {
        entry = (*mElementData)[0];
        while (entry != NULL && !entry->mIsNull) {
            entry = entry->mNext;
        }
    }
    if (entry == NULL) {
        mModCount++;
        if (++mElementCount > mThreshold) {
            Rehash();
            index = (key == NULL ? 0 : (Object::GetHashCode(key) & 0x7FFFFFFF)) % mElementData->GetLength();
        }
        entry = new Entry(key, value);
        entry->mNext = (*mElementData)[index];
        mElementData->Set(index, entry);

        if (oldValue) {
            *oldValue = NULL;
        }
        return NOERROR;
    }

    AutoPtr<IInterface> result = entry->mValue;
    entry->mValue = value;
    if (oldValue) {
        *oldValue = result;
        REFCOUNT_ADD(*oldValue)
    }
    return NOERROR;
}

ECode CWeakHashMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<IInterface> obj;
    return Put(key, value, (IInterface**)&obj);
}

void CWeakHashMap::Rehash()
{
    Int32 length = mElementData->GetLength() * 2;
    if (length == 0) {
        length = 1;
    }
    AutoPtr<ArrayOf<Entry*> > newData = NewEntryArray(length);
    for (Int32 i = 0; i < mElementData->GetLength(); ++i) {
      AutoPtr<Entry> entry = (*mElementData)[i];
      while (entry != NULL) {
        Int32 index = entry->mIsNull ? 0 : (entry->mHash & 0x7FFFFFFF) % length;
        AutoPtr<Entry> next = entry->mNext;
        entry->mNext = (*newData)[index];
        newData->Set(index, entry);
        entry = next;
      }
    }
    mElementData = newData;
    ComputeMaxSize();
}

ECode CWeakHashMap::PutAll(
    /* [in] */ IMap* map)
{
    VALIDATE_NOT_NULL(map);
    PutAllImpl(map);
    return NOERROR;
}

ECode CWeakHashMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    Poll();
    Int32 index = 0;
    AutoPtr<Entry> entry;
    AutoPtr<Entry> last;
    if (key != NULL) {
        index = (Object::GetHashCode(key) & 0x7FFFFFFF) % mElementData->GetLength();
        entry = (*mElementData)[index];
        AutoPtr<IInterface> entryKey;
        while (entry != NULL &&
                !Object::Equals(key, (entryKey = NULL, entry->GetKey((IInterface**)&entryKey), entryKey))) {
            last = entry;
            entry = entry->mNext;
        }
    }
    else {
        entry = (*mElementData)[0];
        while (entry != NULL && !entry->mIsNull) {
            last = entry;
            entry = entry->mNext;
        }
    }
    if (entry != NULL) {
        mModCount++;
        if (last == NULL) {
            mElementData->Set(index, entry->mNext);
        }
        else {
            last->mNext = entry->mNext;
        }
        mElementCount--;
        *value = entry->mValue;
        REFCOUNT_ADD(*value)
        return NOERROR;
    }
    *value = NULL;
    return NOERROR;
}

ECode CWeakHashMap::Remove(
    /* [in] */ IInterface* key)
{
    AutoPtr<IInterface> obj;
    return Remove(key, (IInterface**)&obj);
}

ECode CWeakHashMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    Poll();
    *size = mElementCount;
    return NOERROR;
}

void CWeakHashMap::PutAllImpl(
    /* [in] */ IMap* map)
{
    AutoPtr<ISet> entry;
    map->GetEntrySet((ISet**)&entry);
    if (entry != NULL) {
        AbstractMap::PutAll(map);
    }
}

ECode CWeakHashMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return AbstractMap::Equals(object, result);
}

ECode CWeakHashMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return AbstractMap::GetHashCode(hashCode);
}

} // namespace Utility
} // namespace Elastos
