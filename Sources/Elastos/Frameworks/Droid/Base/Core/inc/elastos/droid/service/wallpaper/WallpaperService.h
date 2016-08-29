
#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSERVICE_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/app/Service.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/internal/view/BaseIWindow.h>
#include <elastos/droid/internal/view/BaseSurfaceHolder.h>
#include <elastos/droid/view/InputEventReceiver.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;
using Elastos::Droid::Internal::View::BaseIWindow;
using Elastos::Droid::Internal::View::BaseSurfaceHolder;
using Elastos::Droid::Internal::View::IBaseIWindow;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Service::Wallpaper::IWallpaperCommand;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IWindowSession;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

/**
 * A wallpaper service is responsible for showing a live wallpaper behind
 * applications that would like to sit on top of it.  This service object
 * itself does very little -- its only purpose is to generate instances of
 * {@link Engine} as needed.  Implementing a wallpaper thus
 * involves subclassing from this, subclassing an Engine implementation,
 * and implementing {@link #onCreateEngine()} to return a new instance of
 * your engine.
 */
class ECO_PUBLIC WallpaperService
    : public Elastos::Droid::App::Service
    , public IWallpaperService
{
protected:
    class WallpaperCommand
        : public Object
        , public IWallpaperCommand
    {
    public:
        CAR_INTERFACE_DECL()

        WallpaperCommand();

    public:
        String mAction;
        Int32 mX;
        Int32 mY;
        Int32 mZ;
        AutoPtr<IBundle> mExtras;
        Boolean mSync;
    };

public:
    class IWallpaperEngineWrapper;

    /**
     * The actual implementation of a wallpaper.  A wallpaper service may
     * have multiple instances running (for example as a real wallpaper
     * and as a preview), each of which is represented by its own Engine
     * instance.  You must implement {@link WallpaperService#onCreateEngine()}
     * to return your concrete Engine implementation.
     */
    class Engine
        : public Object
        , public IWallpaperServiceEngine
    {
    public:
        class MReceiver
            : public BroadcastReceiver
        {
        public:
            TO_STRING_IMPL("WallpaperService::MReceiver")

            MReceiver(
                /* [in] */ Engine* host);

            //@Override
            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

        private:
            Engine* mHost;
        };

        class MSurfaceHolder
            : public BaseSurfaceHolder
        {
        public:
            MSurfaceHolder(
                /* [in] */ Engine* host);

            // @Override
            CARAPI_(Boolean) OnAllowLockCanvas();

            // @Override
            CARAPI OnRelayoutContainer();

            // @Override
            CARAPI OnUpdateSurface();

            CARAPI IsCreating(
                /* [out] */ Boolean* result);

            // @Override
            CARAPI SetFixedSize(
                /* [in] */ Int32 width,
                /* [in] */ Int32 height);

            CARAPI SetKeepScreenOn(
                /* [in] */ Boolean screenOn);

        private:
            Engine* mHost;
        };

        class WallpaperInputEventReceiver
            : public InputEventReceiver
        {
        public:
            CARAPI constructor(
                /* [in] */ IInputChannel* inputChannel,
                /* [in] */ ILooper* looper,
                /* [in] */ Engine* host);

            // @Override
            CARAPI OnInputEvent(
                /* [in] */ IInputEvent* event);

        private:
            Engine* mHost;
        };

        class MWindow
            : public BaseIWindow
        {
            friend class IWallpaperEngineWrapper;
        public:
            CARAPI constructor(
                /* [in] */ IWallpaperServiceEngine* host);

            // @Override
            CARAPI Resized(
                /* [in] */ IRect* frame,
                /* [in] */ IRect* overscanInsets,
                /* [in] */ IRect* contentInsets,
                /* [in] */ IRect* visibleInsets,
                /* [in] */ IRect* stableInsets,
                /* [in] */ Boolean reportDraw,
                /* [in] */ IConfiguration* newConfig);

            // @Override
            CARAPI Moved(
                /* [in] */ Int32 newX,
                /* [in] */ Int32 newY);

            // @Override
            CARAPI DispatchAppVisibility(
                /* [in] */ Boolean visible);

            // @Override
            CARAPI DispatchWallpaperOffsets(
                /* [in] */ Float x,
                /* [in] */ Float y,
                /* [in] */ Float xStep,
                /* [in] */ Float yStep,
                /* [in] */ Boolean sync);

            // @Override
            CARAPI DispatchWallpaperCommand(
                /* [in] */ const String& action,
                /* [in] */ Int32 x,
                /* [in] */ Int32 y,
                /* [in] */ Int32 z,
                /* [in] */ IBundle* extras,
                /* [in] */ Boolean sync);

            ///pay for someone's error,he write a ToString in Binder.car
            CARAPI ToString(
                /* [out] */ String* str)
            {
                return NOERROR;
            }
            ///pay for someone's error
        private:
            Engine* mHost;
        };

        friend class IWallpaperEngineWrapper;
        friend class MWindow;
    public:
        CAR_INTERFACE_DECL()

        Engine();

        Engine(
            /* [in] */ WallpaperService* host);

        CARAPI constructor();

        /**
         * Provides access to the surface in which this wallpaper is drawn.
         */
        CARAPI GetSurfaceHolder(
            /* [out] */ ISurfaceHolder** sh);

        /**
         * Convenience for {@link WallpaperManager#getDesiredMinimumWidth()
         * WallpaperManager.getDesiredMinimumWidth()}, returning the width
         * that the system would like this wallpaper to run in.
         */
        CARAPI GetDesiredMinimumWidth(
            /* [out] */ Int32* width);

        /**
         * Convenience for {@link WallpaperManager#getDesiredMinimumHeight()
         * WallpaperManager.getDesiredMinimumHeight()}, returning the height
         * that the system would like this wallpaper to run in.
         */
        CARAPI GetDesiredMinimumHeight(
            /* [out] */ Int32* height);

        /**
         * Return whether the wallpaper is currently visible to the user,
         * this is the last value supplied to
         * {@link #onVisibilityChanged(boolean)}.
         */
        CARAPI IsVisible(
            /* [out] */ Boolean* result);

        /**
         * Returns true if this engine is running in preview mode -- that is,
         * it is being shown to the user before they select it as the actual
         * wallpaper.
         */
        CARAPI IsPreview(
            /* [out] */ Boolean* result);

        /**
         * Control whether this wallpaper will receive raw touch events
         * from the window manager as the user interacts with the window
         * that is currently displaying the wallpaper.  By default they
         * are turned off.  If enabled, the events will be received in
         * {@link #onTouchEvent(MotionEvent)}.
         */
        CARAPI SetTouchEventsEnabled(
            /* [in] */ Boolean enabled);

        /**
         * Control whether this wallpaper will receive notifications when the wallpaper
         * has been scrolled. By default, wallpapers will receive notifications, although
         * the default static image wallpapers do not. It is a performance optimization to
         * set this to false.
         *
         * @param enabled whether the wallpaper wants to receive offset notifications
         */
        CARAPI SetOffsetNotificationsEnabled(
            /* [in] */ Boolean enabled);

        /** {@hide} */
        CARAPI SetFixedSizeAllowed(
            /* [in] */ Boolean allowed);

        /**
         * Called once to initialize the engine.  After returning, the
         * engine's surface will be created by the framework.
         */
        CARAPI OnCreate(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        /**
         * Called right before the engine is going away.  After this the
         * surface will be destroyed and this Engine object is no longer
         * valid.
         */
        CARAPI OnDestroy();

        /**
         * Called to inform you of the wallpaper becoming visible or
         * hidden.  <em>It is very important that a wallpaper only use
         * CPU while it is visible.</em>.
         */
        CARAPI OnVisibilityChanged(
            /* [in] */ Boolean visible);

        /**
         * Called with the current insets that are in effect for the wallpaper.
         * This gives you the part of the overall wallpaper surface that will
         * generally be visible to the user (ignoring position offsets applied to it).
         *
         * @param insets Insets to apply.
         */
        CARAPI OnApplyWindowInsets(
            /* [in] */ IWindowInsets* insets);

        /**
         * Called as the user performs touch-screen interaction with the
         * window that is currently showing this wallpaper.  Note that the
         * events you receive here are driven by the actual application the
         * user is interacting with, so if it is slow you will get fewer
         * move events.
         */
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event);

        /**
         * Called to inform you of the wallpaper's offsets changing
         * within its contain, corresponding to the container's
         * call to {@link WallpaperManager#setWallpaperOffsets(IBinder, float, float)
         * WallpaperManager.setWallpaperOffsets()}.
         */
        CARAPI OnOffsetsChanged(
            /* [in] */ Float xOffset,
            /* [in] */ Float yOffset,
            /* [in] */ Float xOffsetStep,
            /* [in] */ Float yOffsetStep,
            /* [in] */ Int32 xPixelOffset,
            /* [in] */ Int32 yPixelOffset);

        /**
         * Process a command that was sent to the wallpaper with
         * {@link WallpaperManager#sendWallpaperCommand}.
         * The default implementation does nothing, and always returns null
         * as the result.
         *
         * @param action The name of the command to perform.  This tells you
         * what to do and how to interpret the rest of the arguments.
         * @param x Generic integer parameter.
         * @param y Generic integer parameter.
         * @param z Generic integer parameter.
         * @param extras Any additional parameters.
         * @param resultRequested If true, the caller is requesting that
         * a result, appropriate for the command, be returned back.
         * @return If returning a result, create a Bundle and place the
         * result data in to it.  Otherwise return null.
         */
        CARAPI OnCommand(
            /* [in] */ const String& action,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ Int32 z,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean resultRequested,
            /* [out] */ IBundle** b);

        /**
         * Called when an application has changed the desired virtual size of
         * the wallpaper.
         */
        CARAPI OnDesiredSizeChanged(
            /* [in] */ Int32 desiredWidth,
            /* [in] */ Int32 desiredHeight);

        /**
         * Convenience for {@link SurfaceHolder.Callback#surfaceChanged
         * SurfaceHolder.Callback.surfaceChanged()}.
         */
        CARAPI OnSurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Convenience for {@link SurfaceHolder.Callback2#surfaceRedrawNeeded
         * SurfaceHolder.Callback.surfaceRedrawNeeded()}.
         */
        CARAPI OnSurfaceRedrawNeeded(
            /* [in] */ ISurfaceHolder* holder);

        /**
         * Convenience for {@link SurfaceHolder.Callback#surfaceCreated
         * SurfaceHolder.Callback.surfaceCreated()}.
         */
        CARAPI OnSurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        /**
         * Convenience for {@link SurfaceHolder.Callback#surfaceDestroyed
         * SurfaceHolder.Callback.surfaceDestroyed()}.
         */
        CARAPI OnSurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CARAPI DispatchPointer(
            /* [in] */ IMotionEvent* event);

    protected:
        CARAPI UpdateSurface(
            /* [in] */ Boolean forceRelayout,
            /* [in] */ Boolean forceReport,
            /* [in] */ Boolean redrawNeeded);

        CARAPI Attach(
            /* [in] */ IIWallpaperEngineWrapper* wrapper);

        CARAPI DoDesiredSizeChanged(
            /* [in] */ Int32 desiredWidth,
            /* [in] */ Int32 desiredHeight);

        CARAPI DoDisplayPaddingChanged(
            /* [in] */ IRect* padding);

        CARAPI DoVisibilityChanged(
            /* [in] */ Boolean visible);

        CARAPI ReportVisibility();

        CARAPI DoOffsetsChanged(
            /* [in] */ Boolean always);

        CARAPI DoCommand(
            /* [in] */ IWallpaperCommand* handlecmd);

        CARAPI ReportSurfaceDestroyed();

        CARAPI Detach();

    protected:
        AutoPtr<IIWallpaperEngineWrapper> mIWallpaperEngine;

        // Copies from mIWallpaperEngine.
        AutoPtr<IHandlerCaller> mCaller;
        AutoPtr<IIWallpaperConnection> mConnection;
        AutoPtr<IBinder> mWindowToken;

        Boolean mInitializing;
        Boolean mVisible;
        Boolean mScreenOn;
        Boolean mReportedVisible;
        Boolean mDestroyed;

        // Current window state.
        Boolean mCreated;
        Boolean mSurfaceCreated;
        Boolean mIsCreating;
        Boolean mDrawingAllowed;
        Boolean mOffsetsChanged;
        Boolean mFixedSizeAllowed;
        Int32 mWidth;
        Int32 mHeight;
        Int32 mFormat;
        Int32 mType;
        Int32 mCurWidth;
        Int32 mCurHeight;
        Int32 mWindowFlags;
        Int32 mWindowPrivateFlags;
        Int32 mCurWindowFlags;
        Int32 mCurWindowPrivateFlags;
        AutoPtr<ITypedValue> mOutsetBottom;
        AutoPtr<IRect> mVisibleInsets;
        AutoPtr<IRect> mWinFrame;
        AutoPtr<IRect> mOverscanInsets;
        AutoPtr<IRect> mContentInsets;
        AutoPtr<IRect> mStableInsets;
        AutoPtr<IRect> mDispatchedOverscanInsets;
        AutoPtr<IRect> mDispatchedContentInsets;
        AutoPtr<IRect> mDispatchedStableInsets;
        AutoPtr<IRect> mFinalSystemInsets;
        AutoPtr<IRect> mFinalStableInsets;
        AutoPtr<IConfiguration> mConfiguration;

        Boolean mIsEmulator;
        Boolean mIsCircularEmulator;
        Boolean mWindowIsRound;

        AutoPtr<IWindowManagerLayoutParams> mLayout;
        AutoPtr<IWindowSession> mSession;
        AutoPtr<IInputChannel> mInputChannel;

        Object mLock;

        Boolean mOffsetMessageEnqueued;
        Float mPendingXOffset;
        Float mPendingYOffset;
        Float mPendingXOffsetStep;
        Float mPendingYOffsetStep;
        Boolean mPendingSync;
        AutoPtr<IMotionEvent> mPendingMove;

        AutoPtr<BroadcastReceiver> mReceiver;
        AutoPtr<BaseSurfaceHolder> mSurfaceHolder;
        AutoPtr<WallpaperInputEventReceiver> mInputEventReceiver;
        AutoPtr<BaseIWindow> mWindow;

    private:
        WallpaperService* mHost;
    };

    class IWallpaperEngineWrapper
        : public Object
        , public IIWallpaperEngine
        , public IHandlerCallerCallback
        , public IBinder
        , public IIWallpaperEngineWrapper
    {
        friend class Engine;
    public:
        CAR_INTERFACE_DECL()

        IWallpaperEngineWrapper();

        CARAPI constructor(
            /* [in] */ IWallpaperService* context,
            /* [in] */ IIWallpaperConnection* conn,
            /* [in] */ IBinder* windowToken,
            /* [in] */ Int32 windowType,
            /* [in] */ Boolean isPreview,
            /* [in] */ Int32 reqWidth,
            /* [in] */ Int32 reqHeight,
            /* [in] */ IRect* padding);

        CARAPI SetDesiredSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetDisplayPadding(
            /* [in] */ IRect* padding);

        CARAPI SetVisibility(
            /* [in] */ Boolean visible);

        CARAPI DispatchPointer(
            /* [in] */ IMotionEvent* event);

        CARAPI DispatchWallpaperCommand(
            /* [in] */ const String& action,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ Int32 z,
            /* [in] */ IBundle* extras);

        CARAPI ReportShown();

        CARAPI Destroy();

        CARAPI ExecuteMessage(
            /* [in] */ IMessage* message);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        AutoPtr<IHandlerCaller> mCaller;

        AutoPtr<IIWallpaperConnection> mConnection;
        AutoPtr<IBinder> mWindowToken;
        Int32 mWindowType;
        Boolean mIsPreview;
        Boolean mShownReported;
        Int32 mReqWidth;
        Int32 mReqHeight;
        AutoPtr<IRect> mDisplayPadding;

        AutoPtr<Engine> mEngine;
        WallpaperService* mHost;
    };

    class IWallpaperServiceWrapper
        : public Object
        , public IIWallpaperService
        , public IBinder
        , public IIWallpaperServiceWrapper
    {
    public:
        CAR_INTERFACE_DECL()

        IWallpaperServiceWrapper();

        CARAPI constructor(
            /* [in] */ IWallpaperService* context);

        // @Override
        CARAPI Attach(
            /* [in] */ IIWallpaperConnection* conn,
            /* [in] */ IBinder* windowToken,
            /* [in] */ Int32 windowType,
            /* [in] */ Boolean isPreview,
            /* [in] */ Int32 reqWidth,
            /* [in] */ Int32 reqHeight,
            /* [in] */ IRect* padding);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        AutoPtr<IWallpaperService> mTarget;
        AutoPtr<IIWallpaperEngineWrapper> mWallpaperEngineWrapper;

    };

    friend class WallpaperService::Engine::MSurfaceHolder;
    friend class WallpaperService::Engine::MWindow;
    friend class WallpaperService::Engine;
public:
    CAR_INTERFACE_DECL()

    WallpaperService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnDestroy();

    /**
     * Implement to return the implementation of the internal accessibility
     * service interface.  Subclasses should not override.
     */
    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    /**
     * Must be implemented to return a new instance of the wallpaper's engine.
     * Note that multiple instances may be active at the same time, such as
     * when the wallpaper is currently set as the active wallpaper and the user
     * is in the wallpaper picker viewing a preview of it as well.
     */
    virtual CARAPI OnCreateEngine(
        /* [out] */ IWallpaperServiceEngine** engine) = 0;

protected:
    static const String TAG;
    static const Boolean DEBUG;

private:
    static const Int32 DO_ATTACH = 10;
    static const Int32 DO_DETACH = 20;
    static const Int32 DO_SET_DESIRED_SIZE = 30;
    static const Int32 DO_SET_DISPLAY_PADDING = 40;

    static const Int32 MSG_UPDATE_SURFACE = 10000;
    static const Int32 MSG_VISIBILITY_CHANGED = 10010;
    static const Int32 MSG_WALLPAPER_OFFSETS = 10020;
    static const Int32 MSG_WALLPAPER_COMMAND = 10025;
    static const Int32 MSG_WINDOW_RESIZED = 10030;
    static const Int32 MSG_WINDOW_MOVED = 10035;
    static const Int32 MSG_TOUCH_EVENT = 10040;

    AutoPtr<IArrayList> mActiveEngines;
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSERVICE_H__
