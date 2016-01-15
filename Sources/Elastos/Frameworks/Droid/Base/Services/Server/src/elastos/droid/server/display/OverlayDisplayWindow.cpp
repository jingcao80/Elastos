#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/droid/server/display/OverlayDisplayWindow.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_ISurfaceTextureListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::CScaleGestureDetector;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String OverlayDisplayWindow::TAG("OverlayDisplayWindow");
const Boolean OverlayDisplayWindow::DEBUG = TRUE;

//===================================================================================
//          OverlayDisplayWindow::MyDisplayListener
//===================================================================================
CAR_INTERFACE_IMPL(OverlayDisplayWindow::MyDisplayListener, Object, IDisplayListener);

OverlayDisplayWindow::MyDisplayListener::MyDisplayListener(
    /* [in] */ OverlayDisplayWindow* owner)
    : mHost(owner)
{}

//@Override
ECode OverlayDisplayWindow::MyDisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

//@Override
ECode OverlayDisplayWindow::MyDisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    Int32 defaultDisplayId;
    mHost->mDefaultDisplay->GetDisplayId(&defaultDisplayId);
    if (displayId == defaultDisplayId) {
        if (mHost->UpdateDefaultDisplayInfo()) {
            mHost->Relayout();
            Int32 state;
            mHost->mDefaultDisplayInfo->GetState(&state);
            mHost->mListener->OnStateChanged(state);
        }
        else {
            mHost->Dismiss();
        }
    }
    return NOERROR;
}

//@Override
ECode OverlayDisplayWindow::MyDisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    Int32 defaultDisplayId;
    mHost->mDefaultDisplay->GetDisplayId(&defaultDisplayId);
    if (displayId == defaultDisplayId) {
        mHost->Dismiss();
    }
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayWindow::MySurfaceTextureListener
//===================================================================================
CAR_INTERFACE_IMPL(
    OverlayDisplayWindow::MySurfaceTextureListener, Object, ISurfaceTextureListener);

OverlayDisplayWindow::MySurfaceTextureListener::MySurfaceTextureListener(
    /* [in] */ OverlayDisplayWindow* owner)
    : mHost(owner)
{}

//@Override
ECode OverlayDisplayWindow::MySurfaceTextureListener::OnSurfaceTextureAvailable(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mHost->mListener) {
        Float refreshRate;
        mHost->mDefaultDisplayInfo->GetRefreshRate(&refreshRate);
        Int32 state;
        mHost->mDefaultDisplayInfo->GetState(&state);
        Int64 nanos;
        mHost->mDefaultDisplayInfo->GetPresentationDeadlineNanos(&nanos);
        mHost->mListener->OnWindowCreated(surfaceTexture, refreshRate, nanos, state);
    }
    return NOERROR;
}

//@Override
ECode OverlayDisplayWindow::MySurfaceTextureListener::OnSurfaceTextureDestroyed(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mHost->mListener)
        mHost->mListener->OnWindowDestroyed();
    *res = TRUE;
    return NOERROR;
}

//@Override
ECode OverlayDisplayWindow::MySurfaceTextureListener::OnSurfaceTextureSizeChanged(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

//@Override
ECode OverlayDisplayWindow::MySurfaceTextureListener::OnSurfaceTextureUpdated(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayWindow::MyViewOnTouchListener
//===================================================================================
CAR_INTERFACE_IMPL(
    OverlayDisplayWindow::MyViewOnTouchListener, Object, IViewOnTouchListener);

OverlayDisplayWindow::MyViewOnTouchListener::MyViewOnTouchListener(
    /* [in] */ OverlayDisplayWindow* owner)
    : mHost(owner)
{}

//@Override
ECode OverlayDisplayWindow::MyViewOnTouchListener::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Work in screen coordinates.
    Float oldX, oldY;
    event->GetX(&oldX);
    event->GetY(&oldY);

    Float rawX, rawY;
    event->GetRawX(&rawX);
    event->GetRawY(&rawY);
    event->SetLocation(rawX, rawY);

    Boolean result;
    mHost->mGestureDetector->OnTouchEvent(event, &result);
    mHost->mScaleGestureDetector->OnTouchEvent(event, &result);

    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mHost->SaveWindowParams();
            break;
    }

    // Revert to window coordinates.
    event->SetLocation(oldX, oldY);
    *res = TRUE;

    return NOERROR;
}

//===================================================================================
//          OverlayDisplayWindow::MyOnGestureListener
//===================================================================================
OverlayDisplayWindow::MyOnGestureListener::MyOnGestureListener(
    /* [in] */ OverlayDisplayWindow* owner)
    : mHost(owner)
{}

ECode OverlayDisplayWindow::MyOnGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    mHost->mLiveTranslationX -= distanceX;
    mHost->mLiveTranslationY -= distanceY;
    mHost->Relayout();
    *res = TRUE;
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayWindow::MyOnScaleGestureListener
//===================================================================================
OverlayDisplayWindow::MyOnScaleGestureListener::MyOnScaleGestureListener(
    /* [in] */ OverlayDisplayWindow* owner)
    : mHost(owner)
{}

//@Override
ECode OverlayDisplayWindow::MyOnScaleGestureListener::OnScale(
    /* [in ]*/ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Float scaleFactor = 1.0;
    detector->GetScaleFactor(&scaleFactor);
    mHost->mLiveScale *= scaleFactor;
    mHost->Relayout();
    *res = TRUE;
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayWindow::OverlayDisplayWindow
//===================================================================================
OverlayDisplayWindow::OverlayDisplayWindow(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ Boolean secure,
    /* [in] */ Int32 gravity,
    /* [in] */ IOverlayDisplayWindowListener* listener)
    : INITIAL_SCALE(0.5f)
    , MIN_SCALE(0.3f)
    , MAX_SCALE(1.0f)
    , WINDOW_ALPHA(0.8f)
    , DISABLE_MOVE_AND_RESIZE(FALSE)
    , mContext(context)
    , mName(name)
    , mWidth(width)
    , mHeight(height)
    , mDensityDpi(densityDpi)
    , mSecure(secure)
    , mGravity(gravity)
    , mListener(listener)
    , mWindowVisible(FALSE)
    , mWindowX(0)
    , mWindowY(0)
    , mWindowScale(0)
    , mLiveTranslationX(0)
    , mLiveTranslationY(0)
    , mLiveScale(1.0f)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    assert(resources != NULL);
    resources->GetString(R::string::display_manager_overlay_display_title, &mTitle);

    if (secure) {
        String tmp;
        resources->GetString(R::string::display_manager_overlay_display_secure_suffix, &tmp);
        mTitle += tmp;
    }

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&service);
    mDisplayManager = IDisplayManager::Probe(service);
    service = NULL;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    mWindowManager = IWindowManager::Probe(service);

    mWindowManager->GetDefaultDisplay((IDisplay**)&mDefaultDisplay);

    mDisplayListener = new MyDisplayListener(this);
    mSurfaceTextureListener = new MySurfaceTextureListener(this);
    mOnTouchListener = new MyViewOnTouchListener(this);
    mOnGestureListener = new MyOnGestureListener(this);
    mOnScaleGestureListener = new MyOnScaleGestureListener(this);

    UpdateDefaultDisplayInfo();

    CreateWindow();
}

void OverlayDisplayWindow::Show()
{
    if (!mWindowVisible) {
        mDisplayManager->RegisterDisplayListener(mDisplayListener, NULL);
        if (!UpdateDefaultDisplayInfo()) {
            mDisplayManager->UnregisterDisplayListener(mDisplayListener);
            return;
        }

        ClearLiveState();
        UpdateWindowParams();
        IViewManager::Probe(mWindowManager)->AddView(mWindowContent,
            IViewGroupLayoutParams::Probe(mWindowParams));
        mWindowVisible = TRUE;
    }
}

void OverlayDisplayWindow::Dismiss()
{
    if (mWindowVisible) {
        mDisplayManager->UnregisterDisplayListener(mDisplayListener);
        IViewManager::Probe(mWindowManager)->RemoveView(mWindowContent);
        mWindowVisible = FALSE;
    }
}

void OverlayDisplayWindow::Relayout()
{
    if (mWindowVisible) {
        UpdateWindowParams();
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(mWindowContent,
            IViewGroupLayoutParams::Probe(mWindowParams));
    }
}

//@Override
void OverlayDisplayWindow::Dump(
    /* [in] */ IPrintWriter* pw)
{
    // pw->PrintStringln(String("mWindowVisible=") + mWindowVisible);
    // pw->PrintStringln(String("mWindowX=") + mWindowX);
    // pw->PrintStringln(String("mWindowY=") + mWindowY);
    // pw->PrintStringln(String("mWindowScale=") + mWindowScale);
    // pw->PrintStringln(String("mWindowParams=") + mWindowParams);
    // if (mTextureView != NULL) {
    //     pw->PrintStringln(String("mTextureView.getScaleX()=") + mTextureView.getScaleX());
    //     pw->PrintStringln(String("mTextureView.getScaleY()=") + mTextureView.getScaleY());
    // }
    // pw->PrintStringln(String("mLiveTranslationX=") + mLiveTranslationX);
    // pw->PrintStringln(String("mLiveTranslationY=") + mLiveTranslationY);
    // pw->PrintStringln(String("mLiveScale=") + mLiveScale);
}

Boolean OverlayDisplayWindow::UpdateDefaultDisplayInfo()
{
    Boolean res;
    mDefaultDisplay->GetDisplayInfo(mDefaultDisplayInfo, &res);
    if (!res) {
        Slogger::W(TAG, "Cannot show overlay display because there is no "
            "default display upon which to show it.");
        return FALSE;
    }
    return TRUE;
}

void OverlayDisplayWindow::CreateWindow()
{
    AutoPtr<ILayoutInflater> inflater;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);
    assert(inflater != NULL);

    mWindowContent = NULL;
    inflater->Inflate(
        R::layout::overlay_display_window, NULL, (IView**)&mWindowContent);
    mWindowContent->SetOnTouchListener(mOnTouchListener);

    AutoPtr<IView> view;
    mWindowContent->FindViewById(R::id::overlay_display_window_texture, (IView**)&view);
    mTextureView = ITextureView::Probe(view);
    view->SetPivotX(0);
    view->SetPivotY(0);
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    view->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    layoutParams->SetWidth(mWidth);
    layoutParams->SetHeight(mHeight);
    mTextureView->SetOpaque(FALSE);
    mTextureView->SetSurfaceTextureListener(mSurfaceTextureListener);

    view = NULL;
    mWindowContent->FindViewById(R::id::overlay_display_window_title, (IView**)&view);
    mTitleTextView = ITextView::Probe(view);
    AutoPtr<ICharSequence> title;
    CString::New(mTitle, (ICharSequence**)&title);
    mTitleTextView->SetText(title);

    mWindowParams = NULL;
    CWindowManagerLayoutParams::New(
        IWindowManagerLayoutParams::TYPE_DISPLAY_OVERLAY,
        (IWindowManagerLayoutParams**)&mWindowParams);
    Int32 flags;
    mWindowParams->GetFlags(&flags);
    flags |= IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
        | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
        | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
        | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
    if (mSecure) {
        flags |= IWindowManagerLayoutParams::FLAG_SECURE;
    }
    if (DISABLE_MOVE_AND_RESIZE) {
        flags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
    }
    mWindowParams->SetFlags(flags);

    Int32 privateFlags;
    mWindowParams->GetPrivateFlags(&privateFlags);
    privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_FORCE_HARDWARE_ACCELERATED;
    mWindowParams->SetPrivateFlags(privateFlags);

    mWindowParams->SetAlpha(WINDOW_ALPHA);
    mWindowParams->SetGravity(IGravity::TOP | IGravity::LEFT);
    mWindowParams->SetTitle(title);

    mGestureDetector = NULL;
    CGestureDetector::New(
        mContext, mOnGestureListener, (IGestureDetector**)&mGestureDetector);
    mScaleGestureDetector = NULL;
    CScaleGestureDetector::New(
        mContext, mOnScaleGestureListener,
        (IScaleGestureDetector**)&mScaleGestureDetector);

    // Set the initial position and scale.
    // The position and scale will be clamped when the display is first shown.
    Int32 w, h;
    mDefaultDisplayInfo->GetLogicalWidth(&w);
    mDefaultDisplayInfo->GetLogicalHeight(&h);
    mWindowX = (mGravity & IGravity::LEFT) == IGravity::LEFT ? 0 : w;
    mWindowY = (mGravity & IGravity::TOP) == IGravity::TOP ? 0 : h;
    mWindowScale = INITIAL_SCALE;
}

void OverlayDisplayWindow::UpdateWindowParams()
{
    Int32 w, h;
    mDefaultDisplayInfo->GetLogicalWidth(&w);
    mDefaultDisplayInfo->GetLogicalHeight(&h);

    Float scale = mWindowScale * mLiveScale;
    scale = Elastos::Core::Math::Min(scale, (Float)w / mWidth);
    scale = Elastos::Core::Math::Min(scale, (Float)h / mHeight);
    scale = Elastos::Core::Math::Max(MIN_SCALE, Elastos::Core::Math::Min(MAX_SCALE, scale));

    Float offsetScale = (scale / mWindowScale - 1.0f) * 0.5f;
    Int32 width = (Int32)(mWidth * scale);
    Int32 height = (Int32)(mHeight * scale);
    Int32 x = (Int32)(mWindowX + mLiveTranslationX - width * offsetScale);
    Int32 y = (Int32)(mWindowY + mLiveTranslationY - height * offsetScale);
    x = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(x, w - width));
    y = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(y, h - height));

    if (DEBUG) {
        Slogger::D(TAG, "UpdateWindowParams: scale=%f, offsetScale=%f, "
            "x=%d, width=%d, height=%d", scale, offsetScale, x, width, height);
    }

    IView* view = IView::Probe(mTextureView);
    view->SetScaleX(scale);
    view->SetScaleY(scale);

    IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(mWindowParams);
    mWindowParams->SetX(x);
    mWindowParams->SetY(y);
    vglp->SetWidth(width);
    vglp->SetHeight(height);
}

void OverlayDisplayWindow::SaveWindowParams()
{
    mWindowParams->GetX(&mWindowX);
    mWindowParams->GetY(&mWindowY);
    IView* view = IView::Probe(mTextureView);
    view->GetScaleX(&mWindowScale);
    ClearLiveState();
}

void OverlayDisplayWindow::ClearLiveState()
{
    mLiveTranslationX = 0.0f;
    mLiveTranslationY = 0.0f;
    mLiveScale = 1.0f;
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
