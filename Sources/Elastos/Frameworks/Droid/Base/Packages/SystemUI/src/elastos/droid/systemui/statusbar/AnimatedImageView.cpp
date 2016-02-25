
#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"

using Elastos::Droid::Graphics::Drawable::IAnimatable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL(AnimatedImageView, ImageView, IAnimatedImageView);
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
