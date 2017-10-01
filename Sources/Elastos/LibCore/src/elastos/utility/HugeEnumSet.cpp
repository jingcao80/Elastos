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

#include "HugeEnumSet.h"
#include "Math.h"
#include "Arrays.h"

using Elastos::Core::IEnum;

namespace Elastos {
namespace Utility {

/////////////////////////////////////////
//   HugeEnumSet::HugeEnumSetIterator
/////////////////////////////////////////
CAR_INTERFACE_IMPL(HugeEnumSet::HugeEnumSetIterator, Object, IIterator)

HugeEnumSet::HugeEnumSetIterator::HugeEnumSetIterator(
    /* [in] */ IHugeEnumSet* host)
    : mHost(host)
    , mIndex(0)
    , mMask(0)
    , mLast(NULL)
{
    AutoPtr< ArrayOf<Int64> > bits;
    mHost->GetBits((ArrayOf<Int64>**)&bits);
    mCurrentBits = (*bits)[0];
    ComputeNextElement();
}

ECode HugeEnumSet::HugeEnumSetIterator::ComputeNextElement()
{
    AutoPtr<ArrayOf <Int64> > bits;
    mHost->GetBits((ArrayOf <Int64>**)&bits);
    while (TRUE) {
        if (mCurrentBits != 0) {
            mMask = mCurrentBits & -mCurrentBits; // the lowest 1 bit in currentBits
            return NOERROR;
        } else if (++mIndex < bits->GetLength()) {
            mCurrentBits = (*bits)[mIndex];
        } else {
            mMask = 0;
            return NOERROR;
        }
    }
}

ECode HugeEnumSet::HugeEnumSetIterator::HasNext(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = mMask != 0;
    return NOERROR;
}

ECode HugeEnumSet::HugeEnumSetIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (mMask == 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    Int32 ordinal = Elastos::Core::Math::NumberOfLeadingZeros(mMask) + mIndex * HugeEnumSet::BIT_IN_LONG;

    AutoPtr<ArrayOf <IInterface*> > enums;
    mHost->GetEnums((ArrayOf <IInterface*>**)&enums);
    mLast = (*enums)[ordinal];

    mCurrentBits &= ~mMask;
    ComputeNextElement();
    *object = mLast;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode HugeEnumSet::HugeEnumSetIterator::Remove()
{
    if (mLast == NULL) {
        return  E_ILLEGAL_STATE_EXCEPTION;
    }

    (ICollection::Probe(mHost))->Remove(mLast);
    mLast = NULL;
    return NOERROR;
}

/////////////////////////////////////////
//              HugeEnumSet
/////////////////////////////////////////
CAR_INTERFACE_IMPL(HugeEnumSet, EnumSet, IHugeEnumSet)

const Int32 HugeEnumSet::BIT_IN_LONG = 64;

HugeEnumSet::HugeEnumSet()
    : EnumSet(EIID_IInterface)
    , mEnums(NULL)
    , mBits(NULL)
    , mSize(0)
{
}

HugeEnumSet::HugeEnumSet(
    /* [in] */ const InterfaceID& cls,
    /* [in] */ ArrayOf<IInterface *> * enums)
    : EnumSet(cls)
    , mEnums(enums)
    , mSize(0)
{
    mBits = ArrayOf<Int64>::Alloc((mEnums->GetLength() + BIT_IN_LONG - 1) / BIT_IN_LONG);
}

ECode HugeEnumSet::constructor()
{
    EnumSet::constructor();
    return NOERROR;
}

ECode HugeEnumSet::constructor(
    /* [in] */ const InterfaceID& cls,
    /* [in] */ ArrayOf<IInterface *> * enums)
{
    EnumSet::constructor(cls);
    mBits = ArrayOf<Int64>::Alloc((mEnums->GetLength() + BIT_IN_LONG - 1) / BIT_IN_LONG);
    return NOERROR;
}

ECode HugeEnumSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    //elementClass.cast(object); // Called to throw ClassCastException.
    Int32 ordinal;
    (IEnum::Probe(object))->Ordinal(&ordinal);
    Int32 index = ordinal / BIT_IN_LONG;
    Int32 inBits = ordinal % BIT_IN_LONG;
    Int64 oldBits = (*mBits)[index];
    Int64 newBits = oldBits | (1L << inBits);
    if (oldBits != newBits) {
        (*mBits)[index] = newBits;
        mSize++;
        *modified = TRUE;
        return NOERROR;
    }
    *modified = FALSE;
    return NOERROR;
}

ECode HugeEnumSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    Boolean empty;
    collection->IsEmpty(&empty);
    if (empty || collection == ICollection::Probe(this)) {
        return FALSE;
    }

    if (IEnumSet::Probe(collection)) {
        AutoPtr<IEnumSet> set = IEnumSet::Probe(collection); // raw type due to javac bug 6548436
        //set.elementClass.asSubclass(elementClass); // Called to throw ClassCastException.

        AutoPtr<IHugeEnumSet> hugeSet = IHugeEnumSet::Probe(set);
        Boolean changed = FALSE;
        for (Int32 i = 0; i < mBits->GetLength(); i++) {
            Int64 oldBits = (*mBits)[i];
            AutoPtr< ArrayOf <Int64> > bits;
            hugeSet->GetBits((ArrayOf<Int64>**)&bits);
            Int64 newBits = oldBits | (*bits)[i];
            if (oldBits != newBits) {
                (*mBits)[i] = newBits;
                mSize += Elastos::Core::Math::BitCount(newBits) - Elastos::Core::Math::BitCount(oldBits);
                changed = TRUE;
            }
        }
        *modified = changed;
        return NOERROR;
    }
    return EnumSet::AddAll(collection);
}

ECode HugeEnumSet::Clear()
{
    Arrays::Fill(mBits, (Int64)0);
    mSize = 0;
    return NOERROR;
}

ECode HugeEnumSet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode HugeEnumSet::Complement()
{
    mSize = 0;
    for (Int32 i = 0, length = mBits->GetLength(); i < length; i++) {
        Int64 b = ~(*mBits)[i];

        // zero out unused bits on the last element
        if (i == length - 1) {
            b &= -1L >> (BIT_IN_LONG - (mEnums->GetLength() % BIT_IN_LONG));// >>>
        }

        mSize += Elastos::Core::Math::BitCount(b);
        (*mBits)[i] = b;
    }
    return NOERROR;
}

ECode HugeEnumSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!object || !IsValidType(EIID_IInterface/*object.getClass()*/)) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 ordinal = 0;//((E) object).ordinal();
    Int32 index = ordinal / BIT_IN_LONG;
    Int32 inBits = ordinal % BIT_IN_LONG;
    *result =  ((*mBits)[index] & (1L << inBits)) != 0;
    return NOERROR;
}

ECode HugeEnumSet::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    assert(0 && "TODO");
    return NOERROR;
}

ECode HugeEnumSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean empty;
    if (collection->IsEmpty(&empty), empty) {
        *result = TRUE;
        return NOERROR;
    }
    if (IHugeEnumSet::Probe(collection)) {
        AutoPtr<IHugeEnumSet> set = IHugeEnumSet::Probe(collection);
        if (IsValidType(EIID_IInterface/*set.elementClass*/)) {
            for (Int32 i = 0; i < mBits->GetLength(); i++) {
                AutoPtr< ArrayOf <Int64> > bits;
                set->GetBits((ArrayOf<Int64>**)&bits);
                Int64 setBits = (*bits)[i];
                if (((*mBits)[i] & setBits) != setBits) {
                    *result = FALSE;
                    return NOERROR;
                }
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    Boolean isContainsAll = FALSE;
    *result = !(IHugeEnumSet::Probe(collection)) && (EnumSet::ContainsAll(collection, &isContainsAll), isContainsAll);
    return NOERROR;
}

ECode HugeEnumSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!object) {
        *result = FALSE;
        return NOERROR;
    }
    if (!IsValidType(EIID_IInterface/*object.getClass()*/)) {
         EnumSet::Equals(object, result);
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int64> > bits;
    (IHugeEnumSet::Probe(object))->GetBits((ArrayOf<Int64>**)&bits);
    *result = Arrays::Equals(mBits, bits);
    return NOERROR;
}

ECode HugeEnumSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new HugeEnumSetIterator(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode HugeEnumSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    if (!object || !IsValidType(/*object.getClass()*/EIID_IInterface)) {
        *modified = FALSE;
        return NOERROR;
    }

    Int32 ordinal;
    (IEnum::Probe(object))->Ordinal(&ordinal);
    Int32 index = ordinal / BIT_IN_LONG;
    Int32 inBits = ordinal % BIT_IN_LONG;
    Int64 oldBits = (*mBits)[index];
    Int64 newBits = oldBits & ~(1L << inBits);
    if (oldBits != newBits) {
        (*mBits)[index] = newBits;
        (mSize)--;
        *modified = TRUE;
        return NOERROR;
    }
    *modified = FALSE;
    return NOERROR;
}

ECode HugeEnumSet::Remove(
    /* [in] */ IInterface* object)
{
    Boolean res = FALSE;
    return Remove(object, &res);
}

ECode HugeEnumSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    Boolean empty;
    if (collection->IsEmpty(&empty), empty) {
        *modified = FALSE;
        return NOERROR;
    }

    if (IEnumSet::Probe(collection)) {
        AutoPtr<IEnumSet> set = IEnumSet::Probe(collection);
        if (!IsValidType(/*set.elementClass*/EIID_IInterface)) {
            *modified = FALSE;
            return NOERROR;
        }

        AutoPtr<IHugeEnumSet> hugeSet = IHugeEnumSet::Probe(set);
        Boolean changed = FALSE;
        for (Int32 i = 0; i < mBits->GetLength(); i++) {
            Int64 oldBits = (*mBits)[i];
            AutoPtr< ArrayOf <Int64> > bits;
            hugeSet->GetBits((ArrayOf <Int64>**)&bits);
            Int64 newBits = oldBits& ~(*bits)[i];
            if (oldBits != newBits) {
                (*mBits)[i] = newBits;
                mSize = Elastos::Core::Math::BitCount(newBits) - Elastos::Core::Math::BitCount(oldBits);
                changed = TRUE;
            }
        }
        *modified = changed;
        return NOERROR;
    }

    return EnumSet::RemoveAll(collection, modified);
}

ECode HugeEnumSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean res = FALSE;
    return RemoveAll(collection, &res);
}

ECode HugeEnumSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    if (IEnumSet::Probe(collection)) {
        AutoPtr<IEnumSet> set = IEnumSet::Probe(collection);
        if (!IsValidType(/*set.elementClass*/EIID_IInterface)) {
            if (mSize > 0) {
                Clear();
                *modified = TRUE;
                return NOERROR;
            } else {
                *modified = FALSE;
                return NOERROR;
            }
        }

        AutoPtr<IHugeEnumSet> hugeSet = IHugeEnumSet::Probe(set);
        Boolean changed = FALSE;
        for (Int32 i = 0; i < mBits->GetLength(); i++) {
            Int64 oldBits = (*mBits)[i];
            AutoPtr< ArrayOf<Int64> > bits;
            hugeSet->GetBits((ArrayOf<Int64>**)&bits);
            Int64 newBits = oldBits & (*bits)[i];
            if (oldBits != newBits) {
                (*mBits)[i] = newBits;
                mSize += Elastos::Core::Math::BitCount(newBits) - Elastos::Core::Math::BitCount(oldBits);
                changed = TRUE;
            }
        }
        *modified = changed;
        return NOERROR;
    }
    return EnumSet::RetainAll(collection, modified);
}

ECode HugeEnumSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean res = FALSE;
    return RetainAll(collection, &res);
}

ECode HugeEnumSet::SetRange(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end)
{
    Int32 startOrdinal;
    (IEnum::Probe(start))->Ordinal(&startOrdinal);
    Int32 startIndex = startOrdinal / BIT_IN_LONG;
    Int32 startInBits = startOrdinal % BIT_IN_LONG;

    Int32 endOrdinal;
    (IEnum::Probe(end))->Ordinal(&endOrdinal);
    Int32 endIndex = endOrdinal / BIT_IN_LONG;
    Int32 endInBits = endOrdinal % BIT_IN_LONG;

    if (startIndex == endIndex) {
        Int64 range = (-1L >> (BIT_IN_LONG -(endInBits - startInBits + 1))) << startInBits;//-1l >>>
        mSize -= Elastos::Core::Math::BitCount((*mBits)[startIndex]);
        (*mBits)[startIndex] |= range;
        mSize += Elastos::Core::Math::BitCount((*mBits)[startIndex]);

    } else {
        Int64 range = (-1L >> startInBits) << startInBits;//-1l >>>
        mSize -= Elastos::Core::Math::BitCount((*mBits)[startIndex]);
        (*mBits)[startIndex] |= range;
        mSize += Elastos::Core::Math::BitCount((*mBits)[startIndex]);

        // endInBits + 1 is the number of consecutive ones.
        // 63 - endInBits is the following zeros of the right most one.
        range = -1L >> (BIT_IN_LONG - (endInBits + 1));//-1l >>>
        mSize -= Elastos::Core::Math::BitCount((*mBits)[endIndex]);
        (*mBits)[endIndex] |= range;
        mSize += Elastos::Core::Math::BitCount((*mBits)[endIndex]);
        for (Int32 i = (startIndex + 1); i <= (endIndex - 1); i++) {
            mSize -= Elastos::Core::Math::BitCount((*mBits)[i]);
            (*mBits)[i] = -1L;
            mSize += Elastos::Core::Math::BitCount((*mBits)[i]);
        }
    }
    return NOERROR;
}

ECode HugeEnumSet::GetEnums(
    /*[out, callee]*/ ArrayOf<IInterface*>** arrays)
{
    VALIDATE_NOT_NULL(arrays);
    *arrays = mEnums;
    REFCOUNT_ADD(*arrays);
    return NOERROR;
}

ECode HugeEnumSet::GetBits(
    /*[out, callee]*/ ArrayOf<Int64>** arrays)
{
    VALIDATE_NOT_NULL(arrays);
    *arrays = mBits;
    REFCOUNT_ADD(*arrays);
    return NOERROR;
}

ECode HugeEnumSet::Add(
    /* [in] */ IInterface* object)
{
    return EnumSet::Add(object);
}

ECode HugeEnumSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return EnumSet::AddAll(collection);
}

ECode HugeEnumSet::GetHashCode(
    /* [out] */ Int32* result)
{
    return EnumSet::GetHashCode(result);
}

ECode HugeEnumSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return EnumSet::IsEmpty(result);
}

ECode HugeEnumSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return EnumSet::ToArray(array);
}

ECode HugeEnumSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inarray,
    /* [out, callee] */ ArrayOf<IInterface*>** outarray)
{
    return EnumSet::ToArray(inarray, outarray);
}

} // namespace Utility
} // namespace Elastos