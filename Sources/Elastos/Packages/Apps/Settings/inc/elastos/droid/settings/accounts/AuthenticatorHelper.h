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

/*
 * Copyright (C) 2012 The Android Open Source Project
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

using com::Google::Android::Collect::IMaps;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;

/**
 * Helper class for monitoring accounts on the device for a given user.
 *
 * Classes using this helper should implement {@link OnAccountsUpdateListener}.
 * {@link OnAccountsUpdateListener#OnAccountsUpdate(UserHandle)} will then be
 * called once accounts get updated. For setting up listening for account
 * updates, {@link #ListenToAccountUpdates()} and
 * {@link #StopListeningToAccountUpdates()} should be used.
 */
final public class AuthenticatorHelper extends BroadcastReceiver {
    private static const String TAG = "AuthenticatorHelper";

    private Map<String, AuthenticatorDescription> mTypeToAuthDescription
            = new HashMap<String, AuthenticatorDescription>();
    private AuthenticatorDescription[] mAuthDescs;
    private ArrayList<String> mEnabledAccountTypes = new ArrayList<String>();
    private Map<String, Drawable> mAccTypeIconCache = new HashMap<String, Drawable>();
    private HashMap<String, ArrayList<String>> mAccountTypeToAuthorities = Maps->NewHashMap();

    private final UserHandle mUserHandle;
    private final UserManager mUm;
    private final Context mContext;
    private final OnAccountsUpdateListener mListener;
    private Boolean mListeningToAccountUpdates;

    public interface OnAccountsUpdateListener {
        void OnAccountsUpdate(UserHandle userHandle);
    }

    public AuthenticatorHelper(Context context, UserHandle userHandle, UserManager userManager,
            OnAccountsUpdateListener listener) {
        mContext = context;
        mUm = userManager;
        mUserHandle = userHandle;
        mListener = listener;
        // This guarantees that the helper is ready to use once constructed: the account types and
        // authorities are initialized
        OnAccountsUpdated(NULL);
    }

    public String[] GetEnabledAccountTypes() {
        return mEnabledAccountTypes->ToArray(new String[mEnabledAccountTypes->Size()]);
    }

    CARAPI PreloadDrawableForType(final Context context, final String accountType) {
        new AsyncTask<Void, Void, Void>() {
            //@Override
            protected Void DoInBackground(Void... params) {
                GetDrawableForType(context, accountType);
                return NULL;
            }
        }.ExecuteOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, (Void[]) NULL);
    }

    /**
     * Gets an icon associated with a particular account type. If none found, return NULL.
     * @param accountType the type of account
     * @return a drawable for the icon or NULL if one cannot be found.
     */
    public Drawable GetDrawableForType(Context context, final String accountType) {
        Drawable icon = NULL;
        {    AutoLock syncLock(mAccTypeIconCache);
            if (mAccTypeIconCache->ContainsKey(accountType)) {
                return mAccTypeIconCache->Get(accountType);
            }
        }
        if (mTypeToAuthDescription->ContainsKey(accountType)) {
            try {
                AuthenticatorDescription desc = mTypeToAuthDescription->Get(accountType);
                Context authContext = context->CreatePackageContextAsUser(desc.packageName, 0,
                        mUserHandle);
                icon = mContext->GetPackageManager()->GetUserBadgedIcon(
                        authContext->GetResources()->GetDrawable(desc.iconId), mUserHandle);
                {    AutoLock syncLock(mAccTypeIconCache);
                    mAccTypeIconCache->Put(accountType, icon);
                }
            } catch (PackageManager.NameNotFoundException e) {
            } catch (Resources.NotFoundException e) {
            }
        }
        if (icon == NULL) {
            icon = context->GetPackageManager()->GetDefaultActivityIcon();
        }
        return icon;
    }

    /**
     * Gets the label associated with a particular account type. If none found, return NULL.
     * @param accountType the type of account
     * @return a CharSequence for the label or NULL if one cannot be found.
     */
    public CharSequence GetLabelForType(Context context, final String accountType) {
        CharSequence label = NULL;
        if (mTypeToAuthDescription->ContainsKey(accountType)) {
            try {
                AuthenticatorDescription desc = mTypeToAuthDescription->Get(accountType);
                Context authContext = context->CreatePackageContextAsUser(desc.packageName, 0,
                        mUserHandle);
                label = authContext->GetResources()->GetText(desc.labelId);
            } catch (PackageManager.NameNotFoundException e) {
                Logger::W(TAG, "No label name for account type " + accountType);
            } catch (Resources.NotFoundException e) {
                Logger::W(TAG, "No label icon for account type " + accountType);
            }
        }
        return label;
    }

    /**
     * Updates provider icons. Subclasses should call this in OnCreate()
     * and update any UI that depends on AuthenticatorDescriptions in OnAuthDescriptionsUpdated().
     */
    CARAPI UpdateAuthDescriptions(Context context) {
        mAuthDescs = AccountManager->Get(context)
                .GetAuthenticatorTypesAsUser(mUserHandle->GetIdentifier());
        for (Int32 i = 0; i < mAuthDescs.length; i++) {
            mTypeToAuthDescription->Put(mAuthDescs[i].type, mAuthDescs[i]);
        }
    }

    public Boolean ContainsAccountType(String accountType) {
        return mTypeToAuthDescription->ContainsKey(accountType);
    }

    public AuthenticatorDescription GetAccountTypeDescription(String accountType) {
        return mTypeToAuthDescription->Get(accountType);
    }

    public Boolean HasAccountPreferences(final String accountType) {
        if (ContainsAccountType(accountType)) {
            AuthenticatorDescription desc = GetAccountTypeDescription(accountType);
            if (desc != NULL && desc.accountPreferencesId != 0) {
                return TRUE;
            }
        }
        return FALSE;
    }

    void OnAccountsUpdated(Account[] accounts) {
        UpdateAuthDescriptions(mContext);
        if (accounts == NULL) {
            accounts = AccountManager->Get(mContext).GetAccountsAsUser(mUserHandle->GetIdentifier());
        }
        mEnabledAccountTypes->Clear();
        mAccTypeIconCache->Clear();
        for (Int32 i = 0; i < accounts.length; i++) {
            final Account account = accounts[i];
            if (!mEnabledAccountTypes->Contains(account.type)) {
                mEnabledAccountTypes->Add(account.type);
            }
        }
        BuildAccountTypeToAuthoritiesMap();
        if (mListeningToAccountUpdates) {
            mListener->OnAccountsUpdate(mUserHandle);
        }
    }

    //@Override
    CARAPI OnReceive(final Context context, final Intent intent) {
        // TODO: watch for package upgrades to invalidate cache; see http://b/7206643
        final Account[] accounts = AccountManager->Get(mContext)
                .GetAccountsAsUser(mUserHandle->GetIdentifier());
        OnAccountsUpdated(accounts);
    }

    CARAPI ListenToAccountUpdates() {
        if (!mListeningToAccountUpdates) {
            IntentFilter intentFilter = new IntentFilter();
            intentFilter->AddAction(AccountManager.LOGIN_ACCOUNTS_CHANGED_ACTION);
            // At disk full, certain actions are blocked (such as writing the accounts to storage).
            // It is useful to also listen for recovery from disk full to avoid bugs.
            intentFilter->AddAction(IIntent::ACTION_DEVICE_STORAGE_OK);
            mContext->RegisterReceiverAsUser(this, mUserHandle, intentFilter, NULL, NULL);
            mListeningToAccountUpdates = TRUE;
        }
    }

    CARAPI StopListeningToAccountUpdates() {
        if (mListeningToAccountUpdates) {
            mContext->UnregisterReceiver(this);
            mListeningToAccountUpdates = FALSE;
        }
    }

    public ArrayList<String> GetAuthoritiesForAccountType(String type) {
        return mAccountTypeToAuthorities->Get(type);
    }

    private void BuildAccountTypeToAuthoritiesMap() {
        mAccountTypeToAuthorities->Clear();
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
                Logger::D(TAG, "Added authority " + sa.authority + " to accountType "
                        + sa.accountType);
            }
            authorities->Add(sa.authority);
        }
    }
}
