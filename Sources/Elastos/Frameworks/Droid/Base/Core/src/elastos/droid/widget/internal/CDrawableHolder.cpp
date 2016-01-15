#include "elastos/droid/widget/internal/CDrawableHolder.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

PInterface CDrawableHolder::Probe(
    /* [in] */ REIID riid)
{
    return _CDrawableHolder::Probe(riid);
}

ECode CDrawableHolder::constructor(
    /* [in] */ IBitmapDrawable* drawable)
{
    return DrawableHolder::Init(drawable);
}

ECode CDrawableHolder::constructor(
    /* [in] */ IBitmapDrawable* drawable,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return DrawableHolder::Init(drawable, x, y);
}

ECode CDrawableHolder::AddAnimTo(
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay,
    /* [in] */ const String& property,
    /* [in] */ Float toValue,
    /* [in] */ Boolean replace,
    /* [out] */ IObjectAnimator** animator)
{
    AutoPtr<IObjectAnimator> temp;
    temp= DrawableHolder::AddAnimTo(duration, delay, property, toValue, replace);
    *animator = temp;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode CDrawableHolder::RemoveAnimationFor(
    /* [in] */ const String& property)
{
    return DrawableHolder::RemoveAnimationFor(property);
}

ECode CDrawableHolder::ClearAnimations()
{
    return DrawableHolder::ClearAnimations();
}

ECode CDrawableHolder::Draw(
    /* [in] */ ICanvas* canvas)
{
    return DrawableHolder::Draw(canvas);
}

ECode CDrawableHolder::StartAnimations(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    return DrawableHolder::StartAnimations(listener);
}

ECode CDrawableHolder::SetX(
    /* [in] */ Float value)
{
    return DrawableHolder::SetX(value);
}

ECode CDrawableHolder::SetY(
    /* [in] */ Float value)
{
    return DrawableHolder::SetY(value);
}

ECode CDrawableHolder::SetScaleX(
    /* [in] */ Float value)
{
    return DrawableHolder::SetScaleX(value);
}

ECode CDrawableHolder::SetScaleY(
    /* [in] */ Float value)
{
    return DrawableHolder::SetScaleY(value);
}

ECode CDrawableHolder::SetAlpha(
    /* [in] */ Float alpha)
{
    return DrawableHolder::SetAlpha(alpha);
}

ECode CDrawableHolder::GetX(
    /* [out] */ Float* x)
{
    *x = DrawableHolder::GetX();
    return NOERROR;
}

ECode CDrawableHolder::GetY(
    /* [out] */ Float* y)
{
    *y = DrawableHolder::GetY();
    return NOERROR;
}

ECode CDrawableHolder::GetScaleX(
    /* [out] */ Float* x)
{
    *x = DrawableHolder::GetScaleX();
    return NOERROR;
}

ECode CDrawableHolder::GetScaleY(
    /* [out] */ Float* y)
{
    *y = DrawableHolder::GetScaleY();
    return NOERROR;
}

ECode CDrawableHolder::GetAlpha(
    /* [out] */ Float* alpha)
{
    *alpha = DrawableHolder::GetAlpha();
    return NOERROR;
}

ECode CDrawableHolder::GetDrawable(
    /* [out] */ IBitmapDrawable** d)
{
    AutoPtr<IBitmapDrawable> temp;
    temp = DrawableHolder::GetDrawable();
    *d = temp;
    REFCOUNT_ADD(*d);
    return NOERROR;
}

ECode CDrawableHolder::GetWidth(
    /* [out] */ Int32* width)
{
    *width =DrawableHolder::GetWidth();
    return NOERROR;
}

ECode CDrawableHolder::GetHeight(
    /* [out] */ Int32* height)
{
    *height = DrawableHolder::GetHeight();
    return NOERROR;
}

ECode CDrawableHolder::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return DrawableHolder::OnAnimationCancel(animation);
}

ECode CDrawableHolder::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return DrawableHolder::OnAnimationEnd(animation);
}

ECode CDrawableHolder::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return DrawableHolder::OnAnimationRepeat(animation);
}

ECode CDrawableHolder::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return DrawableHolder::OnAnimationStart(animation);
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

