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

#ifndef __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORD_H__
#define __ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORD_H__

#include "Elastos.Droid.InputMethodService.h"
#include "Elastos.Droid.Text.h"
#include "_Elastos_Droid_Settings_CChooseLockPassword.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::ChooseLockSettingsHelper;
using Elastos::Droid::Settings::IChooseLockPassword;
using Elastos::Droid::Settings::SettingsActivity;
using Elastos::Droid::Settings::IPasswordStage;

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::InputMethodService::IKeyboardView;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CChooseLockPassword)
    , public SettingsActivity
    , public IChooseLockPassword
{
public:
    class ChooseLockPasswordFragment
        : public Fragment
    {
    protected:
        /**
         * Keep track internally of where the user is in choosing a pattern.
         */
        class Stage
            : public Object
            , public IPasswordStage
        {
        public:
            TO_STRING_IMPL("CChooseLockPassword::ChooseLockPasswordFragment::Stage")

            CAR_INTERFACE_DECL()

            CARAPI_(String) GetName();

            CARAPI_(Int32) GetOrdinal();

            static CARAPI_(AutoPtr<Stage>) ValueOf(
                /* [in] */ const String& name);

        private:
            Stage(
                /* [in] */ const String& name,
                /* [in] */ Int32 ordinal,
                /* [in] */ Int32 hintInAlpha,
                /* [in] */ Int32 hintInNumeric,
                /* [in] */ Int32 nextButtonText);

        public:
            static AutoPtr<Stage> INTRODUCTION;
            static AutoPtr<Stage> NEED_TO_CONFIRM;
            static AutoPtr<Stage> CONFIRM_WRONG;

        public:
            Int32 mAlphaHint;
            Int32 mNumericHint;
            Int32 mButtonText;

        private:
            String mName;
            Int32 mOrdinal;
        };

    private:
        class InnerListener
            : public Object
            , public IViewOnClickListener
            , public IOnEditorActionListener
            , public ITextWatcher
        {
        public:
            TO_STRING_IMPL("CChooseLockPassword::ChooseLockPasswordFragment::InnerListener")

            CAR_INTERFACE_DECL()

            InnerListener(
                /* [in] */ ChooseLockPasswordFragment* host);

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
            ChooseLockPasswordFragment* mHost;
        };

        class MyHandler
            : public Handler
        {
        public:
            TO_STRING_IMPL("CChooseLockPassword::ChooseLockPasswordFragment::MyHandler");

            MyHandler(
                /* [in] */ ChooseLockPasswordFragment* host);

            //@Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            ChooseLockPasswordFragment* mHost;
        };

    public:
        // required constructor for fragments
        ChooseLockPasswordFragment();

        ~ChooseLockPasswordFragment();

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
        CARAPI OnResume();

        //@Override
        CARAPI OnPause();

        //@Override
        CARAPI OnSaveInstanceState(
            /* [in] */ IBundle* outState);

        //@Override
        CARAPI OnActivityResult(
            /* [in] */ Int32 requestCode,
            /* [in] */ Int32 resultCode,
            /* [in] */ IIntent* data);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

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

    protected:
        CARAPI_(void) UpdateStage(
            /* [in] */ Stage* stage);

    private:
        /**
         * Validates PIN and returns a message to display if PIN fails test.
         * @param password the raw password the user typed in
         * @return error message to show to user or NULL if password is OK
         */
        CARAPI_(String) ValidatePassword(
            /* [in] */ const String& password);

        CARAPI_(void) HandleNext();

        CARAPI_(void) ShowError(
            /* [in] */ const String& msg,
            /* [in] */ Stage* next);

        /**
         * Update the hint based on current Stage and length of password entry
         */
        CARAPI_(void) UpdateUi();

    protected:
        static const Int32 RESULT_FINISHED;

    private:
        static const String KEY_FIRST_PIN;
        static const String KEY_UI_STAGE;
        static const Int32 CONFIRM_EXISTING_REQUEST;
        static const Int64 ERROR_MESSAGE_TIMEOUT;
        static const Int32 MSG_SHOW_ERROR;

        AutoPtr<ITextView> mPasswordEntry;
        Int32 mPasswordMinLength;
        Int32 mPasswordMaxLength;
        Int32 mPasswordMinLetters;
        Int32 mPasswordMinUpperCase;
        Int32 mPasswordMinLowerCase;
        Int32 mPasswordMinSymbols;
        Int32 mPasswordMinNumeric;
        Int32 mPasswordMinNonLetter;
        AutoPtr<ILockPatternUtils> mLockPatternUtils;
        Int32 mRequestedQuality;
        AutoPtr<ChooseLockSettingsHelper> mChooseLockSettingsHelper;
        AutoPtr<Stage> mUiStage;
        Boolean mDone;
        AutoPtr<ITextView> mHeaderText;
        String mFirstPin;
        AutoPtr<IKeyboardView> mKeyboardView;
        AutoPtr<IPasswordEntryKeyboardHelper> mKeyboardHelper;
        Boolean mIsAlphaMode;
        AutoPtr<IButton> mCancelButton;
        AutoPtr<IButton> mNextButton;

        AutoPtr<MyHandler> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CChooseLockPassword();

    ~CChooseLockPassword();

    CARAPI constructor();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** result);

    static CARAPI_(AutoPtr<IIntent>) CreateIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int32 quality,
        /* [in] */ Boolean isFallback,
        /* [in] */ Int32 minLength,
        /* [in] */ Int32 maxLength,
        /* [in] */ Boolean requirePasswordToDecrypt,
        /* [in] */ Boolean confirmCredentials);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

protected:
    //@Override
    virtual CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

public:
    static const String PASSWORD_MIN_KEY;
    static const String PASSWORD_MAX_KEY;
    static const String PASSWORD_MIN_LETTERS_KEY;
    static const String PASSWORD_MIN_LOWERCASE_KEY;
    static const String PASSWORD_MIN_UPPERCASE_KEY;
    static const String PASSWORD_MIN_NUMERIC_KEY;
    static const String PASSWORD_MIN_SYMBOLS_KEY;
    static const String PASSWORD_MIN_NONLETTER_KEY;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCHOOSELOCKPASSWORD_H__