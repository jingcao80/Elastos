/*
 * Copyright (C) 2010 The Android Open Source Project
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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * Custom preference for displaying power consumption as a bar and an icon on the left for the
 * subsystem/app type.
 *
 */
public class BatteryHistoryPreference extends Preference {

    final private BatteryStats mStats;
    final private Intent mBatteryBroadcast;

    private Boolean mHideLabels;
    private View mLabelHeader;
    private BatteryHistoryChart mChart;

    public BatteryHistoryPreference(Context context, BatteryStats stats, Intent batteryBroadcast) {
        Super(context);
        SetLayoutResource(R.layout.preference_batteryhistory);
        mStats = stats;
        mBatteryBroadcast = batteryBroadcast;
    }

    BatteryStats GetStats() {
        return mStats;
    }

    CARAPI SetHideLabels(Boolean hide) {
        if (mHideLabels != hide) {
            mHideLabels = hide;
            if (mLabelHeader != NULL) {
                mLabelHeader->SetVisibility(hide ? View.GONE : View.VISIBLE);
            }
        }
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        BatteryHistoryChart chart = (BatteryHistoryChart)view->FindViewById(
                R.id.battery_history_chart);
        if (mChart == NULL) {
            // First time: use and initialize this chart.
            chart->SetStats(mStats, mBatteryBroadcast);
            mChart = chart;
        } else {
            // All future times: forget the newly inflated chart, re-use the
            // already initialized chart from last time.
            ViewGroup parent = (ViewGroup)chart->GetParent();
            Int32 index = parent->IndexOfChild(chart);
            parent->RemoveViewAt(index);
            if (mChart->GetParent() != NULL) {
                ((ViewGroup)mChart->GetParent()).RemoveView(mChart);
            }
            parent->AddView(mChart, index);
        }
        mLabelHeader = view->FindViewById(R.id.labelsHeader);
        mLabelHeader->SetVisibility(mHideLabels ? View.GONE : View.VISIBLE);
    }
}
