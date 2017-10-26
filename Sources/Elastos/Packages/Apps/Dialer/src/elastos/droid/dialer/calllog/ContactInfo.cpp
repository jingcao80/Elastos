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

#include "elastos/droid/dialer/calllog/ContactInfo.h"
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Contacts::Common::Util::UriUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(ContactInfo, Object, IContactInfo)

static AutoPtr<ContactInfo> InitEMPTY()
{
    AutoPtr<ContactInfo> contactInfo = new ContactInfo();
    return contactInfo;
}
const AutoPtr<ContactInfo> ContactInfo::EMPTY = InitEMPTY();
const String ContactInfo::GEOCODE_AS_LABEL("");

ContactInfo::ContactInfo()
    : mType(0)
    , mPhotoId(0)
    , mIsBadData(FALSE)
    , mSourceType(0)
{}

ECode ContactInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    // Uses only name and contactUri to determine hashcode.
    // This should be sufficient to have a reasonable distribution of hash codes.
    // Moreover, there should be no two people with the same lookupUri.
    Int32 prime = 31;
    Int32 result = 1;
    Int32 hash;
    result = prime * result +
            (mLookupUri == NULL) ? 0 : Object::GetHashCode(mLookupUri);
    result = prime * result + (mName.IsNull() ? 0 : mName.GetHashCode());
    *hashCode = result;

    return NOERROR;
}

ECode ContactInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ContactInfo> other = (ContactInfo*)IContactInfo::Probe(obj);
    if (this == other.Get()){
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL){
        *result = FALSE;
        return NOERROR;
    }
    if (IContactInfo::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (!UriUtils::AreEqual(mLookupUri, other->mLookupUri)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mName, other->mName)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mType != other->mType) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mLabel, other->mLabel)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mNumber, other->mNumber)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mFormattedNumber, other->mFormattedNumber)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mNormalizedNumber, other->mNormalizedNumber)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mPhotoId != other->mPhotoId) {
        *result = FALSE;
        return NOERROR;
    }
    if (!UriUtils::AreEqual(mPhotoUri, other->mPhotoUri)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mObjectId, other->mObjectId)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode ContactInfo::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    StringBuilder sb("lookupUri");
    sb.Append(mLookupUri);
    sb.Append("name");
    sb.Append(mName);
    sb.Append("type");
    sb.Append(mType);
    sb.Append("label");
    sb.Append(mLabel);
    sb.Append("number");
    sb.Append(mNumber);
    sb.Append("formattedNumber");
    sb.Append(mFormattedNumber);
    sb.Append("normalizedNumber");
    sb.Append(mNormalizedNumber);
    sb.Append("photoId");
    sb.Append(mPhotoId);
    sb.Append("photoUri");
    sb.Append(mPhotoUri);
    sb.Append("objectId");
    sb.Append(mObjectId);
    *info = sb.ToString();
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
