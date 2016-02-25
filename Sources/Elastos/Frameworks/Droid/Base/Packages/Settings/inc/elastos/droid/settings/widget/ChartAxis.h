

package com.android.settings.widget;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::ISpannableStringBuilder;

/**
 * Axis along a {@link ChartView} that knows how to convert between raw point
 * and screen coordinate systems.
 */
public interface ChartAxis {

    /** Set range of raw values this axis should cover. */
    public Boolean SetBounds(Int64 min, Int64 max);
    /** Set range of screen points this axis should cover. */
    public Boolean SetSize(Float size);

    /** Convert raw value into screen point. */
    public Float ConvertToPoint(Int64 value);
    /** Convert screen point into raw value. */
    public Int64 ConvertToValue(Float point);

    /**
     * Build label that describes given raw value. If the label is rounded for
     * display, return the rounded value.
     */
    public Int64 BuildLabel(Resources res, SpannableStringBuilder builder, Int64 value);

    /** Return list of tick points for drawing a grid. */
    public Float[] GetTickPoints();

    /**
     * Test if given raw value should cause the axis to grow or shrink;
     * returning positive value to grow and negative to shrink.
     */
    public Int32 ShouldAdjustAxis(Int64 value);

}
