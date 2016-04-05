/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.applications;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

public class ProcessStatsPreference extends Preference {
    private ProcStatsEntry mEntry;
    private Int32 mProgress;
    private CharSequence mProgressText;

    public ProcessStatsPreference(Context context) {
        This(context, NULL);
    }

    public ProcessStatsPreference(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ProcessStatsPreference(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public ProcessStatsPreference(Context context, AttributeSet attrs, Int32 defStyleAttr,
            Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
        SetLayoutResource(R.layout.preference_app_percentage);
    }

    CARAPI Init(Drawable icon, ProcStatsEntry entry) {
        mEntry = entry;
        SetIcon(icon != NULL ? icon : new ColorDrawable(0));
    }

    public ProcStatsEntry GetEntry() {
        return mEntry;
    }

    CARAPI SetPercent(Double percentOfWeight, Double percentOfTime) {
        mProgress = (Int32) Math->Ceil(percentOfWeight);
        mProgressText = Utils->FormatPercentage((Int32) percentOfTime);
        NotifyChanged();
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        final ProgressBar progress = (ProgressBar) view->FindViewById(android.R.id.progress);
        progress->SetProgress(mProgress);

        final TextView text1 = (TextView) view->FindViewById(android.R.id.text1);
        text1->SetText(mProgressText);
    }
}
