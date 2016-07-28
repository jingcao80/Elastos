
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityViewFlipper.h"
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityViewFlipperLayoutParams.h"
#include "../R.h"
#include <Elastos.Droid.Graphics.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String TAG("KeyguardSecurityViewFlipper");

//============================================================================================
// CKeyguardSecurityViewFlipper::LayoutParams
//============================================================================================
CAR_INTERFACE_IMPL(CKeyguardSecurityViewFlipper::LayoutParams, FrameLayout::FrameLayoutLayoutParams,
        IKeyguardSecurityViewFlipperLayoutParams)

CKeyguardSecurityViewFlipper::LayoutParams::LayoutParams()
    : mMaxWidth(0)
    , mMaxHeight(0)
{
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* other)
{
    return FrameLayout::FrameLayoutLayoutParams::constructor(other);
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IKeyguardSecurityViewFlipperLayoutParams* other)
{
    FrameLayout::FrameLayoutLayoutParams::constructor(IFrameLayoutLayoutParams::Probe(other));

    other->GetMaxWidth(&mMaxWidth);
    other->GetMaxHeight(&mMaxHeight);
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::FrameLayoutLayoutParams::constructor(c, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::KeyguardSecurityViewFlipper_Layout);
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    a->GetDimensionPixelSize(
        R::styleable::KeyguardSecurityViewFlipper_Layout_layout_maxWidth, 0, &mMaxWidth);
    a->GetDimensionPixelSize(
        R::styleable::KeyguardSecurityViewFlipper_Layout_layout_maxHeight, 0, &mMaxHeight);
    return a->Recycle();
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::SetMaxWidth(
    /* [in] */ Int32 value)
{
    mMaxWidth = value;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::GetMaxWidth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mMaxWidth;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::SetMaxHeight(
    /* [in] */ Int32 value)
{
    mMaxHeight = value;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::LayoutParams::GetMaxHeight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mMaxHeight;
    return NOERROR;
}

//================================================================================
// CKeyguardSecurityViewFlipper
//================================================================================

const Boolean CKeyguardSecurityViewFlipper::DEBUG = FALSE;

CAR_OBJECT_IMPL(CKeyguardSecurityViewFlipper)

CAR_INTERFACE_IMPL_2(CKeyguardSecurityViewFlipper, ViewFlipper, IKeyguardSecurityViewFlipper, IKeyguardSecurityView)

CKeyguardSecurityViewFlipper::CKeyguardSecurityViewFlipper()
{
    CRect::New((IRect**)&mTempRect);
}

ECode CKeyguardSecurityViewFlipper::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardSecurityViewFlipper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewFlipper::constructor(context, attrs);
}

ECode CKeyguardSecurityViewFlipper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean bval;
    ViewFlipper::OnTouchEvent(ev, &bval);
    mTempRect->Set(0, 0, 0, 0);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visiable;
        child->GetVisibility(&visiable);
        if (visiable == IView::VISIBLE) {
            OffsetRectIntoDescendantCoords(child, mTempRect);

            Int32 left, top;
            mTempRect->GetLeft(&left);
            mTempRect->GetTop(&top);
            ev->OffsetLocation(left, top);

            Boolean res;
            child->DispatchTouchEvent(ev, &res);
            bval = res || bval;
            ev->OffsetLocation(-left, -top);
        }
    }
    *result = bval;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::GetSecurityView(
    /* [out] */ IKeyguardSecurityView** view)
{
    VALIDATE_NOT_NULL(view)

    Int32 id;
    GetDisplayedChild(&id);
    AutoPtr<IView> child;
    GetChildAt(id, (IView**)&child);
    if (IKeyguardSecurityView::Probe(child) != NULL) {
        *view = IKeyguardSecurityView::Probe(child);
        REFCOUNT_ADD(*view)
        return NOERROR;
    }
    *view = NULL;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->SetKeyguardCallback(callback);
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->SetLockPatternUtils(utils);
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::Reset()
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->Reset();
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::OnPause()
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->OnPause();
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::OnResume(
    /* [in] */ Int32 reason)
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->OnResume(reason);
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->NeedsInput(result);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::GetCallback(
    /* [out] */ IKeyguardSecurityCallback** callback)
{
    VALIDATE_NOT_NULL(callback)

    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->GetCallback(callback);
    }
    else {
        *callback = NULL;
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::ShowUsabilityHint()
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->ShowUsabilityHint();
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::ShowBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityView> active;
    GetSecurityView((IKeyguardSecurityView**)&active);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IKeyguardSecurityView> ksv = IKeyguardSecurityView::Probe(child);
        if (ksv != NULL) {
            ksv->ShowBouncer(ksv.Get() == active.Get() ? duration : 0);
        }
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::HideBouncer(
    /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityView> active;
    GetSecurityView((IKeyguardSecurityView**)&active);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IKeyguardSecurityView> ksv = IKeyguardSecurityView::Probe(child);
        if (ksv != NULL) {
            ksv->HideBouncer(ksv.Get() == active.Get() ? duration : 0);
        }
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::StartAppearAnimation()
{
    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        ksv->StartAppearAnimation();
    }
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IKeyguardSecurityView> ksv;
    GetSecurityView((IKeyguardSecurityView**)&ksv);
    if (ksv != NULL) {
        return ksv->StartDisappearAnimation(finishRunnable, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = IKeyguardSecurityViewFlipperLayoutParams::Probe(p) != NULL;
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result)

    IKeyguardSecurityViewFlipperLayoutParams* ksvflp = IKeyguardSecurityViewFlipperLayoutParams::Probe(p);
    if (ksvflp != NULL) {
        return CKeyguardSecurityViewFlipperLayoutParams::New(ksvflp, result);
    }

    return CKeyguardSecurityViewFlipperLayoutParams::New(p, result);
}

ECode CKeyguardSecurityViewFlipper::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IKeyguardSecurityViewFlipperLayoutParams** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IKeyguardSecurityViewFlipperLayoutParams> params;
    CKeyguardSecurityViewFlipperLayoutParams::New(context, attrs, (IKeyguardSecurityViewFlipperLayoutParams**)&params);
    *result = params;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    using Elastos::Core::Math;

    Int32 widthMode = MeasureSpec::GetMode(widthSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightSpec);
    if (DEBUG && widthMode != Elastos::Droid::View::View::MeasureSpec::AT_MOST) {
        Logger::W(TAG, "onMeasure: widthSpec %08x should be AT_MOST", widthMode);
    }
    if (DEBUG && heightMode != Elastos::Droid::View::View::MeasureSpec::AT_MOST) {
        Logger::W(TAG, "onMeasure: heightSpec %08x should be AT_MOST", widthMode);
    }

    Int32 widthSize = MeasureSpec::GetSize(widthSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightSpec);
    Int32 maxWidth = widthSize;
    Int32 maxHeight = heightSize;
    Int32 count;
    GetChildCount(&count);
    IKeyguardSecurityViewFlipperLayoutParams* ksvflp;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        ksvflp = IKeyguardSecurityViewFlipperLayoutParams::Probe(params);
        LayoutParams* lp = (LayoutParams*)ksvflp;

        if (lp->mMaxWidth > 0 && lp->mMaxWidth < maxWidth) {
            maxWidth = lp->mMaxWidth;
        }
        if (lp->mMaxHeight > 0 && lp->mMaxHeight < maxHeight) {
            maxHeight = lp->mMaxHeight;
        }
    }

    Int32 left, right, top, bottom;
    GetPadding(&left, &top, &right, &bottom);
    Int32 wPadding = left + right;
    Int32 hPadding = top + bottom;
    maxWidth -= wPadding;
    maxHeight -= hPadding;

    Int32 width = widthMode == Elastos::Droid::View::View::MeasureSpec::EXACTLY ? widthSize : 0;
    Int32 height = heightMode == Elastos::Droid::View::View::MeasureSpec::EXACTLY ? heightSize : 0;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        ksvflp = IKeyguardSecurityViewFlipperLayoutParams::Probe(params);
        LayoutParams* lp = (LayoutParams*)ksvflp;

        Int32 childWidthSpec = MakeChildMeasureSpec(maxWidth, lp->mWidth);
        Int32 childHeightSpec = MakeChildMeasureSpec(maxHeight, lp->mHeight);

        child->Measure(childWidthSpec, childHeightSpec);

        Int32 _width;
        child->GetMeasuredWidth(&_width);
        width = Math::Max(width, Math::Min(_width, widthSize - wPadding));

        Int32 _height;
        child->GetMeasuredHeight(&_height);
        height = Math::Max(height, Math::Min(_height, heightSize - hPadding));
    }
    SetMeasuredDimension(width + wPadding, height + hPadding);
    return NOERROR;
}

Int32 CKeyguardSecurityViewFlipper::MakeChildMeasureSpec(
    /* [in] */ Int32 maxSize,
    /* [in] */ Int32 childDimen)
{
    using Elastos::Core::Math;

    Int32 mode;
    Int32 size;
    switch (childDimen) {
        case IViewGroupLayoutParams::WRAP_CONTENT:
            mode = Elastos::Droid::View::View::MeasureSpec::AT_MOST;
            size = maxSize;
            break;
        case IViewGroupLayoutParams::MATCH_PARENT:
            mode = Elastos::Droid::View::View::MeasureSpec::EXACTLY;
            size = maxSize;
            break;
        default:
            mode = Elastos::Droid::View::View::MeasureSpec::EXACTLY;
            size = Math::Min(maxSize, childDimen);
            break;
    }
    return MeasureSpec::MakeMeasureSpec(size, mode);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
