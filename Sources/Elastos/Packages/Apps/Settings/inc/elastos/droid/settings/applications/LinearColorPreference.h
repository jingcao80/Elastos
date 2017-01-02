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
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::Settings::IR;

public class LinearColorPreference extends Preference {
    Float mRedRatio;
    Float mYellowRatio;
    Float mGreenRatio;
    Int32 mRedColor = 0xffaa5030;
    Int32 mYellowColor = 0xffaaaa30;
    Int32 mGreenColor = 0xff30aa50;
    Int32 mColoredRegions = LinearColorBar.REGION_ALL;
    LinearColorBar.OnRegionTappedListener mOnRegionTappedListener;

    public LinearColorPreference(Context context) {
        Super(context);
        SetLayoutResource(R.layout.preference_linearcolor);
    }

    CARAPI SetRatios(Float red, Float yellow, Float green) {
        mRedRatio = red;
        mYellowRatio = yellow;
        mGreenRatio = green;
        NotifyChanged();
    }

    CARAPI SetColors(Int32 red, Int32 yellow, Int32 green) {
        mRedColor = red;
        mYellowColor = yellow;
        mGreenColor = green;
        NotifyChanged();
    }

    CARAPI SetOnRegionTappedListener(LinearColorBar.OnRegionTappedListener listener) {
        mOnRegionTappedListener = listener;
        NotifyChanged();
    }

    CARAPI SetColoredRegions(Int32 regions) {
        mColoredRegions = regions;
        NotifyChanged();
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);

        LinearColorBar colors = (LinearColorBar)view->FindViewById(
                R.id.linear_color_bar);
        colors->SetShowIndicator(FALSE);
        colors->SetColors(mRedColor, mYellowColor, mGreenColor);
        colors->SetRatios(mRedRatio, mYellowRatio, mGreenRatio);
        colors->SetColoredRegions(mColoredRegions);
        colors->SetOnRegionTappedListener(mOnRegionTappedListener);
    }
}
