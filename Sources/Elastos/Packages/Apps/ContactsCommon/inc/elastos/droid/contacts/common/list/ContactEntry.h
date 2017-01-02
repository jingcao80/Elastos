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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

class ContactEntry
    : public Object
    , public IContactEntry
{
public:
    CAR_INTERFACE_DECL()

    ContactEntry();

public:
    String mName;
    String mStatus;
    String mPhoneLabel;
    String mPhoneNumber;
    AutoPtr<IUri> mPhotoUri;
    AutoPtr<IUri> mLookupUri;
    String mLookupKey;
    AutoPtr<IDrawable> mPresenceIcon;
    Int64 mId;
    Int32 mPinned; // = PinnedPositions.UNPINNED;
    Boolean mIsFavorite; // = false;
    Boolean mIsDefaultNumber; // = false;

    static const AutoPtr<IContactEntry> BLANK_ENTRY; // = new ContactEntry();
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__