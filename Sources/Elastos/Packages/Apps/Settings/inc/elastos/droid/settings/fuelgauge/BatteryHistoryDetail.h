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

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Settings::IR;

public class BatteryHistoryDetail extends Fragment {
    public static const String EXTRA_STATS = "stats";
    public static const String EXTRA_BROADCAST = "broadcast";

    private BatteryStats mStats;
    private Intent mBatteryBroadcast;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        String histFile = GetArguments()->GetString(EXTRA_STATS);
        mStats = BatteryStatsHelper->StatsFromFile(GetActivity(), histFile);
        mBatteryBroadcast = GetArguments()->GetParcelable(EXTRA_BROADCAST);
    }
    
    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater->Inflate(R.layout.battery_history_chart, NULL);
        BatteryHistoryChart chart = (BatteryHistoryChart)view->FindViewById(
                R.id.battery_history_chart);
        chart->SetStats(mStats, mBatteryBroadcast);
        return view;
    }
}
