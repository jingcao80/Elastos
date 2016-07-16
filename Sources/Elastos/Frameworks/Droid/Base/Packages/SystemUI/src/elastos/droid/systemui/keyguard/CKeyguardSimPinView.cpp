
#include "elastos/droid/systemui/keyguard/CKeyguardSimPinView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardSimPinView::CheckSimPin::MyRunnable::Run()
{
    return mHost->OnSimCheckResponse((*result)[0], (*result)[1]);
}

ECode CKeyguardSimPinView::CheckSimPin::MyRunnable2::Run()
{
    return mHost->OnSimCheckResponse(IPhoneConstants::PIN_GENERAL_FAILURE, -1);
}

ECode CKeyguardSimPinView::CheckSimPin::Run()
{
    //try {
    Logger::V(TAG, "call supplyPinReportResult()");
    AutoPtr<IInterface> obj = ServiceManager::CheckService(String("phone"));
    AutoPtr<ArrayOf<Int32> > result;
    IITelephony::Probe(obj)->SupplyPinReportResult(mPin, (ArrayOf<Int32>**)&result);
    Logger::V(TAG, "supplyPinReportResult returned: %d %d", (*result)[0], (*result)[1]);

    AutoPtr<IRunnable> r = new CKeyguardSimPinView::CheckSimPin::MyRunnable(this);
    ECode ec = Post(r);
    //} catch (RemoteException e) {
ERROR:
    if (ec == (ECode)RemoteException) {
        Logger::E(TAG, "RemoteException for supplyPinReportResult: %d", ec);
        AutoPtr<IRunnable> r = new CKeyguardSimPinView::CheckSimPin::MyRunnable2(this);
        Post(r);
    }
    return NOERROR;
}

ECode CKeyguardSimPinView::MyRunnable::Run()
{
    if (mHost->mSimUnlockProgressDialog != NULL) {
        mHost->mSimUnlockProgressDialog->Hide();
    }
    if (mResult == IPhoneConstants::PIN_RESULT_SUCCESS) {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        KeyguardUpdateMonitor::GetInstance(context)->ReportSimUnlocked();
        mHost->mCallback->Dismiss(TRUE);
    }
    else {
        if (mResult == IPhoneConstants::PIN_PASSWORD_INCORRECT) {
            if (mAttemptsRemaining <= 2) {
                // this is getting critical - show dialog
                AutoPtr<IDialog> dialog = mHost->GetSimRemainingAttemptsDialog(mAttemptsRemaining);
                dialog->Show();
            }
            else {
                // show message
                String str = mHost->GetPinPasswordErrorMessage(mAttemptsRemaining);
                mHost->mSecurityMessageDisplay->SetMessage(str, TRUE);
            }
        }
        else {
            // "PIN operation failed!" - no idea what this was and no way to
            // find out. :/
            AutoPtr<IContext> context;
            mHost->GetContext((IContext**)&context);
            String str;
            context->GetString(R::string::kg_password_pin_failed, &str);
            mHost->mSecurityMessageDisplay->SetMessage(str, TRUE);
        }
        if (DEBUG) Logger::D(LOG_TAG, "verifyPasswordAndUnlock "
                + " CheckSimPin.onSimCheckResponse: " + mResult
                + " attemptsRemaining=" + mAttemptsRemaining);
        mHost->ResetPasswordText(TRUE /* animate */);
    }
    mHost->mCallback->UserActivity();
    mHost->mCheckSimPinThread = NULL;
    return NOERROR;
}

ECode CKeyguardSimPinView::MyCheckSimPin::OnSimCheckResponse(
    /* [in] */ Int32 result,
    /* [in] */ Int32 attemptsRemaining)
{
    AutoPtr<IRunnable> r = new MyRunnable(mHost, result, attemptsRemaining);
    return Post(r);
}

static const String CKeyguardSimPinView::TAG("KeyguardSimPinView");

static const String CKeyguardSimPinView::LOG_TAG("KeyguardSimPinView");
static const Boolean CKeyguardSimPinView::DEBUG = IKeyguardConstants::DEBUG;

CAR_OBJECT_IMPL(CKeyguardSimPinView)

ECode CKeyguardSimPinView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardSimPinView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return KeyguardPinBasedInputView::constructor(context, attrs);
}

String CKeyguardSimPinView::GetPinPasswordErrorMessage(
    /* [in] */ Int32 attemptsRemaining)
{
    String displayMessage;

    if (attemptsRemaining == 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_wrong_pin_code_pukked, &displayMessage);
    }
    else if (attemptsRemaining > 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetQuantityString(R::plurals::kg_password_wrong_pin_code, attemptsRemaining,
                attemptsRemaining, &displayMessage);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_pin_failed, &displayMessage);
    }
    if (DEBUG) Logger::D(LOG_TAG, "getPinPasswordErrorMessage: attemptsRemaining=%d displayMessage=%d",
            attemptsRemaining + displayMessage);
    return displayMessage;
}

ECode CKeyguardSimPinView::ShouldLockout(
    /* [in] */ Int64 deadline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // SIM PIN doesn't have a timed lockout
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardSimPinView::GetPasswordTextViewId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = R::id::simPinEntry;
    return NOERROR;
}

ECode CKeyguardSimPinView::OnFinishInflate()
{
    KeyguardPinBasedInputView::OnFinishInflate();

    mSecurityMessageDisplay->SetTimeout(0); // don't show ownerinfo/charging status by default
    if (IEmergencyCarrierArea::Probe(mEcaView) != NULL) {
        IEmergencyCarrierArea::Probe(mEcaView)->SetCarrierTextVisible(TRUE);
    }
    return NOERROR;
}

ECode CKeyguardSimPinView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardSimPinView::OnPause()
{
    // dismiss the dialog.
    if (mSimUnlockProgressDialog != NULL) {
        mSimUnlockProgressDialog->Dismiss();
        mSimUnlockProgressDialog = NULL;
    }
    return NOERROR;
}

AutoPtr<IDialog> CKeyguardSimPinView::GetSimUnlockProgressDialog()
{
    if (mSimUnlockProgressDialog == NULL) {
        CProgressDialog::New(mContext, (IProgressDialog**)&mSimUnlockProgressDialog);
        String str;
        mContext->GetString(R::string::kg_sim_unlock_progress_dialog_message, &str);
        mSimUnlockProgressDialog->SetMessage(str);
        mSimUnlockProgressDialog->SetIndeterminate(TRUE);
        mSimUnlockProgressDialog->SetCancelable(FALSE);
        AutoPtr<IWindow> window;
        mSimUnlockProgressDialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    return mSimUnlockProgressDialog;
}

AutoPtr<IDialog> CKeyguardSimPinView::GetSimRemainingAttemptsDialog(
    /* [in] */ Int32 remaining)
{
    String msg = GetPinPasswordErrorMessage(remaining);
    if (mRemainingAttemptsDialog == NULL) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(msg);
        builder->SetCancelable(FALSE);
        builder->SetNeutralButton(R::string::ok, NULL);
        builder->Create((IAlertDialog**)&mRemainingAttemptsDialog);
        AutoPtr<IWindow> window;
        mRemainingAttemptsDialog->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    else {
        mRemainingAttemptsDialog->SetMessage(msg);
    }
    return mRemainingAttemptsDialog;
}

ECode CKeyguardSimPinView::VerifyPasswordAndUnlock()
{
    String entry;
    mPasswordEntry->GetText(&entry);

    if (entry.GetLength() < 4) {
        // otherwise, display a message to the user, and don't submit.
        mSecurityMessageDisplay->SetMessage(R::string::kg_invalid_sim_pin_hint, TRUE);
        ResetPasswordText(TRUE);
        mCallback->UserActivity();
        return NOERROR;
    }

    AutoPtr<IDialog> dialog = GetSimUnlockProgressDialog();
    dialog->Show();

    if (mCheckSimPinThread == NULL) {
        String text;
        mPasswordEntry->GetText(&text);
        mCheckSimPinThread = new MyCheckSimPin(text);
        mCheckSimPinThread->Start();
    }
    return NOERROR;
}

ECode CKeyguardSimPinView::StartAppearAnimation()
{
    return NOERROR;
}

ECode CKeyguardSimPinView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}


} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
