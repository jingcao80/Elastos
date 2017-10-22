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

#ifndef __ELASTOS_DROID_VIEW_SURFACEVIEW_H__
#define __ELASTOS_DROID_VIEW_SURFACEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>
//#include "elastos/droid/view/CSurfaceViewWindow.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Content::Res::ICompatibilityInfoTranslator;
using Elastos::Droid::Internal::View::IBaseIWindow;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC SurfaceView
    : public View
    , public ISurfaceView
{
private:
    class MyHandler : public Handler
    {
    public:
        TO_STRING_IMPL("SurfaceView::MyHandler")

        MyHandler(
            /* [in] */ SurfaceView* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        SurfaceView* mHost;
    };

    class MySurfaceHolder:
        public Object,
        public ISurfaceHolder
    {
    public:
        MySurfaceHolder(
            /* [in] */ SurfaceView* host);

        virtual ~MySurfaceHolder();

        CAR_INTERFACE_DECL()

        CARAPI IsCreating(
            /* [in] */ Boolean* result);

        CARAPI AddCallback(
            /* [in] */ ISurfaceHolderCallback* callback);

        CARAPI RemoveCallback(
            /* [in] */ ISurfaceHolderCallback* callback);

        CARAPI SetFixedSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetSizeFromLayout();

        CARAPI SetFormat(
            /* [in] */ Int32 format);

        /**
         * @deprecated setType is now ignored.
         */
        //@Deprecated
        CARAPI SetType(
            /* [in] */ Int32 type);

        CARAPI SetKeepScreenOn(
            /* [in] */ Boolean screenOn);

        CARAPI LockCanvas(
            /* [out] */ ICanvas** canvas);

        CARAPI LockCanvas(
            /* [in]*/ IRect* dirty,
            /* [out]*/ ICanvas** canvas);

        CARAPI UnlockCanvasAndPost(
            /* [in]*/ ICanvas* canvas);

        CARAPI GetSurface(
            /* [out]*/ ISurface** surface);

        CARAPI GetSurfaceFrame(
            /* [out]*/ IRect** rect);

    private:
        CARAPI_(AutoPtr<ICanvas>) InternalLockCanvas(
            /* [in]*/ IRect* dirty);

    private:
        SurfaceView*    mHost;
    };

    class MyOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnPreDrawListener(
            /* [in] */ SurfaceView* host);

        virtual ~MyOnPreDrawListener();

        CARAPI OnPreDraw(
            /* [out] */Boolean* result);

    private:
        SurfaceView*    mHost;
    };

    class MyOnScrollChangedListener
        : public Object
        , public IOnScrollChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnScrollChangedListener(
            /* [in] */ SurfaceView* host);

        virtual ~MyOnScrollChangedListener();

        CARAPI OnScrollChanged();

    private:
        SurfaceView*    mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SurfaceView();

    virtual ~SurfaceView();

    /**
     * Return the SurfaceHolder providing access and control over this
     * SurfaceView's underlying surface.
     *
     * @return SurfaceHolder The holder of the surface.
     */
    virtual CARAPI GetHolder(
        /* [out] */ ISurfaceHolder** holder);

    virtual CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI GatherTransparentRegion(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* isOpaque);

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Control whether the surface view's surface is placed on top of another
     * regular surface view in the window (but still behind the window itself).
     * This is typically used to place overlays on top of an underlying media
     * surface view.
     *
     * <p>Note that this must be set before the surface view's containing
     * window is attached to the window manager.
     *
     * <p>Calling this overrides any previous call to {@link #setZOrderOnTop}.
     */
    virtual CARAPI SetZOrderMediaOverlay(
        /* [in] */ Boolean isMediaOverlay);

    /**
     * Control whether the surface view's surface is placed on top of its
     * window.  Normally it is placed behind the window, to allow it to
     * (for the most part) appear to composite with the views in the
     * hierarchy.  By setting this, you cause it to be placed above the
     * window.  This means that none of the contents of the window this
     * SurfaceView is in will be visible on top of its surface.
     *
     * <p>Note that this must be set before the surface view's containing
     * window is attached to the window manager.
     *
     * <p>Calling this overrides any previous call to {@link #setZOrderMediaOverlay}.
     */
    virtual CARAPI SetZOrderOnTop(
        /* [in] */ Boolean onTop);

    /**
     * Control whether the surface view's content should be treated as secure,
     * preventing it from appearing in screenshots or from being viewed on
     * non-secure displays.
     *
     * <p>Note that this must be set before the surface view's containing
     * window is attached to the window manager.
     *
     * <p>See {@link android.view.Display#FLAG_SECURE} for details.
     *
     * @param isSecure True if the surface view is secure.
     */
    virtual CARAPI SetSecure(
        /* [in] */ Boolean isSecure);

    /**
     * Hack to allow special layering of windows.  The type is one of the
     * types in WindowManager.LayoutParams.  This is a hack so:
     * @hide
     */
    virtual CARAPI SetWindowType(
        /* [in] */ Int32 type);

    /**
     * Check to see if the surface has fixed size dimensions or if the surface's
     * dimensions are dimensions are dependent on its current layout.
     *
     * @return TRUE if the surface has dimensions that are fixed in size
     * @hide
     */
    virtual CARAPI IsFixedSize(
        /* [out] */ Boolean* isFixedSize);

    virtual CARAPI HandleGetNewSurface();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /** @hide */
    virtual CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

protected:
    CARAPI_(void) Init();

    CARAPI UpdateWindow(
        /* [in] */ Boolean force,
        /* [in] */ Boolean redrawNeeded);

private:
    CARAPI_(AutoPtr<ArrayOf<ISurfaceHolderCallback*> >) GetSurfaceCallbacks();

public:
    static const Int32 KEEP_SCREEN_ON_MSG;
    static const Int32 GET_NEW_SURFACE_MSG;
    static const Int32 UPDATE_WINDOW_MSG;
    static const Boolean DEBUG;

    AutoPtr<IBaseIWindow> mWindow;
    AutoPtr<IRect> mWinFrame;// = new Rect();
    AutoPtr<IHandler> mHandler;
    Object mSurfaceLock;
    Int32 mLastSurfaceWidth;// = -1;
    Int32 mLastSurfaceHeight;// = -1;
    Boolean mUpdateWindowNeeded;
    Boolean mReportDrawNeeded;

private:
//    friend class MySurfaceHolder;
    //friend class CSurfaceViewWindow;

    AutoPtr<ISurfaceHolder> mSurfaceHolder;

    List<AutoPtr<ISurfaceHolderCallback> > mCallbacks;

    AutoPtr<ArrayOf<Int32> > mLocation;// = new Int32[2];

    //ReentrantLock mSurfaceLock = new ReentrantLock();
    AutoPtr<ISurface> mSurface;// = new Surface();       // Current surface in use
    AutoPtr<ISurface> mNewSurface;// = new Surface();    // New surface we are switching to
    Boolean mDrawingStopped;// = TRUE;

    AutoPtr<IWindowManagerLayoutParams> mLayout; //= new WindowManager.LayoutParams();
    AutoPtr<IWindowSession> mSession;
    AutoPtr<IRect> mVisibleInsets;// = new Rect();

    AutoPtr<IRect> mOverscanInsets;
    AutoPtr<IRect> mContentInsets;// = new Rect();
    AutoPtr<IRect> mStableInsets;
    AutoPtr<IConfiguration> mConfiguration;// = new Configuration();

    Int32 mWindowType;// = WindowManager.LayoutParams.TYPE_APPLICATION_MEDIA;

    Boolean mIsCreating;// = FALSE;

    AutoPtr<IOnScrollChangedListener> mScrollChangedListener;

    Boolean mRequestedVisible;// = FALSE;
    Boolean mWindowVisibility;// = FALSE;
    Boolean mViewVisibility;// = FALSE;
    Int32 mRequestedWidth;// = -1;
    Int32 mRequestedHeight;// = -1;
    /* Set SurfaceView's format to 565 by default to maintain backward
     * compatibility with applications assuming this format.
     */
    Int32 mRequestedFormat;// = PixelFormat.RGB_565;

    Boolean mHaveFrame;// = FALSE;
    Boolean mSurfaceCreated;// = FALSE;
    Int64 mLastLockTime;// = 0;

    Boolean mVisible;// = FALSE;
    Int32 mLeft;// = -1;
    Int32 mTop;// = -1;
    Int32 mWidth;// = -1;
    Int32 mHeight;// = -1;
    Int32 mFormat;// = -1;
    Int32 mType;// = -1;
    AutoPtr<IRect> mSurfaceFrame;// = new Rect();
    AutoPtr<ICompatibilityInfoTranslator> mTranslator;

    AutoPtr<IOnPreDrawListener> mDrawListener;

    Boolean mGlobalListenersAdded;
    Object mCallbacksLock;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif
