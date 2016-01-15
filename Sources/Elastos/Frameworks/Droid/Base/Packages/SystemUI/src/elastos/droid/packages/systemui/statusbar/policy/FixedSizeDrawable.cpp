#include "elastos/droid/systemui/statusbar/policy/FixedSizeDrawable.h"


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


FixedSizeDrawable::FixedSizeDrawable()
    : mLeft(0)
    , mTop(0)
    , mRight(0)
    , mBottom(0)
{
}

FixedSizeDrawable::FixedSizeDrawable(
    /* [in] */ IDrawable* that)
    : mLeft(0)
    , mTop(0)
    , mRight(0)
    , mBottom(0)
{
    mDrawable = that;
}

ECode FixedSizeDrawable::Init(
    /* [in] */ IDrawable* that)
{
    mDrawable = that;
    return NOERROR;
}

ECode FixedSizeDrawable::SetFixedBounds(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    mLeft = l;
    mTop = t;
    mRight = r;
    mBottom = b;
    return NOERROR;
}

ECode FixedSizeDrawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return mDrawable->SetBounds(mLeft, mTop, mRight, mBottom);
}

ECode FixedSizeDrawable::SetBounds(
    /* [in] */ IRect* bounds)
{
    return mDrawable->SetBounds(bounds);
}

ECode FixedSizeDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    return mDrawable->Draw(canvas);
}

Int32 FixedSizeDrawable::GetOpacity()
{
    Int32 alpha;
    mDrawable->GetOpacity(&alpha);
    return alpha;
}

ECode FixedSizeDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mDrawable->SetAlpha(alpha);
}

ECode FixedSizeDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mDrawable->SetColorFilter(cf);
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
