
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLRECEIVER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallsManager;
/**
 * Single point of entry for all outgoing and incoming calls. {@link CallActivity} serves as a
 * trampoline activity that captures call intents for individual users and forwards it to
 * the {@link CallReceiver} which interacts with the rest of Telecom, both of which run only as
 * the primary user.
 */
class CallReceiver
    : public BroadcastReceiver
    , public ICallReceiver
{
public:
    TO_STRING_IMPL("CallReceiver")

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Processes CALL, CALL_PRIVILEGED, and CALL_EMERGENCY intents.
     *
     * @param intent Call intent containing data about the handle to call.
     */
    static CARAPI ProcessOutgoingCallIntent(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    static CARAPI ProcessIncomingCallIntent(
        /* [in] */ IIntent* intent);

    static CARAPI GetCallsManager(
        /* [out] */ CallsManager** result);

private:
    CARAPI ProcessUnknownCallIntent(
        /* [in] */ IIntent* intent);

    static CARAPI DisconnectCallAndShowErrorDialog(
        /* [in] */ IContext* context,
        /* [in] */ ICall* call,
        /* [in] */ Int32 errorCode);

public:
    static const String KEY_IS_UNKNOWN_CALL;

    static const String KEY_IS_INCOMING_CALL;

    static const String KEY_IS_DEFAULT_DIALER;

private:
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLRECEIVER_H__
