
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/utility/MapCollections.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::EIID_ICollection;
using Elastos::Utility::EIID_ISet;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::EIID_IMapEntry;

namespace Elastos {
namespace Droid {
namespace Utility {


//=================================================================
// MapCollections::ArrayIterator
//=================================================================
CAR_INTERFACE_IMPL(MapCollections::ArrayIterator, Object, IIterator)

MapCollections::ArrayIterator::ArrayIterator(
    /* [in] */ Int32 offset,
    /* [in] */ MapCollections* mc)
    : mOffset(offset)
    , mSize(0)
    , mIndex(0)
    , mCanRemove(FALSE)
    , mHost(mc)
{
    mSize = mHost->ColGetSize();
}

ECode MapCollections::ArrayIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex < mSize;
    return NOERROR;
}

ECode MapCollections::ArrayIterator::GetNext(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IInterface> res = mHost->ColGetEntry(mIndex, mOffset);
    mIndex++;
    mCanRemove = TRUE;
    *obj = res;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode MapCollections::ArrayIterator::Remove()
{
    if (!mCanRemove) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mIndex--;
    mSize--;
    mCanRemove = FALSE;
    mHost->ColRemoveAt(mIndex);
    return NOERROR;
}

//=================================================================
// MapCollections::MapIterator
//=================================================================

CAR_INTERFACE_IMPL_2(MapCollections::MapIterator, Object, IIterator, IMapEntry)

MapCollections::MapIterator::MapIterator(
    /* [in] */ MapCollections* mc)
    : mIndex(-1)
    , mEntryValid(FALSE)
    , mHost(mc)
{
    mEnd = mHost->ColGetSize() - 1;
}

ECode MapCollections::MapIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex < mEnd;
    return NOERROR;
}

ECode MapCollections::MapIterator::GetNext(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    mIndex++;
    mEntryValid = true;
    *obj = (IIterator*)this;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode MapCollections::MapIterator::Remove()
{
    if (!mEntryValid) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mHost->ColRemoveAt(mIndex);
    mIndex--;
    mEnd--;
    mEntryValid = FALSE;
    return NOERROR;
}

ECode MapCollections::MapIterator::GetKey(
    /* [out] */ IInterface** key)
{
    VALIDATE_NOT_NULL(key)
    *key = NULL;

    if (!mEntryValid) {
        // throw new IllegalStateException(
        Logger::E("MapCollections::MapIterator::GetKey", "IllegalStateException: This container does not support retaining Map.Entry objects");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInterface> entry = mHost->ColGetEntry(mIndex, 0);
    *key = entry;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode MapCollections::MapIterator::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    if (!mEntryValid) {
        // throw new IllegalStateException(
        Logger::E("MapCollections::MapIterator::GetKey", "IllegalStateException: This container does not support retaining Map.Entry objects");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInterface> entry = mHost->ColGetEntry(mIndex, 1);
    *value = entry;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode MapCollections::MapIterator::SetValue(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** oldObject)
{
    VALIDATE_NOT_NULL(oldObject)
    *oldObject = NULL;

    if (!mEntryValid) {
        // throw new IllegalStateException(
        Logger::E("MapCollections::MapIterator::GetKey", "IllegalStateException: This container does not support retaining Map.Entry objects");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInterface> old = mHost->ColSetValue(mIndex, object);
    *oldObject = old;
    REFCOUNT_ADD(*oldObject)
    return NOERROR;
}


ECode MapCollections::MapIterator::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mEntryValid) {
        // throw new IllegalStateException(
        Logger::E("MapCollections::MapIterator::GetKey", "IllegalStateException: This container does not support retaining Map.Entry objects");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (IMapEntry::Probe(o) == NULL) {
        return NOERROR;
    }

    IMapEntry* e = IMapEntry::Probe(o);
    AutoPtr<IInterface> key, value;
    e->GetKey((IInterface**)&key);
    e->GetValue((IInterface**)&value);

    AutoPtr<IInterface> k, v;
    k = mHost->ColGetEntry(mIndex, 0);
    v = mHost->ColGetEntry(mIndex, 1);
    *result = Objects::Equals(key, k) && Objects::Equals(value, v);
    return NOERROR;
}


ECode MapCollections::MapIterator::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = 0;

    if (!mEntryValid) {
        // throw new IllegalStateException(
        Logger::E("MapCollections::MapIterator::GetKey", "IllegalStateException: This container does not support retaining Map.Entry objects");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInterface> k, v;
    k = mHost->ColGetEntry(mIndex, 0);
    v = mHost->ColGetEntry(mIndex, 1);
    *hash = Object::GetHashCode(k) ^ Object::GetHashCode(v);
    return NOERROR;
}

ECode MapCollections::MapIterator::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<IInterface> k, v;
    k = mHost->ColGetEntry(mIndex, 0);
    v = mHost->ColGetEntry(mIndex, 1);

    StringBuilder sb;
    sb.Append(Object::ToString(k));
    sb.Append("=");
    sb.Append(Object::ToString(v));
    *str = sb.ToString();
    return NOERROR;
}

//=================================================================
// MapCollections::EntrySet
//=================================================================
CAR_INTERFACE_IMPL_3(MapCollections::EntrySet, Object, ISet, ICollection, IIterable)

ECode MapCollections::EntrySet::Add(
    /* [in] */ IMapEntry* obj,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::Add(
    /* [in] */ IInterface* obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::AddAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return AddAll(collection, &result);
}

ECode MapCollections::EntrySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 oldSize = mHost->ColGetSize();
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hashNext;
    IMapEntry* me;
    while (it->HasNext(&hashNext), hashNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        me = IMapEntry::Probe(obj);
        AutoPtr<IInterface> key, value;
        me->GetKey((IInterface**)&key);
        me->GetValue((IInterface**)&value);
        mHost->ColPut(key, value);
    }

    *result = oldSize != mHost->ColGetSize();
    return NOERROR;
}

ECode MapCollections::EntrySet::Clear()
{
    mHost->ColClear();
    return NOERROR;
}

ECode MapCollections::EntrySet::Contains(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IMapEntry* e = IMapEntry::Probe(o);
    if (e == NULL)
        return NOERROR;

    AutoPtr<IInterface> key;
    e->GetKey((IInterface**)&key);
    Int32 index = mHost->ColIndexOfKey(key);
    if (index < 0) {
        return NOERROR;
    }

    AutoPtr<IInterface> foundVal = mHost->ColGetEntry(index, 1);
    AutoPtr<IInterface> value;
    e->GetValue((IInterface**)&value);
    *result = Objects::Equals(foundVal, value);
    return NOERROR;
}

ECode MapCollections::EntrySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hashNext, contains;
    while (it->HasNext(&hashNext), hashNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (Contains(obj, &contains), !contains) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode MapCollections::EntrySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->ColGetSize() == 0;
    return NOERROR;
}

ECode MapCollections::EntrySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new MapIterator(mHost);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode MapCollections::EntrySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::RemoveAll(
    /* [in] */ ICollection* object,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::RetainAll(
    /* [in] */ ICollection* object,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::Remove(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::RemoveAll(
    /* [in] */ ICollection* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::RetainAll(
    /* [in] */ ICollection* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mHost->ColGetSize();
    return NOERROR;
}

ECode MapCollections::EntrySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::EntrySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->EqualsSetHelper(this, object);
    return NOERROR;
}

ECode MapCollections::EntrySet::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    Int32 result = 0;
    for (Int32 i = mHost->ColGetSize() - 1; i >= 0; i--) {
        AutoPtr<IInterface> key = mHost->ColGetEntry(i, 0);
        AutoPtr<IInterface> value = mHost->ColGetEntry(i, 1);
        result += Object::GetHashCode(key) ^ Object::GetHashCode(value);
    }
    *hash = result;
    return NOERROR;
}

//=================================================================
// MapCollections::KeySet
//=================================================================
CAR_INTERFACE_IMPL_3(MapCollections::KeySet, Object, ISet, ICollection, IIterable)

ECode MapCollections::KeySet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::KeySet::Add(
    /* [in] */ IInterface* obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::KeySet::AddAll(
    /* [in] */ ICollection* objs)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::KeySet::AddAll(
    /* [in] */ ICollection* objs,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::KeySet::Clear()
{
    mHost->ColClear();
    return NOERROR;
}

ECode MapCollections::KeySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->ColIndexOfKey(object) >= 0;
    return NOERROR;
}

ECode MapCollections::KeySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = mHost->ColGetMap();
    *result = mHost->ContainsAllHelper(map, collection);
    return NOERROR;
}

ECode MapCollections::KeySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->ColGetSize() == 0;
    return NOERROR;
}

ECode MapCollections::KeySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new ArrayIterator(0, mHost);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode MapCollections::KeySet::Remove(
    /* [in] */ IInterface* object)
{
    Boolean result;
    return Remove(object, &result);
}

ECode MapCollections::KeySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 index = mHost->ColIndexOfKey(object);
    if (index >= 0) {
        mHost->ColRemoveAt(index);
        *result = TRUE;
    }

    return NOERROR;
}

ECode MapCollections::KeySet::RemoveAll(
    /* [in] */ ICollection* object)
{
    Boolean result;
    return RemoveAll(object, &result);
}

ECode MapCollections::KeySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = mHost->ColGetMap();
    *result = mHost->RemoveAllHelper(map, collection);
    return NOERROR;
}

ECode MapCollections::KeySet::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean result;
    return RetainAll(collection, &result);
}

ECode MapCollections::KeySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = mHost->ColGetMap();
    *result = mHost->RetainAllHelper(map, collection);
    return NOERROR;
}

ECode MapCollections::KeySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mHost->ColGetSize();
    return NOERROR;
}

ECode MapCollections::KeySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    AutoPtr<ArrayOf<IInterface*> > result = mHost->ToArrayHelper( 0);
    *outArray = result;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode MapCollections::KeySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    AutoPtr<ArrayOf<IInterface*> > result = mHost->ToArrayHelper(inArray, 0);
    *outArray = result;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode MapCollections::KeySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->EqualsSetHelper(this, object);
    return NOERROR;
}

ECode MapCollections::KeySet::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    Int32 result = 0;
    for (Int32 i = mHost->ColGetSize() - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj = mHost->ColGetEntry(i, 0);
        result += Object::GetHashCode(obj);
    }
    *hash = result;
    return NOERROR;
}

//=================================================================
// MapCollections::ValuesCollection
//=================================================================
CAR_INTERFACE_IMPL_2(MapCollections::ValuesCollection, Object, ICollection, IIterable)

ECode MapCollections::ValuesCollection::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::ValuesCollection::Add(
    /* [in] */ IInterface* obj)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::ValuesCollection::AddAll(
    /* [in] */ ICollection* objs,
    /* [out] */ Boolean* result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::ValuesCollection::AddAll(
    /* [in] */ ICollection* objs)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MapCollections::ValuesCollection::Clear()
{
    mHost->ColClear();
    return NOERROR;
}

ECode MapCollections::ValuesCollection::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->ColIndexOfValue(object) >= 0;
    return NOERROR;
}

ECode MapCollections::ValuesCollection::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext, contains;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (Contains(obj, &contains), !contains) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode MapCollections::ValuesCollection::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->ColGetSize() == 0;
    return NOERROR;
}

ECode MapCollections::ValuesCollection::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new ArrayIterator(1, mHost);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode MapCollections::ValuesCollection::Remove(
    /* [in] */ IInterface* object)
{
    Boolean result;
    return Remove(object, &result);
}

ECode MapCollections::ValuesCollection::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = mHost->ColIndexOfValue(object);
    if (index >= 0) {
        mHost->ColRemoveAt(index);
        *result = TRUE;
    }
    return FALSE;
}

ECode MapCollections::ValuesCollection::RemoveAll(
    /* [in] */ ICollection* object)
{
    Boolean result;
    return RemoveAll(object, &result);
}

ECode MapCollections::ValuesCollection::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 N = mHost->ColGetSize();
    Boolean changed = FALSE, contains;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> cur = mHost->ColGetEntry(i, 1);
        collection->Contains(cur, &contains);
        if (contains) {
            mHost->ColRemoveAt(i);
            i--;
            N--;
            changed = TRUE;
        }
    }

    *result = changed;
    return NOERROR;
}

ECode MapCollections::ValuesCollection::RetainAll(
    /* [in] */ ICollection* object)
{
    Boolean result;
    return RetainAll(object, &result);
}

ECode MapCollections::ValuesCollection::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 N = mHost->ColGetSize();
    Boolean changed = FALSE, contains;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> cur = mHost->ColGetEntry(i, 1);
        collection->Contains(cur, &contains);
        if (!contains) {
            mHost->ColRemoveAt(i);
            i--;
            N--;
            changed = TRUE;
        }
    }

    *result = changed;
    return NOERROR;
}

ECode MapCollections::ValuesCollection::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mHost->ColGetSize();
    return NOERROR;
}

ECode MapCollections::ValuesCollection::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    AutoPtr<ArrayOf<IInterface*> > array = mHost->ToArrayHelper(1);
    *outArray = array;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
    //return toArrayHelper(1);
    return NOERROR;
}

ECode MapCollections::ValuesCollection::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)
    AutoPtr<ArrayOf<IInterface*> > array = mHost->ToArrayHelper(inArray, 1);
    *outArray = array;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode MapCollections::ValuesCollection::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    return Object::Equals(o, result);
}

ECode MapCollections::ValuesCollection::GetHashCode(
    /* [out] */ Int32* hash)
{
    return Object::GetHashCode(hash);
}

//=================================================================
// MapCollections
//=================================================================
Boolean MapCollections::ContainsAllHelper(
    /* [in] */ IMap* map,
    /* [in] */ ICollection* collection)
{
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext, contains;
    while (it->HasNext(&hasNext)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        map->ContainsKey(obj, &contains);
        if (!contains) {
            return FALSE;
        }
    }

    return TRUE;
}

Boolean MapCollections::RemoveAllHelper(
    /* [in] */ IMap* map,
    /* [in] */ ICollection* collection)
{
    Int32 oldSize;
    map->GetSize(&oldSize);

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        map->Remove(obj);
    }
    Int32 newSize;
    map->GetSize(&newSize);
    return oldSize != newSize;
}

Boolean MapCollections::RetainAllHelper(
    /* [in] */ IMap* map,
    /* [in] */ ICollection* collection)
{
    Int32 oldSize;
    map->GetSize(&oldSize);
    AutoPtr<ISet> keySet;
    map->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext, contains;
    while (it->HasNext(&hasNext)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        if (collection->Contains(obj, &contains), !contains) {
            it->Remove();
        }
    }
    Int32 newSize;
    map->GetSize(&newSize);
    return oldSize != newSize;
}

AutoPtr<ArrayOf<IInterface*> > MapCollections::ToArrayHelper(
    /* [in] */ Int32 offset)
{
    Int32 N = ColGetSize();
    AutoPtr<ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        result->Set(i, ColGetEntry(i, offset));
    }

    return result;
}

AutoPtr<ArrayOf<IInterface*> > MapCollections::ToArrayHelper(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [in] */ Int32 offset)
{
    AutoPtr<ArrayOf<IInterface*> > array = inArray;
    Int32 N = ColGetSize();
    if (array->GetLength() < N) {
        array = ArrayOf<IInterface*>::Alloc(N);
    }

    for (Int32 i = 0; i < N; i++) {
        array->Set(i, ColGetEntry(i, offset));
    }
    if (array->GetLength() > N) {
        array->Set(N, NULL);;
    }
    return array;
}

Boolean MapCollections::EqualsSetHelper(
    /* [in] */ ISet* set,
    /* [in] */ IInterface* object)
{
    if (TO_IINTERFACE(set) == object) {
        return TRUE;
    }

    ISet* s = ISet::Probe(object);
    if (s != NULL) {
        Int32 sSize, setSize;
        set->GetSize(&setSize);
        s->GetSize(&sSize);

        if (sSize == setSize) {
            Boolean contains;
            set->ContainsAll(ICollection::Probe(s), &contains);
            return contains;
        }
    }

    return FALSE;
}

AutoPtr<ISet> MapCollections::GetEntrySet()
{
    if (mEntrySet == NULL) {
        mEntrySet = new EntrySet(this);
    }
    return (ISet*)mEntrySet.Get();
}

AutoPtr<ISet> MapCollections::GetKeySet()
{
    if (mKeySet == NULL) {
        mKeySet = new KeySet(this);
    }
    return (ISet*)mKeySet.Get();
}

AutoPtr<ICollection> MapCollections::GetValues()
{
    if (mValues == NULL) {
        mValues = new ValuesCollection(this);
    }
    return (ICollection*)mValues.Get();
}

} // Utility
} // Droid
} // Elastos
