

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
