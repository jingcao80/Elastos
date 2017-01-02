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

#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/teleservice/phone/CallLogger.h"
#include "elastos/droid/teleservice/phone/CallStateMonitor.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Phone app module that listens for phone state changes and various other
 * events from the telephony layer, and triggers any resulting UI behavior
 * (like starting the Incoming Call UI, playing in-call tones,
 * updating notifications, writing call log entries, etc.)
 */
class CallNotifier
    : public Handler
{
private:
    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("CallNotifier::MyPhoneStateListener")

        CAR_INTERFACE_DECL()

        MyPhoneStateListener(
            /* [in] */ CallNotifier* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnMessageWaitingIndicatorChanged(
            /* [in] */ Boolean mwi);

        //@Override
        CARAPI OnCallForwardingIndicatorChanged(
            /* [in] */ Boolean cfi);

    private:
        CallNotifier* mHost;
    };

    /**
     * Helper class to play tones through the earpiece (or speaker / BT)
     * during a call, using the ToneGenerator.
     *
     * To use, just instantiate a new InCallTonePlayer
     * (passing in the TONE_* constant for the tone you want)
     * and start() it.
     *
     * When we're done playing the tone, if the phone is idle at that
     * point, we'll reset the audio routing and speaker state.
     * (That means that for tones that get played *after* a call
     * disconnects, like "busy" or "congestion" or "call ended", you
     * should NOT call resetAudioStateAfterDisconnect() yourself.
     * Instead, just start the InCallTonePlayer, which will automatically
     * defer the resetAudioStateAfterDisconnect() call until the tone
     * finishes playing.)
     */
    class InCallTonePlayer
        : public Thread
    {
    public:
        TO_STRING_IMPL("CallNotifier::InCallTonePlayer")

        InCallTonePlayer(
            /* [in] */ CallNotifier* host,
            /* [in] */ Int32 toneId);

        //@Override
        CARAPI Run();

        CARAPI StopTone();

    public:
        // The possible tones we can play.
        static const Int32 TONE_NONE;
        static const Int32 TONE_CALL_WAITING;
        static const Int32 TONE_BUSY;
        static const Int32 TONE_CONGESTION;
        static const Int32 TONE_CALL_ENDED;
        static const Int32 TONE_VOICE_PRIVACY;
        static const Int32 TONE_REORDER;
        static const Int32 TONE_INTERCEPT;
        static const Int32 TONE_CDMA_DROP;
        static const Int32 TONE_OUT_OF_SERVICE;
        static const Int32 TONE_REDIAL;
        static const Int32 TONE_OTA_CALL_END;
        static const Int32 TONE_UNOBTAINABLE_NUMBER;

    private:
        CallNotifier* mHost;

        Int32 mToneId;
        Int32 mState;

        // The tone volume relative to other sounds in the stream
        static const Int32 TONE_RELATIVE_VOLUME_EMERGENCY;
        static const Int32 TONE_RELATIVE_VOLUME_HIPRI;
        static const Int32 TONE_RELATIVE_VOLUME_LOPRI;

        // Buffer time (in msec) to add on to tone timeout value.
        // Needed mainly when the timeout value for a tone is the
        // exact duration of the tone itself.
        static const Int32 TONE_TIMEOUT_BUFFER;

        // The tone state
        static const Int32 TONE_OFF;
        static const Int32 TONE_ON;
        static const Int32 TONE_STOPPED;

    };

    /**
     * Helper class to play SignalInfo tones using the ToneGenerator.
     *
     * To use, just instantiate a new SignalInfoTonePlayer
     * (passing in the ToneID constant for the tone you want)
     * and start() it.
     */
    class SignalInfoTonePlayer
        : public Thread
    {
    public:
        TO_STRING_IMPL("CallNotifier::SignalInfoTonePlayer")

        SignalInfoTonePlayer(
            /* [in] */ CallNotifier* host,
            /* [in] */ Int32 toneId);

        //@Override
        CARAPI Run();

    private:
        CallNotifier* mHost;

        Int32 mToneId;
    };

    class MyBluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        TO_STRING_IMPL("CallNotifier::MyBluetoothProfileServiceListener")

        CAR_INTERFACE_DECL()

        MyBluetoothProfileServiceListener(
            /* [in] */ CallNotifier* host)
            : mHost(host)
        {}

        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        CallNotifier* mHost;
    };

public:
    TO_STRING_IMPL("CallNotifier")

    /**
     * Initialize the singleton CallNotifier instance.
     * This is only done once, at startup, from PhoneApp.onCreate().
     */
    static CARAPI_(AutoPtr<CallNotifier>) Init(
        /* [in] */ IPhoneGlobals* app,
        /* [in] */ IPhone* phone,
        /* [in] */ CallLogger* callLogger,
        /* [in] */ CallStateMonitor* callStateMonitor,
        /* [in] */ IBluetoothManager* bluetoothManager);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI UpdateCallNotifierRegistrationsAfterRadioTechnologyChange();

    /**
     * Posts a delayed PHONE_MWI_CHANGED event, to schedule a "retry" for a
     * failed NotificationMgr.updateMwi() call.
     */
    CARAPI SendMwiChangedDelayed(
        /* [in] */ Int64 delayMillis);

    /**
     * Stops a SignalInfo tone in the following condition
     * 1 - On receiving a New Ringing Call
     * 2 - On disconnecting a call
     * 3 - On answering a Call Waiting Call
     */
    CARAPI StopSignalInfoTone();

    /**
     * Return the private variable mPreviousCdmaCallState.
     */
    CARAPI GetPreviousCdmaCallState(
        /* [out] */ ICallState* state);

    /**
     * Return the private variable mVoicePrivacyState.
     */
    CARAPI GetVoicePrivacyState(
        /* [out] */ Boolean* result);

    /**
     * Return the private variable mIsCdmaRedialCall.
     */
    CARAPI GetIsCdmaRedialCall(
        /* [out] */ Boolean* result);

private:
    /** Private constructor; @see init() */
    CallNotifier(
        /* [in] */ IPhoneGlobals* app,
        /* [in] */ IPhone* phone,
        /* [in] */ CallLogger* callLogger,
        /* [in] */ CallStateMonitor* callStateMonitor,
        /* [in] */ IBluetoothManager* bluetoothManager);

    CARAPI_(void) CreateSignalInfoToneGenerator();

    /**
     * Handles a "new ringing connection" event from the telephony layer.
     */
    CARAPI_(void) OnNewRingingConnection(
        /* [in] */ IAsyncResult* r);

    /**
     * Determines whether or not we're allowed to present incoming calls to the
     * user, based on the capabilities and/or current state of the device.
     *
     * If this method returns true, that means we should immediately reject the
     * current incoming call, without even indicating to the user that an
     * incoming call occurred.
     *
     * (We only reject incoming calls in a few cases, like during an OTASP call
     * when we can't interrupt the user, or if the device hasn't completed the
     * SetupWizard yet.  We also don't allow incoming calls on non-voice-capable
     * devices.  But note that we *always* allow incoming calls while in ECM.)
     *
     * @return true if we're *not* allowed to present an incoming call to
     * the user.
     */
    CARAPI_(Boolean) IgnoreAllIncomingCalls(
        /* [in] */ IPhone* phone);

    CARAPI_(void) OnUnknownConnectionAppeared(
        /* [in] */ IAsyncResult* r);

    /**
     * Updates the phone UI in response to phone state changes.
     *
     * Watch out: certain state changes are actually handled by their own
     * specific methods:
     *   - see onNewRingingConnection() for new incoming calls
     *   - see onDisconnect() for calls being hung up or disconnected
     */
    CARAPI_(void) OnPhoneStateChanged(
        /* [in] */ IAsyncResult* r);

    CARAPI_(void) OnDisconnect(
        /* [in] */ IAsyncResult* r);

    /**
     * Resets the audio mode and speaker state when a call ends.
     */
    CARAPI_(void) ResetAudioStateAfterDisconnect();

    CARAPI_(void) OnMwiChanged(
        /* [in] */ Boolean visible);

    CARAPI_(void) OnCfiChanged(
        /* [in] */ Boolean visible);

    /**
     * Displays a notification when the phone receives a DisplayInfo record.
     */
    CARAPI_(void) OnDisplayInfo(
        /* [in] */ IAsyncResult* r);

    /**
     * Plays a tone when the phone receives a SignalInfo record.
     */
    CARAPI_(void) OnSignalInfo(
        /* [in] */ IAsyncResult* r);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;

    // Time to display the  DisplayInfo Record sent by CDMA network
    static const Int32 DISPLAYINFO_NOTIFICATION_TIME; // msec

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

    /** The singleton instance. */
    static AutoPtr<CallNotifier> sInstance;

    // values used to track the query state
    static const Int32 CALLERINFO_QUERY_READY;
    static const Int32 CALLERINFO_QUERYING;

    // the state of the CallerInfo Query.
    Int32 mCallerInfoQueryState;

    // object used to synchronize access to mCallerInfoQueryState
    Object mCallerInfoQueryStateGuard;

    // Events generated internally:
    static const Int32 PHONE_MWI_CHANGED;
    static const Int32 DISPLAYINFO_NOTIFICATION_DONE;
    static const Int32 UPDATE_IN_CALL_NOTIFICATION;

    AutoPtr<IPhoneGlobals> mApplication;
    AutoPtr<ICallManager> mCM;
    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;
    AutoPtr<CallLogger> mCallLogger;

    // ToneGenerator instance for playing SignalInfo tones
    AutoPtr<IToneGenerator> mSignalInfoToneGenerator;

    // The tone volume relative to other sounds in the stream SignalInfo
    static const Int32 TONE_RELATIVE_VOLUME_SIGNALINFO;

    ICallState mPreviousCdmaCallState;
    Boolean mVoicePrivacyState;
    Boolean mIsCdmaRedialCall;

    // Cached AudioManager
    AutoPtr<IAudioManager> mAudioManager;

    AutoPtr<IBluetoothManager> mBluetoothManager;

    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    AutoPtr<IBluetoothProfileServiceListener> mBluetoothProfileServiceListener;
    static Object THIS;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__