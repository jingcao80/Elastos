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
#include "CTreeSet.h"
#include "CTreeMap.h"
#include "CBoolean.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IComparable;
using Elastos::Core::EIID_ICloneable;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Utility {

CAR_OBJECT_IMPL(CTreeSet)

CAR_INTERFACE_IMPL_5(CTreeSet, AbstractSet, ITreeSet, INavigableSet, ISortedSet, ICloneable, ISerializable)

ECode CTreeSet::constructor()
{
    FAIL_RETURN(CTreeMap::New((INavigableMap**)&mBackingMap));
    return NOERROR;
}

ECode CTreeSet::constructor(
    /* [in] */ INavigableMap* navmap)
{
    mBackingMap = navmap;
    return NOERROR;
}

ECode CTreeSet::constructor(
    /* [in] */ ICollection* map)
{
    this->constructor();
    Boolean isflag = FALSE;
    return AddAll(map, &isflag);
}

ECode CTreeSet::constructor(
    /* [in] */ IComparator* comparator)
{
    return CTreeMap::New(comparator, (INavigableMap**)&mBackingMap);
}

ECode CTreeSet::constructor(
    /* [in] */ ISortedSet* set)
{
    AutoPtr<IComparator> rescom;
    set->GetComparator((IComparator**)&rescom);
    this->constructor(rescom);
    AutoPtr<IIterator> it;
    if (IIterable::Probe(set) == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    set->GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while (it->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        Add(outface, &isflag);
    }
    return NOERROR;
}

ECode CTreeSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr<IBoolean> bltrue;
    CBoolean::New(TRUE, (IBoolean**)&bltrue);
    AutoPtr<IInterface> outface;
    (IMap::Probe(mBackingMap))->Put(object, bltrue, (IInterface**)&outface);
    *modified = outface == NULL;
    return NOERROR;
}

ECode CTreeSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    return AbstractSet::AddAll(collection, modified);
}

ECode CTreeSet::Clear()
{
    return (IMap::Probe(mBackingMap))->Clear();
}

ECode CTreeSet::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    // try {
    AutoPtr<INavigableSet> outset;
    FAIL_RETURN(CTreeSet::New((INavigableSet**)&outset));
    AutoPtr<CTreeSet> clone = (CTreeSet*)outset.Get();
    if (INavigableMap::Probe(mBackingMap)) {
        AutoPtr<IInterface> outface;
        ICloneable::Probe(mBackingMap)->Clone((IInterface**)&outface);
        clone->mBackingMap = INavigableMap::Probe(outface);
    }
    else {
        AutoPtr<ITreeMap> temp;
        FAIL_RETURN(CTreeMap::New(ISortedMap::Probe(mBackingMap), (ITreeMap**)&temp));
        AutoPtr<INavigableMap> resmap = INavigableMap::Probe(temp);
        clone->mBackingMap = resmap;
    }
    *object = outset;
    REFCOUNT_ADD(*object)
    // } catch (CloneNotSupportedException e) {
    //     throw new AssertionError(e);
    // }
    return NOERROR;
}

ECode CTreeSet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    return (ISortedMap::Probe(mBackingMap))->GetComparator(outcom);
}

ECode CTreeSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return (IMap::Probe(mBackingMap))->ContainsKey(object, result);
}

ECode CTreeSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return (IMap::Probe(mBackingMap))->IsEmpty(result);
}

ECode CTreeSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<ISet> outset;
    (IMap::Probe(mBackingMap))->GetKeySet((ISet**)&outset);
    return (IIterable::Probe(outset))->GetIterator(it);
}

ECode CTreeSet::GetDescendingIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<INavigableSet> res;
    GetDescendingSet((INavigableSet**)&res);
    return res->GetIterator(outiter);
}

ECode CTreeSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr<IInterface> outface;
    (IMap::Probe(mBackingMap))->Remove(object, (IInterface**)&outface);
    *modified = outface != NULL;
    return NOERROR;
}

ECode CTreeSet::GetSize(
    /* [out] */ Int32* size)
{
    return (IMap::Probe(mBackingMap))->GetSize(size);
}

ECode CTreeSet::GetFirst(
    /* [out] */ IInterface** outface)
{
    return (ISortedMap::Probe(mBackingMap))->GetFirstKey(outface);
}

ECode CTreeSet::GetLast(
    /* [out] */ IInterface** outface)
{
    return (ISortedMap::Probe(mBackingMap))->GetLastKey(outface);
}

ECode CTreeSet::PollFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IMapEntry> entry;
    mBackingMap->PollFirstEntry((IMapEntry**)&entry);
    if (entry == NULL) {
        *outface = NULL;
    }
    else {
        entry->GetKey(outface);
    }
    return NOERROR;
}

ECode CTreeSet::PollLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IMapEntry> entry;
    mBackingMap->PollLastEntry((IMapEntry**)&entry);
    if (entry == NULL) {
        *outface = NULL;
    }
    else {
        entry->GetKey(outface);
    }
    return NOERROR;
}

ECode CTreeSet::GetHigher(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    return mBackingMap->GetHigherKey(e, outface);
}

ECode CTreeSet::GetLower(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    return mBackingMap->GetLowerKey(e, outface);
}

ECode CTreeSet::GetCeiling(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    return mBackingMap->GetCeilingKey(e, outface);
}

ECode CTreeSet::GetFloor(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    return mBackingMap->GetFloorKey(e, outface);
}

ECode CTreeSet::GetDescendingSet(
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav)

    if (mDescendingSet == NULL) {
        AutoPtr<INavigableMap> res;
        mBackingMap->GetDescendingMap((INavigableMap**)&res);
        CTreeSet::New(res, (INavigableSet**)&mDescendingSet);
    }
    *outnav = mDescendingSet;
    REFCOUNT_ADD(*outnav)
    return NOERROR;
}

ECode CTreeSet::GetSubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav)

    AutoPtr<IComparator> c;
    (ISortedMap::Probe(mBackingMap))->GetComparator((IComparator**)&c);
    Int32 compare = 0;
    if (c == NULL) {
        if (IComparable::Probe(fromElement) == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IComparable::Probe(fromElement)->CompareTo(toElement, &compare);
    }
    else {
        c->Compare(fromElement, toElement, &compare);
    }
    if (compare <= 0) {
        AutoPtr<CTreeSet> outset;
        AutoPtr<INavigableMap> resmap;
        mBackingMap->GetSubMap(fromElement, fromInclusive, toElement, toInclusive, (INavigableMap**)&resmap);
        FAIL_RETURN(CTreeSet::NewByFriend(resmap, (CTreeSet**)&outset));
        *outnav = INavigableSet::Probe(outset);
        REFCOUNT_ADD(*outnav)
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTreeSet::GetHeadSet(
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav)

    // Check for errors
    AutoPtr<IComparator> c;
    (ISortedMap::Probe(mBackingMap))->GetComparator((IComparator**)&c);
    Int32 compare = 0;
    if (c == NULL) {
        if (IComparable::Probe(toElement) == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IComparable::Probe(toElement)->CompareTo(toElement, &compare);
    }
    else {
        c->Compare(toElement, toElement, &compare);
    }
    AutoPtr<CTreeSet> resset;
    AutoPtr<INavigableMap> resmap;
    mBackingMap->GetHeadMap(toElement, inclusive, (INavigableMap**)&resmap);
    FAIL_RETURN(CTreeSet::NewByFriend(resmap, (CTreeSet**)&resset));
    *outnav = INavigableSet::Probe(resset);
    REFCOUNT_ADD(*outnav)
    return NOERROR;
}

ECode CTreeSet::GetTailSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav)

    // Check for errors
    AutoPtr<IComparator> c;
    (ISortedMap::Probe(mBackingMap))->GetComparator((IComparator**)&c);
    Int32 compare = 0;
    if (c == NULL) {
        if (IComparable::Probe(fromElement) == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IComparable::Probe(fromElement)->CompareTo(fromElement, &compare);
    }
    else {
        c->Compare(fromElement, fromElement, &compare);
    }
    AutoPtr<INavigableMap> resmap;
    AutoPtr<CTreeSet> resset;
    mBackingMap->GetTailMap(fromElement, inclusive, (INavigableMap**)&resmap);
    FAIL_RETURN(CTreeSet::NewByFriend(resmap, (CTreeSet**)&resset));
    *outnav = INavigableSet::Probe(resset);
    REFCOUNT_ADD(*outnav)
    return NOERROR;
}

ECode CTreeSet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort)
    AutoPtr<INavigableSet> ns;
    FAIL_RETURN(GetSubSet(start, TRUE, end, FALSE, (INavigableSet**)&ns));
    *outsort = ISortedSet::Probe(ns);
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CTreeSet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort)
    AutoPtr<INavigableSet> ns;
    GetHeadSet(end, FALSE, (INavigableSet**)&ns);
    *outsort = ISortedSet::Probe(ns);
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CTreeSet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort)
    AutoPtr<INavigableSet> ns;
    GetTailSet(start, TRUE, (INavigableSet**)&ns);
    *outsort = ISortedSet::Probe(ns);
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

void CTreeSet::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();
    AutoPtr<IComparator> rescom;
    (ISortedMap::Probe(mBackingMap))->GetComparator((IComparator**)&rescom);
    assert(0 && "TODO");
    // stream->WriteObject(rescom);
    Int32 size = 0;
    (IMap::Probe(mBackingMap))->GetSize(&size);
    (IOutputStream::Probe(stream))->Write(size);
    if (size > 0) {
        AutoPtr<ISet> outset;
        (IMap::Probe(mBackingMap))->GetKeySet((ISet**)&outset);
        AutoPtr<IIterator> it;
        (IIterable::Probe(outset))->GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while (it->HasNext(&isflag), isflag) {
            assert(0 && "TODO");
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            // stream->WriteObject(outface);
        }
    }
}

void CTreeSet::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();
    AutoPtr<IInterface> outface;
    assert(0 && "TODO");
    // stream->ReadObject((IInterface**)&outface);
    AutoPtr<INavigableMap> map;
    CTreeMap::New(IComparator::Probe(outface), (INavigableMap**)&map);
    Int32 size = 0;
    (IInputStream::Probe(stream))->Read(&size);
    if (size > 0) {
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> elem;
            assert(0 && "TODO");
            // stream->ReadObject((IInterface**)&elem);
            AutoPtr<IBoolean> botrue;
            CBoolean::New(TRUE, (IBoolean**)&botrue);
            AutoPtr<IInterface> outface2;
            (IMap::Probe(map))->Put(elem, botrue, (IInterface**)&outface2);
        }
    }
    mBackingMap = map;
}

ECode CTreeSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CTreeSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode CTreeSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode CTreeSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode CTreeSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode CTreeSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode CTreeSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}

ECode CTreeSet::Add(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Add(object);
}

ECode CTreeSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode CTreeSet::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Remove(object);
}

ECode CTreeSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode CTreeSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}


} // namespace Utility
} // namespace Elastos
