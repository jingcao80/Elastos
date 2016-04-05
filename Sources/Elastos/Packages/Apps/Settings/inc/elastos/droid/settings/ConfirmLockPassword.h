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

using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardHelper;
using Elastos::Droid::Internal::Widget::IPasswordEntryKeyboardView;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ICountDownTimer;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Inputmethod::IEditorInfo;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TextView::IOnEditorActionListener;

public class ConfirmLockPassword extends SettingsActivity {

    public static const String PACKAGE = "com.android.settings";
    public static const String HEADER_TEXT = PACKAGE + ".ConfirmLockPattern.header";

    public static class InternalActivity extends ConfirmLockPassword {
    }

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, ConfirmLockPasswordFragment.class->GetName());
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (ConfirmLockPasswordFragment.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        // Disable IME on our window since we provide our own keyboard
        //GetWindow()->SetFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM,
                //WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
        super->OnCreate(savedInstanceState);
        CharSequence msg = GetText(R::string::lockpassword_confirm_your_password_header);
        SetTitle(msg);
    }

    public static class ConfirmLockPasswordFragment extends Fragment implements OnClickListener,
            OnEditorActionListener, TextWatcher {
        private static const String KEY_NUM_WRONG_CONFIRM_ATTEMPTS
                = "confirm_lock_password_fragment.key_num_wrong_confirm_attempts";
        private static const Int64 ERROR_MESSAGE_TIMEOUT = 3000;
        private TextView mPasswordEntry;
        private LockPatternUtils mLockPatternUtils;
        private TextView mHeaderText;
        private Handler mHandler = new Handler();
        private PasswordEntryKeyboardHelper mKeyboardHelper;
        private PasswordEntryKeyboardView mKeyboardView;
        private Button mContinueButton;
        private Int32 mNumWrongConfirmAttempts;
        private CountDownTimer mCountdownTimer;
        private Boolean mIsAlpha;

        // required constructor for fragments
        public ConfirmLockPasswordFragment() {

        }

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);
            mLockPatternUtils = new LockPatternUtils(GetActivity());
            if (savedInstanceState != NULL) {
                mNumWrongConfirmAttempts = savedInstanceState->GetInt(
                        KEY_NUM_WRONG_CONFIRM_ATTEMPTS, 0);
            }
        }

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            final Int32 storedQuality = mLockPatternUtils->GetKeyguardStoredPasswordQuality();
            View view = inflater->Inflate(R.layout.confirm_lock_password, NULL);
            // Disable IME on our window since we provide our own keyboard

            view->FindViewById(R.id.cancel_button).SetOnClickListener(this);
            mContinueButton = (Button) view->FindViewById(R.id.next_button);
            mContinueButton->SetOnClickListener(this);
            mContinueButton->SetEnabled(FALSE); // disable until the user enters at least one Char32

            mPasswordEntry = (TextView) view->FindViewById(R.id.password_entry);
            mPasswordEntry->SetOnEditorActionListener(this);
            mPasswordEntry->AddTextChangedListener(this);

            mKeyboardView = (PasswordEntryKeyboardView) view->FindViewById(R.id.keyboard);
            mHeaderText = (TextView) view->FindViewById(R.id.headerText);
            mIsAlpha = DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC == storedQuality
                    || DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC == storedQuality
                    || DevicePolicyManager.PASSWORD_QUALITY_COMPLEX == storedQuality;

            Intent intent = GetActivity()->GetIntent();
            if (intent != NULL) {
                CharSequence headerMessage = intent->GetCharSequenceExtra(HEADER_TEXT);
                if (TextUtils->IsEmpty(headerMessage)) {
                    headerMessage = GetString(GetDefaultHeader());
                }
                mHeaderText->SetText(headerMessage);
            }

            final Activity activity = GetActivity();
            mKeyboardHelper = new PasswordEntryKeyboardHelper(activity,
                    mKeyboardView, mPasswordEntry);
            mKeyboardHelper->SetKeyboardMode(mIsAlpha ?
                    PasswordEntryKeyboardHelper.KEYBOARD_MODE_ALPHA
                    : PasswordEntryKeyboardHelper.KEYBOARD_MODE_NUMERIC);
            mKeyboardView->RequestFocus();

            Int32 currentType = mPasswordEntry->GetInputType();
            mPasswordEntry->SetInputType(mIsAlpha ? currentType
                    : (InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_PASSWORD));

            if (activity instanceof SettingsActivity) {
                final SettingsActivity sa = (SettingsActivity) activity;
                Int32 id = GetDefaultHeader();
                CharSequence title = GetText(id);
                sa->SetTitle(title);
            }

            return view;
        }

        private Int32 GetDefaultHeader() {
            return mIsAlpha ? R::string::lockpassword_confirm_your_password_header
                    : R::string::lockpassword_confirm_your_pin_header;
        }

        //@Override
        CARAPI OnPause() {
            super->OnPause();
            mKeyboardView->RequestFocus();
            if (mCountdownTimer != NULL) {
                mCountdownTimer->Cancel();
                mCountdownTimer = NULL;
            }
        }

        //@Override
        CARAPI OnResume() {
            // TODO Auto-generated method stub
            super->OnResume();
            mKeyboardView->RequestFocus();
            Int64 deadline = mLockPatternUtils->GetLockoutAttemptDeadline();
            if (deadline != 0) {
                HandleAttemptLockout(deadline);
            }
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);
            outState->PutInt(KEY_NUM_WRONG_CONFIRM_ATTEMPTS, mNumWrongConfirmAttempts);
        }

        private void HandleNext() {
            final String pin = mPasswordEntry->GetText()->ToString();
            if (mLockPatternUtils->CheckPassword(pin)) {

                Intent intent = new Intent();
                if (GetActivity() instanceof ConfirmLockPassword.InternalActivity) {
                    intent->PutExtra(ChooseLockSettingsHelper.EXTRA_KEY_TYPE,
                                    mIsAlpha ? StorageManager.CRYPT_TYPE_PASSWORD
                                             : StorageManager.CRYPT_TYPE_PIN);
                    intent->PutExtra(ChooseLockSettingsHelper.EXTRA_KEY_PASSWORD, pin);
                }

                GetActivity()->SetResult(RESULT_OK, intent);
                GetActivity()->Finish();
            } else {
                if (++mNumWrongConfirmAttempts >= LockPatternUtils.FAILED_ATTEMPTS_BEFORE_TIMEOUT) {
                    Int64 deadline = mLockPatternUtils->SetLockoutAttemptDeadline();
                    HandleAttemptLockout(deadline);
                } else {
                    ShowError(R::string::lockpattern_need_to_unlock_wrong);
                }
            }
        }

        private void HandleAttemptLockout(Int64 elapsedRealtimeDeadline) {
            Int64 elapsedRealtime = SystemClock->ElapsedRealtime();
            ShowError(R::string::lockpattern_too_many_failed_confirmation_attempts_header, 0);
            mPasswordEntry->SetEnabled(FALSE);
            mCountdownTimer = new CountDownTimer(
                    elapsedRealtimeDeadline - elapsedRealtime,
                    LockPatternUtils.FAILED_ATTEMPT_COUNTDOWN_INTERVAL_MS) {

                //@Override
                CARAPI OnTick(Int64 millisUntilFinished) {
                    final Int32 secondsCountdown = (Int32) (millisUntilFinished / 1000);
                    mHeaderText->SetText(GetString(
                            R::string::lockpattern_too_many_failed_confirmation_attempts_footer,
                            secondsCountdown));
                }

                //@Override
                CARAPI OnFinish() {
                    mPasswordEntry->SetEnabled(TRUE);
                    mHeaderText->SetText(GetDefaultHeader());
                    mNumWrongConfirmAttempts = 0;
                }
            }.Start();
        }

        CARAPI OnClick(View v) {
            switch (v->GetId()) {
                case R.id.next_button:
                    HandleNext();
                    break;

                case R.id.cancel_button:
                    GetActivity()->SetResult(RESULT_CANCELED);
                    GetActivity()->Finish();
                    break;
            }
        }

        private void ShowError(Int32 msg) {
            ShowError(msg, ERROR_MESSAGE_TIMEOUT);
        }

        private final Runnable mResetErrorRunnable = new Runnable() {
            CARAPI Run() {
                mHeaderText->SetText(GetDefaultHeader());
            }
        };

        private void ShowError(Int32 msg, Int64 timeout) {
            mHeaderText->SetText(msg);
            mHeaderText->AnnounceForAccessibility(mHeaderText->GetText());
            mPasswordEntry->SetText(NULL);
            mHandler->RemoveCallbacks(mResetErrorRunnable);
            if (timeout != 0) {
                mHandler->PostDelayed(mResetErrorRunnable, timeout);
            }
        }

        // {@link OnEditorActionListener} methods.
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

        // {@link TextWatcher} methods.
        CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
        }

        CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
        }

        CARAPI AfterTextChanged(Editable s) {
            mContinueButton->SetEnabled(mPasswordEntry->GetText()->Length() > 0);
        }
    }
}
