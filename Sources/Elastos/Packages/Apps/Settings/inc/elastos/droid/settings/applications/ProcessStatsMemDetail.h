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

package com.android.settings.applications;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Internal::App::IProcessStats;
using Elastos::Droid::Settings::IR;

using static com::Android::Settings::Utils::IprepareCustomPreferencesList;

public class ProcessStatsMemDetail extends Fragment {
    public static const String EXTRA_MEM_TIMES = "mem_times";
    public static const String EXTRA_MEM_STATE_WEIGHTS = "mem_state_weights";
    public static const String EXTRA_MEM_CACHED_WEIGHT = "mem_cached_weight";
    public static const String EXTRA_MEM_FREE_WEIGHT = "mem_free_weight";
    public static const String EXTRA_MEM_ZRAM_WEIGHT = "mem_zram_weight";
    public static const String EXTRA_MEM_KERNEL_WEIGHT = "mem_kernel_weight";
    public static const String EXTRA_MEM_NATIVE_WEIGHT = "mem_native_weight";
    public static const String EXTRA_MEM_TOTAL_WEIGHT = "mem_total_weight";
    public static const String EXTRA_USE_USS = "use_uss";
    public static const String EXTRA_TOTAL_TIME = "total_time";

    Int64[] mMemTimes;
    Double[] mMemStateWeights;
    Double mMemCachedWeight;
    Double mMemFreeWeight;
    Double mMemZRamWeight;
    Double mMemKernelWeight;
    Double mMemNativeWeight;
    Double mMemTotalWeight;
    Boolean mUseUss;
    Int64 mTotalTime;

    private View mRootView;
    private ViewGroup mMemStateParent;
    private ViewGroup mMemUseParent;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        final Bundle args = GetArguments();
        mMemTimes = args->GetLongArray(EXTRA_MEM_TIMES);
        mMemStateWeights = args->GetDoubleArray(EXTRA_MEM_STATE_WEIGHTS);
        mMemCachedWeight = args->GetDouble(EXTRA_MEM_CACHED_WEIGHT);
        mMemFreeWeight = args->GetDouble(EXTRA_MEM_FREE_WEIGHT);
        mMemZRamWeight = args->GetDouble(EXTRA_MEM_ZRAM_WEIGHT);
        mMemKernelWeight = args->GetDouble(EXTRA_MEM_KERNEL_WEIGHT);
        mMemNativeWeight = args->GetDouble(EXTRA_MEM_NATIVE_WEIGHT);
        mMemTotalWeight = args->GetDouble(EXTRA_MEM_TOTAL_WEIGHT);
        mUseUss = args->GetBoolean(EXTRA_USE_USS);
        mTotalTime = args->GetLong(EXTRA_TOTAL_TIME);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.process_stats_mem_details, container, FALSE);
        PrepareCustomPreferencesList(container, view, view, FALSE);

        mRootView = view;
        CreateDetails();
        return view;
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
    }

    private void CreateDetails() {
        mMemStateParent = (ViewGroup)mRootView->FindViewById(R.id.mem_state);
        mMemUseParent = (ViewGroup)mRootView->FindViewById(R.id.mem_use);

        FillMemStateSection();
        FillMemUseSection();
    }

    private void AddDetailsItem(ViewGroup parent, CharSequence title,
            Float level, CharSequence value) {
        LayoutInflater inflater = GetActivity()->GetLayoutInflater();
        ViewGroup item = (ViewGroup) inflater->Inflate(R.layout.app_percentage_item,
                NULL);
        parent->AddView(item);
        item->FindViewById(android.R.id.icon).SetVisibility(View.GONE);
        TextView titleView = (TextView) item->FindViewById(android.R.id.title);
        TextView valueView = (TextView) item->FindViewById(android.R.id.text1);
        titleView->SetText(title);
        valueView->SetText(value);
        ProgressBar progress = (ProgressBar) item->FindViewById(android.R.id.progress);
        progress->SetProgress(Math->Round(level*100));
    }

    private void FillMemStateSection() {
        CharSequence[] labels = GetResources()->GetTextArray(R.array.proc_stats_memory_states);
        for (Int32 i=0; i<ProcessStats.ADJ_MEM_FACTOR_COUNT; i++) {
            if (mMemTimes[i] > 0) {
                Float level = ((Float)mMemTimes[i])/mTotalTime;
                AddDetailsItem(mMemStateParent, labels[i], level,
                        Formatter->FormatShortElapsedTime(GetActivity(), mMemTimes[i]));
            }
        }
    }

    private void AddMemUseDetailsItem(ViewGroup parent, CharSequence title, Double weight) {
        if (weight > 0) {
            Float level = (Float)(weight/mMemTotalWeight);
            String value = Formatter->FormatShortFileSize(GetActivity(),
                    (Int64)((weight * 1024) / mTotalTime));
            AddDetailsItem(parent, title, level, value);
        }
    }

    private void FillMemUseSection() {
        CharSequence[] labels = GetResources()->GetTextArray(R.array.proc_stats_process_states);
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_kernel_type), mMemKernelWeight);
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_zram_type), mMemZRamWeight);
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_native_type), mMemNativeWeight);
        for (Int32 i=0; i<ProcessStats.STATE_COUNT; i++) {
            AddMemUseDetailsItem(mMemUseParent, labels[i], mMemStateWeights[i]);
        }
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_kernel_cache_type), mMemCachedWeight);
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_free_type), mMemFreeWeight);
        AddMemUseDetailsItem(mMemUseParent,
                GetResources()->GetText(R::string::mem_use_total), mMemTotalWeight);
    }
}
