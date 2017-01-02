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

#ifndef __ELASTOS_DROID_SETTINGS_CONFIRMLOCKPASSWORD_H__
#define __ELASTOS_DROID_SETTINGS_CONFIRMLOCKPASSWORD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/CountDownTimer.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardHelper;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardView;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CountDownTimer;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;

namespace Elastos {
namespace Droid {
namespace Settings {

class ConfirmLockPassword
    : public SettingsActivity
{
public:
    class ConfirmLockPasswordFragment
        : public Fragment
    {
    private:
        class InnerListener
            : public Object
            , public IViewOnClickListener
            , public IOnEditorActionListener
            , public ITextWatcher
        {
        public:
            TO_STRING_IMPL("ConfirmLockPassword::ConfirmLockPasswordFragment::InnerListener")

            CAR_INTERFACE_DECL()

            InnerListener(
                /* [in] */ ConfirmLockPasswordFragment* host);

            CARAPI OnClick(
                /* [in] */ IView* v);

            CARAPI OnEditorAction(
                /* [in] */ ITextView* v,
                /* [in] */ Int32 actionId,
                /* [in] */ IKeyEvent* event,
                /* [out] */ Boolean* result);

            CARAPI BeforeTextChanged(
                /* [in] */ ICharSequence* s,
                /* [in] */ Int32 start,
                /* [in] */ Int32 count,
                /* [in] */ Int32 after);

            CARAPI OnTextChanged(
                /* [in] */ ICharSequence* s,
                /* [in] */ Int32 start,
                /* [in] */ Int32 before,
                /* [in] */ Int32 count);

            CARAPI AfterTextChanged(
                /* [in] */ IEditable* s);

        private:
            ConfirmLockPasswordFragment* mHost;
        };

        class MyRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("ConfirmLockPassword::ConfirmLockPasswordFragment::MyRunnable")

            MyRunnable(
                /* [in] */ ConfirmLockPasswordFragment* host);

            CARAPI Run();

        private:
            ConfirmLockPasswordFragment* mHost;
        };

        class HandleAttemptLockoutCountDownTimer
            : public CountDownTimer
        {
        public:
            TO_STRING_IMPL("ConfirmLockPassword::ConfirmLockPasswordFragment::HandleAttemptLockoutCountDownTimer")

            HandleAttemptLockoutCountDownTimer(
                /* [in] */ ConfirmLockPasswordFragment* host);

            CARAPI constructor(
                /* [in] */ Int64 millisInFuture,
                /* [in] */ Int64 countDownInterval);

            //@Override
            CARAPI OnTick(
                /* [in] */ Int64 millisUntilFinished);

            //@Override
            CARAPI OnFinish();

        private:
            ConfirmLockPasswordFragment* mHost;
        };

    public:
        // required constructor for fragments
        ConfirmLockPasswordFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

        //@Override
        CARAPI OnCreateView(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* container,
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IView** result);

        //@Override
        CARAPI OnPause();

        //@Override
        CARAPI OnResume();

        //@Override
        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

        CARAPI OnClick(
            /* [in] */ IView* v);

        // {@link OnEditorActionListener} methods.
        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // {@link TextWatcher} methods.
        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        CARAPI_(Int32) GetDefaultHeader();

        CARAPI_(void) HandleNext();

        CARAPI_(void) HandleAttemptLockout(
            /* [in] */ Int64 elapsedRealtimeDeadline);

        CARAPI_(void) ShowError(
            /* [in] */ Int32 msg);

        CARAPI_(void) ShowError(
            /* [in] */ Int32 msg,
            /* [in] */ Int64 timeout);

    private:
        static const String KEY_NUM_WRONG_CONFIRM_ATTEMPTS;
        static const Int64 ERROR_MESSAGE_TIMEOUT;
        AutoPtr<ITextView> mPasswordEntry;
        AutoPtr<ILockPatternUtils> mLockPatternUtils;
        AutoPtr<ITextView> mHeaderText;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IPasswordEntryKeyboardHelper> mKeyboardHelper;
        AutoPtr<IPasswordEntryKeyboardView> mKeyboardView;
        AutoPtr<IButton> mContinueButton;
        Int32 mNumWrongConfirmAttempts;
        AutoPtr<HandleAttemptLockoutCountDownTimer> mCountdownTimer;
        Boolean mIsAlpha;

        AutoPtr<MyRunnable> mResetErrorRunnable;
    };

public:
    ConfirmLockPassword();

    ~ConfirmLockPassword();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** result);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    static const String PACKAGE;
    static const String HEADER_TEXT;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

namespace Elastos {
namespace Droid {
namespace Settings {

class ConfirmLockPasswordInternalActivity
    : public ConfirmLockPassword
    , public IConfirmLockPasswordInternalActivity
{
public:
    CAR_INTERFACE_DECL()

    ConfirmLockPasswordInternalActivity();

    ~ConfirmLockPasswordInternalActivity();

    CARAPI constructor();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CONFIRMLOCKPASSWORD_H__