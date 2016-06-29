
#include "elastos/apps/dialer/CPhoneCallDetails.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
CAR_INTERFACE_IMPL(CPhoneCallDetails, Object, IPhoneCallDetails);

CAR_OBJECT_IMPL(CPhoneCallDetails);

ECode CPhoneCallDetails::constructor(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 numberPresentation,
    /* [in] */ ICharSequence* formattedNumber,
    /* [in] */ const String& countryIso,
    /* [in] */ const String& geocode,
    /* [in] */ ArrayOf<Int32>* callTypes,
    /* [in] */ Int64 date,
    /* [in] */ Int64 duration)
{
    return constructor(number, numberPresentation, formattedNumber, countryIso,
            geocode, callTypes, date, duration, String(""), 0,
            String(""), NULL, NULL, 0, String(NULL), NULL, 0, 0, String(NULL));
}

ECode CPhoneCallDetails::constructor(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 numberPresentation,
    /* [in] */ ICharSequence* formattedNumber,
    /* [in] */ const String& countryIso,
    /* [in] */ const String& geocode,
    /* [in] */ ArrayOf<Int32>* callTypes,
    /* [in] */ Int64 date,
    /* [in] */ Int64 duration,
    /* [in] */ const String& accountLabel,
    /* [in] */ IDrawable* accountIcon,
    /* [in] */ Int32 features,
    /* [in] */ Int64 dataUsage,
    /* [in] */ const String& transcription)
{
    return constructor(number, numberPresentation, formattedNumber, countryIso,
            geocode, callTypes, date, duration, String(""), 0, String(""), NULL, NULL,
            0, accountLabel, accountIcon, features, dataUsage, transcription);
}

ECode CPhoneCallDetails::constructor(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 numberPresentation,
    /* [in] */ ICharSequence* formattedNumber,
    /* [in] */ const String& countryIso,
    /* [in] */ const String& geocode,
    /* [in] */ ArrayOf<Int32>* callTypes,
    /* [in] */ Int64 date,
    /* [in] */ Int64 duration,
    /* [in] */ ICharSequence* name,
    /* [in] */ Int32 numberType,
    /* [in] */ ICharSequence* numberLabel,
    /* [in] */ IUri* contactUri,
    /* [in] */ IUri* photoUri,
    /* [in] */ Int32 sourceType,
    /* [in] */ const String& accountLabel,
    /* [in] */ IDrawable* accountIcon,
    /* [in] */ Int32 features,
    /* [in] */ Int64 dataUsage,
    /* [in] */ const String& transcription)
{
    number = mNumber;
    numberPresentation = mNumberPresentation;
    formattedNumber = mFormattedNumber;
    countryIso = mCountryIso;
    geocode = mGeocode;
    callTypes = mCallTypes;
    date = mDate;
    duration = mDuration;
    name = mName;
    numberType = mNumberType;
    numberLabel = mNumberLabel;
    contactUri = mContactUri;
    photoUri = mPhotoUri;
    sourceType = mSourceType;
    accountLabel = mAccountLabel;
    accountIcon = mAccountIcon;
    features = mFeatures;
    dataUsage = mDataUsage;
    transcription = mTranscription;

    return NOERROR;
}

ECode CPhoneCallDetails::GetNumber(
    /* [out] */ ICharSequence** number)
{
    VALIDATE_NOT_NULL(number);
    *number = mNumber;
    REFCOUNT_ADD(*number);
    return NOERROR;
}

ECode CPhoneCallDetails::GetNumberPresentation(
    /* [out] */ Int32* numberPresentation)
{
    VALIDATE_NOT_NULL(numberPresentation);
    *numberPresentation = mNumberPresentation;
    return NOERROR;
}

ECode CPhoneCallDetails::GetFormattedNumber(
    /* [out] */ ICharSequence** formattedNumber)
{
    VALIDATE_NOT_NULL(formattedNumber);
    *formattedNumber = mFormattedNumber;
    REFCOUNT_ADD(*formattedNumber);
    return NOERROR;
}

ECode CPhoneCallDetails::GetCountryIso(
    /* [out] */ String* countryIso)
{
    VALIDATE_NOT_NULL(countryIso);
    *countryIso = mCountryIso;
    return NOERROR;
}

ECode CPhoneCallDetails::GetGeocode(
    /* [out] */ String* geocode)
{
    VALIDATE_NOT_NULL(geocode);
    *geocode = mGeocode;
    return NOERROR;
}

ECode CPhoneCallDetails::GetCallTypes(
    /* [out, callee] */ ArrayOf<Int32>** callTypes)
{
    VALIDATE_NOT_NULL(callTypes);
    *callTypes = mCallTypes;
    REFCOUNT_ADD(*callTypes);
    return NOERROR;
}

ECode CPhoneCallDetails::GetDate(
    /* [out] */ Int64* date)
{
    VALIDATE_NOT_NULL(date);
    *date = mDate;
    return NOERROR;
}

ECode CPhoneCallDetails::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode CPhoneCallDetails::GetName(
    /* [out] */ ICharSequence** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode CPhoneCallDetails::GetNumberType(
    /* [out] */ Int32* numberType)
{
    VALIDATE_NOT_NULL(numberType);
    *numberType = mNumberType;
    return NOERROR;
}

ECode CPhoneCallDetails::GetNumberLabel(
    /* [out] */ ICharSequence** numberLabel)
{
    VALIDATE_NOT_NULL(numberLabel);
    *numberLabel = mNumberLabel;
    REFCOUNT_ADD(*numberLabel);
    return NOERROR;
}

ECode CPhoneCallDetails::GetContactUri(
    /* [out] */ IUri** contactUri)
{
    VALIDATE_NOT_NULL(contactUri);
    *contactUri = mContactUri;
    REFCOUNT_ADD(*contactUri);
    return NOERROR;
}

ECode CPhoneCallDetails::GetPhotoUri(
    /* [out] */ IUri** photoUri)
{
    VALIDATE_NOT_NULL(photoUri);
    *photoUri = mPhotoUri;
    REFCOUNT_ADD(*photoUri);
    return NOERROR;
}

ECode CPhoneCallDetails::GetSourceType(
    /* [out] */ Int32* sourceType)
{
    VALIDATE_NOT_NULL(sourceType);
    *sourceType = mSourceType;
    return NOERROR;
}

ECode CPhoneCallDetails::GetAccountLabel(
    /* [out] */ String* accountLabel)
{
    VALIDATE_NOT_NULL(accountLabel);
    *accountLabel = mAccountLabel;
    return NOERROR;
}

ECode CPhoneCallDetails::GetAccountIcon(
    /* [out] */ IDrawable** accountIcon)
{
    VALIDATE_NOT_NULL(accountIcon);
    *accountIcon = mAccountIcon;
    REFCOUNT_ADD(*accountIcon);
    return NOERROR;
}

ECode CPhoneCallDetails::GetFeatures(
    /* [out] */ Int32* features)
{
    VALIDATE_NOT_NULL(features);
    *features = mFeatures;
    return NOERROR;
}

ECode CPhoneCallDetails::GetDataUsage(
    /* [out] */ Int64* dataUsage)
{
    VALIDATE_NOT_NULL(dataUsage);
    *dataUsage = mDataUsage;
    return NOERROR;
}

ECode CPhoneCallDetails::GetTranscription(
    /* [out] */ String* transcription)
{
    VALIDATE_NOT_NULL(transcription);
    *transcription = mTranscription;
    return NOERROR;
}

} // Dialer
} // Apps
} // Elastos
