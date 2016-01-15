#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"

using Elastos::Droid::Graphics::Drawable::IAnimatable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

AnimatedImageView::AnimatedImageView()
    : mAttached(FALSE)
{}

AnimatedImageView::AnimatedImageView(
    /* [in] */ IContext* context)
    : ImageView(context)
    , mAttached(FALSE)
{}

AnimatedImageView::AnimatedImageView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : ImageView(context, attrs)
    , mAttached(FALSE)
{}

ECode AnimatedImageView::SetImageResource(
    /* [in] */ Int32 resId)
{
    ECode ec = ImageView::SetImageResource(resId);
    UpdateAnim();
    return ec;
}

ECode AnimatedImageView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    ECode ec = ImageView::SetImageDrawable(drawable);
    UpdateAnim();
    return ec;
}

void AnimatedImageView::UpdateAnim()
{
    AutoPtr<IDrawable> drawable = GetDrawable();

    if (mAttached && mAnim != NULL) {
        AutoPtr<IAnimatable> anim = IAnimatable::Probe(mAnim);
        if (anim) anim->Stop();
    }

    if (IAnimationDrawable::Probe(drawable)) {
        mAnim = IAnimationDrawable::Probe(drawable);
        if (IsShown()) {
            AutoPtr<IAnimatable> anim = IAnimatable::Probe(mAnim);
            if (anim) anim->Start();
        }
    }
    else {
        mAnim = NULL;
    }
}

// @Override
ECode AnimatedImageView::OnAttachedToWindow()
{
    ImageView::OnAttachedToWindow();
    mAttached = TRUE;
    return NOERROR;
}

// @Override
ECode AnimatedImageView::OnDetachedFromWindow()
{
    ImageView::OnDetachedFromWindow();
    if (mAnim != NULL) {
        AutoPtr<IAnimatable> anim = IAnimatable::Probe(mAnim);
        if (anim) anim->Stop();
    }
    mAttached = FALSE;
    return NOERROR;
}

// @Override
void AnimatedImageView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 vis)
{
    ImageView::OnVisibilityChanged(changedView, vis);
    if (mAnim != NULL) {
        AutoPtr<IAnimatable> anim = IAnimatable::Probe(mAnim);
        if (anim) {
            if (IsShown()) {
                anim->Start();
            } else {
                anim->Stop();
            }
        }
    }
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
