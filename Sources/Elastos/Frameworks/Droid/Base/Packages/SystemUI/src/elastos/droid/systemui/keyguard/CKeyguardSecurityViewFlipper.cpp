
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityViewFlipper.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardSecurityViewFlipper::LayoutParams, FrameLayout::FrameLayoutLayoutParams,
        IKeyguardSecurityViewFlipperLayoutParams)

CKeyguardSecurityViewFlipper::LayoutParams::LayoutParams()
    : mMaxWidth(0)
    , mMaxHeight(0)
{
}

CARAPI CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* other)
{
    return FrameLayout::FrameLayoutLayoutParams::constructor(other);
}

CARAPI CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IKeyguardSecurityViewFlipperLayoutParams* other)
{
    FrameLayout::FrameLayoutLayoutParams::constructor(IFrameLayoutLayoutParams::Probe(other));

    maxWidth = ((LayoutParams*)other)->mMaxWidth;
    maxHeight = ((LayoutParams*)other)->mMaxHeight;
    return NOERROR;
}

CARAPI CKeyguardSecurityViewFlipper::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::FrameLayoutLayoutParams::constructor(c, attrs);

    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs,
            R::styleable::KeyguardSecurityViewFlipper_Layout, 0, 0, (ITypedArray**)&a);
    a->GetDimensionPixelSize(
            R::styleable::KeyguardSecurityViewFlipper_Layout_layout_maxWidth, 0, &maxWidth);
    a->GetDimensionPixelSize(
            R::styleable::KeyguardSecurityViewFlipper_Layout_layout_maxHeight, 0, &maxHeight);
    return a->Recycle();
}

const String CKeyguardSecurityViewFlipper::TAG("KeyguardSecurityViewFlipper");
const Boolean CKeyguardSecurityViewFlipper::DEBUG = IKeyguardConstants::DEBUG;

CAR_OBJECT_IMPL(CKeyguardSecurityViewFlipper)

CAR_INTERFACE_IMPL(CKeyguardSecurityViewFlipper, ViewFlipper, IKeyguardSecurityView)

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
    return ViewFlipper::constructor(context, attr);
}

ECode CKeyguardSecurityViewFlipper::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean _result;
    ViewFlipper::OnTouchEvent(ev, &_result);
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

            Int32 left;
            mTempRect->GetLeft(&left);
            Int32 top;
            mTempRect->GetTop(&top);
            ev->OffsetLocation(left, top);

            Boolean res;
            child->DispatchTouchEvent(ev, &res);
            _result = res || _result;
            ev->OffsetLocation(-left, -top);
        }
    }
    *result = _result;
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
        ksv->NeedsInput(result)
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
        if (IKeyguardSecurityView::Probe(child) != NULL) {
            AutoPtr<IKeyguardSecurityView> ksv = IKeyguardSecurityView::Probe(child);
            ksv->ShowBouncer(TO_IINTERFACE(ksv) == TO_IINTERFACE(active) ? duration : 0);
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
        if (IKeyguardSecurityView::Probe(child) != NULL) {
            AutoPtr<IKeyguardSecurityView> ksv = IKeyguardSecurityView::Probe(child);
            ksv->HideBouncer(TO_IINTERFACE(ksv) == TO_IINTERFACE(active) ? duration : 0);
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
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode CKeyguardSecurityViewFlipper::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = IKeyguardSecurityViewFlipperLayoutParams::Probe(p) != NULL;
}

ECode CKeyguardSecurityViewFlipper::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)

    AutoPtr<IViewGroupLayoutParams> _params;
    if (IKeyguardSecurityViewFlipperLayoutParams::Probe(p) != NULL) {
        _params = new LayoutParams(IKeyguardSecurityViewFlipperLayoutParams::Probe(p))
        *params = _params;
        REFCOUNT_ADD(*params)
        return NOERROR;
    }
    else {
        _params = new LayoutParams(p);
        *params = _params;
        REFCOUNT_ADD(*params)
        return NOERROR;
    }
}

ECode CKeyguardSecurityViewFlipper::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IKeyguardSecurityViewFlipperLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IKeyguardSecurityViewFlipperLayoutParams> _params = new LayoutParams(context, attrs);
    *params = _params;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode CKeyguardSecurityViewFlipper::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    Int32 widthMode = MeasureSpec::GetMode(widthSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightSpec);
    if (DEBUG && widthMode != IMeasureSpec::AT_MOST) {
        Logger::W(TAG, "onMeasure: widthSpec " + MeasureSpec.toString(widthSpec) +
                " should be AT_MOST");
    }
    if (DEBUG && heightMode != IMeasureSpec::AT_MOST) {
        Logger::W(TAG, "onMeasure: heightSpec " + MeasureSpec.toString(heightSpec) +
                " should be AT_MOST");
    }

    Int32 widthSize = MeasureSpec::GetSize(widthSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightSpec);
    Int32 maxWidth = widthSize;
    Int32 maxHeight = heightSize;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<LayoutParams> lp = (LayoutParams*)IKeyguardSecurityViewFlipperLayoutParams::Proeb(params);

        if (lp->mMaxWidth > 0 && lp->mMaxWidth < maxWidth) {
            maxWidth = lp->mMaxWidth;
        }
        if (lp->mMaxHeight > 0 && lp->mMaxHeight < maxHeight) {
            maxHeight = lp->mMaxHeight;
        }
    }

    Int32 left;
    GetPaddingLeft(&left);
    Int32 right;
    GetPaddingRight(&right);
    Int32 wPadding = left + right;

    Int32 top;
    GetPaddingTop(&top);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 hPadding = top + bottom;
    maxWidth -= wPadding;
    maxHeight -= hPadding;

    Int32 width = widthMode == IMeasureSpec::EXACTLY ? widthSize : 0;
    Int32 height = heightMode == IMeasureSpec::EXACTLY ? heightSize : 0;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<LayoutParams> lp = (LayoutParams*)IKeyguardSecurityViewFlipperLayoutParams::Proeb(params);

        Int32 childWidthSpec;
        MakeChildMeasureSpec(maxWidth, lp->mWidth, &childWidthSpec);
        Int32 childHeightSpec;
        MakeChildMeasureSpec(maxHeight, lp->mHeight, &childHeightSpec);

        child->Measure(childWidthSpec, childHeightSpec);

        Int32 _width;
        child->GetMeasuredWidth(&_width);
        width = Math::Max(width, Math::Min(_width, widthSize - wPadding));

        Int32 _height;
        child->GetMeasuredHeight(&_height);
        height = Math::Max(height, Math::Min(_height, heightSize - hPadding));
    }
    return SetMeasuredDimension(width + wPadding, height + hPadding);
}

Int32 CKeyguardSecurityViewFlipper::MakeChildMeasureSpec(
    /* [in] */ Int32 maxSize,
    /* [in] */ Int32 childDimen)
{
    Int32 mode;
    Int32 size;
    switch (childDimen) {
        case IViewGroupLayoutParams::WRAP_CONTENT:
            mode = IMeasureSpec::AT_MOST;
            size = maxSize;
            break;
        case IViewGroupLayoutParams::MATCH_PARENT:
            mode = IMeasureSpec::EXACTLY;
            size = maxSize;
            break;
        default:
            mode = IMeasureSpec::EXACTLY;
            size = Math::Min(maxSize, childDimen);
            break;
    }
    return MeasureSpec::MakeMeasureSpec(size, mode);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
