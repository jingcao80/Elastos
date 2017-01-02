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

package com.android.settings.users;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Appwidget::IAppWidgetManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IRestrictionEntry;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IMultiSelectListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Drawable::ICircleFramedDrawable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IStringTokenizer;

public class AppRestrictionsFragment extends SettingsPreferenceFragment implements
        OnPreferenceChangeListener, OnClickListener, OnPreferenceClickListener {

    private static const String TAG = AppRestrictionsFragment.class->GetSimpleName();

    private static const Boolean DEBUG = FALSE;

    private static const String PKG_PREFIX = "pkg_";

    protected PackageManager mPackageManager;
    protected UserManager mUserManager;
    protected IPackageManager mIPm;
    protected UserHandle mUser;
    private PackageInfo mSysPackageInfo;

    private PreferenceGroup mAppList;

    private static const Int32 MAX_APP_RESTRICTIONS = 100;

    private static const String DELIMITER = ";";

    /** Key for extra passed in from calling fragment for the userId of the user being edited */
    public static const String EXTRA_USER_ID = "user_id";

    /** Key for extra passed in from calling fragment to indicate if this is a newly created user */
    public static const String EXTRA_NEW_USER = "new_user";

    HashMap<String,Boolean> mSelectedPackages = new HashMap<String,Boolean>();
    private Boolean mFirstTime = TRUE;
    private Boolean mNewUser;
    private Boolean mAppListChanged;
    protected Boolean mRestrictedProfile;

    private static const Int32 CUSTOM_REQUEST_CODE_START = 1000;
    private Int32 mCustomRequestCode = CUSTOM_REQUEST_CODE_START;

    private HashMap<Integer, AppRestrictionsPreference> mCustomRequestMap =
            new HashMap<Integer,AppRestrictionsPreference>();

    private List<SelectableAppInfo> mVisibleApps;
    private List<ApplicationInfo> mUserApps;
    private AsyncTask mAppLoadingTask;

    private BroadcastReceiver mUserBackgrounding = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // Update the user's app selection right away without waiting for a pause
            // OnPause() might come in too late, causing apps to disappear after broadcasts
            // have been scheduled during user startup.
            if (mAppListChanged) {
                if (DEBUG) Logger::D(TAG, "User backgrounding, update app list");
                ApplyUserAppsStates();
                if (DEBUG) Logger::D(TAG, "User backgrounding, done updating app list");
            }
        }
    };

    private BroadcastReceiver mPackageObserver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            OnPackageChanged(intent);
        }
    };

    static class SelectableAppInfo {
        String packageName;
        CharSequence appName;
        CharSequence activityName;
        Drawable icon;
        SelectableAppInfo masterEntry;

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return packageName + ": appName=" + appName + "; activityName=" + activityName
                    + "; icon=" + icon + "; masterEntry=" + masterEntry;
        }
    }

    static class AppRestrictionsPreference extends SwitchPreference {
        private Boolean hasSettings;
        private OnClickListener listener;
        private ArrayList<RestrictionEntry> restrictions;
        private Boolean panelOpen;
        private Boolean immutable;
        private List<Preference> mChildren = new ArrayList<Preference>();

        AppRestrictionsPreference(Context context, OnClickListener listener) {
            Super(context);
            SetLayoutResource(R.layout.preference_app_restrictions);
            this.listener = listener;
        }

        private void SetSettingsEnabled(Boolean enable) {
            hasSettings = enable;
        }

        void SetRestrictions(ArrayList<RestrictionEntry> restrictions) {
            this.restrictions = restrictions;
        }

        void SetImmutable(Boolean immutable) {
            this.immutable = immutable;
        }

        Boolean IsImmutable() {
            return immutable;
        }

        RestrictionEntry GetRestriction(String key) {
            if (restrictions == NULL) return NULL;
            for (RestrictionEntry entry : restrictions) {
                if (entry->GetKey()->Equals(key)) {
                    return entry;
                }
            }
            return NULL;
        }

        ArrayList<RestrictionEntry> GetRestrictions() {
            return restrictions;
        }

        Boolean IsPanelOpen() {
            return panelOpen;
        }

        void SetPanelOpen(Boolean open) {
            panelOpen = open;
        }

        List<Preference> GetChildren() {
            return mChildren;
        }

        //@Override
        protected void OnBindView(View view) {
            super->OnBindView(view);

            View appRestrictionsSettings = view->FindViewById(R.id.app_restrictions_settings);
            appRestrictionsSettings->SetVisibility(hasSettings ? View.VISIBLE : View.GONE);
            view->FindViewById(R.id.settings_divider).SetVisibility(
                    hasSettings ? View.VISIBLE : View.GONE);
            appRestrictionsSettings->SetOnClickListener(listener);
            appRestrictionsSettings->SetTag(this);

            View appRestrictionsPref = view->FindViewById(R.id.app_restrictions_pref);
            appRestrictionsPref->SetOnClickListener(listener);
            appRestrictionsPref->SetTag(this);

            ViewGroup widget = (ViewGroup) view->FindViewById(android.R.id.widget_frame);
            widget->SetEnabled(!IsImmutable());
            if (widget->GetChildCount() > 0) {
                final Switch toggle = (Switch) widget->GetChildAt(0);
                toggle->SetEnabled(!IsImmutable());
                toggle->SetTag(this);
                toggle->SetClickable(TRUE);
                toggle->SetFocusable(TRUE);
                toggle->SetOnCheckedChangeListener(new OnCheckedChangeListener() {
                    //@Override
                    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                        listener->OnClick(toggle);
                    }
                });
            }
        }
    }

    protected void Init(Bundle icicle) {
        if (icicle != NULL) {
            mUser = new UserHandle(icicle->GetInt(EXTRA_USER_ID));
        } else {
            Bundle args = GetArguments();
            if (args != NULL) {
                if (args->ContainsKey(EXTRA_USER_ID)) {
                    mUser = new UserHandle(args->GetInt(EXTRA_USER_ID));
                }
                mNewUser = args->GetBoolean(EXTRA_NEW_USER, FALSE);
            }
        }

        if (mUser == NULL) {
            mUser = android.os.Process->MyUserHandle();
        }

        mPackageManager = GetActivity()->GetPackageManager();
        mIPm = IPackageManager.Stub->AsInterface(ServiceManager->GetService("package"));
        mUserManager = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        mRestrictedProfile = mUserManager->GetUserInfo(mUser->GetIdentifier()).IsRestricted();
        try {
            mSysPackageInfo = mPackageManager->GetPackageInfo("android",
                PackageManager.GET_SIGNATURES);
        } catch (NameNotFoundException nnfe) {
            // ?
        }
        AddPreferencesFromResource(R.xml.app_restrictions);
        mAppList = GetAppPreferenceGroup();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutInt(EXTRA_USER_ID, mUser->GetIdentifier());
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        GetActivity()->RegisterReceiver(mUserBackgrounding,
                new IntentFilter(IIntent::ACTION_USER_BACKGROUND));
        IntentFilter packageFilter = new IntentFilter();
        packageFilter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
        packageFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        packageFilter->AddDataScheme("package");
        GetActivity()->RegisterReceiver(mPackageObserver, packageFilter);

        mAppListChanged = FALSE;
        if (mAppLoadingTask == NULL || mAppLoadingTask->GetStatus() == AsyncTask.Status.FINISHED) {
            mAppLoadingTask = new AppLoadingTask()->Execute((Void[]) NULL);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mNewUser = FALSE;
        GetActivity()->UnregisterReceiver(mUserBackgrounding);
        GetActivity()->UnregisterReceiver(mPackageObserver);
        if (mAppListChanged) {
            new Thread() {
                CARAPI Run() {
                    ApplyUserAppsStates();
                }
            }.Start();
        }
    }

    private void OnPackageChanged(Intent intent) {
        String action = intent->GetAction();
        String packageName = intent->GetData()->GetSchemeSpecificPart();
        // Package added, check if the preference needs to be enabled
        AppRestrictionsPreference pref = (AppRestrictionsPreference)
                FindPreference(GetKeyForPackage(packageName));
        if (pref == NULL) return;

        if ((IIntent::ACTION_PACKAGE_ADDED->Equals(action) && pref->IsChecked())
                || (IIntent::ACTION_PACKAGE_REMOVED->Equals(action) && !pref->IsChecked())) {
            pref->SetEnabled(TRUE);
        }
    }

    protected PreferenceGroup GetAppPreferenceGroup() {
        return GetPreferenceScreen();
    }

    Drawable GetCircularUserIcon() {
        Bitmap userIcon = mUserManager->GetUserIcon(mUser->GetIdentifier());
        if (userIcon == NULL) {
            return NULL;
        }
        CircleFramedDrawable circularIcon =
                CircleFramedDrawable->GetInstance(this->GetActivity(), userIcon);
        return circularIcon;
    }

    protected void ClearSelectedApps() {
        mSelectedPackages->Clear();
    }

    private void ApplyUserAppsStates() {
        final Int32 userId = mUser->GetIdentifier();
        if (!mUserManager->GetUserInfo(userId).IsRestricted() && userId != UserHandle->MyUserId()) {
            Logger::E(TAG, "Cannot apply application restrictions on another user!");
            return;
        }
        for (Map.Entry<String,Boolean> entry : mSelectedPackages->EntrySet()) {
            String packageName = entry->GetKey();
            Boolean enabled = entry->GetValue();
            ApplyUserAppState(packageName, enabled);
        }
    }

    private void ApplyUserAppState(String packageName, Boolean enabled) {
        final Int32 userId = mUser->GetIdentifier();
        if (enabled) {
            // Enable selected apps
            try {
                ApplicationInfo info = mIPm->GetApplicationInfo(packageName,
                        PackageManager.GET_UNINSTALLED_PACKAGES, userId);
                if (info == NULL || info.enabled == FALSE
                        || (info.flags&ApplicationInfo.FLAG_INSTALLED) == 0) {
                    mIPm->InstallExistingPackageAsUser(packageName, mUser->GetIdentifier());
                    if (DEBUG) {
                        Logger::D(TAG, "Installing " + packageName);
                    }
                }
                if (info != NULL && (info.flags&ApplicationInfo.FLAG_HIDDEN) != 0
                        && (info.flags&ApplicationInfo.FLAG_INSTALLED) != 0) {
                    DisableUiForPackage(packageName);
                    mIPm->SetApplicationHiddenSettingAsUser(packageName, FALSE, userId);
                    if (DEBUG) {
                        Logger::D(TAG, "Unhiding " + packageName);
                    }
                }
            } catch (RemoteException re) {
            }
        } else {
            // Blacklist all other apps, system or downloaded
            try {
                ApplicationInfo info = mIPm->GetApplicationInfo(packageName, 0, userId);
                if (info != NULL) {
                    if (mRestrictedProfile) {
                        mIPm->DeletePackageAsUser(packageName, NULL, mUser->GetIdentifier(),
                                PackageManager.DELETE_SYSTEM_APP);
                        if (DEBUG) {
                            Logger::D(TAG, "Uninstalling " + packageName);
                        }
                    } else {
                        DisableUiForPackage(packageName);
                        mIPm->SetApplicationHiddenSettingAsUser(packageName, TRUE, userId);
                        if (DEBUG) {
                            Logger::D(TAG, "Hiding " + packageName);
                        }
                    }
                }
            } catch (RemoteException re) {
            }
        }
    }

    private void DisableUiForPackage(String packageName) {
        AppRestrictionsPreference pref = (AppRestrictionsPreference) FindPreference(
                GetKeyForPackage(packageName));
        if (pref != NULL) {
            pref->SetEnabled(FALSE);
        }
    }

    private Boolean IsSystemPackage(String packageName) {
        try {
            final PackageInfo pi = mPackageManager->GetPackageInfo(packageName, 0);
            if (pi.applicationInfo == NULL) return FALSE;
            final Int32 flags = pi.applicationInfo.flags;
            if ((flags & ApplicationInfo.FLAG_SYSTEM) != 0
                    || (flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0) {
                return TRUE;
            }
        } catch (NameNotFoundException nnfe) {
            // Missing package?
        }
        return FALSE;
    }

    /**
     * Find all pre-installed input methods that are marked as default
     * and add them to an exclusion list so that they aren't
     * presented to the user for toggling.
     * Don't add non-default ones, as they may include other stuff that we
     * don't need to auto-include.
     * @param excludePackages the set of package names to append to
     */
    private void AddSystemImes(Set<String> excludePackages) {
        final Context context = GetActivity();
        if (context == NULL) return;
        InputMethodManager imm = (InputMethodManager)
                context->GetSystemService(Context.INPUT_METHOD_SERVICE);
        List<InputMethodInfo> imis = imm->GetInputMethodList();
        for (InputMethodInfo imi : imis) {
            try {
                if (imi->IsDefault(context) && IsSystemPackage(imi->GetPackageName())) {
                    excludePackages->Add(imi->GetPackageName());
                }
            } catch (Resources.NotFoundException rnfe) {
                // Not default
            }
        }
    }

    /**
     * Add system apps that match an intent to the list, excluding any packages in the exclude list.
     * @param visibleApps list of apps to append the new list to
     * @param intent the intent to match
     * @param excludePackages the set of package names to be excluded, since they're required
     */
    private void AddSystemApps(List<SelectableAppInfo> visibleApps, Intent intent,
            Set<String> excludePackages) {
        if (GetActivity() == NULL) return;
        final PackageManager pm = mPackageManager;
        List<ResolveInfo> launchableApps = pm->QueryIntentActivities(intent,
                PackageManager.GET_DISABLED_COMPONENTS | PackageManager.GET_UNINSTALLED_PACKAGES);
        for (ResolveInfo app : launchableApps) {
            if (app.activityInfo != NULL && app.activityInfo.applicationInfo != NULL) {
                final String packageName = app.activityInfo.packageName;
                Int32 flags = app.activityInfo.applicationInfo.flags;
                if ((flags & ApplicationInfo.FLAG_SYSTEM) != 0
                        || (flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0) {
                    // System app
                    // Skip excluded packages
                    if (excludePackages->Contains(packageName)) continue;
                    Int32 enabled = pm->GetApplicationEnabledSetting(packageName);
                    if (enabled == PackageManager.COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED
                            || enabled == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                        // Check if the app is already enabled for the target user
                        ApplicationInfo targetUserAppInfo = GetAppInfoForUser(packageName,
                                0, mUser);
                        if (targetUserAppInfo == NULL
                                || (targetUserAppInfo.flags&ApplicationInfo.FLAG_INSTALLED) == 0) {
                            continue;
                        }
                    }
                    SelectableAppInfo info = new SelectableAppInfo();
                    info.packageName = app.activityInfo.packageName;
                    info.appName = app.activityInfo.applicationInfo->LoadLabel(pm);
                    info.icon = app.activityInfo->LoadIcon(pm);
                    info.activityName = app.activityInfo->LoadLabel(pm);
                    if (info.activityName == NULL) info.activityName = info.appName;

                    visibleApps->Add(info);
                }
            }
        }
    }

    private ApplicationInfo GetAppInfoForUser(String packageName, Int32 flags, UserHandle user) {
        try {
            ApplicationInfo targetUserAppInfo = mIPm->GetApplicationInfo(packageName, flags,
                    user->GetIdentifier());
            return targetUserAppInfo;
        } catch (RemoteException re) {
            return NULL;
        }
    }

    private class AppLoadingTask extends AsyncTask<Void, Void, Void> {

        //@Override
        protected Void DoInBackground(Void... params) {
            FetchAndMergeApps();
            return NULL;
        }

        //@Override
        protected void OnPostExecute(Void result) {
            PopulateApps();
        }

        //@Override
        protected void OnPreExecute() {
        }
    }

    private void FetchAndMergeApps() {
        mAppList->SetOrderingAsAdded(FALSE);
        mVisibleApps = new ArrayList<SelectableAppInfo>();
        final Context context = GetActivity();
        if (context == NULL) return;
        final PackageManager pm = mPackageManager;
        final IPackageManager ipm = mIPm;

        final HashSet<String> excludePackages = new HashSet<String>();
        AddSystemImes(excludePackages);

        // Add launchers
        Intent launcherIntent = new Intent(IIntent::ACTION_MAIN);
        launcherIntent->AddCategory(Intent.CATEGORY_LAUNCHER);
        AddSystemApps(mVisibleApps, launcherIntent, excludePackages);

        // Add widgets
        Intent widgetIntent = new Intent(AppWidgetManager.ACTION_APPWIDGET_UPDATE);
        AddSystemApps(mVisibleApps, widgetIntent, excludePackages);

        List<ApplicationInfo> installedApps = pm->GetInstalledApplications(
                PackageManager.GET_UNINSTALLED_PACKAGES);
        for (ApplicationInfo app : installedApps) {
            // If it's not installed, skip
            if ((app.flags & ApplicationInfo.FLAG_INSTALLED) == 0) continue;

            if ((app.flags & ApplicationInfo.FLAG_SYSTEM) == 0
                    && (app.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) == 0) {
                // Downloaded app
                SelectableAppInfo info = new SelectableAppInfo();
                info.packageName = app.packageName;
                info.appName = app->LoadLabel(pm);
                info.activityName = info.appName;
                info.icon = app->LoadIcon(pm);
                mVisibleApps->Add(info);
            } else {
                try {
                    PackageInfo pi = pm->GetPackageInfo(app.packageName, 0);
                    // If it's a system app that requires an account and doesn't see restricted
                    // accounts, mark for removal. It might get shown in the UI if it has an icon
                    // but will still be marked as FALSE and immutable.
                    if (mRestrictedProfile
                            && pi.requiredAccountType != NULL && pi.restrictedAccountType == NULL) {
                        mSelectedPackages->Put(app.packageName, FALSE);
                    }
                } catch (NameNotFoundException re) {
                }
            }
        }

        // Get the list of apps already installed for the user
        mUserApps = NULL;
        try {
            mUserApps = ipm->GetInstalledApplications(
                    PackageManager.GET_UNINSTALLED_PACKAGES, mUser->GetIdentifier()).GetList();
        } catch (RemoteException re) {
        }

        if (mUserApps != NULL) {
            for (ApplicationInfo app : mUserApps) {
                if ((app.flags & ApplicationInfo.FLAG_INSTALLED) == 0) continue;

                if ((app.flags & ApplicationInfo.FLAG_SYSTEM) == 0
                        && (app.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) == 0) {
                    // Downloaded app
                    SelectableAppInfo info = new SelectableAppInfo();
                    info.packageName = app.packageName;
                    info.appName = app->LoadLabel(pm);
                    info.activityName = info.appName;
                    info.icon = app->LoadIcon(pm);
                    mVisibleApps->Add(info);
                }
            }
        }

        // Sort the list of visible apps
        Collections->Sort(mVisibleApps, new AppLabelComparator());

        // Remove dupes
        Set<String> dedupPackageSet = new HashSet<String>();
        for (Int32 i = mVisibleApps->Size() - 1; i >= 0; i--) {
            SelectableAppInfo info = mVisibleApps->Get(i);
            if (DEBUG) Logger::I(TAG, info->ToString());
            String both = info.packageName + "+" + info.activityName;
            if (!TextUtils->IsEmpty(info.packageName)
                    && !TextUtils->IsEmpty(info.activityName)
                    && dedupPackageSet->Contains(both)) {
                mVisibleApps->Remove(i);
            } else {
                dedupPackageSet->Add(both);
            }
        }

        // Establish master/slave relationship for entries that share a package name
        HashMap<String,SelectableAppInfo> packageMap = new HashMap<String,SelectableAppInfo>();
        for (SelectableAppInfo info : mVisibleApps) {
            if (packageMap->ContainsKey(info.packageName)) {
                info.masterEntry = packageMap->Get(info.packageName);
            } else {
                packageMap->Put(info.packageName, info);
            }
        }
    }

    private Boolean IsPlatformSigned(PackageInfo pi) {
        return (pi != NULL && pi.signatures != NULL &&
                    mSysPackageInfo.signatures[0].Equals(pi.signatures[0]));
    }

    private Boolean IsAppEnabledForUser(PackageInfo pi) {
        if (pi == NULL) return FALSE;
        final Int32 flags = pi.applicationInfo.flags;
        // Return TRUE if it is installed and not hidden
        return ((flags&ApplicationInfo.FLAG_INSTALLED) != 0
                && (flags&ApplicationInfo.FLAG_HIDDEN) == 0);
    }

    private void PopulateApps() {
        final Context context = GetActivity();
        if (context == NULL) return;
        final PackageManager pm = mPackageManager;
        final IPackageManager ipm = mIPm;

        mAppList->RemoveAll();
        Intent restrictionsIntent = new Intent(IIntent::ACTION_GET_RESTRICTION_ENTRIES);
        final List<ResolveInfo> receivers = pm->QueryBroadcastReceivers(restrictionsIntent, 0);
        Int32 i = 0;
        if (mVisibleApps->Size() > 0) {
            for (SelectableAppInfo app : mVisibleApps) {
                String packageName = app.packageName;
                if (packageName == NULL) continue;
                final Boolean isSettingsApp = packageName->Equals(context->GetPackageName());
                AppRestrictionsPreference p = new AppRestrictionsPreference(context, this);
                final Boolean hasSettings = ResolveInfoListHasPackage(receivers, packageName);
                p->SetIcon(app.icon != NULL ? app.icon->Mutate() : NULL);
                p->SetChecked(FALSE);
                p->SetTitle(app.activityName);
                if (app.masterEntry != NULL) {
                    p->SetSummary(context->GetString(R::string::user_restrictions_controlled_by,
                            app.masterEntry.activityName));
                }
                p->SetKey(GetKeyForPackage(packageName));
                p->SetSettingsEnabled((hasSettings || isSettingsApp) && app.masterEntry == NULL);
                p->SetPersistent(FALSE);
                p->SetOnPreferenceChangeListener(this);
                p->SetOnPreferenceClickListener(this);
                PackageInfo pi = NULL;
                try {
                    pi = ipm->GetPackageInfo(packageName,
                            PackageManager.GET_UNINSTALLED_PACKAGES
                            | PackageManager.GET_SIGNATURES, mUser->GetIdentifier());
                } catch (RemoteException e) {
                }
                if (pi != NULL && (pi.requiredForAllUsers || IsPlatformSigned(pi))) {
                    p->SetChecked(TRUE);
                    p->SetImmutable(TRUE);
                    // If the app is required and has no restrictions, skip showing it
                    if (!hasSettings && !isSettingsApp) continue;
                    // Get and populate the defaults, since the user is not going to be
                    // able to toggle this app ON (it's ON by default and immutable).
                    // Only do this for restricted profiles, not single-user restrictions
                    // Also don't do this for slave icons
                    if (hasSettings && app.masterEntry == NULL) {
                        RequestRestrictionsForApp(packageName, p, FALSE);
                    }
                } else if (!mNewUser && IsAppEnabledForUser(pi)) {
                    p->SetChecked(TRUE);
                }
                if (mRestrictedProfile
                        && pi.requiredAccountType != NULL && pi.restrictedAccountType == NULL) {
                    p->SetChecked(FALSE);
                    p->SetImmutable(TRUE);
                    p->SetSummary(R::string::app_not_supported_in_limited);
                }
                if (mRestrictedProfile && pi.restrictedAccountType != NULL) {
                    p->SetSummary(R::string::app_sees_restricted_accounts);
                }
                if (app.masterEntry != NULL) {
                    p->SetImmutable(TRUE);
                    p->SetChecked(mSelectedPackages->Get(packageName));
                }
                mAppList->AddPreference(p);
                if (isSettingsApp) {
                    p->SetOrder(MAX_APP_RESTRICTIONS * 1);
                } else {
                    p->SetOrder(MAX_APP_RESTRICTIONS * (i + 2));
                }
                mSelectedPackages->Put(packageName, p->IsChecked());
                mAppListChanged = TRUE;
                i++;
            }
        }
        // If this is the first time for a new profile, install/uninstall default apps for profile
        // to avoid taking the hit in OnPause(), which can cause race conditions on user switch.
        if (mNewUser && mFirstTime) {
            mFirstTime = FALSE;
            ApplyUserAppsStates();
        }
    }

    private String GetKeyForPackage(String packageName) {
        return PKG_PREFIX + packageName;
    }

    private class AppLabelComparator implements Comparator<SelectableAppInfo> {

        //@Override
        public Int32 Compare(SelectableAppInfo lhs, SelectableAppInfo rhs) {
            String lhsLabel = lhs.activityName->ToString();
            String rhsLabel = rhs.activityName->ToString();
            return lhsLabel->ToLowerCase()->CompareTo(rhsLabel->ToLowerCase());
        }
    }

    private Boolean ResolveInfoListHasPackage(List<ResolveInfo> receivers, String packageName) {
        for (ResolveInfo info : receivers) {
            if (info.activityInfo.packageName->Equals(packageName)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private void UpdateAllEntries(String prefKey, Boolean checked) {
        for (Int32 i = 0; i < mAppList->GetPreferenceCount(); i++) {
            Preference pref = mAppList->GetPreference(i);
            if (pref instanceof AppRestrictionsPreference) {
                if (prefKey->Equals(pref->GetKey())) {
                    ((AppRestrictionsPreference) pref).SetChecked(checked);
                }
            }
        }
    }

    //@Override
    CARAPI OnClick(View v) {
        if (v->GetTag() instanceof AppRestrictionsPreference) {
            AppRestrictionsPreference pref = (AppRestrictionsPreference) v->GetTag();
            if (v->GetId() == R.id.app_restrictions_settings) {
                OnAppSettingsIconClicked(pref);
            } else if (!pref->IsImmutable()) {
                pref->SetChecked(!pref->IsChecked());
                final String packageName = pref->GetKey()->Substring(PKG_PREFIX->Length());
                mSelectedPackages->Put(packageName, pref->IsChecked());
                if (pref->IsChecked() && pref.hasSettings
                        && pref.restrictions == NULL) {
                    // The restrictions have not been initialized, get and save them
                    RequestRestrictionsForApp(packageName, pref, FALSE);
                }
                mAppListChanged = TRUE;
                // If it's not a restricted profile, apply the changes immediately
                if (!mRestrictedProfile) {
                    ApplyUserAppState(packageName, pref->IsChecked());
                }
                UpdateAllEntries(pref->GetKey(), pref->IsChecked());
            }
        }
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        String key = preference->GetKey();
        if (key != NULL && key->Contains(DELIMITER)) {
            StringTokenizer st = new StringTokenizer(key, DELIMITER);
            final String packageName = st->NextToken();
            final String restrictionKey = st->NextToken();
            AppRestrictionsPreference appPref = (AppRestrictionsPreference)
                    mAppList->FindPreference(PKG_PREFIX+packageName);
            ArrayList<RestrictionEntry> restrictions = appPref->GetRestrictions();
            if (restrictions != NULL) {
                for (RestrictionEntry entry : restrictions) {
                    if (entry->GetKey()->Equals(restrictionKey)) {
                        switch (entry->GetType()) {
                        case RestrictionEntry.TYPE_BOOLEAN:
                            entry->SetSelectedState((Boolean) newValue);
                            break;
                        case RestrictionEntry.TYPE_CHOICE:
                        case RestrictionEntry.TYPE_CHOICE_LEVEL:
                            ListPreference listPref = (ListPreference) preference;
                            entry->SetSelectedString((String) newValue);
                            String readable = FindInArray(entry->GetChoiceEntries(),
                                    entry->GetChoiceValues(), (String) newValue);
                            listPref->SetSummary(readable);
                            break;
                        case RestrictionEntry.TYPE_MULTI_SELECT:
                            Set<String> set = (Set<String>) newValue;
                            String [] selectedValues = new String[set->Size()];
                            set->ToArray(selectedValues);
                            entry->SetAllSelectedStrings(selectedValues);
                            break;
                        default:
                            continue;
                        }
                        if (packageName->Equals(GetActivity()->GetPackageName())) {
                            RestrictionUtils->SetRestrictions(GetActivity(), restrictions, mUser);
                        } else {
                            mUserManager->SetApplicationRestrictions(packageName,
                                    RestrictionUtils->RestrictionsToBundle(restrictions),
                                    mUser);
                        }
                        break;
                    }
                }
            }
        }
        return TRUE;
    }

    private void RemoveRestrictionsForApp(AppRestrictionsPreference preference) {
        for (Preference p : preference.mChildren) {
            mAppList->RemovePreference(p);
        }
        preference.mChildren->Clear();
    }

    private void OnAppSettingsIconClicked(AppRestrictionsPreference preference) {
        if (preference->GetKey()->StartsWith(PKG_PREFIX)) {
            if (preference->IsPanelOpen()) {
                RemoveRestrictionsForApp(preference);
            } else {
                String packageName = preference->GetKey()->Substring(PKG_PREFIX->Length());
                if (packageName->Equals(GetActivity()->GetPackageName())) {
                    // Settings, fake it by using user restrictions
                    ArrayList<RestrictionEntry> restrictions = RestrictionUtils->GetRestrictions(
                            GetActivity(), mUser);
                    OnRestrictionsReceived(preference, packageName, restrictions);
                } else {
                    RequestRestrictionsForApp(packageName, preference, TRUE /*invoke if custom*/);
                }
            }
            preference->SetPanelOpen(!preference->IsPanelOpen());
        }
    }

    /**
     * Send a broadcast to the app to query its restrictions
     * @param packageName package name of the app with restrictions
     * @param preference the preference item for the app toggle
     * @param invokeIfCustom whether to directly launch any custom activity that is returned
     *        for the app.
     */
    private void RequestRestrictionsForApp(String packageName,
            AppRestrictionsPreference preference, Boolean invokeIfCustom) {
        Bundle oldEntries =
                mUserManager->GetApplicationRestrictions(packageName, mUser);
        Intent intent = new Intent(IIntent::ACTION_GET_RESTRICTION_ENTRIES);
        intent->SetPackage(packageName);
        intent->PutExtra(Intent.EXTRA_RESTRICTIONS_BUNDLE, oldEntries);
        intent->AddFlags(IIntent::FLAG_INCLUDE_STOPPED_PACKAGES);
        GetActivity()->SendOrderedBroadcast(intent, NULL,
                new RestrictionsResultReceiver(packageName, preference, invokeIfCustom),
                NULL, Activity.RESULT_OK, NULL, NULL);
    }

    class RestrictionsResultReceiver extends BroadcastReceiver {

        private static const String CUSTOM_RESTRICTIONS_INTENT = Intent.EXTRA_RESTRICTIONS_INTENT;
        String packageName;
        AppRestrictionsPreference preference;
        Boolean invokeIfCustom;

        RestrictionsResultReceiver(String packageName, AppRestrictionsPreference preference,
                Boolean invokeIfCustom) {
            Super();
            this.packageName = packageName;
            this.preference = preference;
            this.invokeIfCustom = invokeIfCustom;
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            Bundle results = GetResultExtras(TRUE);
            final ArrayList<RestrictionEntry> restrictions = results->GetParcelableArrayList(
                    Intent.EXTRA_RESTRICTIONS_LIST);
            Intent restrictionsIntent = (Intent) results->GetParcelable(CUSTOM_RESTRICTIONS_INTENT);
            if (restrictions != NULL && restrictionsIntent == NULL) {
                OnRestrictionsReceived(preference, packageName, restrictions);
                if (mRestrictedProfile) {
                    mUserManager->SetApplicationRestrictions(packageName,
                            RestrictionUtils->RestrictionsToBundle(restrictions), mUser);
                }
            } else if (restrictionsIntent != NULL) {
                preference->SetRestrictions(restrictions);
                if (invokeIfCustom && AppRestrictionsFragment.this->IsResumed()) {
                    Int32 requestCode = GenerateCustomActivityRequestCode(
                            RestrictionsResultReceiver.this.preference);
                    AppRestrictionsFragment.this->StartActivityForResult(
                            restrictionsIntent, requestCode);
                }
            }
        }
    }

    private void OnRestrictionsReceived(AppRestrictionsPreference preference, String packageName,
            ArrayList<RestrictionEntry> restrictions) {
        // Remove any earlier restrictions
        RemoveRestrictionsForApp(preference);
        // Non-custom-activity case - expand the restrictions in-place
        final Context context = preference->GetContext();
        Int32 count = 1;
        for (RestrictionEntry entry : restrictions) {
            Preference p = NULL;
            switch (entry->GetType()) {
            case RestrictionEntry.TYPE_BOOLEAN:
                p = new CheckBoxPreference(context);
                p->SetTitle(entry->GetTitle());
                p->SetSummary(entry->GetDescription());
                ((CheckBoxPreference)p).SetChecked(entry->GetSelectedState());
                break;
            case RestrictionEntry.TYPE_CHOICE:
            case RestrictionEntry.TYPE_CHOICE_LEVEL:
                p = new ListPreference(context);
                p->SetTitle(entry->GetTitle());
                String value = entry->GetSelectedString();
                if (value == NULL) {
                    value = entry->GetDescription();
                }
                p->SetSummary(FindInArray(entry->GetChoiceEntries(), entry->GetChoiceValues(),
                        value));
                ((ListPreference)p).SetEntryValues(entry->GetChoiceValues());
                ((ListPreference)p).SetEntries(entry->GetChoiceEntries());
                ((ListPreference)p).SetValue(value);
                ((ListPreference)p).SetDialogTitle(entry->GetTitle());
                break;
            case RestrictionEntry.TYPE_MULTI_SELECT:
                p = new MultiSelectListPreference(context);
                p->SetTitle(entry->GetTitle());
                ((MultiSelectListPreference)p).SetEntryValues(entry->GetChoiceValues());
                ((MultiSelectListPreference)p).SetEntries(entry->GetChoiceEntries());
                HashSet<String> set = new HashSet<String>();
                for (String s : entry->GetAllSelectedStrings()) {
                    set->Add(s);
                }
                ((MultiSelectListPreference)p).SetValues(set);
                ((MultiSelectListPreference)p).SetDialogTitle(entry->GetTitle());
                break;
            case RestrictionEntry.TYPE_NULL:
            default:
            }
            if (p != NULL) {
                p->SetPersistent(FALSE);
                p->SetOrder(preference->GetOrder() + count);
                // Store the restrictions key string as a key for the preference
                p->SetKey(preference->GetKey()->Substring(PKG_PREFIX->Length()) + DELIMITER
                        + entry->GetKey());
                mAppList->AddPreference(p);
                p->SetOnPreferenceChangeListener(AppRestrictionsFragment.this);
                p->SetIcon(R.drawable.empty_icon);
                preference.mChildren->Add(p);
                count++;
            }
        }
        preference->SetRestrictions(restrictions);
        if (count == 1 // No visible restrictions
                && preference->IsImmutable()
                && preference->IsChecked()) {
            // Special case of required app with no visible restrictions. Remove it
            mAppList->RemovePreference(preference);
        }
    }

    /**
     * Generates a request code that is stored in a map to retrieve the associated
     * AppRestrictionsPreference.
     * @param preference
     * @return
     */
    private Int32 GenerateCustomActivityRequestCode(AppRestrictionsPreference preference) {
        mCustomRequestCode++;
        mCustomRequestMap->Put(mCustomRequestCode, preference);
        return mCustomRequestCode;
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        AppRestrictionsPreference pref = mCustomRequestMap->Get(requestCode);
        if (pref == NULL) {
            Logger::W(TAG, "Unknown requestCode " + requestCode);
            return;
        }

        if (resultCode == Activity.RESULT_OK) {
            String packageName = pref->GetKey()->Substring(PKG_PREFIX->Length());
            ArrayList<RestrictionEntry> list =
                    data->GetParcelableArrayListExtra(Intent.EXTRA_RESTRICTIONS_LIST);
            Bundle bundle = data->GetBundleExtra(Intent.EXTRA_RESTRICTIONS_BUNDLE);
            if (list != NULL) {
                // If there's a valid result, persist it to the user manager.
                pref->SetRestrictions(list);
                mUserManager->SetApplicationRestrictions(packageName,
                        RestrictionUtils->RestrictionsToBundle(list), mUser);
            } else if (bundle != NULL) {
                // If there's a valid result, persist it to the user manager.
                mUserManager->SetApplicationRestrictions(packageName, bundle, mUser);
            }
        }
        // Remove request from the map
        mCustomRequestMap->Remove(requestCode);
    }

    private String FindInArray(String[] choiceEntries, String[] choiceValues,
            String selectedString) {
        for (Int32 i = 0; i < choiceValues.length; i++) {
            if (choiceValues[i].Equals(selectedString)) {
                return choiceEntries[i];
            }
        }
        return selectedString;
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference->GetKey()->StartsWith(PKG_PREFIX)) {
            AppRestrictionsPreference arp = (AppRestrictionsPreference) preference;
            if (!arp->IsImmutable()) {
                final String packageName = arp->GetKey()->Substring(PKG_PREFIX->Length());
                final Boolean newEnabledState = !arp->IsChecked();
                arp->SetChecked(newEnabledState);
                mSelectedPackages->Put(packageName, newEnabledState);
                UpdateAllEntries(arp->GetKey(), newEnabledState);
                mAppListChanged = TRUE;
                ApplyUserAppState(packageName, newEnabledState);
            }
            return TRUE;
        }
        return FALSE;
    }

}
