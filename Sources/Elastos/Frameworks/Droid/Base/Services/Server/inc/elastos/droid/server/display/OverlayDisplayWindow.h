
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_OVERLAYDISPLAYWINDOW_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_OVERLAYDISPLAYWINDOW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/view/ScaleGestureDetector.h"

using Elastos::IO::IPrintWriter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::ISurfaceTextureListener;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::ScaleGestureDetector;
using Elastos::Droid::View::IScaleGestureDetector;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::ITextureView;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IScaleGestureDetectorOnScaleGestureListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Graphics::ISurfaceTexture;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Manages an overlay window on behalf of {@link OverlayDisplayAdapter}.
 * <p>
 * This object must only be accessed on the UI thread.
 * No locks are held by this object and locks must not be held while making called into it.
 * </p>
 */
class OverlayDisplayWindow : public Object
{
private:
    class MyDisplayListener
        : public Object
        , public IDisplayListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyDisplayListener(
            /* [in] */ OverlayDisplayWindow* owner);

        //@Override
        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        //@Override
        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

        //@Override
        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);

    private:
        OverlayDisplayWindow* mHost;
    };

    class MySurfaceTextureListener
        : public Object
        , public ISurfaceTextureListener
    {
    public:
        CAR_INTERFACE_DECL();

        MySurfaceTextureListener(
            /* [in] */ OverlayDisplayWindow* owner);

        //@Override
        CARAPI OnSurfaceTextureAvailable(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        //@Override
        CARAPI OnSurfaceTextureDestroyed(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [out] */ Boolean* res);

        //@Override
        CARAPI OnSurfaceTextureSizeChanged(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        //@Override
        CARAPI OnSurfaceTextureUpdated(
            /* [in] */ ISurfaceTexture* surfaceTexture);

    private:
        OverlayDisplayWindow* mHost;
    };

    class MyViewOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyViewOnTouchListener(
            /* [in] */ OverlayDisplayWindow* owner);

        //@Override
        CARAPI OnTouch(
            /* [in] */ IView* view,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* res);

    private:
        OverlayDisplayWindow* mHost;
    };


    class MyOnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        MyOnGestureListener(
            /* [in] */ OverlayDisplayWindow* owner);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

    private:
        OverlayDisplayWindow* mHost;
    };

    class MyOnScaleGestureListener
        : public ScaleGestureDetector::SimpleOnScaleGestureListener
    {
    public:
        MyOnScaleGestureListener(
            /* [in] */ OverlayDisplayWindow* owner);

        //@Override
        CARAPI OnScale(
            /* [in ]*/ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

    private:
        OverlayDisplayWindow* mHost;
    };

public:
    OverlayDisplayWindow(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ Boolean secure,
        /* [in] */ Int32 gravity,
        /* [in] */ IOverlayDisplayWindowListener* listener);

    CARAPI_(void) Show();

    CARAPI_(void) Dismiss();

    CARAPI_(void) Relayout();

    //@Override
    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(Boolean) UpdateDefaultDisplayInfo();

    CARAPI_(void) CreateWindow();

    CARAPI_(void) UpdateWindowParams();

    CARAPI_(void) SaveWindowParams();

    CARAPI_(void) ClearLiveState();

private:
    static const String TAG;
    static const Boolean DEBUG;// = FALSE;

    const Float INITIAL_SCALE;
    const Float MIN_SCALE;
    const Float MAX_SCALE;
    const Float WINDOW_ALPHA;

    // When TRUE, disables support for moving and resizing the overlay.
    // The window is made non-touchable, which makes it possible to
    // directly interact with the content underneath.
    const Boolean DISABLE_MOVE_AND_RESIZE;

    AutoPtr<IContext> mContext;
    const String mName;
    const Int32 mWidth;
    const Int32 mHeight;
    const Int32 mDensityDpi;
    const Boolean mSecure;
    const Int32 mGravity;

    AutoPtr<IOverlayDisplayWindowListener> mListener;
    String mTitle;

    AutoPtr<IDisplayManager> mDisplayManager;
    AutoPtr<IWindowManager> mWindowManager;

    AutoPtr<IDisplay> mDefaultDisplay;
    AutoPtr<IDisplayInfo> mDefaultDisplayInfo;

    AutoPtr<IView> mWindowContent;
    AutoPtr<IWindowManagerLayoutParams> mWindowParams;
    AutoPtr<ITextureView> mTextureView;
    AutoPtr<ITextView> mTitleTextView;

    AutoPtr<IGestureDetector> mGestureDetector;
    AutoPtr<IScaleGestureDetector> mScaleGestureDetector;

    Boolean mWindowVisible;
    Int32 mWindowX;
    Int32 mWindowY;
    Float mWindowScale;

    Float mLiveTranslationX;
    Float mLiveTranslationY;
    Float mLiveScale;

    AutoPtr<IDisplayListener> mDisplayListener;
    AutoPtr<ISurfaceTextureListener> mSurfaceTextureListener;
    AutoPtr<IViewOnTouchListener> mOnTouchListener;
    AutoPtr<IGestureDetectorOnGestureListener> mOnGestureListener;
    AutoPtr<IScaleGestureDetectorOnScaleGestureListener> mOnScaleGestureListener;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_OVERLAYDISPLAYWINDOW_H__
