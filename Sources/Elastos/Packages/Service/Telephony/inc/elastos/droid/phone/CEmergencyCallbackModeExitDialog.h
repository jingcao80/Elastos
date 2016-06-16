#ifndef  __ELASTOS_DROID_PHONE_CPHONEAPP_H__
#define  __ELASTOS_DROID_PHONE_CPHONEAPP_H__

#include "_Elastos_Droid_Phone_CPhoneApp.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Displays dialog that enables users to exit Emergency Callback Mode
 *
 * @see EmergencyCallbackModeService
 */
CarClass(CEmergencyCallbackModeExitDialog)
    , public Activity
    , public IEmergencyCallbackModeExitDialog
    , public IDialogInterfaceOnDismissListener
{
private:
    class MyCountDownTimer
        : public CountDownTimer
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyCountDownTimer")

        MyCountDownTimer(
            /* [in] */ Int64 millisInFuture,
            /* [in] */ Int64 countDownInterval,
            /* [in] */ CEmergencyCallbackModeExitDialog* host);

        //@Override
        CARAPI OnTick(
            /* [in] */ Int64 millisUntilFinished);

        //@Override
        CARAPI OnFinish();

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 whichButton);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyDialogInterfaceOnClickListener2
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener2(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 whichButton);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyDialogInterfaceOnClickListener3
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener3")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener3(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 whichButton);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyRunnable")

        MyRunnable(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyRunnable2")

        MyRunnable2(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyServiceConnection
        : public ServiceConnection
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyServiceConnection")

        MyServiceConnection(
            /* [in] */ CEmergencyCallbackModeExitDialog* host)
            : mHost(host)
        {}

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyHandler")

        MyHandler(
            /* [in] */ CEmergencyCallbackModeExitDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CEmergencyCallbackModeExitDialog::MyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IEmergencyCallbackModeExitDialog* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CEmergencyCallbackModeExitDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEmergencyCallbackModeExitDialog();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnDestroy();

    /**
     * Closes activity when dialog is dismissed
     */
    //@Override
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

protected:
    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    /**
     * Creates dialog that enables users to exit Emergency Callback Mode
     */
    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** dialog);

private:
    /**
     * Shows Emergency Callback Mode dialog and starts countdown timer
     */
    CARAPI_(void) ShowEmergencyCallbackModeExitDialog();

    /**
     * Returns dialog box text with updated timeout value
     */
    CARAPI_(AutoPtr<ICharSequence>) GetDialogText(
        /* [in] */ Int64 millisUntilFinished);

private:
    static const String TAG;

    AutoPtr<IAlertDialog> mAlertDialog;
    AutoPtr<IProgressDialog> mProgressDialog;
    AutoPtr<ICountDownTimer> mTimer;
    AutoPtr<IEmergencyCallbackModeService> mService;
    AutoPtr<IHandler> mHandler;
    Int32 mDialogType;
    Int64 mEcmTimeout;
    Boolean mInEmergencyCall;
    static const Int32 ECM_TIMER_RESET;
    AutoPtr<IPhone> mPhone;

    /**
     * Waits until bind to the service completes
     */
    AutoPtr<IRunnable> mTask;

    /**
     * Listens for Emergency Callback Mode state change intents
     */
    AutoPtr<IBroadcastReceiver> mEcmExitReceiver;

    /**
     * Class for interacting with the interface of the service
     */
    AutoPtr<IServiceConnection> mConnection;

    /**
     * Class for receiving framework timer reset notifications
     */
    AutoPtr<IHandler> mTimerResetHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEAPP_H__