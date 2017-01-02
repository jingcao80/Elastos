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

#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/widget/AlphabetIndexer.h"
#include "elastos/droid/utility/CSparseInt32Array.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Utility::CSparseInt32Array;

using Elastos::Core::CString;
using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollatorHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

//========================================================================================
//              AlphabetIndexer::
//========================================================================================
CAR_INTERFACE_IMPL_2(AlphabetIndexer, DataSetObserver, ISectionIndexer, IAlphabetIndexer)

AlphabetIndexer::AlphabetIndexer()
    : mColumnIndex(0)
    , mAlphabetLength(0)
    , mAlphaMap(NULL)
{
}

ECode AlphabetIndexer::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 sortedColumnIndex,
    /* [in] */ ICharSequence* alphabet)
{
    DataSetObserver::constructor();

    mDataCursor = cursor;
    mColumnIndex = sortedColumnIndex;
    mAlphabet = alphabet;
    alphabet->GetLength(&mAlphabetLength);
    mAlphabetArray = ArrayOf<String>::Alloc(mAlphabetLength);//new String[mAlphabetLength];
    for (Int32 i = 0; i < mAlphabetLength; i++) {
        AutoPtr<ICharSequence> temp;
        mAlphabet->SubSequence(i, i + 1, (ICharSequence**)&temp);
        temp->ToString(&(*mAlphabetArray)[i]);
    }
    CSparseInt32Array::New(mAlphabetLength, (ISparseInt32Array**)&mAlphaMap);
    if (cursor != NULL) {
        cursor->RegisterDataSetObserver(this);
    }

    // Get a Collator for the current locale for string comparisons.
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
    mCollator->SetStrength(ICollator::PRIMARY);
    return NOERROR;
}

AlphabetIndexer::~AlphabetIndexer()
{
    mAlphaMap = NULL;
}

ECode AlphabetIndexer::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** sections)
{
    VALIDATE_NOT_NULL(sections)

    Int32 length = mAlphabetArray->GetLength();
    AutoPtr<ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<ICharSequence> temp;
        CString::New((*mAlphabetArray)[i], (ICharSequence**)&temp);
        result->Set(i, temp);
    }

    *sections = result;
    REFCOUNT_ADD(*sections)
    return NOERROR;
}

ECode AlphabetIndexer::SetCursor(
    /* [in] */ ICursor* cursor)
{
    if (mDataCursor != NULL) {
        mDataCursor->UnregisterDataSetObserver(this);
    }
    mDataCursor = cursor;
    if (cursor != NULL) {
        mDataCursor->RegisterDataSetObserver(this);
    }
    mAlphaMap->Clear();
    return NOERROR;
}

Int32 AlphabetIndexer::Compare(
    /* [in] */ const String& word,
    /* [in] */ const String& letter)
{
    String firstLetter;
    if (word.IsNullOrEmpty()) {
        firstLetter = String(" ");
    }
    else {
        firstLetter = word.Substring(0, 1);
    }

    Int32 result = 0;
    AutoPtr<ICharSequence> fcs;
    AutoPtr<ICharSequence> lcs;
    CString::New(firstLetter, (ICharSequence**)&fcs);
    CString::New(letter, (ICharSequence**)&lcs);
    mCollator->Compare(fcs, lcs, &result);
    return result;
}

ECode AlphabetIndexer::GetPositionForSection(
    /* [in] */ Int32 sectionIndex,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)

    AutoPtr<ISparseInt32Array> alphaMap = mAlphaMap;
    AutoPtr<ICursor> cursor = mDataCursor;

    if (cursor == NULL || mAlphabet == NULL) {
        *position = 0;
        return NOERROR;
    }

    // Check bounds
    if (sectionIndex <= 0) {
        *position = 0;
        return NOERROR;
    }
    if (sectionIndex >= mAlphabetLength) {
        sectionIndex = mAlphabetLength - 1;
    }

    Int32 savedCursorPos = 0;
    cursor->GetPosition(&savedCursorPos);

    Int32 count = 0;
    cursor->GetCount(&count);
    Int32 start = 0;
    Int32 end = count;
    Int32 pos = 0;

    Char32 letter;
    mAlphabet->GetCharAt(sectionIndex, &letter);
    char cTemp = (char)letter;

    String targetLetter = String(&cTemp, 1);
    Int32 key = letter;
    // Check map
    alphaMap->Get(key, Elastos::Core::Math::INT32_MIN_VALUE, &pos);
    if (Elastos::Core::Math::INT32_MIN_VALUE != pos) {
        // Is it approximate? Using negative value to indicate that it's
        // an approximation and positive value when it is the accurate
        // position.
        if (pos < 0) {
            pos = -pos;
            end = pos;
        }
        else {
            // Not approximate, this is the confirmed start of section, return it
            *position = pos;
            return NOERROR;
        }
    }

    // Do we have the position of the previous section?
    if (sectionIndex > 0) {
        Char32 prevLetter = 0;
        mAlphabet->GetCharAt(sectionIndex - 1, &prevLetter);
        Int32 prevLetterPos = 0;
        alphaMap->Get((Int32)prevLetter, Elastos::Core::Math::INT32_MIN_VALUE, &prevLetterPos);
        if (prevLetterPos != Elastos::Core::Math::INT32_MIN_VALUE) {
            start = Elastos::Core::Math::Abs(prevLetterPos);
        }
    }

    // Now that we have a possibly optimized start and end, let's binary search

    pos = (end + start) / 2;

    Boolean res = FALSE;
    while (pos < end) {
        // Get letter at pos
        cursor->MoveToPosition(pos, &res);
        String curName;
        cursor->GetString(mColumnIndex, &curName);
        if (curName == NULL) {
            if (pos == 0) {
                break;
            }
            else {
                pos--;
                continue;
            }
        }
        Int32 diff = Compare(curName, targetLetter);
        if (diff != 0) {
            // TODO: Commenting out approximation code because it doesn't work for certain
            // lists with custom comparators
            // Enter approximation in hash if a better solution doesn't exist
            // String startingLetter = Character.toString(getFirstLetter(curName));
            // Int32 startingLetterKey = startingLetter.charAt(0);
            // Int32 curPos = alphaMap.get(startingLetterKey, Elastos::Core::Math::INT32_MIN_VALUE);
            // if (curPos == Elastos::Core::Math::INT32_MIN_VALUE || Elastos::Core::Math.abs(curPos) > pos) {
            //     Negative pos indicates that it is an approximation
            //     alphaMap.put(startingLetterKey, -pos);
            // }
            // if (mCollator.compare(startingLetter, targetLetter) < 0) {
            if (diff < 0) {
                start = pos + 1;
                if (start >= count) {
                    pos = count;
                    break;
                }
            }
            else {
                end = pos;
            }
        }
        else {
            // They're the same, but that doesn't mean it's the start
            if (start == pos) {
                // This is it
                break;
            }
            else {
                // Need to go further lower to find the starting row
                end = pos;
            }
        }
        pos = (start + end) / 2;
    }
    alphaMap->Put(key, pos);
    cursor->MoveToPosition(savedCursorPos, &res);
    *position = pos;
    return NOERROR;
}

ECode AlphabetIndexer::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* section)
{
    VALIDATE_NOT_NULL(section)
    Int32 savedCursorPos = 0;
    mDataCursor->GetPosition(&savedCursorPos);
    Boolean res = FALSE;
    mDataCursor->MoveToPosition(position, &res);
    String curName;
    mDataCursor->GetString(mColumnIndex, &curName);
    mDataCursor->MoveToPosition(savedCursorPos, &res);
    // Linear search, as there are only a few items in the section index
    // Could speed this up later if it actually gets used.
    for (Int32 i = 0; i < mAlphabetLength; i++) {
        Char32 letter;
        mAlphabet->GetCharAt(i, &letter);
        char cTemp = (char)letter;
        String targetLetter = String(&cTemp, 1);
        if (Compare(curName, targetLetter) == 0) {
            *section = i;
            return NOERROR;
        }
    }
    *section = 0; // Don't recognize the letter - falls under zero'th section
    return NOERROR;
}

ECode AlphabetIndexer::OnChanged()
{
    DataSetObserver::OnChanged();
    mAlphaMap->Clear();
    return NOERROR;
}

ECode AlphabetIndexer::OnInvalidated()
{
    DataSetObserver::OnInvalidated();
    mAlphaMap->Clear();
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
