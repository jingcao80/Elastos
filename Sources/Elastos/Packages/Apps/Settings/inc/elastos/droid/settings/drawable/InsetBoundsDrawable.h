

package com.android.settings.drawable;

using Elastos::Droid::Graphics::Drawable::IDrawable;

/**
 * Wrapper around another {@link Drawable} that insets requested bounds by a
 * specific amount.
 */
public class InsetBoundsDrawable extends DrawableWrapper {
    private final Int32 mInsetBoundsSides;

    public InsetBoundsDrawable(Drawable drawable, Int32 insetBoundsSides) {
        Super(drawable);
        mInsetBoundsSides = insetBoundsSides;
    }

    //@Override
    CARAPI SetBounds(Int32 left, Int32 top, Int32 right, Int32 bottom) {
        super->SetBounds(left + mInsetBoundsSides, top, right - mInsetBoundsSides, bottom);
    }
}
