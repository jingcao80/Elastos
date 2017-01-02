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

#ifndef __ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Graphics.h>
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IMagnificationCallbacks;
using Elastos::Droid::View::IMagnificationSpec;
using Elastos::Droid::View::IWindowsForAccessibilityCallback;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IWindowInfo;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class CWindowManagerService;
class WindowState;
class DisplayContent;

/**
 * This class contains the accessibility related logic of the window manger.
 */
class AccessibilityController : public Object
{
private:
    /**
     * This class encapsulates the functionality related to display magnification.
     */
    class DisplayMagnifier : public Object
    {
    private:
        class MagnifiedViewport : public Object
        {
        private:
            class ViewportWindow : public Object
            {
            private:
                class AnimationController : public Handler
                {
                public:
                    TO_STRING_IMPL("ViewportWindow::MyHandler")

                    AnimationController(
                        /* [in] */ IContext* context,
                        /* [in] */ ILooper* looper,
                        /* [in] */ ViewportWindow* host);

                    CARAPI_(void) OnFrameShownStateChanged(
                        /* [in] */ Boolean shown,
                        /* [in] */ Boolean animate);

                    CARAPI HandleMessage(
                        /* [in] */ IMessage* message);

                private:
                    static const String PROPERTY_NAME_ALPHA;

                    static const Int32 MIN_ALPHA = 0;
                    static const Int32 MAX_ALPHA = 255;

                    static const Int32 MSG_FRAME_SHOWN_STATE_CHANGED = 1;

                    AutoPtr<IValueAnimator> mShowHideFrameAnimator;
                    ViewportWindow* mHost;
                };

            public:
                ViewportWindow(
                    /* [in] */ IContext* context,
                    /* [in] */ MagnifiedViewport* host);

                CARAPI_(void) SetShown(
                    /* [in] */ Boolean shown,
                    /* [in] */ Boolean animate);

                // @SuppressWarnings("unused")
                // Called reflectively from an animator.
                CARAPI_(Int32) GetAlpha();

                CARAPI_(void) SetAlpha(
                    /* [in] */ Int32 alpha);

                CARAPI_(void) SetBounds(
                    /* [in] */ IRegion* bounds);

                CARAPI_(void) UpdateSize();

                CARAPI_(void) Invalidate(
                    /* [in] */ IRect* dirtyRect);

                /** NOTE: This has to be called within a surface transaction. */
                CARAPI_(void) DrawIfNeeded();

                CARAPI_(void) ReleaseResources();

            private:
                static const String SURFACE_TITLE;

                AutoPtr<IRegion> mBounds;
                AutoPtr<IRect> mDirtyRect;
                AutoPtr<IPaint> mPaint;

                AutoPtr<ISurfaceControl> mSurfaceControl;
                AutoPtr<ISurface> mSurface;

                AutoPtr<AnimationController> mAnimationController;

                Boolean mShown;
                Int32 mAlpha;

                Boolean mInvalidated;
                MagnifiedViewport* mHost;
            };

        public:
            MagnifiedViewport(
                /* [in] */ DisplayMagnifier* host);

            CARAPI_(void) UpdateMagnificationSpecLocked(
                /* [in] */ IMagnificationSpec* spec);

            CARAPI_(void) RecomputeBoundsLocked();

            CARAPI_(void) OnRotationChangedLocked();

            CARAPI_(void) SetMagnifiedRegionBorderShownLocked(
                /* [in] */ Boolean shown,
                /* [in] */ Boolean animate);

            CARAPI_(void) GetMagnifiedFrameInContentCoordsLocked(
                /* [in] */ IRect* rect);

            CARAPI_(Boolean) IsMagnifyingLocked();

            CARAPI_(AutoPtr<IMagnificationSpec>) GetMagnificationSpecLocked();

            /** NOTE: This has to be called within a surface transaction. */
            CARAPI_(void) DrawWindowIfNeededLocked();

            CARAPI_(void) DestroyWindow();

        private:
            CARAPI_(void) PopulateWindowsOnScreenLocked(
                /* [in] */ ISparseArray* outWindows);

        private:
            static const Int32 DEFAUTLT_BORDER_WIDTH_DIP = 5;

            AutoPtr<ISparseArray> mTempWindowStates; // SparseArray<WindowState>

            AutoPtr<IRectF> mTempRectF;

            AutoPtr<IPoint> mTempPoint;

            AutoPtr<IMatrix> mTempMatrix;

            AutoPtr<IRegion> mMagnifiedBounds;
            AutoPtr<IRegion> mOldMagnifiedBounds;

            AutoPtr<IMagnificationSpec> mMagnificationSpec;

            AutoPtr<IWindowManager> mWindowManager;

            Float mBorderWidth;
            Int32 mHalfBorderWidth;
            Int32 mDrawBorderInset;

            AutoPtr<ViewportWindow> mWindow;

            Boolean mFullRedrawNeeded;

            DisplayMagnifier* mHost;
        };

        class MyHandler : public Handler
        {
        public:
            TO_STRING_IMPL("DisplayMagnifier::MyHandler")

            MyHandler(
                /* [in] */ ILooper* looper,
                /* [in] */ DisplayMagnifier* host);

            CARAPI HandleMessage(
                /* [in] */ IMessage* message);

        public:
            static const Int32 MESSAGE_NOTIFY_MAGNIFIED_BOUNDS_CHANGED = 1;
            static const Int32 MESSAGE_NOTIFY_RECTANGLE_ON_SCREEN_REQUESTED = 2;
            static const Int32 MESSAGE_NOTIFY_USER_CONTEXT_CHANGED = 3;
            static const Int32 MESSAGE_NOTIFY_ROTATION_CHANGED = 4;
            static const Int32 MESSAGE_SHOW_MAGNIFIED_REGION_BOUNDS_IF_NEEDED = 5;

            DisplayMagnifier* mHost;
        };

    public:
        DisplayMagnifier(
            /* [in] */ CWindowManagerService* windowManagerService,
            /* [in] */ IMagnificationCallbacks* callbacks);

        CARAPI_(void) SetMagnificationSpecLocked(
            /* [in] */ IMagnificationSpec* spec);

        CARAPI_(void) OnRectangleOnScreenRequestedLocked(
            /* [in] */ IRect* rectangle);

        CARAPI_(void) OnWindowLayersChangedLocked();

        CARAPI_(void) OnRotationChangedLocked(
            /* [in] */ DisplayContent* displayContent,
            /* [in] */ Int32 rotation);

        CARAPI_(void) OnAppWindowTransitionLocked(
            /* [in] */ WindowState* windowState,
            /* [in] */ Int32 transition);

        CARAPI_(void) OnWindowTransitionLocked(
            /* [in] */ WindowState* windowState,
            /* [in] */ Int32 transition);

        CARAPI_(AutoPtr<IMagnificationSpec>) GetMagnificationSpecForWindowLocked(
            /* [in] */ WindowState* windowState);

        CARAPI_(void) DestroyLocked();

        /** NOTE: This has to be called within a surface transaction. */
        CARAPI_(void) DrawMagnifiedRegionBorderIfNeededLocked();

    private:
        static const String TAG;

        static const Boolean DEBUG_WINDOW_TRANSITIONS = FALSE;
        static const Boolean DEBUG_ROTATION = FALSE;
        static const Boolean DEBUG_LAYERS = FALSE;
        static const Boolean DEBUG_RECTANGLE_REQUESTED = FALSE;
        static const Boolean DEBUG_VIEWPORT_WINDOW = FALSE;

        AutoPtr<IRect> mTempRect1;
        AutoPtr<IRect> mTempRect2;

        AutoPtr<IRegion> mTempRegion1;
        AutoPtr<IRegion> mTempRegion2;
        AutoPtr<IRegion> mTempRegion3;
        AutoPtr<IRegion> mTempRegion4;

        AutoPtr<IContext> mContext;
        CWindowManagerService* mWindowManagerService;
        AutoPtr<MagnifiedViewport> mMagnifedViewport;
        AutoPtr<IHandler> mHandler;

        AutoPtr<IMagnificationCallbacks> mCallbacks;

        Int64 mInt64AnimationDuration;
    };

    /**
     * This class encapsulates the functionality related to computing the windows
     * reported for accessibility purposes. These windows are all windows a sighted
     * user can see on the screen.
     */
    class WindowsForAccessibilityObserver : public Object
    {
    private:
        class MyHandler : public Handler
        {
        public:
            TO_STRING_IMPL("WindowsForAccessibilityObserver::MyHandler")

            MyHandler(
                /* [in] */ ILooper* looper,
                /* [in] */ WindowsForAccessibilityObserver* host);

            // @Override
            // @SuppressWarnings("unchecked")
            CARAPI HandleMessage(
                /* [in] */ IMessage* message);

        public:
            static const Int32 MESSAGE_COMPUTE_CHANGED_WINDOWS = 1;
            static const Int32 MESSAGE_NOTIFY_WINDOWS_CHANGED = 2;

        private:
            WindowsForAccessibilityObserver* mHost;
        };

    public:
        WindowsForAccessibilityObserver(
            /* [in] */ CWindowManagerService* windowManagerService,
            /* [in] */ IWindowsForAccessibilityCallback* callback);

        CARAPI_(void) ScheduleComputeChangedWindowsLocked();

        CARAPI_(void) ComputeChangedWindows();

    private:
        CARAPI_(void) ComputeWindowBoundsInScreen(
            /* [in] */ WindowState* windowState,
            /* [in] */ IRect* outBounds);

        static CARAPI_(AutoPtr<IWindowInfo>) ObtainPopulatedWindowInfo(
            /* [in] */ WindowState* windowState,
            /* [in] */ IRect* boundsInScreen);

        CARAPI_(void) CacheWindows(
            /* [in] */ IList* windows);

        CARAPI_(Boolean) WindowChangedNoLayer(
            /* [in] */ IWindowInfo* oldWindow,
            /* [in] */ IWindowInfo* newWindow);

        CARAPI_(void) ClearAndRecycleWindows(
            /* [in] */ IList* windows);

        static CARAPI_(Boolean) IsReportedWindowType(
            /* [in] */ Int32 windowType);

        CARAPI_(void) PopulateVisibleWindowsOnScreenLocked(
            /* [in] */ ISparseArray* outWindows);

    private:
        static const String TAG;

        static const Boolean DEBUG = FALSE;

        AutoPtr<ISparseArray> mTempWindowStates;

        List<AutoPtr<IWindowInfo> > mOldWindows;

        Set<AutoPtr<IBinder> > mTempBinderSet;

        AutoPtr<IRectF> mTempRectF;

        AutoPtr<IMatrix> mTempMatrix;

        AutoPtr<IPoint> mTempPoint;

        AutoPtr<IRect> mTempRect;

        AutoPtr<IRegion> mTempRegion;

        AutoPtr<IRegion> mTempRegion1;

        AutoPtr<IContext> mContext;

        CWindowManagerService* mWindowManagerService;

        AutoPtr<IHandler> mHandler;

        AutoPtr<IWindowsForAccessibilityCallback> mCallback;

        Int64 mRecurringAccessibilityEventsIntervalMillis;
    };

public:
    AccessibilityController(
        /* [in] */ CWindowManagerService* service);

    CARAPI SetMagnificationCallbacksLocked(
        /* [in] */ IMagnificationCallbacks* callbacks);

    CARAPI SetWindowsForAccessibilityCallback(
        /* [in] */ IWindowsForAccessibilityCallback* callback);

    CARAPI_(void) SetMagnificationSpecLocked(
        /* [in] */ IMagnificationSpec* spec);

    CARAPI_(void) OnRectangleOnScreenRequestedLocked(
        /* [in] */ IRect* rectangle);

    CARAPI_(void) OnWindowLayersChangedLocked();

    CARAPI_(void) OnRotationChangedLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Int32 rotation);

    CARAPI_(void) OnAppWindowTransitionLocked(
        /* [in] */ WindowState* windowState,
        /* [in] */ Int32 transition);

    CARAPI_(void) OnWindowTransitionLocked(
        /* [in] */ WindowState* windowState,
        /* [in] */ Int32 transition);

    CARAPI_(void) OnWindowFocusChangedLocked();

    CARAPI_(void) OnSomeWindowResizedOrMovedLocked();

    /** NOTE: This has to be called within a surface transaction. */
    CARAPI_(void) DrawMagnifiedRegionBorderIfNeededLocked();

    CARAPI_(AutoPtr<IMagnificationSpec>) GetMagnificationSpecForWindowLocked(
        /* [in] */ WindowState* windowState);

    CARAPI_(Boolean) HasCallbacksLocked();

private:
    static CARAPI_(void) PopulateTransformationMatrixLocked(
        /* [in] */ WindowState* windowState,
        /* [in] */ IMatrix* outMatrix);

private:
    static const String TAG;

    CWindowManagerService* mWindowManagerService;

    static AutoPtr<ArrayOf<Float> > sTempFloats;

    AutoPtr<DisplayMagnifier> mDisplayMagnifier;

    AutoPtr<WindowsForAccessibilityObserver> mWindowsForAccessibilityObserver;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__
