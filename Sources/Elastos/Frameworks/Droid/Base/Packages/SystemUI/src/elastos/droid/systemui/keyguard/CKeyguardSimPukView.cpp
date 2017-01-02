//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/keyguard/CKeyguardSimPukView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::CoreUtils;

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
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
    cchar->ToString(&(mHost->mPinText));
    cchar->ToString(&(mHost->mPukText));
    mState = ENTER_PUK;
    mHost->mSecurityMessageDisplay->SetMessage(
            R::string::kg_puk_enter_puk_hint, TRUE);
    Boolean res;
    return IView::Probe(mHost->mPasswordEntry)->RequestFocus(&res);
}

ECode CKeyguardSimPukView::CheckSimPuk::MyRunnable::Run()
{
    return mHost->OnSimLockChangedResponse(mResult1, mResult2);
}

ECode CKeyguardSimPukView::CheckSimPuk::MyRunnable2::Run()
{
    return mHost->OnSimLockChangedResponse(IPhoneConstants::PIN_GENERAL_FAILURE, -1);
}


ECode CKeyguardSimPukView::CheckSimPuk::Run()
{
    //try {
    Logger::V(TAG, "call supplyPukReportResult()");

    AutoPtr<IServiceManager> helper;
    CServiceManager::AcquireSingleton((IServiceManager**)&helper);
    AutoPtr<IInterface> obj;
    helper->CheckService(String("phone"), (IInterface**)&obj);
    AutoPtr<IITelephony> t = IITelephony::Probe(obj);
    AutoPtr<ArrayOf<Int32> > result;
    t->SupplyPukReportResult(mPuk, mPin, (ArrayOf<Int32>**)&result);
    Logger::V(TAG, "supplyPukReportResult returned: %d %d", (*result)[0], (*result)[1]);
    AutoPtr<IRunnable> r = new MyRunnable(this, (*result)[0], (*result)[1]);
    Boolean res;
    ECode ec = mHost->Post(r, &res);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "RemoteException for supplyPukReportResult:%d", ec);
        AutoPtr<IRunnable> r = new MyRunnable2(this);
        Boolean res;
        mHost->Post(r, &res);
    }
    return NOERROR;
}

ECode CKeyguardSimPukView::MyRunnable3::Run()
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
                mHost->GetPukRemainingAttemptsDialog(mAttemptsRemaining)->Show();
            }
            else {
                // show message
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(
                        mHost->GetPukPasswordErrorMessage(mAttemptsRemaining));
                mHost->mSecurityMessageDisplay->SetMessage(cchar, TRUE);
            }
        }
        else {
            AutoPtr<IContext> context;
            mHost->GetContext((IContext**)&context);
            String str;
            context->GetString(R::string::kg_password_puk_failed, &str);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
            mHost->mSecurityMessageDisplay->SetMessage(cchar, TRUE);
        }
        if (DEBUG) Logger::D(TAG, "verifyPasswordAndUnlock UpdateSim.onSimCheckResponse: attemptsRemaining=%d",
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
    Boolean res;
    return mHost->Post(r, &res);
}

const String CKeyguardSimPukView::TAG("KeyguardSimPukView");

const Boolean CKeyguardSimPukView::DEBUG = IKeyguardConstants::DEBUG;

CAR_OBJECT_IMPL(CKeyguardSimPukView)

CKeyguardSimPukView::CKeyguardSimPukView()
    : mPukText(NULL)
    , mPinText(NULL)
{
    mStateMachine = new StateMachine(this);
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
        context->GetResources((IResources**)&resources);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<IInteger32> obj = CoreUtils::Convert(attemptsRemaining);
        array->Set(0, obj);
        resources->GetQuantityString(R::plurals::kg_password_wrong_puk_code,
            attemptsRemaining, array, &displayMessage);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetString(R::string::kg_password_puk_failed, &displayMessage);
    }
    if (DEBUG) Logger::D(TAG, "getPukPasswordErrorMessage: attemptsRemaining=%d displayMessage=%s",
            attemptsRemaining, displayMessage.string());
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

    *id = R::id::pukEntry;
    return NOERROR;
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
        IDialogInterface::Probe(mSimUnlockProgressDialog)->Dismiss();
        mSimUnlockProgressDialog = NULL;
    }
    return NOERROR;
}

AutoPtr<IDialog> CKeyguardSimPukView::GetSimUnlockProgressDialog()
{
    if (mSimUnlockProgressDialog == NULL) {
        CProgressDialog::New(mContext, (IProgressDialog**)&mSimUnlockProgressDialog);
        String massage;
        mContext->GetString(R::string::kg_sim_unlock_progress_dialog_message, &massage);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(massage);
        IAlertDialog::Probe(mSimUnlockProgressDialog)->SetMessage(cchar);
        mSimUnlockProgressDialog->SetIndeterminate(TRUE);
        IDialog::Probe(mSimUnlockProgressDialog)->SetCancelable(FALSE);
        if (IActivity::Probe(mContext) == NULL) {
            AutoPtr<IWindow> window;
            IDialog::Probe(mSimUnlockProgressDialog)->GetWindow((IWindow**)&window);
            window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
        }
    }
    return IDialog::Probe(mSimUnlockProgressDialog);
}

AutoPtr<IDialog> CKeyguardSimPukView::GetPukRemainingAttemptsDialog(
    /* [in] */ Int32 remaining)
{
    String msg = GetPukPasswordErrorMessage(remaining);
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
        IAlertDialog::Probe(mRemainingAttemptsDialog)->SetMessage(cchar);
    }
    return IDialog::Probe(mRemainingAttemptsDialog);
}

Boolean CKeyguardSimPukView::CheckPuk()
{
    // make sure the puk is at least 8 digits long.
    String text;
    mPasswordEntry->GetText(&text);
    if (text.GetLength() == 8) {
        mPasswordEntry->GetText(&mPukText);
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
    *result = mPinText.Equals(text);
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

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
