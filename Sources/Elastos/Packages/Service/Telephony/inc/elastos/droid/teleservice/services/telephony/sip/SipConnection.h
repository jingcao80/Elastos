#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPCONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPCONNECTION_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/Connection.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telecomm::Telecom::IAudioState;
using Elastos::Droid::Telecomm::Telecom::Connection;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

class SipConnection
    : public Connection
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("SipConnection::MyHandler")

        MyHandler(
            /* [in] */ SipConnection* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        SipConnection* mHost;
    };

public:
    TO_STRING_IMPL("SipConnection")

    SipConnection();

    CARAPI Initialize(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection);

    //@Override
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* state);

    //@Override
    CARAPI OnStateChanged(
        /* [in] */ Int32 state);

    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    //@Override
    CARAPI OnStopDtmfTone();

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

    CARAPI_(AutoPtr<ISipPhone>) GetPhone();

    CARAPI UpdateCallCapabilities(
        /* [in] */ Boolean force);

    CARAPI OnAddedToCallService();

private:
    CARAPI_(AutoPtr<ICall>) GetCall();

    CARAPI_(Boolean) IsValidRingingCall();

    CARAPI_(void) UpdateState(
        /* [in] */ Boolean force);

    CARAPI_(Int32) BuildCallCapabilities();

    /**
     * Updates the handle on this connection based on the original connection.
     */
    CARAPI_(void) UpdateAddress();

    /**
     * Determines the address for an incoming number.
     *
     * @param number The incoming number.
     * @return The Uri representing the number.
     */
    static CARAPI_(AutoPtr<IUri>) GetAddressFromNumber(
        /* [in] */ const String& number);

    CARAPI_(void) Close();

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    static const Int32 MSG_PRECISE_CALL_STATE_CHANGED = 1;

    AutoPtr<IHandler> mHandler;

    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> mOriginalConnection;
    ICallState mOriginalConnectionState;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPCONNECTION_H__