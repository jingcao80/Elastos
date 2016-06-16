
#include "elastos/droid/phone/CEmergencyCallbackModeExitDialog.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CEmergencyCallbackModeExitDialog::MyCountDownTimer::MyCountDownTimer(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval
    /* [in] */ CEmergencyCallbackModeExitDialog* host)
    : mHost(host)
{
    CountDownTimer::constructor(millisInFuture, countDownInterval);
}

ECode CEmergencyCallbackModeExitDialog::MyCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    AutoPtr<ICharSequence> text = mHost->GetDialogText(millisUntilFinished);
    return mHost->mAlertDialog->SetMessage(text);
}

ECode CEmergencyCallbackModeExitDialog::MyCountDownTimer::OnFinish()
{
    //Do nothing
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 whichButton)
{
    // User clicked Yes. Exit Emergency Callback Mode.
    mHost->mPhone->ExitEmergencyCallbackMode();

    // Show progress dialog
    mHost->ShowDialog(EXIT_ECM_PROGRESS_DIALOG);
    return mTimer->Cancel();
}

CAR_INTERFACE_IMPL(CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener2, Object,
        IDialogInterfaceOnClickListener)

ECode CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener2::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 whichButton)
{
    // User clicked No
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->putExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
    mHost->SetResult(RESULT_OK, intent);
    return mHost->Finish();
}

CAR_INTERFACE_IMPL(CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener3, Object,
        IDialogInterfaceOnClickListener)

ECode CEmergencyCallbackModeExitDialog::MyDialogInterfaceOnClickListener3::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 whichButton)
{
    // User clicked Dismiss
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->putExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
    mHost->SetResult(RESULT_OK, intent);
    return mHost->Finish();
}

ECode CEmergencyCallbackModeExitDialog::MyRunnable::Run()
{
    Looper::Prepare();

    // Bind to the remote service
    AutoPtr<IIntent> intent;
    CIntent::New(this, EmergencyCallbackModeService.class, (IIntent**)&intent);
    BindService(intent, mConnection, IContext::BIND_AUTO_CREATE);

    // Wait for bind to finish
    {
        AutoLock syncLock(mHost);
        //try {
        if (mService == NULL) {
            mHost->Wait();
        //    }
        // } catch (InterruptedException e) {
        //     Logger::D("ECM", "EmergencyCallbackModeExitDialog InterruptedException: "
        //             + e.getMessage());
        //     e.printStackTrace();
        // }
    }

    // Get timeout value and call state from the service
    if (mService != NULL) {
        mService->GetEmergencyCallbackModeTimeout(&mEcmTimeout);
        mService->GetEmergencyCallbackModeCallState(&mInEmergencyCall);
        //try {
            // Unbind from remote service
        UnbindService(mConnection);
        //} catch (IllegalArgumentException e) {
            // Failed to unbind from service. Don't crash as this brings down the entire
            // radio.
        //    Log.w(TAG, "Failed to unbind from EmergencyCallbackModeService");
        //}
    }

    // Show dialog
    AutoPtr<IRunnable> r = new MyRunnable2(this);
    return mHost->mHandler->Post(r);
}

ECode CEmergencyCallbackModeExitDialog::MyRunnable2::Run()
{
    mHost->ShowEmergencyCallbackModeExitDialog();
    return NOERROR;
}

ECode MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    ((EmergencyCallbackModeService::LocalBinder*)service)->GetService((IEmergencyCallbackModeService**)&mService);
    // Notify thread that connection is ready
    {
        AutoLock syncLock(mHost);
        mHost->Notify();
    }
    return NOERROR;
}

ECode MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mService = NULL;
    return NOERROR;
}


CEmergencyCallbackModeExitDialog::MyHandler::MyHandler(
    /* [in] */ CEmergencyCallbackModeExitDialog* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CEmergencyCallbackModeExitDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ECM_TIMER_RESET:
            if(!((Boolean)((AsyncResult) msg.obj).result).booleanValue()) {
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->PutExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
                mHost->SetResult(RESULT_OK, intent);
                mHost->Finish();
            }
            break;
    }
    return NOERROR;
}

ECode CEmergencyCallbackModeExitDialog::MyBroadcastReceiver::constructor(
    /* [in] */ IEmergencyCallbackModeExitDialog* host)
{
    mHost = (CEmergencyCallbackModeService*)host;
    return BroadcastReceiver::constructor();
}

ECode CEmergencyCallbackModeExitDialog::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Received exit Emergency Callback Mode notification close all dialogs
    String action;
    intent->GetAction(&action);
    if (action.Equals(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
        Boolean res;
        intent->GetBooleanExtra(String("phoneinECMState"), FALSE, &res);
        if (res == FALSE) {
            if (mAlertDialog != NULL)
                mHost->mAlertDialog->Dismiss();
            if (mProgressDialog != NULL)
                mHost->mProgressDialog->Dismiss();

            AutoPtr<IIntent> _intent;
            CIntent::New((IIntent**)&_intent);
            _intent->PutExtra(EXTRA_EXIT_ECM_RESULT, TRUE)
            mHost->SetResult(RESULT_OK, _intent);
            mHost->Finish();
        }
    }
    return NOERROR;
}

static const String CEmergencyCallbackModeExitDialog::TAG("EmergencyCallbackMode");

static const Int32 CEmergencyCallbackModeExitDialog::ECM_TIMER_RESET = 1;

CAR_INTERFACE_IMPL_2(CEmergencyCallbackModeExitDialog, Activity, IEmergencyCallbackModeExitDialog,
        IDialogInterfaceOnDismissListener)

CAR_OBJECT_IMPL(CEmergencyCallbackModeExitDialog)

CEmergencyCallbackModeExitDialog::CEmergencyCallbackModeExitDialog()
    : mDialogType(0)
    , mEcmTimeout(0)
    , mInEmergencyCall(FALSE)
{
    mTask = new MyRunnable(this);

    CCEmergencyCallbackModeExitDialogBroadcastReceiver::New(this, (IBroadcastReceiver**)&mEcmExitReceiver);

    mConnection = new MyServiceConnection(this);

    mTimerResetHandler = new MyHandler(this);
}

ECode CEmergencyCallbackModeExitDialog::constructor()
{
    return Activity::constructor();
}

ECode CEmergencyCallbackModeExitDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    // Check if phone is in Emergency Callback Mode. If not, exit.
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    String mode;
    helper->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &mode);
    Boolean isInEcm = StringUtils::ParseBoolean(mode);

    StringBuilder sb;
    sb += "ECMModeExitDialog launched - isInEcm: ";
    sb += isInEcm;
    Logger::I(TAG, sb.ToString());
    if (!isInEcm) {
        return Finish();
    }

    CHandler::New((IHandler**)&mHandler);

    // Start thread that will wait for the connection completion so that it can get
    // timeout value from the service
    AutoPtr<IThread> waitForConnectionCompleteThread;
    CThread::New(NULL, mTask, String("EcmExitDialogWaitThread"), (IThread**)&waitForConnectionCompleteThread);
    waitForConnectionCompleteThread->Start();

    // Register ECM timer reset notfication
    PhoneGlobals::GetPhone((IPhone**)&mPhone);
    mPhone->RegisterForEcmTimerReset(mTimerResetHandler, ECM_TIMER_RESET, NULL);

    // Register receiver for intent closing the dialog
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    return RegisterReceiver(mEcmExitReceiver, filter);
}

ECode CEmergencyCallbackModeExitDialog::OnDestroy()
{
    Activity::OnDestroy();
    //try {
    UnregisterReceiver(mEcmExitReceiver);
    //} catch (IllegalArgumentException e) {
        // Receiver was never registered - silently ignore.
    //}
    // Unregister ECM timer reset notification
    if (mPhone != NULL) {
        mPhone->UnregisterForEcmTimerReset(mHandler);
    }
    return NOERROR;
}

ECode CEmergencyCallbackModeExitDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnRestoreInstanceState(savedInstanceState);
    return savedInstanceState->GetInt32(String("DIALOG_TYPE"), &mDialogType);
}

ECode CEmergencyCallbackModeExitDialog::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Activity::OnSaveInstanceState(outState);
    return outState->PutInt32(String("DIALOG_TYPE"), mDialogType);
}

void CEmergencyCallbackModeExitDialog::ShowEmergencyCallbackModeExitDialog()
{
    if (!IsResumed()) {
        Logger::W(TAG, "Tried to show dialog, but activity was already finished");
        return;
    }
    if(mInEmergencyCall) {
        mDialogType = EXIT_ECM_IN_EMERGENCY_CALL_DIALOG;
        ShowDialog(EXIT_ECM_IN_EMERGENCY_CALL_DIALOG);
    }
    else {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        String action;
        intent->GetAction(&action);
        if (action.Equals(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS)) {
            mDialogType = EXIT_ECM_BLOCK_OTHERS;
            ShowDialog(EXIT_ECM_BLOCK_OTHERS);
        }
        else if (action.Equals(ACTION_SHOW_ECM_EXIT_DIALOG)) {
            mDialogType = EXIT_ECM_DIALOG;
            ShowDialog(EXIT_ECM_DIALOG);
        }

        mTimer = new MyCountDownTimer(mEcmTimeout, 1000);
        mTimer->Start();
    }
}

ECode CEmergencyCallbackModeExitDialog::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    switch (id) {
    case EXIT_ECM_BLOCK_OTHERS:
    case EXIT_ECM_DIALOG:
    {
        AutoPtr<ICharSequence> text = GetDialogText(mEcmTimeout);

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(R.drawable.ic_emergency_callback_mode);
        builder->SetTitle(R.string.phone_in_ecm_notification_title);
        builder->SetMessage(text);

        AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener(this);
        builder->SetPositiveButton(R.string.alert_dialog_yes, listener);

        AutoPtr<IDialogInterfaceOnClickListener> listener2 =new MyDialogInterfaceOnClickListener2(this);
        builder->SetNegativeButton(R.string.alert_dialog_no, listener2);
        builder->Create((IAlertDialog**)&mAlertDialog);
        mAlertDialog->SetOnDismissListener(this);
        *dialog = mAlertDialog;
        REFCOUNT_ADD(*dialog);
        return NOERROR;
    }
    case EXIT_ECM_IN_EMERGENCY_CALL_DIALOG:
    {

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(R.drawable.ic_emergency_callback_mode);
        builder->SetTitle(R.string.phone_in_ecm_notification_title);
        builder->SetMessage(R.string.alert_dialog_in_ecm_call);

        AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener3(this);
        builder->SetNeutralButton(R.string.alert_dialog_dismiss, listener);
        builder->Create((IAlertDialog**)&mAlertDialog);
        mAlertDialog->SetOnDismissListener(this);
        *dialog = mAlertDialog;
        REFCOUNT_ADD(*dialog);
        return NOERROR;
    }
    case EXIT_ECM_PROGRESS_DIALOG:
    {
        CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);

        AutoPtr<ICharSequence> cchar;
        GetText(R.string.progress_dialog_exiting_ecm, (ICharSequence**)&cchar);
        mProgressDialog->SetMessage(cchar);
        mProgressDialog->SetIndeterminate(TRUE);
        mProgressDialog->SetCancelable(FALSE);
        *dialog = mProgressDialog;
        REFCOUNT_ADD(*dialog);
        return NOERROR;
    }
    default:
        *dialog = NULL;
        return NOERROR;
    }
    return NOERROR;
}

AutoPtr<ICharSequence> CEmergencyCallbackModeExitDialog::GetDialogText(
    /* [in] */ Int64 millisUntilFinished)
{
    // Format time
    Int32 minutes = (Int32)(millisUntilFinished / 60000);
    String time = String.format("%d:%02d", minutes,
            (millisUntilFinished % 60000) / 1000);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    switch (mDialogType) {
        case EXIT_ECM_BLOCK_OTHERS:
        {
            AutoPtr<ICharSequence> cchar;
            resources->GetQuantityText(R.plurals.alert_dialog_not_avaialble_in_ecm,
                    minutes, (ICharSequence**)&cchar);
            String str;
            cchar->ToString(&str);
            return String.format(str, time);
        }
        case EXIT_ECM_DIALOG:
        {
            AutoPtr<ICharSequence> cchar;
            resources->GetQuantityText(R.plurals.alert_dialog_exit_ecm,
                    minutes, (ICharSequence**)&cchar);
            String str;
            cchar->ToString(&str);
            return String.format(str, time);
        }
    }
    return NULL;
}

ECode CEmergencyCallbackModeExitDialog::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(EXTRA_EXIT_ECM_RESULT, FALSE);

    SetResult(RESULT_OK, intent);
    return Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos