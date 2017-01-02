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

#include "CConcurrentSkipListSet.h"
#include "CBoolean.h"
#include "CConcurrentSkipListMap.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Concurrent::CConcurrentSkipListMap;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CConcurrentSkipListSet::
//====================================================================
CAR_INTERFACE_IMPL_4(CConcurrentSkipListSet, AbstractSet, INavigableSet, ISortedSet, ISerializable, ICloneable)

CAR_OBJECT_IMPL(CConcurrentSkipListSet)

ECode CConcurrentSkipListSet::constructor()
{
    return CConcurrentSkipListMap::New((INavigableMap**)&mM);
}

ECode CConcurrentSkipListSet::constructor(
    /* [in] */ IComparator* comparator)
{
    return CConcurrentSkipListMap::New(comparator, (INavigableMap**)&mM);
}

ECode CConcurrentSkipListSet::constructor(
    /* [in] */ ICollection* c)
{
    CConcurrentSkipListMap::New((INavigableMap**)&mM);
    Boolean b;
    return AbstractSet::AddAll(c, &b);
}

ECode CConcurrentSkipListSet::constructor(
    /* [in] */ ISortedSet* s)
{
    AutoPtr<IComparator> comp;
    s->GetComparator((IComparator**)&comp);
    CConcurrentSkipListMap::New(comp, (INavigableMap**)&mM);
    Boolean b;
    return AbstractSet::AddAll(ICollection::Probe(s), &b);
}

ECode CConcurrentSkipListSet::constructor(
    /* [in] */ INavigableMap* m)
{
    mM = m;
    return NOERROR;
}

ECode CConcurrentSkipListSet::Clone(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<INavigableSet> clone;
    CConcurrentSkipListSet::New((INavigableSet**)&clone);
    AutoPtr<CConcurrentSkipListSet> p = (CConcurrentSkipListSet*)clone.Get();
    AutoPtr<INavigableMap> map;
    CConcurrentSkipListMap::New(ISortedMap::Probe(mM), (INavigableMap**)&map);
    p->SetMap(map);
    *res = p->Probe(EIID_IInterface);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CConcurrentSkipListSet::Add(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Add(object);
}

ECode CConcurrentSkipListSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::AddAll(collection, result);
}

ECode CConcurrentSkipListSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode CConcurrentSkipListSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode CConcurrentSkipListSet::GetHashCode(
    /* [out] */ Int32* hash)
{
    return AbstractSet::GetHashCode(hash);
}

ECode CConcurrentSkipListSet::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Remove(object);
}

ECode CConcurrentSkipListSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode CConcurrentSkipListSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::RetainAll(collection, result);
}

ECode CConcurrentSkipListSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}

ECode CConcurrentSkipListSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode CConcurrentSkipListSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inarray,
    /* [out, callee] */ ArrayOf<IInterface*>** result)
{
    return AbstractSet::ToArray(inarray, result);
}

/* ---------------- Set operations -------------- */

ECode CConcurrentSkipListSet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return (IMap::Probe(mM))->GetSize(size);
}

ECode CConcurrentSkipListSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (IMap::Probe(mM))->IsEmpty(result);
}

ECode CConcurrentSkipListSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (IMap::Probe(mM))->ContainsKey(object, result);
}

ECode CConcurrentSkipListSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<IBoolean> b;
    CBoolean::New(TRUE, (IBoolean**)&b);
    AutoPtr<IConcurrentMap> map = (IConcurrentMap*)mM->Probe(EIID_IConcurrentMap);
    AutoPtr<IInterface> res;
    map->PutIfAbsent(object, b, (IInterface**)&res);
    *modified = res == NULL;
    return NOERROR;
}

ECode CConcurrentSkipListSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<IBoolean> b;
    CBoolean::New(TRUE, (IBoolean**)&b);
    AutoPtr<IConcurrentMap> map = (IConcurrentMap*)mM->Probe(EIID_IConcurrentMap);
    return map->Remove(object, b, modified);
}

ECode CConcurrentSkipListSet::Clear()
{
    return (IMap::Probe(mM))->Clear();
}

ECode CConcurrentSkipListSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<INavigableSet> s;
    mM->GetNavigableKeySet((INavigableSet**)&s);
    return (IIterable::Probe(s))->GetIterator(it);
}

ECode CConcurrentSkipListSet::GetDescendingIterator(
    /* [out] */ IIterator** outiter)
{
    VALIDATE_NOT_NULL(outiter);
    AutoPtr<INavigableSet> s;
    mM->GetDescendingKeySet((INavigableSet**)&s);
    return (IIterable::Probe(s))->GetIterator(outiter);
}

/* ---------------- AbstractSet Overrides -------------- */

ECode CConcurrentSkipListSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Override AbstractSet version to avoid calling size()
    if (Object::Equals(object, TO_IINTERFACE(this))) {
        *result = TRUE;
        return NOERROR;
    }
    if (object->Probe(EIID_ISet) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICollection> c = (ICollection*) object->Probe(EIID_ICollection);
    Boolean a = FALSE, b = FALSE;
    AbstractSet::ContainsAll(c, &a);
    c->ContainsAll(this, &b);
    *result = a && b;
    return NOERROR;
}

ECode CConcurrentSkipListSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Override AbstractSet version to avoid unnecessary call to size()
    Boolean modified = FALSE;
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Boolean b = FALSE;
        Remove(obj, &b);
        if (b) modified = TRUE;
    }
    *res = modified;
    return NOERROR;
}

/* ---------------- Relational operations -------------- */

ECode CConcurrentSkipListSet::GetLower(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetLowerKey(e, outface);
}

ECode CConcurrentSkipListSet::GetFloor(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetFloorKey(e, outface);
}

ECode CConcurrentSkipListSet::GetCeiling(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetCeilingKey(e, outface);
}

ECode CConcurrentSkipListSet::GetHigher(
    /* [in] */ IInterface* e,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mM->GetHigherKey(e, outface);
}

ECode CConcurrentSkipListSet::PollFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IMapEntry> e;
    mM->PollFirstEntry((IMapEntry**)&e);
    if (e == NULL) {
        *outface = NULL;
        return NOERROR;
    }
    else {
        return e->GetKey(outface);
    }
}

ECode CConcurrentSkipListSet::PollLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    AutoPtr<IMapEntry> e;
    mM->PollLastEntry((IMapEntry**)&e);
    if (e == NULL) {
        *outface = NULL;
        return NOERROR;
    }
    else {
        return e->GetKey(outface);
    }
}

/* ---------------- SortedSet operations -------------- */

ECode CConcurrentSkipListSet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom);
    return (ISortedMap::Probe(mM))->GetComparator(outcom);
}

ECode CConcurrentSkipListSet::GetFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return (ISortedMap::Probe(mM))->GetFirstKey(outface);
}

ECode CConcurrentSkipListSet::GetLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return (ISortedMap::Probe(mM))->GetLastKey(outface);
}

ECode CConcurrentSkipListSet::GetSubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean toInclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> map;
    mM->GetSubMap(fromElement, fromInclusive,
                toElement, toInclusive, (INavigableMap**)&map);
    AutoPtr<INavigableSet> p;
    CConcurrentSkipListSet::New(map, (INavigableSet**)&p);
    *outnav = p.Get();
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetHeadSet(
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> map;
    mM->GetHeadMap(toElement, inclusive, (INavigableMap**)&map);
    AutoPtr<INavigableSet> p;
    CConcurrentSkipListSet::New(map, (INavigableSet**)&p);
    *outnav = p.Get();
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetTailSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> map;
    mM->GetTailMap(fromElement, inclusive, (INavigableMap**)&map);
    AutoPtr<INavigableSet> p;
    CConcurrentSkipListSet::New(map, (INavigableSet**)&p);
    *outnav = p.Get();
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> s;
    GetSubSet(start, TRUE, end, FALSE, (INavigableSet**)&s);
    AutoPtr<ISortedSet> p = (ISortedSet*)s->Probe(EIID_ISortedSet);
    *outsort = p;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> s;
    GetHeadSet(end, FALSE, (INavigableSet**)&s);
    AutoPtr<ISortedSet> p = (ISortedSet*)s->Probe(EIID_ISortedSet);
    *outsort = p;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<INavigableSet> s;
    GetTailSet(start, TRUE, (INavigableSet**)&s);
    AutoPtr<ISortedSet> p = (ISortedSet*)s->Probe(EIID_ISortedSet);
    *outsort = p;
    REFCOUNT_ADD(*outsort);
    return NOERROR;
}

ECode CConcurrentSkipListSet::GetDescendingSet(
    /* [out] */ INavigableSet** outnav)
{
    VALIDATE_NOT_NULL(outnav);
    AutoPtr<INavigableMap> map;
    mM->GetDescendingMap((INavigableMap**)&map);
    AutoPtr<INavigableSet> p;
    CConcurrentSkipListSet::New(map, (INavigableSet**)&p);
    *outnav = p.Get();
    REFCOUNT_ADD(*outnav);
    return NOERROR;
}

void CConcurrentSkipListSet::SetMap(
    /* [in] */ INavigableMap* map)
{
    assert(0 && "TODO");
//    UNSAFE.putObjectVolatile(this, mapOffset, map);
}

// private static final sun.misc.Unsafe UNSAFE;
// private static final long mapOffset;
// static {
//     try {
//         UNSAFE = sun.misc.Unsafe.getUnsafe();
//         Class<?> k = ConcurrentSkipListSet.class;
//         mapOffset = UNSAFE.objectFieldOffset
//             (k.getDeclaredField("m"));
//     } catch (Exception e) {
//         throw new Error(e);
//     }
// }

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
