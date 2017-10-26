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

#ifndef __ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Settings_CUsageAccessSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Utility::IArrayMap;

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

        static CARAPI_(AutoPtr<IDialogFragment>) NewInstance(
            /* [in] */ const String& packageName);

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** dialog);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        static const String ARG_PACKAGE_NAME;
    };

private:
    class PackageEntry
        : public Object
    {
    public:
        TO_STRING_IMPL("CUsageAccessSettings::PackageEntry")

        PackageEntry(
            /* [in] */ const String& packageName);

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
            /* [in] */ CUsageAccessSettings* host);

        ~AppsRequestingAccessFetcher();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result); //ArrayMap<String, PackageEntry>

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* newEntries);// ArrayMap<String, PackageEntry>

    private:
        CARAPI_(void) UpdatePreference(
            /* [in] */ PackageEntry* pe);

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

        CARAPI constructor();

        ~MyPackageMonitor();

        //@Override
        CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        //@Override
        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

    private:
        CUsageAccessSettings* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUsageAccessSettings();

    ~CUsageAccessSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

protected:
    static CARAPI_(Boolean) ShouldIgnorePackage(
        /* [in] */ const String& packageName);

    CARAPI_(void) SetNewMode(
        /* [in] */ PackageEntry* pe,
        /* [in] */ Int32 newMode);

    CARAPI_(void) AllowAccess(
        /* [in] */ const String& packageName);

private:
    CARAPI_(void) UpdateInterestedApps();

public:
    // ArrayMap<String, PackageEntry> mPackageEntryMap = new ArrayMap<>();
    AutoPtr<IArrayMap> mPackageEntryMap;
    AutoPtr<IAppOpsManager> mAppOpsManager;
    AutoPtr<IPreferenceScreen> mPreferenceScreen;

private:
    static const String TAG;

    static const AutoPtr< ArrayOf<String> > PM_USAGE_STATS_PERMISSION;

    static const AutoPtr< ArrayOf<Int32> > APP_OPS_OP_CODES;

    AutoPtr<AppsRequestingAccessFetcher> mLastFetcherTask;

    AutoPtr<MyPackageMonitor> mPackageMonitor;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CUSAGEACCESSSETTINGS_H__