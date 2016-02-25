/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.settings.fuelgauge;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;

using Elastos::Droid::Internal::Os::IBatterySipper;
using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Internal::Os::IPowerProfile;
using Elastos::Droid::Settings::IHelpUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;

using Elastos::Utility::IList;

/**
 * Displays a list of apps and subsystems that consume power, ordered by how much power was
 * consumed since the last time it was unplugged.
 */
public class PowerUsageSummary extends PreferenceFragment {

    private static const Boolean DEBUG = FALSE;

    static const String TAG = "PowerUsageSummary";

    private static const String KEY_APP_LIST = "app_list";

    private static const String BATTERY_HISTORY_FILE = "tmp_bat_history.bin";

    private static const Int32 MENU_STATS_TYPE = Menu.FIRST;
    private static const Int32 MENU_STATS_REFRESH = Menu.FIRST + 1;
    private static const Int32 MENU_BATTERY_SAVER = Menu.FIRST + 2;
    private static const Int32 MENU_HELP = Menu.FIRST + 3;

    private UserManager mUm;

    private BatteryHistoryPreference mHistPref;
    private PreferenceGroup mAppListGroup;
    private String mBatteryLevel;
    private String mBatteryStatus;

    private Int32 mStatsType = BatteryStats.STATS_SINCE_CHARGED;

    private static const Int32 MIN_POWER_THRESHOLD_MILLI_AMP = 5;
    private static const Int32 MAX_ITEMS_TO_LIST = 10;
    private static const Int32 MIN_AVERAGE_POWER_THRESHOLD_MILLI_AMP = 10;
    private static const Int32 SECONDS_IN_HOUR = 60 * 60;

    private BatteryStatsHelper mStatsHelper;

    private BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (IIntent::ACTION_BATTERY_CHANGED->Equals(action)
                    && UpdateBatteryStatus(intent)) {
                if (!mHandler->HasMessages(MSG_REFRESH_STATS)) {
                    mHandler->SendEmptyMessageDelayed(MSG_REFRESH_STATS, 500);
                }
            }
        }
    };

    //@Override
    CARAPI OnAttach(Activity activity) {
        super->OnAttach(activity);
        mUm = (UserManager) activity->GetSystemService(Context.USER_SERVICE);
        mStatsHelper = new BatteryStatsHelper(activity, TRUE);
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mStatsHelper->Create(icicle);

        AddPreferencesFromResource(R.xml.power_usage_summary);
        mAppListGroup = (PreferenceGroup) FindPreference(KEY_APP_LIST);
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();
        mStatsHelper->ClearStats();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        BatteryStatsHelper->DropFile(GetActivity(), BATTERY_HISTORY_FILE);
        UpdateBatteryStatus(GetActivity()->RegisterReceiver(mBatteryInfoReceiver,
                new IntentFilter(IIntent::ACTION_BATTERY_CHANGED)));
        if (mHandler->HasMessages(MSG_REFRESH_STATS)) {
            mHandler->RemoveMessages(MSG_REFRESH_STATS);
            mStatsHelper->ClearStats();
        }
        RefreshStats();
    }

    //@Override
    CARAPI OnPause() {
        BatteryEntry->StopRequestQueue();
        mHandler->RemoveMessages(BatteryEntry.MSG_UPDATE_NAME_ICON);
        GetActivity()->UnregisterReceiver(mBatteryInfoReceiver);
        super->OnPause();
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        mHandler->RemoveMessages(MSG_REFRESH_STATS);
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();
        if (GetActivity()->IsChangingConfigurations()) {
            mStatsHelper->StoreState();
            BatteryEntry->ClearUidCache();
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference instanceof BatteryHistoryPreference) {
            mStatsHelper->StoreStatsHistoryInFile(BATTERY_HISTORY_FILE);
            Bundle args = new Bundle();
            args->PutString(BatteryHistoryDetail.EXTRA_STATS, BATTERY_HISTORY_FILE);
            args->PutParcelable(BatteryHistoryDetail.EXTRA_BROADCAST,
                    mStatsHelper->GetBatteryBroadcast());
            SettingsActivity sa = (SettingsActivity) GetActivity();
            sa->StartPreferencePanel(BatteryHistoryDetail.class->GetName(), args,
                    R::string::history_details_title, NULL, NULL, 0);
            return super->OnPreferenceTreeClick(preferenceScreen, preference);
        }
        if (!(preference instanceof PowerGaugePreference)) {
            return FALSE;
        }
        PowerGaugePreference pgp = (PowerGaugePreference) preference;
        BatteryEntry entry = pgp->GetInfo();
        PowerUsageDetail->StartBatteryDetailPage((SettingsActivity) GetActivity(), mStatsHelper,
                mStatsType, entry, TRUE);
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (DEBUG) {
            menu->Add(0, MENU_STATS_TYPE, 0, R::string::menu_stats_total)
                    .SetIcon(R.drawable.ic_menu_info_details)
                    .SetAlphabeticShortcut('t');
        }
        MenuItem refresh = menu->Add(0, MENU_STATS_REFRESH, 0, R::string::menu_stats_refresh)
                .SetIcon(R.drawable.ic_menu_refresh)
                .SetAlphabeticShortcut('r');
        refresh->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);

        MenuItem batterySaver = menu->Add(0, MENU_BATTERY_SAVER, 0, R::string::battery_saver);
        batterySaver->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);

        String helpUrl;
        if (!TextUtils->IsEmpty(helpUrl = GetResources()->GetString(R::string::help_url_battery))) {
            final MenuItem help = menu->Add(0, MENU_HELP, 0, R::string::help_label);
            HelpUtils->PrepareHelpMenuItem(GetActivity(), help, helpUrl);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_STATS_TYPE:
                if (mStatsType == BatteryStats.STATS_SINCE_CHARGED) {
                    mStatsType = BatteryStats.STATS_SINCE_UNPLUGGED;
                } else {
                    mStatsType = BatteryStats.STATS_SINCE_CHARGED;
                }
                RefreshStats();
                return TRUE;
            case MENU_STATS_REFRESH:
                mStatsHelper->ClearStats();
                RefreshStats();
                mHandler->RemoveMessages(MSG_REFRESH_STATS);
                return TRUE;
            case MENU_BATTERY_SAVER:
                final SettingsActivity sa = (SettingsActivity) GetActivity();
                sa->StartPreferencePanel(BatterySaverSettings.class->GetName(), NULL,
                        R::string::battery_saver, NULL, NULL, 0);
                return TRUE;
            default:
                return FALSE;
        }
    }

    private void AddNotAvailableMessage() {
        Preference notAvailable = new Preference(GetActivity());
        notAvailable->SetTitle(R::string::power_usage_not_available);
        mHistPref->SetHideLabels(TRUE);
        mAppListGroup->AddPreference(notAvailable);
    }

    private Boolean UpdateBatteryStatus(Intent intent) {
        if (intent != NULL) {
            String batteryLevel = com.android.settings.Utils->GetBatteryPercentage(intent);
            String batteryStatus = com.android.settings.Utils->GetBatteryStatus(GetResources(),
                    intent);
            if (!batteryLevel->Equals(mBatteryLevel) || !batteryStatus->Equals(mBatteryStatus)) {
                mBatteryLevel = batteryLevel;
                mBatteryStatus = batteryStatus;
                return TRUE;
            }
        }
        return FALSE;
    }

    private void RefreshStats() {
        mAppListGroup->RemoveAll();
        mAppListGroup->SetOrderingAsAdded(FALSE);
        mHistPref = new BatteryHistoryPreference(GetActivity(), mStatsHelper->GetStats(),
                mStatsHelper->GetBatteryBroadcast());
        mHistPref->SetOrder(-1);
        mAppListGroup->AddPreference(mHistPref);
        Boolean addedSome = FALSE;

        final PowerProfile powerProfile = mStatsHelper->GetPowerProfile();
        final BatteryStats stats = mStatsHelper->GetStats();
        final Double averagePower = powerProfile->GetAveragePower(PowerProfile.POWER_SCREEN_FULL);
        if (averagePower >= MIN_AVERAGE_POWER_THRESHOLD_MILLI_AMP) {
            final List<UserHandle> profiles = mUm->GetUserProfiles();

            mStatsHelper->RefreshStats(BatteryStats.STATS_SINCE_CHARGED, profiles);

            final List<BatterySipper> usageList = mStatsHelper->GetUsageList();

            final Int32 dischargeAmount = stats != NULL ? stats->GetDischargeAmount(mStatsType) : 0;
            final Int32 numSippers = usageList->Size();
            for (Int32 i = 0; i < numSippers; i++) {
                final BatterySipper sipper = usageList->Get(i);
                if ((sipper.value * SECONDS_IN_HOUR) < MIN_POWER_THRESHOLD_MILLI_AMP) {
                    continue;
                }
                final Double percentOfTotal =
                        ((sipper.value / mStatsHelper->GetTotalPower()) * dischargeAmount);
                if (((Int32) (percentOfTotal + .5)) < 1) {
                    continue;
                }
                if (sipper.drainType == BatterySipper.DrainType.OVERCOUNTED) {
                    // Don't show over-counted unless it is at least 2/3 the size of
                    // the largest real entry, and its percent of total is more significant
                    if (sipper.value < ((mStatsHelper->GetMaxRealPower()*2)/3)) {
                        continue;
                    }
                    if (percentOfTotal < 10) {
                        continue;
                    }
                    if ("user".Equals(Build.TYPE)) {
                        continue;
                    }
                }
                if (sipper.drainType == BatterySipper.DrainType.UNACCOUNTED) {
                    // Don't show over-counted unless it is at least 1/2 the size of
                    // the largest real entry, and its percent of total is more significant
                    if (sipper.value < (mStatsHelper->GetMaxRealPower()/2)) {
                        continue;
                    }
                    if (percentOfTotal < 5) {
                        continue;
                    }
                    if ("user".Equals(Build.TYPE)) {
                        continue;
                    }
                }
                final UserHandle userHandle = new UserHandle(UserHandle->GetUserId(sipper->GetUid()));
                final BatteryEntry entry = new BatteryEntry(GetActivity(), mHandler, mUm, sipper);
                final Drawable badgedIcon = mUm->GetBadgedIconForUser(entry->GetIcon(),
                        userHandle);
                final CharSequence contentDescription = mUm->GetBadgedLabelForUser(entry->GetLabel(),
                        userHandle);
                final PowerGaugePreference pref = new PowerGaugePreference(GetActivity(),
                        badgedIcon, contentDescription, entry);

                final Double percentOfMax = (sipper.value * 100) / mStatsHelper->GetMaxPower();
                sipper.percent = percentOfTotal;
                pref->SetTitle(entry->GetLabel());
                pref->SetOrder(i + 1);
                pref->SetPercent(percentOfMax, percentOfTotal);
                if (sipper.uidObj != NULL) {
                    pref->SetKey(Integer->ToString(sipper.uidObj->GetUid()));
                }
                addedSome = TRUE;
                mAppListGroup->AddPreference(pref);
                if (mAppListGroup->GetPreferenceCount() > (MAX_ITEMS_TO_LIST + 1)) {
                    break;
                }
            }
        }
        if (!addedSome) {
            AddNotAvailableMessage();
        }

        BatteryEntry->StartRequestQueue();
    }

    static const Int32 MSG_REFRESH_STATS = 100;

    Handler mHandler = new Handler() {

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case BatteryEntry.MSG_UPDATE_NAME_ICON:
                    BatteryEntry entry = (BatteryEntry) msg.obj;
                    PowerGaugePreference pgp =
                            (PowerGaugePreference) FindPreference(
                                    Integer->ToString(entry.sipper.uidObj->GetUid()));
                    if (pgp != NULL) {
                        final Int32 userId = UserHandle->GetUserId(entry.sipper->GetUid());
                        final UserHandle userHandle = new UserHandle(userId);
                        pgp->SetIcon(mUm->GetBadgedIconForUser(entry->GetIcon(), userHandle));
                        pgp->SetTitle(entry.name);
                    }
                    break;
                case BatteryEntry.MSG_REPORT_FULLY_DRAWN:
                    Activity activity = GetActivity();
                    if (activity != NULL) {
                        activity->ReportFullyDrawn();
                    }
                    break;
                case MSG_REFRESH_STATS:
                    mStatsHelper->ClearStats();
                    RefreshStats();
            }
            super->HandleMessage(msg);
        }
    };
}
