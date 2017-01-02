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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDABSKEYINPUTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDABSKEYINPUTVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/os/CountDownTimer.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CountDownTimer;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardAbsKeyInputView
    : public LinearLayout
    , public IKeyguardSecurityView
{
private:
    class MyCountDownTimer
        : public CountDownTimer
    {
    public:
        TO_STRING_IMPL("KeyguardAbsKeyInputView::MyCountDownTimer")

        MyCountDownTimer(
            /* [in] */ Int64 millisInFuture,
            /* [in] */ Int64 countDownInterval,
            /* [in] */ KeyguardAbsKeyInputView* host)
            : mHost(host)
        {
            CountDownTimer::constructor(millisInFuture, countDownInterval);
        }

        //@Override
        CARAPI OnTick(
            /* [in] */ Int64 millisUntilFinished);

        //@Override
        CARAPI OnFinish();

    private:
        KeyguardAbsKeyInputView* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardAbsKeyInputView")

    CAR_INTERFACE_DECL()

    KeyguardAbsKeyInputView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    CARAPI Reset();

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI GetCallback(
        /* [out] */ IKeyguardSecurityCallback** _callback);

    // Cause a VIRTUAL_KEY vibration
    CARAPI DoHapticKeyClick();

    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI HideBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

protected:
    // Allow subclasses to override this behavior
    CARAPI ShouldLockout(
        /* [in] */ Int64 deadline,
        /* [out] */ Boolean* result);

    virtual CARAPI GetPasswordTextViewId(
        /* [out] */ Int32* id) = 0;

    virtual CARAPI ResetState() = 0;

    //@Override
    CARAPI OnFinishInflate();

    /*
     * Override this if you have a different string for "wrong password"
     *
     * Note that PIN/PUK have their own implementation of verifyPasswordAndUnlock and so don't need this
     */
    CARAPI GetWrongPasswordStringId(
        /* [out] */ Int32* id);

    CARAPI VerifyPasswordAndUnlock();

    virtual CARAPI ResetPasswordText(
        /* [in] */ Boolean animate) = 0;

    virtual CARAPI GetPasswordText(
        /* [out] */ String* text) = 0;

    virtual CARAPI SetPasswordEntryEnabled(
        /* [in] */ Boolean enabled) = 0;

    // Prevent user from using the PIN/Password entry until scheduled deadline.
    CARAPI HandleAttemptLockout(
        /* [in] */ Int64 elapsedRealtimeDeadline);

protected:
    AutoPtr<IKeyguardSecurityCallback> mCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<ISecurityMessageDisplay> mSecurityMessageDisplay;
    AutoPtr<IView> mEcaView;

    Boolean mEnableHaptics;

    // To avoid accidental lockout due to events while the device in in the pocket, ignore
    // any passwords with length less than or equal to this length.
    static const Int32 MINIMUM_PASSWORD_LENGTH_BEFORE_REPORT;

private:
    AutoPtr<IDrawable> mBouncerFrame;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDABSKEYINPUTVIEW_H__