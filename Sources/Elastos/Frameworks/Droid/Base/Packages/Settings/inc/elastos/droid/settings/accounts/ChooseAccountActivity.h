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

package com.android.settings.accounts;

using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Utility::ICharSequences;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using com::Google::Android::Collect::IMaps;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IMap;

using static android::Content::Intent::IEXTRA_USER;

/**
 * Activity asking a user to select an account to be set up.
 *
 * An extra {@link UserHandle} can be specified in the intent as {@link EXTRA_USER}, if the user for
 * which the action needs to be performed is different to the one the Settings App will run in.
 */
public class ChooseAccountActivity extends PreferenceActivity {

    private static const String TAG = "ChooseAccountActivity";
    private String[] mAuthorities;
    private PreferenceGroup mAddAccountGroup;
    private final ArrayList<ProviderEntry> mProviderList = new ArrayList<ProviderEntry>();
    public HashSet<String> mAccountTypesFilter;
    private AuthenticatorDescription[] mAuthDescs;
    private HashMap<String, ArrayList<String>> mAccountTypeToAuthorities = NULL;
    private Map<String, AuthenticatorDescription> mTypeToAuthDescription
            = new HashMap<String, AuthenticatorDescription>();
    // The UserHandle of the user we are choosing an account for
    private UserHandle mUserHandle;
    private UserManager mUm;

    private static class ProviderEntry implements Comparable<ProviderEntry> {
        private final CharSequence name;
        private final String type;
        ProviderEntry(CharSequence providerName, String accountType) {
            name = providerName;
            type = accountType;
        }

        public Int32 CompareTo(ProviderEntry another) {
            if (name == NULL) {
                return -1;
            }
            if (another.name == NULL) {
                return +1;
            }
            return CharSequences->CompareToIgnoreCase(name, another.name);
        }
    }

    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        SetContentView(R.layout.add_account_screen);
        AddPreferencesFromResource(R.xml.add_account_settings);
        mAuthorities = GetIntent()->GetStringArrayExtra(
                AccountPreferenceBase.AUTHORITIES_FILTER_KEY);
        String[] accountTypesFilter = GetIntent()->GetStringArrayExtra(
                AccountPreferenceBase.ACCOUNT_TYPES_FILTER_KEY);
        if (accountTypesFilter != NULL) {
            mAccountTypesFilter = new HashSet<String>();
            for (String accountType : accountTypesFilter) {
                mAccountTypesFilter->Add(accountType);
            }
        }
        mAddAccountGroup = GetPreferenceScreen();
        mUm = UserManager->Get(this);
        mUserHandle = Utils->GetSecureTargetUser(GetActivityToken(), mUm, NULL /* arguments */,
                GetIntent()->GetExtras());
        UpdateAuthDescriptions();
    }

    /**
     * Updates provider icons. Subclasses should call this in OnCreate()
     * and update any UI that depends on AuthenticatorDescriptions in OnAuthDescriptionsUpdated().
     */
    private void UpdateAuthDescriptions() {
        mAuthDescs = AccountManager->Get(this).GetAuthenticatorTypesAsUser(
                mUserHandle->GetIdentifier());
        for (Int32 i = 0; i < mAuthDescs.length; i++) {
            mTypeToAuthDescription->Put(mAuthDescs[i].type, mAuthDescs[i]);
        }
        OnAuthDescriptionsUpdated();
    }

    private void OnAuthDescriptionsUpdated() {
        // Create list of providers to show on preference screen
        for (Int32 i = 0; i < mAuthDescs.length; i++) {
            String accountType = mAuthDescs[i].type;
            CharSequence providerName = GetLabelForType(accountType);

            // Skip preferences for authorities not specified. If no authorities specified,
            // then include them all.
            ArrayList<String> accountAuths = GetAuthoritiesForAccountType(accountType);
            Boolean addAccountPref = TRUE;
            if (mAuthorities != NULL && mAuthorities.length > 0 && accountAuths != NULL) {
                addAccountPref = FALSE;
                for (Int32 k = 0; k < mAuthorities.length; k++) {
                    if (accountAuths->Contains(mAuthorities[k])) {
                        addAccountPref = TRUE;
                        break;
                    }
                }
            }
            if (addAccountPref && mAccountTypesFilter != NULL
                    && !mAccountTypesFilter->Contains(accountType)) {
                addAccountPref = FALSE;
            }
            if (addAccountPref) {
                mProviderList->Add(new ProviderEntry(providerName, accountType));
            } else {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::V(TAG, "Skipped pref " + providerName + ": has no authority we need");
                }
            }
        }

        if (mProviderList->Size() == 1) {
            // If there's only one provider that matches, just run it.
            FinishWithAccountType(mProviderList->Get(0).type);
        } else if (mProviderList->Size() > 0) {
            Collections->Sort(mProviderList);
            mAddAccountGroup->RemoveAll();
            for (ProviderEntry pref : mProviderList) {
                Drawable drawable = GetDrawableForType(pref.type);
                ProviderPreference p =
                        new ProviderPreference(this, pref.type, drawable, pref.name);
                mAddAccountGroup->AddPreference(p);
            }
        } else {
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                final StringBuilder auths = new StringBuilder();
                for (String a : mAuthorities) {
                    auths->Append(a);
                    auths->Append(' ');
                }
                Logger::V(TAG, "No providers found for authorities: " + auths);
            }
            SetResult(RESULT_CANCELED);
            Finish();
        }
    }

    public ArrayList<String> GetAuthoritiesForAccountType(String type) {
        if (mAccountTypeToAuthorities == NULL) {
            mAccountTypeToAuthorities = Maps->NewHashMap();
            SyncAdapterType[] syncAdapters = ContentResolver->GetSyncAdapterTypesAsUser(
                    mUserHandle->GetIdentifier());
            for (Int32 i = 0, n = syncAdapters.length; i < n; i++) {
                final SyncAdapterType sa = syncAdapters[i];
                ArrayList<String> authorities = mAccountTypeToAuthorities->Get(sa.accountType);
                if (authorities == NULL) {
                    authorities = new ArrayList<String>();
                    mAccountTypeToAuthorities->Put(sa.accountType, authorities);
                }
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::D(TAG, "added authority " + sa.authority + " to accountType "
                            + sa.accountType);
                }
                authorities->Add(sa.authority);
            }
        }
        return mAccountTypeToAuthorities->Get(type);
    }

    /**
     * Gets an icon associated with a particular account type. If none found, return NULL.
     * @param accountType the type of account
     * @return a drawable for the icon or NULL if one cannot be found.
     */
    protected Drawable GetDrawableForType(final String accountType) {
        Drawable icon = NULL;
        if (mTypeToAuthDescription->ContainsKey(accountType)) {
            try {
                AuthenticatorDescription desc = mTypeToAuthDescription->Get(accountType);
                Context authContext = CreatePackageContextAsUser(desc.packageName, 0, mUserHandle);
                icon = GetPackageManager()->GetUserBadgedIcon(
                        authContext->GetResources()->GetDrawable(desc.iconId), mUserHandle);
            } catch (PackageManager.NameNotFoundException e) {
                // TODO: place holder icon for missing account icons?
                Logger::W(TAG, "No icon name for account type " + accountType);
            } catch (Resources.NotFoundException e) {
                // TODO: place holder icon for missing account icons?
                Logger::W(TAG, "No icon resource for account type " + accountType);
            }
        }
        return icon;
    }

    /**
     * Gets the label associated with a particular account type. If none found, return NULL.
     * @param accountType the type of account
     * @return a CharSequence for the label or NULL if one cannot be found.
     */
    protected CharSequence GetLabelForType(final String accountType) {
        CharSequence label = NULL;
        if (mTypeToAuthDescription->ContainsKey(accountType)) {
            try {
                AuthenticatorDescription desc = mTypeToAuthDescription->Get(accountType);
                Context authContext = CreatePackageContextAsUser(desc.packageName, 0, mUserHandle);
                label = authContext->GetResources()->GetText(desc.labelId);
            } catch (PackageManager.NameNotFoundException e) {
                Logger::W(TAG, "No label name for account type " + accountType);
            } catch (Resources.NotFoundException e) {
                Logger::W(TAG, "No label resource for account type " + accountType);
            }
        }
        return label;
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferences, Preference preference) {
        if (preference instanceof ProviderPreference) {
            ProviderPreference pref = (ProviderPreference) preference;
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                Logger::V(TAG, "Attempting to add account of type " + pref->GetAccountType());
            }
            FinishWithAccountType(pref->GetAccountType());
        }
        return TRUE;
    }

    private void FinishWithAccountType(String accountType) {
        Intent intent = new Intent();
        intent->PutExtra(AddAccountSettings.EXTRA_SELECTED_ACCOUNT, accountType);
        intent->PutExtra(EXTRA_USER, mUserHandle);
        SetResult(RESULT_OK, intent);
        Finish();
    }
}
