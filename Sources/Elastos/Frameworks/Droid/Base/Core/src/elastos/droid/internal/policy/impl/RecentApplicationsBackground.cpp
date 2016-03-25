
#include "elastos/droid/internal/policy/impl/RecentApplicationsBackground.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                     RecentApplicationsBackground
//=====================================================================
CAR_INTERFACE_IMPL(RecentApplicationsBackground, LinearLayout, IRecentApplicationsBackground)

const String RecentApplicationsBackground::TAG("RecentApplicationsBackground");

RecentApplicationsBackground::RecentApplicationsBackground()
    : mBackgroundSizeChanged(FALSE)
{
    CRect::New((IRect**)&mTmp0);
    CRect::New((IRect**)&mTmp1);
}

ECode RecentApplicationsBackground::constructor(
    /* [in] */ IContext* ctx)
{
    return Init(ctx, NULL);
}

ECode RecentApplicationsBackground::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(ctx, attrs);
}

ECode RecentApplicationsBackground::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);

    GetBackground((IDrawable**)&mBackground);
    SetBackgroundDrawable(NULL);
    SetPadding(0, 0, 0, 0);
    SetGravity(IGravity::CENTER);
    return NOERROR;
}

Boolean RecentApplicationsBackground::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    SetWillNotDraw(FALSE);
    if (mLeft != left || mRight != right || mTop != top || mBottom != bottom) {
        mBackgroundSizeChanged = TRUE;
    }
    return LinearLayout::SetFrame(left, top, right, bottom);
}

Boolean RecentApplicationsBackground::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mBackground || LinearLayout::VerifyDrawable(who);
}

ECode RecentApplicationsBackground::JumpDrawablesToCurrentState()
{
    LinearLayout::JumpDrawablesToCurrentState();
    if (mBackground != NULL) {
        mBackground->JumpToCurrentState();
    }
    return NOERROR;
}

ECode RecentApplicationsBackground::DrawableStateChanged()
{
    AutoPtr<IDrawable> d = mBackground;
    Boolean isStateful = FALSE;
    if (d != NULL && (d->IsStateful(&isStateful), isStateful)) {
        Boolean value = FALSE;
        AutoPtr<ArrayOf<Int32> > drawableState;
        GetDrawableState((ArrayOf<Int32>**)&drawableState);
        d->SetState(drawableState, &value);
    }
    return LinearLayout::DrawableStateChanged();
}

ECode RecentApplicationsBackground::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> background = mBackground;
    if (background != NULL) {
        if (mBackgroundSizeChanged) {
            mBackgroundSizeChanged = FALSE;
            AutoPtr<IRect> chld = mTmp0;
            AutoPtr<IRect> bkg = mTmp1;
            Boolean value = FALSE;
            mBackground->GetPadding(bkg, &value);
            GetChildBounds(chld);
            // This doesn't clamp to this view's bounds, which is what we want,
            // so that the drawing is clipped.
            Int32 chldTop = 0;
            chld->GetTop(&chldTop);
            Int32 bkgTop = 0;
            bkg->GetTop(&bkgTop);
            const Int32 top = chldTop - bkgTop;
            Int32 chldBottom = 0;
            chld->GetBottom(&chldBottom);
            Int32 bkgBottom = 0;
            bkg->GetBottom(&bkgBottom);
            const Int32 bottom = chldBottom + bkgBottom;
            // The background here is a gradient that wants to
            // extend the full width of the screen (whatever that
            // may be).
            Int32 left, right;
            if (FALSE) {
                // This limits the width of the drawable.
                Int32 chldLeft = 0;
                chld->GetLeft(&chldLeft);
                Int32 bkgLeft = 0;
                bkg->GetLeft(&bkgLeft);
                left = chldLeft - bkgLeft;
                Int32 chldRight = 0;
                chld->GetRight(&chldRight);
                Int32 bkgRight = 0;
                bkg->GetRight(&bkgRight);
                right = chldRight + bkgRight;
            } else {
                // This expands it to full width.
                left = 0;
                GetRight(&right);
            }
            background->SetBounds(left, top, right, bottom);
        }
    }
    mBackground->Draw(canvas);

    if (FALSE) {
        AutoPtr<IPaint> p;
        CPaint::New((IPaint**)&p);
        p->SetColor(0x88ffff00);
        AutoPtr<IRect> bounds;
        background->GetBounds((IRect**)&bounds);
        canvas->DrawRect(bounds, p);
    }
    canvas->DrawARGB((Int32)(0.75*0xff), 0, 0, 0);

    return LinearLayout::Draw(canvas);
}

ECode RecentApplicationsBackground::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();
    mBackground->SetCallback(this);
    return SetWillNotDraw(FALSE);
}

ECode RecentApplicationsBackground::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    return mBackground->SetCallback(NULL);
}

void RecentApplicationsBackground::GetChildBounds(
    /* [in] */ IRect* r)
{
    using Elastos::Core::Math;
    r->SetLeft(Math::INT32_MAX_VALUE);
    r->SetTop(Math::INT32_MAX_VALUE);
    r->SetBottom(Math::INT32_MIN_VALUE);
    r->SetRight(Math::INT32_MIN_VALUE);
    Int32 count;
    ViewGroup::GetChildCount(&count);
    const Int32 N = count;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        Int32 visible = 0;
        if ((v->GetVisibility(&visible), visible) == IView::VISIBLE) {
            Int32 rLeft, rRight, rTop, rBottom;
            Int32 vLeft, vRight, vTop, vBottom;
            r->SetLeft(Math::Min((r->GetLeft(&rLeft), rLeft), (v->GetLeft(&vLeft), vLeft)));
            r->SetTop(Math::Min((r->GetTop(&rTop), rTop), (v->GetTop(&vTop), vTop)));
            r->SetRight(Math::Max((r->GetRight(&rRight), rRight), (v->GetRight(&vRight), vRight)));
            r->SetBottom(Math::Max((r->GetBottom(&rBottom), rBottom), (v->GetBottom(&vBottom), vBottom)));
        }
    }
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
