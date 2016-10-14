#ifndef __ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__

// #include "Elastos.Droid.Hardware.h"
// #include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_CUsageAccessSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

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

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CUsageAccessSettings)
    , public SettingsPreferenceFragment
    , public IPreferenceOnPreferenceChangeListener
{
public:
    class WarningDialogFragment
        : public DialogFragment
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CUsageAccessSettings::WarningDialogFragment")

        CAR_INTERFACE_DECL()

        WarningDialogFragment();

        virtual ~WarningDialogFragment();

        CARAPI constructor();
        {
            return DialogFragment::constructor();
        }

        public static AutoPtr<WarningDialogFragment> NewInstance(
            /* [in] */ const String& packageName)
        {
            AutoPtr<WarningDialogFragment> dialog = new WarningDialogFragment();
            dialog->constructor();
            AutoPtr<IBundle> args;
            CBundle::New((IBundle**)&args);
            args->PutString(ARG_PACKAGE_NAME, packageName);
            dialog->SetArguments(args);
            return dialog;
        }

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** dialog)
        {
            VALIDATE_NOT_NULL(dialog)
            *dialog = NULL;

            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::allow_usage_access_title);
            builder->SetMessage(R::string::allow_usage_access_message);
            builder->SetIconAttribute(Elastos::Droid::R::attr::alertDialogIcon);
            builder->SetNegativeButton(R::string::cancel, this);
            builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
            AutoPtr<IAlertDialog> result;
            builder->Create((IAlertDialog**)&result);
            *dialog = IDialog::Probe(result);
            REFCOUNT_ADD(*dialog);
            return NOERROR;
        }

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which)
        {
            if (which == IDialogInterface::BUTTON_POSITIVE) {
                AutoPtr<IFragment> fragment;
                GetParentFragment((IFragment**)&fragment);
                AutoPtr<IBundle> arguments;
                GetArguments((IBundle**)&arguments);
                String str;
                arguments->GetString(ARG_PACKAGE_NAME, &str);
                ((CUsageAccessSettings*)fragment.Get())->AllowAccess(str);
            }
            else {
                dialog->Cancel();
            }
            return NOERROR;
        }

    private:
        static const String ARG_PACKAGE_NAME = "package";
    };

private:
    class PackageEntry
        : public Object
    {
    public:
        TO_STRING_IMPL("CUsageAccessSettings::PackageEntry")

        PackageEntry(
            /* [in] */ const String& packageName)
            : mPermissionGranted(FALSE)
            , mAppOpMode(0)
        {
            mPackageName = packageName;
            mAppOpMode = IAppOpsManager::MODE_DEFAULT;
        }

    public:
        String mPackageName;
        AutoPtr<IPackageInfo> mPackageInfo;
        Boolean mPermissionGranted;
        Int32 mAppOpMode;

        AutoPtr<ISwitchPreference> mPreference;
    };

    /**
     * Fetches the list of Apps that are requesting access to the UsageStats API and updates
     * the PreferenceScreen with the results when complete.
     */
    class AppsRequestingAccessFetcher
        // AsyncTask<Void, Void, ArrayMap<String, PackageEntry>>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CUsageAccessSettings::AppsRequestingAccessFetcher")

        AppsRequestingAccessFetcher(
            /* [in] */ IContext* context,
            /* [in] */ CUsageAccessSettings* host)
            : mHost(host)
        {
            mContext = context;
            context->GetPackageManager((IPackageManager**)&mPackageManager);
            AutoPtr<IActivityThreadHelper> helper;
            CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
            helper->GetPackageManager((IIPackageManager**)&mIPackageManager);
        }

        ~AppsRequestingAccessFetcher();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result); //ArrayMap<String, PackageEntry>
        {
            VALIDATE_NOT_NULL(result)
            *result = NULL;

            AutoPtr< ArrayOf<String> > packages;
            // try {
            ECode ec = mIPackageManager->GetAppOpPermissionPackages(
                    Manifest::permission::PACKAGE_USAGE_STATS, (ArrayOf<String>**)&packages);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::W(TAG, "PackageManager is dead. Can't get list of packages requesting "
                        , Manifest::permission::PACKAGE_USAGE_STATS);
                return NOERROR;
            }

            if (packages == NULL) {
                // No packages are requesting permission to use the UsageStats API.
                return NOERROR;
            }

            AutoPtr<IArrayMap> entries; //ArrayMap<String, PackageEntry>
            CArrayMap::New((IArrayMap**)&entries);
            for (Int32 i = 0; i < packages->GetLength(); i++) {
                String packageName = (*packages)[i];
                if (!ShouldIgnorePackage(packageName)) {
                    AutoPtr<PackageEntry> pe = new PackageEntry(packageName);
                    entries->Put(CoreUtils::Convert(packageName), (IObject*)pe);
                }
            }

             // Load the packages that have been granted the PACKAGE_USAGE_STATS permission.
            AutoPtr<IList> packageInfos;//List<PackageInfo>
            mPackageManager->GetPackagesHoldingPermissions(
                    PM_USAGE_STATS_PERMISSION, 0, (IList**)&packageInfos);
            Int32 packageInfoCount = 0;
            if (packageInfos != NULL) {
                packageInfos->GetSize(&packageInfoCount);
            }
            for (Int32 i = 0; i < packageInfoCount; i++) {
                AutoPtr<IInterface> obj;
                packageInfos->Get(i, (IInterface**)&obj);
                IPackageInfo* packageInfo = IPackageInfo::Probe(obj);
                String packageName;
                packageInfo->GetPackageName(&packageName);
                obj = NULL;
                entries->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
                AutoPtr<PackageEntry> pe = (PackageEntry*)IObject::Probe(obj);
                if (pe != NULL) {
                    pe->mPackageInfo = packageInfo;
                    pe->mPermissionGranted = TRUE;
                }
            }

            // Load the remaining packages that have requested but don't have the
            // PACKAGE_USAGE_STATS permission.
            Int32 packageCount;
            entries->GetSize(&packageCount);
            for (Int32 i = 0; i < packageCount; i++) {
                AutoPtr<IInterface> obj;
                entries->ValueAt(i, (IInterface**)&obj);
                AutoPtr<PackageEntry> pe = (PackageEntry*)IObject::Probe(obj);
                if (pe->mPackageInfo == NULL) {
                    ec = mPackageManager->GetPackageInfo(pe->mPermissionGranted, 0, (IPackageInfo**)&pe->mPackageInfo);
                    if (ec == (ECode) E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
                        // This package doesn't exist. This may occur when an app is uninstalled for
                        // one user, but it is not removed from the system.
                        entries->RemoveAt(i);
                        i--;
                        packageCount--;
                    }
                }
            }

            // Find out which packages have been granted permission from AppOps.
            AutoPtr<IList> packageOps; //List<IAppOpsManagerPackageOps>
            mHost->mAppOpsManager->GetPackagesForOps(APP_OPS_OP_CODES, (IList**)&packageOps);
            Int32 packageOpsCount = 0;
            if(packageOps != NULL) {
                packageOps->GetSize(&packageOpsCount);
            }
            for (Int32 i = 0; i < packageOpsCount; i++) {
                AutoPtr<IInterface> obj;
                packageOps->Get(i, (IInterface**)&obj);
                IAppOpsManagerPackageOps* packageOp = IAppOpsManagerPackageOps::Probe(obj);
                String packageName;
                packageOp->GetPackageName(&packageName);
                obj = NULL;
                entries->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
                AutoPtr<PackageEntry> pe = (PackageEntry*)IObject::Probe(obj);
                if (pe == NULL) {
                    Logger::W(TAG, "AppOp permission exists for package %s but package doesn't exist or did not request UsageStats access", packageName.string());
                    continue;
                }

                Int32 uid, pUid;
                AutoPtr<IApplicationInfo> applicationInfo;
                pe->mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
                applicationInfo->GetUid(&pUid);
                if (packageOp->GetUid(&uid) != pUid) {
                    // This AppOp does not belong to this user.
                    continue;
                }

                AutoPtr<IList> ops;
                packageOp->GetOps((IList**)&ops);
                Int32 size;
                if (ops->GetSize(&size), size < 1) {
                    Logger::W(TAG, "No AppOps permission exists for package %s", packageName.string());
                    continue;
                }

                obj = NULL;
                ops->Get(0, (IInterface**)&obj);
                IAppOpsManagerOpEntry::Probe(obj)->GetMode(&pe->mAppOpMode);
            }

            *result = entries;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        //@Override
        protected
        CARAPI OnPostExecute(
            /* [in] */ IArrayMap* newEntries);// ArrayMap<String, PackageEntry>
        {
            mHost->mLastFetcherTask = NULL;

            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            if (activity == NULL) {
                // We must have finished the Activity while we were processing in the background.
                return NOERROR;
            }

            if (newEntries == NULL) {
                mHost->mPackageEntryMap->Clear();
                mHost->mPreferenceScreen->RemoveAll();
                return NOERROR;
            }

            // Find the deleted entries and remove them from the PreferenceScreen.
            Int32 oldPackageCount;
            mHost->mPackageEntryMap->GetSize(&oldPackageCount);
            for (Int32 i = 0; i < oldPackageCount; i++) {
                AutoPtr<IInterface> obj;
                mHost->mPackageEntryMap->ValueAt(i, (IInterface**)&obj);
                AutoPtr<PackageEntry> oldPackageEntry = (PackageEntry*)IObject::Probe(obj);
                obj = NULL;
                newEntries->Get(CoreUtils::Convert(oldPackageEntry->mPermissionGranted), (IInterface**)&obj);
                AutoPtr<PackageEntry> newPackageEntry = (PackageEntry*)IObject::Probe(obj);
                if (newPackageEntry == NULL) {
                    // This package has been removed.
                    Boolean res;
                    IPreferenceGroup::Probe(mHost->mPreferenceScreen)->RemovePreference(oldPackageEntry->mPreference, &res);
                }
                else {
                    // This package already exists in the preference hierarchy, so reuse that
                    // Preference.
                    newPackageEntry->mPreference = oldPackageEntry->mPreference;
                }
            }

            // Now add new packages to the PreferenceScreen.
            Int32 packageCount;
            newEntries->GetSize(&packageCount);
            for (Int32 i = 0; i < packageCount; i++) {
                AutoPtr<IInterface> obj;
                newEntries->ValueAt(i, (IInterface**)&obj);
                AutoPtr<PackageEntry> packageEntry = (PackageEntry*)IObject::Probe(obj);
                if (packageEntry->mPreference == NULL) {
                    CSwitchPreference::New(mContext, (ISwitchPreference**)&packageEntry->mPreference);
                    packageEntry->mPreference->SetPersistent(FALSE);
                    packageEntry->mPreference->SetOnPreferenceChangeListener(mHost);
                    mHost->mPreferenceScreen->AddPreference(packageEntry->mPreference);
                }
                UpdatePreference(packageEntry);
            }

            mHost->mPackageEntryMap->Clear();
            mHost->mPackageEntryMap = newEntries;
            return NOERROR;
        }

        private void UpdatePreference(
            /* [in] */ PackageEntry* pe)
        {
            AutoPtr<IApplicationInfo> applicationInfo;
            pe->mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            AutoPtr<IDrawable> icon;
            IPackageItemInfo::Probe(applicationInfo)->LoadIcon(mPackageManager, (IDrawable**)&icon);
            pe->mPreference->SetIcon(icon);
            AutoPtr<ICharSequence> cs;
            IPackageItemInfo::Probe(applicationInfo)->LoadLabel(mPackageManager, (ICharSequence**)&cs);
            pe->mPreference->SetTitle(cs);
            pe->mPreference->SetKey(pe->mPermissionGranted);

            Boolean check = FALSE;
            if (pe->mAppOpMode == IAppOpsManager::MODE_ALLOWED) {
                check = TRUE;
            }
            else if (pe->mAppOpMode == IAppOpsManager::MODE_DEFAULT) {
                // If the default AppOps mode is set, then fall back to
                // whether the app has been granted permission by PackageManager.
                check = pe->mPermissionGranted;
            }

            Boolean res;
            if (check != (pe->mPreference->IsChecked(&res), res)) {
                pe->mPreference->SetChecked(check);
            }
        }

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IPackageManager> mPackageManager;
        AutoPtr<IIPackageManager> mIPackageManager;
        CUsageAccessSettings* mHost;
    };

    class MyPackageMonitor
        : public PackageMonitor
    {
    public:
        TO_STRING_IMPL("CUsageAccessSettings::MyPackageMonitor");

        MyPackageMonitor(
            /* [in] */ CUsageAccessSettings* host);
            : mHost(host)
        {}

        CARAPI constructor();
        {
            return PackageMonitor::constructor();
        }

        ~MyPackageMonitor();

        //@Override
        CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid)
        {
            mHost->UpdateInterestedApps();
            return NOERROR;
        }

        //@Override
        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid)
        {
            mHost->UpdateInterestedApps();
            return NOERROR;
        }

    private:
        CUsageAccessSettings* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUsageAccessSettings();

    ~CUsageAccessSettings();

    CARAPI constructor();
    {
        CArrayMap::New((IArrayMap**)&mPackageEntryMap);
        mPackageMonitor = new MyPackageMonitor(this);
        mPackageMonitor->constructor();

        return SettingsPreferenceFragment::constructor();
    }

    static Boolean ShouldIgnorePackage(
        /* [in] */ const String& packageName)
    {
        return packageName.Equals("android") || packageName.Equals("Elastos.Droid.Settings");
    }

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle)
    {
        SettingsPreferenceFragment::OnCreate(icicle);

        AddPreferencesFromResource(R::xml::usage_access_settings);
        GetPreferenceScreen((IPreferenceScreen**)&mPreferenceScreen);
        IPreferenceGroup::Probe(mPreferenceScreen)->SetOrderingAsAdded(FALSE);
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
        mAppOpsManager = IAppOpsManager::Probe(obj);
        return NOERROR;
    }

    //@Override
    CARAPI OnResume()
    {
        SettingsPreferenceFragment::OnResume();

        UpdateInterestedApps();
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        mPackageMonitor->Register(IContext::Probe(activity), Looper::GetMainLooper(), FALSE);
        return NOERROR;
    }

    //@Override
    CARAPI OnPause()
    {
        SettingsPreferenceFragment::OnPause();

        mPackageMonitor->Unregister();
        if (mLastFetcherTask != NULL) {
            mLastFetcherTask->Cancel(TRUE);
            mLastFetcherTask = NULL;
        }
        return NOERROR;
    }

    private void UpdateInterestedApps()
    {
        if (mLastFetcherTask != NULL) {
            // Canceling can only fail for some obscure reason since mLastFetcherTask would be
            // NULL if the task has already completed. So we ignore the result of cancel and
            // spawn a new task to get fresh data. AsyncTask executes tasks serially anyways,
            // so we are safe from running two tasks at the same time.
            mLastFetcherTask->Cancel(TRUE);
        }

        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        mLastFetcherTask = new AppsRequestingAccessFetcher(IContext::Probe(activity), this);
        mLastFetcherTask->Execute();
    }

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result)
    {
        VALIDATE_NOT_NULL(result)
        *result = FALSE;

        String packageName;
        preference->GetKey(&packageName);
        AutoPtr<IInterface> obj;
        mPackageEntryMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<PackageEntry> pe = (PackageEntry*)IObject::Probe(obj);

        if (pe == NULL) {
            Logger::W(TAG, "Preference change event for package %s but that package is no longer valid.",
                    packageName.string());
            return NOERROR;
        }

        if (IBoolean::Probe(newValue) == NULL) {
            AutoPtr<IClassInfo> thisKlass;
            CObject::ReflectClassInfo(newValue, (IClassInfo**)&thisKlass);
            String klassName, klassNamespace;
            thisKlass->GetName(&klassName);
            thisKlass->GetNamespace(&klassNamespace);
            String name = klassNamespace + "." + klassName;
            Logger::W(TAG, "Preference change event for package %s had non Boolean value of type %s",
                    packageName.string(), name.string());
            return NOERROR;
        }

        Boolean res;
        const Int32 newMode = (IBoolean::Probe(newValue)->GetValue(&res), res) ?
                IAppOpsManager::MODE_ALLOWED : IAppOpsManager::MODE_IGNORED;

        // Check if we need to do any work.
        if (pe->mAppOpMode != newMode) {
            if (newMode != IAppOpsManager::MODE_ALLOWED) {
                // Turning off the setting has no warning.
                SetNewMode(pe, newMode);
                *result = TRUE;
                return NOERROR;
            }

            // Turning on the setting has a Warning.
            AutoPtr<IFragmentManager> fm;
            GetChildFragmentManager((IFragmentManager**)&fm);
            AutoPtr<IFragmentTransaction> ft;
            fm->BeginTransaction((IFragmentTransaction**)&ft);
            AutoPtr<IFragment> prev;
            fm->FindFragmentByTag(String("warning"), (IFragment**)&prev);
            if (prev != NULL) {
                ft->Remove(prev);
            }
            WarningDialogFragment::NewInstance(pe->mPackageName)->Show(ft, String("warning"));
            return NOERROR;
        }
        *result = TRUE;
        return NOERROR;
    }

    void SetNewMode(
        /* [in] */ PackageEntry* pe,
        /* [in] */ Int32 newMode)
    {
        AutoPtr<IApplicationInfo> applicationInfo;
        pe->mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 uid;
        applicationInfo->GetUid(&uid);

        mAppOpsManager->SetMode(IAppOpsManager::OP_GET_USAGE_STATS, uid, pe->mPackageName, newMode);
        pe->mAppOpMode = newMode;
    }

    void AllowAccess(
        /* [in] */ const String& packageName)
    {
        AutoPtr<IInterface> obj;
        mPackageEntryMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<PackageEntry> entry = (PackageEntry*)IObject::Probe(obj);
        if (entry == NULL) {
            Logger::W(TAG, "Unable to give access to package %s: it does not exist.", packageName.string());
            return;
        }

        SetNewMode(entry, IAppOpsManager::MODE_ALLOWED);
        entry->mPreference->SetChecked(TRUE);
    }

public:
    // ArrayMap<String, PackageEntry> mPackageEntryMap = new ArrayMap<>();
    AutoPtr<IArrayMap> mPackageEntryMap;
    AutoPtr<IAppOpsManager> mAppOpsManager;
    AutoPtr<IPreferenceScreen> mPreferenceScreen;

private:
    static const String TAG = "CUsageAccessSettings";

    static const AutoPtr< ArrayOf<String> > PM_USAGE_STATS_PERMISSION = new String[] {
            Manifest::permission::PACKAGE_USAGE_STATS
    };

    static const AutoPtr< ArrayOf<Int32> > APP_OPS_OP_CODES = new Int32[] {
            IAppOpsManager::OP_GET_USAGE_STATS
    };

    AutoPtr<AppsRequestingAccessFetcher> mLastFetcherTask;

    AutoPtr<MyPackageMonitor> mPackageMonitor;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__