
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeExitDialog.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeExitDialogBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Core::IBoolean;
using Elastos::Core::StringUtils;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CEmergencyCallbackModeExitDialog::MyCountDownTimer::MyCountDownTimer(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval,
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
    return mHost->mTimer->Cancel();
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
    intent->PutExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
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
    intent->PutExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
    mHost->SetResult(RESULT_OK, intent);
    return mHost->Finish();
}

ECode CEmergencyCallbackModeExitDialog::MyRunnable::Run()
{
    Looper::Prepare();

    // Bind to the remote service
    AutoPtr<IIntent> intent;
    assert(0);
    //CIntent::New(this, EmergencyCallbackModeService.class, (IIntent**)&intent);
    Boolean res;
    mHost->BindService(intent, mHost->mConnection, IContext::BIND_AUTO_CREATE, &res);

    // Wait for bind to finish
    {
        AutoLock syncLock(mHost);
        //try {
        if (mHost->mService == NULL) {
            mHost->Wait();
        }
        // } catch (InterruptedException e) {
        //     Logger::D("ECM", "EmergencyCallbackModeExitDialog InterruptedException: "
        //             + e.getMessage());
        //     e.printStackTrace();
        // }
    }

    // Get timeout value and call state from the service
    if (mHost->mService != NULL) {
        mHost->mService->GetEmergencyCallbackModeTimeout(&(mHost->mEcmTimeout));
        mHost->mService->GetEmergencyCallbackModeCallState(&(mHost->mInEmergencyCall));
        //try {
            // Unbind from remote service
        mHost->UnbindService(mHost->mConnection);
        //} catch (IllegalArgumentException e) {
            // Failed to unbind from service. Don't crash as this brings down the entire
            // radio.
        //    Log.w(TAG, "Failed to unbind from EmergencyCallbackModeService");
        //}
    }

    // Show dialog
    AutoPtr<IRunnable> r = new MyRunnable2(mHost);
    return mHost->mHandler->Post(r, &res);
}

ECode CEmergencyCallbackModeExitDialog::MyRunnable2::Run()
{
    mHost->ShowEmergencyCallbackModeExitDialog();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEmergencyCallbackModeExitDialog::MyServiceConnection, Object, IServiceConnection)

ECode CEmergencyCallbackModeExitDialog::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    IEmergencyCallbackModeServiceLocalBinder::Probe(service)->
            GetService((IEmergencyCallbackModeService**)&(mHost->mService));
    // Notify thread that connection is ready
    {
        AutoLock syncLock(mHost);
        mHost->Notify();
    }
    return NOERROR;
}

ECode CEmergencyCallbackModeExitDialog::MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mHost->mService = NULL;
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
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IObject::Probe(obj);
            AutoPtr<IBoolean> value = IBoolean::Probe(result->mResult);
            Boolean res;
            value->GetValue(&res);

            if(!res) {
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->PutExtra(EXTRA_EXIT_ECM_RESULT, FALSE);
                mHost->SetResult(RESULT_OK, intent);
                mHost->Finish();
            }
            break;
        }
    }
    return NOERROR;
}

ECode CEmergencyCallbackModeExitDialog::MyBroadcastReceiver::constructor(
    /* [in] */ IEmergencyCallbackModeExitDialog* host)
{
    mHost = (CEmergencyCallbackModeExitDialog*)host;
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
            if (mHost->mAlertDialog != NULL)
                IDialogInterface::Probe(mHost->mAlertDialog)->Dismiss();
            if (mHost->mProgressDialog != NULL)
                IDialogInterface::Probe(mHost->mProgressDialog)->Dismiss();

            AutoPtr<IIntent> _intent;
            CIntent::New((IIntent**)&_intent);
            _intent->PutExtra(EXTRA_EXIT_ECM_RESULT, TRUE);
            mHost->SetResult(RESULT_OK, _intent);
            mHost->Finish();
        }
    }
    return NOERROR;
}

const String CEmergencyCallbackModeExitDialog::TAG("EmergencyCallbackMode");

CAR_INTERFACE_IMPL_2(CEmergencyCallbackModeExitDialog, Activity, IEmergencyCallbackModeExitDialog,
        IDialogInterfaceOnDismissListener)

CAR_OBJECT_IMPL(CEmergencyCallbackModeExitDialog)

CEmergencyCallbackModeExitDialog::CEmergencyCallbackModeExitDialog()
    : mDialogType(0)
    , mEcmTimeout(0)
    , mInEmergencyCall(FALSE)
{
    mTask = new MyRunnable(this);

    CEmergencyCallbackModeExitDialogBroadcastReceiver::New(this, (IBroadcastReceiver**)&mEcmExitReceiver);

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
    mPhone = PhoneGlobals::GetPhone();
    mPhone->RegisterForEcmTimerReset(mTimerResetHandler, ECM_TIMER_RESET, NULL);

    // Register receiver for intent closing the dialog
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    AutoPtr<IIntent> intent;
    return RegisterReceiver(mEcmExitReceiver, filter, (IIntent**)&intent);
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
    Boolean res;
    IsResumed(&res);
    if (!res) {
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

        mTimer = new MyCountDownTimer(mEcmTimeout, 1000, this);
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
        builder->SetIcon(Elastos::Droid::TeleService::R::drawable::ic_emergency_callback_mode);
        builder->SetTitle(Elastos::Droid::TeleService::R::string::phone_in_ecm_notification_title);
        builder->SetMessage(text);

        AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener(this);
        builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::alert_dialog_yes, listener);

        AutoPtr<IDialogInterfaceOnClickListener> listener2 =new MyDialogInterfaceOnClickListener2(this);
        builder->SetNegativeButton(Elastos::Droid::TeleService::R::string::alert_dialog_no, listener2);
        builder->Create((IAlertDialog**)&mAlertDialog);
        IDialog::Probe(mAlertDialog)->SetOnDismissListener(this);
        *dialog = IDialog::Probe(mAlertDialog);
        REFCOUNT_ADD(*dialog);
        return NOERROR;
    }
    case EXIT_ECM_IN_EMERGENCY_CALL_DIALOG:
    {

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(Elastos::Droid::TeleService::R::drawable::ic_emergency_callback_mode);
        builder->SetTitle(Elastos::Droid::TeleService::R::string::phone_in_ecm_notification_title);
        builder->SetMessage(Elastos::Droid::TeleService::R::string::alert_dialog_in_ecm_call);

        AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener3(this);
        builder->SetNeutralButton(Elastos::Droid::TeleService::R::string::alert_dialog_dismiss, listener);
        builder->Create((IAlertDialog**)&mAlertDialog);
        IDialog::Probe(mAlertDialog)->SetOnDismissListener(this);
        *dialog = IDialog::Probe(mAlertDialog);
        REFCOUNT_ADD(*dialog);
        return NOERROR;
    }
    case EXIT_ECM_PROGRESS_DIALOG:
    {
        CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);

        AutoPtr<ICharSequence> cchar;
        GetText(Elastos::Droid::TeleService::R::string::progress_dialog_exiting_ecm,
                (ICharSequence**)&cchar);
        IAlertDialog::Probe(mProgressDialog)->SetMessage(cchar);
        mProgressDialog->SetIndeterminate(TRUE);
        IDialog::Probe(mProgressDialog)->SetCancelable(FALSE);
        *dialog = IDialog::Probe(mProgressDialog);
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
    assert(0);
    String time;
    time.AppendFormat("%d:%02d", minutes, (millisUntilFinished % 60000) / 1000);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    switch (mDialogType) {
        case EXIT_ECM_BLOCK_OTHERS:
        {
            AutoPtr<ICharSequence> cchar;
            resources->GetQuantityText(Elastos::Droid::TeleService::R::plurals::alert_dialog_not_avaialble_in_ecm,
                    minutes, (ICharSequence**)&cchar);
            String str;
            cchar->ToString(&str);
            String outStr;
            outStr.AppendFormat(str, time.string());
            AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(outStr);
            return cchar2;
        }
        case EXIT_ECM_DIALOG:
        {
            AutoPtr<ICharSequence> cchar;
            resources->GetQuantityText(Elastos::Droid::TeleService::R::plurals::alert_dialog_exit_ecm,
                    minutes, (ICharSequence**)&cchar);
            String str;
            cchar->ToString(&str);
            String outStr;
            outStr.AppendFormat(str, time.string());
            AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(outStr);
            return cchar2;
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos