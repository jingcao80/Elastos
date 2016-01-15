
#include "CCallerInfo.h"
#include "CTelephonyManager.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::IICountryDetector;
using Elastos::Droid::Location::IICountryDetector;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CCallerInfo::TAG("CCallerInfo");
const Boolean CCallerInfo::VDBG = FALSE;

ECode CCallerInfo::constructor()
{
    //TODO: Move all the basic initialization here?
    mIsEmergency = FALSE;
    mIsVoiceMail = FALSE;
    return NOERROR;
}

ECode CCallerInfo::IsEmergencyNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsEmergency;
    return NOERROR;
}

ECode CCallerInfo::IsVoiceMailNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsVoiceMail;
    return NOERROR;
}

ECode CCallerInfo::UpdateGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& fallbackNumber)
{
    String number = mPhoneNumber.IsEmpty() ? fallbackNumber : mPhoneNumber;
    GetGeoDescription(context, number, &mGeoDescription);
    return NOERROR;
}

ECode CCallerInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // Warning: never check in this file with VERBOSE_DEBUG = true
    // because that will result in PII in the system log.
    const Boolean VERBOSE_DEBUG = FALSE;

    if (VERBOSE_DEBUG) {
        StringBuilder sb;
        sb.Append("super.toString() +  { ");
        sb.Append("\nname: ");
        sb.Append(mName);
        sb.Append("\nphoneNumber: ");
        sb.Append(mPhoneNumber);
        sb.Append("\nnormalizedNumber: ");
        sb.Append(mNormalizedNumber);
        sb.Append("\ngeoDescription: ");
        sb.Append(mGeoDescription);
        sb.Append("\ncnapName: ");
        sb.Append(mCnapName);
        sb.Append("\nnumberPresentation: ");
        sb.Append(mNumberPresentation);
        sb.Append("\nnamePresentation: ");
        sb.Append(mNamePresentation);
        sb.Append("\ncontactExits: ");
        sb.AppendBoolean(mContactExists);
        sb.Append("\nphoneLabel: ");
        sb.Append(mPhoneLabel);
        sb.Append("\nnumberType: ");
        sb.Append(mNumberType);
        sb.Append("\nnumberLabel: ");
        sb.Append(mNumberLabel);
        sb.Append("\nphotoResource: ");
        sb.Append(mPhotoResource);
        sb.Append("\nperson_id: ");
        sb.Append(mPerson_id);
        sb.Append("\nneedUpdate: ");
        sb.AppendBoolean(mNeedUpdate);
        sb.Append("\ncontactRefUri: ");
        String uri;
        mContactRefUri->ToString(&uri);
        sb.Append(uri);
        sb.Append("\ncontactRingtoneUri: ");
        sb.Append(uri);
        sb.Append("\nshouldSendToVoicemail: ");
        sb.AppendBoolean(mShouldSendToVoicemail);
        sb.Append("\ncachedPhoto: ");
        sb.Append("mCachedPhoto");
        sb.Append("\nisCachedPhotoCurrent: ");
        sb.AppendBoolean(mIsCachedPhotoCurrent);
        sb.Append("\nemergency: ");
        sb.AppendBoolean(mIsEmergency);
        sb.Append("\nvoicemail ");
        sb.AppendBoolean(mIsVoiceMail);
        sb.Append("\ncontactExists ");
        sb.AppendBoolean(mContactExists);
        sb.Append(" }");
        *str = sb.ToString();
    } else {
        StringBuilder sb;
        sb.Append("super.toString() +  { ");
        sb.Append("name ");
        sb.Append((mName.IsNull()) ? "null" : "non-null");
        sb.Append(", phoneNumber ");
        sb.Append((mPhoneNumber.IsNull()) ? "null" : "non-null");
        sb.Append(" }");
        *str = sb.ToString();
    }

    return NOERROR;
}

ECode CCallerInfo::MarkAsEmergency(
    /* [in] */ IContext* context,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    context->GetString(R::string::emergency_call_dialog_number_for_display, &mPhoneNumber);
    mPhotoResource = R::drawable::picture_emergency;
    mIsEmergency = TRUE;
    *callerInfo = this;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfo::MarkAsVoiceMail(
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    mIsVoiceMail = TRUE;

    //try {
        String voiceMailLabel;
        AutoPtr<ITelephonyManager> tm;
        CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
        if(FAILED(tm->GetVoiceMailAlphaTag(&voiceMailLabel))) {
            Slogger::E(TAG, "Cannot access VoiceMail.");
        }

        mPhoneNumber = voiceMailLabel;
    /*} catch (SecurityException se) {
        // Should never happen: if this process does not have
        // permission to retrieve VM tag, it should not have
        // permission to retrieve VM number and would not call
        // this method.
        // Leave phoneNumber untouched.
        Log.e(TAG, "Cannot access VoiceMail.", se);
    }*/
    // TODO: There is no voicemail picture?
    // FIXME: FIND ANOTHER ICON
    // photoResource = android.R.drawable.badge_voicemail;
    *callerInfo = this;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CCallerInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CCallerInfo::SetPhoneNumber(
    /* [in] */ const String& phoneNumber)
{
    mPhoneNumber = phoneNumber;
    return NOERROR;
}

ECode CCallerInfo::GetPhoneNumber(
    /* [out] */ String* phoneNumber)
{
    VALIDATE_NOT_NULL(phoneNumber);
    *phoneNumber = mPhoneNumber;
    return NOERROR;
}

ECode CCallerInfo::SetNormalizedNumber(
    /* [in] */ const String& normalizedNumber)
{
    mNormalizedNumber = normalizedNumber;
    return NOERROR;
}

ECode CCallerInfo::GetNormalizedNumber(
    /* [out] */ String* normalizedNumber)
{
    VALIDATE_NOT_NULL(normalizedNumber);
    *normalizedNumber = mNormalizedNumber;
    return NOERROR;
}

ECode CCallerInfo::SetGeoDescription(
    /* [in] */ const String& geoDescription)
{
    mGeoDescription = geoDescription;
    return NOERROR;
}

ECode CCallerInfo::GetGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ String* geoDescription)
{
    VALIDATE_NOT_NULL(geoDescription);
    if (VDBG) Slogger::V(TAG, "getGeoDescription(' %s ')...", number.string());

    if (number.IsEmpty()) {
        *geoDescription = String(NULL);
        return NOERROR;
    }

    /*PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    PhoneNumberOfflineGeocoder geocoder = PhoneNumberOfflineGeocoder.getInstance();

    Locale locale = context.getResources().getConfiguration().locale;
    String countryIso = getCurrentCountryIso(context, locale);
    PhoneNumber pn = null;
    try {
       if (VDBG) Slogger::V(TAG, "parsing '" + number
                       + "' for countryIso '" + countryIso + "'...");
       pn = util.parse(number, countryIso);
       if (VDBG) Slogger::V(TAG, "- parsed number: " + pn);
    } catch (NumberParseException e) {
       Log.w(TAG, "getGeoDescription: NumberParseException for incoming number '" + number + "'");
    }

    if (pn != null) {
       String description = geocoder.getDescriptionForNumber(pn, locale);
       if (VDBG) Slogger::V(TAG, "- got description: '" + description + "'");
       return description;
    } else {
       return null;
    }*/
    assert(0);
    return NOERROR;
}

ECode CCallerInfo::SetCnapName(
    /* [in] */ const String& cnapName)
{
    mCnapName = cnapName;
    return NOERROR;
}

ECode CCallerInfo::GetCnapName(
    /* [out] */ String* cnapName)
{
    VALIDATE_NOT_NULL(cnapName);
    *cnapName = mCnapName;
    return NOERROR;
}

ECode CCallerInfo::SetNumberPresentation(
    /* [in] */ Int32 numberPresentation)
{
    mNumberPresentation = numberPresentation;
    return NOERROR;
}

ECode CCallerInfo::GetNumberPresentation(
    /* [out] */ Int32* numberPresentation)
{
    VALIDATE_NOT_NULL(numberPresentation);
    *numberPresentation = mNumberPresentation;
    return NOERROR;
}

ECode CCallerInfo::SetNamePresentation(
    /* [in] */ Int32 namePresentation)
{
    mNamePresentation = namePresentation;
    return NOERROR;
}

ECode CCallerInfo::GetNamePresentation(
    /* [out] */ Int32* namePresentation)
{
    VALIDATE_NOT_NULL(namePresentation);
    *namePresentation = mNamePresentation;
    return NOERROR;
}

ECode CCallerInfo::GetContactExists(
    /* [out] */ Boolean* contactExists)
{
    VALIDATE_NOT_NULL(contactExists);
    *contactExists = mContactExists;
    return NOERROR;
}

ECode CCallerInfo::SetContactExists(
    /* [in] */ Boolean contactExists)
{
    mContactExists = contactExists;
    return NOERROR;
}

ECode CCallerInfo::SetPhoneLabel(
    /* [in] */ const String& phoneLabel)
{
    mPhoneLabel = phoneLabel;
    return NOERROR;
}

ECode CCallerInfo::GetPhoneLabel(
    /* [out] */ String* phoneLabel)
{
    VALIDATE_NOT_NULL(phoneLabel);
    *phoneLabel = mPhoneLabel;
    return NOERROR;
}

ECode CCallerInfo::SetNumberType(
    /* [in] */ Int32 numberType)
{
    mNumberType = numberType;
    return NOERROR;
}

ECode CCallerInfo::GetNumberType(
    /* [out] */ Int32* numberType)
{
    VALIDATE_NOT_NULL(numberType);
    *numberType = mNumberType;
    return NOERROR;
}

ECode CCallerInfo::SetNumberLabel(
    /* [in] */ const String& numberLabel)
{
    mNumberLabel = numberLabel;
    return NOERROR;
}

ECode CCallerInfo::GetNumberLabel(
    /* [out] */ String* numberLabel)
{
    VALIDATE_NOT_NULL(numberLabel);
    *numberLabel = mNumberLabel;
    return NOERROR;
}

ECode CCallerInfo::SetPhotoResource(
    /* [in] */ Int32 photoResource)
{
    mPhotoResource = photoResource;
    return NOERROR;
}

ECode CCallerInfo::GetPhotoResource(
    /* [out] */ Int32* photoResource)
{
    VALIDATE_NOT_NULL(photoResource);
    *photoResource = mPhotoResource;
    return NOERROR;
}

ECode CCallerInfo::SetPerson_id(
    /* [in] */ Int64 person_id)
{
    mPerson_id = person_id;
    return NOERROR;
}

ECode CCallerInfo::GetPerson_id(
    /* [out] */ Int64* person_id)
{
    VALIDATE_NOT_NULL(person_id);
    *person_id = mPerson_id;
    return NOERROR;
}

ECode CCallerInfo::GetNeedUpdate(
    /* [out] */ Boolean* needUpdate)
{
    VALIDATE_NOT_NULL(needUpdate);
    *needUpdate = mNeedUpdate;
    return NOERROR;
}

ECode CCallerInfo::SetNeedUpdate(
    /* [in] */ Boolean needUpdate)
{
    mNeedUpdate = needUpdate;
    return NOERROR;
}

ECode CCallerInfo::SetContactRefUri(
    /* [in] */ IUri* contactRefUri)
{
    mContactRefUri = contactRefUri;
    return NOERROR;
}

ECode CCallerInfo::GetContactRefUri(
    /* [out] */ IUri** contactRefUri)
{
    VALIDATE_NOT_NULL(contactRefUri);
    *contactRefUri = mContactRefUri;
    REFCOUNT_ADD(*contactRefUri);
    return NOERROR;
}

ECode CCallerInfo::SetContactRingtoneUri(
    /* [in] */ IUri* contactRingtoneUri)
{
    mContactRingtoneUri = contactRingtoneUri;
    return NOERROR;
}

ECode CCallerInfo::GetContactRingtoneUri(
    /* [out] */ IUri** contactRingtoneUri)
{
    VALIDATE_NOT_NULL(contactRingtoneUri);
    *contactRingtoneUri = mContactRingtoneUri;
    REFCOUNT_ADD(*contactRingtoneUri);
    return NOERROR;
}

ECode CCallerInfo::GetShouldSendToVoicemail(
    /* [out] */ Boolean* shouldSendToVoicemail)
{
    VALIDATE_NOT_NULL(shouldSendToVoicemail);
    *shouldSendToVoicemail = mShouldSendToVoicemail;
    return NOERROR;
}

ECode CCallerInfo::SetShouldSendToVoicemail(
    /* [in] */ Boolean shouldSendToVoicemail)
{
    mShouldSendToVoicemail = shouldSendToVoicemail;
    return NOERROR;
}

ECode CCallerInfo::SetCachedPhoto(
    /* [in] */ IDrawable* cachedPhoto)
{
    mCachedPhoto = cachedPhoto;
    return NOERROR;
}

ECode CCallerInfo::GetCachedPhoto(
    /* [out] */ IDrawable** cachedPhoto)
{
    VALIDATE_NOT_NULL(cachedPhoto);
    *cachedPhoto = mCachedPhoto;
    REFCOUNT_ADD(*cachedPhoto);
    return NOERROR;
}

ECode CCallerInfo::SetCachedPhotoIcon(
    /* [in] */ IBitmap* cachedPhotoIcon)
{
    mCachedPhotoIcon = cachedPhotoIcon;
    return NOERROR;
}

ECode CCallerInfo::GetCachedPhotoIcon(
    /* [out] */ IBitmap** cachedPhotoIcon)
{
    VALIDATE_NOT_NULL(cachedPhotoIcon);
    *cachedPhotoIcon = mCachedPhotoIcon;
    REFCOUNT_ADD(*cachedPhotoIcon);
    return NOERROR;
}

ECode CCallerInfo::GetIsCachedPhotoCurrent(
    /* [out] */ Boolean* isCachedPhotoCurrent)
{
    VALIDATE_NOT_NULL(isCachedPhotoCurrent);
    *isCachedPhotoCurrent = mIsCachedPhotoCurrent;
    return NOERROR;
}

ECode CCallerInfo::SetIsCachedPhotoCurrent(
    /* [in] */ Boolean isCachedPhotoCurrent)
{
    mIsCachedPhotoCurrent = isCachedPhotoCurrent;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos