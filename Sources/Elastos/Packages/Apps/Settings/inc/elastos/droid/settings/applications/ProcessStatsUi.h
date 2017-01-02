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

#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSUI_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSUI_H__

#include "_Elastos_Droid_Settings_Applications_CProcessStatsUi.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ITimeUtils;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::ISubMenu;
using Elastos::Droid::Internal::App::IIProcessStats;
using Elastos::Droid::Internal::App::IProcessMap;
using Elastos::Droid::Internal::App::IProcessStats;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;

using Elastos::IO::IInputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

public class ProcessStatsUi extends PreferenceFragment
        implements LinearColorBar.OnRegionTappedListener {
    static const String TAG = "ProcessStatsUi";
    static const Boolean DEBUG = FALSE;

    private static const String KEY_APP_LIST = "app_list";
    private static const String KEY_MEM_STATUS = "mem_status";

    private static const Int32 NUM_DURATIONS = 4;

    private static const Int32 MENU_STATS_REFRESH = Menu.FIRST;
    private static const Int32 MENU_DURATION = Menu.FIRST + 1;
    private static const Int32 MENU_SHOW_SYSTEM = MENU_DURATION + NUM_DURATIONS;
    private static const Int32 MENU_USE_USS = MENU_SHOW_SYSTEM + 1;
    private static const Int32 MENU_TYPE_BACKGROUND = MENU_USE_USS + 1;
    private static const Int32 MENU_TYPE_FOREGROUND = MENU_TYPE_BACKGROUND + 1;
    private static const Int32 MENU_TYPE_CACHED = MENU_TYPE_FOREGROUND + 1;
    private static const Int32 MENU_HELP = MENU_TYPE_CACHED + 1;

    static const Int32 MAX_ITEMS_TO_LIST = 60;

    static const Comparator<ProcStatsEntry> sEntryCompare = new Comparator<ProcStatsEntry>() {
        //@Override
        public Int32 Compare(ProcStatsEntry lhs, ProcStatsEntry rhs) {
            if (lhs.mWeight < rhs.mWeight) {
                return 1;
            } else if (lhs.mWeight > rhs.mWeight) {
                return -1;
            } else if (lhs.mDuration < rhs.mDuration) {
                return 1;
            } else if (lhs.mDuration > rhs.mDuration) {
                return -1;
            }
            return 0;
        }
    };

    private static ProcessStats sStatsXfer;

    IProcessStats mProcessStats;
    UserManager mUm;
    ProcessStats mStats;
    Int32 mMemState;

    private Int64 mDuration;
    private Int64 mLastDuration;
    private Boolean mShowSystem;
    private Boolean mUseUss;
    private Int32 mStatsType;
    private Int32 mMemRegion;

    private MenuItem[] mDurationMenus = new MenuItem[NUM_DURATIONS];
    private MenuItem mShowSystemMenu;
    private MenuItem mUseUssMenu;
    private MenuItem mTypeBackgroundMenu;
    private MenuItem mTypeForegroundMenu;
    private MenuItem mTypeCachedMenu;

    private PreferenceGroup mAppListGroup;
    private Preference mMemStatusPref;

    Int64 mMaxWeight;
    Int64 mTotalTime;

    Int64[] mMemTimes = new Int64[IProcessStats::ADJ_MEM_FACTOR_COUNT];
    Double[] mMemStateWeights = new Double[IProcessStats::STATE_COUNT];
    Double mMemCachedWeight;
    Double mMemFreeWeight;
    Double mMemZRamWeight;
    Double mMemKernelWeight;
    Double mMemNativeWeight;
    Double mMemTotalWeight;

    // The actual duration value to use for each duration option.  Note these
    // are lower than the actual duration, since our durations are computed in
    // batches of 3 hours so we want to allow the time we use to be slightly
    // smaller than the actual time selected instead of bumping up to 3 hours
    // beyond it.
    private static const Int64 DURATION_QUANTUM = IProcessStats::COMMIT_PERIOD;
    private static Int64[] sDurations = new Int64[] {
        3*60*60*1000 - DURATION_QUANTUM/2, 6*60*60*1000 - DURATION_QUANTUM/2,
        12*60*60*1000 - DURATION_QUANTUM/2, 24*60*60*1000 - DURATION_QUANTUM/2
    };
    private static Int32[] sDurationLabels = new Int32[] {
            R::string::menu_duration_3h, R::string::menu_duration_6h,
            R::string::menu_duration_12h, R::string::menu_duration_1d
    };

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        PreferenceFragment::OnCreate(icicle);

        if (icicle != NULL) {
            mStats = sStatsXfer;
        }

        AddPreferencesFromResource(R::xml::process_stats_summary);
        mProcessStats = IProcessStats.Stub->AsInterface(
                ServiceManager->GetService(IProcessStats::SERVICE_NAME));
        mUm = (UserManager)GetActivity()->GetSystemService(Context.USER_SERVICE);
        mAppListGroup = (PreferenceGroup) FindPreference(KEY_APP_LIST);
        mMemStatusPref = mAppListGroup->FindPreference(KEY_MEM_STATUS);
        mDuration = icicle != NULL ? icicle->GetLong("duration", sDurations[0]) : sDurations[0];
        mShowSystem = icicle != NULL ? icicle->GetBoolean("show_system") : FALSE;
        mUseUss = icicle != NULL ? icicle->GetBoolean("use_uss") : FALSE;
        mStatsType = icicle != NULL ? icicle->GetInt("stats_type", MENU_TYPE_BACKGROUND)
                : MENU_TYPE_BACKGROUND;
        mMemRegion = icicle != NULL ? icicle->GetInt("mem_region", LinearColorBar.REGION_GREEN)
                : LinearColorBar.REGION_GREEN;
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnResume() {
        PreferenceFragment::OnResume();
        RefreshStats();
    }

    //@Override
    CARAPI OnPause() {
        PreferenceFragment::OnPause();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        PreferenceFragment::OnSaveInstanceState(outState);
        outState->PutLong("duration", mDuration);
        outState->PutBoolean("show_system", mShowSystem);
        outState->PutBoolean("use_uss", mUseUss);
        outState->PutInt("stats_type", mStatsType);
        outState->PutInt("mem_region", mMemRegion);
    }

    //@Override
    CARAPI OnDestroy() {
        PreferenceFragment::OnDestroy();
        if (GetActivity()->IsChangingConfigurations()) {
            sStatsXfer = mStats;
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference instanceof LinearColorPreference) {
            Bundle args = new Bundle();
            args->PutLongArray(ProcessStatsMemDetail.EXTRA_MEM_TIMES, mMemTimes);
            args->PutDoubleArray(ProcessStatsMemDetail.EXTRA_MEM_STATE_WEIGHTS, mMemStateWeights);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_CACHED_WEIGHT, mMemCachedWeight);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_FREE_WEIGHT, mMemFreeWeight);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_ZRAM_WEIGHT, mMemZRamWeight);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_KERNEL_WEIGHT, mMemKernelWeight);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_NATIVE_WEIGHT, mMemNativeWeight);
            args->PutDouble(ProcessStatsMemDetail.EXTRA_MEM_TOTAL_WEIGHT, mMemTotalWeight);
            args->PutBoolean(ProcessStatsMemDetail.EXTRA_USE_USS, mUseUss);
            args->PutLong(ProcessStatsMemDetail.EXTRA_TOTAL_TIME, mTotalTime);
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    ProcessStatsMemDetail.class->GetName(), args, R::string::mem_details_title,
                    NULL, NULL, 0);
            return TRUE;
        }

        if (!(preference instanceof ProcessStatsPreference)) {
            return FALSE;
        }

        ProcessStatsPreference pgp = (ProcessStatsPreference) preference;
        Bundle args = new Bundle();
        args->PutParcelable(ProcessStatsDetail.EXTRA_ENTRY, pgp->GetEntry());
        args->PutBoolean(ProcessStatsDetail.EXTRA_USE_USS, mUseUss);
        args->PutLong(ProcessStatsDetail.EXTRA_MAX_WEIGHT, mMaxWeight);
        args->PutLong(ProcessStatsDetail.EXTRA_TOTAL_TIME, mTotalTime);
        ((SettingsActivity) GetActivity()).StartPreferencePanel(
                ProcessStatsDetail.class->GetName(), args, R::string::details_title, NULL, NULL, 0);

        return PreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        MenuItem refresh = menu->Add(0, MENU_STATS_REFRESH, 0, R::string::menu_stats_refresh)
                .SetIcon(R.drawable.ic_menu_refresh_holo_dark)
                .SetAlphabeticShortcut('r');
        refresh->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        SubMenu subMenu = menu->AddSubMenu(R::string::menu_proc_stats_duration);
        for (Int32 i=0; i<NUM_DURATIONS; i++) {
            mDurationMenus[i] = subMenu->Add(0, MENU_DURATION+i, 0, sDurationLabels[i])
                            .SetCheckable(TRUE);
        }
        mShowSystemMenu = menu->Add(0, MENU_SHOW_SYSTEM, 0, R::string::menu_show_system)
                .SetAlphabeticShortcut('s')
                .SetCheckable(TRUE);
        mUseUssMenu = menu->Add(0, MENU_USE_USS, 0, R::string::menu_use_uss)
                .SetAlphabeticShortcut('u')
                .SetCheckable(TRUE);
        subMenu = menu->AddSubMenu(R::string::menu_proc_stats_type);
        mTypeBackgroundMenu = subMenu->Add(0, MENU_TYPE_BACKGROUND, 0,
                R::string::menu_proc_stats_type_background)
                .SetAlphabeticShortcut('b')
                .SetCheckable(TRUE);
        mTypeForegroundMenu = subMenu->Add(0, MENU_TYPE_FOREGROUND, 0,
                R::string::menu_proc_stats_type_foreground)
                .SetAlphabeticShortcut('f')
                .SetCheckable(TRUE);
        mTypeCachedMenu = subMenu->Add(0, MENU_TYPE_CACHED, 0,
                R::string::menu_proc_stats_type_cached)
                .SetCheckable(TRUE);

        UpdateMenus();

        /*
        String helpUrl;
        if (!TextUtils->IsEmpty(helpUrl = GetResources()->GetString(R::string::help_url_battery))) {
            final MenuItem help = menu->Add(0, MENU_HELP, 0, R::string::help_label);
            HelpUtils->PrepareHelpMenuItem(GetActivity(), help, helpUrl);
        }
        */
    }

    void UpdateMenus() {
        Int32 closestIndex = 0;
        Int64 closestDelta = Math->Abs(sDurations[0]-mDuration);
        for (Int32 i=1; i<NUM_DURATIONS; i++) {
            Int64 delta = Math->Abs(sDurations[i]-mDuration);
            if (delta < closestDelta) {
                closestDelta = delta;
                closestIndex = i;
            }
        }
        for (Int32 i=0; i<NUM_DURATIONS; i++) {
            if (mDurationMenus[i] != NULL) {
                mDurationMenus[i].SetChecked(i == closestIndex);
            }
        }
        mDuration = sDurations[closestIndex];
        if (mShowSystemMenu != NULL) {
            mShowSystemMenu->SetChecked(mShowSystem);
            mShowSystemMenu->SetEnabled(mStatsType == MENU_TYPE_BACKGROUND);
        }
        if (mUseUssMenu != NULL) {
            mUseUssMenu->SetChecked(mUseUss);
        }
        if (mTypeBackgroundMenu != NULL) {
            mTypeBackgroundMenu->SetChecked(mStatsType == MENU_TYPE_BACKGROUND);
        }
        if (mTypeForegroundMenu != NULL) {
            mTypeForegroundMenu->SetChecked(mStatsType == MENU_TYPE_FOREGROUND);
        }
        if (mTypeCachedMenu != NULL) {
            mTypeCachedMenu->SetChecked(mStatsType == MENU_TYPE_CACHED);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        final Int32 id = item->GetItemId();
        switch (id) {
            case MENU_STATS_REFRESH:
                mStats = NULL;
                RefreshStats();
                return TRUE;
            case MENU_SHOW_SYSTEM:
                mShowSystem = !mShowSystem;
                RefreshStats();
                return TRUE;
            case MENU_USE_USS:
                mUseUss = !mUseUss;
                RefreshStats();
                return TRUE;
            case MENU_TYPE_BACKGROUND:
            case MENU_TYPE_FOREGROUND:
            case MENU_TYPE_CACHED:
                mStatsType = item->GetItemId();
                RefreshStats();
                return TRUE;
            default:
                if (id >= MENU_DURATION && id < (MENU_DURATION+NUM_DURATIONS)) {
                    mDuration = sDurations[id-MENU_DURATION];
                    RefreshStats();
                }
                return FALSE;
        }
    }

    //@Override
    CARAPI OnRegionTapped(Int32 region) {
        if (mMemRegion != region) {
            mMemRegion = region;
            RefreshStats();
        }
    }

    private void AddNotAvailableMessage() {
        Preference notAvailable = new Preference(GetActivity());
        notAvailable->SetTitle(R::string::power_usage_not_available);
        mAppListGroup->AddPreference(notAvailable);
    }

    public static const Int32[] BACKGROUND_AND_SYSTEM_PROC_STATES = new Int32[] {
            IProcessStats::STATE_PERSISTENT, IProcessStats::STATE_IMPORTANT_FOREGROUND,
            IProcessStats::STATE_IMPORTANT_BACKGROUND, IProcessStats::STATE_BACKUP,
            IProcessStats::STATE_HEAVY_WEIGHT, IProcessStats::STATE_SERVICE,
            IProcessStats::STATE_SERVICE_RESTARTING, IProcessStats::STATE_RECEIVER
    };

    public static const Int32[] FOREGROUND_PROC_STATES = new Int32[] {
            IProcessStats::STATE_TOP
    };

    public static const Int32[] CACHED_PROC_STATES = new Int32[] {
            IProcessStats::STATE_CACHED_ACTIVITY, IProcessStats::STATE_CACHED_ACTIVITY_CLIENT,
            IProcessStats::STATE_CACHED_EMPTY
    };

    public static const Int32[] RED_MEM_STATES = new Int32[] {
            IProcessStats::ADJ_MEM_FACTOR_CRITICAL
    };

    public static const Int32[] YELLOW_MEM_STATES = new Int32[] {
            IProcessStats::ADJ_MEM_FACTOR_CRITICAL, IProcessStats::ADJ_MEM_FACTOR_LOW,
            IProcessStats::ADJ_MEM_FACTOR_MODERATE
    };

    private String MakeDuration(Int64 time) {
        StringBuilder sb = new StringBuilder(32);
        TimeUtils->FormatDuration(time, sb);
        return sb->ToString();
    }

    private void RefreshStats() {
        UpdateMenus();

        if (mStats == NULL || mLastDuration != mDuration) {
            Load();
        }

        Int32[] stats;
        Int32 statsLabel;
        if (mStatsType == MENU_TYPE_FOREGROUND) {
            stats = FOREGROUND_PROC_STATES;
            statsLabel = R::string::process_stats_type_foreground;
        } else if (mStatsType == MENU_TYPE_CACHED) {
            stats = CACHED_PROC_STATES;
            statsLabel = R::string::process_stats_type_cached;
        } else {
            stats = mShowSystem ? BACKGROUND_AND_SYSTEM_PROC_STATES
                    : IProcessStats::BACKGROUND_PROC_STATES;
            statsLabel = R::string::process_stats_type_background;
        }

        mAppListGroup->RemoveAll();
        mAppListGroup->SetOrderingAsAdded(FALSE);

        final Int64 elapsedTime = mStats.mTimePeriodEndRealtime-mStats.mTimePeriodStartRealtime;

        mMemStatusPref->SetOrder(-2);
        mAppListGroup->AddPreference(mMemStatusPref);
        String durationString = Utils->FormatElapsedTime(GetActivity(), elapsedTime, FALSE);
        CharSequence memString;
        CharSequence[] memStatesStr = GetResources()->GetTextArray(R.array.ram_states);
        if (mMemState >= 0 && mMemState < memStatesStr.length) {
            memString = memStatesStr[mMemState];
        } else {
            memString = "?";
        }
        mMemStatusPref->SetTitle(GetActivity()->GetString(R::string::process_stats_total_duration,
                GetActivity()->GetString(statsLabel), durationString));
        mMemStatusPref->SetSummary(GetActivity()->GetString(R::string::process_stats_memory_status,
                        memString));
        /*
        mMemStatusPref->SetTitle(DateFormat->Format(DateFormat->GetBestDateTimePattern(
                GetActivity()->GetResources().GetConfiguration().locale,
                "MMMM dd, yyyy h:mm a"), mStats.mTimePeriodStartClock));
        */
        /*
        BatteryHistoryPreference hist = new BatteryHistoryPreference(GetActivity(), mStats);
        hist->SetOrder(-1);
        mAppListGroup->AddPreference(hist);
        */

        Int64 now = SystemClock->UptimeMillis();

        final PackageManager pm = GetActivity()->GetPackageManager();

        mTotalTime = ProcessStats->DumpSingleTime(NULL, NULL, mStats.mMemFactorDurations,
                mStats.mMemFactor, mStats.mStartTime, now);
        if (DEBUG) Logger::D(TAG, "Total time of stats: " + MakeDuration(mTotalTime));

        for (Int32 i=0; i<mMemTimes.length; i++) {
            mMemTimes[i] = 0;
        }
        for (Int32 iscreen=0; iscreen<IProcessStats::ADJ_COUNT; iscreen+=IProcessStats::ADJ_SCREEN_MOD) {
            for (Int32 imem=0; imem<IProcessStats::ADJ_MEM_FACTOR_COUNT; imem++) {
                Int32 state = imem+iscreen;
                mMemTimes[imem] += mStats.mMemFactorDurations[state];
            }
        }

        Int64 memTotalTime;
        Int32[] memStates;

        LinearColorPreference colors = new LinearColorPreference(GetActivity());
        colors->SetOrder(-1);
        switch (mMemRegion) {
            case LinearColorBar.REGION_RED:
                memTotalTime = mMemTimes[IProcessStats::ADJ_MEM_FACTOR_CRITICAL];
                memStates = RED_MEM_STATES;
                break;
            case LinearColorBar.REGION_YELLOW:
                memTotalTime = mMemTimes[IProcessStats::ADJ_MEM_FACTOR_CRITICAL]
                        + mMemTimes[IProcessStats::ADJ_MEM_FACTOR_LOW]
                        + mMemTimes[IProcessStats::ADJ_MEM_FACTOR_MODERATE];
                memStates = YELLOW_MEM_STATES;
                break;
            default:
                memTotalTime = mTotalTime;
                memStates = IProcessStats::ALL_MEM_ADJ;
                break;
        }
        colors->SetColoredRegions(LinearColorBar.REGION_RED);

        // Compute memory badness for chart color.
        Int32[] badColors = com.android.settings.Utils.BADNESS_COLORS;
        Int64 timeGood = mMemTimes[IProcessStats::ADJ_MEM_FACTOR_NORMAL];
        timeGood += (mMemTimes[IProcessStats::ADJ_MEM_FACTOR_MODERATE]*2)/3;
        timeGood += mMemTimes[IProcessStats::ADJ_MEM_FACTOR_LOW]/3;
        Float memBadness = ((Float)timeGood)/mTotalTime;
        Int32 badnessColor = badColors[1 + Math->Round(memBadness*(badColors.length-2))];
        colors->SetColors(badnessColor, badnessColor, badnessColor);

        // We are now going to scale the mMemTimes to match the total elapsed time.
        // These are in uptime, so they will often be smaller than the elapsed time,
        // but if the user taps on the bar we want to show the times to them.  It is confusing
        // to see them be smaller than what we told them the measured duration is, so just
        // scaling them up with make things look reasonable with them none the wiser.
        for (Int32 i=0; i<IProcessStats::ADJ_MEM_FACTOR_COUNT; i++) {
            mMemTimes[i] = (Int64)((mMemTimes[i]*(Double)elapsedTime)/mTotalTime);
        }

        ProcessStats.TotalMemoryUseCollection totalMem = new ProcessStats->TotalMemoryUseCollection(
                IProcessStats::ALL_SCREEN_ADJ, memStates);
        mStats->ComputeTotalMemoryUse(totalMem, now);
        Double freeWeight = totalMem.sysMemFreeWeight + totalMem.sysMemCachedWeight;
        Double usedWeight = totalMem.sysMemKernelWeight + totalMem.sysMemNativeWeight
                + totalMem.sysMemZRamWeight;
        Double backgroundWeight = 0, persBackgroundWeight = 0;
        mMemCachedWeight = totalMem.sysMemCachedWeight;
        mMemFreeWeight = totalMem.sysMemFreeWeight;
        mMemZRamWeight = totalMem.sysMemZRamWeight;
        mMemKernelWeight = totalMem.sysMemKernelWeight;
        mMemNativeWeight = totalMem.sysMemNativeWeight;
        for (Int32 i=0; i<IProcessStats::STATE_COUNT; i++) {
            if (i == IProcessStats::STATE_SERVICE_RESTARTING) {
                // These don't really run.
                mMemStateWeights[i] = 0;
            } else {
                mMemStateWeights[i] = totalMem.processStateWeight[i];
                if (i >= IProcessStats::STATE_HOME) {
                    freeWeight += totalMem.processStateWeight[i];
                } else {
                    usedWeight += totalMem.processStateWeight[i];
                }
                if (i >= IProcessStats::STATE_IMPORTANT_FOREGROUND) {
                    backgroundWeight += totalMem.processStateWeight[i];
                    persBackgroundWeight += totalMem.processStateWeight[i];
                }
                if (i == IProcessStats::STATE_PERSISTENT) {
                    persBackgroundWeight += totalMem.processStateWeight[i];
                }
            }
        }
        if (DEBUG) {
            Logger::I(TAG, "Used RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)((usedWeight * 1024) / memTotalTime)));
            Logger::I(TAG, "Free RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)((freeWeight * 1024) / memTotalTime)));
            Logger::I(TAG, "Total RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)(((freeWeight+usedWeight) * 1024) / memTotalTime)));
            Logger::I(TAG, "Background+Cached RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)((backgroundWeight * 1024) / memTotalTime)));
        }
        mMemTotalWeight = freeWeight + usedWeight;

        // For computing the ratio to show, we want to count the baseline cached RAM we
        // need (at which point we start killing processes) as used RAM, so that if we
        // reach the point of thrashing due to no RAM for any background processes we
        // report that as RAM being full.  To do this, we need to first convert the weights
        // back to actual RAM...  and since the RAM values we compute here won't exactly
        // match the real physical RAM, scale those to the actual physical RAM.  No problem!
        Double usedRam = (usedWeight*1024)/memTotalTime;
        Double freeRam = (freeWeight*1024)/memTotalTime;
        Double totalRam = usedRam + freeRam;
        MemInfoReader memReader = new MemInfoReader();
        memReader->ReadMemInfo();
        Double realTotalRam = memReader->GetTotalSize();
        Double totalScale = realTotalRam / totalRam;
        Double realUsedRam = usedRam * totalScale;
        Double realFreeRam = freeRam * totalScale;
        if (DEBUG) {
            Logger::I(TAG, "Scaled Used RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)realUsedRam));
            Logger::I(TAG, "Scaled Free RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)realFreeRam));
        }
        ActivityManager.MemoryInfo memInfo = new ActivityManager->MemoryInfo();
        ((ActivityManager)GetActivity()->GetSystemService(Context.ACTIVITY_SERVICE)).GetMemoryInfo(
                memInfo);
        if (memInfo.hiddenAppThreshold >= realFreeRam) {
            realUsedRam = realFreeRam;
            realFreeRam = 0;
        } else {
            realUsedRam += memInfo.hiddenAppThreshold;
            realFreeRam -= memInfo.hiddenAppThreshold;
        }
        if (DEBUG) {
            Logger::I(TAG, "Adj Scaled Used RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)realUsedRam));
            Logger::I(TAG, "Adj Scaled Free RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)realFreeRam));
        }

        Float usedRatio = (Float)(realUsedRam/(realFreeRam+realUsedRam));
        colors->SetRatios(usedRatio, 0, 1-usedRatio);

        if (FALSE) {
            colors->SetOnRegionTappedListener(this);
            switch (mMemRegion) {
                case LinearColorBar.REGION_RED:
                    colors->SetColoredRegions(LinearColorBar.REGION_RED);
                    memTotalTime = mMemTimes[IProcessStats::ADJ_MEM_FACTOR_CRITICAL];
                    memStates = RED_MEM_STATES;
                    break;
                case LinearColorBar.REGION_YELLOW:
                    colors->SetColoredRegions(LinearColorBar.REGION_RED
                            | LinearColorBar.REGION_YELLOW);
                    memTotalTime = mMemTimes[IProcessStats::ADJ_MEM_FACTOR_CRITICAL]
                            + mMemTimes[IProcessStats::ADJ_MEM_FACTOR_LOW]
                            + mMemTimes[IProcessStats::ADJ_MEM_FACTOR_MODERATE];
                    memStates = YELLOW_MEM_STATES;
                    break;
                default:
                    colors->SetColoredRegions(LinearColorBar.REGION_ALL);
                    memTotalTime = mTotalTime;
                    memStates = IProcessStats::ALL_MEM_ADJ;
                    break;
            }
            colors->SetRatios(mMemTimes[IProcessStats::ADJ_MEM_FACTOR_CRITICAL] / (Float)mTotalTime,
                    (mMemTimes[IProcessStats::ADJ_MEM_FACTOR_LOW]
                            + mMemTimes[IProcessStats::ADJ_MEM_FACTOR_MODERATE]) / (Float)mTotalTime,
                    mMemTimes[IProcessStats::ADJ_MEM_FACTOR_NORMAL] / (Float)mTotalTime);
        }

        mAppListGroup->AddPreference(colors);

        ProcessStats.ProcessDataCollection totals = new ProcessStats->ProcessDataCollection(
                IProcessStats::ALL_SCREEN_ADJ, memStates, stats);

        ArrayList<ProcStatsEntry> entries = new ArrayList<ProcStatsEntry>();

        /*
        ArrayList<ProcessStats.ProcessState> rawProcs = mStats->CollectProcessesLocked(
                ProcessStats.ALL_SCREEN_ADJ, ProcessStats.ALL_MEM_ADJ,
                ProcessStats.BACKGROUND_PROC_STATES, now, NULL);
        for (Int32 i=0, N=(rawProcs != NULL ? rawProcs->Size() : 0); i<N; i++) {
            procs->Add(new ProcStatsEntry(rawProcs->Get(i), totals));
        }
        */

        if (DEBUG) Logger::D(TAG, "-------------------- PULLING PROCESSES");

        final ProcessMap<ProcStatsEntry> entriesMap = new ProcessMap<ProcStatsEntry>();
        for (Int32 ipkg=0, N=mStats.mPackages->GetMap()->Size(); ipkg<N; ipkg++) {
            final SparseArray<SparseArray<ProcessStats.PackageState>> pkgUids
                    = mStats.mPackages->GetMap()->ValueAt(ipkg);
            for (Int32 iu=0; iu<pkgUids->Size(); iu++) {
                final SparseArray<ProcessStats.PackageState> vpkgs = pkgUids->ValueAt(iu);
                for (Int32 iv=0; iv<vpkgs->Size(); iv++) {
                    final ProcessStats.PackageState st = vpkgs->ValueAt(iv);
                    for (Int32 iproc=0; iproc<st.mProcesses->Size(); iproc++) {
                        final ProcessStats.ProcessState pkgProc = st.mProcesses->ValueAt(iproc);
                        final ProcessStats.ProcessState proc = mStats.mProcesses->Get(pkgProc.mName,
                                pkgProc.mUid);
                        if (proc == NULL) {
                            Logger::W(TAG, "No process found for pkg " + st.mPackageName
                                    + "/" + st.mUid + " proc name " + pkgProc.mName);
                            continue;
                        }
                        ProcStatsEntry ent = entriesMap->Get(proc.mName, proc.mUid);
                        if (ent == NULL) {
                            ent = new ProcStatsEntry(proc, st.mPackageName, totals, mUseUss,
                                    mStatsType == MENU_TYPE_BACKGROUND);
                            if (ent.mDuration > 0) {
                                if (DEBUG) Logger::D(TAG, "Adding proc " + proc.mName + "/"
                                        + proc.mUid + ": time=" + MakeDuration(ent.mDuration) + " ("
                                        + ((((Double)ent.mDuration) / memTotalTime) * 100) + "%)"
                                        + " pss=" + ent.mAvgPss);
                                entriesMap->Put(proc.mName, proc.mUid, ent);
                                entries->Add(ent);
                            }
                        }  else {
                            ent->AddPackage(st.mPackageName);
                        }
                    }
                }
            }
        }

        if (DEBUG) Logger::D(TAG, "-------------------- MAPPING SERVICES");

        // Add in service info.
        if (mStatsType == MENU_TYPE_BACKGROUND) {
            for (Int32 ip=0, N=mStats.mPackages->GetMap()->Size(); ip<N; ip++) {
                SparseArray<SparseArray<ProcessStats.PackageState>> uids
                        = mStats.mPackages->GetMap()->ValueAt(ip);
                for (Int32 iu=0; iu<uids->Size(); iu++) {
                    SparseArray<ProcessStats.PackageState> vpkgs = uids->ValueAt(iu);
                    for (Int32 iv=0; iv<vpkgs->Size(); iv++) {
                        ProcessStats.PackageState ps = vpkgs->ValueAt(iv);
                        for (Int32 is=0, NS=ps.mServices->Size(); is<NS; is++) {
                            ProcessStats.ServiceState ss = ps.mServices->ValueAt(is);
                            if (ss.mProcessName != NULL) {
                                ProcStatsEntry ent = entriesMap->Get(ss.mProcessName, uids->KeyAt(iu));
                                if (ent != NULL) {
                                    if (DEBUG) Logger::D(TAG, "Adding service " + ps.mPackageName
                                            + "/" + ss.mName + "/" + uids->KeyAt(iu) + " to proc "
                                            + ss.mProcessName);
                                    ent->AddService(ss);
                                } else {
                                    Logger::W(TAG, "No process " + ss.mProcessName + "/" + uids->KeyAt(iu)
                                            + " for service " + ss.mName);
                                }
                            }
                        }
                    }
                }
            }
        }

        /*
        SparseArray<ArrayMap<String, ProcStatsEntry>> processes
                = new SparseArray<ArrayMap<String, ProcStatsEntry>>();
        for (Int32 ip=0, N=mStats.mProcesses->GetMap()->Size(); ip<N; ip++) {
            SparseArray<ProcessStats.ProcessState> uids = mStats.mProcesses->GetMap()->ValueAt(ip);
            for (Int32 iu=0; iu<uids->Size(); iu++) {
                ProcessStats.ProcessState st = uids->ValueAt(iu);
                ProcStatsEntry ent = new ProcStatsEntry(st, totals, mUseUss,
                        mStatsType == MENU_TYPE_BACKGROUND);
                if (ent.mDuration > 0) {
                    if (DEBUG) Logger::D(TAG, "Adding proc " + st.mName + "/" + st.mUid + ": time="
                            + MakeDuration(ent.mDuration) + " ("
                            + ((((Double)ent.mDuration) / memTotalTime) * 100) + "%)");
                    procs->Add(ent);
                    ArrayMap<String, ProcStatsEntry> uidProcs = processes->Get(ent.mUid);
                    if (uidProcs == NULL) {
                        uidProcs = new ArrayMap<String, ProcStatsEntry>();
                        processes->Put(ent.mUid, uidProcs);
                    }
                    uidProcs->Put(ent.mName, ent);
                }
            }
        }
        */

        Collections->Sort(entries, sEntryCompare);

        Int64 maxWeight = 1;
        for (Int32 i=0, N=(entries != NULL ? entries->Size() : 0); i<N; i++) {
            ProcStatsEntry proc = entries->Get(i);
            if (maxWeight < proc.mWeight) {
                maxWeight = proc.mWeight;
            }
        }
        if (mStatsType == MENU_TYPE_BACKGROUND) {
            mMaxWeight = (Int64)(mShowSystem ? persBackgroundWeight : backgroundWeight);
            if (mMaxWeight < maxWeight) {
                mMaxWeight = maxWeight;
            }
            if (DEBUG) {
                Logger::I(TAG, "Bar max RAM: " + Formatter->FormatShortFileSize(GetActivity(),
                        (mMaxWeight * 1024) / memTotalTime));
            }
        } else {
            mMaxWeight = maxWeight;
        }

        if (DEBUG) Logger::D(TAG, "-------------------- BUILDING UI");

        // Find where we should stop.  Because we have two properties we are looking at,
        // we need to go from the back looking for the first place either holds.
        Int32 end = entries != NULL ? entries->Size()-1 : -1;
        while (end >= 0) {
            ProcStatsEntry proc = entries->Get(end);
            final Double percentOfWeight = (((Double)proc.mWeight) / mMaxWeight) * 100;
            final Double percentOfTime = (((Double)proc.mDuration) / memTotalTime) * 100;
            if (percentOfWeight >= 1 || percentOfTime >= 25) {
                break;
            }
            end--;
        }
        for (Int32 i=0; i<=end; i++) {
            ProcStatsEntry proc = entries->Get(i);
            final Double percentOfWeight = (((Double)proc.mWeight) / mMaxWeight) * 100;
            final Double percentOfTime = (((Double)proc.mDuration) / memTotalTime) * 100;
            ProcessStatsPreference pref = new ProcessStatsPreference(GetActivity());
            pref->Init(NULL, proc);
            proc->EvaluateTargetPackage(pm, mStats, totals, sEntryCompare, mUseUss,
                    mStatsType == MENU_TYPE_BACKGROUND);
            proc->RetrieveUiData(pm);
            pref->SetTitle(proc.mUiLabel);
            if (proc.mUiTargetApp != NULL) {
                pref->SetIcon(proc.mUiTargetApp->LoadIcon(pm));
            }
            pref->SetOrder(i);
            pref->SetPercent(percentOfWeight, percentOfTime);
            mAppListGroup->AddPreference(pref);
            if (mStatsType == MENU_TYPE_BACKGROUND) {
                if (DEBUG) {
                    Logger::I(TAG, "App " + proc.mUiLabel + ": weightedRam="
                            + Formatter->FormatShortFileSize(GetActivity(),
                                    (proc.mWeight * 1024) / memTotalTime)
                            + ", avgRam=" + Formatter->FormatShortFileSize(GetActivity(),
                                    (proc.mAvgPss*1024)));
                }

            }
            if (mAppListGroup->GetPreferenceCount() > (MAX_ITEMS_TO_LIST+1)) {
                if (DEBUG) Logger::D(TAG, "Done with UI, hit item limit!");
                break;
            }
        }
    }

    private void Load() {
        try {
            mLastDuration = mDuration;
            mMemState = mProcessStats->GetCurrentMemoryState();
            ParcelFileDescriptor pfd = mProcessStats->GetStatsOverTime(mDuration);
            mStats = new ProcessStats(FALSE);
            InputStream is = new ParcelFileDescriptor->AutoCloseInputStream(pfd);
            mStats->Read(is);
            try {
                is->Close();
            } catch (IOException e) {
            }
            if (mStats.mReadError != NULL) {
                Logger::W(TAG, "Failure reading process stats: " + mStats.mReadError);
            }
        } catch (RemoteException e) {
            Logger::E(TAG, "RemoteException:", e);
        }
    }
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSUI_H__