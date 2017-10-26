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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_NEWOUTGOINGCALLINTENTBROADCASTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_NEWOUTGOINGCALLINTENTBROADCASTER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecom::IGatewayInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * OutgoingCallIntentBroadcaster receives CALL and CALL_PRIVILEGED Intents, and broadcasts the
 * ACTION_NEW_OUTGOING_CALL intent. ACTION_NEW_OUTGOING_CALL is an ordered broadcast intent which
 * contains the phone number being dialed. Applications can use this intent to (1) see which numbers
 * are being dialed, (2) redirect a call (change the number being dialed), or (3) prevent a call
 * from being placed.
 *
 * After the other applications have had a chance to see the ACTION_NEW_OUTGOING_CALL intent, it
 * finally reaches the {@link NewOutgoingCallBroadcastIntentReceiver}.
 *
 * Calls where no number is present (like for a CDMA "empty flash" or a nonexistent voicemail
 * number) are exempt from being broadcast.
 *
 * Calls to emergency numbers are still broadcast for informative purposes. The call is placed
 * prior to sending ACTION_NEW_OUTGOING_CALL and cannot be redirected nor prevented.
 */
class NewOutgoingCallIntentBroadcaster
    : public Object
{
public:
    /**
     * Processes the result of the outgoing call broadcast intent, and performs callbacks to
     * the OutgoingCallIntentBroadcasterListener as necessary.
     */
    class NewOutgoingCallBroadcastIntentReceiver
        : public BroadcastReceiver
        , public INewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver
    {
    public:
        TO_STRING_IMPL("NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AutoPtr<NewOutgoingCallIntentBroadcaster> mHost;
    };

public:
    NewOutgoingCallIntentBroadcaster();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CallsManager* callsManager,
        /* [in] */ ICall* call,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean isDefaultPhoneApp);

    /**
     * Processes the supplied intent and starts the outgoing call broadcast process relevant to the
     * intent.
     *
     * This method will handle three kinds of actions:
     *
     * - CALL (intent launched by all third party dialers)
     * - CALL_PRIVILEGED (intent launched by system apps e.g. system Dialer, voice Dialer)
     * - CALL_EMERGENCY (intent launched by lock screen emergency dialer)
     *
     * @return {@link CallActivity#OUTGOING_CALL_SUCCEEDED} if the call succeeded, and an
     *         appropriate {@link DisconnectCause} if the call did not, describing why it failed.
     */
    CARAPI ProcessIntent(
        /* [out] */ Int32* result);

    /**
     * Copy all the expected extras set when a 3rd party gateway provider is to be used, from the
     * source intent to the destination one.
     *
     * @param src Intent which may contain the provider's extras.
     * @param dst Intent where a copy of the extras will be added if applicable.
     */
    CARAPI CheckAndCopyProviderExtras(
        /* [in] */ IIntent* src,
        /* [in] */ IIntent* dst);

    /**
     * Extracts gateway provider information from a provided intent..
     *
     * @param intent to extract gateway provider information from.
     * @param trueHandle The actual call handle that the user is trying to dial
     * @return GatewayInfo object containing extracted gateway provider information as well as
     *     the actual handle the user is trying to dial.
     */
    static CARAPI GetGateWayInfoFromIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IUri* trueHandle,
        /* [out] */ IGatewayInfo** result);

private:
    CARAPI ProcessAddParticipant(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& handle,
        /* [out] */ Boolean* result);

    /**
     * Sends a new outgoing call ordered broadcast so that third party apps can cancel the
     * placement of the call or redirect it to a different number.
     *
     * @param originalCallIntent The original call intent.
     * @param number Call number that was stored in the original call intent.
     * @param receiverRequired Whether or not the result from the ordered broadcast should be
     *     processed using a {@link NewOutgoingCallIntentBroadcaster}.
     */
    CARAPI BroadcastIntent(
        /* [in] */ IIntent* originalCallIntent,
        /* [in] */ const String& number,
        /* [in] */ Boolean receiverRequired);

    /**
     * Check if valid gateway provider information is stored as extras in the intent
     *
     * @param intent to check for
     * @return true if the intent has all the gateway information extras needed.
     */
    CARAPI HasGatewayProviderExtras(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    static CARAPI GetGatewayUriFromString(
        /* [in] */ const String& gatewayUriString,
        /* [out] */ IUri** result);

    CARAPI LaunchSystemDialer(
        /* [in] */ IUri* handle);

    /**
     * Check whether or not this is an emergency number, in order to enforce the restriction
     * that only the CALL_PRIVILEGED and CALL_EMERGENCY intents are allowed to make emergency
     * calls.
     *
     * To prevent malicious 3rd party apps from making emergency calls by passing in an
     * "invalid" number like "9111234" (that isn't technically an emergency number but might
     * still result in an emergency call with some networks), we use
     * isPotentialLocalEmergencyNumber instead of isLocalEmergencyNumber.
     *
     * @param number number to inspect in order to determine whether or not an emergency number
     * is potentially being dialed
     * @return True if the handle is potentially an emergency number.
     */
    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* result);

    /**
     * Given a call intent and whether or not the number to dial is an emergency number, rewrite
     * the call intent action to an appropriate one.
     *
     * @param intent Intent to rewrite the action for
     * @param isPotentialEmergencyNumber Whether or not the number is potentially an emergency
     * number.
     */
    CARAPI RewriteCallIntentAction(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean isPotentialEmergencyNumber);

public:
    /**
     * Legacy string constants used to retrieve gateway provider extras from intents. These still
     * need to be copied from the source call intent to the destination intent in order to
     * support third party gateway providers that are still using old string constants in
     * Telephony.
     */
    static const String EXTRA_GATEWAY_PROVIDER_PACKAGE;
    static const String EXTRA_GATEWAY_URI;
    static const String EXTRA_GATEWAY_ORIGINAL_URI;

private:
    /** Required permission for any app that wants to consume ACTION_NEW_OUTGOING_CALL. */
    static const String PERMISSION;

    static const String EXTRA_ACTUAL_NUMBER_TO_DIAL;

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<ICall> mCall;

    AutoPtr<IIntent> mIntent;

    AutoPtr<IContext> mContext;

    /*
     * Whether or not the outgoing call intent originated from the default phone application. If
     * so, it will be allowed to make emergency calls, even with the ACTION_CALL intent.
     */
    Boolean mIsDefaultOrSystemPhoneApp;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_NEWOUTGOINGCALLINTENTBROADCASTER_H__
