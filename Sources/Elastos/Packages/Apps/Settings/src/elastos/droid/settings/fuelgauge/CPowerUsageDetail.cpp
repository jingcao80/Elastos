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

#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/settings/fuelgauge/CPowerUsageDetail.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Settings::Applications::CInstalledAppDetails;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IApplicationErrorReport;
using Elastos::Droid::App::CApplicationErrorReport;
using Elastos::Droid::App::IApplicationErrorReportBatteryInfo;
using Elastos::Droid::App::CApplicationErrorReportBatteryInfo;
using Elastos::Droid::App::IApplicationErrorReportHelper;
using Elastos::Droid::App::CApplicationErrorReportHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Os::IPowerProfile;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_UNACCOUNTED;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_WIFI;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_CELL;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_SCREEN;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_APP;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_USER;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_BLUETOOTH;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_OVERCOUNTED;
using Elastos::Droid::Internal::Utility::CFastPrintWriter;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::IPrintWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

//===============================================================================
//                  CPowerUsageDetail::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CPowerUsageDetail::InnerListener, Object, IViewOnClickListener)

CPowerUsageDetail::InnerListener::InnerListener(
    /* [in] */ CPowerUsageDetail* host)
    : mHost(host)
{}

ECode CPowerUsageDetail::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  CPowerUsageDetail::MyBroadcastReceiver
//===============================================================================

CPowerUsageDetail::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CPowerUsageDetail* host)
    : mHost(host)
{}

ECode CPowerUsageDetail::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 resultCode;
    GetResultCode(&resultCode);
    return IView::Probe(mHost->mForceStopButton)->SetEnabled(resultCode != IActivity::RESULT_CANCELED);
}

//===============================================================================
//                  CPowerUsageDetail
//===============================================================================

const Int32 CPowerUsageDetail::ACTION_DISPLAY_SETTINGS = 1;
const Int32 CPowerUsageDetail::ACTION_WIFI_SETTINGS = 2;
const Int32 CPowerUsageDetail::ACTION_BLUETOOTH_SETTINGS = 3;
const Int32 CPowerUsageDetail::ACTION_WIRELESS_SETTINGS = 4;
const Int32 CPowerUsageDetail::ACTION_APP_DETAILS = 5;
const Int32 CPowerUsageDetail::ACTION_LOCATION_SETTINGS = 6;
const Int32 CPowerUsageDetail::ACTION_FORCE_STOP = 7;
const Int32 CPowerUsageDetail::ACTION_REPORT = 8;

const Int32 CPowerUsageDetail::USAGE_SINCE_UNPLUGGED = 1;
const Int32 CPowerUsageDetail::USAGE_SINCE_RESET = 2;

const String CPowerUsageDetail::EXTRA_TITLE("title");
const String CPowerUsageDetail::EXTRA_PERCENT("percent");
const String CPowerUsageDetail::EXTRA_GAUGE("gauge");
const String CPowerUsageDetail::EXTRA_UID("uid");
const String CPowerUsageDetail::EXTRA_USAGE_SINCE("since");
const String CPowerUsageDetail::EXTRA_USAGE_DURATION("duration");
const String CPowerUsageDetail::EXTRA_REPORT_DETAILS("report_details");
const String CPowerUsageDetail::EXTRA_REPORT_CHECKIN_DETAILS("report_checkin_details");
const String CPowerUsageDetail::EXTRA_DETAIL_TYPES("types");
const String CPowerUsageDetail::EXTRA_DETAIL_VALUES("values");
const String CPowerUsageDetail::EXTRA_DRAIN_TYPE("drainType");
const String CPowerUsageDetail::EXTRA_ICON_PACKAGE("iconPackage");
const String CPowerUsageDetail::EXTRA_NO_COVERAGE("noCoverage");
const String CPowerUsageDetail::EXTRA_ICON_ID("iconId");
const String CPowerUsageDetail::EXTRA_SHOW_LOCATION_BUTTON("showLocationButton");

static AutoPtr< ArrayOf<Int32> > InitsDrainTypeDesciptions()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(11);
    (*args)[0] = R::string::battery_desc_standby;
    (*args)[1] = R::string::battery_desc_radio;
    (*args)[2] = R::string::battery_desc_voice;
    (*args)[3] = R::string::battery_desc_wifi;
    (*args)[4] = R::string::battery_desc_bluetooth;
    (*args)[5] = R::string::battery_desc_flashlight;
    (*args)[6] = R::string::battery_desc_display;
    (*args)[7] = R::string::battery_desc_apps;
    (*args)[8] = R::string::battery_desc_users;
    (*args)[9] = R::string::battery_desc_unaccounted;
    (*args)[10] = R::string::battery_desc_overcounted;

    return args;
}

AutoPtr< ArrayOf<Int32> > CPowerUsageDetail::sDrainTypeDesciptions = InitsDrainTypeDesciptions();

const String CPowerUsageDetail::TAG("CPowerUsageDetail");

CAR_INTERFACE_IMPL(CPowerUsageDetail, Fragment, IViewOnClickListener)

CAR_OBJECT_IMPL(CPowerUsageDetail)

CPowerUsageDetail::CPowerUsageDetail()
    : mUsageSince(0)
    , mUid(0)
    , mStartTime(0)
    , mNoCoverage(0.0)
    , mUsesGps(FALSE)
    , mShowLocationButton(FALSE)
{}

CPowerUsageDetail::~CPowerUsageDetail()
{}

ECode CPowerUsageDetail::constructor()
{
    mCheckKillProcessesReceiver = new MyBroadcastReceiver(this);
    return Fragment::constructor();
}

void CPowerUsageDetail::StartBatteryDetailPage(
    /* [in] */ CSettingsActivity* caller,
    /* [in] */ IBatteryStatsHelper* helper,
    /* [in] */ Int32 statsType,
    /* [in] */ BatteryEntry* entry,
    /* [in] */ Boolean showLocationButton)
{
    // Initialize mStats if necessary.
    AutoPtr<IBatteryStats> batteryStats;
    helper->GetStats((IBatteryStats**)&batteryStats);

    Int32 dischargeAmount;
    batteryStats->GetDischargeAmount(statsType, &dischargeAmount);
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(CPowerUsageDetail::EXTRA_TITLE, entry->mName);

    Double value, noCoveragePercent;
    entry->mSipper->GetValue(&value);
    entry->mSipper->GetNoCoveragePercent(&noCoveragePercent);
    Double totalPower, maxPower;
    Int64 statsPeriod;
    helper->GetTotalPower(&totalPower);
    helper->GetMaxPower(&maxPower);
    helper->GetStatsPeriod(&statsPeriod);
    args->PutInt32(CPowerUsageDetail::EXTRA_PERCENT, (Int32)((value * dischargeAmount / totalPower) + .5));
    args->PutInt32(CPowerUsageDetail::EXTRA_GAUGE, (Int32)Elastos::Core::Math::Ceil(value * 100 / maxPower));
    args->PutInt64(CPowerUsageDetail::EXTRA_USAGE_DURATION, statsPeriod);
    args->PutString(CPowerUsageDetail::EXTRA_ICON_PACKAGE, entry->mDefaultPackageName);
    args->PutInt32(CPowerUsageDetail::EXTRA_ICON_ID, entry->mIconId);
    args->PutDouble(CPowerUsageDetail::EXTRA_NO_COVERAGE, noCoveragePercent);
    AutoPtr<IBatteryStatsUid> uidObj;
    entry->mSipper->GetUidObj((IBatteryStatsUid**)&uidObj);
    if (uidObj != NULL) {
        Int32 uid;
        uidObj->GetUid(&uid);
        args->PutInt32(CPowerUsageDetail::EXTRA_UID, uid);
    }
    BatterySipperDrainType drainType;
    entry->mSipper->GetDrainType(&drainType);
    args->PutInt32(CPowerUsageDetail::EXTRA_DRAIN_TYPE, drainType);
    args->PutBoolean(CPowerUsageDetail::EXTRA_SHOW_LOCATION_BUTTON, showLocationButton);

    Int32 userId = UserHandle::GetMyUserId();
    AutoPtr< ArrayOf<Int32> > types;
    AutoPtr< ArrayOf<Double> > values;
    switch (drainType) {
        case BatterySipperDrainType_APP:
        case BatterySipperDrainType_USER:
        {
            AutoPtr<IBatteryStatsUid> uid = uidObj;
            types = ArrayOf<Int32>::Alloc(12);
            (*types)[0] = R::string::usage_type_cpu;
            (*types)[1] = R::string::usage_type_cpu_foreground;
            (*types)[2] = R::string::usage_type_wake_lock;
            (*types)[3] = R::string::usage_type_gps;
            (*types)[4] = R::string::usage_type_wifi_running;
            (*types)[5] = R::string::usage_type_data_recv;
            (*types)[6] = R::string::usage_type_data_send;
            (*types)[7] = R::string::usage_type_radio_active;
            (*types)[8] = R::string::usage_type_data_wifi_recv;
            (*types)[9] = R::string::usage_type_data_wifi_send;
            (*types)[10] = R::string::usage_type_audio;
            (*types)[11] = R::string::usage_type_video;

            Int64 cpuTime, cpuFgTime, wakeLockTime, gpsTime, wifiRunningTime, mobileRxPackets, mobileTxPackets, mobileActive, wifiRxPackets, wifiTxPackets;
            entry->mSipper->GetCpuTime(&cpuTime);
            entry->mSipper->GetCpuFgTime(&cpuFgTime);
            entry->mSipper->GetWakeLockTime(&wakeLockTime);
            entry->mSipper->GetGpsTime(&gpsTime);
            entry->mSipper->GetWifiRunningTime(&wifiRunningTime);
            entry->mSipper->GetMobileRxPackets(&mobileRxPackets);
            entry->mSipper->GetMobileTxPackets(&mobileTxPackets);
            entry->mSipper->GetMobileActive(&mobileActive);
            entry->mSipper->GetWifiRxPackets(&wifiRxPackets);
            entry->mSipper->GetWifiTxPackets(&wifiTxPackets);

            values = ArrayOf<Double>::Alloc(12);
            (*values)[0] = cpuTime;
            (*values)[1] = cpuFgTime;
            (*values)[2] = wakeLockTime;
            (*values)[3] = gpsTime;
            (*values)[4] = wifiRunningTime;
            (*values)[5] = mobileRxPackets;
            (*values)[6] = mobileTxPackets;
            (*values)[7] = mobileActive;
            (*values)[8] = wifiRxPackets;
            (*values)[9] = wifiTxPackets;
            (*values)[10] = 0;
            (*values)[11] = 0;

            if (drainType == BatterySipperDrainType_APP) {
                AutoPtr<IWriter> result;
                CStringWriter::New((IWriter**)&result);
                AutoPtr<IPrintWriter> printWriter;
                CFastPrintWriter::New(result, FALSE, 1024, (IPrintWriter**)&printWriter);
                Int32 statsType, _uid;
                helper->GetStatsType(&statsType);
                uid->GetUid(&_uid);
                batteryStats->DumpLocked(caller, printWriter, String(""), statsType, _uid);
                IFlushable::Probe(printWriter)->Flush();
                args->PutString(CPowerUsageDetail::EXTRA_REPORT_DETAILS, Object::ToString(result));

                result = NULL;
                CStringWriter::New((IWriter**)&result);
                printWriter = NULL;
                CFastPrintWriter::New(result, FALSE, 1024, (IPrintWriter**)&printWriter);
                batteryStats->DumpCheckinLocked(caller, printWriter, statsType, _uid);
                IFlushable::Probe(printWriter)->Flush();
                args->PutString(CPowerUsageDetail::EXTRA_REPORT_CHECKIN_DETAILS, Object::ToString(result));
                userId = UserHandle::GetUserId(_uid);
            }
        }
            break;
        case BatterySipperDrainType_CELL:
        {
            types = ArrayOf<Int32>::Alloc(3);
            (*types)[0] = R::string::usage_type_on_time;
            (*types)[1] = R::string::usage_type_no_coverage;
            (*types)[2] = R::string::usage_type_radio_active;

            Int64 usageTime, mobileActive;
            Double noCoveragePercent;
            entry->mSipper->GetUsageTime(&usageTime);
            entry->mSipper->GetNoCoveragePercent(&noCoveragePercent);
            entry->mSipper->GetMobileActive(&mobileActive);

            values = ArrayOf<Double>::Alloc(3);
            (*values)[0] = usageTime;
            (*values)[1] = noCoveragePercent;
            (*values)[2] = mobileActive;
        }
            break;
        case BatterySipperDrainType_WIFI:
        {
            types = ArrayOf<Int32>::Alloc(8);
            (*types)[0] = R::string::usage_type_wifi_running;
            (*types)[1] = R::string::usage_type_cpu;
            (*types)[2] = R::string::usage_type_cpu_foreground;
            (*types)[3] = R::string::usage_type_wake_lock;
            (*types)[4] = R::string::usage_type_data_recv;
            (*types)[5] = R::string::usage_type_data_send;
            (*types)[6] = R::string::usage_type_data_wifi_recv;
            (*types)[7] = R::string::usage_type_data_wifi_send;

            Int64 usageTime, cpuTime, cpuFgTime, wakeLockTime, mobileRxPackets, mobileTxPackets, wifiRxPackets, wifiTxPackets;
            entry->mSipper->GetUsageTime(&usageTime);
            entry->mSipper->GetCpuTime(&cpuTime);
            entry->mSipper->GetCpuFgTime(&cpuFgTime);
            entry->mSipper->GetWakeLockTime(&wakeLockTime);
            entry->mSipper->GetMobileRxPackets(&mobileRxPackets);
            entry->mSipper->GetMobileTxPackets(&mobileTxPackets);
            entry->mSipper->GetWifiRxPackets(&wifiRxPackets);
            entry->mSipper->GetWifiTxPackets(&wifiTxPackets);

            values = ArrayOf<Double>::Alloc(8);
            (*values)[0] = usageTime;
            (*values)[1] = cpuTime;
            (*values)[2] = cpuFgTime;
            (*values)[3] = wakeLockTime;
            (*values)[4] = mobileRxPackets;
            (*values)[5] = mobileTxPackets;
            (*values)[6] = wifiRxPackets;
            (*values)[7] = wifiTxPackets;
        }
            break;
        case BatterySipperDrainType_BLUETOOTH:
        {
            types = ArrayOf<Int32>::Alloc(8);
            (*types)[0] = R::string::usage_type_on_time;
            (*types)[1] = R::string::usage_type_cpu;
            (*types)[2] = R::string::usage_type_cpu_foreground;
            (*types)[3] = R::string::usage_type_wake_lock;
            (*types)[4] = R::string::usage_type_data_recv;
            (*types)[5] = R::string::usage_type_data_send;
            (*types)[6] = R::string::usage_type_data_wifi_recv;
            (*types)[7] = R::string::usage_type_data_wifi_send;

            Int64 usageTime, cpuTime, cpuFgTime, wakeLockTime, mobileRxPackets, mobileTxPackets, wifiRxPackets, wifiTxPackets;
            entry->mSipper->GetUsageTime(&usageTime);
            entry->mSipper->GetCpuTime(&cpuTime);
            entry->mSipper->GetCpuFgTime(&cpuFgTime);
            entry->mSipper->GetWakeLockTime(&wakeLockTime);
            entry->mSipper->GetMobileRxPackets(&mobileRxPackets);
            entry->mSipper->GetMobileTxPackets(&mobileTxPackets);
            entry->mSipper->GetWifiRxPackets(&wifiRxPackets);
            entry->mSipper->GetWifiTxPackets(&wifiTxPackets);

            values = ArrayOf<Double>::Alloc(8);
            (*values)[0] = usageTime;
            (*values)[1] = cpuTime;
            (*values)[2] = cpuFgTime;
            (*values)[3] = wakeLockTime;
            (*values)[4] = mobileRxPackets;
            (*values)[5] = mobileTxPackets;
            (*values)[6] = wifiRxPackets;
            (*values)[7] = wifiTxPackets;
        }
            break;
        case BatterySipperDrainType_UNACCOUNTED:
        {
            types = ArrayOf<Int32>::Alloc(3);
            (*types)[0] = R::string::usage_type_total_battery_capacity;
            (*types)[1] = R::string::usage_type_computed_power;
            (*types)[2] = R::string::usage_type_actual_power;

            AutoPtr<IPowerProfile> pp;
            helper->GetPowerProfile((IPowerProfile**)&pp);
            Double capactity;
            pp->GetBatteryCapacity(&capactity);
            Double computedPower, minDrainedPower;
            helper->GetComputedPower(&computedPower);
            helper->GetMinDrainedPower(&minDrainedPower);

            values = ArrayOf<Double>::Alloc(3);
            (*values)[0] = capactity;
            (*values)[1] = computedPower;
            (*values)[2] = minDrainedPower;
        }
            break;
        case BatterySipperDrainType_OVERCOUNTED:
        {
            types = ArrayOf<Int32>::Alloc(3);
            (*types)[0] = R::string::usage_type_total_battery_capacity;
            (*types)[1] = R::string::usage_type_computed_power;
            (*types)[2] = R::string::usage_type_actual_power;

            AutoPtr<IPowerProfile> pp;
            helper->GetPowerProfile((IPowerProfile**)&pp);
            Double capactity;
            pp->GetBatteryCapacity(&capactity);
            Double computedPower, maxDrainedPower;
            helper->GetComputedPower(&computedPower);
            helper->GetMaxDrainedPower(&maxDrainedPower);
            values = ArrayOf<Double>::Alloc(3);
            (*values)[0] = capactity;
            (*values)[1] = computedPower;
            (*values)[2] = maxDrainedPower;
        }
            break;
        default:
        {
            types = ArrayOf<Int32>::Alloc(1);
            (*types)[0] = R::string::usage_type_on_time;

            Int64 usageTime;
            entry->mSipper->GetUsageTime(&usageTime);
            values = ArrayOf<Double>::Alloc(1);
            (*values)[0] = usageTime;
        }
    }

    args->PutInt32Array(CPowerUsageDetail::EXTRA_DETAIL_TYPES, types);
    args->PutDoubleArray(CPowerUsageDetail::EXTRA_DETAIL_VALUES, values);

    // This is a workaround, see b/17523189
    if (userId == UserHandle::GetMyUserId()) {
        caller->StartPreferencePanel(String("Elastos.Droid.Settings.Fuelgauge.CPowerUsageDetail"), args,
                R::string::details_title, NULL, NULL, 0);
    }
    else {
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        caller->StartPreferencePanelAsUser(String("Elastos.Droid.Settings.Fuelgauge.CPowerUsageDetail"), args,
                R::string::details_title, NULL, uh);
    }
}

ECode CPowerUsageDetail::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Fragment::OnCreate(icicle);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    activity->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDpm = IDevicePolicyManager::Probe(obj);
    return NOERROR;
}

ECode CPowerUsageDetail::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::power_usage_details, container, FALSE, (IView**)&view);
    Utils::PrepareCustomPreferencesList(container, view, view, FALSE);

    mRootView = view;
    CreateDetails();

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPowerUsageDetail::OnResume()
{
    Fragment::OnResume();
    mStartTime = Process::GetElapsedCpuTime();
    CheckForceStop();
    return NOERROR;
}

ECode CPowerUsageDetail::OnPause()
{
    return Fragment::OnPause();
}

void CPowerUsageDetail::CreateDetails()
{
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    args->GetString(EXTRA_TITLE, &mTitle);
    Int32 percentage;
    args->GetInt32(EXTRA_PERCENT, 1, &percentage);
    Int32 gaugeValue;
    args->GetInt32(EXTRA_GAUGE, 1, &gaugeValue);
    args->GetInt32(EXTRA_USAGE_SINCE, USAGE_SINCE_UNPLUGGED, &mUsageSince);
    args->GetInt32(EXTRA_UID, 0, &mUid);
    args->GetInt32(EXTRA_DRAIN_TYPE, &mDrainType);
    args->GetDouble(EXTRA_NO_COVERAGE, 0, &mNoCoverage);
    String iconPackage;
    args->GetString(EXTRA_ICON_PACKAGE, &iconPackage);
    Int32 iconId;
    args->GetInt32(EXTRA_ICON_ID, 0, &iconId);
    args->GetBoolean(EXTRA_SHOW_LOCATION_BUTTON, &mShowLocationButton);
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    if (!TextUtils::IsEmpty(iconPackage)) {
        // try {
            AutoPtr<IPackageManager> pm;
            activity->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IPackageInfo> info;
            pm->GetPackageInfo(iconPackage, 0, (IPackageInfo**)&info);
            AutoPtr<IApplicationInfo> ai;
            info->GetApplicationInfo((IApplicationInfo**)&ai);
            if (ai != NULL) {
                mAppIcon = NULL;
                IPackageItemInfo::Probe(ai)->LoadIcon(pm, (IDrawable**)&mAppIcon);
            }
        // } catch (NameNotFoundException nnfe) {
        //     // Use default icon
        // }
    }
    else if (iconId != 0) {
        AutoPtr<IResources> resources;
        activity->GetResources((IResources**)&resources);
        mAppIcon = NULL;
        resources->GetDrawable(iconId, (IDrawable**)&mAppIcon);
    }
    if (mAppIcon == NULL) {
        AutoPtr<IPackageManager> pm;
        activity->GetPackageManager((IPackageManager**)&pm);
        pm->GetDefaultActivityIcon((IDrawable**)&mAppIcon);
    }

    // Set the description
    AutoPtr<IView> summaryTmp;
    mRootView->FindViewById(Elastos::Droid::R::id::summary, (IView**)&summaryTmp);
    ITextView* summary = ITextView::Probe(summaryTmp);
    summary->SetText(CoreUtils::Convert(GetDescriptionForDrainType()));
    summaryTmp->SetVisibility(IView::VISIBLE);

    args->GetInt32Array(EXTRA_DETAIL_TYPES, (ArrayOf<Int32>**)&mTypes);
    args->GetDoubleArray(EXTRA_DETAIL_VALUES, (ArrayOf<Double>**)&mValues);

    AutoPtr<IView> tmp;
    mRootView->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    mTitleView = ITextView::Probe(tmp);
    mTitleView->SetText(CoreUtils::Convert(mTitle));

    tmp = NULL;
    mRootView->FindViewById(Elastos::Droid::R::id::text1, (IView**)&tmp);
    AutoPtr<ITextView> text1 = ITextView::Probe(tmp);
    text1->SetText(CoreUtils::Convert(Utils::FormatPercentage(percentage)));

    AutoPtr<IView> twoButtonsPanelTmp;
    mRootView->FindViewById(R::id::two_buttons_panel, (IView**)&twoButtonsPanelTmp);
    mTwoButtonsPanel = IViewGroup::Probe(twoButtonsPanelTmp);
    AutoPtr<IView> forceStopButtonTmp;
    mRootView->FindViewById(R::id::left_button, (IView**)&forceStopButtonTmp);
    mForceStopButton = IButton::Probe(forceStopButtonTmp);
    AutoPtr<IView> rportButtonTmp;
    mRootView->FindViewById(R::id::right_button, (IView**)&rportButtonTmp);
    mReportButton = IButton::Probe(rportButtonTmp);
    forceStopButtonTmp->SetEnabled(FALSE);

    tmp = NULL;
    mRootView->FindViewById(Elastos::Droid::R::id::progress, (IView**)&tmp);
    AutoPtr<IProgressBar> progress = IProgressBar::Probe(tmp);
    progress->SetProgress(gaugeValue);

    tmp = NULL;
    mRootView->FindViewById(Elastos::Droid::R::id::icon, (IView**)&tmp);
    AutoPtr<IImageView> icon = IImageView::Probe(tmp);
    icon->SetImageDrawable(mAppIcon);

    tmp = NULL;
    mRootView->FindViewById(R::id::details, (IView**)&tmp);
    mDetailsParent = IViewGroup::Probe(tmp);
    tmp = NULL;
    mRootView->FindViewById(R::id::controls, (IView**)&tmp);
    mControlsParent = IViewGroup::Probe(tmp);
    tmp = NULL;
    mRootView->FindViewById(R::id::messages, (IView**)&tmp);
    mMessagesParent = IViewGroup::Probe(tmp);

    FillDetailsSection();
    FillPackagesSection(mUid);
    FillControlsSection(mUid);
    FillMessagesSection(mUid);

    if (mUid >= IProcess::FIRST_APPLICATION_UID) {
        ITextView::Probe(mForceStopButton)->SetText(R::string::force_stop);
        forceStopButtonTmp->SetTag(CoreUtils::Convert(ACTION_FORCE_STOP));
        AutoPtr<InnerListener> listener = new InnerListener(this);
        forceStopButtonTmp->SetOnClickListener(listener);
        ITextView::Probe(mReportButton)->SetText(Elastos::Droid::R::string::report);
        rportButtonTmp->SetTag(CoreUtils::Convert(ACTION_REPORT));
        rportButtonTmp->SetOnClickListener(listener);

        // check if error reporting is enabled in secure settings
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        AutoPtr<IContentResolver> resolver;
        activity->GetContentResolver((IContentResolver**)&resolver);
        Int32 enabled;
        global->GetInt32(resolver, ISettingsGlobal::SEND_ACTION_APP_ERROR, 0, &enabled);
        if (enabled != 0) {
            if (mPackages != NULL && mPackages->GetLength() > 0) {
                // try {
                    AutoPtr<IPackageManager> pm;
                    activity->GetPackageManager((IPackageManager**)&pm);
                    pm->GetApplicationInfo((*mPackages)[0], 0, (IApplicationInfo**)&mApp);
                    Int32 flags;
                    mApp->GetFlags(&flags);
                    AutoPtr<IApplicationErrorReportHelper> helper;
                    CApplicationErrorReportHelper::AcquireSingleton((IApplicationErrorReportHelper**)&helper);
                    helper->GetErrorReportReceiver(activity, (*mPackages)[0], flags, (IComponentName**)&mInstaller);
                // } catch (NameNotFoundException e) {
                // }
            }
            rportButtonTmp->SetEnabled(mInstaller != NULL);
        }
        else {
            twoButtonsPanelTmp->SetVisibility(IView::GONE);
        }
    }
    else {
        twoButtonsPanelTmp->SetVisibility(IView::GONE);
    }
}

ECode CPowerUsageDetail::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    Int32 value;
    IInteger32::Probe(obj)->GetValue(&value);
    DoAction(value);
    return NOERROR;
}

void CPowerUsageDetail::StartApplicationDetailsActivity()
{
    // start new fragment to display extended information
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(CInstalledAppDetails::ARG_PACKAGE_NAME, (*mPackages)[0]);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
    sa->StartPreferencePanel(String("Elastos.Droid.Settings.Applications.CInstalledAppDetails"), args,
            R::string::application_info_label, NULL, NULL, 0);
}

void CPowerUsageDetail::DoAction(
    /* [in] */ Int32 action)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
    switch (action) {
        case ACTION_DISPLAY_SETTINGS:
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.CDisplaySettings"), NULL,
                    R::string::display_settings_title, NULL, NULL, 0);
            break;
        case ACTION_WIFI_SETTINGS:
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.Wifi.CWifiSettings"), NULL,
                    R::string::wifi_settings, NULL, NULL, 0);
            break;
        case ACTION_BLUETOOTH_SETTINGS:
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.Bluetooth.CBluetoothSettings"), NULL,
                    R::string::bluetooth_settings, NULL, NULL, 0);
            break;
        case ACTION_WIRELESS_SETTINGS:
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.CWirelessSettings"), NULL,
                    R::string::radio_controls_title, NULL, NULL, 0);
            break;
        case ACTION_APP_DETAILS:
            StartApplicationDetailsActivity();
            break;
        case ACTION_LOCATION_SETTINGS:
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.Location.CLocationSettings"), NULL,
                    R::string::location_settings_title, NULL, NULL, 0);
            break;
        case ACTION_FORCE_STOP:
            KillProcesses();
            break;
        case ACTION_REPORT:
            ReportBatteryUse();
            break;
    }
}

void CPowerUsageDetail::FillDetailsSection()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    if (mTypes != NULL && mValues != NULL) {
        for (Int32 i = 0; i < mTypes->GetLength(); i++) {
            // Only add an item if the time is greater than zero
            if ((*mValues)[i] <= 0) continue;
            String label;
            GetString((*mTypes)[i], &label);
            String value;
            switch ((*mTypes)[i]) {
                case R::string::usage_type_data_recv:
                case R::string::usage_type_data_send:
                case R::string::usage_type_data_wifi_recv:
                case R::string::usage_type_data_wifi_send: {
                    const Int64 packets = (Int64) ((*mValues)[i]);
                    value = StringUtils::ToString(packets);
                    break;
                }
                case R::string::usage_type_no_coverage: {
                    const Int32 percentage = (Int32) Elastos::Core::Math::Floor((*mValues)[i]);
                    value = Utils::FormatPercentage(percentage);
                    break;
                }
                case R::string::usage_type_total_battery_capacity:
                case R::string::usage_type_computed_power:
                case R::string::usage_type_actual_power: {
                    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                    args->Set(0, CoreUtils::Convert((Int64)((*mValues)[i])));
                    IContext::Probe(activity)->GetString(R::string::mah, args, &value);
                    break;
                }
                case R::string::usage_type_gps:
                    mUsesGps = TRUE;
                    // Fall through
                default:
                    value = Utils::FormatElapsedTime(IContext::Probe(activity), (*mValues)[i], TRUE);
            }
            AutoPtr<IView> itemTmp;
            inflater->Inflate(R::layout::power_usage_detail_item_text, NULL, (IView**)&itemTmp);
            // IViewGroup* item = IViewGroup::Probe(itemTmp);
            mDetailsParent->AddView(itemTmp);
            AutoPtr<IView> tmp;
            itemTmp->FindViewById(R::id::label, (IView**)&tmp);
            ITextView* labelView = ITextView::Probe(tmp);
            tmp = NULL;
            itemTmp->FindViewById(R::id::value, (IView**)&tmp);
            ITextView* valueView = ITextView::Probe(tmp);
            labelView->SetText(CoreUtils::Convert(label));
            valueView->SetText(CoreUtils::Convert(value));
        }
    }
}

void CPowerUsageDetail::FillControlsSection(
    /* [in] */ Int32 uid)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr< ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    AutoPtr<IPackageInfo> pi;
    // try {
        if (packages != NULL) {
            pm->GetPackageInfo((*packages)[0], 0, (IPackageInfo**)&pi);
        }
    // } catch (NameNotFoundException nnfe) { /* Nothing */ }
    AutoPtr<IApplicationInfo> ai;
    if (pi != NULL) {
        pi->GetApplicationInfo((IApplicationInfo**)&ai);
    }

    Boolean removeHeader = TRUE;
    switch (mDrainType) {
        case BatterySipperDrainType_APP:
            // If it is a Java application and only one package is associated with the Uid
            if (packages != NULL && packages->GetLength() == 1) {
                AddControl(R::string::battery_action_app_details,
                        R::string::battery_sugg_apps_info, ACTION_APP_DETAILS);
                removeHeader = FALSE;
                // If the application has a settings screen, jump to  that
                // TODO:
            }
            // If power usage detail page is launched from location page, suppress "Location"
            // button to prevent circular loops.
            if (mUsesGps && mShowLocationButton) {
                AddControl(R::string::location_settings_title,
                        R::string::battery_sugg_apps_gps, ACTION_LOCATION_SETTINGS);
                removeHeader = FALSE;
            }
            break;
        case BatterySipperDrainType_SCREEN:
            AddControl(R::string::display_settings,
                    R::string::battery_sugg_display, ACTION_DISPLAY_SETTINGS);
            removeHeader = FALSE;
            break;
        case BatterySipperDrainType_WIFI:
            AddControl(R::string::wifi_settings,
                    R::string::battery_sugg_wifi, ACTION_WIFI_SETTINGS);
            removeHeader = FALSE;
            break;
        case BatterySipperDrainType_BLUETOOTH:
            AddControl(R::string::bluetooth_settings,
                    R::string::battery_sugg_bluetooth_basic, ACTION_BLUETOOTH_SETTINGS);
            removeHeader = FALSE;
            break;
        case BatterySipperDrainType_CELL:
            if (mNoCoverage > 10) {
                AddControl(R::string::radio_controls_title,
                        R::string::battery_sugg_radio, ACTION_WIRELESS_SETTINGS);
                removeHeader = FALSE;
            }
            break;
    }
    if (removeHeader) {
        IView::Probe(mControlsParent)->SetVisibility(IView::GONE);
    }
}

void CPowerUsageDetail::AddControl(
    /* [in] */ Int32 title,
    /* [in] */ Int32 summary,
    /* [in] */ Int32 action)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> itemTmp;
    inflater->Inflate(R::layout::power_usage_action_item, NULL, (IView**)&itemTmp);
    // AutoPtr<IViewGroup> item = IViewGroup::Probe(itemTmp);
    mControlsParent->AddView(itemTmp);
    AutoPtr<IView> actionButtonTmp;
    itemTmp->FindViewById(R::id::action_button, (IView**)&actionButtonTmp);
    IButton* actionButton = IButton::Probe(actionButtonTmp);
    AutoPtr<IView> tmp;
    itemTmp->FindViewById(R::id::summary, (IView**)&tmp);
    ITextView* summaryView = ITextView::Probe(tmp);
    String str;
    res->GetString(title, &str);
    ITextView::Probe(actionButton)->SetText(CoreUtils::Convert(str));
    res->GetString(summary, &str);
    summaryView->SetText(CoreUtils::Convert(str));
    AutoPtr<InnerListener> listener = new InnerListener(this);
    actionButtonTmp->SetOnClickListener(listener);
    actionButtonTmp->SetTag(CoreUtils::Convert(action));
}

void CPowerUsageDetail::FillMessagesSection(
    /* [in] */ Int32 uid)
{
    Boolean removeHeader = TRUE;
    switch (mDrainType) {
        case BatterySipperDrainType_UNACCOUNTED:
            AddMessage(R::string::battery_msg_unaccounted);
            removeHeader = FALSE;
            break;
    }
    if (removeHeader) {
        IView::Probe(mMessagesParent)->SetVisibility(IView::GONE);
    }
}

void CPowerUsageDetail::AddMessage(
    /* [in] */ Int32 message)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> item;
    inflater->Inflate(R::layout::power_usage_message_item, NULL, (IView**)&item);
    mMessagesParent->AddView(item);
    AutoPtr<IView> tmp;
    item->FindViewById(R::id::message, (IView**)&tmp);
    ITextView* messageView = ITextView::Probe(tmp);
    AutoPtr<ICharSequence> cs;
    res->GetText(message, (ICharSequence**)&cs);
    messageView->SetText(cs);
}

void CPowerUsageDetail::RemovePackagesSection()
{
    AutoPtr<IView> view;
    mRootView->FindViewById(R::id::packages_section_title, (IView**)&view);
    if (view != NULL) {
        view->SetVisibility(IView::GONE);
    }
    view = NULL;
    mRootView->FindViewById(R::id::packages_section, (IView**)&view);
    if (view != NULL) {
        view->SetVisibility(IView::GONE);
    }
}

void CPowerUsageDetail::KillProcesses()
{
    if (mPackages == NULL) return;
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);
    Int32 userId = UserHandle::GetUserId(mUid);
    for (Int32 i = 0; i < mPackages->GetLength(); i++) {
        am->ForceStopPackageAsUser((*mPackages)[i], userId);
    }
    CheckForceStop();
}

void CPowerUsageDetail::CheckForceStop()
{
    IView* forceStopButton = IView::Probe(mForceStopButton);
    if (mPackages == NULL || mUid < IProcess::FIRST_APPLICATION_UID) {
        forceStopButton->SetEnabled(FALSE);
        return;
    }
    Boolean res;
    for (Int32 i = 0; i < mPackages->GetLength(); i++) {
        if (mDpm->PackageHasActiveAdmins((*mPackages)[i], &res), res) {
            forceStopButton->SetEnabled(FALSE);
            return;
        }
    }
    for (Int32 i = 0; i < mPackages->GetLength(); i++) {
        // try {
            AutoPtr<IApplicationInfo> info;
            mPm->GetApplicationInfo((*mPackages)[i], 0, (IApplicationInfo**)&info);
            Int32 flags;
            info->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_STOPPED) == 0) {
                forceStopButton->SetEnabled(TRUE);
                break;
            }
        // } catch (PackageManager.NameNotFoundException e) {
        // }
    }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->FromParts(String("package"), (*mPackages)[0], String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_QUERY_PACKAGE_RESTART, uri, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_PACKAGES, mPackages);
    intent->PutExtra(IIntent::EXTRA_UID, mUid);
    intent->PutExtra(IIntent::EXTRA_USER_HANDLE, UserHandle::GetUserId(mUid));
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->SendOrderedBroadcast(intent, String(NULL), mCheckKillProcessesReceiver, NULL,
            IActivity::RESULT_CANCELED, String(NULL), NULL);
}

void CPowerUsageDetail::ReportBatteryUse()
{
    if (mPackages == NULL) return;

    AutoPtr<IApplicationErrorReport> report;
    CApplicationErrorReport::New((IApplicationErrorReport**)&report);
    report->SetType(IApplicationErrorReport::TYPE_BATTERY);
    report->SetPackageName((*mPackages)[0]);
    String installerPackageName;
    mInstaller->GetPackageName(&installerPackageName);
    report->SetInstallerPackageName(installerPackageName);
    report->SetProcessName((*mPackages)[0]);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 time;
    sys->GetCurrentTimeMillis(&time);
    report->SetTime(time);
    Int32 flags;
    mApp->GetFlags(&flags);
    report->SetSystemApp((flags & IApplicationInfo::FLAG_SYSTEM) != 0);

    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    AutoPtr<IApplicationErrorReportBatteryInfo> batteryInfo;
    CApplicationErrorReportBatteryInfo::New((IApplicationErrorReportBatteryInfo**)&batteryInfo);
    Int32 usagePercent;
    args->GetInt32(EXTRA_PERCENT, 1, &usagePercent);
    batteryInfo->SetUsagePercent(usagePercent);
    Int64 durationMicros;
    args->GetInt64(EXTRA_USAGE_DURATION, 0, &durationMicros);
    batteryInfo->SetDurationMicros(durationMicros);
    String usageDetails, checkinDetails;
    args->GetString(EXTRA_REPORT_DETAILS, &usageDetails);
    batteryInfo->SetUsageDetails(usageDetails);
    args->GetString(EXTRA_REPORT_CHECKIN_DETAILS, &checkinDetails);
    batteryInfo->SetCheckinDetails(checkinDetails);
    report->SetBatteryInfo(batteryInfo);

    AutoPtr<IIntent> result;
    CIntent::New(IIntent::ACTION_APP_ERROR, (IIntent**)&result);
    result->SetComponent(mInstaller);
    result->PutExtra(IIntent::EXTRA_BUG_REPORT, IParcelable::Probe(report));
    result->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    StartActivity(result);
}

void CPowerUsageDetail::FillPackagesSection(
    /* [in] */ Int32 uid)
{
    if (uid < 1) {
        RemovePackagesSection();
        return;
    }
    AutoPtr<IView> tmp;
    mRootView->FindViewById(R::id::packages_section, (IView**)&tmp);
    IViewGroup* packagesParent = IViewGroup::Probe(tmp);
    if (packagesParent == NULL) return;
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);

    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    //final Drawable defaultActivityIcon = pm->GetDefaultActivityIcon();
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&mPackages);
    if (mPackages == NULL || mPackages->GetLength() < 2) {
        RemovePackagesSection();
        return;
    }

    // Convert package names to user-facing labels where possible
    for (Int32 i = 0; i < mPackages->GetLength(); i++) {
        // try {
            AutoPtr<IApplicationInfo> ai;
            pm->GetApplicationInfo((*mPackages)[i], 0, (IApplicationInfo**)&ai);
            AutoPtr<ICharSequence> label;
            IPackageItemInfo::Probe(ai)->LoadLabel(pm, (ICharSequence**)&label);
            //Drawable icon = defaultActivityIcon;
            if (label != NULL) {
                label->ToString(&(*mPackages)[i]);
            }
            //if (ai.icon != 0) {
            //    icon = ai->LoadIcon(pm);
            //}
            AutoPtr<IView> item;
            inflater->Inflate(R::layout::power_usage_package_item, NULL, (IView**)&item);
            packagesParent->AddView(item);
            AutoPtr<IView> tmp;
            item->FindViewById(R::id::label, (IView**)&tmp);
            ITextView* labelView = ITextView::Probe(tmp);
            labelView->SetText(CoreUtils::Convert((*mPackages)[i]));
        // } catch (NameNotFoundException e) {
        // }
    }
}

String CPowerUsageDetail::GetDescriptionForDrainType()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String str;
    resources->GetString((*sDrainTypeDesciptions)[mDrainType], &str);
    return str;
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos