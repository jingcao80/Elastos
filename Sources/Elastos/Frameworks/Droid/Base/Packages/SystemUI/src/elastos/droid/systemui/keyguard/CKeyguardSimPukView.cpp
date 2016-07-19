
#include "elastos/droid/systemui/keyguard/CKeyguardSimPukView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CKeyguardSimPukView::StateMachine::StateMachine(
    /* [in] */ CKeyguardSimPukView* host)
    : ENTER_PUK(0)
    , ENTER_PIN(1)
    , CONFIRM_PIN(2)
    , DONE(3)
    , mState(ENTER_PUK)
    , mHost(host)
{
}

ECode CKeyguardSimPukView::StateMachine::Next()
{
    Int32 msg = 0;
    if (mState == ENTER_PUK) {
        if (mHost->CheckPuk()) {
            mState = ENTER_PIN;
            msg = R::string::kg_puk_enter_pin_hint;
        }
        else {
            msg = R::string::kg_invalid_sim_puk_hint;
        }
    }
    else if (mState == ENTER_PIN) {
        if (mHost->CheckPin()) {
            mState = CONFIRM_PIN;
            msg = R::string::kg_enter_confirm_pin_hint;
        }
        else {
            msg = R::string::kg_invalid_sim_pin_hint;
        }
    }
    else if (mState == CONFIRM_PIN) {
        Boolean res;
        if (mHost->ConfirmPin(&res), res) {
            mState = DONE;
            msg = R::string::keyguard_sim_unlock_progress_dialog_message;
            mHost->UpdateSim();
        }
        else {
            mState = ENTER_PIN; // try again?
            msg = R::string::kg_invalid_confirm_pin_hint;
        }
    }
    mHost->ResetPasswordText(TRUE);
    if (msg != 0) {
        mHost->mSecurityMessageDisplay->SetMessage(msg, TRUE);
    }
    return NOERROR;
}

ECode CKeyguardSimPukView::StateMachine::Reset()
{
    mHost->mPinText("");
    mHost->mPukText("");
    mState = ENTER_PUK;
    mHost->mSecurityMessageDisplay->SetMessage(R::string::kg_puk_enter_puk_hint, TRUE);
    return mHost->mPasswordEntry->RequestFocus();
}

ECode CKeyguardSimPukView::MyRunnable::Run()
{
    return mHost->OnSimLockChangedResponse(mResult1, mResult2);
}

ECode CKeyguardSimPukView::MyRunnable2::Run()
{
    return mHost->onSimLockChangedResponse(IPhoneConstants::PIN_GENERAL_FAILURE, -1);
}


ECode CKeyguardSimPukView::CheckSimPuk::Run()
{
    //try {
    Logger::V(TAG, "call supplyPukReportResult()");

    AutoPtr<IInterface> obj = ServiceManager::CheckService(String("phone"));
    AutoPtr<IITelephony> t = IITelephony::Probe(obj);
    AutoPtr<ArrayOf<Int32> > result;
    t->SupplyPukReportResult(mPuk, mPin, (ArrayOf<Int32>**)&result);
    Logger::V(TAG, "supplyPukReportResult returned: %d %d", (*result)[0], (*result)[1]);
    AutoPtr<IRunnable> r = new MyRunnable(mHost, (*result)[0], (*result)[1])
    ECode ec = Post(r);
    //} catch (RemoteException e) {
    if (ec == (ECode)RemoteException) {
        Logger::E(TAG, "RemoteException for supplyPukReportResult:", e);
        AutoPtr<IRunnable> r = new MyRunnable2(mHost);
        Post(r);
    }
    return NOERROR;
}

ECode CKeyguardSimPukView::MyRunnable3::Run()
{
    if (mHost->mSimUnlockProgressDialog != NULL) {
        mHost->mSimUnlockProgressDialog->Hide();
    }
    if (mResult == IPhoneConstants::PIN_RESULT_SUCCESS) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        KeyguardUpdateMonitor::GetInstance(context)->ReportSimUnlocked();
        mHost->mCallback->Dismiss(TRUE);
    }
    else {
        if (mResult == IPhoneConstants::PIN_PASSWORD_INCORRECT) {
            if (mAttemptsRemaining <= 2) {
                // this is getting critical - show dialog
                mHost->GetPukRemainingAttemptsDialog(mAttemptsRemaining)->Show();
            }
            else {
                // show message
                mHost->mSecurityMessageDisplay->SetMessage(
                        mHost->GetPukPasswordErrorMessage(mAttemptsRemaining), TRUE);
            }
        }
        else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetString(R::string::kg_password_puk_failed, (IContext**)&context);
            mHost->mSecurityMessageDisplay->SetMessage(context, TRUE);
        }
        if (DEBUG) Logger::D(LOG_TAG, "verifyPasswordAndUnlock UpdateSim.onSimCheckResponse: attemptsRemaining=%d",
                mAttemptsRemaining);
        mHost->mStateMachine->Reset();
    }
    mHost->mCheckSimPukThread = NULL;
    return NOERROR;
}

ECode CKeyguardSimPukView::MyCheckSimPuk::OnSimLockChangedResponse(
    /* [in] */ Int32 result,
    /* [in] */ Int32 attemptsRemaining)
{
    AutoPtr<IRunnable> r = new MyRunnable3(mHost, result, attemptsRemaining);
    return Post(r);
}

const String CKeyguardSimPukView::TAG("KeyguardSimPukView");

const String CKeyguardSimPukView::LOG_TAG("KeyguardSimPukView");
const Boolean CKeyguardSimPukView::DEBUG = IKeyguardConstants::DEBUG;

CAR_OBJECT_IMPL(CKeyguardSimPukView)

CKeyguardSimPukView::CKeyguardSimPukView()
    : mPukText(NULL)
    , mPinText(NULL)
{
    mStateMachine = new StateMachine();
}

String CKeyguardSimPukView::GetPukPasswordErrorMessage(
    /* [in] */ Int32 attemptsRemaining)
{
    String displayMessage;

    if (attemptsRemaining == 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_wrong_puk_code_dead, &displayMessage);
    }
    else if (attemptsRemaining > 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources(&resources);
        resources->GetQuantityString(R::plurals::kg_password_wrong_puk_code, attemptsRemaining,
                        attemptsRemaining, &displayMessage);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_puk_failed, &displayMessage);
    }
    if (DEBUG) Logger::D(LOG_TAG, "getPukPasswordErrorMessage: attemptsRemaining=%d displayMessage=%d",
            attemptsRemaining, displayMessage);
    return displayMessage;
}

ECode CKeyguardSimPukView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardSimPukView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return KeyguardPinBasedInputView::constructor(context, attrs);
}

ECode CKeyguardSimPukView::ResetState()
{
    KeyguardPinBasedInputView::ResetState();
    return mStateMachine->Reset();
}

ECode CKeyguardSimPukView::ShouldLockout(
    /* [in] */ Int64 deadline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // SIM PUK doesn't have a timed lockout
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardSimPukView::GetPasswordTextViewId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *result = R::id::pukEntry;
    return NOERROR;
}
}

ECode CKeyguardSimPukView::OnFinishInflate()
{
    KeyguardPinBasedInputView::OnFinishInflate();

    mSecurityMessageDisplay->SetTimeout(0); // don't show ownerinfo/charging status by default
    if (IEmergencyCarrierArea::Probe(mEcaView) != NULL) {
        IEmergencyCarrierArea::Probe(mEcaView)->SetCarrierTextVisible(TRUE);
    }
    return NOERROR;
}

ECode CKeyguardSimPukView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardSimPukView::OnPause()
{
    // dismiss the dialog.
    if (mSimUnlockProgressDialog != NULL) {
        mSimUnlockProgressDialog->Dismiss();
        mSimUnlockProgressDialog = NULL;
    }
    return NOERROR;
}

AutoPtr<IDialog> CKeyguardSimPukView::GetSimUnlockProgressDialog()
{
    if (mSimUnlockProgressDialog == NULL) {
        CProgressDialog::New(mContext, (IProgressDialog)&mSimUnlockProgressDialog);
        String massage;
        mContext->GetString(R::string::kg_sim_unlock_progress_dialog_message, &massage);
        mSimUnlockProgressDialog->SetMessage(massage);
        mSimUnlockProgressDialog->SetIndeterminate(TRUE);
        mSimUnlockProgressDialog->SetCancelable(FALSE);
        if (IActivity::Probe(mContext) == NULL) {
            AutoPtr<IWindow> window;
            mSimUnlockProgressDialog->GetWindow((IWindow**)&window);
            window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
        }
    }
    return mSimUnlockProgressDialog;
}

AutoPtr<IDialog> CKeyguardSimPukView::GetPukRemainingAttemptsDialog(
    /* [in] */ Int32 remaining)
{
    String msg = getPukPasswordErrorMessage(remaining);
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

Boolean CKeyguardSimPukView::CheckPuk()
{
    // make sure the puk is at least 8 digits long.
    String text;
    mPasswordEntry->GetText(&text);
    if (text.GetLength() == 8) {
        mPukText = mPasswordEntry->GetText(&mPukText);
        return TRUE;
    }
    return FALSE;
}

Boolean CKeyguardSimPukView::CheckPin()
{
    // make sure the PIN is between 4 and 8 digits
    String text;
    mPasswordEntry->GetText(&text);
    Int32 length = text.GetLength();
    if (length >= 4 && length <= 8) {
        mPasswordEntry->GetText(&mPinText);
        return TRUE;
    }
    return FALSE;
}

ECode CKeyguardSimPukView::ConfirmPin(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String text;
    mPasswordEntry->GetText(&text);
    *result = mPinText.equals(text);
    return NOERROR;
}

void CKeyguardSimPukView::UpdateSim()
{
    AutoPtr<IDialog> dialog = GetSimUnlockProgressDialog();
    dialog->Show();

    if (mCheckSimPukThread == NULL) {
        mCheckSimPukThread = new MyCheckSimPuk(this, mPukText, mPinText) ;
        mCheckSimPukThread->Start();
    }
}

ECode CKeyguardSimPukView::VerifyPasswordAndUnlock()
{
    return mStateMachine->Next();
}

ECode CKeyguardSimPukView::StartAppearAnimation()
{
    return NOERROR;
}

ECode CKeyguardSimPukView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
