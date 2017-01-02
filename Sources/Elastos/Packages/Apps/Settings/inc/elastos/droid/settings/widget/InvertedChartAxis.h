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

package com.android.settings.widget;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::ISpannableStringBuilder;

/**
 * Utility to invert another {@link ChartAxis}.
 */
public class InvertedChartAxis implements ChartAxis {
    private final ChartAxis mWrapped;
    private Float mSize;

    public InvertedChartAxis(ChartAxis wrapped) {
        mWrapped = wrapped;
    }

    //@Override
    public Boolean SetBounds(Int64 min, Int64 max) {
        return mWrapped->SetBounds(min, max);
    }

    //@Override
    public Boolean SetSize(Float size) {
        mSize = size;
        return mWrapped->SetSize(size);
    }

    //@Override
    public Float ConvertToPoint(Int64 value) {
        return mSize - mWrapped->ConvertToPoint(value);
    }

    //@Override
    public Int64 ConvertToValue(Float point) {
        return mWrapped->ConvertToValue(mSize - point);
    }

    //@Override
    public Int64 BuildLabel(Resources res, SpannableStringBuilder builder, Int64 value) {
        return mWrapped->BuildLabel(res, builder, value);
    }

    //@Override
    public Float[] GetTickPoints() {
        final Float[] points = mWrapped->GetTickPoints();
        for (Int32 i = 0; i < points.length; i++) {
            points[i] = mSize - points[i];
        }
        return points;
    }

    //@Override
    public Int32 ShouldAdjustAxis(Int64 value) {
        return mWrapped->ShouldAdjustAxis(value);
    }
}
