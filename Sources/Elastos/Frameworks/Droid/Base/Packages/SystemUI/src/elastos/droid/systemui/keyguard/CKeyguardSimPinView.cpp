
#include "elastos/droid/systemui/keyguard/CKeyguardSimPinView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardSimPinView::CheckSimPin::MyRunnable::Run()
{
    return mHost->OnSimCheckResponse((*mResult)[0], (*mResult)[1]);
}

ECode CKeyguardSimPinView::CheckSimPin::MyRunnable2::Run()
{
    return mHost->OnSimCheckResponse(IPhoneConstants::PIN_GENERAL_FAILURE, -1);
}

ECode CKeyguardSimPinView::CheckSimPin::Run()
{
    //try {
    Logger::V(TAG, "call supplyPinReportResult()");
    AutoPtr<IServiceManager> helper;
    CServiceManager::AcquireSingleton((IServiceManager**)&helper);
    AutoPtr<IInterface> obj;
    helper->CheckService(String("phone"), (IInterface**)&obj);
    AutoPtr<ArrayOf<Int32> > result;
    IITelephony::Probe(obj)->SupplyPinReportResult(mPin, (ArrayOf<Int32>**)&result);
    Logger::V(TAG, "supplyPinReportResult returned: %d %d", (*result)[0], (*result)[1]);

    AutoPtr<IRunnable> r = new CKeyguardSimPinView::CheckSimPin::MyRunnable(this, result);
    Boolean res;
    ECode ec = mHost->Post(r, &res);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "RemoteException for supplyPinReportResult: %d", ec);
        AutoPtr<IRunnable> r = new CKeyguardSimPinView::CheckSimPin::MyRunnable2(this);
        Boolean res;
        mHost->Post(r, &res);
    }
    return NOERROR;
}

ECode CKeyguardSimPinView::MyCheckSimPin::MyRunnable2::Run()
{
    if (mHost->mSimUnlockProgressDialog != NULL) {
        IDialog::Probe(mHost->mSimUnlockProgressDialog)->Hide();
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
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
                mHost->mSecurityMessageDisplay->SetMessage(cchar, TRUE);
            }
        }
        else {
            // "PIN operation failed!" - no idea what this was and no way to
            // find out. :/
            AutoPtr<IContext> context;
            mHost->GetContext((IContext**)&context);
            String str;
            context->GetString(R::string::kg_password_pin_failed, &str);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
            mHost->mSecurityMessageDisplay->SetMessage(cchar, TRUE);
        }
        if (DEBUG) Logger::D(TAG, "verifyPasswordAndUnlock "
                " CheckSimPin.onSimCheckResponse: %d attemptsRemaining=%d",
                mResult, mAttemptsRemaining);
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
    AutoPtr<IRunnable> r = new MyRunnable2(mHost, result, attemptsRemaining);
    Boolean res;
    return mHost->Post(r, &res);
}

const String CKeyguardSimPinView::TAG("KeyguardSimPinView");

const Boolean CKeyguardSimPinView::DEBUG = IKeyguardConstants::DEBUG;

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

ECode CKeyguardSimPinView::ResetState()
{
    KeyguardPinBasedInputView::ResetState();
    return mSecurityMessageDisplay->SetMessage(
            R::string::kg_sim_pin_instructions, TRUE);
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
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<IInteger32> obj = CoreUtils::Convert(attemptsRemaining);
        array->Set(0, TO_IINTERFACE(obj));
        resources->GetQuantityString(R::plurals::kg_password_wrong_pin_code,
            attemptsRemaining, array, &displayMessage);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_pin_failed, &displayMessage);
    }
    if (DEBUG) Logger::D(TAG, "getPinPasswordErrorMessage: attemptsRemaining=%d displayMessage=%d",
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
        IDialogInterface::Probe(mSimUnlockProgressDialog)->Dismiss();
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
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        IAlertDialog::Probe(mSimUnlockProgressDialog)->SetMessage(cchar);
        mSimUnlockProgressDialog->SetIndeterminate(TRUE);
        IDialog::Probe(mSimUnlockProgressDialog)->SetCancelable(FALSE);
        AutoPtr<IWindow> window;
        IDialog::Probe(mSimUnlockProgressDialog)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    return IDialog::Probe(mSimUnlockProgressDialog);
}

AutoPtr<IDialog> CKeyguardSimPinView::GetSimRemainingAttemptsDialog(
    /* [in] */ Int32 remaining)
{
    String msg = GetPinPasswordErrorMessage(remaining);
    if (mRemainingAttemptsDialog == NULL) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(msg);
        builder->SetMessage(cchar);
        builder->SetCancelable(FALSE);
        builder->SetNeutralButton(R::string::ok, NULL);
        builder->Create((IAlertDialog**)&mRemainingAttemptsDialog);
        AutoPtr<IWindow> window;
        IDialog::Probe(mRemainingAttemptsDialog)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    else {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(msg);
        mRemainingAttemptsDialog->SetMessage(cchar);
    }
    return IDialog::Probe(mRemainingAttemptsDialog);
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
        mCheckSimPinThread = new MyCheckSimPin(text, this);
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
