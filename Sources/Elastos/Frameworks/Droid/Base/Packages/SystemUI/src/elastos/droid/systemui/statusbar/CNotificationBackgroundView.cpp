
#include "elastos/droid/systemui/statusbar/CNotificationBackgroundView.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::Res::CColorStateListHelper;
using Elastos::Droid::Content::Res::IColorStateListHelper;
using Elastos::Droid::Graphics::Drawable::IRippleDrawable;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CNotificationBackgroundView);
CAR_INTERFACE_IMPL(CNotificationBackgroundView, View, INotificationBackgroundView);
CNotificationBackgroundView::CNotificationBackgroundView()
    : mClipTopAmount(0)
    , mActualHeight(0)
{}

ECode CNotificationBackgroundView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return View::constructor(context, attrs);
}

void CNotificationBackgroundView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Draw(canvas, mBackground);
}

void CNotificationBackgroundView::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IDrawable* drawable)
{
    Int32 width = 0;
    GetWidth(&width);
    if (drawable != NULL) {
        drawable->SetBounds(0, mClipTopAmount, width, mActualHeight);
        drawable->Draw(canvas);
    }
}

Boolean CNotificationBackgroundView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return View::VerifyDrawable(who) || who == mBackground;
}

ECode CNotificationBackgroundView::DrawableStateChanged()
{
    DrawableStateChanged(mBackground);
    return NOERROR;
}

void CNotificationBackgroundView::DrawableStateChanged(
    /* [in] */ IDrawable* d)
{
    Boolean tmp = FALSE;
    if (d != NULL && (d->IsStateful(&tmp), tmp)) {
        AutoPtr<ArrayOf<Int32> > states;
        GetDrawableState((ArrayOf<Int32>**)&states);
        Boolean isStateful = FALSE;
        d->SetState(states, &isStateful);
    }
}

ECode CNotificationBackgroundView::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (mBackground != NULL) {
        mBackground->SetHotspot(x, y);
    }
    return NOERROR;
}

ECode CNotificationBackgroundView::SetCustomBackground(
    /* [in] */ IDrawable* background)
{
    if (mBackground != NULL) {
        mBackground->SetCallback(NULL);
        UnscheduleDrawable(mBackground);
    }
    mBackground = background;
    if (mBackground != NULL) {
        mBackground->SetCallback(this);
    }
    Invalidate();
    return NOERROR;
}

ECode CNotificationBackgroundView::SetCustomBackground(
    /* [in] */ Int32 drawableResId)
{
    AutoPtr<IDrawable> d;
    mContext->GetDrawable(drawableResId, (IDrawable**)&d);
    SetCustomBackground(d);
    return NOERROR;
}

ECode CNotificationBackgroundView::SetTint(
    /* [in] */ Int32 tintColor)
{
    if (tintColor != 0) {
        mBackground->SetColorFilter(tintColor, PorterDuffMode_SRC_ATOP);
    }
    else {
        mBackground->ClearColorFilter();
    }
    Invalidate();
    return NOERROR;
}

ECode CNotificationBackgroundView::SetActualHeight(
    /* [in] */ Int32 actualHeight)
{
    mActualHeight = actualHeight;
    Invalidate();
    return NOERROR;
}

ECode CNotificationBackgroundView::GetActualHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mActualHeight;
    return NOERROR;
}

ECode CNotificationBackgroundView::SetClipTopAmount(
    /* [in] */ Int32 clipTopAmount)
{
    mClipTopAmount = clipTopAmount;
    Invalidate();
    return NOERROR;
}

ECode CNotificationBackgroundView::HasOverlappingRendering(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    // Prevents this view from creating a layer when alpha is animating.
    *has = FALSE;
    return NOERROR;
}

ECode CNotificationBackgroundView::SetState(
    /* [in] */ ArrayOf<Int32>* drawableState)
{
    Boolean isStateful = FALSE;
    mBackground->SetState(drawableState, &isStateful);
    return NOERROR;
}

ECode CNotificationBackgroundView::SetRippleColor(
    /* [in] */ Int32 color)
{
    if (IRippleDrawable::Probe(mBackground)) {
        AutoPtr<IRippleDrawable> ripple = IRippleDrawable::Probe(mBackground);
        AutoPtr<IColorStateListHelper> helper;
        CColorStateListHelper::AcquireSingleton((IColorStateListHelper**)&helper);
        AutoPtr<IColorStateList> list;
        helper->ValueOf(color, (IColorStateList**)&list);
        ripple->SetColor(list);
    }
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
