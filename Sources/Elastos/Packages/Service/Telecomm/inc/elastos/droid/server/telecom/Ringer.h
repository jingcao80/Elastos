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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_RINGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_RINGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/AsyncRingtonePlayer.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include "elastos/droid/server/telecom/InCallTonePlayer.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IVibrator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallAudioManager;
class CallsManager;
/**
 * Controls the ringtone player.
 */
class Ringer
    : public CallsManagerListenerBase
{
public:
    Ringer();

    /** Initializes the Ringer. */
    CARAPI constructor(
        /* [in] */ CallAudioManager* callAudioManager,
        /* [in] */ CallsManager* callsManager,
        /* [in] */ InCallTonePlayer::Factory* playerFactory,
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    // @Override
    CARAPI OnIncomingCallAnswered(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIncomingCallRejected(
        /* [in] */ ICall* call,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    // @Override
    CARAPI OnForegroundCallChanged(
        /* [in] */ ICall* oldForegroundCall,
        /* [in] */ ICall* newForegroundCall);

    /**
     * Silences the ringer for any actively ringing calls.
     */
    CARAPI Silence();

private:
    CARAPI OnRespondedToIncomingCall(
        /* [in] */ ICall* call);

    CARAPI GetTopMostUnansweredCall(
        /* [out] */ ICall** result);

    /**
     * Removes the specified call from the list of unanswered incoming calls and updates the ringer
     * based on the new state of {@link #mRingingCalls}. Safe to call with a call that is not
     * present in the list of incoming calls.
     */
    CARAPI RemoveFromUnansweredCall(
        /* [in] */ ICall* call);

    CARAPI UpdateRinging();

    CARAPI StartRingingOrCallWaiting();

    CARAPI ShouldRingForContact(
        /* [in] */ IUri* contactUri,
        /* [out] */ Boolean* result);

    CARAPI StopRinging();

    CARAPI StopCallWaiting();

    CARAPI ShouldVibrate(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetVibrateWhenRinging(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<ArrayOf<Int64> >) InitVIBRATION_PATTERN();

    static CARAPI_(AutoPtr<IAudioAttributes>) InitVIBRATION_ATTRIBUTES();

private:
    static const AutoPtr<ArrayOf<Int64> > VIBRATION_PATTERN;

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

    /** Indicate that we want the pattern to repeat at the step which turns on vibration. */
    static const Int32 VIBRATION_PATTERN_REPEAT;

    AutoPtr<AsyncRingtonePlayer> mRingtonePlayer;

    /**
     * Used to keep ordering of unanswered incoming calls. There can easily exist multiple incoming
     * calls and explicit ordering is useful for maintaining the proper state of the ringer.
     */
    AutoPtr<IList> mRingingCalls;

    AutoPtr<IWeakReference> mCallAudioManager; // CallAudioManager

    AutoPtr<IWeakReference> mCallsManager; // CallsManager

    AutoPtr<InCallTonePlayer::Factory> mPlayerFactory;

    AutoPtr<IContext> mContext;

    AutoPtr<IVibrator> mVibrator;

    AutoPtr<InCallTonePlayer> mCallWaitingPlayer;

    /**
     * Used to track the status of {@link #mVibrator} in the case of simultaneous incoming calls.
     */
    Boolean mIsVibrating;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_RINGER_H__
