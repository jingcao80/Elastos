
#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CANIMATEDIMAGEVIEW_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CANIMATEDIMAGEVIEW_H__

#include "_Elastos_Droid_Settings_Widget_CAnimatedImageView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IAnimatedRotateDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CAnimatedImageView)
    , public ImageView
{
public:
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
        ImageView::SetImageDrawable(drawable);
        UpdateDrawable();
    }

    //@Override
    CARAPI SetImageResource(Int32 resid) {
        ImageView::SetImageResource(resid);
        UpdateDrawable();
    }

    //@Override
    CARAPI OnAttachedToWindow() {
        ImageView::OnAttachedToWindow();
        UpdateAnimating();
    }

    //@Override
    CARAPI OnDetachedFromWindow() {
        ImageView::OnDetachedFromWindow();
        UpdateAnimating();
    }

    CARAPI SetAnimating(Boolean animating) {
        mAnimating = animating;
        UpdateAnimating();
    }

    //@Override
    protected void OnVisibilityChanged(View changedView, Int32 vis) {
        ImageView::OnVisibilityChanged(changedView, vis);
        UpdateAnimating();
    }
private:
    private AnimatedRotateDrawable mDrawable;
    private Boolean mAnimating;
};

}// namespace Widget
}// namespace Settings
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CANIMATEDIMAGEVIEW_H__