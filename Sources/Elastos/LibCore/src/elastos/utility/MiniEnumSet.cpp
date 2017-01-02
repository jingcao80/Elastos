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

#include "MiniEnumSet.h"
#include "Math.h"

using Elastos::Core::IEnum;

namespace Elastos {
namespace Utility {

/////////////////////////////////////////
//   MiniEnumSet::MiniEnumSetIterator
/////////////////////////////////////////
CAR_INTERFACE_IMPL(MiniEnumSet::MiniEnumSetIterator, Object, IIterator)

MiniEnumSet::MiniEnumSetIterator::MiniEnumSetIterator(
    /* [in] */ IMiniEnumSet* host)
    : mHost(host)
    , mLast(NULL)
{
    Int64 bits;
    mHost->GetBits(&bits);
    mCurrentBits = bits;
    mMask = mCurrentBits & ~mCurrentBits;
}

ECode MiniEnumSet::MiniEnumSetIterator::HasNext(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = mMask != 0;
    return NOERROR;
}

ECode MiniEnumSet::MiniEnumSetIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (mMask == 0) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    Int32 ordinal = Elastos::Core::Math::NumberOfTrailingZeros(mMask);

    AutoPtr<ArrayOf <IInterface*> > enums;
    mHost->GetEnums((ArrayOf <IInterface*>**)&enums);
    mLast = (*enums)[ordinal];

    mCurrentBits &= ~mMask;
    mMask = mCurrentBits & ~mCurrentBits;

    *object = mLast;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode MiniEnumSet::MiniEnumSetIterator::Remove()
{
    if (!mLast) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    (ICollection::Probe(mHost))->Remove(mLast);
    mLast = NULL;
    return NOERROR;
}

/////////////////////////////////////////
//          MiniEnumSet
/////////////////////////////////////////
CAR_INTERFACE_IMPL(MiniEnumSet, EnumSet, IMiniEnumSet)

const Int32 MiniEnumSet::MAX_ELEMENTS = 64;

MiniEnumSet::MiniEnumSet()
    :EnumSet(EIID_IInterface)
    , mEnums(NULL)
    , mSize(0)
    , mBits(0)
{}

MiniEnumSet::MiniEnumSet(
    /* [in] */ const InterfaceID& cls,
    /* [in] */ ArrayOf<IInterface *> * enums)
    : EnumSet(cls)
    , mEnums(enums)
    , mSize(0)
    , mBits(0)
{
}

ECode MiniEnumSet::constructor()
{
    return EnumSet::constructor();
}

ECode MiniEnumSet::constructor(
    /* [in] */ const InterfaceID& cls,
    /* [in] */ ArrayOf<IInterface *> * enums)
{
    EnumSet::constructor(cls);
    mEnums = enums;
    return NOERROR;
}

ECode MiniEnumSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    //elementClass.cast(element); // Called to throw ClassCastException.
    Int64 oldBits = mBits;
    Int32 res;
    (IEnum::Probe(object))->Ordinal(&res);
    Int64 newBits = oldBits | (1L << res);
    if (oldBits != newBits) {
        mBits = newBits;
        mSize++;
        *modified = TRUE;
        return NOERROR;
    }
    *modified = FALSE;
    return NOERROR;
}

ECode MiniEnumSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    Boolean empty;
    if (collection->IsEmpty(&empty), empty) {
        *modified = FALSE;
        return NOERROR;
    }

    if (IEnumSet::Probe(collection)) {
        AutoPtr<IEnumSet> set = IEnumSet::Probe(collection);
        //set.elementClass.asSubclass(elementClass);

        AutoPtr<IMiniEnumSet> miniSet = IMiniEnumSet::Probe(set);
        Int64 oldBits = mBits;
        Int64 bits;
        miniSet->GetBits(&bits);
        Int64 newBits = oldBits | bits;
        mBits = newBits;
        mSize = Elastos::Core::Math::BitCount(newBits);
        *modified = oldBits != newBits;
        return NOERROR;
    }

    return EnumSet::AddAll(collection, modified);
}

ECode MiniEnumSet::Clear()
{
    mBits = 0;
    mSize = 0;
    return NOERROR;
}

ECode MiniEnumSet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode MiniEnumSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new MiniEnumSetIterator(this);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode MiniEnumSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!object || !IsValidType(/*object.getClass()*/EIID_IInterface)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IEnum> element = IEnum::Probe(object);
    Int32 ordinal;
    element->Ordinal(&ordinal);
    *result = (mBits & (1L << ordinal)) != 0;
    return NOERROR;
}

ECode MiniEnumSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean empty;
    if (collection->IsEmpty(&empty), empty) {
        *result = TRUE;
        return NOERROR;
    }

    if (IMiniEnumSet::Probe(collection)) {
        AutoPtr<IMiniEnumSet> set = IMiniEnumSet::Probe(collection);
        Int64 setBits;
        set->GetBits(&setBits);
        *result = IsValidType(/*set.elementClass*/EIID_IInterface) && ((mBits & setBits) == setBits);
        return NOERROR;
    }
    Boolean containsAll;
    *result = !(IEnumSet::Probe(collection)) && (EnumSet::ContainsAll(collection, &containsAll), containsAll);
    return NOERROR;
}

ECode MiniEnumSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    Boolean empty;
    if (collection->IsEmpty(&empty), empty) {
        *modified = FALSE;
        return NOERROR;
    }

    if (IEnumSet::Probe(collection)) {
        AutoPtr<IEnumSet> set = IEnumSet::Probe(collection);

        if (!IsValidType(/*set.elementClass)*/EIID_IInterface)) {
            *modified = FALSE;
            return NOERROR;
        }

        AutoPtr<IMiniEnumSet> miniSet = IMiniEnumSet::Probe(set);
        Int64 oldBits = mBits;
        Int64 bits;
        miniSet->GetBits(&bits);
        Int64 newBits = oldBits & ~bits;
        if (oldBits != newBits) {
            mBits = newBits;
            mSize = Elastos::Core::Math::BitCount(newBits);
            *modified = TRUE;
            return NOERROR;
        }
        *modified = FALSE;
        return NOERROR;
    }
    return EnumSet::RetainAll(collection, modified);
}

ECode MiniEnumSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
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

        AutoPtr<IMiniEnumSet> miniSet = IMiniEnumSet::Probe(set);
        Int64 oldBits = mBits;
        Int64 bits;
        miniSet->GetBits(&bits);
        Int64 newBits = oldBits & bits;

        if (oldBits != newBits) {
            bits = newBits;
            mSize = Elastos::Core::Math::BitCount(newBits);
            *modified = TRUE;
            return NOERROR;
        }
        *modified = FALSE;
        return NOERROR;
    }

    return EnumSet::RetainAll(collection, modified);
}

ECode MiniEnumSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)
    if (!object || !IsValidType(/*object.getClass()*/EIID_IInterface)) {
        *modified = FALSE;
        return NOERROR;
    }

    AutoPtr<IEnum> element = IEnum::Probe(object);
    Int32 ordinal;
    element->Ordinal(&ordinal);
    Int64 oldBits = mBits;
    Int64 newBits = oldBits & ~(1L << ordinal);

    if (oldBits != newBits) {
        mBits = newBits;
        mSize--;
        *modified = TRUE;
        return NOERROR;
    }

    *modified = FALSE;
    return NOERROR;
}

ECode MiniEnumSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!IEnumSet::Probe(object)) {
        return EnumSet::Equals(object, result);
    }
    AutoPtr<IEnumSet> set = IEnumSet::Probe(object);
    if (!IsValidType(/*set.elementClass)*/EIID_IInterface)) {
        Boolean empty;
        *result = mSize == 0 && ((ICollection::Probe(set))->IsEmpty(&empty), empty);
        return NOERROR;
    }
    Int64 bits;
    (IMiniEnumSet::Probe(set))->GetBits(&bits);
    *result = mBits == bits;
    return NOERROR;
}

ECode MiniEnumSet::Complement()
{
    if (mEnums->GetLength() != 0) {
        mBits = ~mBits;
        mBits &= (-1L >> (MAX_ELEMENTS - mEnums->GetLength())); //-1l <<<
        mSize = mEnums->GetLength() - mSize;
    }

    return NOERROR;
}

ECode MiniEnumSet::SetRange(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end)
{
    Int32 eol, sol;
    (IEnum::Probe(start))->Ordinal(&sol);
    (IEnum::Probe(end))->Ordinal(&eol);
    Int32 length = eol - sol + 1;

    Int64 range = (-1L >> (MAX_ELEMENTS - length)) << sol;
    mBits |= range;
    mSize = Elastos::Core::Math::BitCount(mBits);

    return NOERROR;
}

ECode MiniEnumSet::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    assert(0 && "TODO");
    return NOERROR;
}

ECode MiniEnumSet::GetEnums(
    /*[out, callee]*/ ArrayOf<IInterface*>** arrays)
{
    VALIDATE_NOT_NULL(arrays);
    *arrays = mEnums;
    REFCOUNT_ADD(*arrays);
    return NOERROR;
}

ECode MiniEnumSet::GetBits(
    /*[out]*/ Int64* bits)
{
    VALIDATE_NOT_NULL(bits);
    *bits = mBits;
    return NOERROR;
}

ECode MiniEnumSet::Remove(
    /* [in] */ IInterface* object)
{
    Boolean res;
    return Remove(object, &res);
}

ECode MiniEnumSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    Boolean res;
    return RemoveAll(collection, &res);
}

ECode MiniEnumSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    Boolean res;
    return RetainAll(collection, &res);
}

ECode MiniEnumSet::Add(
    /* [in] */ IInterface* object)
{
    return EnumSet::Add(object);
}

ECode MiniEnumSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return EnumSet::AddAll(collection);
}

ECode MiniEnumSet::GetHashCode(
    /* [out] */ Int32* result)
{
    return EnumSet::GetHashCode(result);
}

ECode MiniEnumSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return EnumSet::IsEmpty(result);
}

ECode MiniEnumSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return EnumSet::ToArray(array);
}

ECode MiniEnumSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inarray,
    /* [out, callee] */ ArrayOf<IInterface*>** outarray)
{
    return EnumSet::ToArray(inarray, outarray);
}

} // namespace Utility
} // namespace Elastos