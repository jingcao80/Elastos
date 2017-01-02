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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWMANAGER_H__

#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Telecom.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/ProgressDialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/policy/impl/GlobalActions.h"
#include "elastos/droid/internal/policy/impl/GlobalKeyManager.h"
#include "elastos/droid/internal/policy/impl/RecentApplicationsDialog.h"
#include "elastos/droid/internal/policy/impl/ShortcutManager.h"
#include "elastos/droid/internal/policy/impl/WakeGestureListener.h"
#include "elastos/droid/internal/policy/impl/WindowOrientationListener.h"
#include "elastos/droid/service/gesture/EdgeGestureManager.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/utility/SparseArray.h"
#include "elastos/droid/view/InputEventReceiver.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IIUiModeManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::App::ProgressDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Os::IDeviceKeyHandler;
using Elastos::Droid::Internal::Policy::Impl::GlobalActions;
using Elastos::Droid::Internal::Policy::Impl::IRecentApplicationsDialog;
using Elastos::Droid::Internal::Policy::Impl::Keyguard::IKeyguardServiceDelegate;
using Elastos::Droid::Internal::Policy::Impl::Keyguard::IKeyguardServiceDelegateShowListener;
using Elastos::Droid::Internal::Policy::Impl::ShortcutManager;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Internal::Widget::IPointerLocationView;
using Elastos::Droid::Internal::Utility::Gesture::IEdgeGesturePosition;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IVibrator;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Os::IUEvent;
using Elastos::Droid::Service::Dreams::IDreamManagerInternal;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::Service::Gesture::EdgeGestureManager;
using Elastos::Droid::Service::Gesture::IEdgeGestureManager;
using Elastos::Droid::Service::Gesture::IEdgeGestureActivationListener;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::IApplicationToken;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IFakeWindow;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputEventReceiver;
using Elastos::Droid::View::IInputEventReceiverFactory;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::IOnKeyguardExitResult;
using Elastos::Droid::View::IScreenOnListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerInternal;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IWindowManagerPolicyWindowManagerFuncs;
using Elastos::Droid::View::IWindowState;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Telecom::ITelecomManager;

using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * WindowManagerPolicy implementation for the Android phone UI.  This
  * introduces a new method suffix, Lp, for an internal lock of the
  * PhoneWindowManager.  This is used to protect some internal state, and
  * can be acquired with either the Lw and Li lock held, so has the restrictions
  * of both of those when held.
  */
class PhoneWindowManager
    : public Object
    , public IWindowManagerPolicy
    , public IPhoneWindowManager
{
protected:
    class MyWakeGestureListener
        : public WakeGestureListener
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::MyWakeGestureListener")

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ PhoneWindowManager* host);

        CARAPI OnWakeUp();

    private:
        PhoneWindowManager* mHost;
    };

    class SystemGesturesPointerEventListenerCallbacks
        : public Object
        , public ISystemGesturesPointerEventListenerCallbacks
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks")

        CAR_INTERFACE_DECL()

        SystemGesturesPointerEventListenerCallbacks(
            /* [in] */ PhoneWindowManager* host);

        CARAPI OnSwipeFromTop();

        CARAPI OnSwipeFromBottom();

        CARAPI OnSwipeFromRight();

        CARAPI OnDebug();

    private:
        PhoneWindowManager* mHost;
    };

    class WindowManagerDrawCallbackRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::WindowManagerDrawCallbackRunnable")

        WindowManagerDrawCallbackRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class KeyguardDelegateCallbackShowListener
        : public Object
        , public IKeyguardServiceDelegateShowListener
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::KeyguardDelegateCallbackShowListener")

        KeyguardDelegateCallbackShowListener(
            /* [in] */ PhoneWindowManager* host);

        CAR_INTERFACE_DECL()

        CARAPI OnShown(
            /* [in] */ IBinder* windowToken);

    private:
        PhoneWindowManager* mHost;
    };

    class KeyguardDelegateOnKeyguardExitResult
        : public Object
        , public IOnKeyguardExitResult
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult")

        KeyguardDelegateOnKeyguardExitResult(
            /* [in] */ PhoneWindowManager* host);

        CAR_INTERFACE_DECL()

        CARAPI OnKeyguardExitResult(
            /* [in] */ Boolean success);

    private:
        PhoneWindowManager* mHost;
    };

    class ClearHideNavigationFlagRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ClearHideNavigationFlagRunnable")

        ClearHideNavigationFlagRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class RequestTransientBarsRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::RequestTransientBarsRunnable")

        RequestTransientBarsRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class KeyguardDelegateKeyguardDone
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::KeyguardDelegateKeyguardDone")

        KeyguardDelegateKeyguardDone(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class KeyguardDelegateKeyguardDismiss
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::KeyguardDelegateKeyguardDismiss")

        KeyguardDelegateKeyguardDismiss(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class KeyguardDelegateActivityDrawn
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::KeyguardDelegateActivityDrawn")

        KeyguardDelegateActivityDrawn(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::SettingsObserver")

        SettingsObserver(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI Observe();

    private:
        PhoneWindowManager* mHost;
    };

    class MyOrientationListener
         : public WindowOrientationListener
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::MyOrientationListener")

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnProposedRotationChanged(
            /* [in] */ Int32 rotation);

    private:
        PhoneWindowManager* mHost;
    };

    /**
     * A delayed callback use to determine when it is okay to re-allow applications
     * to use certain system UI flags.  This is used to prevent applications from
     * spamming system UI changes that prevent the navigation bar from being shown.
     */
    // final Runnable mAllowSystemUiDelay = new Runnable() {
    //     @Override public void run() {
    //     }
    // };

    /**
     * Input handler used while nav bar is hidden.  Captures any touch on the screen,
     * to determine when the nav bar should be shown and prevent applications from
     * receiving those touches.
     */
    class HideNavInputEventReceiver
        : public InputEventReceiver
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::HideNavInputEventReceiver")

        CARAPI constructor(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnInputEvent(
            /* [in] */ IInputEvent* event);

    private:
        PhoneWindowManager* mHost;
    };

    class HideNavInputEventReceiverFactory
        : public Object
        , public IInputEventReceiverFactory
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::HideNavInputEventReceiverFactory")

        HideNavInputEventReceiverFactory(
            /* [in] */ PhoneWindowManager* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI CreateInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [out] */ IInputEventReceiver** receiver);

    private:
        PhoneWindowManager* mHost;
    };

    class ScreenshotTimeoutRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenshotTimeoutRunnable")

        ScreenshotTimeoutRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class DockBroadReceiver
        : public BroadcastReceiver
    {
    public:
        DockBroadReceiver(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("PhoneWindowManager::DockBroadReceiver: ")
    private:
        PhoneWindowManager* mHost;
    };

    class DreamBroadReceiver
        : public BroadcastReceiver
    {
    public:
        DreamBroadReceiver(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("PhoneWindowManager::DreamBroadReceiver: ")
    private:
        PhoneWindowManager* mHost;
    };

    class MultiuserBroadReceiver
        : public BroadcastReceiver
    {
    public:
        MultiuserBroadReceiver(
            /*in*/ PhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("PhoneWindowManager::MultiuserBroadReceiver: ")
    private:
        PhoneWindowManager* mHost;
    };

    class WifiDisplayReceiver
        : public BroadcastReceiver
    {
    public:
        WifiDisplayReceiver(
            /*in*/ PhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("PhoneWindowManager::WifiDisplayReceiver: ")
    private:
        PhoneWindowManager* mHost;
    };

    class ScreenLockTimeoutRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenLockTimeoutRunnable")

        ScreenLockTimeoutRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

        CARAPI_(void) SetLockOptions(
            /* [in] */ IBundle* options);

    protected:
        AutoPtr<IBundle> mOptions;
        PhoneWindowManager* mHost;
    };

    class QuickBootPowerLongPressRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::QuickBootPowerLongPressRunnable")

        QuickBootPowerLongPressRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

    protected:
        PhoneWindowManager* mHost;
    };

    class MyEdgeGestureActivationListener
        : public EdgeGestureManager::EdgeGestureActivationListener
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::MyEdgeGestureActivationListener")

        MyEdgeGestureActivationListener(
            /* [in] */ PhoneWindowManager* host);

        CARAPI OnEdgeGestureActivation(
            /* [in] */ Int32 touchX,
            /* [in] */ Int32 touchY,
            /* [in] */ IEdgeGesturePosition* position,
            /* [in] */ Int32 flags);

    protected:
        PhoneWindowManager* mHost;
    };

private:
    class PolicyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::PolicyHandler")

        PolicyHandler(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PhoneWindowManager* mHost;
    };

    class HDMIUEventObserver
        : public UEventObserver
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::HDMIUEventObserver")

        HDMIUEventObserver(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);

    private:
        PhoneWindowManager* mHost;
    };

    class PowerLongPressRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::PowerLongPressRunnable")

        PowerLongPressRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class ScreenshotRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenshotRunnable")

        ScreenshotRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class ScreenshotForLogRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenshotForLogRunnable")

        ScreenshotForLogRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class BackLongPressRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::BackLongPressRunnable")

        BackLongPressRunnable(
            /* [in] */ PhoneWindowManager* host);

        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };


// Anonymous class in member functions.
private:
    class HomeDoubleTapTimeoutRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::HomeDoubleTapTimeoutRunnable")

        HomeDoubleTapTimeoutRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class CollapsePanelsRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::CollapsePanelsRunnable")

        CollapsePanelsRunnable(
            /* [in] */ PhoneWindowManager* host);

        virtual CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    class ScreenshotHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenshotHandler")

        ScreenshotHandler(
            /* [in] */ IServiceConnection *conn,
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IServiceConnection> myConn;
        PhoneWindowManager* mHost;
    };

    class ScreenshotServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::ScreenshotServiceConnection")

        ScreenshotServiceConnection(
            /* [in] */ PhoneWindowManager* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        PhoneWindowManager* mHost;
    };

    class UpdateSettingRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::UpdateSettingRunnable")

        UpdateSettingRunnable(
            /* [in] */ PhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
    };

    // This dialog will consume all events coming in to
    // it, to avoid it trying to do things too early in boot.
    class BootMsgDialog : public ProgressDialog
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::BootMsgDialog")

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 theme);

        // @Override
        CARAPI DispatchKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean *result);

        // @Override
        CARAPI DispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean *result);

        // @Override
        CARAPI DispatchTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean *result);

        // @Override
        CARAPI DispatchTrackballEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean *result);

        // @Override
        CARAPI DispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean *result);

        // @Override
        CARAPI DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean *result);
    };

    class BootMsgRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::BootMsgRunnable")

        BootMsgRunnable(
            /* [in] */ PhoneWindowManager* host,
            /* [in] */ ICharSequence* msg,
            /* [in] */ Int32 titleRes);

        // @Override
        CARAPI Run();

    private:
        PhoneWindowManager* mHost;
        AutoPtr<ICharSequence> mMsg;
        Int32 mTitleRes;
    };

    class UpdateSystemUiVisibilityRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("PhoneWindowManager::UpdateSystemUiVisibilityRunnable")

        UpdateSystemUiVisibilityRunnable(
            /* [in] */ Int32 visibility,
            /* [in] */ Boolean needsMenu,
            /* [in] */ PhoneWindowManager* host);

        virtual CARAPI Run();

    private:
        Int32 mVisibility;
        Boolean mNeedsMenu;
        PhoneWindowManager* mHost;
    };

public:
    static CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

public:
    CAR_INTERFACE_DECL()

    PhoneWindowManager();

    CARAPI constructor();

    CARAPI ShowGlobalActions();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IIWindowManager* windowManager,
        /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs);

    CARAPI SetInitialDisplaySize(
        /* [in] */ IDisplay* display,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density);

    CARAPI IsDefaultOrientationForced(
        /* [out] */ Boolean* result);

    CARAPI SetDisplayOverscan(
        /* [in] */ IDisplay* display,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI UpdateSettings();

    CARAPI CheckAddPermission(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ ArrayOf<Int32>* outAppOp,
        /* [out] */ Int32* addPermisssion);

    CARAPI CheckShowToOwnerOnly(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* result);

    CARAPI AdjustWindowParamsLw(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    CARAPI AdjustConfigurationLw(
        /* [in] */ IConfiguration* config,
        /* [in] */ Int32 keyboardPresence,
        /* [in] */ Int32 navigationPresence);

    CARAPI WindowTypeToLayerLw(
        /* [in] */ Int32 type,
        /* [out] */ Int32* layer);

    CARAPI SubWindowTypeToLayerLw(
        /* [in] */ Int32 type,
        /* [out] */ Int32* layer);

    CARAPI GetMaxWallpaperLayer(
        /* [out] */ Int32* layer);

    CARAPI GetAboveUniverseLayer(
        /* [out] */ Int32* layer);

    CARAPI GetNonDecorDisplayWidth(
        /* [in] */ Int32 fullWidth,
        /* [in] */ Int32 fullHeight,
        /* [in] */ Int32 rotation,
        /* [out] */ Int32* width);

    CARAPI GetNonDecorDisplayHeight(
        /* [in] */ Int32 fullWidth,
        /* [in] */ Int32 fullHeight,
        /* [in] */ Int32 rotation,
        /* [out] */ Int32* height);

    CARAPI GetConfigDisplayWidth(
        /* [in] */ Int32 fullWidth,
        /* [in] */ Int32 fullHeight,
        /* [in] */ Int32 rotation,
        /* [out] */ Int32* value);

    CARAPI GetConfigDisplayHeight(
        /* [in] */ Int32 fullWidth,
        /* [in] */ Int32 fullHeight,
        /* [in] */ Int32 rotation,
        /* [out] */ Int32* value);

    CARAPI IsForceHiding(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* result);

    CARAPI IsKeyguardHostWindow(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* result);

    CARAPI CanBeForceHidden(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* canHiden);

    CARAPI GetWinShowWhenLockedLw(
        /* [out] */ IWindowState** ws);

    CARAPI AddStartingWindow(
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 theme,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon,
        /* [in] */ Int32 logo,
        /* [in] */ Int32 windowFlags,
        /* [out] */ IView** startingWindow);

    CARAPI RemoveStartingWindow(
        /* [in] */ IBinder* appToken,
        /* [in] */ IView* window);

    CARAPI PrepareAddWindowLw(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Int32* added);

    CARAPI RemoveWindowLw(
        /* [in] */ IWindowState* win);

    CARAPI SelectAnimationLw(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 transit,
        /* [out] */ Int32* id);

    CARAPI SelectRotationAnimationLw(
        /* [in] */ ArrayOf<Int32>* anim);

    CARAPI ValidateRotationAnimationLw(
        /* [in] */ Int32 exitAnimId,
        /* [in] */ Int32 enterAnimId,
        /* [in] */ Boolean forceDefault,
        /* [out] */ Boolean* result);

    CARAPI CreateForceHideWallpaperExitAnimation(
        /* [in] */ Boolean goingToNotificationShade,
        /* [in] */ Boolean keyguardShowingMedia,
        /* [out] */ IAnimation** anim);

    CARAPI ShowRecentApps();

    CARAPI CreateForceHideEnterAnimation(
        /* [in] */ Boolean onWallpaper,
        /* [in] */ Boolean goingToNotificationShade,
        /* [out] */ IAnimation** anim);

    CARAPI InterceptKeyBeforeDispatching(
        /* [in] */ IWindowState* win,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int64* result);

    CARAPI DispatchUnhandledKey(
        /* [in] */ IWindowState* win,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ IKeyEvent** keyEvent);

    CARAPI AdjustSystemUiVisibilityLw(
        /* [in] */ Int32 visibility,
        /* [out] */ Int32* newVisibility);

    CARAPI GetContentInsetHintLw(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ IRect* contentInset);

    CARAPI BeginLayoutLw(
        /* [in] */ Boolean isDefaultDisplay,
        /* [in] */ Int32 displayWidth,
        /* [in] */ Int32 displayHeight,
        /* [in] */ Int32 displayRotation);

    CARAPI GetSystemDecorLayerLw(
        /* [out] */ Int32* layer);

    CARAPI GetContentRectLw(
        /* [in] */ IRect* r);

    CARAPI LayoutWindowLw(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowState* attached);

    CARAPI FinishLayoutLw();

    CARAPI BeginPostLayoutPolicyLw(
        /* [in] */ Int32 displayWidth,
        /* [in] */ Int32 displayHeight);

    CARAPI ApplyPostLayoutPolicyLw(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs);

    CARAPI FinishPostLayoutPolicyLw(
        /* [out] */ Int32* result);

    CARAPI AllowAppAnimationsLw(
        /* [out] */ Boolean* allowed);

    CARAPI FocusChangedLw(
        /* [in] */ IWindowState* lastFocus,
        /* [in] */ IWindowState* newFocus,
        /* [out] */ Int32* state);

    CARAPI NotifyLidSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lidOpen);

    CARAPI NotifyCameraLensCoverSwitchChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Boolean lensCovered);

    CARAPI InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int32* result);

    CARAPI InterceptMotionBeforeQueueingNonInteractive(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int32* result);

    CARAPI GoingToSleep(
        /* [in] */ Int32 why);

    CARAPI WakingUp();

    CARAPI ScreenTurnedOff();

    CARAPI ScreenTurningOn(
        /* [in] */ IScreenOnListener* screenOnListener);

    CARAPI IsScreenOn(
        /* [out] */ Boolean* result);

    CARAPI EnableKeyguard(
        /* [in] */ Boolean enabled);

    CARAPI ExitKeyguardSecurely(
       /* [in] */ IOnKeyguardExitResult* callback);

    CARAPI IsKeyguardLocked(
        /* [out] */ Boolean* state);

    CARAPI IsKeyguardSecure(
        /* [out] */ Boolean* state);

    CARAPI InKeyguardRestrictedKeyInputMode(
        /* [out] */ Boolean* restricted);

    CARAPI DismissKeyguardLw();

    CARAPI NotifyActivityDrawnForKeyguardLw();

    CARAPI IsKeyguardDrawnLw(
        /* [out] */ Boolean* result);

    CARAPI StartKeyguardExitAnimation(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    CARAPI RotationForOrientationLw(
        /* [in] */ Int32 orientation,
        /* [in] */ Int32 lastRotation,
        /* [out] */ Int32* surfaceRotation);

    CARAPI RotationHasCompatibleMetricsLw(
        /* [in] */ Int32 orientation,
        /* [in] */ Int32 rotation,
        /* [out] */ Boolean* result);

    CARAPI SetRotationLw(
        /* [in] */ Int32 rotation);

    CARAPI GetUserRotationMode(
        /* [out] */ Int32* result);

    // User rotation: to be used when all else fails in assigning an orientation to the device
    CARAPI SetUserRotationMode(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 rot);

    CARAPI SetSafeMode(
        /* [in] */ Boolean safeMode);

    CARAPI SystemReady();

    CARAPI SystemBooted();

    CARAPI ShowBootMessage(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean always);

    CARAPI HideBootMessages();

    CARAPI UserActivity();

    CARAPI LockNow(
        /* [in] */ IBundle* options);

    CARAPI EnableScreenAfterBoot();

    CARAPI SetCurrentOrientationLw(
        /* [in] */ Int32 newOrientation);

    CARAPI PerformHapticFeedbackLw(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 effectId,
        /* [in] */ Boolean always,
        /* [out] */ Boolean* isSucceed);

    CARAPI KeepScreenOnStartedLw();

    CARAPI KeepScreenOnStoppedLw();

    // Use this instead of checking config_showNavigationBar so that it can be consistently
    // overridden by qemu.hw.mainkeys in the emulator.
    CARAPI HasNavigationBar(
        /* [out] */ Boolean* has);

    CARAPI HasPermanentMenuKey(
            /* [out] */ Boolean* result);

    /**
    * Specifies whether the device needs a navigation bar (because it has no hardware buttons)
    */
    CARAPI NeedsNavigationBar(
        /* [out] */ Boolean* result);

    CARAPI SetLastInputMethodWindowLw(
        /* [in] */ IWindowState* ime,
        /* [in] */ IWindowState* target);

    CARAPI CanMagnifyWindow(
        /* [in] */ Int32 windowType,
        /* [out] */ Boolean* result);

    CARAPI IsTopLevelWindow(
        /* [in] */ Int32 windowType,
        /* [out] */ Boolean* result);

    CARAPI GetInputMethodWindowVisibleHeightLw(
        /* [out] */ Int32 *result);

    CARAPI SetCurrentUserLw(
        /* [in] */ Int32 newUserId);

    // @Override
    CARAPI_(void) Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    static AutoPtr<IIAudioService> GetAudioService();

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) GetLongIntArray(
        /* [in] */ IResources* r,
        /* [in] */ Int32 resid);

    static CARAPI_(AutoPtr<IIDreamManager>) GetDreamManager();

protected:
    CARAPI UpdateEdgeGestureListenerState();

    AutoPtr<IIStatusBarService> GetStatusBarService();

    /*
    * We always let the sensor be switched on by default except when
    * the user has explicitly disabled sensor based rotation or when the
    * screen is switched off.
    */
    CARAPI_(Boolean) NeedSensorRunningLp();

    /*
    * Various use cases for invoking this function
    * screen turning off, should always disable listeners if already enabled
    * screen turned on and current app has sensor based orientation, enable listeners
    * if not already enabled
    * screen turned on and current app does not have sensor orientation, disable listeners if
    * already enabled
    * screen turning on and current app has sensor based orientation, enable listeners if needed
    * screen turning on and current app has nosensor based orientation, do nothing
    */
    CARAPI_(void) UpdateOrientationListenerLp();

    CARAPI_(void) ShowGlobalActionsInternal();

    CARAPI_(Boolean) IsDeviceProvisioned();

    CARAPI_(Boolean) IsUserSetupComplete();

    CARAPI_(void) TriggerVirtualKeypress(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) LaunchCameraAction();

    CARAPI_(void) PerformKeyAction(
        /* [in] */ Int32 behavior);

    CARAPI_(void) ReadLidState();

    CARAPI_(Boolean) KeyguardOn();

    /**
    * A home key -> launch home action was detected.  Take the appropriate action
    * given the situation with the keyguard.
    */
    CARAPI_(void) LaunchHomeFromHotKey();

    CARAPI_(void) SetAttachedWindowFrames(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 fl,
        /* [in] */ Int32 sim,
        /* [in] */ IWindowState* attached,
        /* [in] */ Boolean insetDecors,
        /* [in] */ IRect* pf,
        /* [in] */ IRect* df,
        /* [in] */ IRect* of,
        /* [in] */ IRect* cf,
        /* [in] */ IRect* vf);

    CARAPI_(void) SetHdmiPlugged(
        /* [in] */ Boolean plugged);

    CARAPI_(void) InitializeHdmiState();

    CARAPI_(Boolean) IsMusicActive();

    CARAPI_(void) DispatchMediaKeyWithWakeLock(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) DispatchMediaKeyRepeatWithWakeLock(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) DispatchMediaKeyWithWakeLockToAudioService(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) LaunchVoiceAssistWithWakeLock(
        /* [in] */ Boolean keyguardActive);

    CARAPI_(void) SendCloseSystemWindows();

    CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ const String& reason);

    CARAPI_(void) UpdateUiMode();

    CARAPI_(void) UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration);

    CARAPI_(void) UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration,
        /* [in] */ Boolean forceRelayout);

    /**
     * Return an Intent to launch the currently active dock app as home.  Returns
     * null if the standard home should be launched, which is the case if any of the following is
     * true:
     * <ul>
     *  <li>The device is not in either car mode or desk mode
     *  <li>The device is in car mode but ENABLE_CAR_DOCK_HOME_CAPTURE is false
     *  <li>The device is in desk mode but ENABLE_DESK_DOCK_HOME_CAPTURE is false
     *  <li>The device is in car mode but there's no CAR_DOCK app with METADATA_DOCK_HOME
     *  <li>The device is in desk mode but there's no DESK_DOCK app with METADATA_DOCK_HOME
     * </ul>
     * @return A dock intent.
     */
    CARAPI_(AutoPtr<IIntent>) CreateHomeDockIntent();

    CARAPI_(void) StartDockOrHome();

    /**
    * goes to the home screen
    * @return whether it did anything
    */
    CARAPI_(Boolean) GoHome();

    CARAPI_(AutoPtr<ITelecomManager>) GetTelecommService();

private:
    CARAPI_(void) InterceptPowerKeyDown(
        /* [in] */ Boolean handled);

    CARAPI_(Boolean) InterceptPowerKeyUp(
        /* [in] */ Boolean canceled);

    CARAPI_(void) CancelPendingPowerKeyAction();

    CARAPI_(void) InterceptScreenshotChord();

    CARAPI_(void) InterceptScreenshotLog();

    CARAPI_(void) CancelPendingScreenshotForLog();

    CARAPI_(Int64) GetScreenshotChordLongPressDelay();

    CARAPI_(void) CancelPendingScreenshotChordAction();

    CARAPI_(void) PowerShortPress(
        /* [in] */ Int64 eventTime);

    CARAPI_(void) UpdateKeyAssignments();

    /**
     * @return whether the navigation bar can be hidden, e.g. the device has a
     *         navigation bar and touch exploration is not enabled
     */
    CARAPI_(Boolean) CanHideNavigationBar();

    CARAPI_(void) UpdateWakeGestureListenerLp();

    CARAPI_(Boolean) ShouldEnableWakeGestureLp();

    CARAPI_(void) ReadCameraLensCoverState();

    static CARAPI_(void) AwakenDreams();

    CARAPI_(void) EnablePointerLocation();

    CARAPI_(void) DisablePointerLocation();

    CARAPI_(Int32) ReadRotation(
        /* [in] */  Int32 resID);

    CARAPI_(Boolean) IsHidden(
        /* [in] */ Int32 accessibilityMode);

    CARAPI_(Boolean) UnpinActivity();

    CARAPI_(Boolean) InterceptFallback(
        /* [in] */ IWindowState* win,
        /* [in] */ IKeyEvent* fallbackEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) LaunchAssistLongPressAction();

    CARAPI_(void) LaunchAssistAction();

    CARAPI_(void) LaunchAssistAction(
        /* [in] */ const String& hint);

    CARAPI_(AutoPtr<ISearchManager>) GetSearchManager();

    CARAPI_(void) PreloadRecentApps();

    CARAPI_(void) CancelPreloadRecentApps();

    CARAPI_(void) ToggleRecentApps();

    CARAPI_(void) ShowRecentApps(
        /* [in] */ Boolean triggeredFromAltTab);

    CARAPI_(void) HideRecentApps(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHome);

    CARAPI_(void) ApplyForceImmersiveMode(
        /* [in] */ Int32 pfl,
        /* [in] */ IRect* r);

    CARAPI_(void) ApplyStableConstraints(
        /* [in] */ Int32 sysui,
        /* [in] */ Int32 fl,
        /* [in] */ IRect* r);

    CARAPI_(void) OffsetInputMethodWindowLw(
        /* [in] */ IWindowState* win);

    CARAPI_(void) OffsetVoiceInputWindowLw(
        /* [in] */ IWindowState* win);

    // Assume this is called from the Handler thread.
    CARAPI_(void) TakeScreenshot();

    CARAPI_(void) AcquireQuickBootWakeLock();

    CARAPI_(void) ReleaseQuickBootWakeLock();

    CARAPI_(void) ScheduleLongPressKeyEvent(
        /* [in] */ IKeyEvent* origEvent,
        /* [in] */ Int32 keyCode);

    /** When the screen is off we ignore some keys that might otherwise typically
     * be considered wake keys.  We filter them out here.
     *
     * {@link KeyEvent#KEYCODE_POWER} is notably absent from this list because it
     * is always considered a wake key.
     */
    CARAPI_(Boolean) IsWakeKeyWhenScreenOff(
        /* [in] */ Int32 keyCode);

    CARAPI_(Boolean) ShouldDispatchInputWhenNonInteractive();

    CARAPI_(void) RequestTransientBars(
        /* [in] */ IWindowState* swipeTarget);

    CARAPI_(void) DisableQbCharger();

    CARAPI_(void) FinishKeyguardDrawn();

    CARAPI_(void) FinishWindowsDrawn();

    CARAPI_(void) FinishScreenTurningOn();

    CARAPI_(void) HandleHideBootMessage();

    CARAPI_(void) ShowBootAnimation(
        /* [in] */ Int64 time);

    CARAPI_(Boolean) KeyguardIsShowingTq();

    CARAPI_(Boolean) IsKeyguardSecureIncludingHidden();

    CARAPI_(Boolean) IsLandscapeOrSeascape(
        /* [in] */ Int32 sensorRotation);

    CARAPI_(Boolean) IsAnyPortrait(
        /* [in] */ Int32 sensorRotation);

    CARAPI_(void) UpdateLockScreenTimeout();

    CARAPI_(void) ApplyLidSwitchState();

    CARAPI_(void) PerformAuditoryFeedbackForAccessibilityIfNeed();

    CARAPI_(Boolean) IsGlobalAccessibilityGestureEnabled();

    CARAPI_(Int32) UpdateSystemUiVisibilityLw();

    CARAPI_(Int32) UpdateSystemBarsLw(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 oldVis,
        /* [in] */ Int32 vis);

    CARAPI_(void) ClearClearableFlagsLw();

    CARAPI_(Boolean) IsImmersiveMode(
        /* [in] */ Int32 vis);

    /**
     * @return whether the navigation or status bar can be made translucent
     *
     * This should return true unless touch exploration is not enabled or
     * R.boolean.config_enableTranslucentDecor is false.
     */
    CARAPI_(Boolean) AreTranslucentBarsAllowed();

    /**
     * Processes the result code of {@link IKeyguardService#setOccluded}. This is needed because we
     * immediately need to put the wallpaper directly behind the Keyguard when a window with flag
     * {@link android.view.WindowManager.LayoutParams#FLAG_SHOW_WHEN_LOCKED} gets dismissed. If we
     * would wait for Keyguard to change the flags, that would be running asynchronously and thus be
     * too late so the user might see the window behind.
     *
     * @param setHiddenResult The result code from {@link IKeyguardService#setOccluded}.
     * @return Whether the flags have changed and we have to redo the layout.
     */
    CARAPI_(Boolean) ProcessKeyguardSetHiddenResultLw(
        /* [in] */ Int32 setHiddenResult);

    CARAPI_(Boolean) IsStatusBarKeyguard();

public:
    static String SYSTEM_DIALOG_REASON_KEY;
    static String SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS;
    static String SYSTEM_DIALOG_REASON_RECENT_APPS;
    static String SYSTEM_DIALOG_REASON_HOME_KEY;
    static String SYSTEM_DIALOG_REASON_ASSIST;

protected:
    static String TAG;
    static const Boolean DEBUG;
    static const Boolean localLOGV;
    static const Boolean DEBUG_LAYOUT;
    static const Boolean DEBUG_INPUT;
    static const Boolean DEBUG_STARTING_WINDOW;
    static const Boolean DEBUG_WAKEUP;
    static const Boolean SHOW_STARTING_ANIMATIONS;
    static const Boolean SHOW_PROCESSES_ON_ALT_MENU;

    // Whether to allow dock apps with METADATA_DOCK_HOME to temporarily take over the Home key.
    // No longer recommended for desk docks; still useful in car docks.
    static const Boolean ENABLE_CAR_DOCK_HOME_CAPTURE;// = true;
    static const Boolean ENABLE_DESK_DOCK_HOME_CAPTURE;// = false;

    // QuickBoot time settings
    static const Int32 DEFAULT_LONG_PRESS_POWERON_TIME; // = 500;
    static const Int32 QUICKBOOT_LAUNCH_TIMEOUT; // = 2000;

    static const Int32 SHORT_PRESS_POWER_NOTHING; // = 0;
    static const Int32 SHORT_PRESS_POWER_GO_TO_SLEEP; // = 1;
    static const Int32 SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP; // = 2;
    static const Int32 SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP_AND_GO_HOME; // = 3;

    static const Int32 LONG_PRESS_POWER_NOTHING;
    static const Int32 LONG_PRESS_POWER_GLOBAL_ACTIONS;
    static const Int32 LONG_PRESS_POWER_SHUT_OFF;
    static const Int32 LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM;

    // These need to match the documentation/constant in
    // core/res/res/values/config.xml
    static const Int32 APPLICATION_MEDIA_SUBLAYER;
    static const Int32 APPLICATION_MEDIA_OVERLAY_SUBLAYER;
    static const Int32 APPLICATION_PANEL_SUBLAYER;
    static const Int32 APPLICATION_SUB_PANEL_SUBLAYER;

    /**
     * These are the system UI flags that, when changing, can cause the layout
     * of the screen to change.
     */
    static const Int32 SYSTEM_UI_CHANGING_LAYOUT;

    /* Table of Application Launch keys.  Maps from key codes to Intent categories.
     *
     * These are special keys that are used to launch particular kinds of applications,
     * such as a web browser.  HID defines nearly a hundred of them in the Consumer (0x0C)
     * usage page.  We don't support quite that many yet...
     */
    static AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > sApplicationLaunchKeyCategories;

    static AutoPtr<CRect> mTmpParentFrame;
    static AutoPtr<CRect> mTmpDisplayFrame;
    static AutoPtr<CRect> mTmpOverscanFrame;
    static AutoPtr<CRect> mTmpContentFrame;
    static AutoPtr<CRect> mTmpVisibleFrame;
    static AutoPtr<CRect> mTmpDecorFrame;
    static AutoPtr<CRect> mTmpStableFrame;
    static AutoPtr<CRect> mTmpNavigationFrame;

    static const Boolean PRINT_ANIM;

    // Java has not these two variable.
    //static Boolean mVolumeMute;
    //static Int32 mPreVolume;

    AutoPtr<IRunnable> mWindowManagerDrawCallback;
    AutoPtr<IKeyguardServiceDelegateShowListener> mKeyguardDelegateCallback;
    AutoPtr<HideNavInputEventReceiverFactory> mHideNavInputEventReceiverFactory;
    AutoPtr<ScreenshotTimeoutRunnable> mScreenshotTimeout;
    AutoPtr<DockBroadReceiver> mDockReceiver;
    AutoPtr<DreamBroadReceiver> mDreamReceiver;
    AutoPtr<MultiuserBroadReceiver> mMultiuserReceiver;
    AutoPtr<WifiDisplayReceiver> mWifiDisplayReceiver;
    AutoPtr<ScreenLockTimeoutRunnable> mScreenLockTimeout;
    AutoPtr<QuickBootPowerLongPressRunnable> mQuickBootPowerLongPress;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IWindowManagerPolicyWindowManagerFuncs> mWindowManagerFuncs;
    AutoPtr<IWindowManagerInternal> mWindowManagerInternal;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IDreamManagerInternal> mDreamManagerInternal;
    AutoPtr<IIStatusBarService> mStatusBarService;
    Boolean mPreloadedRecentApps;
    Object mServiceAquireLock;
    AutoPtr<IVibrator> mVibrator; // Vibrator for giving feedback of orientation changes
    AutoPtr<ISearchManager> mSearchManager;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    // Vibrator pattern for haptic feedback of a long press.
    AutoPtr<ArrayOf<Int64> > mLongPressVibePattern;

    // Vibrator pattern for haptic feedback of virtual key press.
    AutoPtr<ArrayOf<Int64> > mVirtualKeyVibePattern;

    // Vibrator pattern for a short vibration.
    AutoPtr<ArrayOf<Int64> > mKeyboardTapVibePattern;

    // Vibrator pattern for a short vibration when tapping on an hour/minute tick of a Clock.
    AutoPtr<ArrayOf<Int64> > mClockTickVibePattern;

    // Vibrator pattern for a short vibration when tapping on a day/month/year date of a Calendar.
    AutoPtr<ArrayOf<Int64> > mCalendarDateVibePattern;

    // Vibrator pattern for haptic feedback during boot when safe mode is disabled.
    AutoPtr<ArrayOf<Int64> > mSafeModeDisabledVibePattern;

    // Vibrator pattern for haptic feedback during boot when safe mode is enabled.
    AutoPtr<ArrayOf<Int64> > mSafeModeEnabledVibePattern;

    /** If true, hitting shift & menu will broadcast Intent.ACTION_BUG_REPORT */
    Boolean mEnableShiftMenuBugReports;

    Boolean mSafeMode;
    AutoPtr<IWindowState> mStatusBar;
    Int32 mStatusBarHeight;
    AutoPtr<IWindowState> mNavigationBar;
    Boolean mHasNavigationBar;
    Boolean mCanHideNavigationBar;
    Boolean mNavigationBarCanMove; // can the navigation bar ever move to the side?
    Boolean mNavigationBarOnBottom; // is the navigation bar on the bottom *right now*?
    Boolean mNavigationBarLeftInLandscape; // Navigation bar left handed?
    Int32 mNavigationBarHeightForRotation[4];
    Int32 mNavigationBarWidthForRotation[4];

    Boolean mBootMessageNeedsHiding;
    AutoPtr<IKeyguardServiceDelegate> mKeyguardDelegate;
    AutoPtr<GlobalActions> mGlobalActions;
    volatile Boolean mPowerKeyHandled; // accessed from input reader and handler thread
    Boolean mPendingPowerKeyUpCanceled;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IWindowState> mLastInputMethodWindow;
    AutoPtr<IWindowState> mLastInputMethodTargetWindow;

    Boolean mRecentsVisible;
    Int32 mRecentAppsHeldModifiers;
    Boolean mLanguageSwitchKeyPressed;

    Int32 mLidState;
    Int32 mCameraLensCoverState;// = CAMERA_LENS_COVER_ABSENT;
    Boolean mHaveBuiltInKeyboard;

    Boolean mSystemReady;
    Boolean mSystemBooted;
    Boolean mHdmiPlugged;
    AutoPtr<IIUiModeManager> mUiModeManager;
    Int32 mUiMode;
    Int32 mDockMode;
    Int32 mLidOpenRotation;
    Boolean mHasRemovableLid;
    Int32 mCarDockRotation;
    Int32 mDeskDockRotation;
    Int32 mUndockedHdmiRotation;
    Int32 mDemoHdmiRotation;
    Boolean mDemoHdmiRotationLock;

    Boolean mWakeGestureEnabledSetting;
    AutoPtr<MyWakeGestureListener> mWakeGestureListener;

    Int32 mUserRotationMode;
    Int32 mUserRotation;
    Int32 mUserRotationAngles;
    Boolean mAccelerometerDefault;

    Boolean mSupportAutoRotation;
    Int32 mAllowAllRotations;
    Boolean mCarDockEnablesAccelerometer;
    Boolean mDeskDockEnablesAccelerometer;
    Int32 mLidKeyboardAccessibility;
    Int32 mLidNavigationAccessibility;
    Boolean mLidControlsSleep;
    Int32 mShortPressOnPowerBehavior;// = -1;
    Int32 mLongPressOnPowerBehavior;
    Boolean mAwake;
    Boolean mScreenOnEarly;
    Boolean mScreenOnFully;
    AutoPtr<IScreenOnListener> mScreenOnListener;
    Boolean mKeyguardDrawComplete;
    Boolean mWindowManagerDrawComplete;
    Boolean mOrientationSensorEnabled;
    Int32 mCurrentAppOrientation;
    Boolean mHasSoftInput;
    Boolean mTranslucentDecorEnabled;// = true;
    Int32 mBackKillTimeout;

    Int32 mDeviceHardwareKeys;

    // Button wake control flags
    Boolean mHomeWakeScreen;
    Boolean mBackWakeScreen;
    Boolean mMenuWakeScreen;
    Boolean mAssistWakeScreen;
    Boolean mAppSwitchWakeScreen;
    Boolean mVolumeWakeScreen;

    // During wakeup by volume keys, we still need to capture subsequent events
    // until the key is released. This is required since the beep sound is produced
    // post keypressed.
    Boolean mVolumeWakeTriggered;

    Int32 mPointerLocationMode; // guarded by mLock

    Int32 mLongPressPoweronTime; //DEFAULT_LONG_PRESS_POWERON_TIME;
    // The last window we were told about in focusChanged.
    AutoPtr<IWindowState> mFocusedWindow;
    AutoPtr<IApplicationToken> mFocusedApp;

    // Behavior of volbtn music controls
    Boolean mVolBtnMusicControls;
    Boolean mIsLongPress;

    // Pointer location view state, only modified on the mHandler Looper.
    AutoPtr<IPointerLocationView> mPointerLocationView;
    // The current size of the screen; really; extends into the overscan area of
    // the screen and doesn't account for any system elements like the status bar.
    Int32 mOverscanScreenLeft, mOverscanScreenTop;
    Int32 mOverscanScreenWidth, mOverscanScreenHeight;

    // The current visible size of the screen; really; (ir)regardless of whether the status
    // bar can be hidden but not extending into the overscan area.
    Int32 mUnrestrictedScreenLeft, mUnrestrictedScreenTop;
    Int32 mUnrestrictedScreenWidth, mUnrestrictedScreenHeight;
    // Like mOverscanScreen*, but allowed to move into the overscan region where appropriate.
    Int32 mRestrictedOverscanScreenLeft, mRestrictedOverscanScreenTop;
    Int32 mRestrictedOverscanScreenWidth, mRestrictedOverscanScreenHeight;
    // The current size of the screen; these may be different than (0,0)-(dw,dh)
    // if the status bar can't be hidden; in that case it effectively carves out
    // that area of the display from all other windows.
    Int32 mRestrictedScreenLeft, mRestrictedScreenTop;
    Int32 mRestrictedScreenWidth, mRestrictedScreenHeight;
    // During layout, the current screen borders accounting for any currently
    // visible system UI elements.
    Int32 mSystemLeft, mSystemTop, mSystemRight, mSystemBottom;
    // For applications requesting stable content insets, these are them.
    Int32 mStableLeft, mStableTop, mStableRight, mStableBottom;
    // For applications requesting stable content insets but have also set the
    // fullscreen window flag, these are the stable dimensions without the status bar.
    Int32 mStableFullscreenLeft, mStableFullscreenTop;
    Int32 mStableFullscreenRight, mStableFullscreenBottom;
    // For force immersive mode
    Int32 mForceImmersiveLeft, mForceImmersiveTop;
    Int32 mForceImmersiveRight, mForceImmersiveBottom;
    // During layout, the current screen borders with all outer decoration
    // (status bar, input method dock) accounted for.
    Int32 mCurLeft, mCurTop, mCurRight, mCurBottom;
    // During layout, the frame in which content should be displayed
    // to the user, accounting for all screen decoration except for any
    // space they deem as available for other content.  This is usually
    // the same as mCur*, but may be larger if the screen decor has supplied
    // content insets.
    Int32 mContentLeft, mContentTop, mContentRight, mContentBottom;
    // During layout, the frame in which voice content should be displayed
    // to the user, accounting for all screen decoration except for any
    // space they deem as available for other content.
    Int32 mVoiceContentLeft, mVoiceContentTop, mVoiceContentRight, mVoiceContentBottom;
    // During layout, the current screen borders along which input method
    // windows are placed.
    Int32 mDockLeft, mDockTop, mDockRight, mDockBottom;
    // During layout, the layer at which the doc window is placed.
    Int32 mDockLayer;
    // During layout, this is the layer of the status bar.
    Int32 mStatusBarLayer;
    Int32 mLastSystemUiFlags;
    // Bits that we are in the process of clearing, so we want to prevent
    // them from being set by applications until everything has been updated
    // to have them clear.
    Int32 mResettingSystemUiFlags;
    // Bits that we are currently always keeping cleared.
    Int32 mForceClearedSystemUiFlags;
    // What we last reported to system UI about whether the compatibility
    // menu needs to be displayed.
    Boolean mLastFocusNeedsMenu;

    AutoPtr<IFakeWindow> mHideNavFakeWindow;

    AutoPtr<IWindowState> mTopFullscreenOpaqueWindowState;
    AutoPtr<IHashSet> mAppsToBeHidden;//IApplicationToken
    AutoPtr<IHashSet> mAppsThatDismissKeyguard;//IApplicationToken
    Boolean mTopIsFullscreen;
    Boolean mForceStatusBar;
    Boolean mForceStatusBarFromKeyguard;
    Boolean mHideLockScreen;
    Boolean mForcingShowNavBar;
    Int32 mForcingShowNavBarLayer;

    Boolean mDevForceNavbar;

    Int32 mDismissKeyguard;

    Boolean mShowingLockscreen;
    Boolean mShowingDream;
    Boolean mDreamingLockscreen;
    Boolean mHomePressed;
    Boolean mHomeConsumed;
    Boolean mHomeDoubleTapPending;
    Boolean mMenuPressed;
    Boolean mAppSwitchLongPressed;
    AutoPtr<IIntent> mHomeIntent;
    AutoPtr<IIntent> mCarDockIntent;
    AutoPtr<IIntent> mDeskDockIntent;
    Boolean mSearchKeyShortcutPending;
    Boolean mConsumeSearchKeyUp;
    Boolean mAssistKeyLongPressed;
    Boolean mPendingMetaAction;

    // Tracks user-customisable behavior for certain key events
    Int32 mLongPressOnHomeBehavior;// = -1;
    Int32 mPressOnMenuBehavior;// = -1;
    Int32 mLongPressOnMenuBehavior;// = -1;
    Int32 mPressOnAssistBehavior;// = -1;
    Int32 mLongPressOnAssistBehavior;// = -1;
    Int32 mPressOnAppSwitchBehavior;// = -1;
    Int32 mLongPressOnAppSwitchBehavior;// = -1;

    // support for activating the lock screen while the screen is on
    Boolean mAllowLockscreenWhenOn;
    Int32 mLockScreenTimeout;
    Boolean mLockScreenTimerActive;

    // Behavior of ENDCALL Button.  (See Settings.System.END_BUTTON_BEHAVIOR.)
    Int32 mEndcallBehavior;

    // Behavior of POWER button while in-call and screen on.
    // (See Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR.)
    Int32 mIncallPowerBehavior;

    // Behavior of HOME button during incomming call ring.
    // (See Settings.Secure.RING_HOME_BUTTON_BEHAVIOR.)
    Int32 mRingHomeBehavior;

    AutoPtr<IDisplay> mDisplay;

    Int32 mLandscapeRotation;  // default landscape rotation
    Int32 mSeascapeRotation;   // "other" landscape rotation, 180 degrees from mLandscapeRotation
    Int32 mPortraitRotation;   // default portrait rotation
    Int32 mUpsideDownRotation; // "other" portrait rotation

    Int32 mOverscanLeft;// = 0;
    Int32 mOverscanTop;// = 0;
    Int32 mOverscanRight;// = 0;
    Int32 mOverscanBottom;// = 0;

    // What we do when the user double-taps on home
    Int32 mDoubleTapOnHomeBehavior;

    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<ShortcutManager> mShortcutManager;
    AutoPtr<IPowerManagerWakeLock> mBroadcastWakeLock;
    AutoPtr<IPowerManagerWakeLock> mQuickBootWakeLock;
    Boolean mHavePendingMediaKeyRepeatWithWakeLock;

    AutoPtr<MyOrientationListener> mOrientationListener;

    Object mScreenshotLock;
    AutoPtr<IServiceConnection> mScreenshotConnection;

    AutoPtr<BootMsgDialog> mBootMsgDialog;

private:
    /** Amount of time (in milliseconds) to wait for windows drawn before powering on. */
    static const Int32 WAITING_FOR_DRAWN_TIMEOUT;// = 1000;

    // States of keyguard dismiss.
    static const Int32 DISMISS_KEYGUARD_NONE; // Keyguard not being dismissed.
    static const Int32 DISMISS_KEYGUARD_START; // Keyguard needs to be dismissed.
    static const Int32 DISMISS_KEYGUARD_CONTINUE; // Keyguard has been dismissed.

    // Time to volume and power must be pressed within this interval of each other.
    static const Int64 SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS;
    // Increase the chord delay when taking a screenshot from the keyguard
    static const Float KEYGUARD_SCREENSHOT_CHORD_DELAY_MULTIPLIER;

    static const Int32 MSG_ENABLE_POINTER_LOCATION;
    static const Int32 MSG_DISABLE_POINTER_LOCATION;
    static const Int32 MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK;
    static const Int32 MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK;
    static const Int32 MSG_KEYGUARD_DRAWN_COMPLETE;// = 5;
    static const Int32 MSG_KEYGUARD_DRAWN_TIMEOUT;// = 6;
    static const Int32 MSG_WINDOW_MANAGER_DRAWN_COMPLETE;// = 7;
    static const Int32 MSG_DISPATCH_SHOW_RECENTS;// = 9;
    static const Int32 MSG_DISPATCH_SHOW_GLOBAL_ACTIONS;// = 10;
    static const Int32 MSG_HIDE_BOOT_MESSAGE;// = 11;
    static const Int32 MSG_LAUNCH_VOICE_ASSIST_WITH_WAKE_LOCK;// = 12;
    static const Int32 MSG_DISPATCH_VOLKEY_WITH_WAKE_LOCK;// = 13;

    // Available custom actions to perform on a key press.
    // Must match values for KEY_HOME_LONG_PRESS_ACTION in:
    // core/java/android/provider/Settings.java
    static const Int32 KEY_ACTION_NOTHING; // = 0;
    static const Int32 KEY_ACTION_MENU; // = 1;
    static const Int32 KEY_ACTION_APP_SWITCH; // = 2;
    static const Int32 KEY_ACTION_SEARCH; // = 3;
    static const Int32 KEY_ACTION_VOICE_SEARCH; // = 4;
    static const Int32 KEY_ACTION_IN_APP_SEARCH; // = 5;
    static const Int32 KEY_ACTION_LAUNCH_CAMERA; // = 6;
    static const Int32 KEY_ACTION_SLEEP; // = 7;
    static const Int32 KEY_ACTION_LAST_APP; // = 8;

    // Masks for checking presence of hardware keys.
    // Must match values in core/res/res/values/config.xml
    static const Int32 KEY_MASK_HOME; // = 0x01;
    static const Int32 KEY_MASK_BACK; // = 0x02;
    static const Int32 KEY_MASK_MENU; // = 0x04;
    static const Int32 KEY_MASK_ASSIST; // = 0x08;
    static const Int32 KEY_MASK_APP_SWITCH; // = 0x10;
    static const Int32 KEY_MASK_CAMERA; // = 0x20;
    static const Int32 KEY_MASK_VOLUME; // = 0x40;

    /* The number of steps between min and max brightness */
    static const Int32 BRIGHTNESS_STEPS;// = 10;

    static AutoPtr<ArrayOf<Int32> > WINDOW_TYPES_WHERE_HOME_DOESNT_WORK;
    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;

// Need to instantiate in the Constructor.
private:
    AutoPtr<HDMIUEventObserver> mHDMIObserver;
    AutoPtr<PowerLongPressRunnable> mPowerLongPress;
    AutoPtr<ScreenshotRunnable> mScreenshotRunnable;
    AutoPtr<ScreenshotForLogRunnable> mScreenshotForLog;

    AutoPtr<BackLongPressRunnable> mBackLongPress;


private:
    AutoPtr<IDeviceKeyHandler> mDeviceKeyHandler;

    /**
     * Lock protecting Internal state.  Must not call out Into window
     * manager with lock held.  (This lock will be acquired in places
     * where the window manager is calling in with its own lock held.)
     */
    Object mLock;
    Object mQuickBootLock;

    /**
     * Keyguard stuff
     */
    AutoPtr<IWindowState> mKeyguardScrim;
    Boolean mKeyguardHidden;
    Boolean mKeyguardDrawnOnce;
    /** The window that is currently dismissing the keyguard. Dismissing the keyguard must only
     * be done once per window. */
    AutoPtr<IWindowState> mWinDismissingKeyguard;

    /** The window that is currently showing "over" the keyguard. If there is an app window
     * belonging to another app on top of this the keyguard shows. If there is a fullscreen
     * app window under this, still dismiss the keyguard but don't show the app underneath. Show
     * the wallpaper. */
    AutoPtr<IWindowState> mWinShowWhenLocked;

    // Screenshot trigger states
    Boolean mScreenshotChordEnabled;
    Boolean mVolumeDownKeyTriggered;
    Int64 mVolumeDownKeyTime;
    Boolean mVolumeDownKeyConsumedByScreenshotChord;
    Boolean mVolumeUpKeyTriggered;
    Boolean mPowerKeyTriggered;
    Int64 mVolumeUpKeyTime;
    Boolean mVolumeUpKeyConsumedByScreenshotChord;
    Int64 mPowerKeyTime;

    Int32 mCurrentUserId;

    // Maps global key codes to the components that will handle them.
    AutoPtr<GlobalKeyManager> mGlobalKeyManager;

    Boolean mWifiDisplayConnected;// = false;
    Int32     mWifiDisplayCustomRotation;// = -1;
    Boolean mHasPermanentMenuKey;
    Boolean mClearedBecauseOfForceShow;
    Boolean mTopWindowIsKeyguard;

    // Fallback actions by key code.
    AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > mFallbackActions;//TODO update to SparseArray
    AutoPtr<ILogDecelerateInterpolator> mLogDecelerateInterpolator;// = new LogDecelerateInterpolator(100, 0);

    // Default display does not rotate, apps that require non-default orientation will have to
    // have the orientation emulated.
    Boolean mForceDefaultOrientation;// = false;
    AutoPtr<IBarController> mStatusBarController;
    AutoPtr<IBarController> mNavigationBarController;
    AutoPtr<IImmersiveModeConfirmation> mImmersiveModeConfirmation;
    AutoPtr<ISystemGesturesPointerEventListener> mSystemGestures;
    AutoPtr<IRunnable> mHomeDoubleTapTimeoutRunnable;
    AutoPtr<IRunnable> mClearHideNavigationFlag;
    AutoPtr<IRunnable> mRequestTransientNav;

    AutoPtr<IEdgeGestureActivationListener> mEdgeGestureActivationListener;
    AutoPtr<IEdgeGestureManager> mEdgeGestureManager;

    Int32 mLastEdgePositions;// = 0;
    Boolean mEdgeListenerActivated;// = false;
    Boolean mUsingEdgeGestureServiceForGestures;// = false;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOWMANAGER_H__
