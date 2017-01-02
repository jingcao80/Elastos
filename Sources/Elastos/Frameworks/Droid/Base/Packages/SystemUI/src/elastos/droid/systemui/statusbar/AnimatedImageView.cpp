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

#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"

using Elastos::Droid::Graphics::Drawable::IAnimatable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL(AnimatedImageView, ImageView, IAnimatedImageView)
AnimatedImageView::AnimatedImageView()
    : mAttached(FALSE)
{}

ECode AnimatedImageView::constructor(
    /* [in] */ IContext* context)
{
    return ImageView::constructor(context);
}

ECode AnimatedImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageView::constructor(context, attrs);
}

ECode AnimatedImageView::SetImageResource(
    /* [in] */ Int32 resId)
{
    ImageView::SetImageResource(resId);
    UpdateAnim();
    return NOERROR;
}

ECode AnimatedImageView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    ImageView::SetImageDrawable(drawable);
    UpdateAnim();
    return NOERROR;
}

void AnimatedImageView::UpdateAnim()
{
    AutoPtr<IDrawable> drawable;
    GetDrawable((IDrawable**)&drawable);

    if (mAttached && mAnim != NULL) {
        IAnimatable::Probe(mAnim)->Stop();
    }

    if (IAnimationDrawable::Probe(drawable)) {
        mAnim = IAnimationDrawable::Probe(drawable);
        Boolean shown = FALSE;
        if (IsShown(&shown), shown) {
            IAnimatable::Probe(mAnim)->Start();
        }
    }
    else {
        mAnim = NULL;
    }
}

ECode AnimatedImageView::OnAttachedToWindow()
{
    ImageView::OnAttachedToWindow();
    mAttached = TRUE;
    return NOERROR;
}

ECode AnimatedImageView::OnDetachedFromWindow()
{
    ImageView::OnDetachedFromWindow();
    if (mAnim != NULL) {
        IAnimatable::Probe(mAnim)->Stop();
    }
    mAttached = FALSE;
    return NOERROR;
}

ECode AnimatedImageView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 vis)
{
    ImageView::OnVisibilityChanged(changedView, vis);
    if (mAnim != NULL) {
        Boolean shown = FALSE;
        if (IsShown(&shown), shown) {
            IAnimatable::Probe(mAnim)->Start();
        }
        else {
            IAnimatable::Probe(mAnim)->Stop();
        }
    }
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
