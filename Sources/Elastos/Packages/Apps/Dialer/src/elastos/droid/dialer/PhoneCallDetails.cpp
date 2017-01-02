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