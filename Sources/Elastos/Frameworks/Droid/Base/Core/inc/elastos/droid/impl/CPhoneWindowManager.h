
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWMANAGER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneWindowManager.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRect.h"
#include "impl/BootAnimationView.h"
#include "impl/RecentApplicationsDialog.h"
#include "impl/ShortcutManager.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/utility/SparseArray.h"
#include "elastos/droid/view/InputEventReceiver.h"
#include <elastos/utility/etl/HashMap.h>
#include <impl/GlobalActions.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashMap;

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::IStatusBarManager;
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
using Elastos::Droid::Internal::Policy::Impl::BootAnimationView;
using Elastos::Droid::Internal::Policy::Impl::GlobalActions;
using Elastos::Droid::Internal::Policy::Impl::ShortcutManager;
using Elastos::Droid::Internal::Policy::Impl::IRecentApplicationsDialog;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::Utility::SparseArray;
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
using Elastos::Droid::View::InputEventReceiver;
using Elastos::Droid::View::IOnKeyguardExitResult;
using Elastos::Droid::View::IScreenOnListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManagerPolicyWindowManagerFuncs;
using Elastos::Droid::View::IWindowState;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::Widget::Internal::IPointerLocationView;
using Elastos::Droid::Widget::IToast;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneWindowManager)
{
protected:
    class PromptEnterMouseModeRunnable
        : public Runnable {
    public:
        PromptEnterMouseModeRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class PromptExitMouseModeRunnable
        : public Runnable {
    public:
        PromptExitMouseModeRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI_(void) Observe();

    private:
        CPhoneWindowManager* mHost;
    };

    // TODO: WindowOrientationListener is not implement.
    class MyOrientationListener
        // : public WindowOrientationListener
    {
    public:
        MyOrientationListener(
            /* [in] */ IContext* context,
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnProposedRotationChanged(
            /* [in] */ Int32 rotation);

    private:
        CPhoneWindowManager* mHost;
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
        HideNavInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnInputEvent(
            /* [in] */ IInputEvent* event);

    private:
        CPhoneWindowManager* mHost;
    };

    class HideNavInputEventReceiverFactory
        : public IInputEventReceiverFactory
        , public ElRefBase
    {
    public:
        HideNavInputEventReceiverFactory(
            /* [in] */ CPhoneWindowManager* host);

        CAR_INTERFACE_DECL();

        // @Override
        CARAPI CreateInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [out] */ IInputEventReceiver** receiver);

    private:
        CPhoneWindowManager* mHost;
    };

    class ScreenshotTimeoutRunnable
        : public Runnable
    {
    public:
        ScreenshotTimeoutRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class DockBroadReceiver
        : public BroadcastReceiver
    {
    public:
        DockBroadReceiver(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CPhoneWindowManager::DockBroadReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CPhoneWindowManager* mHost;
    };

    class DreamBroadReceiver
        : public BroadcastReceiver
    {
    public:
        DreamBroadReceiver(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CPhoneWindowManager::DreamBroadReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CPhoneWindowManager* mHost;
    };

    class MultiuserBroadReceiver
        : public BroadcastReceiver
    {
    public:
        MultiuserBroadReceiver(
            /*in*/ CPhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CPhoneWindowManager::MultiuserBroadReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CPhoneWindowManager* mHost;
    };

    class ScreenLockTimeoutRunnable
        : public Runnable
        , public Object
    {
    public:
        ScreenLockTimeoutRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

        CARAPI_(void) SetLockOptions(
            /* [in] */ IBundle* options);

    protected:
        AutoPtr<IBundle> mOptions;
        CPhoneWindowManager* mHost;
    };

private:
    class PointerLocationInputEventReceiver
        : public InputEventReceiver
    {
    public:
        PointerLocationInputEventReceiver(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ IPointerLocationView* view);

        // @Override
        CARAPI OnInputEvent(
            /* [in] */ IInputEvent* event);

    private:
        const AutoPtr<IPointerLocationView> mView;
    };

    class PolicyHandler
        : public HandlerBase
    {
    public:
        PolicyHandler(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CPhoneWindowManager* mHost;
    };

    class HDMIUEventObserver
        : public UEventObserver
    {
    public:
        HDMIUEventObserver(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);

    private:
        CPhoneWindowManager* mHost;
    };

    class FlickerIntentBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        FlickerIntentBroadcastReceiver(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CPhoneWindowManager::FlickerIntentBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CPhoneWindowManager* mHost;
    };

    class Step1OnRunnable
        : public Runnable
    {
    public:
        Step1OnRunnable(
            /* [in] */ CPhoneWindowManager* mHost);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class Step2OffRunnable
        : public Runnable
    {
    public:
        Step2OffRunnable(
        /* [in] */ CPhoneWindowManager* mHost);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class Step3OnRunnable
        : public Runnable
    {
    public:
        Step3OnRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class BootFastPowerLongPressRunnable
        : public Runnable
    {
    public:
        BootFastPowerLongPressRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class PowerLongPressRunnable
        : public Runnable
    {
    public:
        PowerLongPressRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class ScreenshotChordLongPressRunnable
        : public Runnable
    {
    public:
        ScreenshotChordLongPressRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };


// Anonymous class in member functions.
private:
    class ShowInitLogoRunnable
        : public Runnable
    {
    public:
        ShowInitLogoRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class HideScreenRunnable
        : public Runnable
    {
    public:
        HideScreenRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class HideBootAnimationRunnable
        : public Runnable
    {
    public:
        HideBootAnimationRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class ShowOrHideRecentAppsDialogRunnable
        : public Runnable
    {
    public:
        ShowOrHideRecentAppsDialogRunnable(
            /* [in] */ CPhoneWindowManager* host,
            /* [in] */ Int32 behavior);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
        Int32 mBehavior;
    };

    class ReevaluateRunnable
        : public Runnable
    {
    public:
        ReevaluateRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class CollapsePanelsRunnable
        : public Runnable
    {
    public:
        CollapsePanelsRunnable(
            /* [in] */ CPhoneWindowManager* host);

        virtual CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    // TODO: KeyguardDoneRunnable, because KeyguardMediator is not implement.

    // TODO: KeyguardMediatorDismissRunnable, because KeyguardMediator is not implement.

    class ScreenshotHandler
        : public HandlerBase
    {
    public:
        ScreenshotHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ IServiceConnection *conn,
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IServiceConnection> myConn;
        CPhoneWindowManager* mHost;
    };

    class ScreenshotServiceConnection
        : public IServiceConnection
        , public ElRefBase
    {
    public:
        ScreenshotServiceConnection(
            /* [in] */ CPhoneWindowManager* host);

        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CPhoneWindowManager* mHost;
    };

    // TODO: WaitForKeyguardWindowDrawnRunnable, because KeyguardMediator is not implement.

    // TODO: FinishScreenTurningOnRunnable, because RemoteCallback is not implement.

    class AcquireBootRunnable
        : public Runnable
    {
    public:
        AcquireBootRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class ReleaseBootAVRunnable
        : public Runnable
    {
    public:
        ReleaseBootAVRunnable(
            /* [in] */ CPhoneWindowManager* host);

        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    // TODO: DismissKeyguardLwRunnable, because KeyguardMediator is not implement.

    class UpdateSettingRunnable
        : public Runnable
    {
    public:
        UpdateSettingRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    // This dialog will consume all events coming in to
    // it, to avoid it trying to do things too early in boot.
    class _BootMsgDialog : public ProgressDialog
    {
    public:
        CARAPI Init(
            /* [in] */ IContext* context);

        // @Override
        CARAPI_(Boolean) DispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI_(Boolean) DispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI_(Boolean) DispatchTouchEvent(
            /* [in] */ IMotionEvent* ev);

        // @Override
        CARAPI_(Boolean) DispatchTrackballEvent(
            /* [in] */ IMotionEvent* ev);

        // @Override
        CARAPI_(Boolean) DispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* ev);

        // @Override
        CARAPI_(Boolean) DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);
    };

    class BootMsgDialog
        : public _BootMsgDialog
        , public IProgressDialog
        , public IWindowCallback
        , public IKeyEventCallback
        , public IViewOnCreateContextMenuListener
        , public ElRefBase
    {
    public:
        IPROGRESSDIALOG_METHODS_DECL();

        IALERTDIALOG_METHODS_DECL();

        IDIALOG_METHODS_DECL();

        IWINDOWCALLBACK_METHODS_DECL();

        IKEYEVENTCALLBACK_METHODS_DECL();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnCreateContextMenu(
            /* [in] */ IContextMenu* menu,
            /* [in] */ IView* v,
            /* [in] */ IContextMenuInfo* menuInfo);
    };

    class BootMsgRunnable
        : public Runnable
    {
    public:
        BootMsgRunnable(
            /* [in] */ CPhoneWindowManager* host,
            /* [in] */ ICharSequence* msg);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
        AutoPtr<ICharSequence> mMsg;
    };

    class BootMsgDismissRunnable
        : public Runnable
    {
    public:
        BootMsgDismissRunnable(
            /* [in] */ CPhoneWindowManager* host);

        // @Override
        CARAPI Run();

    private:
        CPhoneWindowManager* mHost;
    };

    class UpdateSystemUiVisibilityRunnable
        : public Runnable
    {
    public:
        UpdateSystemUiVisibilityRunnable(
            /* [in] */ Int32 visibility,
            /* [in] */ Boolean needsMenu,
            /* [in] */ CPhoneWindowManager* host);

        virtual CARAPI Run();

    private:
        Int32 mVisibility;
        Boolean mNeedsMenu;
        CPhoneWindowManager* mHost;
    };

public:
    static CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

public:
    CPhoneWindowManager();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IIWindowManager* windowManager,
        /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs);

    CARAPI SetInitialDisplaySize(
        /* [in] */ IDisplay* display,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density);

    CARAPI_(void) UpdateSettings();

    CARAPI CheckAddPermission(
        /* [in] */ IWindowManagerLayoutParams* attrs,
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

    CARAPI HasSystemNavBar(
        /* [out] */ Boolean* has);

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

    CARAPI DoesForceHide(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* hiden);

    CARAPI CanBeForceHidden(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* canHiden);

    CARAPI AddStartingWindow(
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 theme,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon,
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

    CARAPI CreateForceHideEnterAnimation(
        /* [in] */ Boolean onWallpaper,
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

    CARAPI GetSystemDecorRectLw(
        /* [in] */ IRect* systemRect,
        /* [out] */ Int32* layer);

    CARAPI LayoutWindowLw(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
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

    CARAPI InterceptKeyBeforeQueueing(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [in] */ Boolean isScreenOn,
        /* [out] */ Int32* result);

    CARAPI InterceptMotionBeforeQueueingWhenScreenOff(
        /* [in] */ Int32 policyFlags,
        /* [out] */ Int32* result);

    CARAPI ScreenTurnedOff(
        /* [in] */ Int32 why);

    CARAPI ScreenTurningOn(
        /* [in] */ IScreenOnListener* screenOnListener);

    CARAPI IsScreenOnEarly(
        /* [out] */ Boolean* state);

    CARAPI IsScreenOnFully(
        /* [out] */ Boolean* state);

    CARAPI AcquireBAView();

    CARAPI ReleaseBAView();

    CARAPI HideScreen(
        /* [in] */ Boolean enable);

    CARAPI ShowBootInitLogo(
        /* [in] */ Int32 logo);

    CARAPI ShowPowerCharge(
        /* [in] */ Int32 precent);

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

    CARAPI SetLastInputMethodWindowLw(
        /* [in] */ IWindowState* ime,
        /* [in] */ IWindowState* target);

    CARAPI CanMagnifyWindowLw(
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Boolean* result);

    CARAPI SetCurrentUserLw(
        /* [in] */ Int32 newUserId);

    CARAPI ShowAssistant();

    // @Override
    CARAPI_(void) Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // TODO: Telephone is not implement.
    //static ITelephony GetTelephonyService();

    static AutoPtr<IIAudioService> GetAudioService();

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) GetLongIntArray(
        /* [in] */ IResources* r,
        /* [in] */ Int32 resid);

protected:
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

    CARAPI_(void) ShowGlobalActionsDialog();

    CARAPI_(Boolean) IsDeviceProvisioned();

    /**
     * Create (if necessary) and show or dismiss the recent apps dialog according
     * according to the requested behavior.
     */
    CARAPI_(void) ShowOrHideRecentAppsDialog(
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
        /* [in] */ IRect* cf,
        /* [in] */ IRect* vf);

    CARAPI_(void) SetHdmiPlugged(
        /* [in] */ Boolean plugged);

    CARAPI_(void) InitializeHdmiState();

    /**
    * @return Whether music is being played right now.
    */
    CARAPI_(Boolean) IsMusicActive();

    /**
    * Tell the audio service to adjust the volume appropriate to the event.
    * @param keycode
    */
    CARAPI_(void) HandleVolumeKey(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 keycode);

    CARAPI_(Boolean) IsMute();

    CARAPI_(void) HandleMute();

    CARAPI_(void) DispatchMediaKeyWithWakeLock(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) DispatchMediaKeyRepeatWithWakeLock(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) DispatchMediaKeyWithWakeLockToAudioService(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) SendCloseSystemWindows();

    CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ const String& reason);

    CARAPI_(void) UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration);

    CARAPI_(void) UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration,
        /* [in] */ Boolean forceRelayout);

    CARAPI_(void) StartDockOrHome();

    /**
    * goes to the home screen
    * @return whether it did anything
    */
    CARAPI_(Boolean) GoHome();


private:
    CARAPI_(void) StartFlicker();

    CARAPI_(void) InterceptPowerKeyDown(
        /* [in] */ Boolean handled);

    CARAPI_(void) ShowBootAnimation();

    CARAPI_(void) HideBootAnimation();

    // CARAPI_(void) SetAirplaneModeState(
    //     /* [in] */ Boolean enabled);

    CARAPI_(void) RestoreAirplaneModeState();

    CARAPI_(void) InterceptBootFastPowerKeyDown(
        /* [in] */ Boolean handled);

    CARAPI_(Boolean) InterceptBootFastPowerKeyUp(
        /* [in] */ Boolean canceled);

    CARAPI_(Boolean) InterceptPowerKeyUp(
        /* [in] */ Boolean canceled);

    CARAPI_(void) CancelPendingPowerKeyAction();

    CARAPI_(void) InterceptScreenshotChord();

    CARAPI_(Int64) GetScreenshotChordLongPressDelay();

    CARAPI_(void) CancelPendingScreenshotChordAction();

    CARAPI_(void) HandleLongPressOnHome();

    CARAPI_(void) EnablePointerLocation();

    CARAPI_(void) DisablePointerLocation();

    CARAPI_(void) AcquireBootAnimationView();

    CARAPI_(void) ReleaseBootAnimationView();

    CARAPI_(Int32) ReadRotation(
        /* [in] */  Int32 resID);

    CARAPI_(Boolean) IsHidden(
        /* [in] */ Int32 accessibilityMode);

    CARAPI_(Boolean) IsBuiltInKeyboardVisible();

    Int64 InnerInterceptKeyBeforeDispatching(
        /* [in] */ IWindowState* win,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(Boolean) InterceptFallback(
        /* [in] */ IWindowState* win,
        /* [in] */ IKeyEvent* fallbackEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) LaunchAssistLongPressAction();

    CARAPI_(void) LaunchAssistAction();

    CARAPI_(AutoPtr<ISearchManager>) GetSearchManager();

    CARAPI_(void) ApplyStableConstraints(
        /* [in] */ Int32 sysui,
        /* [in] */ Int32 fl,
        /* [in] */ IRect* r);

    CARAPI_(void) OffsetInputMethodWindowLw(
        /* [in] */ IWindowState* win);

    // Assume this is called from the Handler thread.
    CARAPI_(void) TakeScreenshot();

    /** When the screen is off we ignore some keys that might otherwise typically
     * be considered wake keys.  We filter them out here.
     *
     * {@link KeyEvent#KEYCODE_POWER} is notably absent from this list because it
     * is always considered a wake key.
     */
    CARAPI_(Boolean) IsWakeKeyWhenScreenOff(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) WaitForKeyguard(
        /* [in] */ IScreenOnListener* screenOnListener);

    CARAPI_(void) WaitForKeyguardWindowDrawn(
        /* [in] */ IBinder* windowToken,
        /* [in] */ IScreenOnListener* screenOnListener);

    CARAPI_(void) FinishScreenTurningOn(
        /* [in] */ IScreenOnListener* screenOnListener);

    CARAPI_(void) ShowBootAnimation(
        /* [in] */ Int64 time);

    CARAPI_(Boolean) KeyguardIsShowingTq();

    CARAPI_(Boolean) IsLandscapeOrSeascape(
        /* [in] */ Int32 sensorRotation);

    CARAPI_(Boolean) IsAnyPortrait(
        /* [in] */ Int32 sensorRotation);

    CARAPI_(void) UpdateLockScreenTimeout();

    CARAPI_(void) ApplyLidSwitchState();

    CARAPI_(void) PerformAuditoryFeedbackForAccessibilityIfNeed();

    CARAPI_(Boolean) IsGlobalAccessibilityGestureEnabled();

    CARAPI_(Int32) UpdateSystemUiVisibilityLw();

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
    static const Boolean DEBUG_BOOTFAST;
    static const Boolean DEBUG_STARTING_WINDOW;
    static const Boolean SHOW_STARTING_ANIMATIONS;
    static const Boolean SHOW_PROCESSES_ON_ALT_MENU;

    static const Int32 WINDOW_TYPE_TEST_MODE; //new Window type for AllWinner SDK
    static const Int32 LONG_PRESS_POWER_NOTHING;
    static const Int32 LONG_PRESS_POWER_GLOBAL_ACTIONS;
    static const Int32 LONG_PRESS_POWER_SHUT_OFF;
    static const Int32 LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM;

    // These need to match the documentation/constant in
    // core/res/res/values/config.xml
    static const Int32 LONG_PRESS_HOME_NOTHING;
    static const Int32 LONG_PRESS_HOME_RECENT_DIALOG;
    static const Int32 LONG_PRESS_HOME_RECENT_SYSTEM_UI;

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

    static const Int32 RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS;
    static const Int32 RECENT_APPS_BEHAVIOR_EXIT_TOUCH_MODE_AND_SHOW;
    static const Int32 RECENT_APPS_BEHAVIOR_DISMISS;
    static const Int32 RECENT_APPS_BEHAVIOR_DISMISS_AND_SWITCH;

    static AutoPtr<CRect> mTmpParentFrame;
    static AutoPtr<CRect> mTmpDisplayFrame;
    static AutoPtr<CRect> mTmpContentFrame;
    static AutoPtr<CRect> mTmpVisibleFrame;
    static AutoPtr<CRect> mTmpNavigationFrame;

    static Boolean mFullScreenIsEnable;// = true;
    static Boolean mAlwaysFullScreen;// = false;

    static const Boolean PRINT_ANIM;

    // Java has not these two variable.
    static Boolean mVolumeMute;
    static Int32 mPreVolume;

// Need to instantiate in the Constructor.
protected:
    AutoPtr<PromptEnterMouseModeRunnable> mPromptEnterMouseMode;
    AutoPtr<PromptExitMouseModeRunnable> mPromptExitMouseMode;
    AutoPtr<HideNavInputEventReceiverFactory> mHideNavInputEventReceiverFactory;
    AutoPtr<ScreenshotTimeoutRunnable> mScreenshotTimeout;
    AutoPtr<DockBroadReceiver> mDockReceiver;
    AutoPtr<DreamBroadReceiver> mDreamReceiver;
    AutoPtr<MultiuserBroadReceiver> mMultiuserReceiver;
    AutoPtr<ScreenLockTimeoutRunnable> mScreenLockTimeout;

protected:
    /**
     * Lock protecting Internal state.  Must not call out Into window
     * manager with lock held.  (This lock will be acquired in places
     * where the window manager is calling in with its own lock held.)
     */
    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IWindowManagerPolicyWindowManagerFuncs> mWindowManagerFuncs;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IIStatusBarService> mStatusBarService;
    Object mServiceAquireLock;
    AutoPtr<IVibrator> mVibrator; // Vibrator for giving feedback of orientation changes
    AutoPtr<ISearchManager> mSearchManager;

    // Vibrator pattern for haptic feedback of a long press.
    AutoPtr<ArrayOf<Int64> > mLongPressVibePattern;

    // Vibrator pattern for haptic feedback of virtual key press.
    AutoPtr<ArrayOf<Int64> > mVirtualKeyVibePattern;

    // Vibrator pattern for a short vibration.
    AutoPtr<ArrayOf<Int64> > mKeyboardTapVibePattern;

    // Vibrator pattern for haptic feedback during boot when safe mode is disabled.
    AutoPtr<ArrayOf<Int64> > mSafeModeDisabledVibePattern;

    // Vibrator pattern for haptic feedback during boot when safe mode is enabled.
    AutoPtr<ArrayOf<Int64> > mSafeModeEnabledVibePattern;

    /** If true, hitting shift & menu will broadcast Intent.ACTION_BUG_REPORT */
    Boolean mEnableShiftMenuBugReports;

    Boolean mHeadless;
    Boolean mSafeMode;
    AutoPtr<IWindowState> mStatusBar;
    Boolean mHasSystemNavBar;
    Int32 mStatusBarHeight;
    AutoPtr<IWindowState> mNavigationBar;
    Boolean mHasNavigationBar;
    Boolean mCanHideNavigationBar;
    Boolean mNavigationBarCanMove; // can the navigation bar ever move to the side?
    Boolean mNavigationBarOnBottom; // is the navigation bar on the bottom *right now*?
    Int32 mNavigationBarHeightForRotation[4];
    Int32 mNavigationBarWidthForRotation[4];

    AutoPtr<IWindowState> mKeyguard;
    // TODO: Because KeyguardViewMediator is not implement.
    // KeyguardViewMediator mKeyguardMediator;
    AutoPtr<GlobalActions> mGlobalActions;
    volatile Boolean mPowerKeyHandled; // accessed from input reader and handler thread
    volatile Boolean mPowerBootKeyHandled;
    Boolean mPendingPowerKeyUpCanceled;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IWindowState> mLastInputMethodWindow;
    AutoPtr<IWindowState> mLastInputMethodTargetWindow;

    AutoPtr<IRecentApplicationsDialog> mRecentAppsDialog;
    Int32 mRecentAppsDialogHeldModifiers;
    Boolean mLanguageSwitchKeyPressed;

    Int32 mLidState;
    Boolean mHaveBuiltInKeyboard;

    Boolean mSystemReady;
    Boolean mSystemBooted;
    Boolean mHdmiPlugged;
    Int32 mDockMode;
    Int32 mLidOpenRotation;
    Int32 mCarDockRotation;
    Int32 mDeskDockRotation;
    Int32 mHdmiRotation;
    Boolean mHdmiRotationLock;

    Int32 mUserRotationMode;
    Int32 mUserRotation;
    Boolean mAccelerometerDefault;
    Boolean mBootFastEnable;

    Int32 mAllowAllRotations;
    Boolean mCarDockEnablesAccelerometer;
    Boolean mDeskDockEnablesAccelerometer;
    Int32 mLidKeyboardAccessibility;
    Int32 mLidNavigationAccessibility;
    Boolean mLidControlsSleep;
    Int32 mLongPressOnPowerBehavior;
    Boolean mScreenOnEarly;
    Boolean mScreenOnFully;
    Boolean mOrientationSensorEnabled;
    Int32 mCurrentAppOrientation;
    Boolean mHasSoftInput;

    Int32 mPointerLocationMode; // guarded by mLock

    // The last window we were told about in focusChanged.
    AutoPtr<IWindowState> mFocusedWindow;
    AutoPtr<IApplicationToken> mFocusedApp;
    AutoPtr<BootAnimationView> mBootAnimationView;

    // Pointer location view state, only modified on the mHandler Looper.
    AutoPtr<PointerLocationInputEventReceiver> mPointerLocationInputEventReceiver;
    AutoPtr<IPointerLocationView> mPointerLocationView;
    AutoPtr<IInputChannel> mPointerLocationInputChannel;

    // The current size of the screen; really; (ir)regardless of whether the status
    // bar can be hidden or not
    Int32 mUnrestrictedScreenLeft, mUnrestrictedScreenTop;
    Int32 mUnrestrictedScreenWidth, mUnrestrictedScreenHeight;
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
    // During layout, the current screen borders with all outer decoration
    // (status bar, input method dock) accounted for.
    Int32 mCurLeft, mCurTop, mCurRight, mCurBottom;
    // During layout, the frame in which content should be displayed
    // to the user, accounting for all screen decoration except for any
    // space they deem as available for other content.  This is usually
    // the same as mCur*, but may be larger if the screen decor has supplied
    // content insets.
    Int32 mContentLeft, mContentTop, mContentRight, mContentBottom;
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
    Boolean mTopIsFullscreen;
    Boolean mForceStatusBar;
    Boolean mForceStatusBarFromKeyguard;
    Boolean mHideLockScreen;
    Boolean mForcingShowNavBar;
    Int32 mForcingShowNavBarLayer;

    Int32 mDismissKeyguard;

    Boolean mShowingLockscreen;
    Boolean mShowingDream;
    Boolean mDreamingLockscreen;
    Boolean mHomePressed;
    Boolean mHomeLongPressed;
    AutoPtr<IIntent> mHomeIntent;
    AutoPtr<IIntent> mCarDockIntent;
    AutoPtr<IIntent> mDeskDockIntent;
    Boolean mSearchKeyShortcutPending;
    Boolean mConsumeSearchKeyUp;
    Boolean mAssistKeyLongPressed;

    // support for activating the lock screen while the screen is on
    Boolean mAllowLockscreenWhenOn;
    Int32 mLockScreenTimeout;
    Boolean mLockScreenTimerActive;

    // Behavior of ENDCALL Button.  (See Settings.System.END_BUTTON_BEHAVIOR.)
    Int32 mEndcallBehavior;

    // Behavior of POWER button while in-call and screen on.
    // (See Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR.)
    Int32 mIncallPowerBehavior;

    AutoPtr<IDisplay> mDisplay;

    Int32 mLandscapeRotation;  // default landscape rotation
    Int32 mSeascapeRotation;   // "other" landscape rotation, 180 degrees from mLandscapeRotation
    Int32 mPortraitRotation;   // default portrait rotation
    Int32 mUpsideDownRotation; // "other" portrait rotation

    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<ShortcutManager> mShortcutManager;
    AutoPtr<IPowerManagerWakeLock> mBroadcastWakeLock;
    Boolean mHavePendingMediaKeyRepeatWithWakeLock;

    /* add by Gary. start {{----------------------------------- */
    /* 2011-10-27 */
    /* support mute */
    AutoPtr<IBinder> mCallback;// = new Binder();
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-1 */
    /* support the shortcut keys "setup", "expand" and "browser" */
    Boolean mIsExpanded;// = false;
    AutoPtr<IStatusBarManager> mStatusBarManager;
    /* add by Gary. end   -----------------------------------}} */

    Boolean mKeyEnterMouseMode;// = false;
    AutoPtr<INotificationManager> mNotifationManager;// = null;
    AutoPtr<INotification> mNotificationEnterKeyMouseMode;// = null;
    AutoPtr<IToast>        mMouseToast;// = null;

    // AutoPtr<MyOrientationListener> mOrientationListener;

    Object mScreenshotLock;
    AutoPtr<IServiceConnection> mScreenshotConnection;

    AutoPtr<BootMsgDialog> mBootMsgDialog;

private:
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
    static const Int32 MSG_SHOW_BATTERY_CHARGE;// = 5;
    static const Int32 MSG_SHOW_BOOT_INIT;// = 6;

    static const Int32 NF_ID_ENTER_KEY_MOUSE_MODE;// = 1;

    static const Int32 FLICKER_INTERVAL;// = 30;
    static const Char32 PORT_TYPE;// = 'h';
    static const Int32 PORT_NUM;// = 20;
    static const Int32 ON;// = 1;
    static const Int32 OFF;// = 0;

    static AutoPtr<ArrayOf<Int32> > WINDOW_TYPES_WHERE_HOME_DOESNT_WORK;

// Need to instantiate in the Constructor.
private:
    AutoPtr<HDMIUEventObserver> mHDMIObserver;
    AutoPtr<FlickerIntentBroadcastReceiver> mFlickerIntentReceiver;
    AutoPtr<IRunnable> mStep1On;
    AutoPtr<IRunnable> mStep2Off;
    AutoPtr<IRunnable> mStep3On;
    AutoPtr<IRunnable> mBootFastPowerLongPress;
    AutoPtr<PowerLongPressRunnable> mPowerLongPress;
    AutoPtr<ScreenshotChordLongPressRunnable> mScreenshotChordLongPress;

private:
    /** The window that is currently dismissing the keyguard. Dismissing the keyguard must only
     * be done once per window. */
    AutoPtr<IWindowState> mWinDismissingKeyguard;

    // What we do when the user long presses on home
    Int32 mLongPressOnHomeBehavior;

    // Screenshot trigger states
    Boolean mScreenshotChordEnabled;
    Boolean mVolumeDownKeyTriggered;
    Int64 mVolumeDownKeyTime;
    Boolean mVolumeDownKeyConsumedByScreenshotChord;
    Boolean mVolumeUpKeyTriggered;
    Boolean mPowerKeyTriggered;
    Int64 mPowerKeyTime;
    Boolean mBootFastRuning;

    // Fallback actions by key code.
    AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > mFallbackActions;

    /* add by Gary. start {{----------------------------------- */
    /* 2011-12-7 */
    /* support the mouse mode */
    Int32 mLeftBtn;//  = -1;
    Int32 mMidBtn;//   = -1;
    Int32 mRightBtn;// = -1;
    Int32 mLeft;//     = -1;
    Int32 mRight;//    = -1;
    Int32 mTop;//      = -1;
    Int32 mBottom;//   = -1;

    /*add by chenjd,start {{-----------------------------*/
    /* 2013-03-02 */
    /* flicker led when receive ir signal */
    AutoPtr<IHandler> mFlickerHandler;// = new Handler();
    Boolean mFlickerEnable;//  = true;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWMANAGER_H__
