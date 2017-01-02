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

#ifndef  __ELASTOS_DROID_PHONE_COUTGOINGCALLBROADCASTER_H__
#define  __ELASTOS_DROID_PHONE_COUTGOINGCALLBROADCASTER_H__

#include "_Elastos_Droid_TeleService_Phone_COutgoingCallBroadcaster.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * OutgoingCallBroadcaster receives CALL and CALL_PRIVILEGED Intents, and broadcasts the
 * ACTION_NEW_OUTGOING_CALL intent. ACTION_NEW_OUTGOING_CALL is an ordered broadcast intent which
 * contains the phone number being dialed. Applications can use this intent to (1) see which numbers
 * are being dialed, (2) redirect a call (change the number being dialed), or (3) prevent a call
 * from being placed.
 *
 * After the other applications have had a chance to see the
 * ACTION_NEW_OUTGOING_CALL intent, it finally reaches the
 * {@link OutgoingCallReceiver}, which passes the (possibly modified)
 * intent on to the {@link SipCallOptionHandler}, which will
 * ultimately start the call using the CallController.placeCall() API.
 *
 * Calls where no number is present (like for a CDMA "empty flash" or a nonexistent voicemail
 * number) are exempt from being broadcast.
 * Calls to emergency numbers are still broadcast for informative purposes. The call is placed
 * prior to sending ACTION_NEW_OUTGOING_CALL and cannot be redirected nor prevented.
 */
CarClass(COutgoingCallBroadcaster)
    , public Activity
    , public IOutgoingCallBroadcaster
    , public IDialogInterfaceOnClickListener
    , public IDialogInterfaceOnCancelListener
{
public:
    /**
     * OutgoingCallReceiver finishes NEW_OUTGOING_CALL broadcasts, starting
     * the InCallScreen if the broadcast has not been canceled, possibly with
     * a modified phone number and optional provider info (uri + package name + remote views.)
     */
    class OutgoingCallReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("COutgoingCallBroadcaster::OutgoingCallReceiver")

        CARAPI constructor(
            /* [in] */ IOutgoingCallBroadcaster* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        /**
         * Handes receipt of ordered new_outgoing_call intent. Verifies that the return from the
         * ordered intent is valid.
         * @return true if the call is being attempted; false if we are canceling the call.
         */
        CARAPI DoReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent,
            /* [out] */ Boolean* result);

    private:
        static const String TAG;

        COutgoingCallBroadcaster* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("COutgoingCallBroadcaster::MyHandler")

        MyHandler(
            /* [in] */ COutgoingCallBroadcaster* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        COutgoingCallBroadcaster* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    COutgoingCallBroadcaster();

    CARAPI constructor();

    /** DialogInterface.OnClickListener implementation */
    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 id);

    /** DialogInterface.OnCancelListener implementation */
    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    /**
     * Implement onConfigurationChanged() purely for debugging purposes,
     * to make sure that the android:configChanges element in our manifest
     * is working properly.
     */
    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

protected:
    /**
     * This method is the single point of entry for the CALL intent, which is used (by built-in
     * apps like Contacts / Dialer, as well as 3rd-party apps) to initiate an outgoing voice call.
     *
     *
     */
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** dialog);

private:
    /**
     * Starts the delayed finish() of OutgoingCallBroadcaster in order to give the UI
     * some time to start up.
     */
    CARAPI_(void) StartDelayedFinish();

    /**
     * Launch the SipCallOptionHandler, which is the next step(*) in the
     * outgoing-call sequence after the outgoing call broadcast is
     * complete.
     *
     * (*) We now know exactly what phone number we need to dial, so the next
     *     step is for the SipCallOptionHandler to decide which Phone type (SIP
     *     or PSTN) should be used.  (Depending on the user's preferences, this
     *     decision may also involve popping up a dialog to ask the user to
     *     choose what type of call this should be.)
     *
     * @param context used for the startActivity() call
     *
     * @param intent the intent from the previous step of the outgoing-call
     *   sequence.  Normally this will be the NEW_OUTGOING_CALL broadcast intent
     *   that came in to the OutgoingCallReceiver, although it can also be the
     *   original ACTION_CALL intent that started the whole sequence (in cases
     *   where we don't do the NEW_OUTGOING_CALL broadcast at all, like for
     *   emergency numbers or SIP addresses).
     *
     * @param uri the data URI from the original CALL intent, presumably either
     *   a tel: or sip: URI.  For tel: URIs, note that the scheme-specific part
     *   does *not* necessarily have separators and keypad letters stripped (so
     *   we might see URIs like "tel:(650)%20555-1234" or "tel:1-800-GOOG-411"
     *   here.)
     *
     * @param number the actual number (or SIP address) to dial.  This is
     *   guaranteed to be either a PSTN phone number with separators stripped
     *   out and keypad letters converted to digits (like "16505551234"), or a
     *   raw SIP address (like "user@example.com").
     */
    CARAPI_(void) StartSipCallOptionHandler(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [in] */ IUri* uri,
        /* [in] */ const String& number);

    /**
     * Interprets a given Intent and starts something relevant to the Intent.
     *
     * This method will handle three kinds of actions:
     *
     * - CALL (action for usual outgoing voice calls)
     * - CALL_PRIVILEGED (can come from built-in apps like contacts / voice dialer / bluetooth)
     * - CALL_EMERGENCY (from the EmergencyDialer that's reachable from the lockscreen.)
     *
     * The exact behavior depends on the intent's data:
     *
     * - The most typical is a tel: URI, which we handle by starting the
     *   NEW_OUTGOING_CALL broadcast.  That broadcast eventually triggers
     *   the sequence OutgoingCallReceiver -> SipCallOptionHandler ->
     *   InCallScreen.
     *
     * - Or, with a sip: URI we skip the NEW_OUTGOING_CALL broadcast and
     *   go directly to SipCallOptionHandler, which then leads to the
     *   InCallScreen.
     *
     * - voicemail: URIs take the same path as regular tel: URIs.
     *
     * Other special cases:
     *
     * - Outgoing calls are totally disallowed on non-voice-capable
     *   devices (see handleNonVoiceCapable()).
     *
     * - A CALL intent with the EXTRA_SEND_EMPTY_FLASH extra (and
     *   presumably no data at all) means "send an empty flash" (which
     *   is only meaningful on CDMA devices while a call is already
     *   active.)
     *
     */
    CARAPI_(void) ProcessIntent(
        /* [in] */ IIntent* intent);

    /**
     * Handle the specified CALL or CALL_* intent on a non-voice-capable
     * device.
     *
     * This method may launch a different intent (if there's some useful
     * alternative action to take), or otherwise display an error dialog,
     * and in either case will finish() the current activity when done.
     */
    CARAPI_(void) HandleNonVoiceCapable(
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const Boolean DBG;
    // Do not check in with VDBG = true, since that may write PII to the system log.
    static const Boolean VDBG;

    /** Required permission for any app that wants to consume ACTION_NEW_OUTGOING_CALL. */
    static const String PERMISSION;

    // Dialog IDs
    static const Int32 DIALOG_NOT_VOICE_CAPABLE;

    /** Note message codes < 100 are reserved for the PhoneApp. */
    static const Int32 EVENT_OUTGOING_CALL_TIMEOUT;
    static const Int32 EVENT_DELAYED_FINISH;

    static const Int32 OUTGOING_CALL_TIMEOUT_THRESHOLD; // msec
    static const Int32 DELAYED_FINISH_TIME; // msec

    /**
     * ProgressBar object with "spinner" style, which will be shown if we take more than
     * {@link #EVENT_OUTGOING_CALL_TIMEOUT} msec to handle the incoming Intent.
     */
    AutoPtr<IProgressBar> mWaitingSpinner;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_COUTGOINGCALLBROADCASTER_H__