/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardHelper;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardView;
using Elastos::Droid::Settings::Notification::IRedactionInterstitial;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Inputmethodservice::IKeyboardView;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::Inputmethod::IEditorInfo;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TextView::IOnEditorActionListener;

public class ChooseLockPassword extends SettingsActivity {
    public static const String PASSWORD_MIN_KEY = "lockscreen.password_min";
    public static const String PASSWORD_MAX_KEY = "lockscreen.password_max";
    public static const String PASSWORD_MIN_LETTERS_KEY = "lockscreen.password_min_letters";
    public static const String PASSWORD_MIN_LOWERCASE_KEY = "lockscreen.password_min_lowercase";
    public static const String PASSWORD_MIN_UPPERCASE_KEY = "lockscreen.password_min_uppercase";
    public static const String PASSWORD_MIN_NUMERIC_KEY = "lockscreen.password_min_numeric";
    public static const String PASSWORD_MIN_SYMBOLS_KEY = "lockscreen.password_min_symbols";
    public static const String PASSWORD_MIN_NONLETTER_KEY = "lockscreen.password_min_nonletter";

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, ChooseLockPasswordFragment.class->GetName());
        return modIntent;
    }

    public static Intent CreateIntent(Context context, Int32 quality, final Boolean isFallback,
            Int32 minLength, final Int32 maxLength, Boolean requirePasswordToDecrypt,
            Boolean confirmCredentials) {
        Intent intent = new Intent()->SetClass(context, ChooseLockPassword.class);
        intent->PutExtra(LockPatternUtils.PASSWORD_TYPE_KEY, quality);
        intent->PutExtra(PASSWORD_MIN_KEY, minLength);
        intent->PutExtra(PASSWORD_MAX_KEY, maxLength);
        intent->PutExtra(CChooseLockGeneric::CONFIRM_CREDENTIALS, confirmCredentials);
        intent->PutExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, isFallback);
        intent->PutExtra(CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, requirePasswordToDecrypt);
        return intent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (ChooseLockPasswordFragment.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        // TODO: Fix on phones
        // Disable IME on our window since we provide our own keyboard
        //GetWindow()->SetFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
                //WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
        super->OnCreate(savedInstanceState);
        CharSequence msg = GetText(R::string::lockpassword_choose_your_password_header);
        SetTitle(msg);
    }

    public static class ChooseLockPasswordFragment extends Fragment
            implements OnClickListener, OnEditorActionListener,  TextWatcher {
        private static const String KEY_FIRST_PIN = "first_pin";
        private static const String KEY_UI_STAGE = "ui_stage";
        private TextView mPasswordEntry;
        private Int32 mPasswordMinLength = 4;
        private Int32 mPasswordMaxLength = 16;
        private Int32 mPasswordMinLetters = 0;
        private Int32 mPasswordMinUpperCase = 0;
        private Int32 mPasswordMinLowerCase = 0;
        private Int32 mPasswordMinSymbols = 0;
        private Int32 mPasswordMinNumeric = 0;
        private Int32 mPasswordMinNonLetter = 0;
        private LockPatternUtils mLockPatternUtils;
        private Int32 mRequestedQuality = DevicePolicyManager.PASSWORD_QUALITY_NUMERIC;
        private ChooseLockSettingsHelper mChooseLockSettingsHelper;
        private Stage mUiStage = Stage.Introduction;
        private Boolean mDone = FALSE;
        private TextView mHeaderText;
        private String mFirstPin;
        private KeyboardView mKeyboardView;
        private PasswordEntryKeyboardHelper mKeyboardHelper;
        private Boolean mIsAlphaMode;
        private Button mCancelButton;
        private Button mNextButton;
        private static const Int32 CONFIRM_EXISTING_REQUEST = 58;
        static const Int32 RESULT_FINISHED = RESULT_FIRST_USER;
        private static const Int64 ERROR_MESSAGE_TIMEOUT = 3000;
        private static const Int32 MSG_SHOW_ERROR = 1;

        private Handler mHandler = new Handler() {
            //@Override
            CARAPI HandleMessage(Message msg) {
                if (msg.what == MSG_SHOW_ERROR) {
                    UpdateStage((Stage) msg.obj);
                }
            }
        };

        /**
         * Keep track internally of where the user is in choosing a pattern.
         */
        protected enum Stage {

            Introduction(R::string::lockpassword_choose_your_password_header,
                    R::string::lockpassword_choose_your_pin_header,
                    R::string::lockpassword_continue_label),

            NeedToConfirm(R::string::lockpassword_confirm_your_password_header,
                    R::string::lockpassword_confirm_your_pin_header,
                    R::string::lockpassword_ok_label),

            ConfirmWrong(R::string::lockpassword_confirm_passwords_dont_match,
                    R::string::lockpassword_confirm_pins_dont_match,
                    R::string::lockpassword_continue_label);

            Stage(Int32 hintInAlpha, Int32 hintInNumeric, Int32 nextButtonText) {
                this.alphaHint = hintInAlpha;
                this.numericHint = hintInNumeric;
                this.buttonText = nextButtonText;
            }

            public final Int32 alphaHint;
            public final Int32 numericHint;
            public final Int32 buttonText;
        }

        // required constructor for fragments
        public ChooseLockPasswordFragment() {

        }

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);
            mLockPatternUtils = new LockPatternUtils(GetActivity());
            Intent intent = GetActivity()->GetIntent();
            if (!(GetActivity() instanceof ChooseLockPassword)) {
                throw new SecurityException("Fragment contained in wrong activity");
            }
            mRequestedQuality = Math->Max(intent->GetIntExtra(LockPatternUtils.PASSWORD_TYPE_KEY,
                    mRequestedQuality), mLockPatternUtils->GetRequestedPasswordQuality());
            mPasswordMinLength = Math->Max(
                    intent->GetIntExtra(PASSWORD_MIN_KEY, mPasswordMinLength), mLockPatternUtils
                            .GetRequestedMinimumPasswordLength());
            mPasswordMaxLength = intent->GetIntExtra(PASSWORD_MAX_KEY, mPasswordMaxLength);
            mPasswordMinLetters = Math->Max(intent->GetIntExtra(PASSWORD_MIN_LETTERS_KEY,
                    mPasswordMinLetters), mLockPatternUtils->GetRequestedPasswordMinimumLetters());
            mPasswordMinUpperCase = Math->Max(intent->GetIntExtra(PASSWORD_MIN_UPPERCASE_KEY,
                    mPasswordMinUpperCase), mLockPatternUtils->GetRequestedPasswordMinimumUpperCase());
            mPasswordMinLowerCase = Math->Max(intent->GetIntExtra(PASSWORD_MIN_LOWERCASE_KEY,
                    mPasswordMinLowerCase), mLockPatternUtils->GetRequestedPasswordMinimumLowerCase());
            mPasswordMinNumeric = Math->Max(intent->GetIntExtra(PASSWORD_MIN_NUMERIC_KEY,
                    mPasswordMinNumeric), mLockPatternUtils->GetRequestedPasswordMinimumNumeric());
            mPasswordMinSymbols = Math->Max(intent->GetIntExtra(PASSWORD_MIN_SYMBOLS_KEY,
                    mPasswordMinSymbols), mLockPatternUtils->GetRequestedPasswordMinimumSymbols());
            mPasswordMinNonLetter = Math->Max(intent->GetIntExtra(PASSWORD_MIN_NONLETTER_KEY,
                    mPasswordMinNonLetter), mLockPatternUtils->GetRequestedPasswordMinimumNonLetter());

            mChooseLockSettingsHelper = new ChooseLockSettingsHelper(GetActivity());
        }

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {

            View view = inflater->Inflate(R.layout.choose_lock_password, NULL);

            mCancelButton = (Button) view->FindViewById(R.id.cancel_button);
            mCancelButton->SetOnClickListener(this);
            mNextButton = (Button) view->FindViewById(R.id.next_button);
            mNextButton->SetOnClickListener(this);

            mIsAlphaMode = DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC == mRequestedQuality
                    || DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC == mRequestedQuality
                    || DevicePolicyManager.PASSWORD_QUALITY_COMPLEX == mRequestedQuality;
            mKeyboardView = (PasswordEntryKeyboardView) view->FindViewById(R.id.keyboard);
            mPasswordEntry = (TextView) view->FindViewById(R.id.password_entry);
            mPasswordEntry->SetOnEditorActionListener(this);
            mPasswordEntry->AddTextChangedListener(this);

            final Activity activity = GetActivity();
            mKeyboardHelper = new PasswordEntryKeyboardHelper(activity,
                    mKeyboardView, mPasswordEntry);
            mKeyboardHelper->SetKeyboardMode(mIsAlphaMode ?
                    PasswordEntryKeyboardHelper.KEYBOARD_MODE_ALPHA
                    : PasswordEntryKeyboardHelper.KEYBOARD_MODE_NUMERIC);

            mHeaderText = (TextView) view->FindViewById(R.id.headerText);
            mKeyboardView->RequestFocus();

            Int32 currentType = mPasswordEntry->GetInputType();
            mPasswordEntry->SetInputType(mIsAlphaMode ? currentType
                    : (InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_PASSWORD));

            Intent intent = GetActivity()->GetIntent();
            final Boolean confirmCredentials = intent->GetBooleanExtra("confirm_credentials", TRUE);
            if (savedInstanceState == NULL) {
                UpdateStage(Stage.Introduction);
                if (confirmCredentials) {
                    mChooseLockSettingsHelper->LaunchConfirmationActivity(CONFIRM_EXISTING_REQUEST,
                            NULL, NULL);
                }
            } else {
                mFirstPin = savedInstanceState->GetString(KEY_FIRST_PIN);
                final String state = savedInstanceState->GetString(KEY_UI_STAGE);
                if (state != NULL) {
                    mUiStage = Stage->ValueOf(state);
                    UpdateStage(mUiStage);
                }
            }
            mDone = FALSE;
            if (activity instanceof SettingsActivity) {
                final SettingsActivity sa = (SettingsActivity) activity;
                Int32 id = mIsAlphaMode ? R::string::lockpassword_choose_your_password_header
                        : R::string::lockpassword_choose_your_pin_header;
                CharSequence title = GetText(id);
                sa->SetTitle(title);
            }

            return view;
        }

        //@Override
        CARAPI OnResume() {
            super->OnResume();
            UpdateStage(mUiStage);
            mKeyboardView->RequestFocus();
        }

        //@Override
        CARAPI OnPause() {
            mHandler->RemoveMessages(MSG_SHOW_ERROR);

            super->OnPause();
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);
            outState->PutString(KEY_UI_STAGE, mUiStage->Name());
            outState->PutString(KEY_FIRST_PIN, mFirstPin);
        }

        //@Override
        CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode,
                Intent data) {
            super->OnActivityResult(requestCode, resultCode, data);
            switch (requestCode) {
                case CONFIRM_EXISTING_REQUEST:
                    if (resultCode != Activity.RESULT_OK) {
                        GetActivity()->SetResult(RESULT_FINISHED);
                        GetActivity()->Finish();
                    }
                    break;
            }
        }

        protected void UpdateStage(Stage stage) {
            final Stage previousStage = mUiStage;
            mUiStage = stage;
            UpdateUi();

            // If the stage changed, announce the header for accessibility. This
            // is a no-op when accessibility is disabled.
            if (previousStage != stage) {
                mHeaderText->AnnounceForAccessibility(mHeaderText->GetText());
            }
        }

        /**
         * Validates PIN and returns a message to display if PIN fails test.
         * @param password the raw password the user typed in
         * @return error message to show to user or NULL if password is OK
         */
        private String ValidatePassword(String password) {
            if (password->Length() < mPasswordMinLength) {
                return GetString(mIsAlphaMode ?
                        R::string::lockpassword_password_too_short
                        : R::string::lockpassword_pin_too_short, mPasswordMinLength);
            }
            if (password->Length() > mPasswordMaxLength) {
                return GetString(mIsAlphaMode ?
                        R::string::lockpassword_password_too_long
                        : R::string::lockpassword_pin_too_long, mPasswordMaxLength + 1);
            }
            Int32 letters = 0;
            Int32 numbers = 0;
            Int32 lowercase = 0;
            Int32 symbols = 0;
            Int32 uppercase = 0;
            Int32 nonletter = 0;
            for (Int32 i = 0; i < password->Length(); i++) {
                Char32 c = password->CharAt(i);
                // allow non control Latin-1 characters only
                if (c < 32 || c > 127) {
                    return GetString(R::string::lockpassword_illegal_character);
                }
                if (c >= '0' && c <= '9') {
                    numbers++;
                    nonletter++;
                } else if (c >= 'A' && c <= 'Z') {
                    letters++;
                    uppercase++;
                } else if (c >= 'a' && c <= 'z') {
                    letters++;
                    lowercase++;
                } else {
                    symbols++;
                    nonletter++;
                }
            }
            if (DevicePolicyManager.PASSWORD_QUALITY_NUMERIC == mRequestedQuality
                    || DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality) {
                if (letters > 0 || symbols > 0) {
                    // This shouldn't be possible unless user finds some way to bring up
                    // soft keyboard
                    return GetString(R::string::lockpassword_pin_contains_non_digits);
                }
                // Check for repeated characters or sequences (e.g. '1234', '0000', '2468')
                final Int32 sequence = LockPatternUtils->MaxLengthSequence(password);
                if (DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality
                        && sequence > LockPatternUtils.MAX_ALLOWED_SEQUENCE) {
                    return GetString(R::string::lockpassword_pin_no_sequential_digits);
                }
            } else if (DevicePolicyManager.PASSWORD_QUALITY_COMPLEX == mRequestedQuality) {
                if (letters < mPasswordMinLetters) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_letters, mPasswordMinLetters),
                            mPasswordMinLetters);
                } else if (numbers < mPasswordMinNumeric) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_numeric, mPasswordMinNumeric),
                            mPasswordMinNumeric);
                } else if (lowercase < mPasswordMinLowerCase) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_lowercase, mPasswordMinLowerCase),
                            mPasswordMinLowerCase);
                } else if (uppercase < mPasswordMinUpperCase) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_uppercase, mPasswordMinUpperCase),
                            mPasswordMinUpperCase);
                } else if (symbols < mPasswordMinSymbols) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_symbols, mPasswordMinSymbols),
                            mPasswordMinSymbols);
                } else if (nonletter < mPasswordMinNonLetter) {
                    return String->Format(GetResources()->GetQuantityString(
                            R.plurals.lockpassword_password_requires_nonletter, mPasswordMinNonLetter),
                            mPasswordMinNonLetter);
                }
            } else {
                final Boolean alphabetic = DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC
                        == mRequestedQuality;
                final Boolean alphanumeric = DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC
                        == mRequestedQuality;
                if ((alphabetic || alphanumeric) && letters == 0) {
                    return GetString(R::string::lockpassword_password_requires_alpha);
                }
                if (alphanumeric && numbers == 0) {
                    return GetString(R::string::lockpassword_password_requires_digit);
                }
            }
            If(mLockPatternUtils->CheckPasswordHistory(password)) {
                return GetString(mIsAlphaMode ? R::string::lockpassword_password_recently_used
                        : R::string::lockpassword_pin_recently_used);
            }

            return NULL;
        }

        private void HandleNext() {
            if (mDone) return;

            final String pin = mPasswordEntry->GetText()->ToString();
            if (TextUtils->IsEmpty(pin)) {
                return;
            }
            String errorMsg = NULL;
            if (mUiStage == Stage.Introduction) {
                errorMsg = ValidatePassword(pin);
                if (errorMsg == NULL) {
                    mFirstPin = pin;
                    mPasswordEntry->SetText("");
                    UpdateStage(Stage.NeedToConfirm);
                }
            } else if (mUiStage == Stage.NeedToConfirm) {
                if (mFirstPin->Equals(pin)) {
                    final Boolean isFallback = GetActivity()->GetIntent().GetBooleanExtra(
                            LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE);
                    mLockPatternUtils->ClearLock(isFallback);
                    final Boolean required = GetActivity()->GetIntent().GetBooleanExtra(
                            CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, TRUE);
                    mLockPatternUtils->SetCredentialRequiredToDecrypt(required);
                    mLockPatternUtils->SaveLockPassword(pin, mRequestedQuality, isFallback);
                    GetActivity()->SetResult(RESULT_FINISHED);
                    GetActivity()->Finish();
                    mDone = TRUE;
                    StartActivity(CRedactionInterstitial::CreateStartIntent(GetActivity()));
                } else {
                    CharSequence tmp = mPasswordEntry->GetText();
                    if (tmp != NULL) {
                        Selection->SetSelection((Spannable) tmp, 0, tmp->Length());
                    }
                    UpdateStage(Stage.ConfirmWrong);
                }
            }
            if (errorMsg != NULL) {
                ShowError(errorMsg, mUiStage);
            }
        }

        CARAPI OnClick(View v) {
            switch (v->GetId()) {
                case R.id.next_button:
                    HandleNext();
                    break;

                case R.id.cancel_button:
                    GetActivity()->Finish();
                    break;
            }
        }

        private void ShowError(String msg, final Stage next) {
            mHeaderText->SetText(msg);
            mHeaderText->AnnounceForAccessibility(mHeaderText->GetText());
            Message mesg = mHandler->ObtainMessage(MSG_SHOW_ERROR, next);
            mHandler->RemoveMessages(MSG_SHOW_ERROR);
            mHandler->SendMessageDelayed(mesg, ERROR_MESSAGE_TIMEOUT);
        }

        public Boolean OnEditorAction(TextView v, Int32 actionId, KeyEvent event) {
            // Check if this was the result of hitting the enter or "done" key
            if (actionId == EditorInfo.IME_NULL
                    || actionId == EditorInfo.IME_ACTION_DONE
                    || actionId == EditorInfo.IME_ACTION_NEXT) {
                HandleNext();
                return TRUE;
            }
            return FALSE;
        }

        /**
         * Update the hint based on current Stage and length of password entry
         */
        private void UpdateUi() {
            String password = mPasswordEntry->GetText()->ToString();
            final Int32 length = password->Length();
            if (mUiStage == Stage.Introduction && length > 0) {
                if (length < mPasswordMinLength) {
                    String msg = GetString(mIsAlphaMode ? R::string::lockpassword_password_too_short
                            : R::string::lockpassword_pin_too_short, mPasswordMinLength);
                    mHeaderText->SetText(msg);
                    mNextButton->SetEnabled(FALSE);
                } else {
                    String error = ValidatePassword(password);
                    if (error != NULL) {
                        mHeaderText->SetText(error);
                        mNextButton->SetEnabled(FALSE);
                    } else {
                        mHeaderText->SetText(R::string::lockpassword_press_continue);
                        mNextButton->SetEnabled(TRUE);
                    }
                }
            } else {
                mHeaderText->SetText(mIsAlphaMode ? mUiStage.alphaHint : mUiStage.numericHint);
                mNextButton->SetEnabled(length > 0);
            }
            mNextButton->SetText(mUiStage.buttonText);
        }

        CARAPI AfterTextChanged(Editable s) {
            // Changing the text while error displayed resets to NeedToConfirm state
            if (mUiStage == Stage.ConfirmWrong) {
                mUiStage = Stage.NeedToConfirm;
            }
            UpdateUi();
        }

        CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {

        }

        CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {

        }
    }
}
