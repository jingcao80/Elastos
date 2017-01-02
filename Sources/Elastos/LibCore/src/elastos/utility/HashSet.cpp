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
#include "CHashSet.h"
#include "CHashMap.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL_3(HashSet, AbstractSet, IHashSet, ICloneable, ISerializable)

ECode HashSet::constructor()
{
    AutoPtr<IMap> map;
    FAIL_RETURN(CHashMap::New((IMap**)&map));
    return constructor(map);
}

ECode HashSet::constructor(
    /* [in] */ Int32 capacity)
{
    AutoPtr<IMap> map;
    FAIL_RETURN(CHashMap::New(capacity, (IMap**)&map));
    return constructor(map);
}

ECode HashSet::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Float loadFactor)
{
    AutoPtr<IMap> map;
    FAIL_RETURN(CHashMap::New(capacity, loadFactor, (IMap**)&map));
    return constructor(map);
}

ECode HashSet::constructor(
    /* [in] */ ICollection* collection)
{
    Int32 clolen = 0;
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    collection->GetSize(&clolen);
    AutoPtr<IMap> map;
    FAIL_RETURN(CHashMap::New(clolen < 6 ? 11 : clolen * 2, (IMap**)&map));
    FAIL_RETURN(constructor(map));

    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Add(obj);
    }
    return NOERROR;
}

ECode HashSet::constructor(
    /* [in] */ IMap* backingMap)
{
    mBackingMap = backingMap;
    GetWeakReference((IWeakReference**)&mWeakThis);
    return NOERROR;
}

ECode HashSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    AutoPtr<IInterface> outface;
    mBackingMap->Put(object, mWeakThis, (IInterface**)&outface);
    *modified = outface == NULL;
    return NOERROR;
}

ECode HashSet::Clear()
{
    return mBackingMap->Clear();
}

ECode HashSet::Clone(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    // try {
    // HashSet<E> clone = (HashSet<E>) super.clone();
    // clone.backingMap = (HashMap<E, HashSet<E>>) backingMap.clone();
    AutoPtr<CHashSet> clone;
    FAIL_RETURN(CHashSet::NewByFriend(mBackingMap, (CHashSet**)&clone));
    *outface = ISet::Probe(clone);
    REFCOUNT_ADD(*outface)
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }
    return NOERROR;
}

ECode HashSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return mBackingMap->ContainsKey(object, result);
}

ECode HashSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return mBackingMap->IsEmpty(result);
}

ECode HashSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<ISet> outset;
    mBackingMap->GetKeySet((ISet**)&outset);
    return (IIterable::Probe(outset))->GetIterator(it);
}

ECode HashSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr<IInterface> outface;
    mBackingMap->Remove(object, (IInterface**)&outface);
    *modified = outface != NULL;
    return NOERROR;
}

ECode HashSet::GetSize(
    /* [out] */ Int32* size)
{
    return mBackingMap->GetSize(size);
}

ECode HashSet::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();
    (IOutputStream::Probe(stream))->Write(((CHashMap*)mBackingMap.Get())->mTable->GetLength());
    assert(0 && "TODO");
    // stream->WriteFloat(CHashMap::DEFAULT_LOAD_FACTOR);
    Int32 sizelen = 0;
    GetSize(&sizelen);
    (IOutputStream::Probe(stream))->Write(sizelen);
    // for (E e : this) {
    //     stream.writeObject(e);
    // }
    return E_NOT_IMPLEMENTED;
}

ECode HashSet::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();
    Int32 length = 0;
    (IInputStream::Probe(stream))->Read(&length);
    Float loadFactor = 0;
    assert(0 && "TODO");
    // stream->ReadFloat(&loadFactor);
    mBackingMap = CreateBackingMap(length, loadFactor);
    Int32 elementCount = 0;
    (IInputStream::Probe(stream))->Read(&elementCount);
    for (Int32 i = elementCount; --i >= 0;) {
        AutoPtr<IInterface> key;
        // stream->ReadObject((IInterface**)&key);
        AutoPtr<IInterface> outface;
        mBackingMap->Put(key, mWeakThis, (IInterface**)&outface);
    }
    return E_NOT_IMPLEMENTED;
}

AutoPtr<IMap> HashSet::CreateBackingMap(
    /* [in] */ Int32 capacity,
    /* [in] */ Float loadFactor)
{
    AutoPtr<IMap> outmap;

    CHashMap::New(capacity, loadFactor, (IMap**)&outmap);
    return outmap;
}

ECode HashSet::Add(
    /* [in] */  IInterface* object)
{
    return AbstractSet::Add(object);
}

ECode HashSet::AddAll(
    /* [in] */  ICollection* collection,
    /* [out] */  Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode HashSet::AddAll(
    /* [in] */  ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode HashSet::ContainsAll(
    /* [in] */  ICollection* collection,
    /* [out] */  Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode HashSet::Equals(
    /* [in] */  IInterface* object,
    /* [out] */  Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode HashSet::GetHashCode(
    /* [out] */  Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode HashSet::Remove(
    /* [in] */  IInterface* object)
{
    return AbstractSet::Remove(object);
}

ECode HashSet::RemoveAll(
    /* [in] */  ICollection* collection,
    /* [out] */  Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode HashSet::RemoveAll(
    /* [in] */  ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode HashSet::RetainAll(
    /* [in] */  ICollection* collection,
    /* [out] */  Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode HashSet::RetainAll(
    /* [in] */  ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}

ECode HashSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode HashSet::ToArray(
    /* [in] */  ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}


} // namespace Utility
} // namespace Elastos
