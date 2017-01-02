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

#include "elastos/droid/systemui/keyguard/KeyguardAbsKeyInputView.h"
#include "elastos/droid/systemui/keyguard/CKeyguardMessageArea.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/systemui/keyguard/KeyguardSecurityViewHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardAbsKeyInputView::MyCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    Int32 secondsRemaining = (Int32) (millisUntilFinished / 1000);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<IInteger32> obj = CoreUtils::Convert(secondsRemaining);
    array->Set(0, TO_IINTERFACE(obj));
    return mHost->mSecurityMessageDisplay->SetMessage(
            R::string::kg_too_many_failed_attempts_countdown,
            TRUE, array);
}

ECode KeyguardAbsKeyInputView::MyCountDownTimer::OnFinish()
{
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
    mHost->mSecurityMessageDisplay->SetMessage(cchar, FALSE);
    return mHost->ResetState();
}

const Int32 KeyguardAbsKeyInputView::MINIMUM_PASSWORD_LENGTH_BEFORE_REPORT = 3;

CAR_INTERFACE_IMPL(KeyguardAbsKeyInputView, LinearLayout, IKeyguardSecurityView)

KeyguardAbsKeyInputView::KeyguardAbsKeyInputView()
    : mEnableHaptics(FALSE)
{

}

ECode KeyguardAbsKeyInputView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode KeyguardAbsKeyInputView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode KeyguardAbsKeyInputView::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    mLockPatternUtils->IsTactileFeedbackEnabled(&mEnableHaptics);
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::Reset()
{
    // start fresh
    ResetPasswordText(FALSE /* animate */);
    // if the user is currently locked out, enforce it.
    Int64 deadline;
    mLockPatternUtils->GetLockoutAttemptDeadline(&deadline);

    Boolean res;
    ShouldLockout(deadline, &res);
    if (res) {
        HandleAttemptLockout(deadline);
    }
    else {
        ResetState();
    }
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::ShouldLockout(
    /* [in] */ Int64 deadline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = deadline != 0;
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::OnFinishInflate()
{
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&mLockPatternUtils);

    AutoPtr<CKeyguardMessageArea::Helper> helper =
            new CKeyguardMessageArea::Helper();
    helper->constructor(this);
    mSecurityMessageDisplay = ISecurityMessageDisplay::Probe(helper);
    FindViewById(R::id::keyguard_selector_fade_container, (IView**)&mEcaView);
    AutoPtr<IView> bouncerFrameView;
    FindViewById(R::id::keyguard_bouncer_frame, (IView**)&bouncerFrameView);
    if (bouncerFrameView != NULL) {
        bouncerFrameView->GetBackground((IDrawable**)&mBouncerFrame);
    }
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::GetWrongPasswordStringId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = R::string::kg_wrong_password;
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::VerifyPasswordAndUnlock()
{
    String entry;
    GetPasswordText(&entry);
    Boolean res;
    if (mLockPatternUtils->CheckPassword(entry, &res), res) {
        mCallback->ReportUnlockAttempt(TRUE);
        mCallback->Dismiss(TRUE);
    }
    else {
        if (entry.GetLength() > MINIMUM_PASSWORD_LENGTH_BEFORE_REPORT ) {
            // to avoid accidental lockout, only count attempts that are long enough to be a
            // real password. This may require some tweaking.
            mCallback->ReportUnlockAttempt(FALSE);
            AutoPtr<IKeyguardUpdateMonitor> monitor =
                    KeyguardUpdateMonitor::GetInstance(mContext);
            Int32 attempts;
            monitor->GetFailedUnlockAttempts(&attempts);
            if (0 == (attempts % ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT)) {
                Int64 deadline;
                mLockPatternUtils->SetLockoutAttemptDeadline(&deadline);
                HandleAttemptLockout(deadline);
            }
        }
        Int32 id;
        GetWrongPasswordStringId(&id);
        mSecurityMessageDisplay->SetMessage(id, TRUE);
    }
    ResetPasswordText(TRUE /* animate */);
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::HandleAttemptLockout(
    /* [in] */ Int64 elapsedRealtimeDeadline)
{
    SetPasswordEntryEnabled(FALSE);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    AutoPtr<MyCountDownTimer> timer = new MyCountDownTimer(
            elapsedRealtimeDeadline - elapsedRealtime, 1000, this);
    return timer->Start();
}

ECode KeyguardAbsKeyInputView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mCallback->UserActivity();
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::OnPause()
{
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::OnResume(
    /* [in] */ Int32 reason)
{
    return Reset();
}

ECode KeyguardAbsKeyInputView::GetCallback(
    /* [out] */ IKeyguardSecurityCallback** _callback)
{
    VALIDATE_NOT_NULL(_callback)

    *_callback = mCallback;;
    REFCOUNT_ADD(*_callback);
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::DoHapticKeyClick()
{
    if (mEnableHaptics) {
        Boolean res;
        PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY,
                IHapticFeedbackConstants::FLAG_IGNORE_VIEW_SETTING
                | IHapticFeedbackConstants::FLAG_IGNORE_GLOBAL_SETTING, &res);
    }
    return NOERROR;
}

ECode KeyguardAbsKeyInputView::ShowBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::ShowBouncer(mSecurityMessageDisplay,
            mEcaView, mBouncerFrame, duration);
}

ECode KeyguardAbsKeyInputView::HideBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::HideBouncer(mSecurityMessageDisplay,
            mEcaView, mBouncerFrame, duration);
}

ECode KeyguardAbsKeyInputView::StartDisappearAnimation(
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
