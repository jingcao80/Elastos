#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * Manages a single phone call handled by CDMA.
 */
class CdmaConnection
    : public TelephonyConnection
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

    CdmaConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ IEmergencyTonePlayer* emergencyTonePlayer,
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
    static const Int32 MSG_CALL_WAITING_MISSED;
    static const Int32 MSG_DTMF_SEND_CONFIRMATION;
    static const Int32 TIMEOUT_CALL_WAITING_MILLIS;

    AutoPtr<IHandler> mHandler;

     /**
     * {@code True} if the CDMA connection should allow mute.
     */
    Boolean mAllowMute;
    Boolean mIsOutgoing;
    // Queue of pending short-DTMF characters.
    AutoPtr<IQueue> mDtmfQueue;
    AutoPtr<IEmergencyTonePlayer> mEmergencyTonePlayer;

    // Indicates that the DTMF confirmation from telephony is pending.
    Boolean mDtmfBurstConfirmationPending;
    Boolean mIsCallWaiting;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONNECTION_H__