
#include "elastos/droid/launcher2/DragView.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/DragLayer.h"
#include "elastos/droid/launcher2/CDragLayerLayoutParams.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CPorterDuffColorFilter;
using Elastos::Droid::Graphics::IPorterDuffColorFilter;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DragView::MyListener, Object, IAnimatorUpdateListener);

DragView::MyListener::MyListener(
    /* [in] */ DragView* host,
    /* [in] */ Float offsetX,
    /* [in] */ Float offsetY,
    /* [in] */ Float initialScale,
    /* [in] */ Float scale)
    : mHost(host)
    , mOffsetX(offsetX)
    , mOffsetY(offsetY)
    , mInitialScale(initialScale)
    , mScale(scale)
{
}

ECode DragView::MyListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fvalue = IFloat::Probe(obj);
    Float value;
    fvalue->GetValue(&value);

    Int32 deltaX = (Int32)((value * mOffsetX) - mHost->mOffsetX);
    Int32 deltaY = (Int32)((value * mOffsetY) - mHost->mOffsetY);

    mHost->mOffsetX += deltaX;
    mHost->mOffsetY += deltaY;
    IView::Probe(mHost)->SetScaleX(mInitialScale + (value * (mScale - mInitialScale)));
    IView::Probe(mHost)->SetScaleY(mInitialScale + (value * (mScale - mInitialScale)));
    if (mHost->sDragAlpha != 1.0f) {
        IView::Probe(mHost)->SetAlpha(mHost->sDragAlpha * value + (1.0f - value));
    }

    AutoPtr<IViewParent> res;
    IView::Probe(mHost)->GetParent((IViewParent**)&res);
    if (res == NULL) {
        IAnimator::Probe(animation)->Cancel();
    }
    else {
        Float tmp;
        IView::Probe(mHost)->GetTranslationX(&tmp);
        IView::Probe(mHost)->SetTranslationX(tmp + deltaX);
        IView::Probe(mHost)->GetTranslationY(&tmp);
        IView::Probe(mHost)->SetTranslationY(tmp + deltaY);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DragView::MyListener2, Object, IAnimatorUpdateListener);

DragView::MyListener2::MyListener2(
    /* [in] */ DragView* host)
    : mHost(host)
{
}

ECode DragView::MyListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    return animation->GetAnimatedFraction(&(mHost->mCrossFadeProgress));
}

DragView::MyRunnable::MyRunnable(
    /* [in] */ DragView* host)
    : mHost(host)
{
}

ECode DragView::MyRunnable::Run()
{
    return  IAnimator::Probe(mHost->mAnim)->Start();
}

Float DragView::sDragAlpha = 1.0f;

CAR_INTERFACE_IMPL(DragView, View, IDragView);

DragView::DragView()
    : mRegistrationX(0)
    , mRegistrationY(0)
    , mHasDrawn(FALSE)
    , mCrossFadeProgress(0.0f)
    , mOffsetX(0.0f)
    , mOffsetY(0.0f)
    , mInitialScale(1.0f)
{
}

ECode DragView::constructor(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 registrationX,
    /* [in] */ Int32 registrationY,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float initialScale)
{
    View::constructor(IContext::Probe(launcher));
    AutoPtr<IDragLayer> dl;
    launcher->GetDragLayer((IDragLayer**)&dl);
    mDragLayer = dl.Get();
    mInitialScale = initialScale;

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 _offsetX;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewOffsetX, &_offsetX);
    Float offsetX = (Float)_offsetX;

    Int32 _offsetY;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewOffsetY, &_offsetY);
    Float offsetY = (Float)_offsetY;

    Int32 _scaleDps;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewScale, &_scaleDps);
    Float scaleDps = _scaleDps;
    Float scale = (width + scaleDps) * 1.0f / width;

    // Set the initial scale to avoid any jumps
    SetScaleX(initialScale);
    SetScaleY(initialScale);

    // Animate the view into the correct position
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    mAnim = LauncherAnimUtils::OfFloat(IView::Probe(this), array);
    mAnim->SetDuration(150);
    AutoPtr<IAnimatorUpdateListener> listener = new MyListener(this, offsetX, offsetY,
            initialScale, scale);
    mAnim->AddUpdateListener(listener);

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    helper->CreateBitmap(bitmap, left, top, width, height, (IBitmap**)&mBitmap);
    AutoPtr<IRect> rect;
    CRect::New(0, 0, width, height, (IRect**)&rect);
    SetDragRegion(rect);

    // The point in our scaled bitmap that the touch events are located
    mRegistrationX = registrationX;
    mRegistrationY = registrationY;

    // Force a measure, because Workspace uses getMeasuredHeight() before the layout pass
    Int32 ms = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    Measure(ms, ms);
    CPaint::New(IPaint::FILTER_BITMAP_FLAG, (IPaint**)&mPaint);
    return NOERROR;
}

ECode DragView::GetOffsetY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = mOffsetY;
    return NOERROR;
}

ECode DragView::GetDragRegionLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);

    return mDragRegion->GetLeft(left);
}

ECode DragView::GetDragRegionTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);

    return mDragRegion->GetTop(top);
}

ECode DragView::GetDragRegionWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    return mDragRegion->GetWidth(width);
}

ECode DragView::GetDragRegionHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    return mDragRegion->GetHeight(height);
}

ECode DragView::SetDragVisualizeOffset(
    /* [in] */ IPoint* p)
{
    mDragVisualizeOffset = p;
    return NOERROR;
}

ECode DragView::GetDragVisualizeOffset(
    /* [out] */ IPoint** point)
{
    VALIDATE_NOT_NULL(point);

    *point = mDragVisualizeOffset;
    REFCOUNT_ADD(*point);
    return NOERROR;
}

ECode DragView::SetDragRegion(
    /* [in] */ IRect* r)
{
    mDragRegion = r;
    return NOERROR;
}

ECode DragView::GetDragRegion(
    /* [out] */ IRect** rec)
{
    VALIDATE_NOT_NULL(rec);

    *rec = mDragRegion;
    REFCOUNT_ADD(*rec);
    return NOERROR;
}

ECode DragView::GetInitialScale(
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    *scale = mInitialScale;
    return NOERROR;
}

ECode DragView::UpdateInitialScaleToCurrentScale()
{
    return GetScaleX(&mInitialScale);
}

ECode DragView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width, height;
    mBitmap->GetWidth(&width);
    mBitmap->GetHeight(&height);
    SetMeasuredDimension(width, height);
    return NOERROR;
}

void DragView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    //@SuppressWarnings("all") // suppress dead code warning
    const Boolean debug = FALSE;
    if (debug) {
        AutoPtr<IPaint> p;
        CPaint::New((IPaint**)&p);
        p->SetStyle(PaintStyle_FILL);
        p->SetColor(0x66ffffff);
        Int32 width;
        GetWidth(&width);
        Int32 height;
        GetHeight(&height);
        canvas->DrawRect(0, 0, width, height, p);
    }

    mHasDrawn = TRUE;
    Boolean crossFade = mCrossFadeProgress > 0 && mCrossFadeBitmap != NULL;
    if (crossFade) {
        Int32 alpha = crossFade ? (Int32)(255 * (1 - mCrossFadeProgress)) : 255;
        mPaint->SetAlpha(alpha);
    }
    canvas->DrawBitmap(mBitmap, 0.0f, 0.0f, mPaint);
    if (crossFade) {
        mPaint->SetAlpha((Int32)(255 * mCrossFadeProgress));
        Int32 tmp;
        canvas->Save(&tmp);
        Int32 width;
        mBitmap->GetWidth(&width);
        Int32 width2;
        mCrossFadeBitmap->GetWidth(&width2);
        Float sX = (width * 1.0f) / width2;
        Int32 height;
        mBitmap->GetHeight(&height);
        Int32 height2;
        mCrossFadeBitmap->GetHeight(&height2);
        Float sY = (height * 1.0f) / height2;
        canvas->Scale(sX, sY);
        canvas->DrawBitmap(mCrossFadeBitmap, 0.0f, 0.0f, mPaint);
        canvas->Restore();
    }
}

ECode DragView::SetCrossFadeBitmap(
    /* [in] */ IBitmap* crossFadeBitmap)
{
    mCrossFadeBitmap = crossFadeBitmap;
    return NOERROR;
}

ECode DragView::CrossFade(
    /* [in] */ Int32 duration)
{
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(IView::Probe(this), array);
    va->SetDuration(duration);
    AutoPtr<IDecelerateInterpolator> polator;
    CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&polator);
    IAnimator::Probe(va)->SetInterpolator(ITimeInterpolator::Probe(polator));
    AutoPtr<IAnimatorUpdateListener> listener = new MyListener2(this);
    va->AddUpdateListener(listener);
    return IAnimator::Probe(va)->Start();
}

ECode DragView::SetColor(
    /* [in] */ Int32 color)
{
    if (mPaint == NULL) {
        CPaint::New(IPaint::FILTER_BITMAP_FLAG, (IPaint**)&mPaint);
    }
    if (color != 0) {
        AutoPtr<IPorterDuffColorFilter> filter;
        CPorterDuffColorFilter::New(color, PorterDuffMode_SRC_ATOP, (IPorterDuffColorFilter**)&filter);
        mPaint->SetColorFilter(IColorFilter::Probe(filter));
    }
    else {
        mPaint->SetColorFilter(NULL);
    }
    return Invalidate();
}

ECode DragView::HasDrawn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHasDrawn;
    return NOERROR;
}

ECode DragView::SetAlpha(
    /* [in] */ Float alpha)
{
    View::SetAlpha(alpha);
    mPaint->SetAlpha((Int32)(255 * alpha));
    return Invalidate();
}

ECode DragView::Show(
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY)
{
    IViewGroup::Probe(mDragLayer)->AddView(this);

    // Start the pick-up animation
    AutoPtr<CDragLayerLayoutParams> lp;
    CDragLayerLayoutParams::NewByFriend(0, 0, (CDragLayerLayoutParams**)&lp);
    mBitmap->GetWidth(&(lp->mWidth));
    mBitmap->GetHeight(&(lp->mHeight));
    lp->mCustomPosition = TRUE;
    SetLayoutParams(lp);
    SetTranslationX(touchX - mRegistrationX);
    SetTranslationY(touchY - mRegistrationY);
    // Post the animation to skip other expensive work happening on the first frame
    AutoPtr<MyRunnable> run = new MyRunnable(this);
    Boolean res;
    return Post(run, &res);
}

ECode DragView::CancelAnimation()
{
    if (mAnim != NULL) {
        Boolean res;
        IAnimator::Probe(mAnim)->IsRunning(&res);
        if (res) {
            return IAnimator::Probe(mAnim)->Cancel();
        }
    }
    return NOERROR;
}

ECode DragView::ResetLayoutParams()
{
    mOffsetX = mOffsetY = 0;
    return RequestLayout();
}

ECode DragView::Move(
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY)
{
    SetTranslationX(touchX - mRegistrationX + (Int32)mOffsetX);
    return SetTranslationY(touchY - mRegistrationY + (Int32)mOffsetY);
}

ECode DragView::Remove()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (res != NULL) {
        return IViewGroup::Probe(mDragLayer)->RemoveView(this);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos