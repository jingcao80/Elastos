

package com.android.settings;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Internal::Widget::LockPatternView::ICell;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::ICountDownTimer;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Utility::IList;

/**
 * Launch this when you want the user to confirm their lock pattern.
 *
 * Sets an activity result of {@link Activity#RESULT_OK} when the user
 * successfully confirmed their pattern.
 */
public class ConfirmLockPattern extends SettingsActivity {

    public static class InternalActivity extends ConfirmLockPattern {
    }

    /**
     * Names of {@link CharSequence} fields within the originating {@link Intent}
     * that are used to configure the keyguard confirmation view's labeling.
     * The view will use the system-defined resource strings for any labels that
     * the caller does not supply.
     */
    public static const String PACKAGE = "com.android.settings";
    public static const String HEADER_TEXT = PACKAGE + ".ConfirmLockPattern.header";
    public static const String FOOTER_TEXT = PACKAGE + ".ConfirmLockPattern.footer";
    public static const String HEADER_WRONG_TEXT = PACKAGE + ".ConfirmLockPattern.header_wrong";
    public static const String FOOTER_WRONG_TEXT = PACKAGE + ".ConfirmLockPattern.footer_wrong";

    private enum Stage {
        NeedToUnlock,
        NeedToUnlockWrong,
        LockedOut
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        CharSequence msg = GetText(R::string::lockpassword_confirm_your_pattern_header);
        SetTitle(msg);
    }

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, ConfirmLockPatternFragment.class->GetName());
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (ConfirmLockPatternFragment.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    public static class ConfirmLockPatternFragment extends Fragment {

        // how Int64 we wait to clear a wrong pattern
        private static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS = 2000;

        private static const String KEY_NUM_WRONG_ATTEMPTS = "num_wrong_attempts";

        private LockPatternView mLockPatternView;
        private LockPatternUtils mLockPatternUtils;
        private Int32 mNumWrongConfirmAttempts;
        private CountDownTimer mCountdownTimer;

        private TextView mHeaderTextView;
        private TextView mFooterTextView;

        // caller-supplied text for various prompts
        private CharSequence mHeaderText;
        private CharSequence mFooterText;
        private CharSequence mHeaderWrongText;
        private CharSequence mFooterWrongText;

        // required constructor for fragments
        public ConfirmLockPatternFragment() {

        }

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);
            mLockPatternUtils = new LockPatternUtils(GetActivity());
        }

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View view = inflater->Inflate(R.layout.confirm_lock_pattern, NULL);
            mHeaderTextView = (TextView) view->FindViewById(R.id.headerText);
            mLockPatternView = (LockPatternView) view->FindViewById(R.id.lockPattern);
            mFooterTextView = (TextView) view->FindViewById(R.id.footerText);

            // make it so unhandled touch events within the unlock screen go to the
            // lock pattern view.
            final LinearLayoutWithDefaultTouchRecepient topLayout
                    = (LinearLayoutWithDefaultTouchRecepient) view->FindViewById(R.id.topLayout);
            topLayout->SetDefaultTouchRecepient(mLockPatternView);

            Intent intent = GetActivity()->GetIntent();
            if (intent != NULL) {
                mHeaderText = intent->GetCharSequenceExtra(HEADER_TEXT);
                mFooterText = intent->GetCharSequenceExtra(FOOTER_TEXT);
                mHeaderWrongText = intent->GetCharSequenceExtra(HEADER_WRONG_TEXT);
                mFooterWrongText = intent->GetCharSequenceExtra(FOOTER_WRONG_TEXT);
            }

            mLockPatternView->SetTactileFeedbackEnabled(mLockPatternUtils->IsTactileFeedbackEnabled());
            mLockPatternView->SetOnPatternListener(mConfirmExistingLockPatternListener);
            UpdateStage(Stage.NeedToUnlock);

            if (savedInstanceState != NULL) {
                mNumWrongConfirmAttempts = savedInstanceState->GetInt(KEY_NUM_WRONG_ATTEMPTS);
            } else {
                // on first launch, if no lock pattern is set, then finish with
                // success (don't want user to get stuck confirming something that
                // doesn't exist).
                if (!mLockPatternUtils->SavedPatternExists()) {
                    GetActivity()->SetResult(Activity.RESULT_OK);
                    GetActivity()->Finish();
                }
            }
            return view;
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            // deliberately not calling super since we are managing this in full
            outState->PutInt(KEY_NUM_WRONG_ATTEMPTS, mNumWrongConfirmAttempts);
        }

        //@Override
        CARAPI OnPause() {
            super->OnPause();

            if (mCountdownTimer != NULL) {
                mCountdownTimer->Cancel();
            }
        }

        //@Override
        CARAPI OnResume() {
            super->OnResume();

            // if the user is currently locked out, enforce it.
            Int64 deadline = mLockPatternUtils->GetLockoutAttemptDeadline();
            if (deadline != 0) {
                HandleAttemptLockout(deadline);
            } else if (!mLockPatternView->IsEnabled()) {
                // The deadline has passed, but the timer was cancelled...
                // Need to clean up.
                mNumWrongConfirmAttempts = 0;
                UpdateStage(Stage.NeedToUnlock);
            }
        }

        private void UpdateStage(Stage stage) {
            switch (stage) {
                case NeedToUnlock:
                    if (mHeaderText != NULL) {
                        mHeaderTextView->SetText(mHeaderText);
                    } else {
                        mHeaderTextView->SetText(R::string::lockpattern_need_to_unlock);
                    }
                    if (mFooterText != NULL) {
                        mFooterTextView->SetText(mFooterText);
                    } else {
                        mFooterTextView->SetText(R::string::lockpattern_need_to_unlock_footer);
                    }

                    mLockPatternView->SetEnabled(TRUE);
                    mLockPatternView->EnableInput();
                    break;
                case NeedToUnlockWrong:
                    if (mHeaderWrongText != NULL) {
                        mHeaderTextView->SetText(mHeaderWrongText);
                    } else {
                        mHeaderTextView->SetText(R::string::lockpattern_need_to_unlock_wrong);
                    }
                    if (mFooterWrongText != NULL) {
                        mFooterTextView->SetText(mFooterWrongText);
                    } else {
                        mFooterTextView->SetText(R::string::lockpattern_need_to_unlock_wrong_footer);
                    }

                    mLockPatternView->SetDisplayMode(LockPatternView.DisplayMode.Wrong);
                    mLockPatternView->SetEnabled(TRUE);
                    mLockPatternView->EnableInput();
                    break;
                case LockedOut:
                    mLockPatternView->ClearPattern();
                    // enabled = FALSE means: disable input, and have the
                    // appearance of being disabled.
                    mLockPatternView->SetEnabled(FALSE); // appearance of being disabled
                    break;
            }

            // Always announce the header for accessibility. This is a no-op
            // when accessibility is disabled.
            mHeaderTextView->AnnounceForAccessibility(mHeaderTextView->GetText());
        }

        private Runnable mClearPatternRunnable = new Runnable() {
            CARAPI Run() {
                mLockPatternView->ClearPattern();
            }
        };

        // clear the wrong pattern unless they have started a new one
        // already
        private void PostClearPatternRunnable() {
            mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
            mLockPatternView->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS);
        }

        /**
         * The pattern listener that responds according to a user confirming
         * an existing lock pattern.
         */
        private LockPatternView.OnPatternListener mConfirmExistingLockPatternListener
                = new LockPatternView->OnPatternListener()  {

            CARAPI OnPatternStart() {
                mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
            }

            CARAPI OnPatternCleared() {
                mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
            }

            CARAPI OnPatternCellAdded(List<Cell> pattern) {

            }

            CARAPI OnPatternDetected(List<LockPatternView.Cell> pattern) {
                if (mLockPatternUtils->CheckPattern(pattern)) {

                    Intent intent = new Intent();
                    if (GetActivity() instanceof ConfirmLockPattern.InternalActivity) {
                        intent->PutExtra(ChooseLockSettingsHelper.EXTRA_KEY_TYPE,
                                        StorageManager.CRYPT_TYPE_PATTERN);
                        intent->PutExtra(ChooseLockSettingsHelper.EXTRA_KEY_PASSWORD,
                                        LockPatternUtils->PatternToString(pattern));
                    }

                    GetActivity()->SetResult(Activity.RESULT_OK, intent);
                    GetActivity()->Finish();
                } else {
                    if (pattern->Size() >= LockPatternUtils.MIN_PATTERN_REGISTER_FAIL &&
                            ++mNumWrongConfirmAttempts
                            >= LockPatternUtils.FAILED_ATTEMPTS_BEFORE_TIMEOUT) {
                        Int64 deadline = mLockPatternUtils->SetLockoutAttemptDeadline();
                        HandleAttemptLockout(deadline);
                    } else {
                        UpdateStage(Stage.NeedToUnlockWrong);
                        PostClearPatternRunnable();
                    }
                }
            }
        };


        private void HandleAttemptLockout(Int64 elapsedRealtimeDeadline) {
            UpdateStage(Stage.LockedOut);
            Int64 elapsedRealtime = SystemClock->ElapsedRealtime();
            mCountdownTimer = new CountDownTimer(
                    elapsedRealtimeDeadline - elapsedRealtime,
                    LockPatternUtils.FAILED_ATTEMPT_COUNTDOWN_INTERVAL_MS) {

                //@Override
                CARAPI OnTick(Int64 millisUntilFinished) {
                    mHeaderTextView->SetText(R::string::lockpattern_too_many_failed_confirmation_attempts_header);
                    final Int32 secondsCountdown = (Int32) (millisUntilFinished / 1000);
                    mFooterTextView->SetText(GetString(
                            R::string::lockpattern_too_many_failed_confirmation_attempts_footer,
                            secondsCountdown));
                }

                //@Override
                CARAPI OnFinish() {
                    mNumWrongConfirmAttempts = 0;
                    UpdateStage(Stage.NeedToUnlock);
                }
            }.Start();
        }
    }
}
