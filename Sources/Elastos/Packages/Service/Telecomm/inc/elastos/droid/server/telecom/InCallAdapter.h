
#ifndef __ELASTOS_DROID_SERVER_TELECOM_INCALLADAPTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_INCALLADAPTER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/InCallController.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telecom::IIInCallAdapter;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Receives call commands and updates from in-call app and passes them through to CallsManager.
 * {@link InCallController} creates an instance of this class and passes it to the in-call app after
 * binding to it. This adapter can receive commands and updates until the in-call app is unbound.
 */
class InCallAdapter
    : public Object
    , public IIInCallAdapter
    , public IBinder
{
private:
    class InCallAdapterHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("InCallAdapter::InCallAdapterHandler")

        InCallAdapterHandler(
            /* [in] */ InCallAdapter* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        InCallAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /** Persists the specified parameters. */
    CARAPI constructor(
        /* [in] */ CallsManager* callsManager,
        /* [in] */ ICallIdMapper* callIdMapper);

    // @Override
    CARAPI AnswerCall(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    /** {@inheritDoc} */
    // @Override
    CARAPI DeflectCall(
        /* [in] */ const String& callId,
        /* [in] */ const String& deflectNumber);

    // @Override
    CARAPI RejectCall(
        /* [in] */ const String& callId,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    // @Override
    CARAPI PlayDtmfTone(
        /* [in] */ const String& callId,
        /* [in] */ Char32 digit);

    // @Override
    CARAPI StopDtmfTone(
        /* [in] */ const String& callId);

    // @Override
    CARAPI PostDialContinue(
        /* [in] */ const String& callId,
        /* [in] */ Boolean proceed);

    // @Override
    CARAPI DisconnectCall(
        /* [in] */ const String& callId);

    // @Override
    CARAPI HoldCall(
        /* [in] */ const String& callId);

    // @Override
    CARAPI UnholdCall(
        /* [in] */ const String& callId);

    // @Override
    CARAPI PhoneAccountSelected(
        /* [in] */ const String& callId,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    // @Override
    CARAPI Mute(
        /* [in] */ Boolean shouldMute);

    // @Override
    CARAPI SetAudioRoute(
        /* [in] */ Int32 route);

    // @Override
    CARAPI Conference(
        /* [in] */ const String& callId,
        /* [in] */ const String& otherCallId);

    // @Override
    CARAPI SplitFromConference(
        /* [in] */ const String& callId);

    // @Override
    CARAPI MergeConference(
        /* [in] */ const String& callId);

    // @Override
    CARAPI SwapConference(
        /* [in] */ const String& callId);

    // @Override
    CARAPI TurnOnProximitySensor();

    // @Override
    CARAPI TurnOffProximitySensor(
        /* [in] */ Boolean screenOnImmediately);

    // @Override
    CARAPI SwitchToOtherActiveSub(
        /* [in] */ const String& sub,
        /* [in] */ Boolean retainLch);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static const Int32 MSG_ANSWER_CALL;

    static const Int32 MSG_REJECT_CALL;

    static const Int32 MSG_PLAY_DTMF_TONE;

    static const Int32 MSG_STOP_DTMF_TONE;

    static const Int32 MSG_POST_DIAL_CONTINUE;

    static const Int32 MSG_DISCONNECT_CALL;

    static const Int32 MSG_HOLD_CALL;

    static const Int32 MSG_UNHOLD_CALL;

    static const Int32 MSG_MUTE;

    static const Int32 MSG_SET_AUDIO_ROUTE;

    static const Int32 MSG_CONFERENCE;

    static const Int32 MSG_SPLIT_FROM_CONFERENCE;

    static const Int32 MSG_SWAP_WITH_BACKGROUND_CALL;

    static const Int32 MSG_PHONE_ACCOUNT_SELECTED;

    static const Int32 MSG_TURN_ON_PROXIMITY_SENSOR;

    static const Int32 MSG_TURN_OFF_PROXIMITY_SENSOR;

    static const Int32 MSG_MERGE_CONFERENCE;

    static const Int32 MSG_SWAP_CONFERENCE;

    static const Int32 MSG_SET_SWITCH_SUBSCRIPTION;

    static const Int32 MSG_DEFLECT_CALL;

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ICallIdMapper> mCallIdMapper;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_INCALLADAPTER_H__
