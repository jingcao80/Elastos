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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Class to hold contact information
 */
class ContactEntry : public Object
{
public:
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
    Int32 mPinned;
    Boolean mIsFavorite;
    Boolean mIsDefaultNumber;

    static const AutoPtr<ContactEntry> BLANK_ENTRY;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
