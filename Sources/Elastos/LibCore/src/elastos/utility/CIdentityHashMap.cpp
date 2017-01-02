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

#include "Elastos.CoreLibrary.IO.h"
#include "CIdentityHashMap.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::EIID_ICloneable;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

class NullObject
    : public Object
{
};

//==========================================================
//       CIdentityHashMap
//==========================================================
const Int64 CIdentityHashMap::sSerialVersionUID;

const Int32 CIdentityHashMap::DEFAULT_MAX_SIZE;

/* Default load factor of 0.75; */
const Int32 CIdentityHashMap::sLoadFactor;

const AutoPtr<IInterface> CIdentityHashMap::NULL_OBJECT = (IObject*)new NullObject;  //$NON-LOCK-1$

CAR_OBJECT_IMPL(CIdentityHashMap)

CAR_INTERFACE_IMPL_3(CIdentityHashMap, AbstractMap, IIdentityHashMap, ISerializable, ICloneable)

CIdentityHashMap::CIdentityHashMap()
    : mSize(0)
    , mThreshold(0)
    , mModCount(0)
{
}

ECode CIdentityHashMap::constructor()
{
    return constructor(DEFAULT_MAX_SIZE);
}

ECode CIdentityHashMap::constructor(
    /* [in] */ Int32 maxSize)
{
    if (maxSize >= 0) {
        mSize = 0;
        mThreshold = GetThreshold(maxSize);
        mElementData = NewElementArray(ComputeElementArraySize());
    }
    else {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CIdentityHashMap::constructor(
    /* [in] */ IMap* map)
{
    Int32 maplen = 0;
    map->GetSize(&maplen);
    FAIL_RETURN(constructor(maplen < 6 ? 11 : maplen * 2));
    return PutAllImpl(map);
}

Int32 CIdentityHashMap::GetThreshold(
    /* [in] */ Int32 maxSize)
{
    // assign the mThreshold to maxSize initially, this will change to a
    // higher value if rehashing occurs.
    return maxSize > 3 ? maxSize : 3;
}

Int32 CIdentityHashMap::ComputeElementArraySize()
{
    Int32 arraySize = (Int32) (((Int64) mThreshold * 10000) / sLoadFactor) * 2;
    // ensure arraySize is positive, the above cast from long to int type
    // leads to overflow and negative arraySize if mThreshold is too big
    return arraySize < 0 ? -arraySize : arraySize;
}

AutoPtr< ArrayOf<IInterface*> > CIdentityHashMap::NewElementArray(
    /* [in] */ Int32 s)
{
    AutoPtr< ArrayOf<IInterface*> > res = ArrayOf<IInterface*>::Alloc(s);
    return res;
}

AutoPtr<IInterface> CIdentityHashMap::MassageValue(
    /* [in] */ IInterface* value)
{
    AutoPtr<IInterface> res = ((value == NULL_OBJECT) ? NULL : value);
    return res;
}

ECode CIdentityHashMap::Clear()
{
    mSize = 0;
    for (Int32 i = 0; i < mElementData->GetLength(); i++) {
        mElementData->Set(i, NULL);
    }
    mModCount++;
    return NOERROR;
}

ECode CIdentityHashMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (key == NULL) {
        key = NULL_OBJECT;
    }

    Int32 index = FindIndex(key, mElementData);
    *result = (*mElementData)[index] == key;
    return NOERROR;
}

ECode CIdentityHashMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (value == NULL) {
        value = NULL_OBJECT;
    }

    for (Int32 i = 1; i < mElementData->GetLength(); i = i + 2) {
        if ((*mElementData)[i] == value) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    if (key == NULL) {
        key = NULL_OBJECT;
    }

    Int32 index = FindIndex(key, mElementData);

    if ((*mElementData)[index] == key) {
        AutoPtr<IInterface> result = (*mElementData)[index + 1];
        *value = MassageValue(result);
        REFCOUNT_ADD(*value)
        return NOERROR;
    }

    *value = NULL;
    return NOERROR;
}

AutoPtr<CIdentityHashMap::IdentityHashMapEntry> CIdentityHashMap::GetEntry(
    /* [in] */ IInterface* key)
{
    if (key == NULL) {
        key = NULL_OBJECT;
    }

    Int32 index = FindIndex(key, mElementData);
    if ((*mElementData)[index] == key) {
        return GetEntry(index);
    }

    return NULL;
}

AutoPtr<CIdentityHashMap::IdentityHashMapEntry> CIdentityHashMap::GetEntry(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> key = (*mElementData)[index];
    AutoPtr<IInterface> value = (*mElementData)[index + 1];

    if (key == NULL_OBJECT) {
        key = NULL;
    }
    if (value == NULL_OBJECT) {
        value = NULL;
    }

    AutoPtr<IdentityHashMapEntry> res = new IdentityHashMapEntry(this, key, value);
    return res;
}

Int32 CIdentityHashMap::FindIndex(
    /* [in] */ IInterface* key,
    /* [in] */ ArrayOf<IInterface*>* array)
{
    Int32 length = array->GetLength();
    Int32 index = GetModuloHash(key, length);
    Int32 last = (index + length - 2) % length;
    while (index != last) {
        if ((*array)[index] == key || ((*array)[index] == NULL)) {
            /*
             * Found the key, or the next empty spot (which means key is not
             * in the table)
             */
            break;
        }
        index = (index + 2) % length;
    }
    return index;
}

Int32 CIdentityHashMap::GetModuloHash(
    /* [in] */ IInterface* key,
    /* [in] */ Int32 length)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int32 hash;
    system->IdentityHashCode(key, &hash);
    return ((hash & 0x7FFFFFFF) % (length / 2)) * 2;
}

ECode CIdentityHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    AutoPtr<IInterface> _key = key;
    AutoPtr<IInterface> _value = value;
    if (_key == NULL) {
        _key = NULL_OBJECT;
    }

    if (_value == NULL) {
        _value = NULL_OBJECT;
    }

    Int32 index = FindIndex(_key, mElementData);

    // if the key doesn't exist in the table
    if ((*mElementData)[index] != _key) {
        mModCount++;
        if (++mSize > mThreshold) {
            Rehash();
            index = FindIndex(_key, mElementData);
        }

        // insert the key and assign the value to null initially
        mElementData->Set(index, _key);
        mElementData->Set(index + 1, NULL);
    }

    // insert value to where it needs to go, return the old value
    AutoPtr<IInterface> result = (*mElementData)[index + 1];
    mElementData->Set(index + 1, _value);

    if (oldValue) {
        AutoPtr<IInterface> res = MassageValue(result);
        *oldValue = res;
        REFCOUNT_ADD(*oldValue)
    }
    else {
        MassageValue(result);
    }

    return NOERROR;
}

ECode CIdentityHashMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    AutoPtr<IInterface> oldValue;
    return Put(key, value, (IInterface**)&oldValue);
}

ECode CIdentityHashMap::PutAll(
    /* [in] */ IMap* map)
{
    return PutAllImpl(map);
}

ECode CIdentityHashMap::Rehash()
{
    Int32 newlength = mElementData->GetLength() * 2;
    if (newlength == 0) {
        newlength = 1;
    }
    AutoPtr< ArrayOf<IInterface*> > newData = NewElementArray(newlength);
    for (Int32 i = 0; i < mElementData->GetLength(); i = i + 2) {
        AutoPtr<IInterface> key = (*mElementData)[i];
        if (key != NULL) {
            // if not empty
            Int32 index = FindIndex(key, newData);
            newData->Set(index, key);
            AutoPtr<IInterface> res = (*mElementData)[i + 1];
            newData->Set(index + 1, res);
        }
    }
    mElementData = newData;
    return ComputeMaxSize();
}

ECode CIdentityHashMap::ComputeMaxSize()
{
    mThreshold = (Int32) ((Int64) (mElementData->GetLength() / 2) * sLoadFactor / 10000);
    return NOERROR;
}

ECode CIdentityHashMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)

    if (key == NULL) {
        key = NULL_OBJECT;
    }

    Boolean hashedOk = FALSE;
    Int32 index, next, hash;
    AutoPtr<IInterface> result;
    AutoPtr<IInterface> object;
    index = next = FindIndex(key, mElementData);

    if ((*mElementData)[index] != key) {
        *value = NULL;
        return NOERROR;
    }

    // store the value for this key
    result = (*mElementData)[index + 1];

    // shift the following elements up if needed
    // until we reach an empty spot
    Int32 length = mElementData->GetLength();
    while (TRUE) {
        next = (next + 2) % length;
        object = (*mElementData)[next];
        if (object == NULL) {
            break;
        }

        hash = GetModuloHash(object, length);
        hashedOk = hash > index;
        if (next < index) {
            hashedOk = hashedOk || (hash <= next);
        }
        else {
            hashedOk = hashedOk && (hash <= next);
        }
        if (!hashedOk) {
            mElementData->Set(index, object);
            AutoPtr<IInterface> outface = (*mElementData)[next + 1];
            mElementData->Set(index + 1, outface);
            index = next;
        }
    }

    mSize--;
    mModCount++;

    // clear both the key and the value
    mElementData->Set(index, NULL);
    mElementData->Set(index + 1, NULL);

    AutoPtr<IInterface> res = MassageValue(result);
    *value = res;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CIdentityHashMap::Remove(
    /* [in] */ PInterface key)
{
    AutoPtr<IInterface> value;
    return Remove(key, (IInterface**)&value);
}

ECode CIdentityHashMap::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries)

    AutoPtr<ISet> outent = (ISet*) new IdentityHashMapEntrySet(this);
    *entries = outent;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode CIdentityHashMap::GetKeySet(
    /* [out] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet)

    if (mKeySet == NULL) {
        mKeySet = (ISet*)new IdentityHashMapKeySet(this);
    }
    *keySet = mKeySet;
    REFCOUNT_ADD(*keySet);
    return NOERROR;
}

ECode CIdentityHashMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value)

    if (mValuesCollection == NULL) {
        mValuesCollection = (ICollection*) new IdentityHashMapValues(this);
    }

    *value = mValuesCollection;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CIdentityHashMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * We need to override the equals method in AbstractMap because
     * AbstractMap.equals will call ((Map) object).entrySet().contains() to
     * determine equality of the entries, so it will defer to the argument
     * for comparison, meaning that reference-based comparison will not take
     * place. We must ensure that all comparison is implemented by methods
     * in this class (or in one of our inner classes) for reference-based
     * comparison to take place.
     */
    if (TO_IINTERFACE(this) == object) {
        *result = TRUE;
        return NOERROR;
    }
    if (IMap::Probe(object)) {
        AutoPtr<IMap> map = (IMap*) object->Probe(EIID_IMap);
        Int32 len1 = 0;
        Int32 len2 = 0;
        GetSize(&len1);
        map->GetSize(&len2);
        if (len1 != len2) {
            *result = FALSE;
            return NOERROR;
        }

        AutoPtr<ISet> set;
        GetEntrySet((ISet**)&set);
        // ensure we use the equals method of the set created by "this"
        AutoPtr<ISet> mapset;
        map->GetEntrySet((ISet**)&mapset);
        return (ICollection::Probe(set))->Equals(mapset, result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    // try {
    AutoPtr<CIdentityHashMap> cloneHashMap;
    FAIL_RETURN(CIdentityHashMap::NewByFriend((CIdentityHashMap**)&cloneHashMap));
    cloneHashMap->mElementData = NewElementArray(mElementData->GetLength());
    // System.arraycopy(mElementData, 0, cloneHashMap.mElementData, 0, mElementData->GetLength());
    cloneHashMap->mElementData->Copy(0, mElementData, 0, mElementData->GetLength());
    *object = cloneHashMap->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }
    return NOERROR;
}

ECode CIdentityHashMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSize == 0;
    return NOERROR;
}

ECode CIdentityHashMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = mSize;
    return NOERROR;
}

ECode CIdentityHashMap::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();
    (IOutputStream::Probe(stream))->Write(mSize);
    AutoPtr<ISet> entries;
    GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> iterator;
    (IIterable::Probe(entries))->GetIterator((IIterator**)&iterator);
    Boolean isflag = FALSE;
    while (iterator->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        iterator->GetNext((IInterface**)&outface);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        assert(0 && "TODO");
        // stream.writeObject(entry.key);
        // stream.writeObject(entry.value);
    }
    return E_NOT_IMPLEMENTED;
}

ECode CIdentityHashMap::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();
    Int32 savedSize = 0;
    (IInputStream::Probe(stream))->Read(&savedSize);
    mThreshold = GetThreshold(DEFAULT_MAX_SIZE);
    mElementData = NewElementArray(ComputeElementArraySize());
    for (Int32 i = savedSize; --i >= 0;) {
        AutoPtr<IInterface> key;
        assert(0 && "TODO");
        // stream->ReadObject((IInterface**)&key);
        // Put(key, (V) stream.readObject());
    }
    mSize = savedSize;
    return E_NOT_IMPLEMENTED;
}

ECode CIdentityHashMap::PutAllImpl(
    /* [in] */ IMap* map)
{
    AutoPtr<ISet> entries;
    map->GetEntrySet((ISet**)&entries);
    if (entries != NULL) {
        AbstractMap::PutAll(map);
    }
    return NOERROR;
}

ECode CIdentityHashMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractMap::GetHashCode(hashCode);
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapEntry
//==========================================================
CIdentityHashMap::IdentityHashMapEntry::IdentityHashMapEntry(
    /* [in] */ CIdentityHashMap* map,
    /* [in] */ IInterface* theKey,
    /* [in] */ IInterface* theValue)
    : MapEntry(theKey, theValue)
    , mMap(map)
{}

ECode CIdentityHashMap::IdentityHashMapEntry::Clone(
    /* [out] */ IInterface** outface)
{
    return MapEntry::Clone(outface);
}

ECode CIdentityHashMap::IdentityHashMapEntry::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (TO_IINTERFACE(this) == object) {
        *value = TRUE;
        return NOERROR;
    }
    if (IMapEntry::Probe(object)) {
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(object);
        AutoPtr<IInterface> keyface;
        AutoPtr<IInterface> valueface;
        entry->GetKey((IInterface**)&keyface);
        entry->GetValue((IInterface**)&valueface);
        *value = (mKey == keyface) && (mValue == valueface);
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntry::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int32 key, valuehash;
    system->IdentityHashCode(TO_IINTERFACE(this), &key);
    system->IdentityHashCode(TO_IINTERFACE(this), &valuehash);

    *value = key ^ valuehash;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = Object::ToString(mKey) + String("=") + Object::ToString(mValue);
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntry::SetValue(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IInterface> result;
    FAIL_RETURN(MapEntry::SetValue(object, (IInterface**)&result));
    AutoPtr<IInterface> outface1;
    FAIL_RETURN(mMap->Put(mKey, object, (IInterface**)&outface1));
    *outface = result;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapIterator
//==========================================================
CAR_INTERFACE_IMPL(CIdentityHashMap::IdentityHashMapIterator, Object, IIterator)

CIdentityHashMap::IdentityHashMapIterator::IdentityHashMapIterator(
    /* [in] */ MapEntry::Type* value,
    /* [in] */ CIdentityHashMap* hm)
    : mAssociatedMap(hm)
    , mExpectedModCount(hm->mModCount)
    , mType(value)
    , mCanRemove(FALSE)
    , mPosition(0)
    , mLastPosition(0)
{}

ECode CIdentityHashMap::IdentityHashMapIterator::HasNext(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    while (mPosition < mAssociatedMap->mElementData->GetLength()) {
        // if this is an empty spot, go to the next one
        if ((*mAssociatedMap->mElementData)[mPosition] == NULL) {
            mPosition += 2;
        }
        else {
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapIterator::CheckConcurrentMod()
{
    if (mExpectedModCount != mAssociatedMap->mModCount) {
        // throw new ConcurrentModificationException();
        return E_CONCURRENT_MODIFICATION_EXCEPTION;
    }
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapIterator::GetNext(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    FAIL_RETURN(CheckConcurrentMod());
    Boolean isflag = FALSE;
    if (HasNext(&isflag), !isflag) {
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    AutoPtr<IdentityHashMapEntry> result = mAssociatedMap->GetEntry(mPosition);
    mLastPosition = mPosition;
    mPosition += 2;

    mCanRemove = TRUE;
    AutoPtr<IInterface> res;
    mType->Get(result.Get(), (IInterface**)&res);
    *outface = res;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapIterator::Remove()
{
    FAIL_RETURN(CheckConcurrentMod());
    if (!mCanRemove) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mCanRemove = FALSE;
    AutoPtr<IInterface> oldres;
    mAssociatedMap->Remove((*mAssociatedMap->mElementData)[mLastPosition], (IInterface**)&oldres);
    mPosition = mLastPosition;
    mExpectedModCount++;
    return NOERROR;
}


//==========================================================
//       CIdentityHashMap::IdentityHashMapEntrySet
//==========================================================

CIdentityHashMap::IdentityHashMapEntrySet::IdentityHashMapEntrySet(
    /* [in] */ CIdentityHashMap* hm)
{
    mAssociatedMap = hm;
}

AutoPtr<CIdentityHashMap> CIdentityHashMap::IdentityHashMapEntrySet::GetHashMap()
{
    return mAssociatedMap;
}

ECode CIdentityHashMap::IdentityHashMapEntrySet::GetSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mAssociatedMap->mSize;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntrySet::Clear()
{
    return mAssociatedMap->Clear();
}

ECode CIdentityHashMap::IdentityHashMapEntrySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    if (Contains(object, &isflag), isflag) {
        AutoPtr<IInterface> oldres;
        AutoPtr<IMapEntry> mapres = IMapEntry::Probe(object);
        AutoPtr<IInterface> keyface;
        if (mapres != NULL) {
            mapres->GetKey((IInterface**)&keyface);
        }
        mAssociatedMap->Remove(keyface, (IInterface**)&oldres);
        *value = TRUE;
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntrySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (IMapEntry::Probe(object)) {
        AutoPtr<IMapEntry> res = IMapEntry::Probe(object);
        AutoPtr<IInterface> keyface;
        res->GetKey((IInterface**)&keyface);

        AutoPtr<IdentityHashMapEntry> entry = mAssociatedMap->GetEntry(keyface);
        // we must call equals on the entry obtained from "this"
        Boolean isflag = FALSE;
        *value = entry != NULL && (entry->Equals(object, &isflag), isflag);
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapEntrySet::GetIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<MapEntry::Type> restype = new IdentityHashMapMapEntryType();
    AutoPtr<IIterator> resiter = (IIterator*) new IdentityHashMapIterator(restype, mAssociatedMap);
    *outiter = resiter;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapKeySet
//==========================================================

CIdentityHashMap::IdentityHashMapKeySet::IdentityHashMapKeySet(
    /* [in] */ CIdentityHashMap* hm)
{
    mAssociatedMap = hm;
}

ECode CIdentityHashMap::IdentityHashMapKeySet::GetSize(
    /* [out] */ Int32* value)
{
    return mAssociatedMap->GetSize(value);
}

ECode CIdentityHashMap::IdentityHashMapKeySet::Clear()
{
    return mAssociatedMap->Clear();
}

ECode CIdentityHashMap::IdentityHashMapKeySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    if (mAssociatedMap->ContainsKey(object, &isflag), isflag) {
        AutoPtr<IInterface> outface;
        mAssociatedMap->Remove(object, (IInterface**)&outface);
        *value = TRUE;
    }
    else {
        *value = FALSE;
    }
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapKeySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return mAssociatedMap->ContainsKey(object, value);
}

ECode CIdentityHashMap::IdentityHashMapKeySet::GetIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<MapEntry::Type> restype = new IdentityHashMapKeySetMapEntryType();
    AutoPtr<IIterator> resiter = (IIterator*) new IdentityHashMapIterator(restype, mAssociatedMap);
    *outiter = resiter;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapValues
//==========================================================

CIdentityHashMap::IdentityHashMapValues::IdentityHashMapValues(
    /* [in] */ CIdentityHashMap* hm)
{
    mAssociatedMap = hm;
}

ECode CIdentityHashMap::IdentityHashMapValues::GetSize(
    /* [out] */ Int32* value)
{
    return mAssociatedMap->GetSize(value);
}

ECode CIdentityHashMap::IdentityHashMapValues::Clear()
{
    return mAssociatedMap->Clear();
}

ECode CIdentityHashMap::IdentityHashMapValues::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    while (it->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        if (object == outface) {
            it->Remove();
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CIdentityHashMap::IdentityHashMapValues::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return mAssociatedMap->ContainsValue(object, value);
}

ECode CIdentityHashMap::IdentityHashMapValues::GetIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<MapEntry::Type> restype = new IdentityHashMapValuesMapEntryType();
    AutoPtr<IIterator> resiter = (IIterator*) new IdentityHashMapIterator(restype, mAssociatedMap);
    *outiter = resiter;
    REFCOUNT_ADD(*outiter)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapMapEntryType
//==========================================================

ECode CIdentityHashMap::IdentityHashMapMapEntryType::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = entry;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapKeySetMapEntryType
//==========================================================

ECode CIdentityHashMap::IdentityHashMapKeySetMapEntryType::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IInterface> keyface;
    FAIL_RETURN(entry->GetKey((IInterface**)&keyface));
    *outface = keyface;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

//==========================================================
//       CIdentityHashMap::IdentityHashMapValuesMapEntryType
//==========================================================

ECode CIdentityHashMap::IdentityHashMapValuesMapEntryType::Get(
    /* [in] */ IMapEntry* entry,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IInterface> valueface;
    FAIL_RETURN(entry->GetValue((IInterface**)&valueface));
    *outface = valueface;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
