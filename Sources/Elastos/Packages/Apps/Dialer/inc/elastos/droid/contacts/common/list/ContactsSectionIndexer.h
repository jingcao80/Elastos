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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__

#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A section indexer that is configured with precomputed section titles and
 * their respective counts.
 */
class ContactsSectionIndexer
    : public Object
    , public ISectionIndexer
{
public:
    /**
     * Constructor.
     *
     * @param sections a non-null array
     * @param counts a non-null array of the same size as <code>sections</code>
     */
    ContactsSectionIndexer(
        /* [in] */ ArrayOf<String>* sections,
        /* [in] */ ArrayOf<Int32>* counts);

    CAR_INTERFACE_DECL()

    CARAPI GetSections(
        /* [out, callee] */ ArrayOf<IInterface*>** sections);

    CARAPI GetPositionForSection(
        /* [in] */ Int32 sectionIndex,
        /* [out] */ Int32* position);

    CARAPI GetSectionForPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* section);

    CARAPI_(void) SetProfileHeader(
        /* [in] */ const String& header);

private:
    static const String BLANK_HEADER_STRING;

    AutoPtr<ArrayOf<String> > mSections;
    AutoPtr<ArrayOf<Int32> > mPositions;
    Int32 mCount;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTSSECTIONINDEXER_H__
