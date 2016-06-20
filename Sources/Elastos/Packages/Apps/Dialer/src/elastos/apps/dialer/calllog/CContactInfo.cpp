
#include "elastos/apps/dialer/calllog/ContactInfo.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CContactInfo, Object, IContactInfo);

CAR_OBJECT_IMPL(CContactInfo);

AutoPtr<IContactInfo> createEMPTY()
{
    AutoPtr<IContactInfo> contactInfo;
    CContactInfo::New((IContactInfo**)&contactInfo);
    return contactInfo;
}

AutoPtr<IContactInfo> CContactInfo::EMPTY = createEMPTY();

String CContactInfo::GEOCODE_AS_LABEL("");

CContactInfo::CContactInfo()
    : mSourceType(0)
{}

ECode CContactInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    // Uses only name and contactUri to determine hashcode.
    // This should be sufficient to have a reasonable distribution of hash codes.
    // Moreover, there should be no two people with the same lookupUri.
    Int32 prime = 31;
    Int32 result = 1;
    Int32 hash;
    result = prime * result + ((mLookupUri == NULL) ? 0 : mLookupUri->GetHashCode(&uriHash), hash);
    result = prime * result + ((mName == NULL) ? 0 : mName->GetHashCode(&hash), hash);
    *hashCode = result;

    return NOERROR;
}

ECode CContactInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (this == obj){
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL){
        *result = FALSE;
        return NOERROR;
    }
    if (getClass() != obj.getClass()) {
        *result = FALSE;
        return NOERROR;
    }
    CContactInfo* other = (CContactInfo*)obj;
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
    if (!TextUtils.equals(mObjectId, other->mObjectId)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CContactInfo::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    // TODO: simple implement
    *info = "Elastos::Apps::Dialer::CallLog::CContactInfo";

    // return Objects.toStringHelper(this).add("lookupUri", lookupUri).add("name", name).add(
    //         "type", type).add("label", label).add("number", number).add("formattedNumber",
    //         formattedNumber).add("normalizedNumber", normalizedNumber).add("photoId", photoId)
    //         .add("photoUri", photoUri).add("objectId", objectId).toString();

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
