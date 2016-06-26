#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICE_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICE_H__

#include "_Elastos_Droid_Phone_CEmergencyCallbackModeService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/CountDownTimer.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Binder.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CountDownTimer;
using Elastos::Droid::Os::ICountDownTimer;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Application service that inserts/removes Emergency Callback Mode notification and
 * updates Emergency Callback Mode countdown clock in the notification
 *
 * @see EmergencyCallbackModeExitDialog
 */
CarClass(CEmergencyCallbackModeService)
    , public Elastos::Droid::App::Service
    , public IEmergencyCallbackModeService
{
public:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeService::MyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IEmergencyCallbackModeService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CEmergencyCallbackModeService* mHost;
    };

    /**
     * Class for clients to access
     */
    class LocalBinder
        : public Binder
        , public IEmergencyCallbackModeServiceLocalBinder
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeService::LocalBinder")

        CAR_INTERFACE_DECL()

        CARAPI GetService(
            /* [out] */ IEmergencyCallbackModeService** service);
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeService::MyHandler")

        MyHandler(
            /* [in] */ CEmergencyCallbackModeService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CEmergencyCallbackModeService* mHost;
    };

    class MyCountDownTimer
        : public CountDownTimer
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeService::MyCountDownTimer")

        MyCountDownTimer(
            /* [in] */ Int64 millisInFuture,
            /* [in] */ Int64 countDownInterval,
            /* [in] */ CEmergencyCallbackModeService* host);

        //@Override
        CARAPI OnTick(
            /* [in] */ Int64 millisUntilFinished);

        //@Override
        CARAPI OnFinish();

    private:
        CEmergencyCallbackModeService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEmergencyCallbackModeService();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    /**
     * Returns Emergency Callback Mode timeout value
     */
    CARAPI GetEmergencyCallbackModeTimeout(
        /* [out] */ Int64* timeout);

    /**
     * Returns Emergency Callback Mode call state
     */
    CARAPI GetEmergencyCallbackModeCallState(
        /* [out] */ Boolean* state);

private:
    /**
     * Start timer notification for Emergency Callback Mode
     */
    CARAPI_(void) StartTimerNotification();

    /**
     * Shows notification for Emergency Callback Mode
     */
    CARAPI_(void) ShowNotification(
        /* [in] */ Int64 millisUntilFinished);

    /**
     * Handle ECM_TIMER_RESET notification
     */
    CARAPI_(void) ResetEcmTimer(
        /* [in] */ AsyncResult* r);

private:
    // Default Emergency Callback Mode timeout value
    static const Int32 DEFAULT_ECM_EXIT_TIMER_VALUE;
    static const String TAG;

    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<ICountDownTimer> mTimer;
    Int64 mTimeLeft;
    AutoPtr<IPhone> mPhone;
    Boolean mInEmergencyCall;

    static const Int32 ECM_TIMER_RESET = 1;

    AutoPtr<IHandler> mHandler;

    /**
     * Listens for Emergency Callback Mode intents
     */
    AutoPtr<IBroadcastReceiver> mEcmReceiver;

    // This is the object that receives interactions from clients.
    AutoPtr<IBinder> mBinder;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICE_H__