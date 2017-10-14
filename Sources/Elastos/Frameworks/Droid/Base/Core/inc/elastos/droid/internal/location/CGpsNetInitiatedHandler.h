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

#ifndef __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNETINITIATEDHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNETINITIATEDHANDLER_H__

#include "_Elastos_Droid_Internal_Location_CGpsNetInitiatedHandler.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/telephony/PhoneStateListener.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::IINetInitiatedListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::PhoneStateListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

/**
 * A GPS Network-initiated Handler class used by LocationManager.
 *
 * {@hide}
 */
CarClass(CGpsNetInitiatedHandler)
    , public Object
    , public IGpsNetInitiatedHandler
{
public:
    class GpsNiNotification
        : public Object
        , public IGpsNiNotification
    {
    public:
        GpsNiNotification();

        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI GetNotificationId(
            /* [out] */ Int32* notifacationId);

        CARAPI SetNotificationId(
            /* [in] */ Int32 notifacationId);

        CARAPI GetNiType(
            /* [out] */ Int32* niType);

        CARAPI SetNiType(
            /* [in] */ Int32 niType);

        CARAPI GetNeedNotify(
            /* [out] */ Boolean* needNotify);

        CARAPI SetNeedNotify(
            /* [in] */ Boolean needNotify);

        CARAPI GetNeedVerify(
            /* [out] */ Boolean* needVerify);

        CARAPI SetNeedVerify(
            /* [in] */ Boolean needVerify);

        CARAPI GetPrivacyOverride(
            /* [out] */ Boolean* privacyOverride);

        CARAPI SetPrivacyOverride(
            /* [in] */ Boolean privacyOverride);

        CARAPI GetTimeOut(
            /* [out] */ Int32* timeout);

        CARAPI SetTimeOut(
            /* [in] */ Int32 timeOut);

        CARAPI GetDefaultResponse(
            /* [out] */ Int32* defaultResponse);

        CARAPI SetDefaultResponse(
            /* [in] */ Int32 defaultResponse);

        CARAPI GetRequestorId(
            /* [out] */ String* requestorId);

        CARAPI SetRequestorId(
            /* [in] */ const String& requestorId);

        CARAPI GetText(
            /* [out] */ String* text);

        CARAPI SetText(
            /* [in] */ const String& text);

        CARAPI GetRequestorIdEncoding(
            /* [out] */ Int32* requestorIdEncoding);

        CARAPI SetRequestorIdEncoding(
            /* [in] */ Int32 requestorIdEncoding);

        CARAPI GetTextEncoding(
            /* [out] */ Int32* textEncoding);

        CARAPI SetTextEncoding(
            /* [in] */ Int32 textEncoding);

        CARAPI GetExtras(
            /* [out] */ IBundle** extras);

        CARAPI SetExtras(
            /* [in] */ IBundle* extras);

    public:
        Int32 mNotificationId;
        Int32 mNiType;
        Boolean mNeedNotify;
        Boolean mNeedVerify;
        Boolean mPrivacyOverride;
        Int32 mTimeout;
        Int32 mDefaultResponse;
        String mRequestorId;
        String mText;
        Int32 mRequestorIdEncoding;
        Int32 mTextEncoding;
        AutoPtr<IBundle> mExtras;
    };

    class GpsNiResponse
        : public Object
        , public IGpsNiResponse
    {
    public:
        GpsNiResponse();

        CAR_INTERFACE_DECL()

        CARAPI constructor();

        /* User response, one of the values in GpsUserResponseType */
        CARAPI GetUserResponse(
            /* [out] */ Int32* userResponse);

        CARAPI SetUserResponse(
            /* [in] */ Int32 userResponse);

        /* Optional extra data to pass with the user response */
        CARAPI GetExtras(
            /* [out] */ IBundle** extras);

        CARAPI SetExtras(
            /* [in] */ IBundle* extras);

    public:
        Int32 mUserResponse;
        AutoPtr<IBundle> mExtras;
    };

private:
    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CGpsNetInitiatedHandler::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ CGpsNetInitiatedHandler* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CGpsNetInitiatedHandler* mHost;
    };

    class MyPhoneStateListener : public PhoneStateListener
    {
    public:
        MyPhoneStateListener(
            /* [in] */ CGpsNetInitiatedHandler* host);

        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

    private:
        CGpsNetInitiatedHandler* mHost;
    };

public:
    CGpsNetInitiatedHandler();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetInitiatedListener* netInitiatedListener,
        /* [in] */ Boolean isSuplEsEnabled);

    CARAPI SetSuplEsEnabled(
        /* [in] */ Boolean isEnabled);

    CARAPI GetSuplEsEnabled(
        /* [out] */ Boolean* res);

    /**
     * Updates Location enabler based on location setting.
     */
    CARAPI UpdateLocationMode();

    /**
     * Checks if user agreed to use location.
     */
    CARAPI GetLocationEnabled(
        /* [out] */ Boolean* res);

    // Note: Currently, there are two mechanisms involved to determine if a
    // phone is in emergency mode:
    // 1. If the user is making an emergency call, this is provided by activly
    //    monitoring the outgoing phone number;
    // 2. If the device is in a emergency callback state, this is provided by
    //    system properties.
    // If either one of above exists, the phone is considered in an emergency
    // mode. Because of this complexity, we need to be careful about how to set
    // and clear the emergency state.
    CARAPI SetInEmergency(
        /* [in] */ Boolean isInEmergency);

    CARAPI GetInEmergency(
        /* [out] */ Boolean* res);

    // Handles NI events from HAL
    CARAPI HandleNiNotification(
        /* [in] */ IGpsNiNotification* notif);

    // Converts a string (or Hex string) to a char array
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) StringToByteArray(
        /* [in] */ const String& original,
        /* [in] */ Boolean isHex);

    /**
     * Unpacks an byte array containing 7-bit packed characters into a String.
     *
     * @param input a 7-bit packed char array
     * @return the unpacked String
     */
    static CARAPI_(String) DecodeGSMPackedString(
        /* [in] */ ArrayOf<Byte>* input);

    static CARAPI_(String) DecodeUTF8String(
        /* [in] */ ArrayOf<Byte>* input);

    static CARAPI_(String) DecodeUCS2String(
        /* [in] */ ArrayOf<Byte>* input);

    // change this to configure dialog display (for verification)
    static CARAPI_(String) GetDialogTitle(
        /* [in] */ IGpsNiNotification* notif,
        /* [in] */ IContext* context);

private:
    // handle NI form HAL when SUPL_ES is disabled.
    CARAPI_(void) HandleNi(
        /* [in] */ GpsNiNotification* notif);

    // handle NI from HAL when the SUPL_ES is enabled
    CARAPI_(void) HandleNiInEs(
        /* [in] */ GpsNiNotification* notif);

    // Sets the NI notification.
    /*synchronized*/
    CARAPI_(void) SetNiNotification(
        /* [in] */ IGpsNiNotification* notif);

    // Opens the notification dialog and waits for user input
    CARAPI_(void) OpenNiDialog(
        /* [in] */ IGpsNiNotification* notif);

    // Construct the intent for bringing up the dialog activity, which shows the
    // notification and takes user input
    CARAPI_(AutoPtr<IIntent>) GetDlgIntent(
        /* [in] */ IGpsNiNotification* notif);

    /** Decode NI string
     *
     * @param original   The text string to be decoded
     * @param isHex      Specifies whether the content of the string has been encoded as a Hex string. Encoding
     *                   a string as Hex can allow zeros inside the coded text.
     * @param coding     Specifies the coding scheme of the string, such as GSM, UTF8, UCS2, etc. This coding scheme
     *                      needs to match those used passed to HAL from the native GPS driver. Decoding is done according
     *                   to the <code> coding </code>, after a Hex string is decoded. Generally, if the
     *                   notification strings don't need further decoding, <code> coding </code> encoding can be
     *                   set to -1, and <code> isHex </code> can be false.
     * @return the decoded string
     */
    static CARAPI_(String) DecodeString(
        /* [in] */ const String& original,
        /* [in] */ Boolean isHex,
        /* [in] */ Int32 coding);

    // change this to configure notification display
    static CARAPI_(String) GetNotifTicker(
        /* [in] */ IGpsNiNotification* notif,
        /* [in] */ IContext* context);

    // change this to configure notification display
    static CARAPI_(String) GetNotifTitle(
        /* [in] */ IGpsNiNotification* notif,
        /* [in] */ IContext* context);

    // change this to configure notification display
    static CARAPI_(String) GetNotifMessage(
        /* [in] */ IGpsNiNotification* notif,
        /* [in] */ IContext* context);

    // change this to configure dialog display (for verification)
    static CARAPI_(String) GetDialogMessage(
        /* [in] */ IGpsNiNotification* notif,
        /* [in] */ IContext* context);

    static CARAPI_(String) GsmAlphabet_Gsm7BitPackedToString(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 lengthSeptets,
        /* [in] */ Int32 numPaddingBits);

private:
    static const String TAG;// = "GpsNetInitiatedHandler";

    static const String sLanguage; // this static value defined in GsmAlphabet.java
    static const String sShift; // this static value defined in GsmAlphabet.java
    static const Byte GSM_EXTENDED_ESCAPE;

    static const Boolean DEBUG;// = TRUE;
    static const Boolean VERBOSE;// = FALSE;

    AutoPtr<IContext> mContext;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<PhoneStateListener> mPhoneStateListener;

    // parent gps location provider
    AutoPtr<ILocationManager> mLocationManager;

    // configuration of notificaiton behavior
    Boolean mPlaySounds;// = false;
    Boolean mPopupImmediately;// = true;

    // read the SUPL_ES form gps.conf
    volatile Boolean mIsSuplEsEnabled;

    // Set to true if the phone is having emergency call.
    volatile Boolean mIsInEmergency;

    // If Location function is enabled.
    volatile Boolean mIsLocationEnabled;

    AutoPtr<IINetInitiatedListener> mNetInitiatedListener;

    // Set to true if string from HAL is encoded as Hex, e.g., "3F0039"
    static Boolean mIsHexInput;// = TRUE;

    AutoPtr<BroadcastReceiver> mBroadcastReciever;

    /**
     * The notification that is shown when a network-initiated notification
     * (and verification) event is received.
     * <p>
     * This is lazily created, so use {@link #setNINotification()}.
     */
    AutoPtr<INotification> mNiNotification;
};

} // namespace Location
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_LOCATION_CGPSNETINITIATEDHANDLER_H__
