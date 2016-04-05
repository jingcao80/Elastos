

package com.android.settings;

using Elastos::Droid::Internal::Content::IPackageMonitor;

using Elastos::Droid::IManifest;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IList;

public class UsageAccessSettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceChangeListener {

    private static const String TAG = "UsageAccessSettings";

    private static const String[] PM_USAGE_STATS_PERMISSION = new String[] {
            Manifest::permission::PACKAGE_USAGE_STATS
    };

    private static const Int32[] APP_OPS_OP_CODES = new Int32[] {
            AppOpsManager.OP_GET_USAGE_STATS
    };

    private static class PackageEntry {
        public PackageEntry(String packageName) {
            this.packageName = packageName;
            this.appOpMode = AppOpsManager.MODE_DEFAULT;
        }

        final String packageName;
        PackageInfo packageInfo;
        Boolean permissionGranted;
        Int32 appOpMode;

        SwitchPreference preference;
    }

    /**
     * Fetches the list of Apps that are requesting access to the UsageStats API and updates
     * the PreferenceScreen with the results when complete.
     */
    private class AppsRequestingAccessFetcher extends
            AsyncTask<Void, Void, ArrayMap<String, PackageEntry>> {

        private final Context mContext;
        private final PackageManager mPackageManager;
        private final IPackageManager mIPackageManager;

        public AppsRequestingAccessFetcher(Context context) {
            mContext = context;
            mPackageManager = context->GetPackageManager();
            mIPackageManager = ActivityThread->GetPackageManager();
        }

        //@Override
        protected ArrayMap<String, PackageEntry> DoInBackground(Void... params) {
            final String[] packages;
            try {
                packages = mIPackageManager->GetAppOpPermissionPackages(
                        Manifest::permission::PACKAGE_USAGE_STATS);
            } catch (RemoteException e) {
                Logger::W(TAG, "PackageManager is dead. Can't get list of packages requesting "
                        + Manifest::permission::PACKAGE_USAGE_STATS);
                return NULL;
            }

            if (packages == NULL) {
                // No packages are requesting permission to use the UsageStats API.
                return NULL;
            }

            ArrayMap<String, PackageEntry> entries = new ArrayMap<>();
            for (final String packageName : packages) {
                if (!ShouldIgnorePackage(packageName)) {
                    entries->Put(packageName, new PackageEntry(packageName));
                }
            }

             // Load the packages that have been granted the PACKAGE_USAGE_STATS permission.
            final List<PackageInfo> packageInfos = mPackageManager->GetPackagesHoldingPermissions(
                    PM_USAGE_STATS_PERMISSION, 0);
            final Int32 packageInfoCount = packageInfos != NULL ? packageInfos->Size() : 0;
            for (Int32 i = 0; i < packageInfoCount; i++) {
                final PackageInfo packageInfo = packageInfos->Get(i);
                final PackageEntry pe = entries->Get(packageInfo.packageName);
                if (pe != NULL) {
                    pe.packageInfo = packageInfo;
                    pe.permissionGranted = TRUE;
                }
            }

            // Load the remaining packages that have requested but don't have the
            // PACKAGE_USAGE_STATS permission.
            Int32 packageCount = entries->Size();
            for (Int32 i = 0; i < packageCount; i++) {
                final PackageEntry pe = entries->ValueAt(i);
                if (pe.packageInfo == NULL) {
                    try {
                        pe.packageInfo = mPackageManager->GetPackageInfo(pe.packageName, 0);
                    } catch (PackageManager.NameNotFoundException e) {
                        // This package doesn't exist. This may occur when an app is uninstalled for
                        // one user, but it is not removed from the system.
                        entries->RemoveAt(i);
                        i--;
                        packageCount--;
                    }
                }
            }

            // Find out which packages have been granted permission from AppOps.
            final List<AppOpsManager.PackageOps> packageOps = mAppOpsManager->GetPackagesForOps(
                    APP_OPS_OP_CODES);
            final Int32 packageOpsCount = packageOps != NULL ? packageOps->Size() : 0;
            for (Int32 i = 0; i < packageOpsCount; i++) {
                final AppOpsManager.PackageOps packageOp = packageOps->Get(i);
                final PackageEntry pe = entries->Get(packageOp->GetPackageName());
                if (pe == NULL) {
                    Logger::W(TAG, "AppOp permission exists for package " + packageOp->GetPackageName()
                            + " but package doesn't exist or did not request UsageStats access");
                    continue;
                }

                if (packageOp->GetUid() != pe.packageInfo.applicationInfo.uid) {
                    // This AppOp does not belong to this user.
                    continue;
                }

                if (packageOp->GetOps()->Size() < 1) {
                    Logger::W(TAG, "No AppOps permission exists for package "
                            + packageOp->GetPackageName());
                    continue;
                }

                pe.appOpMode = packageOp->GetOps()->Get(0).GetMode();
            }

            return entries;
        }

        //@Override
        protected void OnPostExecute(ArrayMap<String, PackageEntry> newEntries) {
            mLastFetcherTask = NULL;

            if (GetActivity() == NULL) {
                // We must have finished the Activity while we were processing in the background.
                return;
            }

            if (newEntries == NULL) {
                mPackageEntryMap->Clear();
                mPreferenceScreen->RemoveAll();
                return;
            }

            // Find the deleted entries and remove them from the PreferenceScreen.
            final Int32 oldPackageCount = mPackageEntryMap->Size();
            for (Int32 i = 0; i < oldPackageCount; i++) {
                final PackageEntry oldPackageEntry = mPackageEntryMap->ValueAt(i);
                final PackageEntry newPackageEntry = newEntries->Get(oldPackageEntry.packageName);
                if (newPackageEntry == NULL) {
                    // This package has been removed.
                    mPreferenceScreen->RemovePreference(oldPackageEntry.preference);
                } else {
                    // This package already exists in the preference hierarchy, so reuse that
                    // Preference.
                    newPackageEntry.preference = oldPackageEntry.preference;
                }
            }

            // Now add new packages to the PreferenceScreen.
            final Int32 packageCount = newEntries->Size();
            for (Int32 i = 0; i < packageCount; i++) {
                final PackageEntry packageEntry = newEntries->ValueAt(i);
                if (packageEntry.preference == NULL) {
                    packageEntry.preference = new SwitchPreference(mContext);
                    packageEntry.preference->SetPersistent(FALSE);
                    packageEntry.preference->SetOnPreferenceChangeListener(UsageAccessSettings.this);
                    mPreferenceScreen->AddPreference(packageEntry.preference);
                }
                UpdatePreference(packageEntry);
            }

            mPackageEntryMap->Clear();
            mPackageEntryMap = newEntries;
        }

        private void UpdatePreference(PackageEntry pe) {
            pe.preference->SetIcon(pe.packageInfo.applicationInfo->LoadIcon(mPackageManager));
            pe.preference->SetTitle(pe.packageInfo.applicationInfo->LoadLabel(mPackageManager));
            pe.preference->SetKey(pe.packageName);

            Boolean check = FALSE;
            if (pe.appOpMode == AppOpsManager.MODE_ALLOWED) {
                check = TRUE;
            } else if (pe.appOpMode == AppOpsManager.MODE_DEFAULT) {
                // If the default AppOps mode is set, then fall back to
                // whether the app has been granted permission by PackageManager.
                check = pe.permissionGranted;
            }

            if (check != pe.preference->IsChecked()) {
                pe.preference->SetChecked(check);
            }
        }
    }

    static Boolean ShouldIgnorePackage(String packageName) {
        return packageName->Equals("android") || packageName->Equals("com.android.settings");
    }

    private AppsRequestingAccessFetcher mLastFetcherTask;
    ArrayMap<String, PackageEntry> mPackageEntryMap = new ArrayMap<>();
    AppOpsManager mAppOpsManager;
    PreferenceScreen mPreferenceScreen;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.usage_access_settings);
        mPreferenceScreen = GetPreferenceScreen();
        mPreferenceScreen->SetOrderingAsAdded(FALSE);
        mAppOpsManager = (AppOpsManager) GetSystemService(Context.APP_OPS_SERVICE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        UpdateInterestedApps();
        mPackageMonitor->Register(GetActivity(), Looper->GetMainLooper(), FALSE);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        mPackageMonitor->Unregister();
        if (mLastFetcherTask != NULL) {
            mLastFetcherTask->Cancel(TRUE);
            mLastFetcherTask = NULL;
        }
    }

    private void UpdateInterestedApps() {
        if (mLastFetcherTask != NULL) {
            // Canceling can only fail for some obscure reason since mLastFetcherTask would be
            // NULL if the task has already completed. So we ignore the result of cancel and
            // spawn a new task to get fresh data. AsyncTask executes tasks serially anyways,
            // so we are safe from running two tasks at the same time.
            mLastFetcherTask->Cancel(TRUE);
        }

        mLastFetcherTask = new AppsRequestingAccessFetcher(GetActivity());
        mLastFetcherTask->Execute();
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        final String packageName = preference->GetKey();
        final PackageEntry pe = mPackageEntryMap->Get(packageName);
        if (pe == NULL) {
            Logger::W(TAG, "Preference change event for package " + packageName
                    + " but that package is no longer valid.");
            return FALSE;
        }

        if (!(newValue instanceof Boolean)) {
            Logger::W(TAG, "Preference change event for package " + packageName
                    + " had non Boolean value of type " + newValue->GetClass()->GetName());
            return FALSE;
        }

        final Int32 newMode = (Boolean) newValue ?
                AppOpsManager.MODE_ALLOWED : AppOpsManager.MODE_IGNORED;

        // Check if we need to do any work.
        if (pe.appOpMode != newMode) {
            if (newMode != AppOpsManager.MODE_ALLOWED) {
                // Turning off the setting has no warning.
                SetNewMode(pe, newMode);
                return TRUE;
            }

            // Turning on the setting has a Warning.
            FragmentTransaction ft = GetChildFragmentManager()->BeginTransaction();
            Fragment prev = GetChildFragmentManager()->FindFragmentByTag("warning");
            if (prev != NULL) {
                ft->Remove(prev);
            }
            WarningDialogFragment->NewInstance(pe.packageName).Show(ft, "warning");
            return FALSE;
        }
        return TRUE;
    }

    void SetNewMode(PackageEntry pe, Int32 newMode) {
        mAppOpsManager->SetMode(AppOpsManager.OP_GET_USAGE_STATS,
        pe.packageInfo.applicationInfo.uid, pe.packageName, newMode);
        pe.appOpMode = newMode;
    }

    void AllowAccess(String packageName) {
        final PackageEntry entry = mPackageEntryMap->Get(packageName);
        if (entry == NULL) {
            Logger::W(TAG, "Unable to give access to package " + packageName + ": it does not exist.");
            return;
        }

        SetNewMode(entry, AppOpsManager.MODE_ALLOWED);
        entry.preference->SetChecked(TRUE);
    }

    private final PackageMonitor mPackageMonitor = new PackageMonitor() {
        //@Override
        CARAPI OnPackageAdded(String packageName, Int32 uid) {
            UpdateInterestedApps();
        }

        //@Override
        CARAPI OnPackageRemoved(String packageName, Int32 uid) {
            UpdateInterestedApps();
        }
    };

    public static class WarningDialogFragment extends DialogFragment
            implements DialogInterface.OnClickListener {
        private static const String ARG_PACKAGE_NAME = "package";

        public static WarningDialogFragment NewInstance(String packageName) {
            WarningDialogFragment dialog = new WarningDialogFragment();
            Bundle args = new Bundle();
            args->PutString(ARG_PACKAGE_NAME, packageName);
            dialog->SetArguments(args);
            return dialog;
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            return new AlertDialog->Builder(GetActivity())
                    .SetTitle(R::string::allow_usage_access_title)
                    .SetMessage(R::string::allow_usage_access_message)
                    .SetIconAttribute(android.R.attr.alertDialogIcon)
                    .SetNegativeButton(R::string::cancel, this)
                    .SetPositiveButton(android.R::string::ok, this)
                    .Create();
        }

        //@Override
        CARAPI OnClick(DialogInterface dialog, Int32 which) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                ((UsageAccessSettings) GetParentFragment()).AllowAccess(
                        GetArguments()->GetString(ARG_PACKAGE_NAME));
            } else {
                dialog->Cancel();
            }
        }
    }
}
