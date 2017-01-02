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

#include "StringCharacterIterator.h"
#include "CStringCharacterIterator.h"

using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_3(StringCharacterIterator, Object, IStringCharacterIterator, ICharacterIterator, ICloneable)

StringCharacterIterator::StringCharacterIterator()
    : mStart(0)
    , mEnd(0)
    , mOffset(0)
{
}

StringCharacterIterator::~StringCharacterIterator()
{
}

ECode StringCharacterIterator::constructor(
    /* [in] */ const String& value)
{
    mString = String(value);
    mStart = mOffset = 0;
    mEnd = mString.GetLength();
    return NOERROR;
}

ECode StringCharacterIterator::constructor(
    /* [in] */ const String& value,
    /* [in] */ Int32 location)
{
    mString = String(value);
    mStart = 0;
    mEnd = mString.GetLength();
    if (location < 0 || location > mEnd) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mOffset = location;
    return NOERROR;
}

ECode StringCharacterIterator::constructor(
    /* [in] */ const String& value,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 location)
{
    mString = value;
    if (start < 0 || end > (Int32)(mString.GetLength()) || start > end
            || location < start || location > end) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    this->mStart = start;
    this->mEnd = end;
    mOffset = location;
    return NOERROR;
}

ECode StringCharacterIterator::Clone(
    /* [out] */ IInterface** copy)
{
    VALIDATE_NOT_NULL(copy)
    *copy = NULL;

    AutoPtr<IStringCharacterIterator> sci;
    CStringCharacterIterator::New(mString, (IStringCharacterIterator**)&sci);
    FAIL_RETURN(CloneImpl(sci));
    *copy = TO_IINTERFACE(sci);
    REFCOUNT_ADD(*copy)
    return NOERROR;
}

ECode StringCharacterIterator::CloneImpl(
    /* [in] */ IStringCharacterIterator* obj)
{
    assert(obj);
    StringCharacterIterator* sci = (StringCharacterIterator*)obj;
    sci->mString = mString;
    sci->mStart = mStart;
    sci->mEnd = mEnd;
    sci->mOffset = mOffset;
    return NOERROR;
}

ECode StringCharacterIterator::GetCurrent(
    /* [out] */ Char32* currentCharacter)
{
    VALIDATE_NOT_NULL(currentCharacter);
    if (mOffset == mEnd) {
        *currentCharacter = ICharacterIterator::DONE;
        return NOERROR;
    }
    *currentCharacter = mString.GetChar(mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IStringCharacterIterator* obj = IStringCharacterIterator::Probe(other);
    if (obj == NULL) return NOERROR;

    StringCharacterIterator* sci = (StringCharacterIterator*)obj;
    return sci->mString.Equals(mString)
        && sci->mStart == mStart
        && sci->mEnd == mEnd
        && sci->mOffset == mOffset;
}

ECode StringCharacterIterator::GetFirst(
    /* [out] */ Char32* firstCharacter)
{
    VALIDATE_NOT_NULL(firstCharacter);
    if (mStart == mEnd) {
        *firstCharacter = ICharacterIterator::DONE;
        return NOERROR;
    }
    mOffset = mStart;
    *firstCharacter = mString.GetChar(mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::GetBeginIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mStart;
    return NOERROR;
}

ECode StringCharacterIterator::GetEndIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mEnd;
    return NOERROR;
}

ECode StringCharacterIterator::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mOffset;
    return NOERROR;
}

ECode StringCharacterIterator::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash =  mString.GetHashCode() + mStart + mEnd + mOffset;
    return NOERROR;
}

ECode StringCharacterIterator::GetLast(
    /* [out] */ Char32* endCharacter)
{
    VALIDATE_NOT_NULL(endCharacter);
    if (mStart == mEnd) {
        *endCharacter = ICharacterIterator::DONE;
        return NOERROR;
    }
    mOffset = mEnd - 1;
    *endCharacter = mString.GetChar(mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::GetNext(
    /* [out] */ Char32* nextCharacter)
{
    VALIDATE_NOT_NULL(nextCharacter);
    if (mOffset >= (mEnd - 1)) {
        mOffset = mEnd;
        *nextCharacter = ICharacterIterator::DONE;
        return NOERROR;
    }
    *nextCharacter = mString.GetChar(++mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::GetPrevious(
    /* [out] */ Char32* previousCharacter)
{
    VALIDATE_NOT_NULL(previousCharacter);
    if (mOffset == mStart) {
        *previousCharacter = ICharacterIterator::DONE;
        return NOERROR;
    }
    *previousCharacter = mString.GetChar(--mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::SetIndex(
    /* [in] */ Int32 location,
    /* [out] */ Char32* index)
{
    VALIDATE_NOT_NULL(index);
    if (location < mStart || location > mEnd) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mOffset = location;
    if (mOffset == mEnd) {
        *index = ICharacterIterator::DONE;
        return NOERROR;
    }
    *index = mString.GetChar(mOffset);
    return NOERROR;
}

ECode StringCharacterIterator::SetText(
    /* [in] */ const String& value)
{
    mString = value;
    mStart = mOffset = 0;
    mEnd = value.GetLength();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
