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

#include "ContactEntry.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

AutoPtr<IContactEntry> CreateBLANK_ENTRY()
{
    AutoPtr<IContactEntry> entry;
    CContactEntry::New((IContactEntry**)&entry);
    return entry;
}

const AutoPtr<IContactEntry> BLANK_ENTRY = CreateBLANK_ENTRY();

CAR_INTERFACE_IMPL(ContactEntry, Object, IContactEntry)

ContactEntry::ContactEntry()
    : mPinned(IPinnedPositions::UNPINNED)
    , mIsFavorite(FALSE)
    , mIsDefaultNumber(FALSE)
{}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
