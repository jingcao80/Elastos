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

package com.android.settings.deviceinfo;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

using Elastos::Droid::Settings::IR;
using com::Google::Android::Collect::ILists;

using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

/**
 * Creates a percentage bar chart inside a preference.
 */
public class UsageBarPreference extends Preference {
    private PercentageBarChart mChart = NULL;

    private final List<PercentageBarChart.Entry> mEntries = Lists->NewArrayList();

    public UsageBarPreference(Context context) {
        This(context, NULL);
    }

    public UsageBarPreference(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public UsageBarPreference(Context context, AttributeSet attrs, Int32 defStyle) {
        This(context, attrs, defStyle, 0);
    }

    public UsageBarPreference(Context context, AttributeSet attrs, Int32 defStyleAttr,
            Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
        SetLayoutResource(R.layout.preference_memoryusage);
    }

    CARAPI AddEntry(Int32 order, Float percentage, Int32 color) {
        mEntries->Add(PercentageBarChart->CreateEntry(order, percentage, color));
        Collections->Sort(mEntries);
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        mChart = (PercentageBarChart) view->FindViewById(R.id.percentage_bar_chart);
        mChart->SetEntries(mEntries);
    }

    CARAPI Commit() {
        if (mChart != NULL) {
            mChart->Invalidate();
        }
    }

    CARAPI Clear() {
        mEntries->Clear();
    }
}
