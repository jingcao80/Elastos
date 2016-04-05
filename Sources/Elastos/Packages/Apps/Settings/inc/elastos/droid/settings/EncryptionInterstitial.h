/*
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.settings;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Utility::IList;

using Elastos::Droid::Accessibilityservice::IAccessibilityServiceInfo;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::DialogInterface::IOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;

public class EncryptionInterstitial extends SettingsActivity {

    private static const String EXTRA_PASSWORD_QUALITY = "extra_password_quality";
    public static const String EXTRA_REQUIRE_PASSWORD = "extra_require_password";

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, EncryptionInterstitialFragment.class->GetName());
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        return EncryptionInterstitialFragment.class->GetName()->Equals(fragmentName);
    }

    public static Intent CreateStartIntent(Context ctx, Int32 quality,
            Boolean requirePasswordDefault) {
        return new Intent(ctx, EncryptionInterstitial.class)
                .PutExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE)
                .PutExtra(EXTRA_PREFS_SET_BACK_TEXT, (String) NULL)
                .PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, ctx->GetString(
                        R::string::encryption_continue_button))
                .PutExtra(EXTRA_PASSWORD_QUALITY, quality)
                .PutExtra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, R::string::encryption_interstitial_header)
                .PutExtra(EXTRA_REQUIRE_PASSWORD, requirePasswordDefault);
    }

    public static class EncryptionInterstitialFragment extends SettingsPreferenceFragment
            implements View.OnClickListener, OnClickListener {

        private static const Int32 ACCESSIBILITY_WARNING_DIALOG = 1;
        private RadioButton mRequirePasswordToDecryptButton;
        private RadioButton mDontRequirePasswordToDecryptButton;
        private TextView mEncryptionMessage;
        private Boolean mPasswordRequired;

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            final Int32 layoutId = R.layout.encryption_interstitial;
            View view = inflater->Inflate(layoutId, container, FALSE);
            mRequirePasswordToDecryptButton =
                    (RadioButton) view->FindViewById(R.id.encrypt_require_password);
            mDontRequirePasswordToDecryptButton =
                    (RadioButton) view->FindViewById(R.id.encrypt_dont_require_password);
            mEncryptionMessage =
                    (TextView) view->FindViewById(R.id.encryption_message);
            Int32 quality = GetActivity()->GetIntent().GetIntExtra(EXTRA_PASSWORD_QUALITY, 0);
            final Int32 msgId;
            final Int32 enableId;
            final Int32 disableId;
            switch (quality) {
                case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING:
                    msgId = R::string::encryption_interstitial_message_pattern;
                    enableId = R::string::encrypt_require_pattern;
                    disableId = R::string::encrypt_dont_require_pattern;
                    break;
                case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
                case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX:
                    msgId = R::string::encryption_interstitial_message_pin;
                    enableId = R::string::encrypt_require_pin;
                    disableId = R::string::encrypt_dont_require_pin;
                    break;
                default:
                    msgId = R::string::encryption_interstitial_message_password;
                    enableId = R::string::encrypt_require_password;
                    disableId = R::string::encrypt_dont_require_password;
                    break;
            }
            mEncryptionMessage->SetText(msgId);

            mRequirePasswordToDecryptButton->SetOnClickListener(this);
            mRequirePasswordToDecryptButton->SetText(enableId);

            mDontRequirePasswordToDecryptButton->SetOnClickListener(this);
            mDontRequirePasswordToDecryptButton->SetText(disableId);

            SetRequirePasswordState(GetActivity()->GetIntent().GetBooleanExtra(
                    EXTRA_REQUIRE_PASSWORD, TRUE));
            return view;
        }

        //@Override
        CARAPI OnClick(View v) {
            if (v == mRequirePasswordToDecryptButton) {
                final Boolean accEn = AccessibilityManager->GetInstance(GetActivity()).IsEnabled();
                if (accEn && !mPasswordRequired) {
                    SetRequirePasswordState(FALSE); // clear the UI state
                    ShowDialog(ACCESSIBILITY_WARNING_DIALOG);
                } else {
                    SetRequirePasswordState(TRUE);
                }
            } else {
                SetRequirePasswordState(FALSE);
            }
        }

        //@Override
        public Dialog OnCreateDialog(Int32 dialogId) {
            Switch(dialogId) {
                case ACCESSIBILITY_WARNING_DIALOG: {
                    final Int32 quality = new LockPatternUtils(GetActivity())
                            .GetKeyguardStoredPasswordQuality();
                    final Int32 titleId;
                    final Int32 messageId;
                    switch (quality) {
                        case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING:
                            titleId = R::string::encrypt_talkback_dialog_require_pattern;
                            messageId = R::string::encrypt_talkback_dialog_message_pattern;
                            break;
                        case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
                        case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX:
                            titleId = R::string::encrypt_talkback_dialog_require_pin;
                            messageId = R::string::encrypt_talkback_dialog_message_pin;
                            break;
                        default:
                            titleId = R::string::encrypt_talkback_dialog_require_password;
                            messageId = R::string::encrypt_talkback_dialog_message_password;
                            break;
                    }


                    List<AccessibilityServiceInfo> list =
                            AccessibilityManager->GetInstance(GetActivity())
                            .GetEnabledAccessibilityServiceList(
                                    AccessibilityServiceInfo.FEEDBACK_ALL_MASK);
                    final CharSequence exampleAccessibility;
                    if (list->IsEmpty()) {
                        // This should never happen.  But we shouldn't crash
                        exampleAccessibility = "";
                    } else {
                        exampleAccessibility = list->Get(0).GetResolveInfo()
                                .LoadLabel(GetPackageManager());
                    }
                    return new AlertDialog->Builder(GetActivity())
                        .SetTitle(titleId)
                        .SetMessage(GetString(messageId, exampleAccessibility))
                        .SetCancelable(TRUE)
                        .SetPositiveButton(android.R::string::ok, this)
                        .SetNegativeButton(android.R::string::cancel, this)
                        .Create();
                }
                default: throw new IllegalArgumentException();
            }
        }

        private void SetRequirePasswordState(Boolean required) {
            mPasswordRequired = required;
            mRequirePasswordToDecryptButton->SetChecked(required);
            mDontRequirePasswordToDecryptButton->SetChecked(!required);

            // Updates value returned by SettingsActivity->OnActivityResult().
            SettingsActivity sa = (SettingsActivity)GetActivity();
            Intent resultIntentData = sa->GetResultIntentData();
            resultIntentData = resultIntentData == NULL ? new Intent() : resultIntentData;
            resultIntentData->PutExtra(EXTRA_REQUIRE_PASSWORD, mPasswordRequired);
            sa->SetResultIntentData(resultIntentData);
        }

        //@Override
        CARAPI OnClick(DialogInterface dialog, Int32 which) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                SetRequirePasswordState(TRUE);
            } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                SetRequirePasswordState(FALSE);
            }
        }
    }
}
