#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_PSTNINCOMINGCALLNOTIFIER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_PSTNINCOMINGCALLNOTIFIER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Listens to incoming-call events from the associated phone object and notifies Telecom upon each
 * occurence. One instance of these exists for each of the telephony-based call services.
 */
class PstnIncomingCallNotifier
    : public Object
    , public IPstnIncomingCallNotifier
{
public:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PstnIncomingCallNotifier::TtyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IPstnIncomingCallNotifier* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PstnIncomingCallNotifier* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PstnIncomingCallNotifier::MyHandler")

        MyHandler(
            /* [in] */ PstnIncomingCallNotifier* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PstnIncomingCallNotifier* mHost;
    };

public:
    TO_STRING_IMPL("PstnIncomingCallNotifier")

    CAR_INTERFACE_DECL()

    /**
     * Persists the specified parameters and starts listening to phone events.
     *
     * @param phoneProxy The phone object for listening to incoming calls.
     */
    PstnIncomingCallNotifier(
        /* [in] */ IPhoneProxy* phoneProxy);

    CARAPI Teardown();

private:
    /**
     * Register for notifications from the base phone.
     * TODO: We should only need to interact with the phoneproxy directly. However,
     * since the phoneproxy only interacts directly with CallManager we either listen to callmanager
     * or we have to poke into the proxy like this.  Neither is desirable. It would be better if
     * this class and callManager could register generically with the phone proxy instead and get
     * radio techonology changes directly.  Or better yet, just register for the notifications
     * directly with phone proxy and never worry about the technology changes. This requires a
     * change in opt/telephony code.
     */
    CARAPI_(void) RegisterForNotifications();

    CARAPI_(void) UnregisterForNotifications();

    /**
     * Verifies the incoming call and triggers sending the incoming-call intent to Telecom.
     *
     * @param asyncResult The result object from the new ringing event.
     */
    CARAPI_(void) HandleNewRingingConnection(
        /* [in] */ AsyncResult* asyncResult);

    CARAPI_(void) HandleCdmaCallWaiting(
        /* [in] */ AsyncResult* asyncResult);

    CARAPI_(void) HandleNewUnknownConnection(
        /* [in] */ AsyncResult* asyncResult);

    CARAPI_(void) AddNewUnknownCall(
        /* [in] */ IConnection* connection);

    /**
     * Sends the incoming call intent to telecom.
     */
    CARAPI_(void) SendIncomingCallIntent(
        /* [in] */ IConnection* connection);

private:
    /** New ringing connection event code. */
    static const Int32 EVENT_NEW_RINGING_CONNECTION = 100;
    static const Int32 EVENT_CDMA_CALL_WAITING = 101;
    static const Int32 EVENT_UNKNOWN_CONNECTION = 102;

    /** The phone proxy object to listen to. */
    AutoPtr<IPhoneProxy> mPhoneProxy;

    /**
     * The base phone implementation behind phone proxy. The underlying phone implementation can
     * change underneath when the radio technology changes. We listen for these events and update
     * the base phone in this variable. We save it so that when the change happens, we can
     * unregister from the events we were listening to.
     */
    AutoPtr<IPhone> mPhoneBase;

    /**
     * Used to listen to events from {@link #mPhoneBase}.
     */
    AutoPtr<IHandler> mHandler;

     /**
     * Receiver to listen for radio technology change events.
     */
    AutoPtr<IBroadcastReceiver> mRATReceiver;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_PSTNINCOMINGCALLNOTIFIER_H__