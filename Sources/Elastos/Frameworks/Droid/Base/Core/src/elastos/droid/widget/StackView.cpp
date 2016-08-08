
#include "elastos/droid/widget/StackView.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CStackViewLayoutParams.h"
#include "elastos/droid/widget/CStackViewStackFrame.h"
#include "elastos/droid/animation/CPropertyValuesHolderHelper.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CTableMaskFilterHelper.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BlurMaskFilterBlur_NORMAL;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IMaskFilter;
using Elastos::Droid::Graphics::ITableMaskFilter;
using Elastos::Droid::Graphics::ITableMaskFilterHelper;
using Elastos::Droid::Graphics::CTableMaskFilterHelper;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::PorterDuffMode_DST_OUT;
using Elastos::Droid::Graphics::CBlurMaskFilter;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::RegionOp_UNION;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(StackView::LayoutParams, ViewGroup::LayoutParams, IStackViewLayoutParams)

StackView::LayoutParams::LayoutParams()
    : mHorizontalOffset(0)
    , mVerticalOffset(0)
{
    CRect::New((IRect**)&mParentRect);
    CRect::New((IRect**)&mInvalidateRect);
    CRectF::New((IRectF**)&mInvalidateRectf);
    CRect::New((IRect**)&mGlobalInvalidateRect);
}

CARAPI StackView::LayoutParams::constructor(
    /* [in] */ IView* view)
{
    ViewGroup::LayoutParams::constructor(0, 0);
    mWidth = 0;
    mHeight = 0;
    mHorizontalOffset = 0;
    mVerticalOffset = 0;
    mView = view;
    return NOERROR;
}

CARAPI StackView::LayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::LayoutParams::constructor(context, attrs);
    mHorizontalOffset = 0;
    mVerticalOffset = 0;
    mWidth = 0;
    mHeight = 0;
    return NOERROR;
}

void StackView::LayoutParams::InvalidateGlobalRegion(
    /* [in] */ IView* v,
    /* [in] */ IRect* r)
{
    mGlobalInvalidateRect->Set(r);
    Int32 width = 0, height =0;
    GetWidth(&width);
    GetHeight(&height);
    mGlobalInvalidateRect->Union(0, 0, width, height);
    AutoPtr<IView> p = v;
    AutoPtr<IViewParent> vp;
    v->GetParent((IViewParent**)&vp);
    if (IView::Probe(vp) == NULL) return;

    Boolean firstPass = TRUE;
    mParentRect->Set(0, 0, 0, 0);
    Boolean res = FALSE;
    Int32 rl = 0, rr = 0, rt = 0, rb = 0;
    vp = NULL;
    while ((p->GetParent((IViewParent**)&vp), IView::Probe(vp) != NULL)
        && (mParentRect->Contains(mGlobalInvalidateRect, &res), !res)) {
        Int32 l = 0, scrollX = 0, t = 0, scrollY = 0, width = 0, height = 0;
        if (!firstPass) {
            p->GetLeft(&l);
            p->GetScrollX(&scrollX);
            p->GetTop(&t);
            p->GetScrollY(&scrollY);
            mGlobalInvalidateRect->Offset(l - scrollX, t - scrollY);
        }
        firstPass = FALSE;
        p = IView::Probe(vp);
        vp = NULL;
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

AutoPtr<IRect> StackView::LayoutParams::GetInvalidateRect()
{
    return mInvalidateRect;
}

void StackView::LayoutParams::ResetInvalidateRect()
{
    mInvalidateRect->Set(0, 0, 0, 0);
}

ECode StackView::LayoutParams::SetVerticalOffset(
    /* [in] */ Int32 newVerticalOffset)
{
    return SetOffsets(mHorizontalOffset, newVerticalOffset);
}

ECode StackView::LayoutParams::SetHorizontalOffset(
    /* [in] */ Int32 newHorizontalOffset)
{
    return SetOffsets(newHorizontalOffset, mVerticalOffset);
}

ECode StackView::LayoutParams::SetOffsets(
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

///////////////////////////////////////////////////////////////////////
//                  StackView::StackFrame
///////////////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(StackView::StackFrame, FrameLayout, IStackFrame)

ECode StackView::StackFrame::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

void StackView::StackFrame::SetTransformAnimator(
    /* [in] */ IObjectAnimator* oa)
{
    mTransformAnimator = oa;
}

void StackView::StackFrame::SetSliderAnimator(
    /* [in] */ IObjectAnimator* oa)
{
    mSliderAnimator = oa;
}

Boolean StackView::StackFrame::CancelTransformAnimator()
{
    if (mTransformAnimator) {
        AutoPtr<IObjectAnimator> oa = mTransformAnimator;
        if (oa) {
            IAnimator::Probe(oa)->Cancel();
            return TRUE;
        }
    }
    return FALSE;
}

Boolean StackView::StackFrame::CancelSliderAnimator()
{
    if (mSliderAnimator) {
        AutoPtr<IObjectAnimator> oa = mSliderAnimator;
        if (oa) {
            IAnimator::Probe(oa)->Cancel();
            return TRUE;
        }
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////
//                  StackView::HolographicHelper
///////////////////////////////////////////////////////////////////////
const Int32 StackView::HolographicHelper::RES_OUT;
const Int32 StackView::HolographicHelper::CLICK_FEEDBACK;

StackView::HolographicHelper::HolographicHelper(
    /* [in] */ IContext* context)
{
    CPaint::New((IPaint**)&mHolographicPaint);
    CPaint::New((IPaint**)&mErasePaint);
    CPaint::New((IPaint**)&mBlurPaint);

    CCanvas::New((ICanvas**)&mCanvas);
    CCanvas::New((ICanvas**)&mMaskCanvas);
    mTmpXY = ArrayOf<Int32>::Alloc(2);
    CMatrix::New((IMatrix**)&mIdentityMatrix);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensity);

    mHolographicPaint->SetFilterBitmap(TRUE);
    AutoPtr<ITableMaskFilterHelper> helper;
    CTableMaskFilterHelper::AcquireSingleton((ITableMaskFilterHelper**)&helper);
    AutoPtr<ITableMaskFilter> tmf;
    helper->CreateClipTable(0, 30, (ITableMaskFilter**)&tmf);
    mHolographicPaint->SetMaskFilter(IMaskFilter::Probe(tmf));
    AutoPtr<IPorterDuffXfermode> pd;
    CPorterDuffXfermode::New(PorterDuffMode_DST_OUT, (IPorterDuffXfermode**)&pd);
    mErasePaint->SetXfermode(Elastos::Droid::Graphics::IXfermode::Probe(pd));
    mErasePaint->SetFilterBitmap(TRUE);

    CBlurMaskFilter::New(2 * mDensity, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&mSmallBlurMaskFilter);
    CBlurMaskFilter::New(4 * mDensity, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&mLargeBlurMaskFilter);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateClickOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 color)
{
    return CreateOutline(v, CLICK_FEEDBACK, color);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateResOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 color)
{
    return CreateOutline(v, RES_OUT, color);
}

AutoPtr<IBitmap> StackView::HolographicHelper::CreateOutline(
    /* [in] */ IView* v,
    /* [in] */ Int32 type,
    /* [in] */ Int32 color)
{
    mHolographicPaint->SetColor(color);
    if (type == RES_OUT) {
        mBlurPaint->SetMaskFilter(IMaskFilter::Probe(mSmallBlurMaskFilter));
    }
    else if (type == CLICK_FEEDBACK) {
        mBlurPaint->SetMaskFilter(IMaskFilter::Probe(mLargeBlurMaskFilter));
    }

    Int32 width = 0, height = 0;
    v->GetMeasuredWidth(&width);
    v->GetMeasuredHeight(&height);
    if (width == 0 || height == 0) {
        return NULL;
    }

    AutoPtr<IResources> res;
    v->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(dm, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    mCanvas->SetBitmap(bitmap);

    Float rotationX = 0, rotation = 0, translationY = 0, translationX = 0;
    v->GetRotationX(&rotationX);
    v->GetRotation(&rotation);
    v->GetTranslationY(&translationY);
    v->GetTranslationX(&translationX);
    v->SetRotationX(0);
    v->SetRotation(0);
    v->SetTranslationY(0);
    v->SetTranslationX(0);
    v->Draw(mCanvas);
    v->SetRotationX(rotationX);
    v->SetRotation(rotation);
    v->SetTranslationY(translationY);
    v->SetTranslationX(translationX);

    DrawOutline(mCanvas, bitmap);
    mCanvas->SetBitmap(NULL);
    return bitmap;
}

void StackView::HolographicHelper::DrawOutline(
    /* [in] */ ICanvas* dest,
    /* [in] */ IBitmap* src)
{
    AutoPtr<IBitmap> mask;
    src->ExtractAlpha(mBlurPaint, mTmpXY, (IBitmap**)&mask);
    mMaskCanvas->SetBitmap(mask);
    mMaskCanvas->DrawBitmap(src, -(*mTmpXY)[0], -(*mTmpXY)[1], mErasePaint);
    dest->DrawColor(0, PorterDuffMode_CLEAR);
    dest->SetMatrix(mIdentityMatrix);
    dest->DrawBitmap(mask, (*mTmpXY)[0], (*mTmpXY)[1], mHolographicPaint);
    mMaskCanvas->SetBitmap(NULL);
    mask->Recycle();
}

///////////////////////////////////////////////////////////////////////
//                  StackView::StackSlider
///////////////////////////////////////////////////////////////////////

const Int32 StackView::StackSlider::NORMAL_MODE;
const Int32 StackView::StackSlider::BEGINNING_OF_STACK_MODE;
const Int32 StackView::StackSlider::END_OF_STACK_MODE;

StackView::StackSlider::StackSlider(
    /* [in] */ StackView* host)
    : mHost(host)
{
}

StackView::StackSlider::StackSlider(const StackSlider& copy)
{
    mView = copy.mView;
    mYProgress = copy.mYProgress;
    mXProgress = copy.mXProgress;
    mMode = copy.mMode;
    mHost = copy.mHost;
}

void StackView::StackSlider::SetView(
        /* [in] */ IView* v)
{
    mView = v;
}

void StackView::StackSlider::SetYProgress(
    /* [in] */ Float r)
{
    r = Elastos::Core::Math::Min(1.0f, r);
    r = Elastos::Core::Math::Max(0.0f, r);

    mYProgress = r;
    if (!mView) return;

    AutoPtr<IViewGroupLayoutParams> viewLp;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);
    AutoPtr<IViewGroupLayoutParams> highlightLp;
    IView::Probe(mHost->mHighlight)->GetLayoutParams((IViewGroupLayoutParams**)&highlightLp);

    Int32 stackDirection = (mHost->mStackMode == ITEMS_SLIDE_UP) ? 1 : -1;

    if (Elastos::Core::Math::Compare(0.0f, mYProgress) != 0 && Elastos::Core::Math::Compare(1.0f, mYProgress) != 0) {
        Int32 type = 0;
        mView->GetLayerType(&type);
        if (type == IView::LAYER_TYPE_NONE) {
            mView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
        }
    }
    else {
        Int32 type = 0;
        mView->GetLayerType(&type);
        if (type != IView::LAYER_TYPE_NONE) {
            mView->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
        }
    }

    switch (mMode) {
        case NORMAL_MODE:
        {
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(-r * stackDirection * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(-r * stackDirection * mHost->mSlideAmount));
            IView::Probe(mHost->mHighlight)->SetAlpha(HighlightAlphaInterpolator(r));

            Float alpha = ViewAlphaInterpolator(1 - r);

            Float vah = 0;
            mView->GetAlpha(&vah);
            Int32 visible = 0;
            mView->GetVisibility(&visible);
            if (vah == 0 && alpha != 0 && visible != IView::VISIBLE) {
                mView->SetVisibility(IView::VISIBLE);
            }
            else if (alpha == 0 && vah != 0 && visible == IView::VISIBLE) {
                mView->SetVisibility(IView::INVISIBLE);
            }

            mView->SetAlpha(alpha);
            mView->SetRotationX(stackDirection * 90.0f * RotationInterpolator(r));
            IView::Probe(mHost->mHighlight)->SetRotationX(stackDirection * 90.0f * RotationInterpolator(r));
            break;
        }

        case END_OF_STACK_MODE:
        {
            r = r * 0.2f;
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(-stackDirection * r * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(-stackDirection * r * mHost->mSlideAmount));
            IView::Probe(mHost->mHighlight)->SetAlpha(HighlightAlphaInterpolator(r));
            break;
        }
        case BEGINNING_OF_STACK_MODE:
        {
            r = (1-r) * 0.2f;
            AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
            AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
            assert(slp && shlp);
            slp->SetVerticalOffset(Elastos::Core::Math::Round(stackDirection * r * mHost->mSlideAmount));
            shlp->SetVerticalOffset(Elastos::Core::Math::Round(stackDirection * r * mHost->mSlideAmount));
            IView::Probe(mHost->mHighlight)->SetAlpha(HighlightAlphaInterpolator(r));
            break;
        }
    }
}

void StackView::StackSlider::SetXProgress(
    /* [in] */ Float r)
{
    r = Elastos::Core::Math::Min(2.0f, r);
    r = Elastos::Core::Math::Max(-2.0f, r);

    mXProgress = r;

    if (!mView) return;

    AutoPtr<IViewGroupLayoutParams> viewLp;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);
    AutoPtr<IViewGroupLayoutParams> highlightLp;
    IView::Probe(mHost->mHighlight)->GetLayoutParams((IViewGroupLayoutParams**)&highlightLp);

    r *= 0.2f;
    AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(viewLp);
    AutoPtr<IStackViewLayoutParams> shlp = IStackViewLayoutParams::Probe(highlightLp);
    assert(slp && shlp);
    slp->SetHorizontalOffset(Elastos::Core::Math::Round(r * mHost->mSlideAmount));
    shlp->SetHorizontalOffset(Elastos::Core::Math::Round(r * mHost->mSlideAmount));
}

void StackView::StackSlider::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
}

Float StackView::StackSlider::GetDurationForNeutralPosition()
{
    return GetDuration(FALSE, 0);
}

Float StackView::StackSlider::GetDurationForOffscreenPosition()
{
     return GetDuration(TRUE, 0);
}

Float StackView::StackSlider::GetDurationForNeutralPosition(
    /* [in] */ Float r)
{
    return GetDuration(FALSE, r);
}

Float StackView::StackSlider::GetDurationForOffscreenPosition(
    /* [in] */ Float r)
{
    return GetDuration(TRUE, r);
}

Float StackView::StackSlider::GetYProgress()
{
    return mYProgress;
}

Float StackView::StackSlider::GetXProgress()
{
    return mXProgress;
}

Float StackView::StackSlider::Cubic(
    /* [in] */ Float r)
{
    return (Float) (Elastos::Core::Math::Pow(2 * r - 1, 3) + 1) / 2.0f;
}

Float StackView::StackSlider::HighlightAlphaInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.4f;
    if (r < pivot) {
        return 0.85f * Cubic(r / pivot);
    }
    else {
        return 0.85f * Cubic(1 - (r - pivot) / (1 - pivot));
    }
}

Float StackView::StackSlider::ViewAlphaInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.3f;
    if (r > pivot) {
        return (r - pivot) / (1 - pivot);
    }
    else {
        return 0;
    }
}

Float StackView::StackSlider::RotationInterpolator(
    /* [in] */ Float r)
{
    Float pivot = 0.2f;
    if (r < pivot) {
        return 0;
    }
    else {
        return (r - pivot) / (1 - pivot);
    }
}

Float StackView::StackSlider::GetDuration(
    /* [in] */ Boolean invert,
    /* [in] */ Float r)
{
    if (mView) {
        AutoPtr<IViewGroupLayoutParams> viewLp;
        mView->GetLayoutParams((IViewGroupLayoutParams**)&viewLp);

        AutoPtr<StackView::LayoutParams> slp = (StackView::LayoutParams*)viewLp.Get();
        Float d = (Float) Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(slp->mHorizontalOffset, 2)
            + Elastos::Core::Math::Pow(slp->mVerticalOffset, 2));
        Float maxd = (Float) Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(mHost->mSlideAmount, 2)
            + Elastos::Core::Math::Pow(0.4f * mHost->mSlideAmount, 2));

        if (r == 0) {
            return (invert ? (1 - d / maxd) : d / maxd) * DEFAULT_ANIMATION_DURATION;
        }
        else {
            Float duration = invert ? d / Elastos::Core::Math::Abs(r) :
                    (maxd - d) / Elastos::Core::Math::Abs(r);
            if (duration < MINIMUM_ANIMATION_DURATION || duration > DEFAULT_ANIMATION_DURATION) {
                return GetDuration(invert, 0);
            }
            else {
                return duration;
            }
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//                  StackView::PostRun
///////////////////////////////////////////////////////////////////////
StackView::PostRun::PostRun(
    /* [in] */ IView* v)
    : mView(v)
{
}

ECode StackView::PostRun::Run()
{
    mView->SetAlpha(0);
    return NOERROR;
}

const String StackView::TAG("StackView");
const Int32 StackView::DEFAULT_ANIMATION_DURATION;
const Int32 StackView::MINIMUM_ANIMATION_DURATION;
const Int32 StackView::STACK_RELAYOUT_DURATION;
const Int32 StackView::ITEMS_SLIDE_UP;
const Int32 StackView::ITEMS_SLIDE_DOWN;
const Int32 StackView::GESTURE_NONE;
const Int32 StackView::GESTURE_SLIDE_UP;
const Int32 StackView::GESTURE_SLIDE_DOWN;
const Int32 StackView::INVALID_POINTER;
const Int32 StackView::NUM_ACTIVE_VIEWS;
const Int32 StackView::FRAME_PADDING;
const Int32 StackView::MIN_TIME_BETWEEN_INTERACTION_AND_AUTOADVANCE;
const Float StackView::PERSPECTIVE_SHIFT_FACTOR_Y;
const Float StackView::PERSPECTIVE_SHIFT_FACTOR_X;
const Float StackView::PERSPECTIVE_SCALE_FACTOR;
const Float StackView::SWIPE_THRESHOLD_RATIO;
const Float StackView::SLIDE_UP_RATIO;
const Int64 StackView::MIN_TIME_BETWEEN_SCROLLS;

AutoPtr<IRect> InitStatic()
{
    AutoPtr<IRect> temp;
    CRect::New((IRect**)&temp);
    return temp;
}

AutoPtr<IRect> StackView::sStackInvalidateRect = InitStatic();
AutoPtr<StackView::HolographicHelper> StackView::sHolographicHelper;

CAR_INTERFACE_IMPL(StackView, AdapterViewAnimator, IStackView)

StackView::StackView()
    : mPerspectiveShiftX(0)
    , mPerspectiveShiftY(0)
    , mNewPerspectiveShiftX(0)
    , mNewPerspectiveShiftY(0)
    , mInitialY(0)
    , mInitialX(0)
    , mActivePointerId(0)
    , mYVelocity(0)
    , mSwipeGestureType(GESTURE_NONE)
    , mSlideAmount(0)
    , mSwipeThreshold(0)
    , mTouchSlop(0)
    , mMaximumVelocity(0)
    , mResOutColor(0)
    , mClickColor(0)
    , mTransitionIsSetup(FALSE)
    , mClickFeedbackIsValid(FALSE)
    , mFirstLayoutHappened(FALSE)
    , mLastInteractionTime(0)
    , mLastScrollTime(0)
    , mStackMode(0)
    , mFramePadding(0)
{
    CRect::New((IRect**)&mTouchRect);
}

ECode StackView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ Int32 defStyleRes)
{
    AdapterViewAnimator::constructor(context, attrs, defStyle, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::StackView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    a->GetColor(R::styleable::StackView_resOutColor, 0, &mResOutColor);
    a->GetColor(R::styleable::StackView_clickColor, 0, &mClickColor);

    a->Recycle();
    InitStackView();
    return NOERROR;
}

void StackView::InitStackView()
{
    ConfigureViewAnimator(NUM_ACTIVE_VIEWS, 1);
    SetStaticTransformationsEnabled(TRUE);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    mActivePointerId = INVALID_POINTER;
    CImageView::New(context, (IImageView**)&mHighlight);
    AutoPtr<IView> highlight = IView::Probe(mHighlight);
    AutoPtr<IViewGroupLayoutParams> hLP;
    CStackViewLayoutParams::New(highlight, (IViewGroupLayoutParams**)&hLP);
    highlight->SetLayoutParams(hLP);
    AddViewInLayout(highlight, -1, hLP);

    CImageView::New(context, (IImageView**)&mClickFeedback);
    AutoPtr<IView> clickFeedback = IView::Probe(mClickFeedback);
    AutoPtr<IViewGroupLayoutParams> cLP;
    CStackViewLayoutParams::New(clickFeedback, (IViewGroupLayoutParams**)&cLP);
    clickFeedback->SetLayoutParams(cLP);
    AddViewInLayout(clickFeedback, -1, cLP);
    clickFeedback->SetVisibility(IView::INVISIBLE);

    mStackSlider = new StackSlider(this);

    if (!sHolographicHelper) {
        sHolographicHelper = new HolographicHelper(mContext);
    }

    SetClipChildren(FALSE);
    SetClipToPadding(FALSE);

    mStackMode = ITEMS_SLIDE_DOWN;
    mWhichChild = -1;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density = 0;
    dm->GetDensity(&density);

    mFramePadding = (Int32)Elastos::Core::Math::Ceil(density * FRAME_PADDING);
}

void StackView::TransformViewForTransition(
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex,
    /* [in] */ IView* view,
    /* [in] */ Boolean animate)
{
    if (animate) {
        AutoPtr<StackFrame> sf = (StackFrame*)view;
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        AutoPtr<IViewGroupLayoutParams> lp;
        view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        slp->SetVerticalOffset(0);
        slp->SetHorizontalOffset(0);
    }

    if (fromIndex == -1 && toIndex == GetNumActiveViews() - 1) {
        TransformViewAtIndex(toIndex, view, FALSE);
        view->SetVisibility(IView::VISIBLE);
        view->SetAlpha(1.0f);
    }
    else if (fromIndex == 0 && toIndex == 1) {
        AutoPtr<StackFrame> sf = (StackFrame*)view;
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        view->SetVisibility(IView::VISIBLE);
        Int32 duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition(mYVelocity));
        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        animationSlider->SetView(view);

        if(animate) {
            AutoPtr<IPropertyValuesHolderHelper> helper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
            AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 0.0f;
            AutoPtr<IPropertyValuesHolder> slideInY, slideInX;
            helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&slideInY);
            helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&slideInX);

            AutoPtr<IObjectAnimatorHelper> oaHelp;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
            AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
            pArray->Set(0, slideInX);
            pArray->Set(1, slideInY);
            AutoPtr<IObjectAnimator> slideIn;
            oaHelp->OfPropertyValuesHolder((IObject*)animationSlider, pArray, (IObjectAnimator**)&slideIn);
            AutoPtr<IAnimator> animator = IAnimator::Probe(slideIn);
            animator->SetDuration(duration);

            AutoPtr<ITimeInterpolator> lip;
            CLinearInterpolator::New((ITimeInterpolator**)&lip);
            animator->SetInterpolator(lip);
            sf->SetSliderAnimator(slideIn);
            animator->Start();
        }
        else {
            animationSlider->SetYProgress(0.0f);
            animationSlider->SetXProgress(0.0f);
        }
    }
    else if (fromIndex == 1 && toIndex == 0) {
        AutoPtr<StackFrame> sf = (StackFrame*)view;
        assert(sf != NULL);
        sf->CancelSliderAnimator();
        Int32 duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition(mYVelocity));

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        animationSlider->SetView(view);
        if (animate) {
            AutoPtr<IPropertyValuesHolderHelper> helper;
            CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
            AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
            (*array)[0] = 1.0f;
            AutoPtr<IPropertyValuesHolder> slideOutY, slideOutX;
            helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&slideOutY);
            (*array)[0] = 0.0f;
            helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&slideOutX);

            AutoPtr<IObjectAnimatorHelper> oaHelp;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
            AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
            pArray->Set(0, slideOutX);
            pArray->Set(1, slideOutY);
            AutoPtr<IObjectAnimator> slideOut;
            oaHelp->OfPropertyValuesHolder((IObject*)animationSlider, pArray, (IObjectAnimator**)&slideOut);
            AutoPtr<IAnimator> animator = IAnimator::Probe(slideOut);
            animator->SetDuration(duration);

            AutoPtr<ITimeInterpolator> lip;
            CLinearInterpolator::New((ITimeInterpolator**)&lip);
            animator->SetInterpolator(lip);
            sf->SetSliderAnimator(slideOut);
            animator->Start();
        }
        else {
            animationSlider->SetYProgress(1.0f);
            animationSlider->SetXProgress(0.0f);
        }
    }
    else if (toIndex == 0) {
        view->SetAlpha(0.0f);
        view->SetVisibility(IView::INVISIBLE);
    }
    else if ((fromIndex == 0 || fromIndex == 1) && toIndex > 1) {
        view->SetVisibility(IView::VISIBLE);
        view->SetAlpha(1.0f);
        view->SetRotationX(0.0f);
        AutoPtr<IViewGroupLayoutParams> lp;
        view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(lp);
        slp->SetVerticalOffset(0);
        slp->SetHorizontalOffset(0);
    }
    else if (fromIndex == -1) {
        view->SetAlpha(1.0f);
        view->SetVisibility(IView::VISIBLE);
    }
    else if (toIndex == -1) {
        if (animate) {
            AutoPtr<IRunnable> run = new PostRun(view);
            Boolean res;
            PostDelayed(run, STACK_RELAYOUT_DURATION, &res);
        }
        else {
            view->SetAlpha(0.0f);
        }
    }

    if (toIndex != -1) {
        TransformViewAtIndex(toIndex, view, animate);
    }
}

void StackView::TransformViewAtIndex(
        /* [in] */ Int32 index,
        /* [in] */ IView* view,
        /* [in] */ Int32 animate)
{
    Float maxPerspectiveShiftY = mPerspectiveShiftY;
    Float maxPerspectiveShiftX = mPerspectiveShiftX;

    if (mStackMode == ITEMS_SLIDE_DOWN) {
        index = mMaxNumActiveViews - index - 1;
        if (index == mMaxNumActiveViews - 1) index--;
    }
    else {
        index--;
        if (index < 0) index++;
    }

    Float r = (index * 1.0f) / (mMaxNumActiveViews - 2);

    Float scale = 1 - PERSPECTIVE_SCALE_FACTOR * (1 - r);

    Int32 measuredWidth, measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    GetMeasuredWidth(&measuredWidth);
    Float perspectiveTranslationY = r * maxPerspectiveShiftY;
    Float scaleShiftCorrectionY = (scale - 1) * (measuredHeight * (1 - PERSPECTIVE_SHIFT_FACTOR_Y) / 2.0f);
    Float transY = perspectiveTranslationY + scaleShiftCorrectionY;

    Float perspectiveTranslationX = (1 - r) * maxPerspectiveShiftX;
    Float scaleShiftCorrectionX =  (1 - scale) * (measuredWidth * (1 - PERSPECTIVE_SHIFT_FACTOR_X) / 2.0f);
    Float transX = perspectiveTranslationX + scaleShiftCorrectionX;

    if (IStackFrame::Probe(view) != NULL) {
        AutoPtr<StackFrame> sf = (StackFrame*)view;
        assert(sf != NULL);
        sf->CancelTransformAnimator();
    }

    if (animate) {
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        AutoPtr<IPropertyValuesHolder> translationX, translationY, scalePropX, scalePropY;
        (*array)[0] = transX;
        helper->OfFloat(String("translationX"), array, (IPropertyValuesHolder**)&translationX);
        (*array)[0] = transY;
        helper->OfFloat(String("translationY"), array, (IPropertyValuesHolder**)&translationY);
        (*array)[0] = scale;
        helper->OfFloat(String("scaleX"), array, (IPropertyValuesHolder**)&scalePropX);
        helper->OfFloat(String("scaleY"), array, (IPropertyValuesHolder**)&scalePropY);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(4);
        pArray->Set(0, scalePropX);
        pArray->Set(1, scalePropY);
        pArray->Set(2, translationY);
        pArray->Set(3, translationX);
        AutoPtr<IObjectAnimator> oa;
        oaHelp->OfPropertyValuesHolder(view, pArray, (IObjectAnimator**)&oa);
        IAnimator::Probe(oa)->SetDuration(STACK_RELAYOUT_DURATION);
        AutoPtr<StackFrame> sf = (StackFrame*)view;
        if (sf) {
            sf->SetTransformAnimator(oa);
        }
        IAnimator::Probe(oa)->Start();
    }
    else {
        view->SetTranslationX(transX);
        view->SetTranslationY(transY);
        view->SetScaleX(scale);
        view->SetScaleY(scale);
    }
}

void StackView::SetupStackSlider(
    /* [in] */ IView* view,
    /* [in] */ Int32 mode)
{
    mStackSlider->SetMode(mode);
    if (view) {
        mHighlight->SetImageBitmap(sHolographicHelper->CreateResOutline(view, mResOutColor));
        AutoPtr<IView> highlight = IView::Probe(mHighlight);
        Float rotation = 0, ty = 0, tx = 0;
        view->GetRotation(&rotation);
        highlight->SetRotation(rotation);
        view->GetTranslationY(&ty);
        highlight->SetTranslationY(ty);
        view->GetTranslationX(&tx);
        highlight->SetTranslationX(tx);
        highlight->BringToFront();
        view->BringToFront();
        mStackSlider->SetView(view);

        view->SetVisibility(IView::VISIBLE);
    }
}

ECode StackView::ShowNext()
{
    if (mSwipeGestureType != GESTURE_NONE) return NOERROR;
    if (!mTransitionIsSetup) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(1);
        if (v ) {
            SetupStackSlider(v, StackSlider::NORMAL_MODE);
            mStackSlider->SetYProgress(0);
            mStackSlider->SetXProgress(0);
        }
    }
    AdapterViewAnimator::ShowNext();
    return NOERROR;
}

ECode StackView::ShowPrevious()
{
    if (mSwipeGestureType != GESTURE_NONE) return NOERROR;
    if (!mTransitionIsSetup) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(0);
        if (v) {
            SetupStackSlider(v, StackSlider::NORMAL_MODE);
            mStackSlider->SetYProgress(1);
            mStackSlider->SetXProgress(0);
        }
    }
    AdapterViewAnimator::ShowPrevious();
    return NOERROR;
}

void StackView::ShowOnly(
    /* [in] */ Int32 childIndex,
    /* [in] */ Boolean animate)
{
    AdapterViewAnimator::ShowOnly(childIndex, animate);

    for (Int32 i = mCurrentWindowEnd; i >= mCurrentWindowStart; i--) {
        Int32 index = Modulo(i, GetWindowSize());
        if (mViewsMap.Find(index) != mViewsMap.End()) {
            AutoPtr<ViewAndMetaData> vm = mViewsMap.Find(index)->mSecond;
            if (vm) {
                AutoPtr<IView> v =vm->mView;
                if (v) v->BringToFront();
            }
        }
    }
    if (mHighlight) {
        IView::Probe(mHighlight)->BringToFront();
    }
    mTransitionIsSetup = FALSE;
    mClickFeedbackIsValid = FALSE;
}

void StackView::UpdateClickFeedback()
{
    if (!mClickFeedbackIsValid) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(1);
        if (v) {
            AutoPtr<IBitmap> map = sHolographicHelper->CreateClickOutline(v, mClickColor);
            mClickFeedback->SetImageBitmap(map);
            Float x = 0, y = 0;
            v->GetTranslationX(&x);
            v->GetTranslationY(&y);
            IView::Probe(mClickFeedback)->SetTranslationX(x);
            IView::Probe(mClickFeedback)->SetTranslationY(y);
        }
        mClickFeedbackIsValid = TRUE;
    }
}

void StackView::ShowTapFeedback(
    /* [in] */ IView* child)
{
    UpdateClickFeedback();
    IView::Probe(mClickFeedback)->SetVisibility(IView::VISIBLE);
    IView::Probe(mClickFeedback)->BringToFront();
    Invalidate();
}

void StackView::HideTapFeedback(
    /* [in] */ IView* child)
{
    IView::Probe(mClickFeedback)->SetVisibility(IView::INVISIBLE);
    Invalidate();
}

void StackView::UpdateChildTransforms()
{
    for (Int32 i = 0; i < GetNumActiveViews(); i++) {
        AutoPtr<IView> v = GetViewAtRelativeIndex(i);
        if (v) {
            TransformViewAtIndex(i, v, FALSE);
        }
    }
}

AutoPtr<IFrameLayout> StackView::GetFrameForChild()
{
    AutoPtr<CStackViewStackFrame> fl;
    CStackViewStackFrame::NewByFriend(mContext, (CStackViewStackFrame**)&fl);
    fl->SetPadding(mFramePadding, mFramePadding, mFramePadding, mFramePadding);
    return fl;
}

void StackView::ApplyTransformForChildAtIndex(
    /* [in] */ IView* child,
    /* [in] */ Int32 relativeIndex)
{
    //do nothing
}

ECode StackView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Boolean expandClipRegion = FALSE;
    Boolean res = FALSE;
    canvas->GetClipBounds(sStackInvalidateRect, &res);
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<StackView::LayoutParams> slp = (StackView::LayoutParams*)lp.Get();
        Int32 visible = 0;
        child->GetVisibility(&visible);
        Float alpha = 0.0f;
        child->GetAlpha(&alpha);
        if ((slp->mHorizontalOffset == 0 && slp->mVerticalOffset == 0) || alpha == 0.0f || visible != IView::VISIBLE) {
            slp->ResetInvalidateRect();
        }
        AutoPtr<IRect> childInvalidateRect = slp->GetInvalidateRect();
        childInvalidateRect->IsEmpty(&res);
        if (!res) {
            expandClipRegion = TRUE;
            sStackInvalidateRect->Union(childInvalidateRect);
        }
    }

    if (expandClipRegion) {
        Int32 result = 0;
        canvas->Save(ICanvas::CLIP_SAVE_FLAG, &result);
        canvas->ClipRect(sStackInvalidateRect, RegionOp_UNION, &res);
        AdapterViewAnimator::DispatchDraw(canvas);
        canvas->Restore();
    }
    else {
        AdapterViewAnimator::DispatchDraw(canvas);
    }
    return NOERROR;
}

void StackView::OnLayout()
{
    if (!mFirstLayoutHappened) {
        mFirstLayoutHappened = TRUE;
        UpdateChildTransforms();
    }

    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    Int32 newSlideAmount = Elastos::Core::Math::Round(SLIDE_UP_RATIO * measuredHeight);
    if (mSlideAmount != newSlideAmount) {
        mSlideAmount = newSlideAmount;
        mSwipeThreshold = Elastos::Core::Math::Round(SWIPE_THRESHOLD_RATIO * newSlideAmount);
    }

    if (Elastos::Core::Math::Compare(mPerspectiveShiftY, mNewPerspectiveShiftY) != 0 ||
            Elastos::Core::Math::Compare(mPerspectiveShiftX, mNewPerspectiveShiftX) != 0) {

        mPerspectiveShiftY = mNewPerspectiveShiftY;
        mPerspectiveShiftX = mNewPerspectiveShiftX;
        UpdateChildTransforms();
    }
}

ECode StackView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 source = 0;
    Elastos::Droid::View::IInputEvent::Probe(event)->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action = 0;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_SCROLL: {
                Float vscroll = 0;
                event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                if (vscroll < 0) {
                    PacedScroll(FALSE);
                    *res = TRUE;
                    return NOERROR;
                }
                else if (vscroll > 0) {
                    PacedScroll(TRUE);
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    return AdapterViewAnimator::OnGenericMotionEvent(event, res);
}

void StackView::PacedScroll(
    /* [in] */ Boolean up)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 timeSinceLastScroll = millis - mLastScrollTime;
    if (timeSinceLastScroll > MIN_TIME_BETWEEN_SCROLLS) {
        if (up) {
            ShowPrevious();
        }
        else {
            ShowNext();
        }
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mLastScrollTime);
    }
}

ECode StackView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 action = 0;
    ev->GetAction(&action);
    switch(action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN:
        {
            if (mActivePointerId == INVALID_POINTER) {
                ev->GetX(&mInitialX);
                ev->GetY(&mInitialY);
                ev->GetPointerId(0, &mActivePointerId);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE:
        {
            Int32 pointerIndex = 0;
            ev->FindPointerIndex(mActivePointerId, &pointerIndex);
            if (pointerIndex == INVALID_POINTER) {
                //Log.d(TAG, "Error: No data for our primary pointer.");
                *res = FALSE;
                return NOERROR;
            }
            Float newY = 0;
            ev->GetY(pointerIndex, &newY);
            Float deltaY = newY - mInitialY;

            BeginGestureIfNeeded(deltaY);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            mActivePointerId = INVALID_POINTER;
            mSwipeGestureType = GESTURE_NONE;
        }
    }

    *res = mSwipeGestureType != GESTURE_NONE;
    return NOERROR;
}

void StackView::BeginGestureIfNeeded(
    /* [in] */ Float deltaY)
{
    if ((Int32)Elastos::Core::Math::Abs(deltaY) > mTouchSlop && mSwipeGestureType == GESTURE_NONE) {
        Int32 swipeGestureType = deltaY < 0 ? GESTURE_SLIDE_UP : GESTURE_SLIDE_DOWN;
        CancelLongPress();
        RequestDisallowInterceptTouchEvent(TRUE);

        if (!mAdapter) return;
        Int32 adapterCount;
        GetCount(&adapterCount);

        Int32 activeIndex = 0;
        if (mStackMode == ITEMS_SLIDE_UP) {
            activeIndex = (swipeGestureType == GESTURE_SLIDE_DOWN) ? 0 : 1;
        }
        else {
            activeIndex = (swipeGestureType == GESTURE_SLIDE_DOWN) ? 1 : 0;
        }

        Boolean endOfStack = mLoopViews && adapterCount == 1 &&
            ((mStackMode == ITEMS_SLIDE_UP && swipeGestureType == GESTURE_SLIDE_UP) ||
             (mStackMode == ITEMS_SLIDE_DOWN && swipeGestureType == GESTURE_SLIDE_DOWN));
        Boolean beginningOfStack = mLoopViews && adapterCount == 1 &&
            ((mStackMode == ITEMS_SLIDE_DOWN && swipeGestureType == GESTURE_SLIDE_UP) ||
             (mStackMode == ITEMS_SLIDE_UP && swipeGestureType == GESTURE_SLIDE_DOWN));

        Int32 stackMode = 0;
        if (mLoopViews && !beginningOfStack && !endOfStack) {
            stackMode = StackSlider::NORMAL_MODE;
        }
        else if (mCurrentWindowStartUnbounded + activeIndex == -1 || beginningOfStack) {
            activeIndex++;
            stackMode = StackSlider::BEGINNING_OF_STACK_MODE;
        }
        else if (mCurrentWindowStartUnbounded + activeIndex == adapterCount - 1 || endOfStack) {
            stackMode = StackSlider::END_OF_STACK_MODE;
        }
        else {
            stackMode = StackSlider::NORMAL_MODE;
        }

        mTransitionIsSetup = stackMode == StackSlider::NORMAL_MODE;

        AutoPtr<IView> v = GetViewAtRelativeIndex(activeIndex);
        if (!v) return;

        SetupStackSlider(v, stackMode);

        mSwipeGestureType = swipeGestureType;
        CancelHandleClick();
    }
}

ECode StackView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AdapterViewAnimator::OnTouchEvent(ev, res);

    Int32 action = 0;
    ev->GetAction(&action);
    Int32 pointerIndex = 0;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == INVALID_POINTER) {
        // no data for our primary pointer, this shouldn't happen, log it
        //Log.d(TAG, "Error: No data for our primary pointer.");
        *res = FALSE;
        return NOERROR;
    }

    Float newY = 0, newX = 0;
    ev->GetY(pointerIndex, &newY);
    ev->GetX(pointerIndex, &newX);
    Float deltaY = newY - mInitialY;
    Float deltaX = newX - mInitialX;
    if (!mVelocityTracker) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    mVelocityTracker->AddMovement(ev);

    switch (action & IMotionEvent::ACTION_MASK)
    {
        case IMotionEvent::ACTION_MOVE:
        {
            BeginGestureIfNeeded(deltaY);

            Float rx = deltaX / (mSlideAmount * 1.0f);
            if (mSwipeGestureType == GESTURE_SLIDE_DOWN) {
                Float r = (deltaY - mTouchSlop * 1.0f) / mSlideAmount * 1.0f;
                if (mStackMode == ITEMS_SLIDE_DOWN) r = 1 - r;
                mStackSlider->SetYProgress(1 - r);
                mStackSlider->SetXProgress(rx);
                *res = TRUE;
                return NOERROR;
            }
            else if (mSwipeGestureType == GESTURE_SLIDE_UP) {
                Float r = -(deltaY + mTouchSlop * 1.0f) / mSlideAmount * 1.0f;
                if (mStackMode == ITEMS_SLIDE_DOWN) r = 1 - r;
                mStackSlider->SetYProgress(r);
                mStackSlider->SetXProgress(rx);
                *res = TRUE;
                return NOERROR;
            }
            break;
        }
        case IMotionEvent::ACTION_UP:
        {
            HandlePointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
        {
            OnSecondaryPointerUp(ev);
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        {
            mActivePointerId = INVALID_POINTER;
            mSwipeGestureType = GESTURE_NONE;
            break;
        }
    }
    *res = TRUE;
    return NOERROR;
}

void StackView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 activePointerIndex = 0 , pointerId = 0;
    ev->GetActionIndex(&activePointerIndex);
    ev->GetPointerId(activePointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {

        Int32 activeViewIndex = (mSwipeGestureType == GESTURE_SLIDE_DOWN) ? 0 : 1;

        AutoPtr<IView> v = GetViewAtRelativeIndex(activeViewIndex);
        if (!v) return;
        Int32 pointerCount = 0;
        ev->GetPointerCount(&pointerCount);
        for (Int32 index = 0; index < pointerCount; index++) {
            if (index != activePointerIndex) {

                Float x = 0, y = 0;
                ev->GetX(index, &x);
                ev->GetY(index, &y);
                Int32 left = 0, top = 0, right = 0, bottom = 0;
                v->GetLeft(&left);
                v->GetTop(&top);
                v->GetRight(&right);
                v->GetBottom(&bottom);
                mTouchRect->Set(left, top, right, bottom);
                Boolean res = FALSE;
                mTouchRect->Contains(Elastos::Core::Math::Round(x), Elastos::Core::Math::Round(y), &res);
                if (res) {
                    Float oldX = 0, oldY =0;
                    ev->GetX(activePointerIndex, &oldX);
                    ev->GetY(activePointerIndex, &oldY);

                    mInitialY += (y - oldY);
                    mInitialX += (x - oldX);

                    ev->GetPointerId(index, &mActivePointerId);
                    if (mVelocityTracker) {
                        mVelocityTracker->Clear();
                    }

                    return;
                }
            }
        }

        HandlePointerUp(ev);
    }
}

void StackView::HandlePointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 pointerIndex = 0;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    Float newY = 0;
    ev->GetY(pointerIndex, &newY);
    Int32 deltaY = (Int32) (newY - mInitialY);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastInteractionTime);

    if (mVelocityTracker) {
        mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
        Float y;
        mVelocityTracker->GetYVelocity(mActivePointerId, &y);
        mYVelocity = (Int32)y;
    }

    if (mVelocityTracker) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }

    if (deltaY > mSwipeThreshold && mSwipeGestureType == GESTURE_SLIDE_DOWN
            && mStackSlider->mMode == StackSlider::NORMAL_MODE) {
        mSwipeGestureType = GESTURE_NONE;

        if (mStackMode == ITEMS_SLIDE_UP) {
            ShowPrevious();
        }
        else {
            ShowNext();
        }
        IView::Probe(mHighlight)->BringToFront();
    }
    else if (deltaY < -mSwipeThreshold && mSwipeGestureType == GESTURE_SLIDE_UP
            && mStackSlider->mMode == StackSlider::NORMAL_MODE) {
        mSwipeGestureType = GESTURE_NONE;

        if (mStackMode == ITEMS_SLIDE_UP) {
            ShowNext();
        }
        else {
            ShowPrevious();
        }

        IView::Probe(mHighlight)->BringToFront();
    }
    else if (mSwipeGestureType == GESTURE_SLIDE_UP ) {
        Int32 duration = 0;
        Float finalYProgress = (mStackMode == ITEMS_SLIDE_DOWN) ? 1 : 0;
        if (mStackMode == ITEMS_SLIDE_UP || mStackSlider->mMode != StackSlider::NORMAL_MODE) {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition());
        }
        else {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition());
        }

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalYProgress;
        AutoPtr<IPropertyValuesHolder> snapBackY;
        helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&snapBackY);
        (*array)[0] = 0.0f;
        AutoPtr<IPropertyValuesHolder> snapBackX;
        helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&snapBackX);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
        pArray->Set(0, snapBackX);
        pArray->Set(1, snapBackY);
        AutoPtr<IObjectAnimator> pa;
        oaHelp->OfPropertyValuesHolder((IObject*)animationSlider, pArray, (IObjectAnimator**)&pa);
        AutoPtr<IAnimator> animator = IAnimator::Probe(pa);
        animator->SetDuration(duration);
        AutoPtr<ITimeInterpolator> lip;
        CLinearInterpolator::New((ITimeInterpolator**)&lip);
        animator->SetInterpolator(lip);
        animator->Start();
    }
    else if (mSwipeGestureType == GESTURE_SLIDE_DOWN) {
        Float finalYProgress = (mStackMode == ITEMS_SLIDE_DOWN) ? 0 : 1;
        Int32 duration = 0;
        if (mStackMode == ITEMS_SLIDE_DOWN || mStackSlider->mMode != StackSlider::NORMAL_MODE) {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForNeutralPosition());
        }
        else {
            duration = Elastos::Core::Math::Round(mStackSlider->GetDurationForOffscreenPosition());
        }

        AutoPtr<StackSlider> animationSlider = new StackSlider(*mStackSlider);
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
        AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = finalYProgress;
        AutoPtr<IPropertyValuesHolder> snapBackY;
        helper->OfFloat(String("YProgress"), array, (IPropertyValuesHolder**)&snapBackY);
        (*array)[0] = 0.0f;
        AutoPtr<IPropertyValuesHolder> snapBackX;
        helper->OfFloat(String("XProgress"), array, (IPropertyValuesHolder**)&snapBackX);

        AutoPtr<IObjectAnimatorHelper> oaHelp;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelp);
        AutoPtr< ArrayOf<IPropertyValuesHolder*> > pArray = ArrayOf<IPropertyValuesHolder*>::Alloc(2);
        pArray->Set(0, snapBackX);
        pArray->Set(1, snapBackY);
        AutoPtr<IObjectAnimator> pa;
        oaHelp->OfPropertyValuesHolder((IObject*)animationSlider, pArray, (IObjectAnimator**)&pa);
        AutoPtr<IAnimator> animator = IAnimator::Probe(pa);
        animator->SetDuration(duration);
        animator->Start();
    }

    mActivePointerId = INVALID_POINTER;
    mSwipeGestureType = GESTURE_NONE;
}

AutoPtr<IViewGroupLayoutParams> StackView::CreateOrReuseLayoutParams(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> currentLp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&currentLp);
    AutoPtr<IStackViewLayoutParams> slp = IStackViewLayoutParams::Probe(currentLp);
    if (slp) {
        slp->SetHorizontalOffset(0);
        slp->SetVerticalOffset(0);
        currentLp->SetWidth(0);
    }
    else {
        CStackViewLayoutParams::New(v, (IStackViewLayoutParams**)&slp);
    }
    return IViewGroupLayoutParams::Probe(slp);
}

ECode StackView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    CheckForAndHandleDataChanged();

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 childRight = 0;
        child->GetMeasuredWidth(&childRight);
        childRight += mPaddingLeft;
        Int32 childBottom = 0;
        child->GetMeasuredHeight(&childBottom);
        childBottom += mPaddingTop;
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AutoPtr<StackView::LayoutParams> slp = (StackView::LayoutParams*)lp.Get();
        child->Layout(mPaddingLeft + slp->mHorizontalOffset, mPaddingTop + slp->mVerticalOffset,
                childRight + slp->mHorizontalOffset, childBottom + slp->mVerticalOffset);

    }
    OnLayout();
    return NOERROR;
}

ECode StackView::Advance()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 timeSinceLastInteraction = millis - mLastInteractionTime;

    if (!mAdapter) return NOERROR;
    Int32 adapterCount;
    GetCount(&adapterCount);
    if (adapterCount == 1 && mLoopViews) return NOERROR;

    if (mSwipeGestureType == GESTURE_NONE &&
            timeSinceLastInteraction > MIN_TIME_BETWEEN_INTERACTION_AND_AUTOADVANCE) {
        ShowNext();
    }
    return NOERROR;
}

void StackView::MeasureChildren()
{
    Int32 count;
    GetChildCount(&count);

    Int32 measuredWidth, measuredHeight;
    GetMeasuredWidth(&measuredWidth);
    GetMeasuredHeight(&measuredHeight);

    Int32 childWidth = Elastos::Core::Math::Round(measuredWidth*(1-PERSPECTIVE_SHIFT_FACTOR_X)) - mPaddingLeft - mPaddingRight;
    Int32 childHeight = Elastos::Core::Math::Round(measuredHeight*(1-PERSPECTIVE_SHIFT_FACTOR_Y)) - mPaddingTop - mPaddingBottom;

    Int32 maxWidth = 0;
    Int32 maxHeight = 0;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->Measure(MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::AT_MOST),
            MeasureSpec::MakeMeasureSpec(childHeight, MeasureSpec::AT_MOST));

        if (child.Get() != IView::Probe(mHighlight) && child.Get() != IView::Probe(mClickFeedback)) {
            Int32 childMeasuredWidth = 0;
            child->GetMeasuredWidth(&childMeasuredWidth);
            Int32 childMeasuredHeight = 0;
            child->GetMeasuredHeight(&childMeasuredHeight);
            if (childMeasuredWidth > maxWidth) {
                maxWidth = childMeasuredWidth;
            }
            if (childMeasuredHeight > maxHeight) {
                maxHeight = childMeasuredHeight;
            }
        }
    }

    mNewPerspectiveShiftX = PERSPECTIVE_SHIFT_FACTOR_X * measuredWidth;
    mNewPerspectiveShiftY = PERSPECTIVE_SHIFT_FACTOR_Y * measuredHeight;

    if (maxWidth > 0 && count > 0 && maxWidth < childWidth) {
        mNewPerspectiveShiftX = measuredWidth - maxWidth;
    }

    if (maxHeight > 0 && count > 0 && maxHeight < childHeight) {
        mNewPerspectiveShiftY = measuredHeight - maxHeight;
    }
}

ECode StackView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSpecSize = MeasureSpec::GetSize(heightMeasureSpec);
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean haveChildRefSize = (mReferenceChildWidth != -1 && mReferenceChildHeight != -1);

    Float factorY = 1/(1 - PERSPECTIVE_SHIFT_FACTOR_Y);
    if (heightSpecMode == MeasureSpec::UNSPECIFIED) {
        heightSpecSize = haveChildRefSize ? Elastos::Core::Math::Round(mReferenceChildHeight * (1 + factorY)) +
                mPaddingTop + mPaddingBottom : 0;
    }
    else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 height = Elastos::Core::Math::Round(mReferenceChildHeight * (1 + factorY)) + mPaddingTop + mPaddingBottom;
            if (height <= heightSpecSize) {
                heightSpecSize = height;
            }
            else {
                heightSpecSize |= IView::MEASURED_STATE_TOO_SMALL;

            }
        }
        else {
            heightSpecSize = 0;
        }
    }

    Float factorX = 1/(1 - PERSPECTIVE_SHIFT_FACTOR_X);
    if (widthSpecMode == MeasureSpec::UNSPECIFIED) {
        widthSpecSize = haveChildRefSize ? Elastos::Core::Math::Round(mReferenceChildWidth * (1 + factorX)) +
                mPaddingLeft + mPaddingRight : 0;
    }
    else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 width = mReferenceChildWidth + mPaddingLeft + mPaddingRight;
            if (width <= widthSpecSize) {
                widthSpecSize = width;
            }
            else {
                widthSpecSize |= IView::MEASURED_STATE_TOO_SMALL;
            }
        }
        else {
            widthSpecSize = 0;
        }
    }
    SetMeasuredDimension(widthSpecSize, heightSpecSize);
    MeasureChildren();
    return NOERROR;
}

ECode StackView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AdapterView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CStackView"), (ICharSequence**)&seq);
    return Elastos::Droid::View::Accessibility::IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode StackView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AdapterView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CStackView"), (ICharSequence**)&seq);
    info->SetClassName(seq);
    Int32 childCount;
    GetChildCount(&childCount);
    info->SetScrollable(childCount > 1);
    Boolean res;
    IsEnabled(&res);
    if (res) {
        Int32 index;
        GetDisplayedChild(&index);
        if (index < childCount - 1) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
        if (index > 0) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
    }
    return NOERROR;
}

ECode StackView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AdapterView::PerformAccessibilityAction(action, arguments, res);
    if (*res) {
        return NOERROR;
    }
    IsEnabled(res);
    if (!(*res)) {
        return NOERROR;
    }
    Int32 index;
    GetDisplayedChild(&index);
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD:
        {
            Int32 childCount;
            GetChildCount(&childCount);
            if (index < childCount - 1) {
                ShowNext();
                *res = TRUE;
                return NOERROR;
            }
        }
        break;
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD:
        {
            if (index > 0) {
                ShowPrevious();
                *res = TRUE;
                return NOERROR;
            }
        }
        break;
    }
    *res = FALSE;
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
