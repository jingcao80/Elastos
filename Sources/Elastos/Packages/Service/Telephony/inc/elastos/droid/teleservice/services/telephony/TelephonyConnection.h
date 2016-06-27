#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONECONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONECONNECTION_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/telecomm/telecom/Connection.h"
#include "elastos/droid/internal/telephony/Connection.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IConnectionPostDialListener;
using Elastos::Droid::Telecomm::Telecom::IAudioState;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Base class for CDMA and GSM connections.
 */
class TelephonyConnection
    : public Elastos::Droid::Telecomm::Telecom::Connection
    , public ITelephonyConnection
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TelephonyConnection::MyHandler")

        MyHandler(
            /* [in] */ TelephonyConnection* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TelephonyConnection* mHost;
    };


    class MyConnectionPostDialListener
        : public Object
        , public IConnectionPostDialListener
    {
    public:
        TO_STRING_IMPL("TelephonyConnection::MyHandler")

        CAR_INTERFACE_DECL()

        MyConnectionPostDialListener(
            /* [in] */ TelephonyConnection* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnPostDialWait();

    private:
        TelephonyConnection* mHost;
    };

    class MyConnectionListener
        : public Elastos::Droid::Internal::Telephony::Connection::ListenerBase
    {
    public:
        TO_STRING_IMPL("TelephonyConnection::MyHandler")

        MyConnectionListener(
            /* [in] */ TelephonyConnection* host)
            : mHost(host)
        {}

        CARAPI OnVideoStateChanged(
            /* [in] */ Int32 videoState);

        CARAPI OnLocalVideoCapabilityChanged(
            /* [in] */ Boolean capable);

        CARAPI OnRemoteVideoCapabilityChanged(
            /* [in] */ Boolean capable);

        CARAPI OnVideoProviderChanged(
            /* [in] */ IConnectionVideoProvider* videoProvider);

        CARAPI OnAudioQualityChanged(
            /* [in] */ Int32 audioQuality);

    private:
        TelephonyConnection* mHost;
    };

public:
    TO_STRING_IMPL("TelephonyConnection");

    CAR_INTERFACE_DECL()

    TelephonyConnection();

    //@Override
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* audioState);

    //@Override
    CARAPI OnStateChanged(
        /* [in] */ Int32 state);

    //@Override
    CARAPI OnDisconnect();

    //@Override
    CARAPI OnSeparate();

    //@Override
    CARAPI OnAbort();

    //@Override
    CARAPI OnHold();

    //@Override
    CARAPI OnUnhold();

    //@Override
    CARAPI OnAnswer(
        /* [in] */ Int32 videoState);

    //@Override
    CARAPI OnReject();

    //@Override
    CARAPI OnPostDialContinue(
        /* [in] */ Boolean proceed);

    CARAPI PerformHold();

    CARAPI PerformUnhold();

    CARAPI PerformConference(
        /* [in] */ ITelephonyConnection* otherConnection);

    CARAPI OnRemovedFromCallService();

    CARAPI SetOriginalConnection(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection);

    CARAPI GetOriginalConnection(
        /* [out] */ Elastos::Droid::Internal::Telephony::IConnection** con);

    CARAPI GetPhone(
        /* [out] */ IPhone** phone);

    CARAPI UpdateState();

    /**
     * Returns the local video capability state for the connection.
     *
     * @return {@code True} if the connection has local video capabilities.
     */
    CARAPI IsLocalVideoCapable(
        /* [out] */ Boolean* result);

    /**
     * Returns the remote video capability state for the connection.
     *
     * @return {@code True} if the connection has remote video capabilities.
     */
    CARAPI IsRemoteVideoCapable(
        /* [out] */ Boolean* result);

    /**
     * Sets whether video capability is present locally.  Used during rebuild of the
     * {@link PhoneCapabilities} to set the video call capabilities.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetLocalVideoCapable(
        /* [in] */ Boolean capable);

    /**
     * Sets whether video capability is present remotely.  Used during rebuild of the
     * {@link PhoneCapabilities} to set the video call capabilities.
     *
     * @param capable {@code True} if video capable.
     */
    CARAPI SetRemoteVideoCapable(
        /* [in] */ Boolean capable);

    /**
     * Sets the current call audio quality.  Used during rebuild of the
     * {@link PhoneCapabilities} to set or unset the {@link PhoneCapabilities#VoLTE} capability.
     *
     * @param audioQuality The audio quality.
     */
    CARAPI SetAudioQuality(
        /* [in] */ Int32 audioQuality);

    /**
     * Obtains the current call audio quality.
     */
    CARAPI GetAudioQuality(
        /* [out] */ Int32* result);

    CARAPI ResetStateForConference();

    CARAPI SetHoldingForConference(
        /* [out] */ Boolean* result);

protected:
    CARAPI constructor(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection);

    virtual CARAPI BuildCallCapabilities(
        /* [out] */ Int32* result) = 0;

    CARAPI UpdateCallCapabilities();

    CARAPI UpdateAddress();

    CARAPI Hangup(
        /* [in] */ Int32 telephonyDisconnectCode);

    CARAPI GetCall(
        /* [out] */ ICall** call);

private:
    CARAPI_(Boolean) HasMultipleTopLevelCalls();

    CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) GetForegroundConnection();

    /**
     * Checks to see the original connection corresponds to an active incoming call. Returns false
     * if there is no such actual call, or if the associated call is not incoming (See
     * {@link Call.State#isRinging}).
     */
    CARAPI_(Boolean) IsValidRingingCall();

    CARAPI_(void) SetActiveInternal();

    CARAPI_(void) Close();

    /**
     * Applies the video capability states to the CallCapabilities bit-mask.
     *
     * @param capabilities The CallCapabilities bit-mask.
     * @return The capabilities with video capabilities applied.
     */
    CARAPI_(Int32) ApplyVideoCapabilities(
        /* [in] */ Int32 capabilities);

    /**
     * Applies the audio capabilities to the {@code CallCapabilities} bit-mask.  A call with high
     * definition audio is considered to have the {@code VoLTE} call capability as VoLTE uses high
     * definition audio.
     *
     * @param callCapabilities The {@code CallCapabilities} bit-mask.
     * @return The capabilities with the audio capabilities applied.
     */
    CARAPI_(Int32) ApplyAudioQualityCapabilities(
        /* [in] */ Int32 callCapabilities);

    /**
     * Applies capabilities specific to conferences termination to the
     * {@code CallCapabilities} bit-mask.
     *
     * @param callCapabilities The {@code CallCapabilities} bit-mask.
     * @return The capabilities with the IMS conference capabilities applied.
     */
    CARAPI_(Int32) ApplyConferenceTerminationCapabilities(
        /* [in] */ Int32 callCapabilities);

    static CARAPI_(AutoPtr<IUri>) GetAddressFromNumber(
        /* [in] */ const String& number);

    /**
     * Applies a capability to a capabilities bit-mask.
     *
     * @param capabilities The capabilities bit-mask.
     * @param capability The capability to apply.
     * @return The capabilities bit-mask with the capability applied.
     */
    CARAPI_(Int32) ApplyCapability(
        /* [in] */ Int32 capabilities,
        /* [in] */ Int32 capability);

    /**
     * Removes a capability from a capabilities bit-mask.
     *
     * @param capabilities The capabilities bit-mask.
     * @param capability The capability to remove.
     * @return The capabilities bit-mask with the capability removed.
     */
    CARAPI_(Int32) RemoveCapability(
        /* [in] */ Int32 capabilities,
        /* [in] */ Int32 capability);

private:
    static const Int32 MSG_PRECISE_CALL_STATE_CHANGED = 1;
    static const Int32 MSG_RINGBACK_TONE = 2;
    static const Int32 MSG_HANDOVER_STATE_CHANGED = 3;
    static const Int32 MSG_DISCONNECT = 4;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IConnectionPostDialListener> mPostDialListener;

    /**
     * Listener for listening to events in the {@link com.android.internal.telephony.Connection}.
     */
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnectionListener> mOriginalConnectionListener;

    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> mOriginalConnection;
    ICallState mOriginalConnectionState;

    /**
     * Determines if the {@link TelephonyConnection} has local video capabilities.
     * This is used when {@link TelephonyConnection#updateCallCapabilities()}} is called,
     * ensuring the appropriate {@link PhoneCapabilities} are set.  Since {@link PhoneCapabilities}
     * can be rebuilt at any time it is necessary to track the video capabilities between rebuild.
     * The {@link PhoneCapabilities} (including video capabilities) are communicated to the telecom
     * layer.
     */
    Boolean mLocalVideoCapable;

    /**
     * Determines if the {@link TelephonyConnection} has remote video capabilities.
     * This is used when {@link TelephonyConnection#updateCallCapabilities()}} is called,
     * ensuring the appropriate {@link PhoneCapabilities} are set.  Since {@link PhoneCapabilities}
     * can be rebuilt at any time it is necessary to track the video capabilities between rebuild.
     * The {@link PhoneCapabilities} (including video capabilities) are communicated to the telecom
     * layer.
     */
    Boolean mRemoteVideoCapable;

    /**
     * Determines the current audio quality for the {@link TelephonyConnection}.
     * This is used when {@link TelephonyConnection#updateCallCapabilities}} is called to indicate
     * whether a call has the {@link android.telecom.CallCapabilities#VoLTE} capability.
     */
    Int32 mAudioQuality;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONECONNECTION_H__