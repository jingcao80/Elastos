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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/CUsageAccessSettings.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IAppOpsManagerOpEntry;
using Elastos::Droid::App::IAppOpsManagerPackageOps;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::CSwitchPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Manifest;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CUsageAccessSettings::WarningDialogFragment
//===============================================================================

const String CUsageAccessSettings::WarningDialogFragment::ARG_PACKAGE_NAME("package");

CAR_INTERFACE_IMPL(CUsageAccessSettings::WarningDialogFragment, DialogFragment, IDialogInterfaceOnClickListener)

CUsageAccessSettings::WarningDialogFragment::WarningDialogFragment()
{}

CUsageAccessSettings::WarningDialogFragment::~WarningDialogFragment()
{}

ECode CUsageAccessSettings::WarningDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

AutoPtr<IDialogFragment> CUsageAccessSettings::WarningDialogFragment::NewInstance(
    /* [in] */ const String& packageName)
{
    AutoPtr<WarningDialogFragment> dialog = new WarningDialogFragment();
    dialog->constructor();
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(ARG_PACKAGE_NAME, packageName);
    dialog->SetArguments(args);
    return (IDialogFragment*)dialog;
}

ECode CUsageAccessSettings::WarningDialogFragment::OnCreateDialog(
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

ECode CUsageAccessSettings::WarningDialogFragment::OnClick(
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

//===============================================================================
//                  CUsageAccessSettings::PackageEntry
//===============================================================================

CUsageAccessSettings::PackageEntry::PackageEntry(
    /* [in] */ const String& packageName)
    : mPermissionGranted(FALSE)
    , mAppOpMode(0)
{
    mPackageName = packageName;
    mAppOpMode = IAppOpsManager::MODE_DEFAULT;
}

//===============================================================================
//                  CUsageAccessSettings::AppsRequestingAccessFetcher
//===============================================================================

CUsageAccessSettings::AppsRequestingAccessFetcher::AppsRequestingAccessFetcher(
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

CUsageAccessSettings::AppsRequestingAccessFetcher::~AppsRequestingAccessFetcher()
{}

ECode CUsageAccessSettings::AppsRequestingAccessFetcher::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result) //ArrayMap<String, PackageEntry>
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr< ArrayOf<String> > packages;
    // try {
    ECode ec = mIPackageManager->GetAppOpPermissionPackages(
            Manifest::permission::PACKAGE_USAGE_STATS, (ArrayOf<String>**)&packages);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W("CUsageAccessSettings", "PackageManager is dead. Can't get list of packages requesting %s",
                Manifest::permission::PACKAGE_USAGE_STATS.string());
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
        entries->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<PackageEntry> pe = (PackageEntry*)IObject::Probe(obj);
        if (pe->mPackageInfo == NULL) {
            ec = mPackageManager->GetPackageInfo(StringUtils::ToString(pe->mPermissionGranted),
                    0, (IPackageInfo**)&pe->mPackageInfo);
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
            Logger::W("CUsageAccessSettings", "AppOp permission exists for package %s but package doesn't exist or did not request UsageStats access", packageName.string());
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
            Logger::W("CUsageAccessSettings", "No AppOps permission exists for package %s", packageName.string());
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

ECode CUsageAccessSettings::AppsRequestingAccessFetcher::OnPostExecute(
    /* [in] */ IInterface* _newEntries)// ArrayMap<String, PackageEntry>
{
    mHost->mLastFetcherTask = NULL;

    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    if (activity == NULL) {
        // We must have finished the Activity while we were processing in the background.
        return NOERROR;
    }

    IPreferenceGroup* hostPreferenceScreen = IPreferenceGroup::Probe(mHost->mPreferenceScreen);

    if (_newEntries == NULL) {
        mHost->mPackageEntryMap->Clear();
        hostPreferenceScreen->RemoveAll();
        return NOERROR;
    }

    AutoPtr<IArrayMap> newEntries = IArrayMap::Probe(_newEntries);
    // Find the deleted entries and remove them from the PreferenceScreen.
    Boolean res;
    Int32 oldPackageCount;
    mHost->mPackageEntryMap->GetSize(&oldPackageCount);
    for (Int32 i = 0; i < oldPackageCount; i++) {
        AutoPtr<IInterface> obj;
        mHost->mPackageEntryMap->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<PackageEntry> oldPackageEntry = (PackageEntry*)IObject::Probe(obj);
        obj = NULL;
        newEntries->Get(CoreUtils::Convert(oldPackageEntry->mPermissionGranted), (IInterface**)&obj);
        AutoPtr<PackageEntry> newPackageEntry = (PackageEntry*)IObject::Probe(obj);
        if (newPackageEntry == NULL) {
            // This package has been removed.
            hostPreferenceScreen->RemovePreference(
                    IPreference::Probe(oldPackageEntry->mPreference), &res);
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
        newEntries->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<PackageEntry> packageEntry = (PackageEntry*)IObject::Probe(obj);
        if (packageEntry->mPreference == NULL) {
            CSwitchPreference::New(mContext, (ISwitchPreference**)&packageEntry->mPreference);
            IPreference* packageEntryPreference = IPreference::Probe(packageEntry->mPreference);
            packageEntryPreference->SetPersistent(FALSE);
            packageEntryPreference->SetOnPreferenceChangeListener(mHost);
            hostPreferenceScreen->AddPreference(packageEntryPreference, &res);
        }
        UpdatePreference(packageEntry);
    }

    mHost->mPackageEntryMap->Clear();
    mHost->mPackageEntryMap = newEntries;
    return NOERROR;
}

void CUsageAccessSettings::AppsRequestingAccessFetcher::UpdatePreference(
    /* [in] */ PackageEntry* pe)
{
    AutoPtr<IApplicationInfo> applicationInfo;
    pe->mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    IPackageItemInfo* applicationInfoPii = IPackageItemInfo::Probe(applicationInfo);
    AutoPtr<IDrawable> icon;
    applicationInfoPii->LoadIcon(mPackageManager, (IDrawable**)&icon);
    IPreference* pePreference = IPreference::Probe(pe->mPreference);
    pePreference->SetIcon(icon);
    AutoPtr<ICharSequence> cs;
    applicationInfoPii->LoadLabel(mPackageManager, (ICharSequence**)&cs);
    pePreference->SetTitle(cs);
    pePreference->SetKey(StringUtils::ToString(pe->mPermissionGranted));

    Boolean check = FALSE;
    if (pe->mAppOpMode == IAppOpsManager::MODE_ALLOWED) {
        check = TRUE;
    }
    else if (pe->mAppOpMode == IAppOpsManager::MODE_DEFAULT) {
        // If the default AppOps mode is set, then fall back to
        // whether the app has been granted permission by PackageManager.
        check = pe->mPermissionGranted;
    }

    ITwoStatePreference* tspPreference = ITwoStatePreference::Probe(pe->mPreference);
    Boolean res;
    if (check != (tspPreference->IsChecked(&res), res)) {
        tspPreference->SetChecked(check);
    }
}

//===============================================================================
//                  CUsageAccessSettings::MyPackageMonitor
//===============================================================================

CUsageAccessSettings::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CUsageAccessSettings* host)
    : mHost(host)
{}

ECode CUsageAccessSettings::MyPackageMonitor::constructor()
{
    return PackageMonitor::constructor();
}

CUsageAccessSettings::MyPackageMonitor::~MyPackageMonitor()
{}

ECode CUsageAccessSettings::MyPackageMonitor::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mHost->UpdateInterestedApps();
    return NOERROR;
}

ECode CUsageAccessSettings::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    mHost->UpdateInterestedApps();
    return NOERROR;
}

//===============================================================================
//                  CUsageAccessSettings
//===============================================================================

const String CUsageAccessSettings::TAG("CUsageAccessSettings");

static const AutoPtr< ArrayOf<String> > InitPM_USAGE_STATS_PERMISSION()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = Manifest::permission::PACKAGE_USAGE_STATS;

    return args;
}

const AutoPtr< ArrayOf<String> > CUsageAccessSettings::PM_USAGE_STATS_PERMISSION = InitPM_USAGE_STATS_PERMISSION();

static const AutoPtr< ArrayOf<Int32> > InitAPP_OPS_OP_CODES()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
    (*args)[0] = IAppOpsManager::OP_GET_USAGE_STATS;

    return args;
}

const AutoPtr< ArrayOf<Int32> > CUsageAccessSettings::APP_OPS_OP_CODES = InitAPP_OPS_OP_CODES();

CAR_INTERFACE_IMPL(CUsageAccessSettings, SettingsPreferenceFragment, IPreferenceOnPreferenceChangeListener)

CAR_OBJECT_IMPL(CUsageAccessSettings)

CUsageAccessSettings::CUsageAccessSettings()
{}

CUsageAccessSettings::~CUsageAccessSettings()
{}

ECode CUsageAccessSettings::constructor()
{
    CArrayMap::New((IArrayMap**)&mPackageEntryMap);
    mPackageMonitor = new MyPackageMonitor(this);
    mPackageMonitor->constructor();

    return SettingsPreferenceFragment::constructor();
}

Boolean CUsageAccessSettings::ShouldIgnorePackage(
    /* [in] */ const String& packageName)
{
    return packageName.Equals("android") || packageName.Equals("Elastos.Droid.Settings");
}

ECode CUsageAccessSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::usage_access_settings);
    GetPreferenceScreen((IPreferenceScreen**)&mPreferenceScreen);
    IPreferenceGroup::Probe(mPreferenceScreen)->SetOrderingAsAdded(FALSE);
    AutoPtr<IInterface> obj = GetSystemService(IContext::APP_OPS_SERVICE);
    mAppOpsManager = IAppOpsManager::Probe(obj);
    return NOERROR;
}

ECode CUsageAccessSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    UpdateInterestedApps();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mPackageMonitor->Register(IContext::Probe(activity), Looper::GetMainLooper(), FALSE);
    return NOERROR;
}

ECode CUsageAccessSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();

    mPackageMonitor->Unregister();
    if (mLastFetcherTask != NULL) {
        mLastFetcherTask->Cancel(TRUE);
        mLastFetcherTask = NULL;
    }
    return NOERROR;
}

void CUsageAccessSettings::UpdateInterestedApps()
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
    AutoPtr< ArrayOf<IInterface*> > args;
    mLastFetcherTask->Execute(args);
}

ECode CUsageAccessSettings::OnPreferenceChange(
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
        Int32 data;
        WarningDialogFragment::NewInstance(pe->mPackageName)->Show(ft, String("warning"), &data);
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

void CUsageAccessSettings::SetNewMode(
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

void CUsageAccessSettings::AllowAccess(
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
    ITwoStatePreference::Probe(entry->mPreference)->SetChecked(TRUE);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos