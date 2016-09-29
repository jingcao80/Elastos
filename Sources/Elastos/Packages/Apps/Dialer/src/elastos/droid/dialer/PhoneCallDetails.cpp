
#include "elastos/droid/dialer/PhoneCallDetails.h"

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Dialer {

PhoneCallDetails::PhoneCallDetails(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 numberPresentation,
    /* [in] */ ICharSequence* formattedNumber,
    /* [in] */ const String& countryIso,
    /* [in] */ const String& geocode,
    /* [in] */ ArrayOf<Int32>* callTypes,
    /* [in] */ Int64 date,
    /* [in] */ Int64 duration)
    : mNumber(number)
    , mNumberPresentation(numberPresentation)
    , mFormattedNumber(formattedNumber)
    , mCountryIso(countryIso)
    , mGeocode(geocode)
    , mCallTypes(callTypes)
    , mDate(date)
    , mDuration(duration)
    , mNumberType(0)
    , mSourceType(0)
    , mFeatures(0)
    , mDataUsage(0)
{
    CString::New(String(""), (ICharSequence**)&mName);
    CString::New(String(""), (ICharSequence**)&mNumberLabel);
}

PhoneCallDetails::PhoneCallDetails(
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
    : mNumber(number)
    , mNumberPresentation(numberPresentation)
    , mFormattedNumber(formattedNumber)
    , mCountryIso(countryIso)
    , mGeocode(geocode)
    , mCallTypes(callTypes)
    , mDate(date)
    , mDuration(duration)
    , mNumberType(0)
    , mSourceType(0)
    , mAccountLabel(accountLabel)
    , mAccountIcon(accountIcon)
    , mFeatures(features)
    , mDataUsage(dataUsage)
    , mTranscription(transcription)
{
    CString::New(String(""), (ICharSequence**)&mName);
    CString::New(String(""), (ICharSequence**)&mNumberLabel);
}

PhoneCallDetails::PhoneCallDetails(
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
    : mNumber(number)
    , mNumberPresentation(numberPresentation)
    , mFormattedNumber(formattedNumber)
    , mCountryIso(countryIso)
    , mGeocode(geocode)
    , mCallTypes(callTypes)
    , mDate(date)
    , mDuration(duration)
    , mName(name)
    , mNumberType(0)
    , mNumberLabel(numberLabel)
    , mContactUri(contactUri)
    , mPhotoUri(photoUri)
    , mSourceType(0)
    , mAccountLabel(accountLabel)
    , mAccountIcon(accountIcon)
    , mFeatures(features)
    , mDataUsage(dataUsage)
    , mTranscription(transcription)
{}

} // Dialer
} // Droid
} // Elastos