#ifndef __ELASTOS_APPS_DIALER_CPHONECALLDETAILS_H__
#define __ELASTOS_APPS_DIALER_CPHONECALLDETAILS_H__

#include "_Elastos_Apps_Dialer_CPhoneCallDetails.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Apps {
namespace Dialer {

CarClass(CPhoneCallDetails)
    , public Object
    , public IPhoneCallDetails
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 numberPresentation,
        /* [in] */ ICharSequence* formattedNumber,
        /* [in] */ const String& countryIso,
        /* [in] */ const String& geocode,
        /* [in] */ ArrayOf<Int32>* callTypes,
        /* [in] */ Int64 date,
        /* [in] */ Int64 duration);

    CARAPI constructor(
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
        /* [in] */ const String& transcription);

    CARAPI constructor(
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
        /* [in] */ const String& transcription);

    CARAPI GetNumber(
        /* [out] */ ICharSequence** number);

    CARAPI GetNumberPresentation(
        /* [out] */ Int32* numberPresentation);

    CARAPI GetFormattedNumber(
        /* [out] */ ICharSequence** formattedNumber);

    CARAPI GetCountryIso(
        /* [out] */ String* countryIso);

    CARAPI GetGeocode(
        /* [out] */ String* geocode);

    CARAPI GetCallTypes(
        /* [out, callee] */ ArrayOf<Int32>** callTypes);

    CARAPI GetDate(
        /* [out] */ Int64* date);

    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    CARAPI GetName(
        /* [out] */ ICharSequence** name);

    CARAPI GetNumberType(
        /* [out] */ Int32* numberType);

    CARAPI GetNumberLabel(
        /* [out] */ ICharSequence** numberLabel);

    CARAPI GetContactUri(
        /* [out] */ IUri** contactUri);

    CARAPI GetPhotoUri(
        /* [out] */ IUri** photoUri);

    CARAPI GetSourceType(
        /* [out] */ Int32* sourceType);

    CARAPI GetAccountLabel(
        /* [out] */ String* accountLabel);

    CARAPI GetAccountIcon(
        /* [out] */ IDrawable** accountIcon);

    CARAPI GetFeatures(
        /* [out] */ Int32* features);

    CARAPI GetDataUsage(
        /* [out] */ Int64* dataUsage);

    CARAPI GetTranscription(
        /* [out] */ String* transcription);

public:
        /** The number of the other party involved in the call. */
    AutoPtr<ICharSequence> mNumber;
    /** The number presenting rules set by the network, e.g., {@link Calls#PRESENTATION_ALLOWED} */
    Int32 mNumberPresentation;
    /** The formatted version of {@link #number}. */
    AutoPtr<ICharSequence> mFormattedNumber;
    /** The country corresponding with the phone number. */
    String mCountryIso;
    /** The geocoded location for the phone number. */
    String mGeocode;
    /**
     * The type of calls, as defined in the call log table, e.g., {@link Calls#INCOMING_TYPE}.
     * <p>
     * There might be multiple types if this represents a set of entries grouped together.
     */
    AutoPtr<ArrayOf<Int32> > mCallTypes;
    /** The date of the call, in milliseconds since the epoch. */
    Int64 mDate;
    /** The duration of the call in milliseconds, or 0 for missed calls. */
    Int64 mDuration;
    /** The name of the contact, or the empty string. */
    AutoPtr<ICharSequence> mName;
    /** The type of phone, e.g., {@link Phone#TYPE_HOME}, 0 if not available. */
    Int32 mNumberType;
    /** The custom label associated with the phone number in the contact, or the empty string. */
    AutoPtr<ICharSequence> mNumberLabel;
    /** The URI of the contact associated with this phone call. */
    AutoPtr<IUri> mContactUri;
    /**
     * The photo URI of the picture of the contact that is associated with this phone call or
     * null if there is none.
     * <p>
     * This is meant to store the high-res photo only.
     */
    AutoPtr<IUri> mPhotoUri;
    /**
     * The source type of the contact associated with this call.
     */
    Int32 mSourceType;
    /**
     * The unique identifier for the account associated with the call.
     */
    String mAccountLabel;
    /**
     * The icon for the account associated with the call.
     */
    AutoPtr<IDrawable> mAccountIcon;
    /**
     * Features applicable to this call.
     */
    Int32 mFeatures;
    /**
     * Total data usage for this call.
     */
    AutoPtr<IInteger64> mDataUsage;
    /**
     * Voicemail transcription
     */
    String mTranscription;
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CPHONECALLDETAILS_H__
