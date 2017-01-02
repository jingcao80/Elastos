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

#ifndef __ELASTOS_UTILITY_CTREESET_H__
#define __ELASTOS_UTILITY_CTREESET_H__

#include "_Elastos_Utility_CTreeSet.h"
#include "AbstractSet.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectStreamField;
using Elastos::Core::IComparator;
using Elastos::Core::ICloneable;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

CarClass(CTreeSet)
    , public AbstractSet
    , public ITreeSet
    , public INavigableSet
    , public ISortedSet
    , public ICloneable
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INavigableMap* navmap);

    CARAPI constructor(
        /* [in] */ ICollection* map);

    CARAPI constructor(
        /* [in] */ IComparator* comparator);

    CARAPI constructor(
        /* [in] */ ISortedSet* set);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI Clear();

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI GetLower(
        /* [in] */ IInterface* e,
        /* [out] */ IInterface** outface);

    CARAPI GetFloor(
        /* [in] */ IInterface* e,
        /* [out] */ IInterface** outface);

    CARAPI GetCeiling(
        /* [in] */ IInterface* e,
        /* [out] */ IInterface** outface);

    CARAPI GetHigher(
        /* [in] */ IInterface* e,
        /* [out] */ IInterface** outface);

    CARAPI PollFirst(
        /* [out] */ IInterface** outface);

    CARAPI PollLast(
        /* [out] */ IInterface** outface);

    CARAPI GetDescendingSet(
        /* [out] */ INavigableSet** outnav);

    CARAPI GetDescendingIterator(
        /* [out] */ IIterator** outiter);

    CARAPI GetSubSet(
        /* [in] */ IInterface* fromElement,
        /* [in] */ Boolean fromInclusive,
        /* [in] */ IInterface* toElement,
        /* [in] */ Boolean toInclusive,
        /* [out] */ INavigableSet** outnav);

    CARAPI GetHeadSet(
        /* [in] */ IInterface* toElement,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableSet** outnav);

    CARAPI GetTailSet(
        /* [in] */ IInterface* fromElement,
        /* [in] */ Boolean inclusive,
        /* [out] */ INavigableSet** outnav);

    CARAPI GetComparator(
        /* [out] */ IComparator** outcom);

    CARAPI GetFirst(
        /* [out] */ IInterface** outface);

    CARAPI GetHeadSet(
        /* [in] */ IInterface* end,
        /* [out] */ ISortedSet** outsort);

    CARAPI GetLast(
        /* [out] */ IInterface** outface);

    CARAPI GetSubSet(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end,
        /* [out] */ ISortedSet** outsort);

    CARAPI GetTailSet(
        /* [in] */ IInterface* start,
        /* [out] */ ISortedSet** outsort);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

protected:
    CARAPI_(String) GetClassName() { return String("CTreeSet"); }

private:
    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* stream);

private:
    /** Keys are this set's elements. Values are always Boolean.TRUE */
    /* transient */ AutoPtr<INavigableMap> mBackingMap;

    /* transient */ AutoPtr<INavigableSet> mDescendingSet;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CTREESET_H__
