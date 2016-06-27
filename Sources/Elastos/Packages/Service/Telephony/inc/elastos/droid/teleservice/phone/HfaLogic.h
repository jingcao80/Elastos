#ifndef  __ELASTOS_DROID_PHONE_HFALOGIC_H__
#define  __ELASTOS_DROID_PHONE_HFALOGIC_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Starts and displays status for Hands Free Activation (HFA).
 *
 * This class operates with Hands Free Activation apps.
 * It starts by broadcasting the intent com.android.action.START_HFA.
 * An HFA app will pick that up and start the HFA process.
 * If it fails it return ERROR_HFA Intent and upon success returns COMPLETE_HFA.
 *
 * If successful, we bounce the radio so that the service picks up the new number.
 * Once the radio is back on we callback the requestor.
 *
 * If there is an error, we do not bounce the radio but still callback with a failure.
 *
 * TODO(klp): We need system-only permissions for the HFA intents.
 */
class HfaLogic
    : public Object
    , public IHfaLogic
{
public:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("HfaLogic::MyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IHfaLogic* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        HfaLogic* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("HfaLogic::MyHandler")

        MyHandler(
            /* [in] */ HfaLogic* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        HfaLogic* mHost;
    };

public:
    TO_STRING_IMPL("HfaLogic")

    CAR_INTERFACE_DECL()

    HfaLogic(
        /* [in] */ IContext* context,
        /* [in] */ IHfaLogicHfaLogicCallback* _callback,
        /* [in] */ IPendingIntent* intent);

    CARAPI Start();

private:
    CARAPI_(void) StartProvisioning();

    CARAPI_(void) SendHfaCommand(
        /* [in] */ const String& action);

    CARAPI_(void) OnHfaError(
        /* [in] */ const String& errorMsg);

    CARAPI_(void) OnHfaSuccess();

    CARAPI_(void) OnTotalSuccess();

    CARAPI_(void) BounceRadio();

    CARAPI_(void) OnServiceStateChange(
        /* [in] */ IServiceState* state);

    CARAPI_(void) StartHfaIntentReceiver();

    CARAPI_(void) StopHfaIntentReceiver();

    CARAPI_(void) SendFinalResponse(
        /* [in] */ Int32 responseCode,
        /* [in] */ const String& errorCode);

private:
    static const String TAG;

    static const String ACTION_START;
    static const String ACTION_ERROR;
    static const String ACTION_CANCEL;
    static const String ACTION_COMPLETE;

    static const Int32 SERVICE_STATE_CHANGED = 1;

    Int32 mPhoneMonitorState;
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IHfaLogicHfaLogicCallback> mCallback;
    AutoPtr<IPendingIntent> mResponseIntent;
    AutoPtr<IContext> mContext;

    // No retry at the moment. Increase later if necessary.
    static const Int32 DEFAULT_RETRY_COUNT;
    Int32 mRetryCount;

    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_HFALOGIC_H__