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

#ifndef __ELASTOS_DROID_SERVER_WM_CWINDOWMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_WM_CWINDOWMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "_Elastos_Droid_Server_Wm_CWindowManagerService.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/InputEventReceiver.h"
#include "elastos/droid/server/AttributeCache.h"
#include "elastos/droid/server/wm/WindowState.h"
#include "elastos/droid/server/wm/AppWindowToken.h"
#include "elastos/droid/server/wm/FakeWindowImpl.h"
#include "elastos/droid/server/wm/WindowToken.h"
#include "elastos/droid/server/wm/Watermark.h"
#include "elastos/droid/server/wm/StrictModeFlash.h"
#include "elastos/droid/server/wm/CSession.h"
#include "elastos/droid/server/wm/AppWindowAnimator.h"
#include "elastos/droid/server/wm/WindowStateAnimator.h"
#include "elastos/droid/server/wm/DisplaySettings.h"
#include "elastos/droid/server/wm/CircularDisplayMask.h"
#include "elastos/droid/server/wm/EmulatorDisplayOverlay.h"
#include "elastos/droid/server/wm/AppTransition.h"
#include "elastos/droid/server/wm/WindowBinder.h"
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/server/wm/TaskGroup.h"
#include "elastos/droid/server/wm/FocusedStackFrame.h"
#include "elastos/droid/server/wm/AccessibilityController.h"
#include "elastos/droid/server/wm/PointerEventDispatcher.h"
#include "elastos/droid/server/wm/KeyguardDisableHandler.h"
#include "elastos/droid/server/wm/InputMonitor.h"
#include "elastos/droid/server/wm/ViewServer.h"
#include "elastos/droid/server/wm/DragState.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/server/display/CDisplayManagerService.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IAppOpsManagerOnOpChangedInternalListener;
using Elastos::Droid::App::IAppOpsManagerOnOpChangedListener;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Hardware::Display::IDisplayManagerInternal;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Input::IIInputManager;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Internal::View::IIInputMethodManager;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Server::IWatchdogMonitor;
using Elastos::Droid::Server::Display::CDisplayManagerService;
using Elastos::Droid::Server::Input::CInputManagerService;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IWindowManagerInternal;
using Elastos::Droid::View::IIWindowId;
using Elastos::Droid::View::IWindowSession;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IIInputFilter;
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IWindowInfo;
using Elastos::Droid::View::IOnKeyguardExitResult;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IFakeWindow;
using Elastos::Droid::View::IInputEventReceiverFactory;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IApplicationToken;
using Elastos::Droid::View::IWindowContentFrameStats;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManagerPolicyWindowManagerFuncs;
using Elastos::Droid::View::IMagnificationSpec;
using Elastos::Droid::View::IIWindowSessionCallback;;
using Elastos::Droid::View::IWindowsForAccessibilityCallback;
using Elastos::Droid::View::IMagnificationCallbacks;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISynchronize;
using Elastos::Core::IInteger32;
using Elastos::Net::ISocket;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;

typedef IArrayList WindowList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class FakeWindowImpl;
class DisplayContent;
class WindowAnimator;

class DragInputEventReceiver : public InputEventReceiver
{
public:
    CARAPI constructor(
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ ILooper* looper,
        /* [in] */ CWindowManagerService* host);

    CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event);

private:
    CWindowManagerService* mHost;
};


CarClass(CWindowManagerService)
    , public Object
    , public IIWindowManager
    , public IWindowManagerPolicyWindowManagerFuncs
    , public IWatchdogMonitor
    , public IBinder
{
public:
    class RotationWatcher : public Object
    {
    public:
        RotationWatcher(
            /* [in] */ IRotationWatcher* w,
            /* [in] */ IProxyDeathRecipient* d)
            : mWatcher(w)
            , mDeathRecipient(d)
        {}

    public:
        AutoPtr<IRotationWatcher> mWatcher;
        AutoPtr<IProxyDeathRecipient> mDeathRecipient;

    };

    /** Pulled out of performLayoutAndPlaceSurfacesLockedInner in order to refactor into multiple
     * methods. */
    class LayoutFields : public Object
    {
    public:
        LayoutFields();

    public:
        static const Int32 SET_UPDATE_ROTATION;
        static const Int32 SET_WALLPAPER_MAY_CHANGE;
        static const Int32 SET_FORCE_HIDING_CHANGED;
        static const Int32 SET_ORIENTATION_CHANGE_COMPLETE;
        static const Int32 SET_TURN_ON_SCREEN;
        static const Int32 SET_WALLPAPER_ACTION_PENDING;

        Boolean mWallpaperForceHidingChanged;
        Boolean mWallpaperMayChange;
        Boolean mOrientationChangeComplete;
        AutoPtr<IObject> mLastWindowFreezeSource;
        Boolean mWallpaperActionPending;

        // Set to true when the display contains content to show the user.
        // When false, the display manager may choose to mirror or blank the display.
        Boolean mDisplayHasContent;

        // Only set while traversing the default display based on its content.
        // Affects the behavior of mirroring on secondary displays.
        Boolean mObscureApplicationContentOnSecondaryDisplays;

        Float mPreferredRefreshRate;

    private:
        AutoPtr<IWindowSession> mHoldScreen;
        Boolean mObscured;
        Boolean mSyswin;
        Float mScreenBrightness;
        Float mButtonBrightness;
        Int64 mUserActivityTimeout;
        Boolean mUpdateRotation;

        friend class CWindowManagerService;
    };

    class LocalBroadcastReceiver : public BroadcastReceiver
    {
    public:
        LocalBroadcastReceiver(
            /* [in] */ CWindowManagerService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CWindowManagerService::LocalBroadcastReceiver: ")
    private:
        CWindowManagerService* mHost;
    };

    class SecurelyOnKeyguardExitResult
        : public Object
        , public IOnKeyguardExitResult
    {
    public:
        SecurelyOnKeyguardExitResult(
            /* [in] */ IOnKeyguardExitResult* callback)
            : mCallback(callback)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnKeyguardExitResult(
            /* [in] */ Boolean success);

    private:
        AutoPtr<IOnKeyguardExitResult> mCallback;
    };

    // -------------------------------------------------------------
    // Async Handler
    // -------------------------------------------------------------
    class H : public Handler
    {
    public:
        TO_STRING_IMPL("CWindowManagerService::H")

        H(
            /* [in] */ CWindowManagerService * host);

        ECode HandleMessage(
            /* [in] */ IMessage* msg);

        static String MsgToString(
            /* [in] */ Int32 msg);
    public:
        static const Int32 REPORT_FOCUS_CHANGE; // = 2;
        static const Int32 REPORT_LOSING_FOCUS; // = 3;
        static const Int32 DO_TRAVERSAL; // = 4;
        static const Int32 ADD_STARTING; // = 5;
        static const Int32 REMOVE_STARTING; // = 6;
        static const Int32 FINISHED_STARTING; // = 7;
        static const Int32 REPORT_APPLICATION_TOKEN_WINDOWS; // = 8;
        static const Int32 REPORT_APPLICATION_TOKEN_DRAWN; // = 9;
        static const Int32 WINDOW_FREEZE_TIMEOUT; // = 11;

        static const Int32 APP_TRANSITION_TIMEOUT; // = 13;
        static const Int32 PERSIST_ANIMATION_SCALE; // = 14;
        static const Int32 FORCE_GC; // = 15;
        static const Int32 ENABLE_SCREEN; // = 16;
        static const Int32 APP_FREEZE_TIMEOUT; // = 17;
        static const Int32 SEND_NEW_CONFIGURATION; // = 18;
        static const Int32 REPORT_WINDOWS_CHANGE; // = 19;
        static const Int32 DRAG_START_TIMEOUT; // = 20;
        static const Int32 DRAG_END_TIMEOUT; // = 21;
        static const Int32 REPORT_HARD_KEYBOARD_STATUS_CHANGE; // = 22;
        static const Int32 BOOT_TIMEOUT; // = 23;
        static const Int32 WAITING_FOR_DRAWN_TIMEOUT; // = 24;
        static const Int32 SHOW_STRICT_MODE_VIOLATION; // = 25;
        static const Int32 DO_ANIMATION_CALLBACK; // = 26;

        static const Int32 DO_DISPLAY_ADDED; // = 27;
        static const Int32 DO_DISPLAY_REMOVED; // = 28;
        static const Int32 DO_DISPLAY_CHANGED; // = 29;

        static const Int32 CLIENT_FREEZE_TIMEOUT; // = 30;
        static const Int32 TAP_OUTSIDE_STACK; // = 31;
        static const Int32 NOTIFY_ACTIVITY_DRAWN; // = 32;

        static const Int32 ALL_WINDOWS_DRAWN; // = 33;

        static const Int32 NEW_ANIMATOR_SCALE; // = 34;

        static const Int32 SHOW_CIRCULAR_DISPLAY_MASK; // = 35;
        static const Int32 SHOW_EMULATOR_DISPLAY_OVERLAY; // = 36;

        static const Int32 CHECK_IF_BOOT_ANIMATION_FINISHED; // = 37;

    private:
        CWindowManagerService * mHost;
    };

private:
    class SettingsObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CWindowManagerService::SettingsObserver")

        SettingsObserver(
            /* [in] */ CWindowManagerService* host);

        CARAPI constructor();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CWindowManagerService* mHost;
    };

    class WindowManagerServiceCreator : public Runnable
    {
    public:
        WindowManagerServiceCreator(
            /* [in] */ IContext* context,
            /* [in] */ IIInputManager* im,
            /* [in] */ Boolean haveInputMethods,
            /* [in] */ Boolean showBootMsgs,
            /* [in] */ Boolean onlyCore);

        CARAPI Run();

    public:
        AutoPtr<IContext> mContext;
        AutoPtr<IIInputManager> mIm;
        Boolean mHaveInputMethods;
        Boolean mShowBootMsgs;
        Boolean mOnlyCore;
        CWindowManagerService* mInstance;
    };

    class MyLowPowerModeListener
        : public Object
        , public ILowPowerModeListener
    {
    public:
        MyLowPowerModeListener(
            /* [in] */ CWindowManagerService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnLowPowerModeChanged(
            /* [in] */ Boolean enabled);

    private:
        CWindowManagerService* mHost;
    };

    class MyAppOpsManagerOnOpChangedListener
        : public Object
        , public IAppOpsManagerOnOpChangedListener
        , public IAppOpsManagerOnOpChangedInternalListener
    {
    public:
        MyAppOpsManagerOnOpChangedListener(
            /* [in] */ CWindowManagerService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnOpChanged(
            /* [in] */ Int32 op,
            /* [in] */ const String& packageName);

        CARAPI OnOpChanged(
            /* [in] */ const String& op,
            /* [in] */ const String& packageName);

    private:
        CWindowManagerService* mHost;
    };

    class PolicyInitializer : public Runnable
    {
    public:
        PolicyInitializer(
            /* [in] */ CWindowManagerService* wmService);

        CARAPI Run();

    private:
        CWindowManagerService* mHost;
    };

    class RotationWatcherDeathRecipint
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        RotationWatcherDeathRecipint(
            /* [in] */ CWindowManagerService* owner,
            /* [in] */ IBinder* watcherBinder);

        //@Override
        CARAPI ProxyDied();

    private:
        CWindowManagerService* mOwner;
        AutoPtr<IBinder> mWatcherBinder;
    };

    class LocalService
        : public Object
        , public IWindowManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ CWindowManagerService* host);

        CARAPI RequestTraversalFromDisplayManager();

        CARAPI SetMagnificationSpec(
            /* [in] */ IMagnificationSpec* spec);

        CARAPI GetCompatibleMagnificationSpecForWindow(
            /* [in] */ IBinder* windowToken,
            /* [out] */ IMagnificationSpec** spec);

        CARAPI SetMagnificationCallbacks(
            /* [in] */ IMagnificationCallbacks* callbacks);

        CARAPI SetWindowsForAccessibilityCallback(
            /* [in] */ IWindowsForAccessibilityCallback* callback);

        CARAPI SetInputFilter(
            /* [in] */ IIInputFilter* filter);

        CARAPI GetFocusedWindowToken(
            /* [out] */ IBinder** binder);

        CARAPI IsKeyguardLocked(
            /* [out] */ Boolean* isLocked);

        CARAPI ShowGlobalActions();

        CARAPI GetWindowFrame(
            /* [in] */ IBinder* token,
            /* [in] */ IRect* outBounds);

        CARAPI WaitForAllWindowsDrawn(
            /* [in] */ IRunnable* callback,
            /* [in] */ Int64 timeout);

    private:
        CWindowManagerService* mHost;
    };

public:
    CWindowManagerService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static AutoPtr<CWindowManagerService> Main(
        /* [in] */ IContext* context,
        /* [in] */ IIInputManager* im,
        /* [in] */ Boolean haveInputMethods,
        /* [in] */ Boolean showBootMsgs,
        /* [in] */ Boolean onlyCore);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIInputManager* inputManager,
        /* [in] */ Boolean haveInputMethods,
        /* [in] */ Boolean showBootMsgs,
        /* [in] */ Boolean onlyCore);

    CARAPI_(AutoPtr<InputMonitor>) GetInputMonitor();

    /**
     * Return the list of Windows from the passed token on the given Display.
     * @param token The token with all the windows.
     * @param displayContent The display we are interested in.
     * @return List of windows from token that are on displayContent.
     */
    CARAPI_(AutoPtr<WindowList>) GetTokenWindowsOnDisplay(
        /* [in] */ WindowToken* token,
        /* [in] */ DisplayContent* displayContent);

    /** TODO(cmautner): Is this the same as {@link WindowState#canReceiveKeys()} */
    static CARAPI_(Boolean) CanBeImeTarget(
        /* [in] */ WindowState* w);

    /**
     * Dig through the WindowStates and find the one that the Input Method will target.
     * @param willMove
     * @return The index+1 in mWindows of the discovered target.
     */
    CARAPI_(Int32) FindDesiredInputMethodWindowIndexLocked(
        /* [in] */ Boolean willMove);

    CARAPI_(void) AddInputMethodWindowToListLocked(
        /* [in] */ WindowState* win);

    CARAPI_(void) SetInputMethodAnimLayerAdjustment(
        /* [in] */ Int32 adj);

    CARAPI_(void) LogWindowList(
        /* [in] */ WindowList* windows,
        /* [in] */ const String& prefix);

    CARAPI_(void) MoveInputMethodDialogsLocked(
        /* [in] */ Int32 pos);

    CARAPI_(Boolean) MoveInputMethodWindowsIfNeededLocked(
        /* [in] */ Boolean needAssignLayers);

    CARAPI_(Boolean) IsWallpaperVisible(
        /* [in] */ WindowState* wallpaperTarget);

    CARAPI_(Int32) AdjustWallpaperWindowsLocked();

    CARAPI_(void) SetWallpaperAnimLayerAdjustmentLocked(
        /* [in] */ Int32 adj);

    CARAPI_(Boolean) UpdateWallpaperOffsetLocked(
        /* [in] */ WindowState* wallpaperWin,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh,
        /* [in] */ Boolean sync);

    CARAPI_(void) WallpaperOffsetsComplete(
        /* [in] */ IBinder* window);

    CARAPI_(void) UpdateWallpaperOffsetLocked(
        /* [in] */ WindowState* changingTarget,
        /* [in] */ Boolean sync);

    /**
     * Check wallpaper for visiblity change and notify window if so.
     * @param wallpaper The wallpaper to test and notify.
     * @param visible Current visibility.
     */
    CARAPI_(void) DispatchWallpaperVisibility(
        /* [in] */ WindowState* wallpaper,
        /* [in] */ Boolean visible);

    CARAPI_(void) UpdateWallpaperVisibilityLocked();

    CARAPI_(Int32) AddWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 displayId,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IInputChannel* inInputChannel,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IInputChannel** outInputChannel);

    /**
     * Returns whether screen capture is disabled for all windows of a specific user.
     */
    CARAPI_(Boolean) IsScreenCaptureDisabledLocked(
        /* [in] */ Int32 userId);

    /**
     * Set mScreenCaptureDisabled for specific user
     */
    // @Override
    CARAPI SetScreenCaptureDisabled(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean disabled);

    CARAPI_(void) RemoveWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client);

    CARAPI_(void) RemoveWindowLocked(
        /* [in] */ CSession* session,
        /* [in] */ WindowState* win);

    CARAPI_(void) RemoveWindowInnerLocked(
        /* [in] */ CSession* session,
        /* [in] */ WindowState* win);

    CARAPI_(void) UpdateAppOpsState();

    // static CARAPI_(void) LogSurface(
    //     /* [in] */ WindowState* w,
    //     /* [in] */ const String& msg,
    //     /* [in] */ RuntimeException where);

    // static void logSurface(SurfaceControl s, String title, String msg, RuntimeException where);

    CARAPI_(void) SetTransparentRegionWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ IRegion* region);

    CARAPI_(void) SetInsetsWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ Int32 touchableInsets,
        /* [in] */ IRect* contentInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRegion* touchableRegion);

    CARAPI_(void) GetWindowDisplayFrame(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ IRect* outDisplayFrame);

    CARAPI_(void) SetWindowWallpaperPositionLocked(
        /* [in] */ WindowState* window,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float xStep,
        /* [in] */ Float yStep);

    CARAPI_(void) WallpaperCommandComplete(
        /* [in] */ IBinder* window,
        /* [in] */ IBundle* result);

    CARAPI_(void) SetWindowWallpaperDisplayOffsetLocked(
        /* [in] */ WindowState* window,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(AutoPtr<IBundle>) SendWindowWallpaperCommandLocked(
        /* [in] */ WindowState* window,
        /* [in] */ const String& action,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean sync);

    CARAPI_(void) SetUniverseTransformLocked(
        /* [in] */ WindowState* window,
        /* [in] */ Float alpha,
        /* [in] */ Float offx,
        /* [in] */ Float offy,
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    CARAPI_(void) OnRectangleOnScreenRequested(
        /* [in] */ IBinder* token,
        /* [in] */ IRect* rectangle);

    CARAPI_(AutoPtr<IIWindowId>) GetWindowId(
        /* [in] */ IBinder* token);

    CARAPI_(Int32) RelayoutWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 requestedWidth,
        /* [in] */ Int32 requestedHeight,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 flags,
        /* [in] */ IRect* inFrame,
        /* [in] */ IRect* inOverscanInsets,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IRect* inVisibleInsets,
        /* [in] */ IRect* inStableInsets,
        /* [in] */ IConfiguration* inConfig,
        /* [in] */ ISurface* inSurface,
        /* [out] */ IRect** outFrame,
        /* [out] */ IRect** outOverscanInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IRect** outVisibleInsets,
        /* [out] */ IRect** outStableInsets,
        /* [out] */ IConfiguration** outConfig,
        /* [out] */ ISurface** outSurface);

    CARAPI_(void) PerformDeferredDestroyWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client);

    CARAPI_(Boolean) OutOfMemoryWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client);

    CARAPI_(void) FinishDrawingWindow(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client);

    CARAPI_(void) ValidateAppTokens(
        /* [in] */ Int32 stackId,
        /* [in] */ IArrayList* tasks);

    CARAPI_(void) ValidateStackOrder(
        /* [in] */ ArrayOf<IInteger32*>* remoteStackIds);

    CARAPI_(Boolean) CheckCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& func);

    CARAPI_(Boolean) OkToDisplay();

    CARAPI_(AutoPtr<AppWindowToken>) FindAppWindowToken(
        /* [in] */ IBinder* token);

    CARAPI AddWindowToken(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type);

    CARAPI RemoveWindowToken(
        /* [in] */ IBinder* token);

    CARAPI AddAppToken(
        /* [in] */ Int32 addPos,
        /* [in] */ IApplicationToken* token,
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 requestedOrientation,
        /* [in] */ Boolean fullscreen,
        /* [in] */ Boolean showWhenLocked,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean voiceInteraction,
        /* [in] */ Boolean launchTaskBehind);

    CARAPI SetAppGroupId(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 groupId);

    CARAPI_(Int32) GetOrientationFromWindowsLocked();

    CARAPI_(Int32) GetOrientationFromAppTokensLocked();

    // Re-evaluate the current orientation from the caller's state.
    // If there is a change, the new Configuration is returned and the
    // caller must call setNewConfiguration() sometime later.
    CARAPI UpdateOrientationFromAppTokens(
        /* [in] */ IConfiguration* currentConfig,
        /* [in] */ IBinder* freezeThisOneIfNeeded,
        /* [out] */ IConfiguration** config);

    /*
     * Determine the new desired orientation of the display, returning
     * a non-null new Configuration if it has changed from the current
     * orientation.  IF TRUE IS RETURNED SOMEONE MUST CALL
     * setNewConfiguration() TO TELL THE WINDOW MANAGER IT CAN UNFREEZE THE
     * SCREEN.  This will typically be done for you if you call
     * sendNewConfiguration().
     *
     * The orientation is computed from non-application windows first. If none of
     * the non-application windows specify orientation, the orientation is computed from
     * application tokens.
     * @see android.view.IWindowManager#updateOrientationFromAppTokens(
     * android.os.IBinder)
     */
    CARAPI_(Boolean) UpdateOrientationFromAppTokensLocked(
        /* [in] */ Boolean inTransaction);

    CARAPI SetNewConfiguration(
        /* [in] */ IConfiguration* config);

    CARAPI SetAppOrientation(
        /* [in] */ IApplicationToken* token,
        /* [in] */ Int32 requestedOrientation);

    CARAPI GetAppOrientation(
        /* [in] */ IApplicationToken* token,
        /* [out] */ Int32* orientation);

    /** Call while in a Surface transaction. */
    CARAPI_(void) SetFocusedStackLayer();

    CARAPI_(void) SetFocusedStackFrame();

    CARAPI SetFocusedApp(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean moveFocusNow);

    CARAPI PrepareAppTransition(
        /* [in] */ Int32 transit,
        /* [in] */ Boolean alwaysKeepCurrent);

    CARAPI GetPendingAppTransition(
        /* [out] */ Int32* transit);

    CARAPI OverridePendingAppTransition(
        /* [in] */ const String& capsuleName,
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim,
        /* [in] */ IIRemoteCallback* startedCallback);

    CARAPI OverridePendingAppTransitionScaleUp(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 startWidth,
        /* [in] */ Int32 startHeight);

    CARAPI OverridePendingAppTransitionThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI OverridePendingAppTransitionAspectScaledThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI ExecuteAppTransition();

    CARAPI SetAppStartingWindow(
        /* [in] */ IBinder* token,
        /* [in] */ const String& cap,
        /* [in] */ Int32 theme,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon,
        /* [in] */ Int32 logo,
        /* [in] */ Int32 windowFlags,
        /* [in] */ IBinder* transferFrom,
        /* [in] */ Boolean createIfNeeded);

    CARAPI_(void) RemoveAppStartingWindow(
        /* [in] */ IBinder* token);

    CARAPI SetAppWillBeHidden(
        /* [in] */ IBinder* token);

    CARAPI_(void) SetAppFullscreen(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean toOpaque);

    CARAPI_(void) SetWindowOpaque(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean isOpaque);

    CARAPI_(void) SetWindowOpaqueLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean isOpaque);

    CARAPI_(Boolean) SetTokenVisibilityLocked(
        /* [in] */ AppWindowToken* wtoken,
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Boolean visible,
        /* [in] */ Int32 transit,
        /* [in] */ Boolean performLayout,
        /* [in] */ Boolean isVoiceInteraction);

    CARAPI SetAppVisibility(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean visible);

    CARAPI_(void) UnsetAppFreezingScreenLocked(
        /* [in] */ AppWindowToken* wtoken,
        /* [in] */ Boolean unfreezeSurfaceNow,
        /* [in] */ Boolean force);

    CARAPI StartAppFreezingScreen(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 configChanges);

    CARAPI StopAppFreezingScreen(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean force);

    CARAPI_(void) RemoveAppFromTaskLocked(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI RemoveAppToken(
        /* [in] */ IBinder* token);

    CARAPI_(void) ScheduleRemoveStartingWindowLocked(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI_(void) DumpAppTokensLocked();

    CARAPI_(void) DumpWindowsLocked();

    CARAPI_(void) TmpRemoveTaskWindowsLocked(
        /* [in] */ Task* task);

    CARAPI_(void) MoveStackWindowsLocked(
        /* [in] */ DisplayContent* displayContent);

    CARAPI_(void) MoveTaskToTop(
        /* [in] */ Int32 taskId);

    CARAPI_(void) MoveTaskToBottom(
        /* [in] */ Int32 taskId);

    /**
     * Create a new TaskStack and place it on a DisplayContent.
     * @param stackId The unique identifier of the new stack.
     * @param displayId The unique identifier of the DisplayContent.
     */
    CARAPI_(void) AttachStack(
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 displayId);

    CARAPI_(void) DetachStackLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ TaskStack* stack);

    CARAPI_(void) DetachStack(
        /* [in] */ Int32 stackId);

    CARAPI_(void) RemoveStack(
        /* [in] */ Int32 stackId);

    CARAPI_(void) RemoveTaskLocked(
        /* [in] */ Task* task);

    CARAPI_(void) RemoveTask(
        /* [in] */ Int32 taskId);

    CARAPI_(void) AddTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 stackId,
        /* [in] */ Boolean toTop);

    CARAPI ResizeStack(
        /* [in] */ Int32 stackId,
        /* [in] */ IRect* bounds);

    CARAPI_(void) GetStackBounds(
        /* [in] */ Int32 stackId,
        /* [in] */ IRect* bounds);

    CARAPI StartFreezingScreen(
        /* [in] */ Int32 exitAnim,
        /* [in] */ Int32 enterAnim);

    CARAPI StopFreezingScreen();

    // these require DISABLE_KEYGUARD permission
    CARAPI DisableKeyguard(
        /* [in] */ IBinder* token,
        /* [in] */ const String& tag);

    CARAPI ReenableKeyguard(
        /* [in] */ IBinder* token);

    /**
     * @see android.app.KeyguardManager#exitKeyguardSecurely
     */
    CARAPI ExitKeyguardSecurely(
        /* [in] */ IOnKeyguardExitResult* keyguardCallback);

    CARAPI InKeyguardRestrictedInputMode(
        /* [out] */ Boolean* result);

    CARAPI IsKeyguardLocked(
        /* [out] */ Boolean* isSecure);

    CARAPI IsKeyguardSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI DismissKeyguard();

    CARAPI KeyguardGoingAway(
        /* [in] */ Boolean disableWindowAnimations,
        /* [in] */ Boolean keyguardGoingToNotificationShade,
        /* [in] */ Boolean keyguardShowingMedia);

    /**
    * Get the current x offset for the wallpaper
    */
    CARAPI GetLastWallpaperX(
        /* [out] */ Int32* x);

    /**
    * Get the current y offset for the wallpaper
    */
    CARAPI GetLastWallpaperY(
        /* [out] */ Int32* y);

    CARAPI HasPermanentMenuKey(
        /* [out] */ Boolean* result);

    /**
    * Device needs a software navigation bar (because it has no hardware keys).
    */
    CARAPI NeedsNavigationBar(
    /* [out] */ Boolean* result);

    CARAPI_(void) KeyguardWaitingForActivityDrawn();

    CARAPI_(void) NotifyActivityDrawnForKeyguard();

    CARAPI_(void) ShowGlobalActions();

    CARAPI CloseSystemDialogs(
        /* [in] */ const String& reason);

    static CARAPI_(Float) FixScale(
        /* [in] */ Float scale);

    CARAPI SetAnimationScale(
        /* [in] */ Int32 which,
        /* [in] */ Float scale);

    CARAPI SetAnimationScales(
        /* [in] */ ArrayOf<Float>* scales);

    CARAPI_(Float) GetWindowAnimationScaleLocked();

    CARAPI_(Float) GetTransitionAnimationScaleLocked();

    // These can only be called with the SET_ANIMATON_SCALE permission.
    CARAPI GetAnimationScale(
        /* [in] */ Int32 which,
        /* [out] */ Float* scale);

    CARAPI GetAnimationScales(
        /* [out, callee] */ ArrayOf<Float>** scales);

    CARAPI GetCurrentAnimatorScale(
        /* [out] */ Float* scale);

    CARAPI_(void) DispatchNewAnimatorScaleLocked(
        /* [in] */ CSession* session);

    CARAPI RegisterPointerEventListener(
        /* [in] */ IPointerEventListener* listener);

    CARAPI UnregisterPointerEventListener(
        /* [in] */ IPointerEventListener* listener);

    CARAPI AddSystemUIVisibilityFlag(
        /* [in] */ Int32 flags);
    /**
     * Returns a code that describes the current state of the lid switch.
     */
    // Called by window manager policy.  Not exposed externally.
    CARAPI GetLidState(
        /* [out] */ Int32* lidState);

    // Called by window manager policy.  Not exposed externally.
    CARAPI GetCameraLensCoverState(
        /* [out] */ Int32* state);

    /**
     * Switch the keyboard layout for the given device.
     * Direction should be +1 or -1 to go to the next or previous keyboard layout.
     */
    CARAPI SwitchKeyboardLayout(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 direction);

    // Called by window manager policy.  Not exposed externally.
    CARAPI Shutdown(
        /* [in] */ Boolean confirm);

    // Called by window manager policy.  Not exposed externally.
    CARAPI RebootSafeMode(
        /* [in] */ Boolean confirm);

    CARAPI_(void) SetCurrentProfileIds(
        /* [in] */ ArrayOf<Int32>* currentProfileIds);

    CARAPI_(void) SetCurrentUser(
        /* [in] */ Int32 newUserId,
        /* [in] */ ArrayOf<Int32>* currentProfileIds);

    /* Called by WindowState */
    CARAPI_(Boolean) IsCurrentProfileLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) EnableScreenAfterBoot();

    CARAPI EnableScreenIfNeeded();

    CARAPI_(void) EnableScreenIfNeededLocked();

    CARAPI_(void) PerformBootTimeout();

    CARAPI_(void) PerformEnableScreen();

    CARAPI_(void) ShowBootMessage(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean always);

    CARAPI_(void) HideBootMessagesLocked();

    CARAPI SetInTouchMode(
        /* [in] */ Boolean mode);

    CARAPI_(void) ShowCircularDisplayMaskIfNeeded();

    CARAPI_(void) ShowEmulatorDisplayOverlayIfNeeded();

    CARAPI_(void) ShowCircularMask();

    CARAPI_(void) ShowEmulatorDisplayOverlay();

    CARAPI_(void) HandleTapOutsideStack(
        /* [in] */ DisplayContent* dc,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // For StrictMode flashing a red border on violations from the UI
    // thread.  The uid/pid is implicit from the Binder call, and the Window
    // Manager uses that to determine whether or not the red border should
    // actually be shown.  (it will be ignored that pid doesn't have windows
    // on screen)
    // TODO: more accounting of which pid(s) turned it on, keep count,
    // only allow disables from pids which have count on, etc.
    CARAPI ShowStrictModeViolation(
        /* [in] */ Boolean on);

    // Proxy to set the system property for whether the flashing
    // should be enabled.  The 'enabled' value is null or blank for
    // the system default (differs per build variant) or any valid
    // boolean string as parsed by SystemProperties.getBoolean().
    CARAPI SetStrictModeVisualIndicatorPreference(
        /* [in] */ const String& value);

    /**
     * Takes a snapshot of the screen.  In landscape mode this grabs the whole screen.
     * In portrait mode, it grabs the upper region of the screen based on the vertical dimension
     * of the target image.
     *
     * @param displayId the Display to take a screenshot of.
     * @param width the width of the target bitmap or -1 for a full screenshot
     * @param height the height of the target bitmap or -1 for a full screenshot
     * @param force565 if true the returned bitmap will be RGB_565, otherwise it
     *                 will be the same config as the surface
     */
    // @Override
    CARAPI ScreenshotApplications(
        /* [in] */ IBinder* appToken,
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight,
        /* [in] */ Boolean force565,
        /* [out] */ IBitmap** bitmap);

    /**
     * Freeze rotation changes.  (Enable "rotation lock".)
     * Persists across reboots.
     * @param rotation The desired rotation to freeze to, or -1 to use the
     * current rotation.
     */
    CARAPI FreezeRotation(
        /* [in] */ Int32 rotation);

    /**
     * Thaw rotation changes.  (Disable "rotation lock".)
     * Persists across reboots.
     */
    CARAPI ThawRotation();

    /**
     * Recalculate the current rotation.
     *
     * Called by the window manager policy whenever the state of the system changes
     * such that the current rotation might need to be updated, such as when the
     * device is docked or rotated into a new posture.
     */
    CARAPI UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration,
        /* [in] */ Boolean forceRelayout);

    /**
     * Temporarily pauses rotation changes until resumed.
     *
     * This can be used to prevent rotation changes from occurring while the user is
     * performing certain operations, such as drag and drop.
     *
     * This call nests and must be matched by an equal number of calls to
     * {@link #resumeRotationLocked}.
     */
    CARAPI_(void) PauseRotationLocked();

    /**
     * Resumes normal rotation changes after being paused.
     */
    CARAPI_(void) ResumeRotationLocked();

    CARAPI_(void) UpdateRotationUnchecked(
        /* [in] */ Boolean alwaysSendConfiguration,
        /* [in] */ Boolean forceRelayout);

    // TODO(multidisplay): Rotate any display?
    /**
     * Updates the current rotation.
     *
     * Returns true if the rotation has been changed.  In this case YOU
     * MUST CALL sendNewConfiguration() TO UNFREEZE THE SCREEN.
     */
    CARAPI_(Boolean) UpdateRotationUncheckedLocked(
        /* [in] */ Boolean inTransaction);

    /**
     * Retrieve the current screen orientation, constants as per
     * {@link android.view.Surface}.
     */
    CARAPI GetRotation(
        /* [out] */ Int32* rotation);

    CARAPI IsRotationFrozen(
        /* [out] */ Boolean* result);

    /**
     * Watch the rotation of the screen.  Returns the current rotation,
     * calls back when it changes.
     */
    CARAPI WatchRotation(
        /* [in] */ IRotationWatcher* watcher,
        /* [out] */ Int32* rotation);

    /**
     * Remove a rotation watcher set using watchRotation.
     * @hide
     */
    CARAPI RemoveRotationWatcher(
        /* [in] */ IRotationWatcher* watcher);

    /**
     * Apps that use the compact menu panel (as controlled by the panelMenuIsCompact
     * theme attribute) on devices that feature a physical options menu key attempt to position
     * their menu panel window along the edge of the screen nearest the physical menu key.
     * This lowers the travel distance between invoking the menu panel and selecting
     * a menu option.
     *
     * This method helps control where that menu is placed. Its current implementation makes
     * assumptions about the menu key and its relationship to the screen based on whether
     * the device's natural orientation is portrait (width < height) or landscape.
     *
     * The menu key is assumed to be located along the bottom edge of natural-portrait
     * devices and along the right edge of natural-landscape devices. If these assumptions
     * do not hold for the target device, this method should be changed to reflect that.
     *
     * @return A {@link Gravity} value for placing the options menu window
     */
    CARAPI GetPreferredOptionsPanelGravity(
        /* [out] */ Int32* value);

    /**
     * Starts the view server on the specified port.
     *
     * @param port The port to listener to.
     *
     * @return True if the server was successfully started, false otherwise.
     *
     * @see com.android.server.wm.ViewServer
     * @see com.android.server.wm.ViewServer#VIEW_SERVER_DEFAULT_PORT
     */
    CARAPI StartViewServer(
        /* [in] */ Int32 port,
        /* [out] */ Boolean* result);

    CARAPI StopViewServer(
        /* [out] */ Boolean* result);

    /**
     * Indicates whether the view server is running.
     *
     * @return True if the server is running, false otherwise.
     *
     * @see com.android.server.wm.ViewServer
     */
    CARAPI IsViewServerRunning(
        /* [out] */ Boolean* result);

    /**
     * Lists all availble windows in the system. The listing is written in the
     * specified Socket's output stream with the following syntax:
     * windowHashCodeInHexadecimal windowName
     * Each line of the ouput represents a different window.
     *
     * @param client The remote client to send the listing to.
     * @return False if an error occured, true otherwise.
     */
    CARAPI_(Boolean) ViewServerListWindows(
        /* [in] */ ISocket* client);

    // TODO(multidisplay): Extend to multiple displays.
    /**
     * Returns the focused window in the following format:
     * windowHashCodeInHexadecimal windowName
     *
     * @param client The remote client to send the listing to.
     * @return False if an error occurred, true otherwise.
     */
    CARAPI_(Boolean) ViewServerGetFocusedWindow(
        /* [in] */ ISocket* client);

    /**
     * Sends a command to a target window. The result of the command, if any, will be
     * written in the output stream of the specified socket.
     *
     * The parameters must follow this syntax:
     * windowHashcode extra
     *
     * Where XX is the length in characeters of the windowTitle.
     *
     * The first parameter is the target window. The window with the specified hashcode
     * will be the target. If no target can be found, nothing happens. The extra parameters
     * will be delivered to the target window and as parameters to the command itself.
     *
     * @param client The remote client to sent the result, if any, to.
     * @param command The command to execute.
     * @param parameters The command parameters.
     *
     * @return True if the command was successfully delivered, false otherwise. This does
     *         not indicate whether the command itself was successful.
     */
    CARAPI_(Boolean) ViewServerWindowCommand(
        /* [in] */ ISocket* client,
        /* [in] */ const String& command,
        /* [in] */ const String& parameters);

    CARAPI_(void) AddWindowChangeListener(
        /* [in] */ IWindowChangeListener* listener);

    CARAPI_(void) RemoveWindowChangeListener(
        /* [in] */ IWindowChangeListener* listener);

    /*
     * Instruct the Activity Manager to fetch the current configuration and broadcast
     * that to config-changed listeners if appropriate.
     */
    CARAPI SendNewConfiguration();

    CARAPI_(AutoPtr<IConfiguration>) ComputeNewConfiguration();

    CARAPI_(AutoPtr<IConfiguration>) ComputeNewConfigurationLocked();

    CARAPI_(Boolean) ComputeScreenConfigurationLocked(
        /* [in] */ IConfiguration* config);

    CARAPI_(Boolean) IsHardKeyboardAvailable();

    CARAPI_(void) UpdateShowImeWithHardKeyboard();

    CARAPI_(void) SetOnHardKeyboardStatusChangeListener(
        /* [in] */ IOnHardKeyboardStatusChangeListener* listener);

    CARAPI_(void) NotifyHardKeyboardStatusChange();

    CARAPI_(AutoPtr<IBinder>) PrepareDragSurface(
        /* [in] */ IIWindow* window,
        /* [in] */ ISurfaceSession* session,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ISurface* outSurface);

    CARAPI PauseKeyDispatching(
        /* [in] */ IBinder* token);

    CARAPI ResumeKeyDispatching(
        /* [in] */ IBinder* token);

    CARAPI SetEventDispatching(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) DetectSafeMode();

    CARAPI DisplayReady();

    CARAPI SystemReady();

    CARAPI OpenSession(
        /* [in] */ IIWindowSessionCallback* callback,
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [out] */ IWindowSession** session);

    CARAPI InputMethodClientHasFocus(
        /* [in] */ IInputMethodClient* client,
        /* [out] */ Boolean* result);

    CARAPI GetInitialDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ IPoint* inSize,
        /* [out] */ IPoint** outSize);

    CARAPI GetBaseDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ IPoint* inSize,
        /* [out] */ IPoint** outSize);

    CARAPI SetForcedDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI ClearForcedDisplaySize(
        /* [in] */ Int32 displayId);

    CARAPI SetOverscan(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI GetInitialDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [out] */ Int32* result);

    CARAPI GetBaseDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [out] */ Int32* result);

    CARAPI SetForcedDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 density);

    CARAPI ClearForcedDisplayDensity(
        /* [in] */ Int32 displayId);

    CARAPI WindowForClientLocked(
        /* [in] */ CSession* session,
        /* [in] */ IIWindow* client,
        /* [in] */ Boolean throwOnError,
        /* [out] */ WindowState** win);

    CARAPI WindowForClientLocked(
        /* [in] */ CSession* session,
        /* [in] */ IBinder* client,
        /* [in] */ Boolean throwOnError,
        /* [out] */ WindowState** win);

    CARAPI_(void) RebuildAppWindowListLocked();

    CARAPI_(void) MakeWindowFreezingScreenIfNeededLocked(
        /* [in] */ WindowState* w);

    /**
     * Extracted from {@link #performLayoutAndPlaceSurfacesLockedInner} to reduce size of method.
     * @param windows List of windows on default display.
     * @return bitmap indicating if another pass through layout must be made.
     */
    CARAPI_(Int32) HandleAppTransitionReadyLocked(
        /* [in] */ WindowList* windows);

    CARAPI_(void) CheckDrawnWindowsLocked();

    CARAPI_(void) SetHoldScreenLocked(
        /* [in] */ CSession* newHoldScreen);

    CARAPI RequestTraversal();

    CARAPI_(void) RequestTraversalLocked();

    /** Note that Locked in this case is on mLayoutToAnim */
    CARAPI_(void) ScheduleAnimationLocked();

    CARAPI_(Boolean) CopyAnimToLayoutParamsLocked();

    /** If a window that has an animation specifying a colored background and the current wallpaper
     * is visible, then the color goes *below* the wallpaper so we don't cause the wallpaper to
     * suddenly disappear. */
    CARAPI_(Int32) AdjustAnimationBackground(
        /* [in] */ WindowStateAnimator* winAnimator);

    CARAPI_(Boolean) ReclaimSomeSurfaceMemoryLocked(
        /* [in] */ WindowStateAnimator* winAnimator,
        /* [in] */ const String& operation,
        /* [in] */ Boolean secure);

    static CARAPI_(Int32) GetPropertyInt(
        /* [in] */ ArrayOf<String>* tokens,
        /* [in] */ Int32 index,
        /* [in] */ Int32 defUnits,
        /* [in] */ Int32 defDps,
        /* [in] */ IDisplayMetrics* dm);

    CARAPI_(void) CreateWatermarkInTransaction();

    /**
     * Called by the status bar to notify Views of changes to System UI visiblity.
     */
    CARAPI StatusBarVisibilityChanged(
        /* [in] */ Int32 visibility);

    // TOOD(multidisplay): StatusBar on multiple screens?
    CARAPI_(void) UpdateStatusBarVisibilityLocked(
        /* [in] */ Int32 visibility);

    /**
     * Ask the window manager to re-evaluate the system UI flags.
     */
    CARAPI ReevaluateStatusBarVisibility();

    /**
     * Add a fake window to the window manager.  This window sits
     * at the top of the other windows and consumes events.
     */
    CARAPI AddFakeWindow(
        /* [in] */ ILooper* looper,
        /* [in] */ IInputEventReceiverFactory* inputEventReceiverFactory,
        /* [in] */ const String& name,
        /* [in] */ Int32 windowType,
        /* [in] */ Int32 layoutParamsFlags,
        /* [in] */ Int32 layoutParamsPrivateFlags,
        /* [in] */ Boolean canReceiveKeys,
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean touchFullscreen,
        /* [out] */ IFakeWindow** fakeWindow);

    CARAPI_(Boolean) RemoveFakeWindowLocked(
        /* [in] */ IFakeWindow* window);

    // It is assumed that this method is called only by InputMethodManagerService.
    CARAPI_(void) SaveLastInputMethodWindowForTransition();

    CARAPI_(Int32) GetInputMethodWindowVisibleHeight();

    /**
     * Device has a software navigation bar (separate from the status bar).
     */
    CARAPI HasNavigationBar(
        /* [out] */ Boolean* result);

    /**
     * Lock the device immediately with the specified options (can be null).
     */
    CARAPI LockNow(
        /* [in] */ IBundle* options);

    CARAPI_(void) ShowRecentApps();

    /**
     * Device is in safe mode.
     */
    CARAPI IsSafeModeEnabled(
        /* [out] */ Boolean* isSafe);

    CARAPI ClearWindowContentFrameStats(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI GetWindowContentFrameStats(
        /* [in] */ IBinder* token,
        /* [out] */ IWindowContentFrameStats** stats);

    // CARAPI_(void) DumpPolicyLocked(PrintWriter pw, String[] args, boolean dumpAll);

    // CARAPI_(void) DumpAnimatorLocked(PrintWriter pw, String[] args, boolean dumpAll);

    // CARAPI_(void) DumpTokensLocked(PrintWriter pw, boolean dumpAll);

    // CARAPI_(void) DumpSessionsLocked(PrintWriter pw, boolean dumpAll);

    // CARAPI_(void) DumpWindowsLocked(PrintWriter pw, boolean dumpAll,
    //     ArrayList<WindowState> windows);

    // CARAPI_(void) DumpWindowsNoHeaderLocked(PrintWriter pw, boolean dumpAll,
    //     ArrayList<WindowState> windows);

    // CARAPI_(Boolean) DumpWindows(PrintWriter pw, String name, String[] args,
    //         int opti, boolean dumpAll);

    // CARAPI_(void) DumpLastANRLocked(PrintWriter pw);

    /**
     * Saves information about the state of the window manager at
     * the time an ANR occurred before anything else in the system changes
     * in response.
     *
     * @param appWindowToken The application that ANR'd, may be null.
     * @param windowState The window that ANR'd, may be null.
     * @param reason The reason for the ANR, may be null.
     */
    CARAPI_(void) SaveANRStateLocked(
        /* [in] */ AppWindowToken* appWindowToken,
        /* [in] */ WindowState* windowState,
        /* [in] */ const String& reason);

    // CARAPI_(void) Dump(FileDescriptor fd, PrintWriter pw, String[] args);

    // Called by the heartbeat to ensure locks are not held indefnitely (for deadlock detection).
    CARAPI Monitor();

    CARAPI_(void) DebugLayoutRepeats(
        /* [in] */ const String& msg,
        /* [in] */ Int32 pendingLayoutChanges);

    CARAPI_(void) CreateDisplayContentLocked(
        /* [in] */ IDisplay* display);

    /**
     * Retrieve the DisplayContent for the specified displayId. Will create a new DisplayContent if
     * there is a Display for the displayId.
     * @param displayId The display the caller is interested in.
     * @return The DisplayContent associated with displayId or null if there is no Display for it.
     */
    CARAPI_(AutoPtr<DisplayContent>) GetDisplayContentLocked(
        /* [in] */ Int32 displayId);

    // There is an inherent assumption that this will never return null.
    CARAPI_(AutoPtr<DisplayContent>) GetDefaultDisplayContentLocked();

    CARAPI_(AutoPtr<WindowList>) GetDefaultWindowListLocked();

    CARAPI_(AutoPtr<IDisplayInfo>) GetDefaultDisplayInfoLocked();

    /**
     * Return the list of WindowStates associated on the passed display.
     * @param display The screen to return windows from.
     * @return The list of WindowStates on the screen, or null if the there is no screen.
     */
    CARAPI_(AutoPtr<WindowList>) GetWindowListLocked(
        /* [in] */ IDisplay* display);

    CARAPI_(AutoPtr<WindowList>) GetWindowListLocked(
        /* [in] */ Int32 displayId);

    CARAPI OnDisplayAdded(
        /* [in] */ Int32 displayId);

    CARAPI_(void) HandleDisplayAdded(
        /* [in] */ Int32 displayId);

    CARAPI OnDisplayRemoved(
        /* [in] */ Int32 displayId);

    CARAPI OnDisplayChanged(
        /* [in] */ Int32 displayId);

    CARAPI GetWindowManagerLock(
        /* [out] */ ISynchronize** lock);

    CARAPI ToString(
        /* [ou] */ String* str);

private:
    CARAPI_(void) InitPolicy();

    CARAPI_(void) PlaceWindowAfter(
        /* [in] */ WindowState* pos,
        /* [in] */ WindowState* window);

    CARAPI_(void) PlaceWindowBefore(
        /* [in] */ WindowState* pos,
        /* [in] */ WindowState* window);

    //This method finds out the index of a window that has the same app token as
    //win. used for z ordering the windows in mWindows
    CARAPI_(Int32) FindIdxBasedOnAppTokens(
        /* [in] */ WindowState* win);

    /**
     * Recursive search through a WindowList and all of its windows' children.
     * @param targetWin The window to search for.
     * @param windows The list to search.
     * @return The index of win in windows or of the window that is an ancestor of win.
     */
    CARAPI_(Int32) IndexOfWinInWindowList(
        /* [in] */ WindowState* win,
        /* [in] */ WindowList* windows);

    CARAPI_(Int32) AddAppWindowToListLocked(
        /* [in] */ WindowState* targetWin);

    CARAPI_(void) AddFreeWindowToListLocked(
        /* [in] */ WindowState* win);

    CARAPI_(void) AddAttachedWindowToListLocked(
        /* [in] */ WindowState* win,
        /* [in] */ Boolean addToToken);

    CARAPI_(void) AddWindowToListInOrderLocked(
        /* [in] */ WindowState* win,
        /* [in] */ Boolean addToToken);

    CARAPI_(Int32) TmpRemoveWindowLocked(
        /* [in] */ Int32 interestingPos,
        /* [in] */ WindowState* win);

    CARAPI_(void) ReAddWindowToListInOrderLocked(
        /* [in] */ WindowState* win);

    CARAPI_(Boolean) ApplyAnimationLocked(
        /* [in] */ AppWindowToken* atoken,
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Int32 transit,
        /* [in] */ Boolean enter,
        /* [in] */ Boolean isVoiceInteraction);

    CARAPI CreateTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 userId,
        /* [in] */ AppWindowToken* atoken,
        /* [out] */ Task** task);

    CARAPI_(AutoPtr<IConfiguration>) UpdateOrientationFromAppTokensLocked(
        /* [in] */ IConfiguration* currentConfig,
        /* [in] */ IBinder* freezeThisOneIfNeeded);

    CARAPI_(void) StartAppFreezingScreenLocked(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI_(Boolean) TmpRemoveAppWindowsLocked(
        /* [in] */ WindowToken* token);

    CARAPI_(Int32) FindAppWindowInsertionPointLocked(
        /* [in] */ AppWindowToken* target);

    CARAPI_(Int32) ReAddWindowLocked(
        /* [in] */ Int32 index,
        /* [in] */ WindowState* win);

    CARAPI_(Int32) ReAddAppWindowsLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Int32 index,
        /* [in] */ WindowToken* token);

    CARAPI_(void) SetAnimatorDurationScale(
        /* [in] */ Float scale);

    CARAPI_(Boolean) CheckWaitingForWindowsLocked();

    CARAPI_(Boolean) CheckBootAnimationCompleteLocked();

    CARAPI_(void) CheckQuickBootException();

    CARAPI_(void) ShowStrictModeViolation(
        /* [in] */ Int32 arg,
        /* [in] */ Int32 pid);

    static CARAPI_(void) ConvertCropForSurfaceFlinger(
        /* [in] */ IRect* crop,
        /* [in] */ Int32 rot,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(Boolean) IsSystemSecure();

    CARAPI_(void) NotifyWindowsChanged();

    CARAPI_(void) NotifyFocusChanged();

    CARAPI_(AutoPtr<WindowState>) FindWindow(
        /* [in] */ Int32 hashCode);

    CARAPI_(void) AdjustDisplaySizeRanges(
        /* [in] */ IDisplayInfo* displayInfo,
        /* [in] */ Int32 rotation,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(Int32) ReduceConfigLayout(
        /* [in] */ Int32 curLayout,
        /* [in] */ Int32 rotation,
        /* [in] */ Float density,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(void) ComputeSizeRangesAndScreenLayout(
        /* [in] */ IDisplayInfo* displayInfo,
        /* [in] */ Boolean rotated,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh,
        /* [in] */ Float density,
        /* [in] */ IConfiguration* outConfig);

    CARAPI_(Int32) ReduceCompatConfigWidthSize(
        /* [in] */ Int32 curSize,
        /* [in] */ Int32 rotation,
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(Int32) ComputeCompatSmallestWidth(
        /* [in] */ Boolean rotated,
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ Int32 dw,
        /* [in] */ Int32 dh);

    CARAPI_(AutoPtr<WindowState>) GetFocusedWindow();

    CARAPI_(AutoPtr<WindowState>) GetFocusedWindowLocked();

    CARAPI_(void) DisplayReady(
        /* [in] */ Int32 displayId);

    // -------------------------------------------------------------
    // Async Handler
    // -------------------------------------------------------------
    CARAPI HandleReportFocusChange();

    CARAPI HandleReportLosingFocus();

    CARAPI HandleDoTraversal();

    CARAPI HandleAddStarting(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI HandleRemoveStarting(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI HandleFinishedStarting();

    CARAPI HandleReportApplicationTokenDrawn(
        /* [in] */ AppWindowToken* wtoken);

    CARAPI HandleReportApplicationTokenWindows(
        /* [in] */ Boolean nowVisible,
        /* [in] */ Boolean nowGone,
        /* [in] */ AppWindowToken* wtoken);

    CARAPI HandleWindowFreezeTimeout();

    CARAPI HandleAppTransitionTimeout();

    CARAPI HandlePersistAnimationScale();

    CARAPI HandleForceGc();

    CARAPI HandleAppFreezeTimeout();

    CARAPI HandleClientFreezeTimeout();

    CARAPI HandleReportWindowsChange();

    CARAPI HandleDragStartTimeout(
        /* [in] */ IBinder* winBinder);

    CARAPI HandleDragEndTimeout(
        /* [in] */ IBinder* winBinder);

    CARAPI HandleWaitingForDrawnTimeout();

    CARAPI_(void) ReadForcedDisplaySizeAndDensityLocked(
        /* [in] */ DisplayContent* displayContent);

    // displayContent must not be null
    CARAPI_(void) SetForcedDisplaySizeLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // displayContent must not be null
    CARAPI_(void) SetForcedDisplayDensityLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Int32 density);

    // displayContent must not be null
    CARAPI_(void) ReconfigureDisplayLocked(
        /* [in] */ DisplayContent* displayContent);

    CARAPI_(void) ConfigureDisplayPolicyLocked(
        /* [in] */ DisplayContent* displayContent);

    CARAPI_(void) SetOverscanLocked(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(void) RebuildAppWindowListLocked(
        /* [in] */ DisplayContent* displayContent);

    CARAPI_(void) AssignLayersLocked(
        /* [in] */ WindowList* windows);

    CARAPI_(void) PerformLayoutAndPlaceSurfacesLocked();

    CARAPI_(void) PerformLayoutAndPlaceSurfacesLockedLoop();

    CARAPI_(void) PerformLayoutLockedInner(
        /* [in] */ DisplayContent* displayContent,
        /* [in] */ Boolean initial,
        /* [in] */ Boolean updateInputWindows);

    /**
     * Extracted from {@link #performLayoutAndPlaceSurfacesLockedInner} to reduce size of method.
     * @return bitmap indicating if another pass through layout must be made.
     */
    CARAPI_(Int32) HandleAnimatingStoppedAndTransitionLocked();

    CARAPI_(void) UpdateResizingWindows(
        /* [in] */ WindowState* w);

    /**
     * Extracted from {@link #performLayoutAndPlaceSurfacesLockedInner} to reduce size of method.
     *
     * @param w WindowState this method is applied to.
     * @param currentTime The time which animations use for calculating transitions.
     * @param innerDw Width of app window.
     * @param innerDh Height of app window.
     */
    CARAPI_(void) HandleNotObscuredLocked(
        /* [in] */ WindowState* w,
        /* [in] */ Int64 currentTime,
        /* [in] */ Int32 innerDw,
        /* [in] */ Int32 innerDh);

    CARAPI_(void) HandleFlagDimBehind(
        /* [in] */ WindowState* w);

    CARAPI_(void) HandlePrivateFlagFullyTransparent(
        /* [in] */ WindowState* w);

    CARAPI_(void) HandleFlagBlurBehind(
        /* [in] */ WindowState* w);

    CARAPI_(void) HandlePrivateFlagBlurWithMasking(
        /* [in] */ WindowState* w);

    CARAPI_(void) UpdateAllDrawnLocked(
        /* [in] */ DisplayContent* displayContent);

    // "Something has changed!  Let's make it correct now."
    CARAPI_(void) PerformLayoutAndPlaceSurfacesLockedInner(
        /* [in] */ Boolean recoveringMemory);

    CARAPI_(Int32) ToBrightnessOverride(
        /* [in] */ Float value);

    CARAPI_(Boolean) NeedsLayout();

    CARAPI_(Boolean) UpdateFocusedWindowLocked(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean updateInputWindows);

    CARAPI_(AutoPtr<WindowState>) ComputeFocusedWindowLocked();

    CARAPI_(AutoPtr<WindowState>) FindFocusedWindowLocked(
        /* [in] */ DisplayContent* displayContent);

    CARAPI_(void) StartFreezingDisplayLocked(
        /* [in] */ Boolean inTransaction,
        /* [in] */ Int32 exitAnim,
        /* [in] */ Int32 enterAnim);

    CARAPI_(void) StopFreezingDisplayLocked();

    CARAPI_(AutoPtr<DisplayContent>) NewDisplayContentLocked(
        /* [in] */ IDisplay* display);

    CARAPI_(void) HandleDisplayRemovedLocked(
        /* [in] */ Int32 displayId);

    CARAPI_(void) HandleDisplayChangedLocked(
        /* [in] */ Int32 displayId);

public:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_ADD_REMOVE;
    static const Boolean DEBUG_FOCUS;
    static const Boolean DEBUG_FOCUS_LIGHT;
    static const Boolean DEBUG_ANIM;
    static const Boolean DEBUG_LAYOUT;
    static const Boolean DEBUG_RESIZE;
    static const Boolean DEBUG_LAYERS;
    static const Boolean DEBUG_INPUT;
    static const Boolean DEBUG_INPUT_METHOD;
    static const Boolean DEBUG_VISIBILITY;
    static const Boolean DEBUG_WINDOW_MOVEMENT;
    static const Boolean DEBUG_TOKEN_MOVEMENT;
    static const Boolean DEBUG_ORIENTATION;
    static const Boolean DEBUG_APP_ORIENTATION;
    static const Boolean DEBUG_CONFIGURATION;
    static const Boolean DEBUG_APP_TRANSITIONS;
    static const Boolean DEBUG_STARTING_WINDOW;
    static const Boolean DEBUG_REORDER;
    static const Boolean DEBUG_WALLPAPER;
    static const Boolean DEBUG_WALLPAPER_LIGHT;
    static const Boolean DEBUG_DRAG;
    static const Boolean DEBUG_SCREEN_ON;
    static const Boolean DEBUG_SCREENSHOT;
    static const Boolean DEBUG_BOOT;
    static const Boolean DEBUG_LAYOUT_REPEATS;
    static const Boolean DEBUG_SURFACE_TRACE;
    static const Boolean DEBUG_WINDOW_TRACE;
    static const Boolean DEBUG_TASK_MOVEMENT;
    static const Boolean DEBUG_STACK;
    static const Boolean DEBUG_DISPLAY;
    static const Boolean SHOW_SURFACE_ALLOC;
    static const Boolean SHOW_TRANSACTIONS;
    static const Boolean SHOW_LIGHT_TRANSACTIONS;
    static const Boolean HIDE_STACK_CRAWLS;
    static const Int32 LAYOUT_REPEAT_THRESHOLD = 4;

    static const Boolean PROFILE_ORIENTATION = FALSE;
    static const Boolean localLOGV;

    /** How much to multiply the policy's type layer, to reserve room
     * for multiple windows of the same type and Z-ordering adjustment
     * with TYPE_LAYER_OFFSET. */
    static const Int32 TYPE_LAYER_MULTIPLIER = 10000;

    /** Offset from TYPE_LAYER_MULTIPLIER for moving a group of windows above
     * or below others in the same layer. */
    static const Int32 TYPE_LAYER_OFFSET = 1000;

    /** How much to increment the layer for each window, to reserve room
     * for effect surfaces between them.
     */
    static const Int32 WINDOW_LAYER_MULTIPLIER = 5;

    /**
     * Dim surface layer is immediately below target window.
     */
    static const Int32 LAYER_OFFSET_DIM = 1+1;

    /**
     * Blur surface layer is immediately below dim layer.
     */
    static const Int32 LAYER_OFFSET_BLUR = 2+1;

    /**
      * Blur_with_masking layer is immediately below blur layer.
      */
    static const Int32 LAYER_OFFSET_BLUR_WITH_MASKING = 1;

    /**
     * FocusedStackFrame layer is immediately above focused window.
     */
    static const Int32 LAYER_OFFSET_FOCUSED_STACK = 1;

    /**
     * Animation thumbnail is as far as possible below the window above
     * the thumbnail (or in other words as far as possible above the window
     * below it).
     */
    static const Int32 LAYER_OFFSET_THUMBNAIL = WINDOW_LAYER_MULTIPLIER - 1;

    /**
     * Layer at which to put the rotation freeze snapshot.
     */
    static const Int32 FREEZE_LAYER = (TYPE_LAYER_MULTIPLIER * 200) + 1;

    /**
     * Layer at which to put the mask for emulated screen sizes.
     */
    static const Int32 MASK_LAYER = TYPE_LAYER_MULTIPLIER * 200;

    /** The maximum length we will accept for a loaded animation duration:
     * this is 10 seconds.
     */
    static const Int32 MAX_ANIMATION_DURATION = 10 * 1000;

    /** Amount of time (in milliseconds) to animate the fade-in-out transition for
     * compatible windows.
     */
    static const Int32 DEFAULT_FADE_IN_OUT_DURATION = 400;

    /** Amount of time (in milliseconds) to delay before declaring a window freeze timeout. */
    static const Int32 WINDOW_FREEZE_TIMEOUT_DURATION = 3000;

    /**
     * If true, the window manager will do its own custom freezing and general
     * management of the screen during rotation.
     */
    static const Boolean CUSTOM_SCREEN_ROTATION = TRUE;

    // Default input dispatching timeout in nanoseconds.
    static const Int64 DEFAULT_INPUT_DISPATCHING_TIMEOUT_NANOS = 5000 * 1000000LL;

    // Poll interval in milliseconds for watching boot animation finished.
    static const Int32 BOOT_ANIMATION_POLL_INTERVAL = 200;

    // The name of the boot animation service in init.rc.
    static const String BOOT_ANIMATION_SERVICE;

    /** Minimum value for attachStack and resizeStack weight value */
    static const Float STACK_WEIGHT_MIN = 0.2f;

    /** Maximum value for attachStack and resizeStack weight value */
    static const Float STACK_WEIGHT_MAX = 0.8f;

    static const Int32 UPDATE_FOCUS_NORMAL = 0;
    static const Int32 UPDATE_FOCUS_WILL_ASSIGN_LAYERS = 1;
    static const Int32 UPDATE_FOCUS_PLACING_SURFACES = 2;
    static const Int32 UPDATE_FOCUS_WILL_PLACE_SURFACES = 3;

    // We give a wallpaper up to 150ms to finish scrolling.
    static const Int64 WALLPAPER_TIMEOUT = 150LL;
    // Time we wait after a timeout before trying to wait again.
    static const Int64 WALLPAPER_TIMEOUT_RECOVERY = 10000LL;

    static const Int32 ADJUST_WALLPAPER_LAYERS_CHANGED = 1<<1;
    static const Int32 ADJUST_WALLPAPER_VISIBILITY_CHANGED = 1<<2;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    /**
     * Current user when multi-user is enabled. Don't show windows of
     * non-current user. Also see mCurrentProfileIds.
     */
    Int32 mCurrentUserId;
    /**
     * Users that are profiles of the current user. These are also allowed to show windows
     * on the current user.
     */
    AutoPtr< ArrayOf<Int32> > mCurrentProfileIds;

    AutoPtr<IContext> mContext;

    Boolean mHaveInputMethods;

    Boolean mHasPermanentDpad;

    Boolean mAllowBootMessages;

    Boolean mLimitedAlphaCompositing;

    AutoPtr<IWindowManagerPolicy> mPolicy;

    AutoPtr<IIActivityManager> mActivityManager;

    AutoPtr<IIBatteryStats> mBatteryStats;

    AutoPtr<IAppOpsManager> mAppOps;

    AutoPtr<DisplaySettings> mDisplaySettings;

    /**
     * All currently active sessions with clients.
     */
    AutoPtr<IArraySet> mSessions;//new ArraySet<CSession>();

    /**
     * Mapping from an IIWindow IBinder to the server's Window object.
     * This is also used as the lock for all of our state.
     * NOTE: Never call into methods that lock ActivityManagerService while holding this object.
     */
    HashMap<AutoPtr<IBinder>, AutoPtr<WindowState> > mWindowMap;
    AutoPtr<Object> mWindowMapLock;

    /**
     * Mapping from a token IBinder to a WindowToken object.
     */
    HashMap<AutoPtr<IBinder>, AutoPtr<WindowToken> > mTokenMap;

    /**
     * List of window tokens that have finished starting their application,
     * and now need to have the policy remove their windows.
     */
    List< AutoPtr<AppWindowToken> > mFinishedStarting;

    /**
     * Fake windows added to the window manager.  Note: ordered from top to
     * bottom, opposite of mWindows.
     */
    List< AutoPtr<FakeWindowImpl> > mFakeWindows;

    /**
     * Windows that are being resized.  Used so we can tell the client about
     * the resize after closing the transaction in which we resized the
     * underlying surface.
     */
    AutoPtr<WindowList> mResizingWindows;

    /**
     * Windows whose animations have ended and now must be removed.
     */
    AutoPtr<WindowList> mPendingRemove;

    /**
     * Stacks whose animations have ended and whose tasks, apps, selves may now be removed.
     */
    AutoPtr<IArraySet> mPendingStacksRemove;// = new ArraySet<TaskStack>();

    /**
     * Used when processing mPendingRemove to avoid working on the original array.
     */
    AutoPtr< ArrayOf<WindowState*> > mPendingRemoveTmp;

    /**
     * Windows whose surface should be destroyed.
     */
    AutoPtr<WindowList> mDestroySurface;

    /**
     * Windows that have lost input focus and are waiting for the new
     * focus window to be displayed before they are told about this.
     */
    AutoPtr<WindowList> mLosingFocus;

    /**
     * This is set when we have run out of memory, and will either be an empty
     * list or contain windows that need to be force removed.
     */
    AutoPtr<WindowList> mForceRemoves;

    /**
     * Windows that clients are waiting to have drawn.
     */
    AutoPtr<WindowList> mWaitingForDrawn;
    /**
     * And the callback to make when they've all been drawn.
     */
    AutoPtr<IRunnable> mWaitingForDrawnCallback;

    /**
     * Windows that have called relayout() while we were running animations,
     * so we need to tell when the animation is done.
     */
    AutoPtr<WindowList> mRelayoutWhileAnimating;

    /**
     * Used when rebuilding window list to keep track of windows that have
     * been removed.
     */
    AutoPtr< ArrayOf<WindowState*> > mRebuildTmp;

    /**
     * Stores for each user whether screencapture is disabled
     * This array is essentially a cache for all userId for
     * {@link android.app.admin.DevicePolicyManager#getScreenCaptureDisabled}
     */
    AutoPtr<ISparseArray> mScreenCaptureDisabled; // = new SparseArray<Boolean>();

    AutoPtr<IIInputMethodManager> mInputMethodManager;

    AutoPtr<AccessibilityController> mAccessibilityController;

    AutoPtr<ISurfaceSession> mFxSession;
    AutoPtr<Watermark> mWatermark;
    AutoPtr<StrictModeFlash> mStrictModeFlash;
    AutoPtr<CircularDisplayMask> mCircularDisplayMask;
    AutoPtr<EmulatorDisplayOverlay> mEmulatorDisplayOverlay;
    AutoPtr<FocusedStackFrame> mFocusedStackFrame;

    Int32 mFocusedStackLayer;

    AutoPtr< ArrayOf<Float> > mTmpFloats;
    AutoPtr<IRect> mTmpContentRect;

    Boolean mDisplayReady;
    Boolean mSafeMode;
    Boolean mDisplayEnabled;
    Boolean mSystemBooted;
    Boolean mForceDisplayEnabled;
    Boolean mShowingBootMessages;
    Boolean mBootAnimationStopped;

    String mLastANRState;

    /** All DisplayContents in the world, kept here */
    AutoPtr<ISparseArray> mDisplayContents; // =  new SparseArray<DisplayContent>(2);

    Int32 mRotation;
    Int32 mForcedAppOrientation;
    Boolean mAltOrientation;
    List<AutoPtr<RotationWatcher> > mRotationWatchers;
    Int32 mDeferredRotationPauseCount;

    Int32 mSystemDecorLayer;
    AutoPtr<IRect> mScreenRect;

    Boolean mTraversalScheduled;
    Boolean mDisplayFrozen;
    Int64 mDisplayFreezeTime;
    Int32 mLastDisplayFreezeDuration;
    AutoPtr<IObject> mLastFinishedFreezeSource;
    Boolean mWaitingForConfig;
    Boolean mWindowsFreezingScreen;
    Boolean mClientFreezingScreen;
    Int32 mAppsFreezingScreen;
    Int32 mLastWindowForcedOrientation;

    Int32 mLayoutSeq;

    Int32 mLastStatusBarVisibility;

    // State while inside of layoutAndPlaceSurfacesLocked().
    Boolean mFocusMayChange;

    AutoPtr<IConfiguration> mCurConfiguration;

    AutoPtr<AppTransition> mAppTransition;
    Boolean mStartingIconInTransition;
    Boolean mSkipAppTransitionAnimation;
    List< AutoPtr<AppWindowToken> > mOpeningApps;
    List< AutoPtr<AppWindowToken> > mClosingApps;

    Boolean mIsTouchDevice;

    AutoPtr<IDisplayMetrics> mDisplayMetrics;
    AutoPtr<IDisplayMetrics> mRealDisplayMetrics;
    AutoPtr<IDisplayMetrics> mTmpDisplayMetrics;
    AutoPtr<IDisplayMetrics> mCompatDisplayMetrics;

    AutoPtr<H> mH;

    AutoPtr<IChoreographer> mChoreographer;

    AutoPtr<WindowState> mCurrentFocus;
    AutoPtr<WindowState> mLastFocus;

    /** This just indicates the window the input method is on top of, not
     * necessarily the window its input is going to. */
    AutoPtr<WindowState> mInputMethodTarget;

    /** If true hold off on modifying the animation layer of mInputMethodTarget */
    Boolean mInputMethodTargetWaitingAnim;
    Int32 mInputMethodAnimLayerAdjustment;

    AutoPtr<WindowState> mInputMethodWindow;
    AutoPtr<WindowList> mInputMethodDialogs;

    Boolean mHardKeyboardAvailable;
    Boolean mShowImeWithHardKeyboard;
    AutoPtr<IOnHardKeyboardStatusChangeListener> mHardKeyboardStatusChangeListener;
    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<IArrayList> mWallpaperTokens;

    // If non-null, this is the currently visible window that is associated
    // with the wallpaper.
    AutoPtr<WindowState> mWallpaperTarget;
    // If non-null, we are in the middle of animating from one wallpaper target
    // to another, and this is the lower one in Z-order.
    AutoPtr<WindowState> mLowerWallpaperTarget;
    // If non-null, we are in the middle of animating from one wallpaper target
    // to another, and this is the higher one in Z-order.
    AutoPtr<WindowState> mUpperWallpaperTarget;

    Int32 mWallpaperAnimLayerAdjustment;
    Float mLastWallpaperX;
    Float mLastWallpaperY;
    Float mLastWallpaperXStep;
    Float mLastWallpaperYStep;
    Int32 mLastWallpaperDisplayOffsetX;
    Int32 mLastWallpaperDisplayOffsetY;
    // This is set when we are waiting for a wallpaper to tell us it is done
    // changing its scroll position.
    AutoPtr<WindowState> mWaitingOnWallpaper;
    // The last time we had a timeout when waiting for a wallpaper.
    Int64 mLastWallpaperTimeoutTime;
    Boolean mAnimateWallpaperWithTarget;

    AutoPtr<AppWindowToken> mFocusedApp;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerManagerInternal> mPowerManagerInternal;

    Float mWindowAnimationScaleSetting;
    Float mTransitionAnimationScaleSetting;
    Float mAnimatorDurationScaleSetting;
    Boolean mAnimationsDisabled;

    AutoPtr<CInputManagerService> mInputManager;
    AutoPtr<IDisplayManagerInternal> mDisplayManagerInternal;
    AutoPtr<IDisplayManager> mDisplayManager;

    // Who is holding the screen on.
    AutoPtr<CSession> mHoldingScreenOn;
    AutoPtr<IPowerManagerWakeLock> mHoldingScreenWakeLock;

    Boolean mTurnOnScreen;

    AutoPtr<DragState> mDragState;

    // For frozen screen animations.
    Int32 mExitAnimId;
    Int32 mEnterAnimId;

    AutoPtr<LayoutFields> mInnerFields;

    Boolean mAnimationScheduled;

    AutoPtr<WindowAnimator> mAnimator;
    Object mAnimatorLock;

    AutoPtr<ISparseArray> mTaskIdToTask;// = new SparseArray<Task>();

    /** All of the TaskStacks in the window manager, unordered. For an ordered list call
     * DisplayContent.getStacks(). */
    AutoPtr<ISparseArray> mStackIdToStack;// = new SparseArray<TaskStack>();

    /**
     * Whether the UI is currently running in touch mode (not showing
     * navigational focus because the user is directly pressing the screen).
     */
    Boolean mInTouchMode;

    AutoPtr<IConfiguration> mTempConfiguration;

    // The desired scaling factor for compatible apps.
    Float mCompatibleScreenScale;

    // If true, only the core apps and services are being launched because the device
    // is in a special boot mode, such as being encrypted or waiting for a decryption password.
    // For example, when this flag is true, there will be no wallpaper service.
    Boolean mOnlyCore;

    AutoPtr<InputMonitor> mInputMonitor;

private:
    // Maximum number of milliseconds to wait for input devices to be enumerated before
    // proceding with safe mode detection.
    static const Int32 INPUT_DEVICES_READY_FOR_SAFE_MODE_DETECTION_TIMEOUT_MILLIS = 1000;
    static const String SYSTEM_SECURE;
    static const String SYSTEM_DEBUGGABLE;
    static const String DENSITY_OVERRIDE;
    static const String SIZE_OVERRIDE;
    static const String SYSTEM_DEFAULT_ROTATION;
    static const Int32 MAX_SCREENSHOT_RETRIES = 3;

    // The flag describing a full screen app window (where the app takes care of drawing under the
    // SystemUI bars)
    static const Int32 SYSTEM_UI_FLAGS_LAYOUT_STABLE_FULLSCREEN =
            IView::SYSTEM_UI_FLAG_LAYOUT_STABLE | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;

    static const String PROPERTY_EMULATOR_CIRCULAR;

    AutoPtr<KeyguardDisableHandler> mKeyguardDisableHandler;

    Int32 mSfHwRotation;

    Boolean mKeyguardWaitingForActivityDrawn;

    // This is held as long as we have the screen frozen, to give us time to
    // perform a rotation animation when turning off shows the lock screen which
    // changes the orientation.
    AutoPtr<IPowerManagerWakeLock> mScreenFrozenLock;

    /** Skip repeated AppWindowTokens initialization. Note that AppWindowsToken's version of this
     * is a long initialized to Long.MIN_VALUE so that it doesn't match this value on startup. */
    Int32 mTransactionSequence;

    /** Only do a maximum of 6 repeated layouts. After that quit */
    Int32 mLayoutRepeatCount;

    AutoPtr<PointerEventDispatcher> mPointerEventDispatcher;

    AutoPtr<IWindowContentFrameStats> mTempWindowRenderStats;

    AutoPtr<ViewServer> mViewServer;
    List< AutoPtr<IWindowChangeListener> > mWindowChangeListeners;
    Boolean mWindowsChanged;
    Boolean mEventDispatchingEnabled;

    Boolean mInLayout;

    AutoPtr<WindowBinder> mWindowBinder;

    Boolean mForceDisableHardwareKeyboard;

    friend class H;
    friend class CSession;
    friend class WindowAnimator;
    friend class WindowStateAnimator;
    friend class AppWindowToken;
    friend class DragState;
    friend class InputMonitor;
    // friend class InputManagerCallbacks;
};

} // Wm
} // Server
} // Droid
} // Elastos

template <>
struct Conversion<Elastos::Droid::Server::Wm::CWindowManagerService*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SERVER_WM_CWINDOWMANAGERSERVICE_H__
