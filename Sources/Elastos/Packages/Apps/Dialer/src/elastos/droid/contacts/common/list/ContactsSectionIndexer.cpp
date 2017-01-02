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

#include "elastos/droid/contacts/common/list/ContactsSectionIndexer.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::EIID_ISectionIndexer;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const String ContactsSectionIndexer::BLANK_HEADER_STRING(" ");

ContactsSectionIndexer::ContactsSectionIndexer(
    /* [in] */ ArrayOf<String>* sections,
    /* [in] */ ArrayOf<Int32>* counts)
{
    if (sections == NULL || counts == NULL) {
        assert(0);
        // throw new NullPointerException();
    }

    if (sections->GetLength() != counts->GetLength()) {
        Logger::E("ContactsSectionIndexer", "The sections and counts arrays must have the same length");
        assert(0);
    }

    // TODO process sections/counts based on current locale and/or specific section titles

    mSections = sections;
    mPositions = ArrayOf<Int32>::Alloc(counts->GetLength());
    Int32 position = 0;
    for (Int32 i = 0; i < counts->GetLength(); i++) {
        if (TextUtils::IsEmpty((*mSections)[i])) {
            (*mSections)[i] = BLANK_HEADER_STRING;
        }
        else if (!(*mSections)[i].Equals(BLANK_HEADER_STRING)) {
            (*mSections)[i] = (*mSections)[i].Trim();
        }

        (*mPositions)[i] = position;
        position += (*counts)[i];
    }
    mCount = position;
}

CAR_INTERFACE_IMPL(ContactsSectionIndexer, Object, ISectionIndexer)

ECode ContactsSectionIndexer::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** sections)
{
    VALIDATE_NOT_NULL(sections)
    AutoPtr<ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(mSections->GetLength());
    for (Int32 i = 0; i < mSections->GetLength(); ++i) {
        AutoPtr<ICharSequence> cs;
        CString::New((*mSections)[i], (ICharSequence**)&cs);
        temp->Set(i, cs);
    }
    *sections = temp;
    REFCOUNT_ADD(*sections)
    return NOERROR;
}

ECode ContactsSectionIndexer::GetPositionForSection(
    /* [in] */ Int32 section,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    if (section < 0 || section >= mSections->GetLength()) {
        *position = -1;
        return NOERROR;
    }

    *position = (*mPositions)[section];
    return NOERROR;
}

ECode ContactsSectionIndexer::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* section)
{
    VALIDATE_NOT_NULL(section)
    if (position < 0 || position >= mCount) {
        *section = -1;
        return NOERROR;
    }

    Int32 index;
    Arrays::BinarySearch(mPositions, position, &index);

    /*
     * Consider this example: section positions are 0, 3, 5; the supplied
     * position is 4. The section corresponding to position 4 starts at
     * position 3, so the expected return value is 1. Binary search will not
     * find 4 in the array and thus will return -insertPosition-1, i.e. -3.
     * To get from that number to the expected value of 1 we need to negate
     * and subtract 2.
     */
    *section = index >= 0 ? index : -index - 2;
    return NOERROR;
}

void ContactsSectionIndexer::SetProfileHeader(
    /* [in] */ const String& header)
{
    if (mSections != NULL) {
        // Don't do anything if the header is already set properly.
        if (mSections->GetLength() > 0 && header.Equals((*mSections)[0])) {
            return;
        }

        // Since the section indexer isn't aware of the profile at the top, we need to add a
        // special section at the top for it and shift everything else down.
        AutoPtr<ArrayOf<String> > tempSections = ArrayOf<String>::Alloc(mSections->GetLength() + 1);
        AutoPtr<ArrayOf<Int32> > tempPositions = ArrayOf<Int32>::Alloc(mPositions->GetLength() + 1);
        (*tempSections)[0] = header;
        (*tempPositions)[0] = 0;
        for (Int32 i = 1; i <= mPositions->GetLength(); i++) {
            (*tempSections)[i] = (*mSections)[i - 1];
            (*tempPositions)[i] = (*mPositions)[i - 1] + 1;
        }
        mSections = tempSections;
        mPositions = tempPositions;
        mCount++;
    }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
