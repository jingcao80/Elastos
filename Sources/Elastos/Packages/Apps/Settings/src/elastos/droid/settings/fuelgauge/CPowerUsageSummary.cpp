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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/settings/fuelgauge/CPowerUsageSummary.h"
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryDetail.h"
#include "elastos/droid/settings/fuelgauge/CPowerGaugePreference.h"
#include "elastos/droid/settings/fuelgauge/CPowerUsageDetail.h"
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryPreference.h"
#include "elastos/droid/settings/HelpUtils.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Os::IBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::CBatteryStatsHelperHelper;
using Elastos::Droid::Internal::Os::CBatteryStatsHelper;
using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Internal::Os::IPowerProfile;
using Elastos::Droid::Internal::Os::BatterySipperDrainType;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_OVERCOUNTED;
using Elastos::Droid::Internal::Os::BatterySipperDrainType_UNACCOUNTED;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IBatteryStatsUid;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

//===============================================================================
//                  CPowerUsageSummary::MyBroadcastReceiver
//===============================================================================

CPowerUsageSummary::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CPowerUsageSummary* host)
    : mHost(host)
{}

ECode CPowerUsageSummary::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_BATTERY_CHANGED.Equals(action)
            && mHost->UpdateBatteryStatus(intent)) {
        Boolean res;
    mHost->mHandler->HasMessages(MSG_REFRESH_STATS, &res);
        if (!res) {
            mHost->mHandler->SendEmptyMessageDelayed(MSG_REFRESH_STATS, 500, &res);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CPowerUsageSummary::MyHandler
//===============================================================================

CPowerUsageSummary::MyHandler::MyHandler(
    /* [in] */ CPowerUsageSummary* host)
    : mHost(host)
{}

ECode CPowerUsageSummary::MyHandler::constructor()
{
    return Handler::constructor();
}

ECode CPowerUsageSummary::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case BatteryEntry::MSG_UPDATE_NAME_ICON: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            BatteryEntry* entry = (BatteryEntry*) IBatteryEntry::Probe(obj);
            AutoPtr<IBatteryStatsUid> uidObj;
            entry->mSipper->GetUidObj((IBatteryStatsUid**)&uidObj);
            Int32 uid;
            uidObj->GetUid(&uid);
            AutoPtr<IPreference> pref;
            mHost->FindPreference(CoreUtils::Convert(StringUtils::ToString(uid)), (IPreference**)&pref);
            CPowerGaugePreference* pgp = (CPowerGaugePreference*)IPowerGaugePreference::Probe(pref);
            if (pgp != NULL) {
                Int32 sipUid;
                entry->mSipper->GetUid(&sipUid);
                const Int32 userId = UserHandle::GetUserId(sipUid);
                AutoPtr<IUserHandle> userHandle;
                CUserHandle::New(userId, (IUserHandle**)&userHandle);
                AutoPtr<IDrawable> drawable;
                mHost->mUm->GetBadgedIconForUser(entry->GetIcon(), userHandle, (IDrawable**)&drawable);
                pgp->SetIcon(drawable);
                pgp->SetTitle(CoreUtils::Convert(entry->mName));
            }
            break;
        }
        case BatteryEntry::MSG_REPORT_FULLY_DRAWN: {
            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            if (activity != NULL) {
                activity->ReportFullyDrawn();
            }
            break;
        }
        case MSG_REFRESH_STATS:
            mHost->mStatsHelper->ClearStats();
            mHost->RefreshStats();
            break;
    }
    return Handler::HandleMessage(msg);
}

//===============================================================================
//                  CPowerUsageSummary
//===============================================================================

const String CPowerUsageSummary::TAG("CPowerUsageSummary");
const Boolean CPowerUsageSummary::DEBUG = FALSE;
const String CPowerUsageSummary::KEY_APP_LIST("app_list");
const String CPowerUsageSummary::BATTERY_HISTORY_FILE("tmp_bat_history.bin");
const Int32 CPowerUsageSummary::MENU_STATS_TYPE = IMenu::FIRST;
const Int32 CPowerUsageSummary::MENU_STATS_REFRESH = IMenu::FIRST + 1;
const Int32 CPowerUsageSummary::MENU_BATTERY_SAVER = IMenu::FIRST + 2;
const Int32 CPowerUsageSummary::MENU_HELP = IMenu::FIRST + 3;
const Int32 CPowerUsageSummary::MIN_POWER_THRESHOLD_MILLI_AMP = 5;
const Int32 CPowerUsageSummary::MAX_ITEMS_TO_LIST = 10;
const Int32 CPowerUsageSummary::MIN_AVERAGE_POWER_THRESHOLD_MILLI_AMP = 10;
const Int32 CPowerUsageSummary::SECONDS_IN_HOUR = 60 * 60;
const Int32 CPowerUsageSummary::MSG_REFRESH_STATS;

CAR_OBJECT_IMPL(CPowerUsageSummary)

CPowerUsageSummary::CPowerUsageSummary()
    : mStatsType(IBatteryStats::STATS_SINCE_CHARGED)
{}

CPowerUsageSummary::~CPowerUsageSummary()
{}

ECode CPowerUsageSummary::constructor()
{
    mBatteryInfoReceiver = new MyBroadcastReceiver(this);
    mHandler = new MyHandler(this);
    mHandler->constructor();
    return PreferenceFragment::constructor();
}

ECode CPowerUsageSummary::OnAttach(
    /* [in] */ IActivity* activity)
{
    PreferenceFragment::OnAttach(activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IUserManager::Probe(obj);
    CBatteryStatsHelper::New(IContext::Probe(activity), TRUE, (IBatteryStatsHelper**)&mStatsHelper);
    return NOERROR;
}

ECode CPowerUsageSummary::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceFragment::OnCreate(icicle);
    mStatsHelper->Create(icicle);

    AddPreferencesFromResource(R::xml::power_usage_summary);
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(KEY_APP_LIST), (IPreference**)&pref);
    mAppListGroup = IPreferenceGroup::Probe(pref);
    SetHasOptionsMenu(TRUE);
    return NOERROR;
}

ECode CPowerUsageSummary::OnStart()
{
    PreferenceFragment::OnStart();
    mStatsHelper->ClearStats();
    return NOERROR;
}

ECode CPowerUsageSummary::OnResume()
{
    PreferenceFragment::OnResume();
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IBatteryStatsHelperHelper> helper;
    CBatteryStatsHelperHelper::AcquireSingleton((IBatteryStatsHelperHelper**)&helper);
    helper->DropFile(activity, BATTERY_HISTORY_FILE);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    activity->RegisterReceiver(mBatteryInfoReceiver, filter, (IIntent**)&intent);
    UpdateBatteryStatus(intent);
    Boolean res;
    mHandler->HasMessages(MSG_REFRESH_STATS, &res);
    if (res) {
        mHandler->RemoveMessages(MSG_REFRESH_STATS);
        mStatsHelper->ClearStats();
    }
    RefreshStats();
    return NOERROR;
}

ECode CPowerUsageSummary::OnPause()
{
    BatteryEntry::StopRequestQueue();
    mHandler->RemoveMessages(BatteryEntry::MSG_UPDATE_NAME_ICON);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mBatteryInfoReceiver);
    return PreferenceFragment::OnPause();
}

ECode CPowerUsageSummary::OnStop()
{
    PreferenceFragment::OnStop();
    return mHandler->RemoveMessages(MSG_REFRESH_STATS);
}

ECode CPowerUsageSummary::OnDestroy()
{
    PreferenceFragment::OnDestroy();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Boolean res;
    if (activity->IsChangingConfigurations(&res), res) {
        mStatsHelper->StoreState();
        BatteryEntry::ClearUidCache();
    }
    return NOERROR;
}

ECode CPowerUsageSummary::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (IBatteryHistoryPreference::Probe(preference) != NULL) {
        mStatsHelper->StoreStatsHistoryInFile(BATTERY_HISTORY_FILE);
        AutoPtr<IBundle> args;
        CBundle::New((IBundle**)&args);
        args->PutString(CBatteryHistoryDetail::EXTRA_STATS, BATTERY_HISTORY_FILE);
        AutoPtr<IIntent> intent;
        mStatsHelper->GetBatteryBroadcast((IIntent**)&intent);
        args->PutParcelable(CBatteryHistoryDetail::EXTRA_BROADCAST, IParcelable::Probe(intent));
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
        sa->StartPreferencePanel(String("Elastos.Droid.Settings.Fuelgauge.CBatteryHistoryDetail"), args,
                R::string::history_details_title, NULL, NULL, 0);
        return PreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
    }
    if (IPowerGaugePreference::Probe(preference) == NULL) {
        return NOERROR;
    }
    CPowerGaugePreference* pgp = (CPowerGaugePreference*) IPowerGaugePreference::Probe(preference);
    AutoPtr<BatteryEntry> entry = pgp->GetInfo();

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CPowerUsageDetail::StartBatteryDetailPage((CSettingsActivity*) ISettingsActivity::Probe(activity),
            mStatsHelper, mStatsType, entry, TRUE);
    return PreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
}

ECode CPowerUsageSummary::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    if (DEBUG) {
        AutoPtr<IMenuItem> item;
        menu->Add(0, MENU_STATS_TYPE, 0, R::string::menu_stats_total, (IMenuItem**)&item);
        item->SetIcon(Elastos::Droid::R::drawable::ic_menu_info_details);
        item->SetAlphabeticShortcut('t');
    }

    AutoPtr<IMenuItem> refresh;
    menu->Add(0, MENU_STATS_REFRESH, 0, R::string::menu_stats_refresh, (IMenuItem**)&refresh);
    refresh->SetIcon(Elastos::Droid::R::drawable::ic_menu_refresh);
    refresh->SetAlphabeticShortcut('r');
    refresh->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM | IMenuItem::SHOW_AS_ACTION_WITH_TEXT);

    AutoPtr<IMenuItem> batterySaver;
    menu->Add(0, MENU_BATTERY_SAVER, 0, R::string::battery_saver, (IMenuItem**)&batterySaver);
    batterySaver->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String helpUrl;
    resources->GetString(R::string::help_url_battery, &helpUrl);
    if (!TextUtils::IsEmpty(helpUrl)) {
        AutoPtr<IMenuItem> help;
        menu->Add(0, MENU_HELP, 0, R::string::help_label, (IMenuItem**)&help);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        HelpUtils::PrepareHelpMenuItem(IContext::Probe(activity), help, helpUrl);
    }
    return NOERROR;
}

ECode CPowerUsageSummary::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_STATS_TYPE:
            if (mStatsType == IBatteryStats::STATS_SINCE_CHARGED) {
                mStatsType = IBatteryStats::STATS_SINCE_UNPLUGGED;
            }
            else {
                mStatsType = IBatteryStats::STATS_SINCE_CHARGED;
            }
            RefreshStats();
            *result = TRUE;
            return NOERROR;
        case MENU_STATS_REFRESH:
            mStatsHelper->ClearStats();
            RefreshStats();
            mHandler->RemoveMessages(MSG_REFRESH_STATS);
            *result = TRUE;
            return NOERROR;
        case MENU_BATTERY_SAVER: {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            CSettingsActivity* sa = (CSettingsActivity*) ISettingsActivity::Probe(activity);
            sa->StartPreferencePanel(String("Elastos.Droid.Settings.Fuelgauge.CBatterySaverSettings"), NULL,
                    R::string::battery_saver, NULL, NULL, 0);
            *result = TRUE;
            return NOERROR;
        }
        default:
            return NOERROR;
    }
    return NOERROR;
}

void CPowerUsageSummary::AddNotAvailableMessage()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreference> notAvailable;
    CPreference::New(IContext::Probe(activity), (IPreference**)&notAvailable);
    notAvailable->SetTitle(R::string::power_usage_not_available);
    ((CBatteryHistoryPreference*)mHistPref.Get())->SetHideLabels(TRUE);
    Boolean res;
    mAppListGroup->AddPreference(notAvailable, &res);
}

Boolean CPowerUsageSummary::UpdateBatteryStatus(
    /* [in] */ IIntent* intent)
{
    if (intent != NULL) {
        String batteryLevel = Utils::GetBatteryPercentage(intent);
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String batteryStatus = Utils::GetBatteryStatus(resources, intent);
        if (!batteryLevel.Equals(mBatteryLevel) || !batteryStatus.Equals(mBatteryStatus)) {
            mBatteryLevel = batteryLevel;
            mBatteryStatus = batteryStatus;
            return TRUE;
        }
    }
    return FALSE;
}

void CPowerUsageSummary::RefreshStats()
{
    mAppListGroup->RemoveAll();
    mAppListGroup->SetOrderingAsAdded(FALSE);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);

    AutoPtr<IBatteryStats> stats;
    mStatsHelper->GetStats((IBatteryStats**)&stats);
    AutoPtr<IIntent> intent;
    mStatsHelper->GetBatteryBroadcast((IIntent**)&intent);
    CBatteryHistoryPreference::New(activity, stats, intent, (IBatteryHistoryPreference**)&mHistPref);
    IPreference::Probe(mHistPref)->SetOrder(-1);
    Boolean res;
    mAppListGroup->AddPreference(IPreference::Probe(mHistPref), &res);
    Boolean addedSome = FALSE;

    AutoPtr<IPowerProfile> powerProfile;
    mStatsHelper->GetPowerProfile((IPowerProfile**)&powerProfile);
    Double averagePower;
    powerProfile->GetAveragePower(IPowerProfile::POWER_SCREEN_FULL, &averagePower);
    if (averagePower >= MIN_AVERAGE_POWER_THRESHOLD_MILLI_AMP) {
        AutoPtr<IList> profiles; // List<UserHandle>
        mUm->GetUserProfiles((IList**)&profiles);

        mStatsHelper->RefreshStats(IBatteryStats::STATS_SINCE_CHARGED, profiles);

        AutoPtr<IList> usageList;//List<BatterySipper>
        mStatsHelper->GetUsageList((IList**)&usageList);

        Int32 dischargeAmount = 0;
        if (stats != NULL) {
            stats->GetDischargeAmount(mStatsType, &dischargeAmount);
        }
        Int32 numSippers;
        usageList->GetSize(&numSippers);
        for (Int32 i = 0; i < numSippers; i++) {
            AutoPtr<IInterface> obj;
            usageList->Get(i, (IInterface**)&obj);
            IBatterySipper* sipper = IBatterySipper::Probe(obj);
            Double value;
            sipper->GetValue(&value);
            if ((value * SECONDS_IN_HOUR) < MIN_POWER_THRESHOLD_MILLI_AMP) {
                continue;
            }
            Double totalPower;
            mStatsHelper->GetTotalPower(&totalPower);
            const Double percentOfTotal = ((value / totalPower) * dischargeAmount);
            if (((Int32) (percentOfTotal + .5)) < 1) {
                continue;
            }
            BatterySipperDrainType drainType;
            sipper->GetDrainType(&drainType);
            Double maxRealPower;
            mStatsHelper->GetMaxRealPower(&maxRealPower);
            if (drainType == BatterySipperDrainType_OVERCOUNTED) {
                // Don't show over-counted unless it is at least 2/3 the size of
                // the largest real entry, and its percent of total is more significant
                if (value < ((maxRealPower*2)/3)) {
                    continue;
                }
                if (percentOfTotal < 10) {
                    continue;
                }
                if (String("user").Equals(Build::TYPE)) {
                    continue;
                }
            }
            if (drainType == BatterySipperDrainType_UNACCOUNTED) {
                // Don't show over-counted unless it is at least 1/2 the size of
                // the largest real entry, and its percent of total is more significant
                if (value < (maxRealPower/2)) {
                    continue;
                }
                if (percentOfTotal < 5) {
                    continue;
                }
                if (String("user").Equals(Build::TYPE)) {
                    continue;
                }
            }
            Int32 uid;
            sipper->GetUid(&uid);
            AutoPtr<IUserHandle> userHandle;
            CUserHandle::New(UserHandle::GetUserId(uid), (IUserHandle**)&userHandle);
            AutoPtr<BatteryEntry> entry = new BatteryEntry(activity, mHandler, mUm, sipper);
            AutoPtr<IDrawable> badgedIcon;
            mUm->GetBadgedIconForUser(entry->GetIcon(), userHandle, (IDrawable**)&badgedIcon);
            AutoPtr<ICharSequence> contentDescription;
            mUm->GetBadgedLabelForUser(CoreUtils::Convert(entry->GetLabel()), userHandle, (ICharSequence**)&contentDescription);
            AutoPtr<IPreference> pref;
            CPowerGaugePreference::New(activity, badgedIcon, contentDescription, entry, (IPreference**)&pref);

            Double maxPower;
            mStatsHelper->GetMaxPower(&maxPower);
            Double percentOfMax = (value * 100) / maxPower;
            sipper->SetPercent(percentOfTotal);
            pref->SetTitle(CoreUtils::Convert(entry->GetLabel()));
            pref->SetOrder(i + 1);
            ((CPowerGaugePreference*)pref.Get())->SetPercent(percentOfMax, percentOfTotal);
            AutoPtr<IBatteryStatsUid> uidObj;
            sipper->GetUidObj((IBatteryStatsUid**)&uidObj);
            if (uidObj != NULL) {
                Int32 uid;
                uidObj->GetUid(&uid);
                pref->SetKey(StringUtils::ToString(uid));
            }
            addedSome = TRUE;
            mAppListGroup->AddPreference(pref, &res);
            Int32 count;
            mAppListGroup->GetPreferenceCount(&count);
            if (count > (MAX_ITEMS_TO_LIST + 1)) {
                break;
            }
        }
    }
    if (!addedSome) {
        AddNotAvailableMessage();
    }

    BatteryEntry::StartRequestQueue();
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos