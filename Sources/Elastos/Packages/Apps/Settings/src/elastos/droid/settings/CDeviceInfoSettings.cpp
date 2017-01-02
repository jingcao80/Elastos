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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/CDeviceInfoSettings.h"
#include "elastos/droid/settings/CDevelopmentSettings.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SELinux.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Search::Index;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Utility::Arrays;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDeviceInfoSettings::MyBaseSearchIndexProvider
//===============================================================================

CDeviceInfoSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CDeviceInfoSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CDeviceInfoSettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::device_info_settings);
    AutoPtr< ArrayOf<ISearchIndexableResource*> > args = ArrayOf<ISearchIndexableResource*>::Alloc(1);
    args->Set(0, sir);

    return Arrays::AsList(args, result);
}

ECode CDeviceInfoSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IList> keys;
    CArrayList::New((IList**)&keys);
    if (IsPropertyMissing(PROPERTY_SELINUX_STATUS)) {
        keys->Add(CoreUtils::Convert(KEY_SELINUX_STATUS));
    }
    if (IsPropertyMissing(PROPERTY_URL_SAFETYLEGAL)) {
        keys->Add(CoreUtils::Convert(KEY_SAFETY_LEGAL));
    }
    if (IsPropertyMissing(PROPERTY_EQUIPMENT_ID)) {
        keys->Add(CoreUtils::Convert(KEY_EQUIPMENT_ID));
    }
    // Remove Baseband version if wifi-only device
    if (Utils::IsWifiOnly(context)) {
        keys->Add(CoreUtils::Convert(KEY_BASEBAND_VERSION));
    }
    // Dont show feedback option if there is no reporter.
    if (TextUtils::IsEmpty(GetFeedbackReporterPackage(context))) {
        keys->Add(CoreUtils::Convert(KEY_DEVICE_FEEDBACK));
    }
    if (!CheckIntentAction(context, String("android.settings.TERMS"))) {
        keys->Add(CoreUtils::Convert(KEY_TERMS));
    }
    if (!CheckIntentAction(context, String("android.settings.LICENSE"))) {
        keys->Add(CoreUtils::Convert(KEY_LICENSE));
    }
    if (!CheckIntentAction(context, String("android.settings.COPYRIGHT"))) {
        keys->Add(CoreUtils::Convert(KEY_COPYRIGHT));
    }
    if (UserHandle::GetMyUserId() != IUserHandle::USER_OWNER) {
        keys->Add(CoreUtils::Convert(KEY_SYSTEM_UPDATE_SETTINGS));
    }

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R::bool_::config_additional_system_update_setting_enable, &res);
    if (!res) {
        keys->Add(CoreUtils::Convert(KEY_UPDATE_SETTING));
    }

    *result = keys;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean CDeviceInfoSettings::MyBaseSearchIndexProvider::IsPropertyMissing(
    /* [in] */ const String& property)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(property, &str);
    return str.Equals("");
}

Boolean CDeviceInfoSettings::MyBaseSearchIndexProvider::CheckIntentAction(
    /* [in] */ IContext* context,
    /* [in] */ const String& action)
{
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);

    // Find the activity that is in the system image
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> list; // List<ResolveInfo>
    pm->QueryIntentActivities(intent, 0, (IList**)&list);
    Int32 listSize;
    list->GetSize(&listSize);

    for (Int32 i = 0; i < listSize; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 flags;
        applicationInfo->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
            return TRUE;
        }
    }

    return FALSE;
}

//===============================================================================
//                  CDeviceInfoSettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CDeviceInfoSettings::SEARCH_INDEX_DATA_PROVIDER;

const Int32 CDeviceInfoSettings::TAPS_TO_BE_A_DEVELOPER = 7;

const String CDeviceInfoSettings::TAG("CDeviceInfoSettings");
const String CDeviceInfoSettings::FILENAME_PROC_VERSION("/proc/version");
const String CDeviceInfoSettings::FILENAME_MSV("/sys/board_properties/soc/msv");

const String CDeviceInfoSettings::KEY_CONTAINER("container");
const String CDeviceInfoSettings::KEY_REGULATORY_INFO("regulatory_info");
const String CDeviceInfoSettings::KEY_TERMS("terms");
const String CDeviceInfoSettings::KEY_LICENSE("license");
const String CDeviceInfoSettings::KEY_COPYRIGHT("copyright");
const String CDeviceInfoSettings::KEY_SYSTEM_UPDATE_SETTINGS("system_update_settings");
const String CDeviceInfoSettings::PROPERTY_URL_SAFETYLEGAL("ro.url.safetylegal");
const String CDeviceInfoSettings::PROPERTY_SELINUX_STATUS("ro.build.selinux");
const String CDeviceInfoSettings::KEY_KERNEL_VERSION("kernel_version");
const String CDeviceInfoSettings::KEY_BUILD_NUMBER("build_number");
const String CDeviceInfoSettings::KEY_DEVICE_MODEL("device_model");
const String CDeviceInfoSettings::KEY_SELINUX_STATUS("selinux_status");
const String CDeviceInfoSettings::KEY_BASEBAND_VERSION("baseband_version");
const String CDeviceInfoSettings::KEY_FIRMWARE_VERSION("firmware_version");
const String CDeviceInfoSettings::KEY_UPDATE_SETTING("additional_system_update_settings");
const String CDeviceInfoSettings::KEY_EQUIPMENT_ID("fcc_equipment_id");
const String CDeviceInfoSettings::PROPERTY_EQUIPMENT_ID("ro.ril.fccid");
const String CDeviceInfoSettings::KEY_DEVICE_FEEDBACK("device_feedback");
const String CDeviceInfoSettings::KEY_SAFETY_LEGAL("safetylegal");

AutoPtr<IIndexableSearchIndexProvider> CDeviceInfoSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CDeviceInfoSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CDeviceInfoSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CDeviceInfoSettings)

CDeviceInfoSettings::CDeviceInfoSettings()
    : mDevHitCountdown(0)
{}

CDeviceInfoSettings::~CDeviceInfoSettings()
{}

ECode CDeviceInfoSettings::constructor()
{
    mHits = ArrayOf<Int64>::Alloc(3);

    return SettingsPreferenceFragment::constructor();
}

ECode CDeviceInfoSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::device_info_settings);

    SetStringSummary(KEY_FIRMWARE_VERSION, Build::VERSION::RELEASE);
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(KEY_FIRMWARE_VERSION), (IPreference**)&pref);
    pref->SetEnabled(TRUE);
    SetValueSummary(KEY_BASEBAND_VERSION, String("gsm.version.baseband"));
    Logger::I("CDeviceInfoSettings::OnCreate", "================ GetMsvSuffix() is TODO ================");
    // SetStringSummary(KEY_DEVICE_MODEL, Build::MODEL + GetMsvSuffix());
    SetValueSummary(KEY_EQUIPMENT_ID, PROPERTY_EQUIPMENT_ID);
    SetStringSummary(KEY_DEVICE_MODEL, Build::MODEL);
    SetStringSummary(KEY_BUILD_NUMBER, Build::DISPLAY);
    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_BUILD_NUMBER), (IPreference**)&pref);
    pref->SetEnabled(TRUE);
    FindPreference(CoreUtils::Convert(KEY_KERNEL_VERSION), (IPreference**)&pref);
    pref->SetSummary(CoreUtils::Convert(GetFormattedKernelVersion()));

    if (!SELinux::IsSELinuxEnabled()) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String status;
        resources->GetString(R::string::selinux_status_disabled, &status);
        SetStringSummary(KEY_SELINUX_STATUS, status);
    }
    else if (!SELinux::IsSELinuxEnforced()) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String status;
        resources->GetString(R::string::selinux_status_permissive, &status);
        SetStringSummary(KEY_SELINUX_STATUS, status);
    }

    // Remove selinux information if property is not present
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreferenceGroup* _screen = IPreferenceGroup::Probe(screen);
    RemovePreferenceIfPropertyMissing(_screen, KEY_SELINUX_STATUS,
            PROPERTY_SELINUX_STATUS);

    // Remove Safety information preference if PROPERTY_URL_SAFETYLEGAL is not set
    RemovePreferenceIfPropertyMissing(_screen, KEY_SAFETY_LEGAL,
            PROPERTY_URL_SAFETYLEGAL);

    // Remove Equipment id preference if FCC ID is not set by RIL
    RemovePreferenceIfPropertyMissing(_screen, KEY_EQUIPMENT_ID,
            PROPERTY_EQUIPMENT_ID);

    // Remove Baseband version if wifi-only device
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* _activity = IContext::Probe(activity);
    Boolean res;
    if (Utils::IsWifiOnly(_activity)) {
        pref = NULL;
        FindPreference(CoreUtils::Convert(KEY_BASEBAND_VERSION), (IPreference**)&pref);
        _screen->RemovePreference(pref, &res);
    }

    // Dont show feedback option if there is no reporter.
    if (TextUtils::IsEmpty(GetFeedbackReporterPackage(_activity))) {
        pref = NULL;
        FindPreference(CoreUtils::Convert(KEY_DEVICE_FEEDBACK), (IPreference**)&pref);
        _screen->RemovePreference(pref, &res);
    }

    /*
     * Settings is a generic app and should not contain any device-specific
     * info.
     */
    // final Activity act = GetActivity();
    // These are contained in the "container" preference group
    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_CONTAINER), (IPreference**)&pref);
    AutoPtr<IPreferenceGroup> parentPreference = IPreferenceGroup::Probe(pref);
    Utils::UpdatePreferenceToSpecificActivityOrRemove(_activity, parentPreference, KEY_TERMS,
            Utils::UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
    Utils::UpdatePreferenceToSpecificActivityOrRemove(_activity, parentPreference, KEY_LICENSE,
            Utils::UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
    Utils::UpdatePreferenceToSpecificActivityOrRemove(_activity, parentPreference, KEY_COPYRIGHT,
            Utils::UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);

    // These are contained by the root preference screen
    parentPreference = IPreferenceGroup::Probe(screen);
    if (UserHandle::GetMyUserId() == IUserHandle::USER_OWNER) {
        Utils::UpdatePreferenceToSpecificActivityOrRemove(_activity, parentPreference,
                KEY_SYSTEM_UPDATE_SETTINGS,
                Utils::UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
    }
    else {
        // Remove for secondary users
        RemovePreference(KEY_SYSTEM_UPDATE_SETTINGS);
    }

    // Read platform settings for additional system update setting
    RemovePreferenceIfBoolFalse(KEY_UPDATE_SETTING,
            R::bool_::config_additional_system_update_setting_enable);

    // Remove regulatory information if none present.
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_SHOW_REGULATORY_INFO, (IIntent**)&intent);
    AutoPtr<IList> list;
    GetPackageManager()->QueryIntentActivities(intent, 0, (IList**)&list);
    if (list->IsEmpty(&res), res) {
        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert(KEY_REGULATORY_INFO), (IPreference**)&pref);
        if (pref != NULL) {
            _screen->RemovePreference(pref, &res);
        }
    }
    return NOERROR;
}

ECode CDeviceInfoSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ISharedPreferences> sp;
    IContext::Probe(activity)->GetSharedPreferences(CDevelopmentSettings::PREF_FILE, IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    Boolean res;
    sp->GetBoolean(CDevelopmentSettings::PREF_SHOW, Build::TYPE.Equals("eng"), &res);
    mDevHitCountdown = res ? -1 : TAPS_TO_BE_A_DEVELOPER;
    mDevHitToast = NULL;
    return NOERROR;
}

ECode CDeviceInfoSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String key;
    preference->GetKey(&key);
    if (key.Equals(KEY_FIRMWARE_VERSION)) {
        mHits->Copy(0, mHits, 1, mHits->GetLength() -1);
        (*mHits)[mHits->GetLength()-1] = SystemClock::GetUptimeMillis();
        if ((*mHits)[0] >= (SystemClock::GetUptimeMillis()-500)) {
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
            intent->SetClassName(String("Elastos.Droid"),
                    String("Elastos.Droid.Internal.App.CPlatLogoActivity"));
            // try {
                ECode ec = StartActivity(intent);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Unable to start activity %s", TO_CSTR(intent));
                }
            // } catch (Exception e) {
            //     Logger::E(TAG, "Unable to start activity " + intent->ToString());
            // }
        }
    }
    else if (key.Equals(KEY_BUILD_NUMBER)) {
        // Don't enable developer options for secondary users.
        if (UserHandle::GetMyUserId() != IUserHandle::USER_OWNER) {
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<IUserManager> um = IUserManager::Probe(GetSystemService(IContext::USER_SERVICE));
        Boolean res;
        if (um->HasUserRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES, &res), res) {
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<IActivity> _activity;
        GetActivity((IActivity**)&_activity);
        IContext* activity = IContext::Probe(_activity);
        if (mDevHitCountdown > 0) {
            mDevHitCountdown--;
            if (mDevHitCountdown == 0) {
                AutoPtr<ISharedPreferences> sp;
                activity->GetSharedPreferences(CDevelopmentSettings::PREF_FILE,
                        IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
                AutoPtr<ISharedPreferencesEditor> editor;
                sp->Edit((ISharedPreferencesEditor**)&editor);
                editor->PutBoolean(CDevelopmentSettings::PREF_SHOW, TRUE);
                editor->Apply();
                if (mDevHitToast != NULL) {
                    mDevHitToast->Cancel();
                }
                AutoPtr<IToastHelper> helper;
                CToastHelper::AcquireSingleton((IToastHelper**)&helper);
                mDevHitToast = NULL;
                helper->MakeText(activity, R::string::show_dev_on,
                        IToast::LENGTH_LONG, (IToast**)&mDevHitToast);
                mDevHitToast->Show();
                // This is good time to index the Developer Options
                AutoPtr<IContext> ctx;
                activity->GetApplicationContext((IContext**)&ctx);
                Index::GetInstance(ctx)->UpdateFromClassNameResource(
                        String("Elastos.Droid.Settings.CDevelopmentSettings"), TRUE, TRUE);

            }
            else if (mDevHitCountdown > 0
                    && mDevHitCountdown < (TAPS_TO_BE_A_DEVELOPER-2)) {
                if (mDevHitToast != NULL) {
                    mDevHitToast->Cancel();
                }

                AutoPtr<IToastHelper> helper;
                CToastHelper::AcquireSingleton((IToastHelper**)&helper);
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(mDevHitCountdown));
                String str;
                resources->GetQuantityString(
                        R::plurals::show_dev_countdown, mDevHitCountdown, args, &str);
                mDevHitToast = NULL;
                helper->MakeText(activity, CoreUtils::Convert(str),
                        IToast::LENGTH_SHORT, (IToast**)&mDevHitToast);
                mDevHitToast->Show();
            }
        }
        else if (mDevHitCountdown < 0) {
            if (mDevHitToast != NULL) {
                mDevHitToast->Cancel();
            }

            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            mDevHitToast = NULL;
            helper->MakeText(activity, R::string::show_dev_already,
                    IToast::LENGTH_LONG, (IToast**)&mDevHitToast);
            mDevHitToast->Show();
        }
    }
    else if (key.Equals(KEY_DEVICE_FEEDBACK)) {
        SendFeedback();
    }
    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
}

void CDeviceInfoSettings::RemovePreferenceIfPropertyMissing(
    /* [in] */ IPreferenceGroup* preferenceGroup,
    /* [in] */ const String& preference,
    /* [in] */ const String& property )
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(property, &str);
    if (str.Equals("")) {
        // Property is missing so remove preference from group
        // try {
            AutoPtr<IPreference> pref;
            FindPreference(CoreUtils::Convert(preference), (IPreference**)&pref);
            Boolean res;
            preferenceGroup->RemovePreference(pref, &res);
        // } catch (RuntimeException e) {
        //     Logger::D(TAG, "Property '" + property + "' missing and no '"
        //             + preference + "' preference");
        // }
    }
}

void CDeviceInfoSettings::RemovePreferenceIfBoolFalse(
    /* [in] */ const String& preference,
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(resId, &res);
    if (!res) {
        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert(preference), (IPreference**)&pref);
        if (pref != NULL) {
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            Boolean res;
            IPreferenceGroup::Probe(screen)->RemovePreference(pref, &res);
        }
    }
}

void CDeviceInfoSettings::SetStringSummary(
    /* [in] */ const String& preference,
    /* [in] */ const String& value)
{
    // try {
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(preference), (IPreference**)&pref);
    ECode ec = pref->SetSummary(CoreUtils::Convert(value));
    if (FAILED(ec)) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String str;
        resources->GetString(R::string::device_info_default, &str);
        pref->SetSummary(CoreUtils::Convert(str));
    }
    // } catch (RuntimeException e) {
    //     FindPreference(CoreUtils::Convert(preference)).SetSummary(
    //         GetResources()->GetString(R::string::device_info_default));
    // }
}

void CDeviceInfoSettings::SetValueSummary(
    /* [in] */ const String& preference,
    /* [in] */ const String& property)
{
    // try {
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(preference), (IPreference**)&pref);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String did;
    resources->GetString(R::string::device_info_default, &did);
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(property, did, &str);
    pref->SetSummary(CoreUtils::Convert(str));
    // } catch (RuntimeException e) {
    //     // No recovery
    // }
}

void CDeviceInfoSettings::SendFeedback()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    String reporterPackage = GetFeedbackReporterPackage(IContext::Probe(activity));
    if (TextUtils::IsEmpty(reporterPackage)) {
        return;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_BUG_REPORT, (IIntent**)&intent);
    intent->SetPackage(reporterPackage);
    StartActivityForResult(intent, 0);
}

ECode CDeviceInfoSettings::ReadLine(
    /* [in] */ const String& filename,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IReader> fr;
    CFileReader::New(filename, (IReader**)&fr);
    AutoPtr<IBufferedReader> reader;
    CBufferedReader::New(fr, 256, (IBufferedReader**)&reader);
    // try {
    String str;
    ECode ec = reader->ReadLine(&str);
    if (SUCCEEDED(ec)) {
        *result = str;
        ICloseable::Probe(reader)->Close();
        return NOERROR;
    }
    ICloseable::Probe(reader)->Close();
    return ec;
    // } finally {
    //     reader->Close();
    // }
}

String CDeviceInfoSettings::GetFormattedKernelVersion()
{
    // try {
    String str;
    ECode ec = ReadLine(FILENAME_PROC_VERSION, &str);
    if (SUCCEEDED(ec)) {
        return FormatKernelVersion(str);
    }
    else { //IOException
        Logger::E(TAG, "IO Exception when getting kernel version for Device Info screen 0x%08x", ec);

        return String("Unavailable");
    }
}

String CDeviceInfoSettings::FormatKernelVersion(
    /* [in] */ const String& rawKernelVersion)
{
    // Example (see tests for more):
    // Linux version 3.0.31-g6fb96c9 (android-build@xxx.xxx.xxx.xxx.com)
    //     (gcc version 4.6.x-xxx 20120106 (prerelease) (GCC) ) #1 SMP PREEMPT
    //     Thu Jun 28 11:02:39 PDT 2012

    StringBuilder builder;
    builder += "Linux version (\\S+) "; /* group 1: "3.0.31-g6fb96c9" */
    builder += "\\((\\S+?)\\) ";        /* group 2: "x@y.com" (kernel builder) */
    builder += "(?:\\(gcc.+? \\)) ";    /* ignore: GCC version information */
    builder += "(#\\d+) ";              /* group 3: "#1" */
    builder += "(?:.*?)?";              /* ignore: optional SMP, PREEMPT, and any CONFIG_FLAGS */
    builder += "((Sun|Mon|Tue|Wed|Thu|Fri|Sat).+)"; /* group 4: "Thu Jun 28 11:02:39 PDT 2012" */

    String PROC_VERSION_REGEX = builder.ToString();

    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(PROC_VERSION_REGEX, (IPattern**)&pattern);
    AutoPtr<IMatcher> m;
    pattern->Matcher(rawKernelVersion, (IMatcher**)&m);
    Boolean res;
    Int32 count;
    if (m->Matches(&res), !res) {
        Logger::E(TAG, "Regex did not match on /proc/version: %s", rawKernelVersion.string());
        return String("Unavailable");
    }
    else if (IMatchResult::Probe(m)->GroupCount(&count), count < 4) {
        Logger::E(TAG, "Regex match on /proc/version only returned %d groups", count);
        return String("Unavailable");
    }

    builder.Reset();
    IMatchResult* mr = IMatchResult::Probe(m);
    String str;
    mr->Group(1, &str);
    builder += str; builder += "\n"; // 3.0.31-g6fb96c9
    mr->Group(2, &str);
    builder += str; builder += " ";
    mr->Group(3, &str);
    builder += str; builder += "\n"; // x@y.com #1
    mr->Group(4, &str);
    builder += str;  // Thu Jun 28 11:02:39 PDT 2012

    return builder.ToString();
}

String CDeviceInfoSettings::GetMsvSuffix()
{
    // Production devices should have a non-zero value. If we can't read it, assume it's a
    // production device so that we don't accidentally show that it's an ENGINEERING device.

    // try {
    String msv;
    ECode ec = ReadLine(FILENAME_MSV, &msv);
    if (SUCCEEDED(ec)) {
        // Parse as a hex number. If it evaluates to a zero, then it's an engineering build.
        Int64 data;
        ec = StringUtils::Parse(msv, 16, &data);
        if (ec == (ECode) E_NUMBER_FORMAT_EXCEPTION) {
            // Fail quietly, returning empty string should be sufficient
            return String("");
        }
        else {
            if (data == 0) {
                return String(" (ENGINEERING)");
            }
        }
    }
    // } catch (IOException ioe) {
        // Fail quietly, as the file may not exist on some devices.
    // } catch (NumberFormatException nfe) {
        // Fail quietly, returning empty string should be sufficient
    // }
    return String("");
}

String CDeviceInfoSettings::GetFeedbackReporterPackage(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String feedbackReporter;
    resources->GetString(R::string::oem_preferred_feedback_reporter, &feedbackReporter);
    if (TextUtils::IsEmpty(feedbackReporter)) {
        // Reporter not configured. Return.
        return feedbackReporter;
    }
    // Additional checks to ensure the reporter is on system image, and reporter is
    // configured to listen to the intent. Otherwise, dont show the "send feedback" option.
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_BUG_REPORT, (IIntent**)&intent);

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> resolvedPackages;// List<ResolveInfo>
    pm->QueryIntentActivities(intent, IPackageManager::GET_RESOLVED_FILTER, (IList**)&resolvedPackages);

    Int32 size;
    resolvedPackages->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        resolvedPackages->Get(i, (IInterface**)&obj);
        IResolveInfo* info = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (activityInfo != NULL) {
            String packageName;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
            if (!TextUtils::IsEmpty(packageName)) {
                // try {
                AutoPtr<IApplicationInfo> ai;
                ECode ec = pm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&ai);
                if (SUCCEEDED(ec)) {
                    Int32 flags;
                    ai->GetFlags(&flags);
                    if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                        // Package is on the system image
                        if (TextUtils::Equals(packageName, feedbackReporter)) {
                            return feedbackReporter;
                        }
                    }
                }
                else {//PackageManager.NameNotFoundException
                    // No need to do anything here.
                }
            }
        }
    }
    return String(NULL);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos