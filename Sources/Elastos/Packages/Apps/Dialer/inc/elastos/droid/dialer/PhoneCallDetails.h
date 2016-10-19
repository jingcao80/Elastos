
#ifndef __ELASTOS_DROID_DIALER_PHONECALLDETAILS_H__
#define __ELASTOS_DROID_DIALER_PHONECALLDETAILS_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * The details of a phone call to be shown in the UI.
 */
class PhoneCallDetails : public Object
{
public:
    PhoneCallDetails(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 numberPresentation,
        /* [in] */ ICharSequence* formattedNumber,
        /* [in] */ const String& countryIso,
        /* [in] */ const String& geocode,
        /* [in] */ ArrayOf<Int32>* callTypes,
        /* [in] */ Int64 date,
        /* [in] */ Int64 duration);

    PhoneCallDetails(
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

    PhoneCallDetails(
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
    Int64 mDataUsage;
    /**
     * Voicemail transcription
     */
    String mTranscription;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_PHONECALLDETAILS_H__
