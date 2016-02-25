/*
 * Copyright (C) 2013 The Android Open Source Project
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

using Elastos::Utility::IHashSet;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IRestrictionsManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;

/**
 * Base class for settings screens that should be pin protected when in restricted mode.
 * The constructor for this class will take the restriction key that this screen should be
 * locked by.  If {@link RestrictionsManager->HasRestrictionsProvider()} and
 * {@link UserManager->HasUserRestriction()}, then the user will have to enter the restrictions
 * pin before seeing the Settings screen.
 *
 * If this settings screen should be pin protected whenever
 * {@link RestrictionsManager->HasRestrictionsProvider()} returns TRUE, pass in
 * {@link RESTRICT_IF_OVERRIDABLE} to the constructor instead of a restrictions key.
 */
public class RestrictedSettingsFragment extends SettingsPreferenceFragment {

    protected static const String RESTRICT_IF_OVERRIDABLE = "restrict_if_overridable";

    // No RestrictedSettingsFragment screens should use this number in startActivityForResult.
    private static const Int32 REQUEST_PIN_CHALLENGE = 12309;

    private static const String KEY_CHALLENGE_SUCCEEDED = "chsc";
    private static const String KEY_CHALLENGE_REQUESTED = "chrq";

    // If the restriction PIN is entered correctly.
    private Boolean mChallengeSucceeded;
    private Boolean mChallengeRequested;

    private UserManager mUserManager;
    private RestrictionsManager mRestrictionsManager;

    private final String mRestrictionKey;

    // Receiver to clear pin status when the screen is turned off.
    private BroadcastReceiver mScreenOffReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (!mChallengeRequested) {
                mChallengeSucceeded = FALSE;
                mChallengeRequested = FALSE;
            }
        }
    };

    /**
     * @param restrictionKey The restriction key to check before pin protecting
     *            this settings page. Pass in {@link RESTRICT_IF_OVERRIDABLE} if it should
     *            be protected whenever a restrictions provider is set. Pass in
     *            NULL if it should never be protected.
     */
    public RestrictedSettingsFragment(String restrictionKey) {
        mRestrictionKey = restrictionKey;
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mRestrictionsManager = (RestrictionsManager) GetSystemService(Context.RESTRICTIONS_SERVICE);
        mUserManager = (UserManager) GetSystemService(Context.USER_SERVICE);

        if (icicle != NULL) {
            mChallengeSucceeded = icicle->GetBoolean(KEY_CHALLENGE_SUCCEEDED, FALSE);
            mChallengeRequested = icicle->GetBoolean(KEY_CHALLENGE_REQUESTED, FALSE);
        }

        IntentFilter offFilter = new IntentFilter(IIntent::ACTION_SCREEN_OFF);
        offFilter->AddAction(IIntent::ACTION_USER_PRESENT);
        GetActivity()->RegisterReceiver(mScreenOffReceiver, offFilter);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        if (GetActivity()->IsChangingConfigurations()) {
            outState->PutBoolean(KEY_CHALLENGE_REQUESTED, mChallengeRequested);
            outState->PutBoolean(KEY_CHALLENGE_SUCCEEDED, mChallengeSucceeded);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (ShouldBeProviderProtected(mRestrictionKey)) {
            EnsurePin();
        }
    }

    //@Override
    CARAPI OnDestroy() {
        GetActivity()->UnregisterReceiver(mScreenOffReceiver);
        super->OnDestroy();
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == REQUEST_PIN_CHALLENGE) {
            if (resultCode == Activity.RESULT_OK) {
                mChallengeSucceeded = TRUE;
                mChallengeRequested = FALSE;
            } else {
                mChallengeSucceeded = FALSE;
            }
            return;
        }

        super->OnActivityResult(requestCode, resultCode, data);
    }

    private void EnsurePin() {
        if (!mChallengeSucceeded && !mChallengeRequested
                && mRestrictionsManager->HasRestrictionsProvider()) {
            Intent intent = mRestrictionsManager->CreateLocalApprovalIntent();
            if (intent != NULL) {
                mChallengeRequested = TRUE;
                mChallengeSucceeded = FALSE;
                PersistableBundle request = new PersistableBundle();
                request->PutString(RestrictionsManager.REQUEST_KEY_MESSAGE,
                        GetResources()->GetString(R::string::restr_pin_enter_admin_pin));
                intent->PutExtra(RestrictionsManager.EXTRA_REQUEST_BUNDLE, request);
                StartActivityForResult(intent, REQUEST_PIN_CHALLENGE);
            }
        }
    }

    /**
     * Returns TRUE if this activity is restricted, but no restrictions provider has been set.
     * Used to determine if the settings UI should disable UI.
     */
    protected Boolean IsRestrictedAndNotProviderProtected() {
        if (mRestrictionKey == NULL || RESTRICT_IF_OVERRIDABLE->Equals(mRestrictionKey)) {
            return FALSE;
        }
        return mUserManager->HasUserRestriction(mRestrictionKey)
                && !mRestrictionsManager->HasRestrictionsProvider();
    }

    protected Boolean HasChallengeSucceeded() {
        return (mChallengeRequested && mChallengeSucceeded) || !mChallengeRequested;
    }

    /**
     * Returns TRUE if this restrictions key is locked down.
     */
    protected Boolean ShouldBeProviderProtected(String restrictionKey) {
        if (restrictionKey == NULL) {
            return FALSE;
        }
        Boolean restricted = RESTRICT_IF_OVERRIDABLE->Equals(restrictionKey)
                || mUserManager->HasUserRestriction(mRestrictionKey);
        return restricted && mRestrictionsManager->HasRestrictionsProvider();
    }

    /**
     * Returns whether restricted or actionable UI elements should be removed or disabled.
     */
    protected Boolean IsUiRestricted() {
        return IsRestrictedAndNotProviderProtected() || !HasChallengeSucceeded();
    }
}
