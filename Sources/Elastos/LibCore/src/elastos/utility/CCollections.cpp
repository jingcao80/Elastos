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

#include "CCollections.h"
#include "Collections.h"

namespace Elastos{
namespace Utility{

CAR_INTERFACE_IMPL(CCollections, Singleton, ICollections)

CAR_SINGLETON_IMPL(CCollections)

ECode CCollections::BinarySearch(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return Collections::BinarySearch(list, object, index);
}

ECode CCollections::BinarySearch(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object,
    /* [in] */ IComparator* comparator,
    /* [out] */ Int32* index)
{
    return Collections::BinarySearch(list, object, comparator, index);
}

ECode CCollections::Copy(
    /* [in] */ IList* destination,
    /* [in] */ IList* source)
{
    return Collections::Copy(destination, source);
}

ECode CCollections::Enumeration(
    /* [in] */ ICollection* collection,
    /* [out] */ IEnumeration** result)
{
    return Collections::Enumeration(collection, result);
}

ECode CCollections::Fill(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object)
{
    return Collections::Fill(list, object);
}

ECode CCollections::Max(
    /* [in] */ ICollection* collection,
    /* [out] */ IInterface** result)
{
    return Collections::Max(collection, result);
}

ECode CCollections::Max(
    /* [in] */ ICollection* collection,
    /* [in] */ IComparator* comparator,
    /* [out] */ IInterface** result)
{
    return Collections::Max(collection, comparator, result);
}

ECode CCollections::Min(
    /* [in] */ ICollection* collection,
    /* [out] */ IInterface** result)
{
    return Collections::Min(collection, result);
}

ECode CCollections::Min(
    /* [in] */ ICollection* collection,
    /* [in] */ IComparator* comparator,
    /* [out] */ IInterface** result)
{
    return Collections::Min(collection, comparator, result);
}

ECode CCollections::NCopies(
    /* [in] */ Int32 length,
    /* [in] */ IInterface* object,
    /* [out] */ IList** result)
{
    return Collections::NCopies(length, object, result);
}

ECode CCollections::Reverse(
    /* [in] */ IList* list)
{
    return Collections::Reverse(list);
}

ECode CCollections::ReverseOrder(
    /* [out] */ IComparator** result)
{
    return Collections::ReverseOrder(result);
}

ECode CCollections::ReverseOrder(
    /* [in] */ IComparator* c,
    /* [out] */ IComparator** result)
{
    return Collections::ReverseOrder(c, result);
}

ECode CCollections::Shuffle(
    /* [in] */ IList* list)
{
    return Collections::Shuffle(list);
}

ECode CCollections::Shuffle(
    /* [in] */ IList* list,
    /* [in] */ IRandom* random)
{
    return Collections::Shuffle(list, random);
}

ECode CCollections::Singleton(
    /* [in] */ IInterface* object,
    /* [out] */ ISet** out)
{
    return Collections::Singleton(object, out);
}

ECode CCollections::SingletonList(
    /* [in] */ IInterface* object,
    /* [out] */ IList** result)
{
    return Collections::SingletonList(object, result);
}

ECode CCollections::SingletonMap(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IMap** result)
{
    return Collections::SingletonMap(key, value, result);
}

ECode CCollections::Sort(
    /* [in] */ IList* list)
{
    return Collections::Sort(list);
}

ECode CCollections::Sort(
    /* [in] */ IList* list,
    /* [in] */ IComparator* comparator)
{
    return Collections::Sort(list, comparator);
}

ECode CCollections::Swap(
    /* [in] */ IList* list,
    /* [in] */ Int32 index1,
    /* [in] */ Int32 index2)
{
    return Collections::Swap(list, index1, index2);
}

ECode CCollections::ReplaceAll(
    /* [in] */ IList* list,
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* obj2,
    /* [out] */ Boolean* result)
{
    return Collections::ReplaceAll(list, obj, obj2, result);
}

ECode CCollections::Rotate(
    /* [in] */ IList* lst,
    /* [in] */ Int32 dist)
{
    return Collections::Rotate(lst, dist);
}

ECode CCollections::IndexOfSubList(
    /* [in] */ IList* list,
    /* [in] */ IList* sublist,
    /* [out] */ Int32* index)
{
    return Collections::IndexOfSubList(list, sublist, index);
}

ECode CCollections::LastIndexOfSubList(
    /* [in] */ IList* list,
    /* [in] */ IList* sublist,
    /* [out] */ Int32* index)
{
    return Collections::LastIndexOfSubList(list, sublist, index);
}

ECode CCollections::List(
    /* [in] */ IEnumeration* enumeration,
    /* [out] */ IArrayList** result)
{
    return Collections::List(enumeration, result);
}

ECode CCollections::SynchronizedCollection(
    /* [in] */ ICollection* collection,
    /* [out] */ ICollection** result)
{
    return Collections::SynchronizedCollection(collection, result);
}

ECode CCollections::SynchronizedList(
    /* [in] */ IList* list,
    /* [out] */ IList** result)
{
    return Collections::SynchronizedList(list, result);
}

ECode CCollections::SynchronizedMap(
    /* [in] */ IMap* map,
    /* [out] */ IMap** result)
{
    return Collections::SynchronizedMap(map, result);
}

ECode CCollections::SynchronizedSet(
    /* [in] */ ISet* set,
    /* [out] */ ISet** result)
{
    return Collections::SynchronizedSet(set, result);
}

ECode CCollections::SynchronizedSortedMap(
    /* [in] */ ISortedMap* map,
    /* [out] */ ISortedMap** result)
{
    return Collections::SynchronizedSortedMap(map, result);
}

ECode CCollections::SynchronizedSortedSet(
    /* [in] */ ISortedSet* set,
    /* [out] */ ISortedSet** result)
{
    return Collections::SynchronizedSortedSet(set, result);
}

ECode CCollections::UnmodifiableCollection(
    /* [in] */ ICollection* collection,
    /* [out] */ ICollection** result)
{
    return Collections::UnmodifiableCollection(collection, result);
}

ECode CCollections::UnmodifiableList(
    /* [in] */ IList* list,
    /* [out] */ IList** result)
{
    return Collections::UnmodifiableList(list, result);
}

ECode CCollections::UnmodifiableMap(
    /* [in] */ IMap* map,
    /* [out] */ IMap** result)
{
    return Collections::UnmodifiableMap(map, result);
}

ECode CCollections::UnmodifiableSet(
    /* [in] */ ISet* set,
    /* [out] */ ISet** result)
{
    return Collections::UnmodifiableSet(set, result);
}

ECode CCollections::UnmodifiableSortedMap(
    /* [in] */ ISortedMap* map,
    /* [out] */ ISortedMap** result)
{
    return Collections::UnmodifiableSortedMap(map, result);
}

ECode CCollections::UnmodifiableSortedSet(
    /* [in] */ ISortedSet* set,
    /* [out] */ ISortedSet** result)
{
    return Collections::UnmodifiableSortedSet(set, result);
}

ECode CCollections::Frequency(
    /* [in] */ ICollection* c,
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    return Collections::Frequency(c, o, result);
}

ECode CCollections::GetEmptyList(
    /* [out] */ IList** result)
{
    return Collections::GetEmptyList(result);
}

ECode CCollections::GetEmptySet(
    /* [out] */ ISet** result)
{
    return Collections::GetEmptySet(result);
}

ECode CCollections::GetEmptyMap(
    /* [out] */ IMap** result)
{
    return Collections::GetEmptyMap(result);
}

ECode CCollections::GetEmptyEnumeration(
    /* [out] */ IEnumeration** result)
{
    return Collections::GetEmptyEnumeration(result);
}

ECode CCollections::GetEmptyIterator(
    /* [out] */ IIterator** result)
{
    return Collections::GetEmptyIterator(result);
}

ECode CCollections::GetEmptyListIterator(
    /* [out] */ IListIterator** result)
{
    return Collections::GetEmptyListIterator(result);
}

ECode CCollections::CheckedCollection(
    /* [in] */ ICollection* c,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ICollection** result)
{
    return Collections::CheckedCollection(c, type, result);
}

ECode CCollections::CheckedMap(
    /* [in] */ IMap* m,
    /* [in] */ const InterfaceID& keyType,
    /* [in] */ const InterfaceID& valueType,
    /* [out] */ IMap** result)
{
    return Collections::CheckedMap(m, keyType, valueType, result);
}

ECode CCollections::CheckedList(
    /* [in] */ IList* list,
    /* [in] */ const InterfaceID& type,
    /* [out] */ IList** result)
{
    return Collections::CheckedList(list, type, result);
}

ECode CCollections::CheckedSet(
    /* [in] */ ISet* s,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ISet** result)
{
    return Collections::CheckedSet(s, type, result);
}

ECode CCollections::CheckedSortedMap(
    /* [in] */ ISortedMap* m,
    /* [in] */ const InterfaceID& keyType,
    /* [in] */ const InterfaceID& valueType,
    /* [out] */ ISortedMap** result)
{
    return Collections::CheckedSortedMap(m, keyType, valueType, result);
}

ECode CCollections::CheckedSortedSet(
    /* [in] */ ISortedSet* s,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ISortedSet** result)
{
    return Collections::CheckedSortedSet(s, type, result);
}

ECode CCollections::AddAll(
    /* [in] */ ICollection* c,
    /* [in] */ ArrayOf<IInterface*>* a,
    /* [out] */ Boolean* result)
{
    return Collections::AddAll(c, a, result);
}

ECode CCollections::AddAll(
    /* [in] */ ICollection* c,
    /* [in] */ ArrayOf<IInterface*>* a)
{
    Boolean result;
    return Collections::AddAll(c, a, &result);
}


ECode CCollections::Disjoint(
    /* [in] */ ICollection* c1,
    /* [in] */ ICollection* c2,
    /* [out] */ Boolean* result)
{
    return Collections::Disjoint(c1, c2, result);
}

ECode CCollections::CheckType(
    /* [in] */ IInterface* obj,
    /* [in] */ const InterfaceID& type)
{
    return Collections::CheckType(obj, type);
}

ECode CCollections::SetFromMap(
    /* [in] */ IMap* map,
    /* [out] */ ISet** result)
{
    return Collections::SetFromMap(map, result);
}

ECode CCollections::AsLifoQueue(
    /* [in] */ IDeque* deque,
    /* [out] */ IQueue** result)
{
    return Collections::AsLifoQueue(deque, result);
}

} // namespace Utility
} // namespace Elastos