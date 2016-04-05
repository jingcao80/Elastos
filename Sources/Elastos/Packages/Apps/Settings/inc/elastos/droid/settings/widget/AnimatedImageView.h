

package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IAnimatedRotateDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;

public class AnimatedImageView extends ImageView {
    private AnimatedRotateDrawable mDrawable;
    private Boolean mAnimating;

    public AnimatedImageView(Context context) {
        Super(context);
    }

    public AnimatedImageView(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    private void UpdateDrawable() {
        if (IsShown() && mDrawable != NULL) {
            mDrawable->Stop();
        }
        final Drawable drawable = GetDrawable();
        if (drawable instanceof AnimatedRotateDrawable) {
            mDrawable = (AnimatedRotateDrawable) drawable;
            // TODO: define in drawable xml once we have public attrs.
            mDrawable->SetFramesCount(56);
            mDrawable->SetFramesDuration(32);
            if (IsShown() && mAnimating) {
                mDrawable->Start();
            }
        } else {
            mDrawable = NULL;
        }
    }

    private void UpdateAnimating() {
        if (mDrawable != NULL) {
            if (IsShown() && mAnimating) {
                mDrawable->Start();
            } else {
                mDrawable->Stop();
            }
        }
    }

    //@Override
    CARAPI SetImageDrawable(Drawable drawable) {
        super->SetImageDrawable(drawable);
        UpdateDrawable();
    }

    //@Override
    CARAPI SetImageResource(Int32 resid) {
        super->SetImageResource(resid);
        UpdateDrawable();
    }

    //@Override
    CARAPI OnAttachedToWindow() {
        super->OnAttachedToWindow();
        UpdateAnimating();
    }

    //@Override
    CARAPI OnDetachedFromWindow() {
        super->OnDetachedFromWindow();
        UpdateAnimating();
    }

    CARAPI SetAnimating(Boolean animating) {
        mAnimating = animating;
        UpdateAnimating();
    }

    //@Override
    protected void OnVisibilityChanged(View changedView, Int32 vis) {
        super->OnVisibilityChanged(changedView, vis);
        UpdateAnimating();
    }
}
