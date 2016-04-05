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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

/**
 * Custom preference for displaying power consumption as a bar and an icon on
 * the left for the subsystem/app type.
 */
public class PowerGaugePreference extends Preference {
    private BatteryEntry mInfo;
    private Int32 mProgress;
    private CharSequence mProgressText;
    private final CharSequence mContentDescription;

    public PowerGaugePreference(Context context, Drawable icon, CharSequence contentDescription,
            BatteryEntry info) {
        Super(context);
        SetLayoutResource(R.layout.preference_app_percentage);
        SetIcon(icon != NULL ? icon : new ColorDrawable(0));
        mInfo = info;
        mContentDescription = contentDescription;
    }

    CARAPI SetPercent(Double percentOfMax, Double percentOfTotal) {
        mProgress = (Int32) Math->Ceil(percentOfMax);
        mProgressText = Utils->FormatPercentage((Int32) (percentOfTotal + 0.5));
        NotifyChanged();
    }

    BatteryEntry GetInfo() {
        return mInfo;
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        final ProgressBar progress = (ProgressBar) view->FindViewById(android.R.id.progress);
        progress->SetProgress(mProgress);

        final TextView text1 = (TextView) view->FindViewById(android.R.id.text1);
        text1->SetText(mProgressText);

        if (mContentDescription != NULL) {
            final TextView titleView = (TextView) view->FindViewById(android.R.id.title);
            titleView->SetContentDescription(mContentDescription);
        }
    }
}
