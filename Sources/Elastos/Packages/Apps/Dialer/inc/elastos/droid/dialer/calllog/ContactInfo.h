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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFO_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFO_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Information for a contact as needed by the Call Log.
 */
class ContactInfo
    : public Object
    , public IContactInfo
{
public:
    CAR_INTERFACE_DECL()

    ContactInfo();

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

public:
    static const AutoPtr<ContactInfo> EMPTY;

    static const String GEOCODE_AS_LABEL;

    AutoPtr<IUri> mLookupUri;
    String mLookupKey;
    String mName;
    Int32 mType;
    String mLabel;
    String mNumber;
    String mFormattedNumber;
    String mNormalizedNumber;
    /** The photo for the contact, if available. */
    Int64 mPhotoId;
    /** The high-res photo for the contact, if available. */
    AutoPtr<IUri> mPhotoUri;
    Boolean mIsBadData;
    String mObjectId;

    Int32 mSourceType;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFO_H__
