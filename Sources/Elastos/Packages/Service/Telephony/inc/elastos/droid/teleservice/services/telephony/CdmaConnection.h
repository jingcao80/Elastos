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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/teleservice/services/telephony/TelephonyConnection.h"
#include "elastos/droid/teleservice/services/telephony/EmergencyTonePlayer.h"
#include "elastos/droid/teleservice/services/telephony/EmergencyTonePlayer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Manages a single phone call handled by CDMA.
 */
class CdmaConnection
    : public TelephonyConnection
    , public ICdmaConnection
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CdmaConnection::MyHandler")

        MyHandler(
            /* [in] */ CdmaConnection* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CdmaConnection* mHost;
    };

public:
    TO_STRING_IMPL("CdmaConnection")

    CAR_INTERFACE_DECL();

    CdmaConnection(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection,
        /* [in] */ EmergencyTonePlayer* emergencyTonePlayer,
        /* [in] */ Boolean allowMute,
        /* [in] */ Boolean isOutgoing);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 digit);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnStopDtmfTone();

    //@Override
    CARAPI OnReject();

    using TelephonyConnection::OnAnswer;

    // /@Override
    CARAPI OnAnswer();

    // /@Override
    CARAPI OnStateChanged(
        /* [in] */ Int32 state);

    CARAPI ForceAsDialing(
        /* [in] */ Boolean isDialing);

    CARAPI IsOutgoing(
        /* [out] */ Boolean* result);

    CARAPI IsCallWaiting(
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI BuildCallCapabilities(
        /* [out] */ Int32* result);

private:
    /**
     * We do not get much in the way of confirmation for Cdma call waiting calls. There is no
     * indication that a rejected call succeeded, a call waiting call has stopped. Instead we
     * simulate this for the user. We allow TIMEOUT_CALL_WAITING_MILLIS milliseconds before we
     * assume that the call was missed and reject it ourselves. reject the call automatically.
     */
    CARAPI_(void) StartCallWaitingTimer();

    CARAPI_(void) HangupCallWaiting(
        /* [in] */ Int32 telephonyDisconnectCause);

    /**
     * Read the settings to determine which type of DTMF method this CDMA phone calls.
     */
    CARAPI_(Boolean) UseBurstDtmf();

    CARAPI_(void) SendShortDtmfToNetwork(
        /* [in] */ Char32 digit);

    CARAPI_(void) SendBurstDtmfStringLocked(
        /* [in] */ const String& dtmfString);

    CARAPI_(void) HandleBurstDtmfConfirmation();

    CARAPI_(Boolean) IsEmergency();

private:
    static const Int32 MSG_CALL_WAITING_MISSED = 1;
    static const Int32 MSG_DTMF_SEND_CONFIRMATION = 2;
    static const Int32 TIMEOUT_CALL_WAITING_MILLIS;

    AutoPtr<IHandler> mHandler;

     /**
     * {@code True} if the CDMA connection should allow mute.
     */
    Boolean mAllowMute;
    Boolean mIsOutgoing;
    // Queue of pending short-DTMF characters.
    AutoPtr<IQueue> mDtmfQueue;
    AutoPtr<EmergencyTonePlayer> mEmergencyTonePlayer;

    // Indicates that the DTMF confirmation from telephony is pending.
    Boolean mDtmfBurstConfirmationPending;
    Boolean mIsCallWaiting;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__