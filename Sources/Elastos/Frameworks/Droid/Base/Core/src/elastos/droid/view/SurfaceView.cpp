//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/SurfaceView.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/view/CSurface.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/CSurfaceViewWindow.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/graphics/PixelFormat.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::PixelFormat;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::RegionOp_UNION;

namespace Elastos {
namespace Droid {
namespace View {

const char* TAG = "SurfaceView";
const Boolean SurfaceView::DEBUG = FALSE;

const Int32 SurfaceView::KEEP_SCREEN_ON_MSG = 1;
const Int32 SurfaceView::GET_NEW_SURFACE_MSG = 2;
const Int32 SurfaceView::UPDATE_WINDOW_MSG = 3;

ECode SurfaceView::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    switch(what) {
        case SurfaceView::KEEP_SCREEN_ON_MSG: {
            mHost->SetKeepScreenOn(arg1 != 0);
        } break;
        case SurfaceView::GET_NEW_SURFACE_MSG: {
            mHost->HandleGetNewSurface();
        } break;
        case SurfaceView::UPDATE_WINDOW_MSG: {
            mHost->UpdateWindow(FALSE, FALSE);
        } break;
    }

    return NOERROR;
}

//==============================================================================
//          SurfaceView::MySurfaceHolder
//==============================================================================
CAR_INTERFACE_IMPL(SurfaceView::MySurfaceHolder, Object, ISurfaceHolder)

SurfaceView::MySurfaceHolder::MySurfaceHolder(
    /* [in] */ SurfaceView* host)
    : mHost(host)
{}

SurfaceView::MySurfaceHolder::~MySurfaceHolder()
{}

ECode SurfaceView::MySurfaceHolder::IsCreating(
    /* [in] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->mIsCreating;
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::AddCallback(
    /* [in] */ ISurfaceHolderCallback* callback)
{
    AutoLock lock(mHost->mCallbacksLock);
    AutoPtr<ISurfaceHolderCallback> cb = callback;
    // This is a linear search, but in practice we'll
    // have only a couple callbacks, so it doesn't matter.
    List<AutoPtr<ISurfaceHolderCallback> >::Iterator iter
        = Find(mHost->mCallbacks.Begin(), mHost->mCallbacks.End(), cb);
    if (iter == mHost->mCallbacks.End()) {
        mHost->mCallbacks.PushBack(cb);
    }

    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::RemoveCallback(
    /* [in] */ ISurfaceHolderCallback* callback)
{
    AutoLock lock(mHost->mCallbacksLock);
    AutoPtr<ISurfaceHolderCallback> cb = callback;
    List<AutoPtr<ISurfaceHolderCallback> >::Iterator iter
        = Find(mHost->mCallbacks.Begin(), mHost->mCallbacks.End(), cb);
    if (iter != mHost->mCallbacks.End()) {
        mHost->mCallbacks.Erase(iter);
    }

    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::SetFixedSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mHost->mRequestedWidth != width || mHost->mRequestedHeight != height) {
        mHost->mRequestedWidth = width;
        mHost->mRequestedHeight = height;
        mHost->RequestLayout();
    }

    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::SetSizeFromLayout()
{
    if (mHost->mRequestedWidth != -1 || mHost->mRequestedHeight != -1) {
        mHost->mRequestedWidth = mHost->mRequestedHeight = -1;
        mHost->RequestLayout();
    }

    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::SetFormat(
    /* [in] */ Int32 format)
{
    // for backward compatibility reason, OPAQUE always
    // means 565 for SurfaceView
    if (format == IPixelFormat::OPAQUE) {
        format = IPixelFormat::RGB_565;
    }

    mHost->mRequestedFormat = format;
    if (mHost->mWindow != NULL) {
        mHost->UpdateWindow(FALSE, FALSE);
    }

    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::SetType(
    /* [in] */ Int32 type)
{
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::SetKeepScreenOn(
    /* [in] */ Boolean screenOn)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(SurfaceView::KEEP_SCREEN_ON_MSG,
        screenOn ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode SurfaceView::MySurfaceHolder::LockCanvas(
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas);
    AutoPtr<ICanvas> _canvas = InternalLockCanvas(NULL);
    *canvas = _canvas.Get();
    REFCOUNT_ADD(*canvas);
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::LockCanvas(
    /* [in]*/ IRect* dirty,
    /* [out]*/ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas);
    AutoPtr<ICanvas> _canvas = InternalLockCanvas(dirty);
    *canvas = _canvas.Get();
    REFCOUNT_ADD(*canvas);
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::UnlockCanvasAndPost(
    /* [in]*/ ICanvas* canvas)
{
    mHost->mSurface->UnlockCanvasAndPost(canvas);
    mHost->mSurfaceLock.Unlock();
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::GetSurface(
    /* [out]*/ ISurface** surface)
{
    VALIDATE_NOT_NULL(surface);
    *surface = mHost->mSurface;
    REFCOUNT_ADD(*surface);
    return NOERROR;
}

ECode SurfaceView::MySurfaceHolder::GetSurfaceFrame(
    /* [out]*/ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = mHost->mSurfaceFrame;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

AutoPtr<ICanvas> SurfaceView::MySurfaceHolder::InternalLockCanvas(
    /* [in]*/ IRect* _dirty)
{
    AutoLock lock(mHost->mSurfaceLock);

    if (DEBUG) {
        Logger::I("SurfaceView", "Locking canvas... stopped=%d, win=%p\n",
            mHost->mDrawingStopped, mHost->mWindow.Get());
    }

    AutoPtr<IRect> dirty = _dirty;
    AutoPtr<ICanvas> c;
    if (!mHost->mDrawingStopped && mHost->mWindow != NULL) {
        ECode ec = mHost->mSurface->LockCanvas(dirty, (ICanvas**)&c);
        if (FAILED(ec)) {
            Logger::E("SurfaceView", "Exception locking surface %08x", ec);
        }
    }

    if (DEBUG) Logger::I("SurfaceView", "Returned canvas: %p", c.Get());
    if (c != NULL) {
        mHost->mLastLockTime = SystemClock::GetUptimeMillis();
        return c;
    }

    // If the Surface is not ready to be drawn, then return NULL,
    // but throttle calls to this function so it isn't called more
    // than every 100ms.
    Int64 now = SystemClock::GetUptimeMillis();
    Int64 nextTime = mHost->mLastLockTime + 100;
    if (nextTime > now) {
        usleep(nextTime-now);
        now = SystemClock::GetUptimeMillis();
    }

    mHost->mLastLockTime = now;

    return c;
}

//==============================================================================
//          SurfaceView::MyOnPreDrawListener
//==============================================================================

CAR_INTERFACE_IMPL(SurfaceView::MyOnPreDrawListener, Object, IOnPreDrawListener)

SurfaceView::MyOnPreDrawListener::MyOnPreDrawListener(
    /* [in] */ SurfaceView* host)
    : mHost(host)
{
}

SurfaceView::MyOnPreDrawListener::~MyOnPreDrawListener()
{
}

ECode SurfaceView::MyOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // reposition ourselves where the surface is
    Int32 w, h;
    mHost->GetWidth(&w);
    mHost->GetHeight(&h);
    mHost->mHaveFrame = w > 0 && h > 0;
    mHost->UpdateWindow(FALSE, FALSE);
    *result = TRUE;
    return NOERROR;
}

//==============================================================================
//          SurfaceView::MyOnScrollChangedListener
//==============================================================================

CAR_INTERFACE_IMPL(SurfaceView::MyOnScrollChangedListener, Object, IOnScrollChangedListener)

SurfaceView::MyOnScrollChangedListener::MyOnScrollChangedListener(
    /* [in] */ SurfaceView* host)
    : mHost(host)
{
}

SurfaceView::MyOnScrollChangedListener::~MyOnScrollChangedListener()
{
}

ECode SurfaceView::MyOnScrollChangedListener::OnScrollChanged()
{
    mHost->UpdateWindow(FALSE, FALSE);
    return NOERROR;
}

//==============================================================================
//          SurfaceView
//==============================================================================
CAR_INTERFACE_IMPL(SurfaceView, View, ISurfaceView)

SurfaceView::SurfaceView()
    : mLastSurfaceWidth(-1)
    , mLastSurfaceHeight(-1)
    , mUpdateWindowNeeded(FALSE)
    , mReportDrawNeeded(FALSE)
    , mDrawingStopped(TRUE)
    , mWindowType(IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA)
    , mIsCreating(FALSE)
    , mRequestedVisible(FALSE)
    , mWindowVisibility(FALSE)
    , mViewVisibility(FALSE)
    , mRequestedWidth(-1)
    , mRequestedHeight(-1)
    , mRequestedFormat(IPixelFormat::RGB_565)
    , mHaveFrame(FALSE)
    , mSurfaceCreated(FALSE)
    , mLastLockTime(0)
    , mVisible(FALSE)
    , mLeft(-1)
    , mTop(-1)
    , mWidth(-1)
    , mHeight(-1)
    , mFormat(-1)
{
}

ECode SurfaceView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(View::constructor(context));
    Init();
    return NOERROR;
}

ECode SurfaceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(View::constructor(context, attrs));
    Init();
    return NOERROR;
}

ECode SurfaceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyle));
    Init();
    return NOERROR;
}

ECode SurfaceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyleAttr, defStyleRes));
    Init();
    return NOERROR;
}

SurfaceView::~SurfaceView()
{
}

void SurfaceView::Init()
{
    mLocation = ArrayOf<Int32>::Alloc(2);
    CSurface::New((ISurface**)&mSurface);       // Current surface in use
    CSurface::New((ISurface**)&mNewSurface);    // New surface we are switching to
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&mLayout);
    CRect::New((IRect**)&mVisibleInsets);
    CRect::New((IRect**)&mWinFrame);
    CRect::New((IRect**)&mOverscanInsets);
    CRect::New((IRect**)&mContentInsets);
    CRect::New((IRect**)&mStableInsets);
    CSurfaceViewWindow::New(this, (IBaseIWindow**)&mWindow);
    CConfiguration::New((IConfiguration**)&mConfiguration);
    AutoPtr<MyHandler> h = new MyHandler(this);
    h->constructor();
    mHandler = h.Get();

    mScrollChangedListener = new MyOnScrollChangedListener(this);
    CRect::New((IRect**)&mSurfaceFrame);
    mDrawListener = new MyOnPreDrawListener(this);

    SetWillNotDraw(TRUE);
}

ECode SurfaceView::GetHolder(
    /* [out] */ ISurfaceHolder** holder)
{
    *holder = mSurfaceHolder;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

ECode SurfaceView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    mParent->RequestTransparentRegion(this);
    GetWindowSession((IWindowSession**)&mSession);

    assert(mSession != NULL);
    AutoPtr<IBinder> token;
    GetWindowToken((IBinder**)&token);
    mLayout->SetToken(token);

    AutoPtr<ICharSequence> text;
    CString::New(String("SurfaceView"), (ICharSequence**)&text);
    mLayout->SetTitle(text);
    Int32 visibility;
    GetVisibility(&visibility);
    mViewVisibility = visibility == IView::VISIBLE;

    if (!mGlobalListenersAdded) {
        AutoPtr<IViewTreeObserver> observer;
        GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnScrollChangedListener(mScrollChangedListener);
        observer->AddOnPreDrawListener(mDrawListener);
        mGlobalListenersAdded = TRUE;
    }
    return NOERROR;
}

ECode SurfaceView::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    View::OnWindowVisibilityChanged(visibility);
    mWindowVisibility = visibility == IView::VISIBLE;
    mRequestedVisible = mWindowVisibility && mViewVisibility;
    UpdateWindow(FALSE, FALSE);
    return NOERROR;
}

ECode SurfaceView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    View::SetVisibility(visibility);
    mViewVisibility = visibility == IView::VISIBLE;

    Boolean newRequestedVisible = mWindowVisibility && mViewVisibility;
    if (newRequestedVisible != mRequestedVisible) {
        // our base class (View) invalidates the layout only when
        // we go from/to the GONE state. However, SurfaceView needs
        // to request a re-layout when the visibility changes at all.
        // This is needed because the transparent region is computed
        // as part of the layout phase, and it changes (obviously) when
        // the visibility changes.
        RequestLayout();
    }
    mRequestedVisible = newRequestedVisible;

    UpdateWindow(FALSE, FALSE);

    return NOERROR;
}

ECode SurfaceView::OnDetachedFromWindow()
{
    if (mGlobalListenersAdded) {
        AutoPtr<IViewTreeObserver> observer;
        GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnScrollChangedListener(mScrollChangedListener);
        observer->RemoveOnPreDrawListener(mDrawListener);
        mGlobalListenersAdded = FALSE;
    }

    mRequestedVisible = FALSE;
    UpdateWindow(FALSE, FALSE);
    mHaveFrame = FALSE;
    if (mWindow != NULL) {
        mSession->Remove(IIWindow::Probe(mWindow));
        mWindow = NULL;
    }

    mSession = NULL;
    mLayout->SetToken(NULL);

    return View::OnDetachedFromWindow();
}

ECode SurfaceView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = mRequestedWidth >= 0
            ? ResolveSizeAndState(mRequestedWidth, widthMeasureSpec, 0)
            : GetDefaultSize(0, widthMeasureSpec);
    Int32 height = mRequestedHeight >= 0
            ? ResolveSizeAndState(mRequestedHeight, heightMeasureSpec, 0)
            : GetDefaultSize(0, heightMeasureSpec);

    SetMeasuredDimension(width, height);
    return NOERROR;
}

/** @hide */
Boolean SurfaceView::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Boolean result = View::SetFrame(left, top, right, bottom);
    UpdateWindow(FALSE, FALSE);
    return result;
}

ECode SurfaceView::GatherTransparentRegion(
    /* [in] */ IRegion* region,
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque);
    if (mWindowType == IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL) {
        return View::GatherTransparentRegion(region, isOpaque);
    }

    if ((mPrivateFlags & PFLAG_SKIP_DRAW) == 0) {
        // this view draws, remove it from the transparent region
        View::GatherTransparentRegion(region, isOpaque);
    }
    else if (region != NULL) {
        Int32 w, h;
        GetWidth(&w);
        GetHeight(&h);
        if (w > 0 && h > 0) {
            GetLocationInWindow(mLocation);
            // otherwise, punch a hole in the whole hierarchy
            Int32 l = (*mLocation)[0];
            Int32 t = (*mLocation)[1];

            Boolean res;
            region->Op(l, t, l+w, t+h, RegionOp_UNION, &res);
        }
    }
    if (PixelFormat::FormatHasAlpha(mRequestedFormat)) {
        *isOpaque = FALSE;
    }
    return NOERROR;
}

ECode SurfaceView::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mWindowType != IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL) {
        // draw() is not called when PFLAG_SKIP_DRAW is set
        if ((mPrivateFlags & PFLAG_SKIP_DRAW) == 0) {
            // punch a whole in the view-hierarchy below us
            canvas->DrawColor(0, PorterDuffMode_CLEAR);
        }
    }
    return View::Draw(canvas);
}

ECode SurfaceView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mWindowType != IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL) {
        // if PFLAG_SKIP_DRAW is cleared, draw() has already punched a hole
        if ((mPrivateFlags & PFLAG_SKIP_DRAW) == PFLAG_SKIP_DRAW) {
            // punch a whole in the view-hierarchy below us
            canvas->DrawColor(0, PorterDuffMode_CLEAR);
        }
    }

    return View::DispatchDraw(canvas);
}

ECode SurfaceView::SetZOrderMediaOverlay(
    /* [in] */ Boolean isMediaOverlay)
{
    mWindowType = isMediaOverlay
            ? IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA_OVERLAY
            : IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA;

    return NOERROR;
}

ECode SurfaceView::SetZOrderOnTop(
    /* [in] */ Boolean onTop)
{
    Int32 flags;
    mLayout->GetFlags(&flags);
    if (onTop) {
        mWindowType = IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL;
        // ensures the surface is placed below the IME
        flags |= IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM;
    } else {
        mWindowType = IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA;
        flags &= ~IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM;
    }
    mLayout->SetFlags(flags);
    return NOERROR;
}

ECode SurfaceView::SetSecure(
    /* [in] */ Boolean isSecure)
{
    Int32 flags;
    mLayout->GetFlags(&flags);
    if (isSecure) {
        flags |= IWindowManagerLayoutParams::FLAG_SECURE;
    } else {
        flags &= ~IWindowManagerLayoutParams::FLAG_SECURE;
    }
    mLayout->SetFlags(flags);
    return NOERROR;
}

ECode SurfaceView::SetWindowType(
    /* [in] */ Int32 type)
{
    mWindowType = type;
    return NOERROR;
}

ECode SurfaceView::UpdateWindow(
    /* [in] */ Boolean force,
    /* [in] */ Boolean redrawNeeded)
{
    if (!mHaveFrame) {
        return NOERROR;
    }

    AutoPtr<IViewRootImpl> root;
    GetViewRootImpl((IViewRootImpl**)&root);
    ViewRootImpl* viewRoot = (ViewRootImpl*)root.Get();
    if (viewRoot != NULL) {
        mTranslator = viewRoot->mTranslator;
    }

    if (mTranslator != NULL) {
        ((CSurface*)mSurface.Get())->SetCompatibilityTranslator(mTranslator);
    }

    Int32 myWidth = mRequestedWidth;
    if (myWidth <= 0) GetWidth(&myWidth);
    Int32 myHeight = mRequestedHeight;
    if (myHeight <= 0) GetHeight(&myHeight);

    GetLocationInWindow(mLocation);
    Boolean creating = mWindow == NULL;
    Boolean formatChanged = mFormat != mRequestedFormat;
    Boolean sizeChanged = mWidth != myWidth || mHeight != myHeight;
    Boolean visibleChanged = mVisible != mRequestedVisible;

    if (force || creating || formatChanged || sizeChanged || visibleChanged
        || mLeft != (*mLocation)[0] || mTop != (*mLocation)[1]
        || mUpdateWindowNeeded || mReportDrawNeeded || redrawNeeded) {

        if (DEBUG)
            Logger::I(TAG, "Changes: creating=%d format=%d size=%d visible=%d left=%d top=%d w=%d h=%d",
                creating, formatChanged, sizeChanged, visibleChanged,
                (mLeft != (*mLocation)[0]), (mTop != (*mLocation)[1]),
                myWidth, myHeight);

        //try {
            Boolean visible = mVisible = mRequestedVisible;
            mLeft = (*mLocation)[0];
            mTop = (*mLocation)[1];
            mWidth = myWidth;
            mHeight = myHeight;
            mFormat = mRequestedFormat;

            // Scaling/Translate window's layout here because mLayout is not used elsewhere.
            // Places the window relative
            mLayout->SetX(mLeft);
            mLayout->SetY(mTop);
            Int32 w, h;
            GetWidth(&w);
            GetHeight(&h);

            (IViewGroupLayoutParams::Probe(mLayout))->SetWidth(w);
            (IViewGroupLayoutParams::Probe(mLayout))->SetHeight(h);

            if (mTranslator != NULL) {
                mTranslator->TranslateLayoutParamsInAppWindowToScreen(mLayout);
            }

            mLayout->SetFormat(mRequestedFormat);
            Int32 flags;
            mLayout->GetFlags(&flags);
            flags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                    | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
                    | IWindowManagerLayoutParams::FLAG_SCALED
                    | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
            mLayout->SetFlags(flags);

            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            AutoPtr<ICompatibilityInfo> compat;
            resources->GetCompatibilityInfo((ICompatibilityInfo**)&compat);
            Boolean supportsScreen;
            compat->SupportsScreen(&supportsScreen);

            if (!supportsScreen) {
                mLayout->GetFlags(&flags);
                mLayout->SetFlags(flags | IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW);
            }

            mLayout->GetFlags(&flags);
            mLayout->SetFlags(flags | IWindowManagerLayoutParams::PRIVATE_FLAG_NO_MOVE_ANIMATION);

            if (mWindow == NULL) {
                AutoPtr<IDisplay> display;
                GetDisplay((IDisplay**)&display);
                CSurfaceViewWindow::New(this, (IBaseIWindow**)&mWindow);
                mLayout->SetType(mWindowType);
                mLayout->SetGravity(IGravity::START | IGravity::TOP);

                assert(mSession != NULL && display != NULL && mWindow != NULL);

                Int32 displayId = 0;
                display->GetDisplayId(&displayId);
                AutoPtr<IRect> outContentInsets;
                Int32 result = 0;
                Int32 seq;
                mWindow->GetSeq(&seq);
                mSession->AddToDisplayWithoutInputChannel(
                        IIWindow::Probe(mWindow), seq, mLayout,
                        mVisible ? IView::VISIBLE : IView::GONE,
                        displayId, mContentInsets, (IRect**)&outContentInsets, &result);
                mContentInsets->Set(outContentInsets);
            }

            Boolean realSizeChanged;
            Boolean reportDrawNeeded;
            Int32 relayoutResult = 0;

            {
                AutoLock lock(mSurfaceLock);
                mUpdateWindowNeeded = FALSE;
                reportDrawNeeded = mReportDrawNeeded;
                mReportDrawNeeded = FALSE;
                mDrawingStopped = !visible;

                //if (DEBUG) Log.i(TAG, "Cur surface: " + mSurface);
                mSurfaceLock.Unlock();
                AutoPtr<IRect> outFrame, outOverscanInsets, outContentInsets, outVisibleInsets, outStableInsets;
                AutoPtr<IConfiguration> outConfig;
                AutoPtr<ISurface> outSurface;
                Int32 seq;
                mWindow->GetSeq(&seq);
                mSession->Relayout(
                        IIWindow::Probe(mWindow), seq, mLayout, mWidth, mHeight,
                        visible ? IView::VISIBLE : IView::GONE,
                        IWindowManagerGlobal::RELAYOUT_DEFER_SURFACE_DESTROY,
                        mWinFrame, mOverscanInsets, mContentInsets, mVisibleInsets,
                        mStableInsets, mConfiguration, mNewSurface,
                        (IRect**)&outFrame, (IRect**)&outOverscanInsets, (IRect**)&outContentInsets,
                        (IRect**)&outVisibleInsets, (IRect**)&outStableInsets,
                        (IConfiguration**)&outConfig, &relayoutResult, (ISurface**)&outSurface);
                mSurfaceLock.Lock();
                mWinFrame = outFrame;
                mContentInsets = outContentInsets;
                mVisibleInsets = outVisibleInsets;
                mConfiguration = outConfig;
                mNewSurface = outSurface;

                if ((relayoutResult & IWindowManagerGlobal::RELAYOUT_RES_FIRST_TIME) != 0) {
                    mReportDrawNeeded = TRUE;
                }

                /*if (localLOGV) Log.i(TAG, "New surface: " + mSurface
                        + ", vis=" + visible + ", frame=" + mWinFrame);*/

                Int32 surfaceWidth, surfaceHeight;
                mWinFrame->GetWidth(&surfaceWidth);
                mWinFrame->GetHeight(&surfaceHeight);
                if (mTranslator != NULL) {
                    Float appInvertedScale = 0.0;
                    mTranslator->GetApplicationInvertedScale(&appInvertedScale);
                    surfaceWidth = (Int32)(surfaceWidth * appInvertedScale + 0.5f);
                    surfaceHeight = (Int32)(surfaceHeight * appInvertedScale + 0.5f);
                }

                mSurfaceFrame->Set(0, 0, surfaceWidth, surfaceHeight);

                realSizeChanged = mLastSurfaceWidth != surfaceWidth
                        || mLastSurfaceHeight != surfaceHeight;
                mLastSurfaceWidth = surfaceWidth;
                mLastSurfaceHeight = surfaceHeight;
            }

            //try {
            redrawNeeded |= creating | reportDrawNeeded;

            AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks;
            Int32 size = 0;
            Boolean surfaceChanged = (relayoutResult
                    & IWindowManagerGlobal::RELAYOUT_RES_SURFACE_CHANGED) != 0;
            if (mSurfaceCreated && (surfaceChanged || (!visible && visibleChanged))) {
                mSurfaceCreated = FALSE;
                Boolean valid = FALSE;
                mSurface->IsValid(&valid);
                if (valid) {
                    // if (DEBUG) Log.i(TAG, "visibleChanged -- surfaceDestroyed");
                    callbacks = GetSurfaceCallbacks();
                    size = callbacks->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                        assert(c != NULL);
                        c->SurfaceDestroyed(mSurfaceHolder.Get());
                    }
                }
            }

            mSurface->TransferFrom(mNewSurface);

            Boolean valid = FALSE;
            mSurface->IsValid(&valid);
            if (visible && valid) {
                if (!mSurfaceCreated && (surfaceChanged || visibleChanged)) {
                    mSurfaceCreated = TRUE;
                    mIsCreating = TRUE;
                    // if (DEBUG) Log.i(TAG, "visibleChanged -- surfaceCreated");
                    if (callbacks == NULL) {
                        callbacks = GetSurfaceCallbacks();
                    }
                    size = callbacks->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                        assert(c != NULL);
                        c->SurfaceCreated(mSurfaceHolder.Get());
                    }
                }
                if (creating || formatChanged || sizeChanged
                        || visibleChanged || realSizeChanged) {
                    // if (DEBUG) Log.i(TAG, "surfaceChanged -- format=" + mFormat
                    //         + " w=" + myWidth + " h=" + myHeight);
                    if (callbacks == NULL) {
                        callbacks = GetSurfaceCallbacks();
                    }
                    size = callbacks->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                        assert(c != NULL);
                        c->SurfaceChanged(mSurfaceHolder.Get(), mFormat, myWidth, myHeight);
                    }
                }
                if (redrawNeeded) {
                    // if (DEBUG) Log.i(TAG, "surfaceRedrawNeeded");
                    if (callbacks == NULL) {
                        callbacks = GetSurfaceCallbacks();
                    }
                    size = callbacks->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<ISurfaceHolderCallback> c = (*callbacks)[i];
                        assert(c != NULL);
                        if (ISurfaceHolderCallback2::Probe(c) != NULL) {
                            ISurfaceHolderCallback2::Probe(c)->SurfaceRedrawNeeded(
                                    mSurfaceHolder);
                        }
                    }
                }
            }

            mIsCreating = FALSE;
            if (redrawNeeded) {
                // if (DEBUG) Log.i(TAG, "finishedDrawing");
                mSession->FinishDrawing(IIWindow::Probe(mWindow));
            }

            mSession->PerformDeferredDestroy(IIWindow::Probe(mWindow));
        //} catch (RemoteException ex) {
        //}
        /*if (localLOGV) Log.v(
            TAG, "Layout: x=" + mLayout.x + " y=" + mLayout.y +
            " w=" + mLayout.width + " h=" + mLayout.height +
            ", frame=" + mSurfaceFrame);*/
    }
    return NOERROR;
}

AutoPtr<ArrayOf<ISurfaceHolderCallback*> > SurfaceView::GetSurfaceCallbacks()
{
    AutoLock lock(mCallbacksLock);

    AutoPtr<ArrayOf<ISurfaceHolderCallback*> > callbacks;
    Int32 size = mCallbacks.GetSize();
    callbacks = ArrayOf<ISurfaceHolderCallback*>::Alloc(size);
    List<AutoPtr<ISurfaceHolderCallback> >::Iterator iter = mCallbacks.Begin();
    for (Int32 i = 0; iter != mCallbacks.End(); ++iter, ++i) {
        callbacks->Set(i, *iter);
    }

    return callbacks;
}

ECode SurfaceView::HandleGetNewSurface()
{
    return UpdateWindow(FALSE, FALSE);
}

ECode SurfaceView::IsFixedSize(
    /* [out] */ Boolean* isFixedSize)
{
    VALIDATE_NOT_NULL(isFixedSize);
    *isFixedSize = mRequestedWidth != -1 || mRequestedHeight != -1;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
