/*
 * Copyright (C) 2007 The Android Open Source Project
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

using com::Google::Android::Collect::ILists;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::LockPatternView::ICell;
using Elastos::Droid::Settings::Notification::IRedactionInterstitial;

using static com::Android::Internal::Widget::LockPatternView::IDisplayMode;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

/**
 * If the user has a lock pattern set already, makes them confirm the existing one.
 *
 * Then, prompts the user to choose a lock pattern:
 * - prompts for initial pattern
 * - asks for confirmation / restart
 * - saves chosen password when confirmed
 */
public class ChooseLockPattern extends SettingsActivity {
    /**
     * Used by the choose lock pattern wizard to indicate the wizard is
     * finished, and each activity in the wizard should finish.
     * <p>
     * Previously, each activity in the wizard would finish itself after
     * starting the next activity. However, this leads to broken 'Back'
     * behavior. So, now an activity does not finish itself until it gets this
     * result.
     */
    static const Int32 RESULT_FINISHED = RESULT_FIRST_USER;

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, ChooseLockPatternFragment.class->GetName());
        return modIntent;
    }

    public static Intent CreateIntent(Context context, final Boolean isFallback,
            Boolean requirePassword, Boolean confirmCredentials) {
        Intent intent = new Intent(context, ChooseLockPattern.class);
        intent->PutExtra("key_lock_method", "pattern");
        intent->PutExtra(ChooseLockGeneric.CONFIRM_CREDENTIALS, confirmCredentials);
        intent->PutExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, isFallback);
        intent->PutExtra(EncryptionInterstitial.EXTRA_REQUIRE_PASSWORD, requirePassword);
        return intent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (ChooseLockPatternFragment.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        // RequestWindowFeature(Window.FEATURE_NO_TITLE);
        super->OnCreate(savedInstanceState);
        CharSequence msg = GetText(R::string::lockpassword_choose_your_pattern_header);
        SetTitle(msg);
    }

    //@Override
    public Boolean OnKeyDown(Int32 keyCode, KeyEvent event) {
        // *** TODO ***
        // chooseLockPatternFragment->OnKeyDown(keyCode, event);
        return super->OnKeyDown(keyCode, event);
    }

    public static class ChooseLockPatternFragment extends Fragment
            implements View.OnClickListener {

        public static const Int32 CONFIRM_EXISTING_REQUEST = 55;

        // how Int64 after a confirmation message is shown before moving on
        static const Int32 INFORMATION_MSG_TIMEOUT_MS = 3000;

        // how Int64 we wait to clear a wrong pattern
        private static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS = 2000;

        private static const Int32 ID_EMPTY_MESSAGE = -1;

        protected TextView mHeaderText;
        protected LockPatternView mLockPatternView;
        protected TextView mFooterText;
        private TextView mFooterLeftButton;
        private TextView mFooterRightButton;
        protected List<LockPatternView.Cell> mChosenPattern = NULL;

        /**
         * The patten used during the help screen to show how to draw a pattern.
         */
        private final List<LockPatternView.Cell> mAnimatePattern =
                Collections->UnmodifiableList(Lists->NewArrayList(
                        LockPatternView.Cell->Of(0, 0),
                        LockPatternView.Cell->Of(0, 1),
                        LockPatternView.Cell->Of(1, 1),
                        LockPatternView.Cell->Of(2, 1)
                ));

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
                    UpdateStage(Stage.Introduction);
                    break;
            }
        }

        /**
         * The pattern listener that responds according to a user choosing a new
         * lock pattern.
         */
        protected LockPatternView.OnPatternListener mChooseNewLockPatternListener =
                new LockPatternView->OnPatternListener() {

                CARAPI OnPatternStart() {
                    mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
                    PatternInProgress();
                }

                CARAPI OnPatternCleared() {
                    mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
                }

                CARAPI OnPatternDetected(List<LockPatternView.Cell> pattern) {
                    if (mUiStage == Stage.NeedToConfirm || mUiStage == Stage.ConfirmWrong) {
                        if (mChosenPattern == NULL) throw new IllegalStateException(
                                "NULL chosen pattern in stage 'need to confirm");
                        if (mChosenPattern->Equals(pattern)) {
                            UpdateStage(Stage.ChoiceConfirmed);
                        } else {
                            UpdateStage(Stage.ConfirmWrong);
                        }
                    } else if (mUiStage == Stage.Introduction || mUiStage == Stage.ChoiceTooShort){
                        if (pattern->Size() < LockPatternUtils.MIN_LOCK_PATTERN_SIZE) {
                            UpdateStage(Stage.ChoiceTooShort);
                        } else {
                            mChosenPattern = new ArrayList<LockPatternView.Cell>(pattern);
                            UpdateStage(Stage.FirstChoiceValid);
                        }
                    } else {
                        throw new IllegalStateException("Unexpected stage " + mUiStage + " when "
                                + "entering the pattern.");
                    }
                }

                CARAPI OnPatternCellAdded(List<Cell> pattern) {

                }

                private void PatternInProgress() {
                    mHeaderText->SetText(R::string::lockpattern_recording_inprogress);
                    mFooterText->SetText("");
                    mFooterLeftButton->SetEnabled(FALSE);
                    mFooterRightButton->SetEnabled(FALSE);
                }
         };


        /**
         * The states of the left footer button.
         */
        enum LeftButtonMode {
            Cancel(R::string::cancel, TRUE),
            CancelDisabled(R::string::cancel, FALSE),
            Retry(R::string::lockpattern_retry_button_text, TRUE),
            RetryDisabled(R::string::lockpattern_retry_button_text, FALSE),
            Gone(ID_EMPTY_MESSAGE, FALSE);


            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            LeftButtonMode(Int32 text, Boolean enabled) {
                this.text = text;
                this.enabled = enabled;
            }

            final Int32 text;
            final Boolean enabled;
        }

        /**
         * The states of the right button.
         */
        enum RightButtonMode {
            Continue(R::string::lockpattern_continue_button_text, TRUE),
            ContinueDisabled(R::string::lockpattern_continue_button_text, FALSE),
            Confirm(R::string::lockpattern_confirm_button_text, TRUE),
            ConfirmDisabled(R::string::lockpattern_confirm_button_text, FALSE),
            Ok(android.R::string::ok, TRUE);

            /**
             * @param text The displayed text for this mode.
             * @param enabled Whether the button should be enabled.
             */
            RightButtonMode(Int32 text, Boolean enabled) {
                this.text = text;
                this.enabled = enabled;
            }

            final Int32 text;
            final Boolean enabled;
        }

        /**
         * Keep track internally of where the user is in choosing a pattern.
         */
        protected enum Stage {

            Introduction(
                    R::string::lockpattern_recording_intro_header,
                    LeftButtonMode.Cancel, RightButtonMode.ContinueDisabled,
                    ID_EMPTY_MESSAGE, TRUE),
            HelpScreen(
                    R::string::lockpattern_settings_help_how_to_record,
                    LeftButtonMode.Gone, RightButtonMode.Ok, ID_EMPTY_MESSAGE, FALSE),
            ChoiceTooShort(
                    R::string::lockpattern_recording_incorrect_too_short,
                    LeftButtonMode.Retry, RightButtonMode.ContinueDisabled,
                    ID_EMPTY_MESSAGE, TRUE),
            FirstChoiceValid(
                    R::string::lockpattern_pattern_entered_header,
                    LeftButtonMode.Retry, RightButtonMode.Continue, ID_EMPTY_MESSAGE, FALSE),
            NeedToConfirm(
                    R::string::lockpattern_need_to_confirm,
                    LeftButtonMode.Cancel, RightButtonMode.ConfirmDisabled,
                    ID_EMPTY_MESSAGE, TRUE),
            ConfirmWrong(
                    R::string::lockpattern_need_to_unlock_wrong,
                    LeftButtonMode.Cancel, RightButtonMode.ConfirmDisabled,
                    ID_EMPTY_MESSAGE, TRUE),
            ChoiceConfirmed(
                    R::string::lockpattern_pattern_confirmed_header,
                    LeftButtonMode.Cancel, RightButtonMode.Confirm, ID_EMPTY_MESSAGE, FALSE);


            /**
             * @param headerMessage The message displayed at the top.
             * @param leftMode The mode of the left button.
             * @param rightMode The mode of the right button.
             * @param footerMessage The footer message.
             * @param patternEnabled Whether the pattern widget is enabled.
             */
            Stage(Int32 headerMessage,
                    LeftButtonMode leftMode,
                    RightButtonMode rightMode,
                    Int32 footerMessage, Boolean patternEnabled) {
                this.headerMessage = headerMessage;
                this.leftMode = leftMode;
                this.rightMode = rightMode;
                this.footerMessage = footerMessage;
                this.patternEnabled = patternEnabled;
            }

            final Int32 headerMessage;
            final LeftButtonMode leftMode;
            final RightButtonMode rightMode;
            final Int32 footerMessage;
            final Boolean patternEnabled;
        }

        private Stage mUiStage = Stage.Introduction;
        private Boolean mDone = FALSE;

        private Runnable mClearPatternRunnable = new Runnable() {
            CARAPI Run() {
                mLockPatternView->ClearPattern();
            }
        };

        private ChooseLockSettingsHelper mChooseLockSettingsHelper;

        private static const String KEY_UI_STAGE = "uiStage";
        private static const String KEY_PATTERN_CHOICE = "chosenPattern";

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);
            mChooseLockSettingsHelper = new ChooseLockSettingsHelper(GetActivity());
            if (!(GetActivity() instanceof ChooseLockPattern)) {
                throw new SecurityException("Fragment contained in wrong activity");
            }
        }

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {

            // SetupViews()
            View view = inflater->Inflate(R.layout.choose_lock_pattern, NULL);
            mHeaderText = (TextView) view->FindViewById(R.id.headerText);
            mLockPatternView = (LockPatternView) view->FindViewById(R.id.lockPattern);
            mLockPatternView->SetOnPatternListener(mChooseNewLockPatternListener);
            mLockPatternView->SetTactileFeedbackEnabled(
                    mChooseLockSettingsHelper->Utils()->IsTactileFeedbackEnabled());

            mFooterText = (TextView) view->FindViewById(R.id.footerText);

            mFooterLeftButton = (TextView) view->FindViewById(R.id.footerLeftButton);
            mFooterRightButton = (TextView) view->FindViewById(R.id.footerRightButton);

            mFooterLeftButton->SetOnClickListener(this);
            mFooterRightButton->SetOnClickListener(this);

            // make it so unhandled touch events within the unlock screen go to the
            // lock pattern view.
            final LinearLayoutWithDefaultTouchRecepient topLayout
                    = (LinearLayoutWithDefaultTouchRecepient) view->FindViewById(
                    R.id.topLayout);
            topLayout->SetDefaultTouchRecepient(mLockPatternView);

            final Boolean confirmCredentials = GetActivity()->GetIntent()
                    .GetBooleanExtra("confirm_credentials", TRUE);

            if (savedInstanceState == NULL) {
                if (confirmCredentials) {
                    // first launch. As a security measure, we're in NeedToConfirm mode until we
                    // know there isn't an existing password or the user confirms their password.
                    UpdateStage(Stage.NeedToConfirm);
                    Boolean launchedConfirmationActivity =
                        mChooseLockSettingsHelper->LaunchConfirmationActivity(
                                CONFIRM_EXISTING_REQUEST, NULL, NULL);
                    if (!launchedConfirmationActivity) {
                        UpdateStage(Stage.Introduction);
                    }
                } else {
                    UpdateStage(Stage.Introduction);
                }
            } else {
                // restore from previous state
                final String patternString = savedInstanceState->GetString(KEY_PATTERN_CHOICE);
                if (patternString != NULL) {
                    mChosenPattern = LockPatternUtils->StringToPattern(patternString);
                }
                UpdateStage(Stage->Values()[savedInstanceState->GetInt(KEY_UI_STAGE)]);
            }
            mDone = FALSE;
            return view;
        }

        CARAPI OnClick(View v) {
            if (v == mFooterLeftButton) {
                if (mUiStage.leftMode == LeftButtonMode.Retry) {
                    mChosenPattern = NULL;
                    mLockPatternView->ClearPattern();
                    UpdateStage(Stage.Introduction);
                } else if (mUiStage.leftMode == LeftButtonMode.Cancel) {
                    // They are canceling the entire wizard
                    GetActivity()->SetResult(RESULT_FINISHED);
                    GetActivity()->Finish();
                } else {
                    throw new IllegalStateException("left footer button pressed, but stage of " +
                        mUiStage + " doesn't make sense");
                }
            } else if (v == mFooterRightButton) {

                if (mUiStage.rightMode == RightButtonMode.Continue) {
                    if (mUiStage != Stage.FirstChoiceValid) {
                        throw new IllegalStateException("expected ui stage " + Stage.FirstChoiceValid
                                + " when button is " + RightButtonMode.Continue);
                    }
                    UpdateStage(Stage.NeedToConfirm);
                } else if (mUiStage.rightMode == RightButtonMode.Confirm) {
                    if (mUiStage != Stage.ChoiceConfirmed) {
                        throw new IllegalStateException("expected ui stage " + Stage.ChoiceConfirmed
                                + " when button is " + RightButtonMode.Confirm);
                    }
                    SaveChosenPatternAndFinish();
                } else if (mUiStage.rightMode == RightButtonMode.Ok) {
                    if (mUiStage != Stage.HelpScreen) {
                        throw new IllegalStateException("Help screen is only mode with ok button, but " +
                                "stage is " + mUiStage);
                    }
                    mLockPatternView->ClearPattern();
                    mLockPatternView->SetDisplayMode(DisplayMode.Correct);
                    UpdateStage(Stage.Introduction);
                }
            }
        }

        public Boolean OnKeyDown(Int32 keyCode, KeyEvent event) {
            if (keyCode == KeyEvent.KEYCODE_BACK && event->GetRepeatCount() == 0) {
                if (mUiStage == Stage.HelpScreen) {
                    UpdateStage(Stage.Introduction);
                    return TRUE;
                }
            }
            if (keyCode == KeyEvent.KEYCODE_MENU && mUiStage == Stage.Introduction) {
                UpdateStage(Stage.HelpScreen);
                return TRUE;
            }
            return FALSE;
        }

        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);

            outState->PutInt(KEY_UI_STAGE, mUiStage->Ordinal());
            if (mChosenPattern != NULL) {
                outState->PutString(KEY_PATTERN_CHOICE,
                        LockPatternUtils->PatternToString(mChosenPattern));
            }
        }

        /**
         * Updates the messages and buttons appropriate to what stage the user
         * is at in choosing a view.  This doesn't handle clearing out the pattern;
         * the pattern is expected to be in the right state.
         * @param stage
         */
        protected void UpdateStage(Stage stage) {
            final Stage previousStage = mUiStage;

            mUiStage = stage;

            // header text, footer text, visibility and
            // enabled state all known from the stage
            if (stage == Stage.ChoiceTooShort) {
                mHeaderText->SetText(
                        GetResources()->GetString(
                                stage.headerMessage,
                                LockPatternUtils.MIN_LOCK_PATTERN_SIZE));
            } else {
                mHeaderText->SetText(stage.headerMessage);
            }
            if (stage.footerMessage == ID_EMPTY_MESSAGE) {
                mFooterText->SetText("");
            } else {
                mFooterText->SetText(stage.footerMessage);
            }

            if (stage.leftMode == LeftButtonMode.Gone) {
                mFooterLeftButton->SetVisibility(View.GONE);
            } else {
                mFooterLeftButton->SetVisibility(View.VISIBLE);
                mFooterLeftButton->SetText(stage.leftMode.text);
                mFooterLeftButton->SetEnabled(stage.leftMode.enabled);
            }

            mFooterRightButton->SetText(stage.rightMode.text);
            mFooterRightButton->SetEnabled(stage.rightMode.enabled);

            // same for whether the patten is enabled
            if (stage.patternEnabled) {
                mLockPatternView->EnableInput();
            } else {
                mLockPatternView->DisableInput();
            }

            // the rest of the stuff varies enough that it is easier just to handle
            // on a case by case basis.
            mLockPatternView->SetDisplayMode(DisplayMode.Correct);

            switch (mUiStage) {
                case Introduction:
                    mLockPatternView->ClearPattern();
                    break;
                case HelpScreen:
                    mLockPatternView->SetPattern(DisplayMode.Animate, mAnimatePattern);
                    break;
                case ChoiceTooShort:
                    mLockPatternView->SetDisplayMode(DisplayMode.Wrong);
                    PostClearPatternRunnable();
                    break;
                case FirstChoiceValid:
                    break;
                case NeedToConfirm:
                    mLockPatternView->ClearPattern();
                    break;
                case ConfirmWrong:
                    mLockPatternView->SetDisplayMode(DisplayMode.Wrong);
                    PostClearPatternRunnable();
                    break;
                case ChoiceConfirmed:
                    break;
            }

            // If the stage changed, announce the header for accessibility. This
            // is a no-op when accessibility is disabled.
            if (previousStage != stage) {
                mHeaderText->AnnounceForAccessibility(mHeaderText->GetText());
            }
        }


        // clear the wrong pattern unless they have started a new one
        // already
        private void PostClearPatternRunnable() {
            mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
            mLockPatternView->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS);
        }

        private void SaveChosenPatternAndFinish() {
            if (mDone) return;
            LockPatternUtils utils = mChooseLockSettingsHelper->Utils();
            final Boolean lockVirgin = !utils->IsPatternEverChosen();

            final Boolean isFallback = GetActivity()->GetIntent()
                .GetBooleanExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE);

            final Boolean required = GetActivity()->GetIntent().GetBooleanExtra(
                    EncryptionInterstitial.EXTRA_REQUIRE_PASSWORD, TRUE);
            utils->SetCredentialRequiredToDecrypt(required);
            utils->SaveLockPattern(mChosenPattern, isFallback);
            utils->SetLockPatternEnabled(TRUE);

            if (lockVirgin) {
                utils->SetVisiblePatternEnabled(TRUE);
            }

            GetActivity()->SetResult(RESULT_FINISHED);
            GetActivity()->Finish();
            mDone = TRUE;
            StartActivity(RedactionInterstitial->CreateStartIntent(GetActivity()));
        }
    }
}
