#include <widget/StackViewLayoutParams.h>
#include <graphics/CRect.h>
#include <graphics/CRectF.h>
#include <elastos/core/Math.h>

using Elastos::Core::Math;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::View::IViewParent;

namespace Elastos {
namespace Droid {
namespace Widget {

StackViewLayoutParams::StackViewLayoutParams()
    : mHorizontalOffset(0)
    , mVerticalOffset(0)
{
    CRect::New((IRect**)&mParentRect);
    CRect::New((IRect**)&mInvalidateRect);
    CRectF::New((IRectF**)&mInvalidateRectf);
    CRect::New((IRect**)&mGlobalInvalidateRect);
}

CARAPI StackViewLayoutParams::Init(
    /* [in] */ IStackView* sv,
    /* [in] */ IView* view)
{
    LayoutParams::Init(0, 0);
    mWidth = 0;
    mHeight = 0;
    mHorizontalOffset = 0;
    mVerticalOffset = 0;
    mView = view;
    mStackView = sv;
    return NOERROR;
}

CARAPI StackViewLayoutParams::Init(
    /* [in] */ IStackView* sv,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LayoutParams::Init(context, attrs);
    mHorizontalOffset = 0;
    mVerticalOffset = 0;
    mWidth = 0;
    mHeight = 0;
    mStackView = sv;
    return NOERROR;
}

StackViewLayoutParams::StackViewLayoutParams(
    /* [in] */ IStackView* sv,
    /* [in] */ IView* view)
    : LayoutParams(0, 0)
    , mHorizontalOffset(0)
    , mVerticalOffset(0)
    , mView(view)
    , mStackView(sv)
{
    mWidth = 0;
    mHeight = 0;
    CRect::New((IRect**)&mParentRect);
    CRect::New((IRect**)&mInvalidateRect);
    CRectF::New((IRectF**)&mInvalidateRectf);
    CRect::New((IRect**)&mGlobalInvalidateRect);
}

StackViewLayoutParams::StackViewLayoutParams(
    /* [in] */ IStackView* sv,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LayoutParams(context, attrs)
    , mHorizontalOffset(0)
    , mVerticalOffset(0)
    , mStackView(sv)
{
    mWidth = 0;
    mHeight = 0;
    CRect::New((IRect**)&mParentRect);
    CRect::New((IRect**)&mInvalidateRect);
    CRectF::New((IRectF**)&mInvalidateRectf);
    CRect::New((IRect**)&mGlobalInvalidateRect);
}

ECode StackViewLayoutParams::SetVerticalOffset(
    /* [in] */ Int32 newVerticalOffset)
{
    SetOffsets(mHorizontalOffset, newVerticalOffset);
    return NOERROR;
}

Int32 StackViewLayoutParams::GetVerticalOffset()
{
    return mVerticalOffset;
}

ECode StackViewLayoutParams::SetHorizontalOffset(
    /* [in] */ Int32 newHorizontalOffset)
{
    SetOffsets(newHorizontalOffset, mVerticalOffset);
    return NOERROR;
}

Int32 StackViewLayoutParams::GetHorizontalOffset()
{
    return mHorizontalOffset;
}

CARAPI StackViewLayoutParams::SetOffsets(
    /* [in] */ Int32 newHorizontalOffset,
    /* [in] */ Int32 newVerticalOffset)
{
    Int32 horizontalOffsetDelta = newHorizontalOffset - mHorizontalOffset;
    mHorizontalOffset = newHorizontalOffset;
    Int32 verticalOffsetDelta = newVerticalOffset - mVerticalOffset;
    mVerticalOffset = newVerticalOffset;

    if (mView) {
        mView->RequestLayout();
        Int32 l = 0, r = 0, t = 0, b = 0;
        mView->GetLeft(&l);
        mView->GetRight(&r);
        mView->GetTop(&t);
        mView->GetBottom(&b);
        Int32 left = Elastos::Core::Math::Min(l + horizontalOffsetDelta, l);
        Int32 right = Elastos::Core::Math::Max(r + horizontalOffsetDelta, r);
        Int32 top = Elastos::Core::Math::Min(t + verticalOffsetDelta, t);
        Int32 bottom = Elastos::Core::Math::Max(b + verticalOffsetDelta, b);

        mInvalidateRectf->Set(left, top, right, bottom);

        Float il = 0, it = 0, ir = 0, ib = 0;;
        mInvalidateRectf->GetLeft(&il);
        mInvalidateRectf->GetTop(&it);
        mInvalidateRectf->GetRight(&ir);
        mInvalidateRectf->GetBottom(&ib);
        Float xoffset = -il;
        Float yoffset = -it;
        mInvalidateRectf->Offset(xoffset, yoffset);
        AutoPtr<IMatrix> matrix;
        mView->GetMatrix((IMatrix**)&matrix);
        Boolean res = FALSE;
        matrix->MapRect(mInvalidateRectf, &res);
        mInvalidateRectf->Offset(-xoffset, -yoffset);

        mInvalidateRect->Set((Int32)Elastos::Core::Math::Floor(il), (Int32)Elastos::Core::Math::Floor(it),
            (Int32)Elastos::Core::Math::Ceil(ir), (Int32)Elastos::Core::Math::Ceil(ib));

        InvalidateGlobalRegion(mView, mInvalidateRect);
    }
    return NOERROR;
}

void StackViewLayoutParams::InvalidateGlobalRegion(
    /* [in] */ IView* v,
    /* [in] */ IRect* r)
{
    mGlobalInvalidateRect->Set(r);
    Int32 width = 0, height =0;
    mStackView->GetWidth(&width);
    mStackView->GetHeight(&height);
    mGlobalInvalidateRect->Union(0, 0, width, height);
    AutoPtr<IView> p = v;
    AutoPtr<IViewParent> vp;
    v->GetParent((IViewParent**)&vp);
    if (IView::Probe(vp) == NULL) return;

    Boolean firstPass = TRUE;
    mParentRect->Set(0, 0, 0, 0);
    Boolean res = FALSE;
    mParentRect->Contains(mGlobalInvalidateRect, &res);
    Int32 rl = 0, rr = 0, rt = 0, rb = 0;
    vp = NULL;
    p->GetParent((IViewParent**)&vp);
    while (IView::Probe(vp) && !res) {
        Int32 l = 0, scrollX = 0, t = 0, scrollY = 0, width = 0, height = 0;
        if (!firstPass) {
            p->GetLeft(&l);
            p->GetScrollX(&scrollX);
            p->GetTop(&t);
            p->GetScrollY(&scrollY);
            mGlobalInvalidateRect->Offset(l - scrollX, t - scrollY);
        }
        firstPass = FALSE;
        AutoPtr<IView> p = IView::Probe(vp);
        p->GetWidth(&width);
        p->GetScrollX(&scrollX);
        p->GetHeight(&height);
        p->GetScrollY(&scrollY);
        mParentRect->Set(scrollX, scrollY, width + scrollX, height + scrollY);
        mGlobalInvalidateRect->GetLeft(&rl);
        mGlobalInvalidateRect->GetTop(&rt);
        mGlobalInvalidateRect->GetRight(&rr);
        mGlobalInvalidateRect->GetBottom(&rb);
        p->Invalidate(rl, rt, rr, rb);
    }

    mGlobalInvalidateRect->GetLeft(&rl);
    mGlobalInvalidateRect->GetTop(&rt);
    mGlobalInvalidateRect->GetRight(&rr);
    mGlobalInvalidateRect->GetBottom(&rb);
    p->Invalidate(rl, rt, rr, rb);
}

AutoPtr<IRect> StackViewLayoutParams::GetInvalidateRect()
{
    return mInvalidateRect;
}

ECode StackViewLayoutParams::ResetInvalidateRect()
{
    mInvalidateRect->Set(0, 0, 0, 0);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
