
#include "elastos/droid/launcher2/DragView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DragView::MyListener, Object, IAnimatorUpdateListener);

DragView::MyListener::MyListener(
    /* [in] */ DragView* host)
    : mHost(host)
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

    Int32 deltaX = (Int32)((value * offsetX) - mOffsetX);
    Int32 deltaY = (Int32)((value * offsetY) - mOffsetY);

    mOffsetX += deltaX;
    mOffsetY += deltaY;
    mHost->SetScaleX(initialScale + (value * (scale - initialScale)));
    mHost->SetScaleY(initialScale + (value * (scale - initialScale)));
    if (mHost->sDragAlpha != 1f) {
        mHost->SetAlpha(mHost->sDragAlpha * value + (1f - value));
    }

    AutoPtr<IViewParent> res;
    mHost->GetParent((IViewParent**)&res);
    if (res == NULL) {
        animation->Cancel();
    }
    else {
        Float tmp;
        mHost->GetTranslationX(&tmp);
        mHost->SetTranslationX(tmp + deltaX);
        mHost->GetTranslationY(&tmp);
        mHost->SetTranslationY(tmp + deltaY);
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
    reutn mHost->mAnim->Start();
}

Float DragView::sDragAlpha = 1f;

DragView::DragView(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 registrationX,
    /* [in] */ Int32 registrationY,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float initialScale)
    : mRegistrationX(0)
    , mRegistrationY(0)
    , mHasDrawn(FALSE)
    , mCrossFadeProgress(0f)
    , mOffsetX(0.0f)
    , mOffsetY(0.0f)
    , mInitialScale(1f)
{
    View(launcher);
    launcher->GetDragLayer((IDragLayer**)&mDragLayer);
    mInitialScale = initialScale;

    AutoPtr<IResources> res;
    getResources((IResources**)&res);
    Float offsetX;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewOffsetX, &offsetX);
    Float offsetY;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewOffsetY, &offsetY);
    Float scaleDps;
    res->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::dragViewScale, &scaleDps);
    Float scale = (width + scaleDps) / width;

    // Set the initial scale to avoid any jumps
    SetScaleX(initialScale);
    SetScaleY(initialScale);

    // Animate the view into the correct position
    mAnim = LauncherAnimUtils::OfFloat(this, 0f, 1f);
    mAnim->SetDuration(150);
    AutoPtr<IAnimatorUpdateListener> listener = new MyListener(this);
    mAnim->AddUpdateListener(listener);

    Bitmap::CreateBitmap(bitmap, left, top, width, height, (IBitmap**)&mBitmap);
    AutoPtr<IRect> rect;
    CRect::New(0, 0, width, height, (IRect**)&rect);
    SetDragRegion(rect);

    // The point in our scaled bitmap that the touch events are located
    mRegistrationX = registrationX;
    mRegistrationY = registrationY;

    // Force a measure, because Workspace uses getMeasuredHeight() before the layout pass
    Int32 ms = View::MeasureSpec::MakeMeasureSpec(0, IView::MeasureSpec::UNSPECIFIED);
    Measure(ms, ms);
    CPaint::New(IPaint::FILTER_BITMAP_FLAG, (IPaint**)&mPaint);
}

Float DragView::GetOffsetY()
{
    return mOffsetY;
}

Int32 DragView::GetDragRegionLeft()
{
    Int32 left;
    mDragRegion->GetLeft(&left);
    return left;
}

Int32 DragView::GetDragRegionTop()
{
    Int32 top;
    mDragRegion->GetTop(&top);
    return top;
}

Int32 DragView::GetDragRegionWidth()
{
    Int32 width;
    mDragRegion->GetWidth(&width);
    return width;
}

Int32 DragView::GetDragRegionHeight()
{
    Int32 height;
    mDragRegion->GetHeight(&height);
    return height;
}

ECode DragView::SetDragVisualizeOffset(
    /* [in] */ IPoint* p)
{
    mDragVisualizeOffset = p;
    return NOERROR;
}

AutoPtr<IPoint> DragView::GetDragVisualizeOffset()
{
    return mDragVisualizeOffset;
}

ECode DragView::SetDragRegion(
    /* [in] */ IRect* r)
{
    mDragRegion = r;
    return NOERROR;
}

AutoPtr<IRect> DragView::GetDragRegion()
{
    return mDragRegion;
}

Float DragView::GetInitialScale()
{
    return mInitialScale;
}

ECode DragView::UpdateInitialScaleToCurrentScale()
{
    return GetScaleX(&mInitialScale);
}

void DragView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width;
    mBitmap->GetWidth(&width);
    Int32 height;
    mBitmap->GetHeight(&height);
    SetMeasuredDimension(width, height);
    return;
}

void DragView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    //@SuppressWarnings("all") // suppress dead code warning
    const Boolean debug = FALSE;
    if (debug) {
        AutoPtr<IPaint> p;
        CPaint::New((IPaint**)&p);
        p->SetStyle(IPaint::Style::FILL);
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
        canvas->Save();
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
    AutoPtr<IValueAnimator> va = LauncherAnimUtils::OfFloat(this, 0f, 1f);
    va->SetDuration(duration);
    AutoPtr<IDecelerateInterpolator> polator;
    CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&polator);
    va->SetInterpolator(polator);
    AutoPtr<IAnimatorUpdateListener> listener = new MyListener2(this);
    va->AddUpdateListener(listener);
    return va->Start();
}

ECode DragView::SetColor(
    /* [in] */ Int32 color)
{
    if (mPaint == NULL) {
        CPaint::New(IPaint::FILTER_BITMAP_FLAG, (IPaint**)&mPaint);
    }
    if (color != 0) {
        AutoPtr<IPorterDuffColorFilter> filter;
        CPorterDuffColorFilter::New(color, PorterDuff::Mode.SRC_ATOP, (IPorterDuffColorFilter**)&filter);
        mPaint->SetColorFilter(filter);
    }
    else {
        mPaint->SetColorFilter(NULL);
    }
    return Invalidate();
}

Boolean DragView::HasDrawn()
{
    return mHasDrawn;
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
    mDragLayer->AddView(this);

    // Start the pick-up animation
    AutoPtr<DragLayer::LayoutParams> lp = new DragLayer::LayoutParams(0, 0);
    mBitmap->GetWidth(&(lp->mWidth));
    mBitmap->GetHeight(&(lp->mHeight));
    lp->mCustomPosition = TRUE;
    SetLayoutParams(lp);
    SetTranslationX(touchX - mRegistrationX);
    SetTranslationY(touchY - mRegistrationY);
    // Post the animation to skip other expensive work happening on the first frame
    AutoPtr<MyRunnable> run = new MyRunnable(this);
    return Post(run);
}

ECode DragView::CancelAnimation()
{
    if (mAnim != NULL) {
        Boolean res;
        mAnim->IsRunning(&res);
        if (res) {
            return mAnim->Cancel();
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
        return mDragLayer->RemoveView(this);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos