#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"
#include "elastos/droid/systemui/screenshot/SaveImageInBackgroundData.h"
#include "elastos/droid/systemui/screenshot/SaveImageInBackgroundTask.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::App::CNotificationBigTextStyle;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBigTextStyle;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::INotificationStyle;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Media::CMediaActionSound;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IFloat;
using Elastos::Core::ISystem;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyViewOnTouchListener
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(GlobalScreenshot::MyViewOnTouchListener, Object, IViewOnTouchListener)

GlobalScreenshot::MyViewOnTouchListener::MyViewOnTouchListener()
{}

ECode GlobalScreenshot::MyViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorListenerAdapter
//--------------------------------------------------------------------------------------------------

GlobalScreenshot::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ IRunnable* finisher,
    /* [in] */ GlobalScreenshot* host)
    : mFinisher(finisher)
    , mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    // Save the screenshot once we have a bit of time now
    mHost->SaveScreenshotInWorkerThread(mFinisher);
    IViewManager::Probe(mHost->mWindowManager)->RemoveView(mHost->mScreenshotLayout);

    // Clear any references to the bitmap
    mHost->mScreenBitmap = NULL;
    mHost->mScreenshotView->SetImageBitmap(NULL);
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyRunnable
//--------------------------------------------------------------------------------------------------

GlobalScreenshot::MyRunnable::MyRunnable(
    /* [in] */ GlobalScreenshot* host)
    : mHost(host)
{}

ECode GlobalScreenshot::MyRunnable::Run()
{
    // Play the shutter sound to notify that we've taken a screenshot
    mHost->mCameraSound->Play(IMediaActionSound::SHUTTER_CLICK);
    IView::Probe(mHost->mScreenshotView)->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    IView::Probe(mHost->mScreenshotView)->BuildLayer();
    IAnimator::Probe(mHost->mScreenshotAnimation)->Start();
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyInterpolator1
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(GlobalScreenshot::MyInterpolator1, Object, ITimeInterpolator, IInterpolator)

GlobalScreenshot::MyInterpolator1::MyInterpolator1(
    /* [in] */ Float flashDurationPct)
    : mFlashDurationPct(flashDurationPct)
{}

ECode GlobalScreenshot::MyInterpolator1::GetInterpolation(
    /* [in] */ Float x,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation)
    // Flash the flash view in and out quickly
    if (x <= mFlashDurationPct) {
        *interpolation = (Float) Elastos::Core::Math::Sin(Elastos::Core::Math::PI * (x / mFlashDurationPct));
    }
    *interpolation = 0.0f;
    return NOERROR;
}

ECode GlobalScreenshot::MyInterpolator1::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyInterpolator2
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(GlobalScreenshot::MyInterpolator2, Object, ITimeInterpolator, IInterpolator)

GlobalScreenshot::MyInterpolator2::MyInterpolator2(
    /* [in] */ Float flashPeakDurationPct,
    /* [in] */ Float flashDurationPct)
    : mFlashPeakDurationPct(flashPeakDurationPct)
    , mFlashDurationPct(flashDurationPct)
{}

ECode GlobalScreenshot::MyInterpolator2::GetInterpolation(
    /* [in] */ Float x,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation)
    // We start scaling when the flash is at it's peak
    if (x < mFlashPeakDurationPct) {
        *interpolation = 0.0f;
        return NOERROR;
    }
    *interpolation = (x - mFlashDurationPct) / (1.0f - mFlashDurationPct);
    return NOERROR;
}

ECode GlobalScreenshot::MyInterpolator2::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorListenerAdapter2
//--------------------------------------------------------------------------------------------------

GlobalScreenshot::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ GlobalScreenshot* host)
    : mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->mBackgroundView->SetAlpha((Int32)(0.0f));
    IView::Probe(mHost->mBackgroundView)->SetVisibility(IView::VISIBLE);
    mHost->mScreenshotView->SetAlpha((Int32)(0.0f));
    IView::Probe(mHost->mScreenshotView)->SetTranslationX(0.0f);
    IView::Probe(mHost->mScreenshotView)->SetTranslationY(0.0f);
    IView::Probe(mHost->mScreenshotView)->SetScaleX(SCREENSHOT_SCALE + mHost->mBgPaddingScale);
    IView::Probe(mHost->mScreenshotView)->SetScaleY(SCREENSHOT_SCALE + mHost->mBgPaddingScale);
    IView::Probe(mHost->mScreenshotView)->SetVisibility(IView::VISIBLE);
    mHost->mScreenshotFlash->SetAlpha(0.0f);
    IView::Probe(mHost->mScreenshotFlash)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

ECode GlobalScreenshot::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return IView::Probe(mHost->mScreenshotFlash)->SetVisibility(IView::GONE);
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorUpdateListener
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(GlobalScreenshot::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

GlobalScreenshot::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ IInterpolator* scaleInterpolator,
    /* [in] */ IInterpolator* flashAlphaInterpolator,
    /* [in] */ GlobalScreenshot* host)
    : mScaleInterpolator(scaleInterpolator)
    , mFlashAlphaInterpolator(flashAlphaInterpolator)
    , mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fObj = IFloat::Probe(obj);
    if (fObj == NULL)
        return E_NULL_POINTER_EXCEPTION;
    Float t;
    fObj->GetValue(&t);
    Float polator;
    ITimeInterpolator::Probe(mScaleInterpolator)->GetInterpolation(t, &polator);
    Float scaleT = (GlobalScreenshot::SCREENSHOT_SCALE + mHost->mBgPaddingScale)
        - polator * (GlobalScreenshot::SCREENSHOT_SCALE
        - GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE);
    mHost->mBackgroundView->SetAlpha(polator * BACKGROUND_ALPHA);
    mHost->mScreenshotView->SetAlpha(t);
    IView::Probe(mHost->mScreenshotView)->SetScaleX(scaleT);
    IView::Probe(mHost->mScreenshotView)->SetScaleY(scaleT);
    Float polator2;
    ITimeInterpolator::Probe(mFlashAlphaInterpolator)->GetInterpolation(t, &polator2);
    mHost->mScreenshotFlash->SetAlpha(polator2);
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorListenerAdapter3
//--------------------------------------------------------------------------------------------------

GlobalScreenshot::MyAnimatorListenerAdapter3::MyAnimatorListenerAdapter3(
    /* [in] */ GlobalScreenshot* host)
    : mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IView::Probe(mHost->mBackgroundView)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mScreenshotView)->SetVisibility(IView::GONE);
    IView::Probe(mHost->mScreenshotView)->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorUpdateListener2
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(GlobalScreenshot::MyAnimatorUpdateListener2, Object, IAnimatorUpdateListener)

GlobalScreenshot::MyAnimatorUpdateListener2::MyAnimatorUpdateListener2(
    /* [in] */ GlobalScreenshot* host)
    : mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fObj = IFloat::Probe(obj);
    if (fObj == NULL)
        return E_NULL_POINTER_EXCEPTION;
    Float t;
    fObj->GetValue(&t);
    Float scaleT = (GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE + mHost->mBgPaddingScale)
        - t * (GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE
        - GlobalScreenshot::SCREENSHOT_FAST_DROP_OUT_MIN_SCALE);
    mHost->mBackgroundView->SetAlpha((1.0f - t) * GlobalScreenshot::BACKGROUND_ALPHA);
    mHost->mScreenshotView->SetAlpha(1.0f - t);
    IView::Probe(mHost->mScreenshotView)->SetScaleX(scaleT);
    IView::Probe(mHost->mScreenshotView)->SetScaleY(scaleT);
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyInterpolator3
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(GlobalScreenshot::MyInterpolator3, Object, ITimeInterpolator, IInterpolator)

GlobalScreenshot::MyInterpolator3::MyInterpolator3(
    /* [in] */ Float scaleDurationPct)
    : mScaleDurationPct(scaleDurationPct)
{}

ECode GlobalScreenshot::MyInterpolator3::GetInterpolation(
    /* [in] */ Float x,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation)

    if (x < mScaleDurationPct) {
        // Decelerate, and scale the input accordingly
        *interpolation = (Float) (1.0f - Elastos::Core::Math::Pow(1.0f - (x / mScaleDurationPct), 2.0f));
        return NOERROR;
    }
    *interpolation = 1.0f;
    return NOERROR;
}

ECode GlobalScreenshot::MyInterpolator3::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot::MyAnimatorUpdateListener3
//--------------------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(GlobalScreenshot::MyAnimatorUpdateListener3, Object, IAnimatorUpdateListener)

GlobalScreenshot::MyAnimatorUpdateListener3::MyAnimatorUpdateListener3(
    /* [in] */ IInterpolator* scaleInterpolator,
    /* [in] */ IPointF* finalPos,
    /* [in] */ GlobalScreenshot* host)
    : mScaleInterpolator(scaleInterpolator)
    , mFinalPos(finalPos)
    , mHost(host)
{}

ECode GlobalScreenshot::MyAnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> fObj = IFloat::Probe(obj);
    if (fObj == NULL)
        return E_NULL_POINTER_EXCEPTION;
    Float t;
    fObj->GetValue(&t);
    Float polator;
    ITimeInterpolator::Probe(mScaleInterpolator)->GetInterpolation(t, &polator);

    Float scaleT = (GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE + mHost->mBgPaddingScale)
        - polator * (GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE - GlobalScreenshot::SCREENSHOT_DROP_OUT_MIN_SCALE);
    mHost->mBackgroundView->SetAlpha((1.0f - t) * BACKGROUND_ALPHA);
    mHost->mScreenshotView->SetAlpha(1.0f - polator);
    IView::Probe(mHost->mScreenshotView)->SetScaleX(scaleT);
    IView::Probe(mHost->mScreenshotView)->SetScaleY(scaleT);
    Float x,y;
    mFinalPos->GetX(&x);
    mFinalPos->GetY(&y);
    IView::Probe(mHost->mScreenshotView)->SetTranslationX(t * x);
    IView::Probe(mHost->mScreenshotView)->SetTranslationY(t * y);
    return NOERROR;
}

//--------------------------------------------------------------------------------------------------
//         GlobalScreenshot
//--------------------------------------------------------------------------------------------------

const String GlobalScreenshot::TAG("GlobalScreenshot");

const Int32 GlobalScreenshot::SCREENSHOT_NOTIFICATION_ID;
const Int32 GlobalScreenshot::SCREENSHOT_FLASH_TO_PEAK_DURATION;
const Int32 GlobalScreenshot::SCREENSHOT_DROP_IN_DURATION;
const Int32 GlobalScreenshot::SCREENSHOT_DROP_OUT_DELAY;
const Int32 GlobalScreenshot::SCREENSHOT_DROP_OUT_DURATION;
const Int32 GlobalScreenshot::SCREENSHOT_DROP_OUT_SCALE_DURATION;
const Int32 GlobalScreenshot::SCREENSHOT_FAST_DROP_OUT_DURATION;
const Float GlobalScreenshot::BACKGROUND_ALPHA = 0.5f;
const Float GlobalScreenshot::SCREENSHOT_SCALE = 1.0f;
const Float GlobalScreenshot::SCREENSHOT_DROP_IN_MIN_SCALE = SCREENSHOT_SCALE * 0.725f;
const Float GlobalScreenshot::SCREENSHOT_DROP_OUT_MIN_SCALE = SCREENSHOT_SCALE * 0.45f;
const Float GlobalScreenshot::SCREENSHOT_FAST_DROP_OUT_MIN_SCALE = SCREENSHOT_SCALE * 0.6f;
const Float GlobalScreenshot::SCREENSHOT_DROP_OUT_MIN_SCALE_OFFSET = 0.0f;

GlobalScreenshot::GlobalScreenshot(
    /* [in] */ IContext* context)
    : mContext(context)
{
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IInterface> lifObj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&lifObj);
    AutoPtr<ILayoutInflater> layoutInflater = ILayoutInflater::Probe(lifObj);

    // Inflate the screenshot layout
    CMatrix::New((IMatrix**)&mDisplayMatrix);
    layoutInflater->Inflate(R::layout::global_screenshot, NULL, (IView**)&mScreenshotLayout);
    AutoPtr<IView> v;
    mScreenshotLayout->FindViewById(R::id::global_screenshot_background, (IView**)&v);
    mBackgroundView = IImageView::Probe(v);
    v = NULL;
    mScreenshotLayout->FindViewById(R::id::global_screenshot, (IView**)&v);
    mScreenshotView = IImageView::Probe(v);
    v = NULL;
    mScreenshotLayout->FindViewById(R::id::global_screenshot_flash, (IView**)&v);
    mScreenshotFlash = IImageView::Probe(v);
    mScreenshotLayout->SetFocusable(TRUE);

    AutoPtr<MyViewOnTouchListener> votl = new MyViewOnTouchListener();
    mScreenshotLayout->SetOnTouchListener(votl);

    // Setup the window that we are going to use
    CWindowManagerLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::MATCH_PARENT, 0, 0,
        IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY,
        IWindowManagerLayoutParams::FLAG_FULLSCREEN
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED
            | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED,
        IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&mWindowLayoutParams);

    AutoPtr<ICharSequence> cs;
    CString::New(String("ScreenshotAnimation"), (ICharSequence**)&cs);
    mWindowLayoutParams->SetTitle(cs);

    AutoPtr<IInterface> wmObj;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wmObj);
    mWindowManager = IWindowManager::Probe(wmObj);

    AutoPtr<IInterface> nmObj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&nmObj);
    mNotificationManager = INotificationManager::Probe(nmObj);

    mWindowManager->GetDefaultDisplay((IDisplay**)&mDisplay);

    CDisplayMetrics::New((IDisplayMetrics**)&mDisplayMetrics);
    mDisplay->GetRealMetrics(mDisplayMetrics);

    // Get the various target sizes
    r->GetDimensionPixelSize(Elastos::Droid::R::dimen::notification_large_icon_height, &mNotificationIconSize);

    // Scale has to account for both sides of the bg
    Int32 size;
    r->GetDimensionPixelSize(R::dimen::global_screenshot_bg_padding, &size);
    mBgPadding = (Float)size;
    Int32 wp;
    mDisplayMetrics->GetWidthPixels(&wp);
    mBgPaddingScale = mBgPadding /  wp;

    // determine the optimal preview size
    Int32 panelWidth = 0;
    // try {
    r->GetDimensionPixelSize(R::dimen::notification_panel_width, &panelWidth);
    // } catch (Resources.NotFoundException e) {
    // }
    if (panelWidth <= 0) {
        // includes notification_panel_width==match_parent (-1)
        panelWidth = wp;
    }
    mPreviewWidth = panelWidth;
    r->GetDimensionPixelSize(R::dimen::notification_max_height, &mPreviewHeight);

    // Setup the Camera shutter sound
    CMediaActionSound::New((IMediaActionSound**)&mCameraSound);
    mCameraSound->Load(IMediaActionSound::SHUTTER_CLICK);
}

void GlobalScreenshot::SaveScreenshotInWorkerThread(
    /* [in] */ IRunnable* finisher)
{
    AutoPtr<SaveImageInBackgroundData> data = new SaveImageInBackgroundData();
    data->mContext = mContext;
    data->mImage = mScreenBitmap;
    data->mIconSize = mNotificationIconSize;
    data->mFinisher = finisher;
    data->mPreviewWidth = mPreviewWidth;
    data->mPreviewheight = mPreviewHeight;
    if (mSaveInBgTask != NULL) {
        mSaveInBgTask->Cancel(FALSE);
    }
    mSaveInBgTask = new SaveImageInBackgroundTask(mContext, data, mNotificationManager,
            SCREENSHOT_NOTIFICATION_ID);
    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
    params->Set(0, (IObject*)data.Get());
    mSaveInBgTask->Execute(params.Get());
}

Float GlobalScreenshot::GetDegreesForRotation(
    /* [in] */ Int32 value)
{
    switch (value) {
    case ISurface::ROTATION_90:
        return 360.0f - 90.0f;
    case ISurface::ROTATION_180:
        return 360.0f - 180.0f;
    case ISurface::ROTATION_270:
        return 360.0f - 270.0f;
    }
    return 0.0f;
}

void GlobalScreenshot::TakeScreenshot(
    /* [in] */ IRunnable* finisher,
    /* [in] */ Boolean statusBarVisible,
    /* [in] */ Boolean navBarVisible)
{
    // We need to orient the screenshot correctly (and the Surface api seems to take screenshots
    // only in the natural orientation of the device :!)
    mDisplay->GetRealMetrics(mDisplayMetrics);
    AutoPtr<ArrayOf<Float> > dims = ArrayOf<Float>::Alloc(2);
    Int32 wp, hp;
    mDisplayMetrics->GetWidthPixels(&wp);
    mDisplayMetrics->GetHeightPixels(&hp);
    (*dims)[0] = wp;
    (*dims)[1] = hp;
    Int32 rotation;
    mDisplay->GetRotation(&rotation);
    Float degrees = GetDegreesForRotation(rotation);
    Boolean requiresRotation = (degrees > 0);
    if (requiresRotation) {
        // Get the dimensions of the device in its native orientation
        mDisplayMatrix->Reset();
        Boolean isRotate;
        mDisplayMatrix->PreRotate(-degrees, &isRotate);
        mDisplayMatrix->MapPoints(dims);
        (*dims)[0] = Elastos::Core::Math::Abs((*dims)[0]);
        (*dims)[1] = Elastos::Core::Math::Abs((*dims)[1]);
    }

    // Take the screenshot
    AutoPtr<ISurfaceControlHelper> sch;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&sch);
    sch->Screenshot((Int32)((*dims)[0]), (Int32)((*dims)[1]), (IBitmap**)&mScreenBitmap);
    if (mScreenBitmap == NULL) {
        NotifyScreenshotError(mContext, mNotificationManager);
        finisher->Run();
        return;
    }

    if (requiresRotation) {
        // Rotate the screenshot to the current orientation
        AutoPtr<IBitmapHelper> bh;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
        AutoPtr<IBitmap> ss;
        bh->CreateBitmap(wp, hp, BitmapConfig_ARGB_8888, (IBitmap**)&ss);
        AutoPtr<ICanvas> c;
        CCanvas::New(ss, (ICanvas**)&c);
        Int32 w, h;
        ss->GetWidth(&w);
        ss->GetHeight(&h);
        c->Translate(w / 2, h / 2);

        c->Rotate(degrees);
        c->Translate(-(*dims)[0] / 2, -(*dims)[1] / 2);
        c->DrawBitmap(mScreenBitmap, 0.0f, 0.0f, NULL);
        c->SetBitmap(NULL);
        // Recycle the previous bitmap
        mScreenBitmap->Recycle();
        mScreenBitmap = ss;
    }

    // Optimizations
    mScreenBitmap->SetHasAlpha(FALSE);
    mScreenBitmap->PrepareToDraw();

    // Start the post-screenshot animation
    StartAnimation(finisher, wp, hp, statusBarVisible, navBarVisible);
}

void GlobalScreenshot::StartAnimation(
    /* [in] */ IRunnable* finisher,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Boolean statusBarVisible,
    /* [in] */ Boolean navBarVisible)
{
    // Add the view for the animation
    mScreenshotView->SetImageBitmap(mScreenBitmap);
    Boolean b;
    mScreenshotLayout->RequestFocus(&b);

    // Setup the animation with the screenshot just taken
    if (mScreenshotAnimation != NULL) {
        IAnimator::Probe(mScreenshotAnimation)->End();
        IAnimator::Probe(mScreenshotAnimation)->RemoveAllListeners();
    }

    IViewManager::Probe(mWindowManager)->AddView(mScreenshotLayout,
        IViewGroupLayoutParams::Probe(mWindowLayoutParams));
    AutoPtr<IValueAnimator> screenshotDropInAnim = CreateScreenshotDropInAnimation();
    AutoPtr<IValueAnimator> screenshotFadeOutAnim = CreateScreenshotDropOutAnimation(w, h,
        statusBarVisible, navBarVisible);
    CAnimatorSet::New((IAnimatorSet**)&mScreenshotAnimation);

    AutoPtr<ArrayOf<IAnimator*> > items = ArrayOf<IAnimator*>::Alloc(2);
    items->Set(0, IAnimator::Probe(screenshotDropInAnim));
    items->Set(1, IAnimator::Probe(screenshotFadeOutAnim));
    mScreenshotAnimation->PlaySequentially(items);

    AutoPtr<MyAnimatorListenerAdapter> ala = new MyAnimatorListenerAdapter(finisher, this);

    IAnimator::Probe(mScreenshotAnimation)->AddListener(ala);
    AutoPtr<MyRunnable> rble = new MyRunnable(this);
    Boolean isPost;
    mScreenshotLayout->Post(rble, &isPost);
}

AutoPtr<IValueAnimator> GlobalScreenshot::CreateScreenshotDropInAnimation()
{
    const Float flashPeakDurationPct = ((Float) (SCREENSHOT_FLASH_TO_PEAK_DURATION)
        / SCREENSHOT_DROP_IN_DURATION);
    const Float flashDurationPct = 2.0f * flashPeakDurationPct;
    AutoPtr<MyInterpolator1> mi1 = new MyInterpolator1(flashDurationPct);
    const AutoPtr<IInterpolator> flashAlphaInterpolator = mi1;

    AutoPtr<MyInterpolator2> mi2 = new MyInterpolator2(flashPeakDurationPct, flashDurationPct);
    const AutoPtr<IInterpolator> scaleInterpolator = mi2;

    AutoPtr<IValueAnimatorHelper> vah;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = 0.0f;
    (*values)[1] = 1.0f;
    AutoPtr<IValueAnimator> anim;
    vah->OfFloat(values, (IValueAnimator**)&anim);

    IAnimator::Probe(anim)->SetDuration(SCREENSHOT_DROP_IN_DURATION);
    AutoPtr<MyAnimatorListenerAdapter2> mala = new MyAnimatorListenerAdapter2(this);
    IAnimator::Probe(anim)->AddListener(mala);

    AutoPtr<MyAnimatorUpdateListener> maul = new MyAnimatorUpdateListener(
        scaleInterpolator, flashAlphaInterpolator, this);
    anim->AddUpdateListener(maul);
    return anim;
}

AutoPtr<IValueAnimator> GlobalScreenshot::CreateScreenshotDropOutAnimation(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Boolean statusBarVisible,
    /* [in] */ Boolean navBarVisible)
{
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = 0.0f;
    (*values)[1] = 1.0f;
    AutoPtr<IValueAnimatorHelper> vah;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
    AutoPtr<IValueAnimator> anim;
    vah->OfFloat(values, (IValueAnimator**)&anim);
    IAnimator::Probe(anim)->SetStartDelay(SCREENSHOT_DROP_OUT_DELAY);

    AutoPtr<MyAnimatorListenerAdapter3> ala = new MyAnimatorListenerAdapter3(this);

    IAnimator::Probe(anim)->AddListener(ala);

    if (!statusBarVisible || !navBarVisible) {
        // There is no status bar/nav bar, so just fade the screenshot away in place
        IAnimator::Probe(anim)->SetDuration(SCREENSHOT_FAST_DROP_OUT_DURATION);
        AutoPtr<MyAnimatorUpdateListener2> maul = new MyAnimatorUpdateListener2(this);
        anim->AddUpdateListener(maul);
    }
    else {
        // In the case where there is a status bar, animate to the origin of the bar (top-left)
        const Float scaleDurationPct = (Float) SCREENSHOT_DROP_OUT_SCALE_DURATION
                / SCREENSHOT_DROP_OUT_DURATION;
        AutoPtr<MyInterpolator3> mi = new MyInterpolator3(scaleDurationPct);
        const AutoPtr<IInterpolator> scaleInterpolator = mi;

        // Determine the bounds of how to scale
        Float halfScreenWidth = (w - 2.0f * mBgPadding) / 2.0f;
        Float halfScreenHeight = (h - 2.0f * mBgPadding) / 2.0f;
        const Float offsetPct = SCREENSHOT_DROP_OUT_MIN_SCALE_OFFSET;
        AutoPtr<IPointF> pf;
        CPointF::New(
            -halfScreenWidth + (SCREENSHOT_DROP_OUT_MIN_SCALE + offsetPct) * halfScreenWidth,
            -halfScreenHeight + (SCREENSHOT_DROP_OUT_MIN_SCALE + offsetPct) * halfScreenHeight,
            (IPointF**)&pf);
        const AutoPtr<IPointF> finalPos = pf;

        // Animate the screenshot to the status bar
        IAnimator::Probe(anim)->SetDuration(SCREENSHOT_DROP_OUT_DURATION);
        AutoPtr<MyAnimatorUpdateListener3> aul = new MyAnimatorUpdateListener3(
            scaleInterpolator, finalPos, this);
        anim->AddUpdateListener(aul);
    }
    return anim;
}

void GlobalScreenshot::NotifyScreenshotError(
    /* [in] */ IContext* context,
    /* [in] */ INotificationManager* nManager)
{
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);

    // Clear all existing notification, compose the new notification and show it
    AutoPtr<INotificationBuilder> b;
    CNotificationBuilder::New(context, (INotificationBuilder**)&b);

    String s1;
    r->GetString(R::string::screenshot_failed_title, &s1);
    AutoPtr<ICharSequence> cs;
    CString::New(s1, (ICharSequence**)&cs);
    b->SetTicker(cs);
    b->SetContentTitle(cs);

    String s2;
    r->GetString(R::string::screenshot_failed_text, &s2);
    cs = NULL;
    CString::New(s2, (ICharSequence**)&cs);
    b->SetContentText(cs);
    b->SetSmallIcon(R::drawable::stat_notify_image_error);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 millis;
    sys->GetCurrentTimeMillis(&millis);
    b->SetWhen(millis);
    b->SetVisibility(INotification::VISIBILITY_PUBLIC); // ok to show outside lockscreen
    b->SetCategory(INotification::CATEGORY_ERROR);
    b->SetAutoCancel(TRUE);
    Int32 color;
    r->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &color);
    b->SetColor(color);

    AutoPtr<INotificationBigTextStyle> nbts;
    CNotificationBigTextStyle::New(b, (INotificationBigTextStyle**)&nbts);
    nbts->BigText(cs);
    AutoPtr<INotificationStyle> ns = INotificationStyle::Probe(nbts);
    AutoPtr<INotification> n;
    ns->Build((INotification**)&n);

    nManager->Notify(SCREENSHOT_NOTIFICATION_ID, n);
}

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos