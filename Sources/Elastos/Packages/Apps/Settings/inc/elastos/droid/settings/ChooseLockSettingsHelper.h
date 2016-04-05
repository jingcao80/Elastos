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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IIntent;

public class ChooseLockSettingsHelper {

    static const String EXTRA_KEY_TYPE = "type";
    static const String EXTRA_KEY_PASSWORD = "password";

    private LockPatternUtils mLockPatternUtils;
    private Activity mActivity;
    private Fragment mFragment;

    public ChooseLockSettingsHelper(Activity activity) {
        mActivity = activity;
        mLockPatternUtils = new LockPatternUtils(activity);
    }

    public ChooseLockSettingsHelper(Activity activity, Fragment fragment) {
        This(activity);
        mFragment = fragment;
    }

    public LockPatternUtils Utils() {
        return mLockPatternUtils;
    }

    /**
     * If a pattern, password or PIN exists, prompt the user before allowing them to change it.
     * @param message optional message to display about the action about to be done
     * @param details optional detail message to display
     * @return TRUE if one exists and we launched an activity to confirm it
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     */
    Boolean LaunchConfirmationActivity(Int32 request, CharSequence message, CharSequence details) {
        return LaunchConfirmationActivity(request, message, details, FALSE);
    }

    /**
     * If a pattern, password or PIN exists, prompt the user before allowing them to change it.
     * @param message optional message to display about the action about to be done
     * @param details optional detail message to display
     * @param returnCredentials if TRUE, put credentials into intent. Note that if this is TRUE,
                                this can only be called internally.
     * @return TRUE if one exists and we launched an activity to confirm it
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     */
    Boolean LaunchConfirmationActivity(Int32 request, CharSequence message, CharSequence details,
                                       Boolean returnCredentials) {
        Boolean launched = FALSE;
        switch (mLockPatternUtils->GetKeyguardStoredPasswordQuality()) {
            case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING:
                launched = ConfirmPattern(request, message, details, returnCredentials);
                break;
            case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
            case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX:
            case DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC:
            case DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC:
            case DevicePolicyManager.PASSWORD_QUALITY_COMPLEX:
                // TODO: update UI layout for ConfirmPassword to show message and details
                launched = ConfirmPassword(request, message, returnCredentials);
                break;
        }
        return launched;
    }

    /**
     * Launch screen to confirm the existing lock pattern.
     * @param message shown in header of ConfirmLockPattern if not NULL
     * @param details shown in footer of ConfirmLockPattern if not NULL
     * @param returnCredentials if TRUE, put credentials into intent.
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     * @return TRUE if we launched an activity to confirm pattern
     */
    private Boolean ConfirmPattern(Int32 request, CharSequence message,
                                   CharSequence details, Boolean returnCredentials) {
        if (!mLockPatternUtils->IsLockPatternEnabled() || !mLockPatternUtils->SavedPatternExists()) {
            return FALSE;
        }
        final Intent intent = new Intent();
        // supply header and footer text in the intent
        intent->PutExtra(ConfirmLockPattern.HEADER_TEXT, message);
        intent->PutExtra(ConfirmLockPattern.FOOTER_TEXT, details);
        intent->SetClassName("com.android.settings",
                            returnCredentials
                            ? ConfirmLockPattern.InternalActivity.class->GetName()
                            : ConfirmLockPattern.class->GetName());
        if (mFragment != NULL) {
            mFragment->StartActivityForResult(intent, request);
        } else {
            mActivity->StartActivityForResult(intent, request);
        }
        return TRUE;
    }

    /**
     * Launch screen to confirm the existing lock password.
     * @param message shown in header of ConfirmLockPassword if not NULL
     * @param returnCredentials if TRUE, put credentials into intent.
     * @see #OnActivityResult(Int32, Int32, android.content.Intent)
     * @return TRUE if we launched an activity to confirm password
     */
    private Boolean ConfirmPassword(Int32 request, CharSequence message,
            Boolean returnCredentials) {
        if (!mLockPatternUtils->IsLockPasswordEnabled()) return FALSE;
        final Intent intent = new Intent();
        // supply header text in the intent
        intent->PutExtra(ConfirmLockPattern.HEADER_TEXT, message);
        intent->SetClassName("com.android.settings",
                            returnCredentials
                            ? ConfirmLockPassword.InternalActivity.class->GetName()
                            : ConfirmLockPassword.class->GetName());
        if (mFragment != NULL) {
            mFragment->StartActivityForResult(intent, request);
        } else {
            mActivity->StartActivityForResult(intent, request);
        }
        return TRUE;
    }


}
