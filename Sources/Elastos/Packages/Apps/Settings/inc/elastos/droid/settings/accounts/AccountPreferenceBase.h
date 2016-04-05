/*

 * Copyright (C) 2008 The Android Open Source Project
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

package com.android.settings.accounts;

using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISyncStatusObserver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::Resources::ITheme;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IContextThemeWrapper;

using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IDate;

class AccountPreferenceBase extends SettingsPreferenceFragment
        implements AuthenticatorHelper.OnAccountsUpdateListener {

    protected static const String TAG = "AccountSettings";

    public static const String AUTHORITIES_FILTER_KEY = "authorities";
    public static const String ACCOUNT_TYPES_FILTER_KEY = "account_types";

    private final Handler mHandler = new Handler();

    private UserManager mUm;
    private Object mStatusChangeListenerHandle;
    protected AuthenticatorHelper mAuthenticatorHelper;
    protected UserHandle mUserHandle;

    private java.text.DateFormat mDateFormat;
    private java.text.DateFormat mTimeFormat;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);
        final Activity activity = GetActivity();
        mUserHandle = Utils->GetSecureTargetUser(activity->GetActivityToken(), mUm, GetArguments(),
                activity->GetIntent()->GetExtras());
        mAuthenticatorHelper = new AuthenticatorHelper(activity, mUserHandle, mUm, this);
    }

    /**
     * Overload to handle account updates.
     */
    //@Override
    CARAPI OnAccountsUpdate(UserHandle userHandle) {

    }

    /**
     * Overload to handle authenticator description updates
     */
    protected void OnAuthDescriptionsUpdated() {

    }

    /**
     * Overload to handle sync state updates.
     */
    protected void OnSyncStateUpdated() {

    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Activity activity = GetActivity();

        mDateFormat = DateFormat->GetDateFormat(activity);
        mTimeFormat = DateFormat->GetTimeFormat(activity);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mStatusChangeListenerHandle = ContentResolver->AddStatusChangeListener(
                ContentResolver.SYNC_OBSERVER_TYPE_ACTIVE
                | ContentResolver.SYNC_OBSERVER_TYPE_STATUS
                | ContentResolver.SYNC_OBSERVER_TYPE_SETTINGS,
                mSyncStatusObserver);
        OnSyncStateUpdated();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        ContentResolver->RemoveStatusChangeListener(mStatusChangeListenerHandle);
    }

    private SyncStatusObserver mSyncStatusObserver = new SyncStatusObserver() {
        CARAPI OnStatusChanged(Int32 which) {
            mHandler->Post(new Runnable() {
                CARAPI Run() {
                    OnSyncStateUpdated();
                }
            });
        }
    };

    public ArrayList<String> GetAuthoritiesForAccountType(String type) {
        return mAuthenticatorHelper->GetAuthoritiesForAccountType(type);
    }

    /**
     * Gets the preferences.xml file associated with a particular account type.
     * @param accountType the type of account
     * @return a PreferenceScreen inflated from accountPreferenceId.
     */
    public PreferenceScreen AddPreferencesForType(final String accountType,
            PreferenceScreen parent) {
        PreferenceScreen prefs = NULL;
        if (mAuthenticatorHelper->ContainsAccountType(accountType)) {
            AuthenticatorDescription desc = NULL;
            try {
                desc = mAuthenticatorHelper->GetAccountTypeDescription(accountType);
                if (desc != NULL && desc.accountPreferencesId != 0) {
                    // Load the context of the target package, then apply the
                    // base Settings theme (no references to local resources)
                    // and create a context theme wrapper so that we get the
                    // correct text colors. Control colors will still be wrong,
                    // but there's not much we can do about it since we can't
                    // reference local color resources.
                    final Context targetCtx = GetActivity()->CreatePackageContextAsUser(
                            desc.packageName, 0, mUserHandle);
                    final Theme baseTheme = GetResources()->NewTheme();
                    baseTheme->ApplyStyle(com.android.settings.R.style.Theme_SettingsBase, TRUE);
                    final Context themedCtx = new ContextThemeWrapper(targetCtx, 0);
                    themedCtx->GetTheme()->SetTo(baseTheme);
                    prefs = GetPreferenceManager()->InflateFromResource(themedCtx,
                            desc.accountPreferencesId, parent);
                }
            } catch (PackageManager.NameNotFoundException e) {
                Logger::W(TAG, "Couldn't load preferences.xml file from " + desc.packageName);
            } catch (Resources.NotFoundException e) {
                Logger::W(TAG, "Couldn't load preferences.xml file from " + desc.packageName);
            }
        }
        return prefs;
    }

    CARAPI UpdateAuthDescriptions() {
        mAuthenticatorHelper->UpdateAuthDescriptions(GetActivity());
        OnAuthDescriptionsUpdated();
    }

    protected Drawable GetDrawableForType(final String accountType) {
        return mAuthenticatorHelper->GetDrawableForType(GetActivity(), accountType);
    }

    protected CharSequence GetLabelForType(final String accountType) {
        return mAuthenticatorHelper->GetLabelForType(GetActivity(), accountType);
    }

    protected String FormatSyncDate(Date date) {
        // TODO: Switch to using DateUtils.formatDateTime
        return mDateFormat->Format(date) + " " + mTimeFormat->Format(date);
    }
}
