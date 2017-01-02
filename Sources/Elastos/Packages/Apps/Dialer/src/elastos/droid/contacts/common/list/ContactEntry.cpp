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

#include "elastos/droid/contacts/common/list/ContactEntry.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IContactsContractPinnedPositions;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const AutoPtr<ContactEntry> ContactEntry::BLANK_ENTRY = new ContactEntry();

ContactEntry::ContactEntry()
    : mId(0)
    , mPinned(IContactsContractPinnedPositions::UNPINNED)
    , mIsFavorite(FALSE)
    , mIsDefaultNumber(FALSE)
{}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
