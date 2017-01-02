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