
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CAMERAWIDGETFRAME_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CAMERAWIDGETFRAME_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetFrame.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class CameraWidgetFrame
    : public KeyguardWidgetFrame
    , public IViewOnClickListener
{
private:
    class FixedSizeFrameLayout
        : public FrameLayout
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::FixedSizeFrameLayout")

        FixedSizeFrameLayout(
            /* [in] */ IContext* context);

    protected:
        //@Override
        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

    public:
        Int32 mWidth;
        Int32 mHeight;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable")

        MyRunnable(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable2")

        MyRunnable2(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable3")

        MyRunnable3(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyRunnable4
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable4")

        MyRunnable4(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyRunnable5
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable5")

        MyRunnable5(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyRunnable6
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyRunnable6")

        MyRunnable6(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CameraWidgetFrame* mHost;
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CameraWidgetFrame::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CameraWidgetFrame* host)
            : mHost(host)
            , mShowing(FALSE)
        {}

        //@Override
        CARAPI OnKeyguardVisibilityChanged(
            /* [in] */ Boolean showing);

    private:
        CameraWidgetFrame* mHost;
        Boolean mShowing;
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<CameraWidgetFrame>) Create(
        /* [in] */ IContext* context,
        /* [in] */ ICameraWidgetFrameCallbacks* callbacks,
        /* [in] */ KeyguardActivityLauncher* launcher);

    //@Override
    CARAPI SetOnLongClickListener(
        /* [in] */ IViewOnLongClickListener* l);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI OnActive(
        /* [in] */ Boolean isActive);

    //@Override
    CARAPI OnUserInteraction(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnScreenTurnedOff();

    //@Override
    CARAPI OnBouncerShowing(
        /* [in] */ Boolean showing);

    CARAPI SetInsets(
        /* [in] */ IRect* insets);

    CARAPI SetUseFastTransition(
        /* [in] */ Boolean useFastTransition);

protected:
    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnFocusLost();

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    CameraWidgetFrame();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICameraWidgetFrameCallbacks* callbacks,
        /* [in] */ KeyguardActivityLauncher* activityLauncher,
        /* [in] */ CameraWidgetInfo* widgetInfo,
        /* [in] */ IView* previewWidget);

    static CARAPI_(AutoPtr<IView>) GetPreviewWidget(
        /* [in] */ IContext* context,
        /* [in] */ CameraWidgetInfo* widgetInfo);

    static CARAPI_(AutoPtr<IView>) InflateWidgetView(
        /* [in] */ IContext* context,
        /* [in] */ CameraWidgetInfo* widgetInfo);

    static CARAPI_(AutoPtr<IView>) InflateGenericWidgetView(
        /* [in] */ IContext* context);

    CARAPI_(void) Render();

    CARAPI_(void) TransitionToCamera();

    CARAPI_(void) Recover();

    CARAPI_(void) RescheduleTransitionToCamera();

    CARAPI_(void) CancelTransitionToCamera();

    CARAPI_(void) OnCameraLaunched();

    CARAPI_(void) Reset();

    CARAPI_(void) EnableWindowExitAnimation(
        /* [in] */ Boolean isEnabled);

    CARAPI_(void) OnKeyguardVisibilityChanged(
        /* [in] */ Boolean showing);

    CARAPI_(void) OnSecureCameraActivityStarted();

    CARAPI_(String) InstanceId();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 WIDGET_ANIMATION_DURATION; // ms
    static const Int32 WIDGET_WAIT_DURATION; // ms
    static const Int32 RECOVERY_DELAY; // ms

    AutoPtr<IHandler> mHandler;
    AutoPtr<KeyguardActivityLauncher> mActivityLauncher;
    AutoPtr<ICameraWidgetFrameCallbacks> mCallbacks;
    AutoPtr<CameraWidgetInfo> mWidgetInfo;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IPoint> mRenderedSize;
    AutoPtr<ArrayOf<Int32> > mTmpLoc;

    Int64 mLaunchCameraStart;
    Boolean mActive;
    Boolean mTransitioning;
    Boolean mDown;

    AutoPtr<IRect> mInsets;

    AutoPtr<FixedSizeFrameLayout> mPreview;
    AutoPtr<IView> mFullscreenPreview;
    AutoPtr<IView> mFakeNavBar;
    Boolean mUseFastTransition;

    AutoPtr<IRunnable> mTransitionToCameraRunnable;

    AutoPtr<IRunnable> mTransitionToCameraEndAction;

    AutoPtr<IRunnable> mPostTransitionToCameraEndAction;

    AutoPtr<IRunnable> mRecoverRunnable;

    AutoPtr<IRunnable> mRenderRunnable;

    AutoPtr<IRunnable> mSecureCameraActivityStartedRunnable;

    AutoPtr<MyKeyguardUpdateMonitorCallback> mCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_CAMERAWIDGETFRAME_H__