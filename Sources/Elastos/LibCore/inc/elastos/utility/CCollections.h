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

#ifndef __ELASTOS_UTILITY_CCOLLECTIONSHELPER_H__
#define __ELASTOS_UTILITY_CCOLLECTIONSHELPER_H__

#include "elastos/core/Singleton.h"
#include "_Elastos_Utility_CCollections.h"

using Elastos::Core::Singleton;
using Elastos::Core::IComparator;

namespace Elastos{
namespace Utility{

CarClass(CCollections)
    , public Singleton
    , public ICollections
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI BinarySearch(
        /* [in] */ IList* list,
        /* [in] */ IInterface* object,
        /* [out] */ Int32* index);

    CARAPI BinarySearch(
        /* [in] */ IList* list,
        /* [in] */ IInterface* object,
        /* [in] */ IComparator* comparator,
        /* [out] */ Int32* index);

    CARAPI Copy(
        /* [in] */ IList* destination,
        /* [in] */ IList* source);

    CARAPI Enumeration(
        /* [in] */ ICollection* collection,
        /* [out] */ IEnumeration** result);

    CARAPI Fill(
        /* [in] */ IList* list,
        /* [in] */ IInterface* object);

    CARAPI Max(
        /* [in] */ ICollection* collection,
        /* [out] */ IInterface** result);

    CARAPI Max(
        /* [in] */ ICollection* collection,
        /* [in] */ IComparator* comparator,
        /* [out] */ IInterface** result);

    CARAPI Min(
        /* [in] */ ICollection* collection,
        /* [out] */ IInterface** result);

    CARAPI Min(
        /* [in] */ ICollection* collection,
        /* [in] */ IComparator* comparator,
        /* [out] */ IInterface** result);

    CARAPI NCopies(
        /* [in] */ Int32 length,
        /* [in] */ IInterface* object,
        /* [out] */ IList** result);

    CARAPI Reverse(
        /* [in] */ IList* list);

    CARAPI ReverseOrder(
        /* [out] */ IComparator** result);

    CARAPI ReverseOrder(
        /* [in] */ IComparator* c,
        /* [out] */ IComparator** result);

    CARAPI Shuffle(
        /* [in] */ IList* list);

    CARAPI Shuffle(
        /* [in] */ IList* list,
        /* [in] */ IRandom* random);

    CARAPI Singleton(
        /* [in] */ IInterface* object,
        /* [out] */ ISet** out);

    CARAPI SingletonList(
        /* [in] */ IInterface* object,
        /* [out] */ IList** result);

    CARAPI SingletonMap(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IMap** result);

    CARAPI Sort(
        /* [in] */ IList* list);

    CARAPI Sort(
        /* [in] */ IList* list,
        /* [in] */ IComparator* comparator);

    CARAPI Swap(
        /* [in] */ IList* list,
        /* [in] */ Int32 index1,
        /* [in] */ Int32 index2);

    CARAPI ReplaceAll(
        /* [in] */ IList* list,
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* obj2,
        /* [out] */ Boolean* result);

    CARAPI Rotate(
        /* [in] */ IList* lst,
        /* [in] */ Int32 dist);

    CARAPI IndexOfSubList(
        /* [in] */ IList* list,
        /* [in] */ IList* sublist,
        /* [out] */ Int32* index);

    CARAPI LastIndexOfSubList(
        /* [in] */ IList* list,
        /* [in] */ IList* sublist,
        /* [out] */ Int32* index);

    CARAPI List(
        /* [in] */ IEnumeration* enumeration,
        /* [out] */ IArrayList** result);

    CARAPI SynchronizedCollection(
        /* [in] */ ICollection* collection,
        /* [out] */ ICollection** result);

    CARAPI SynchronizedList(
        /* [in] */ IList* list,
        /* [out] */ IList** result);

    CARAPI SynchronizedMap(
        /* [in] */ IMap* map,
        /* [out] */ IMap** result);

    CARAPI SynchronizedSet(
        /* [in] */ ISet* set,
        /* [out] */ ISet** result);

    CARAPI SynchronizedSortedMap(
        /* [in] */ ISortedMap* map,
        /* [out] */ ISortedMap** result);

    CARAPI SynchronizedSortedSet(
        /* [in] */ ISortedSet* set,
        /* [out] */ ISortedSet** result);

    CARAPI UnmodifiableCollection(
        /* [in] */ ICollection* collection,
        /* [out] */ ICollection** result);

    CARAPI UnmodifiableList(
        /* [in] */ IList* list,
        /* [out] */ IList** result);

    CARAPI UnmodifiableMap(
        /* [in] */ IMap* map,
        /* [out] */ IMap** result);

    CARAPI UnmodifiableSet(
        /* [in] */ ISet* set,
        /* [out] */ ISet** result);

    CARAPI UnmodifiableSortedMap(
        /* [in] */ ISortedMap* map,
        /* [out] */ ISortedMap** result);

    CARAPI UnmodifiableSortedSet(
        /* [in] */ ISortedSet* set,
        /* [out] */ ISortedSet** result);

    CARAPI Frequency(
        /* [in] */ ICollection* c,
        /* [in] */ IInterface* o,
        /* [out] */ Int32* result);

    CARAPI GetEmptyList(
        /* [out] */ IList** result);

    CARAPI GetEmptySet(
        /* [out] */ ISet** result);

    CARAPI GetEmptyMap(
        /* [out] */ IMap** result);

    CARAPI GetEmptyEnumeration(
        /* [out] */ IEnumeration** result);

    CARAPI GetEmptyIterator(
        /* [out] */ IIterator** result);

    CARAPI GetEmptyListIterator(
        /* [out] */ IListIterator** result);

    CARAPI CheckedCollection(
        /* [in] */ ICollection* c,
        /* [in] */ const InterfaceID& type,
        /* [out] */ ICollection** result);

    CARAPI CheckedMap(
        /* [in] */ IMap* m,
        /* [in] */ const InterfaceID& keyType,
        /* [in] */ const InterfaceID& valueType,
        /* [out] */ IMap** result);

    CARAPI CheckedList(
        /* [in] */ IList* list,
        /* [in] */ const InterfaceID& type,
        /* [out] */ IList** result);

    CARAPI CheckedSet(
        /* [in] */ ISet* s,
        /* [in] */ const InterfaceID& type,
        /* [out] */ ISet** result);

    CARAPI CheckedSortedMap(
        /* [in] */ ISortedMap* m,
        /* [in] */ const InterfaceID& keyType,
        /* [in] */ const InterfaceID& valueType,
        /* [out] */ ISortedMap** result);

    CARAPI CheckedSortedSet(
        /* [in] */ ISortedSet* s,
        /* [in] */ const InterfaceID& type,
        /* [out] */ ISortedSet** result);

    CARAPI AddAll(
        /* [in] */ ICollection* c,
        /* [in] */ ArrayOf<IInterface*>* a,
        /* [out] */ Boolean* result);

    CARAPI AddAll(
        /* [in] */ ICollection* c,
        /* [in] */ ArrayOf<IInterface*>* a);

    CARAPI Disjoint(
        /* [in] */ ICollection* c1,
        /* [in] */ ICollection* c2,
        /* [out] */ Boolean* result);

    CARAPI CheckType(
        /* [in] */ IInterface* obj,
        /* [in] */ const InterfaceID& type);

    CARAPI SetFromMap(
        /* [in] */ IMap* map,
        /* [out] */ ISet** result);

    CARAPI AsLifoQueue(
        /* [in] */ IDeque* deque,
        /* [out] */ IQueue** result);
};

} //namespace Utility
} //namespace Elastos

#endif //__ELASTOS_UTILITY_CCOLLECTIONSHELPER_H__