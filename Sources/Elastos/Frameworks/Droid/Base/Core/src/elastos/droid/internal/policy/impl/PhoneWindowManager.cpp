#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/internal/policy/impl/PhoneWindowManager.h"
#include "elastos/droid/internal/policy/impl/CBarController.h"
#include "elastos/droid/internal/policy/impl/CImmersiveModeConfirmation.h"
#include "elastos/droid/internal/policy/impl/CLogDecelerateInterpolator.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/internal/policy/impl/CRecentApplicationsDialog.h"
#include "elastos/droid/internal/policy/impl/CSystemGesturesPointerEventListener.h"
#include "elastos/droid/internal/policy/impl/SystemGesturesPointerEventListener.h"
#include "elastos/droid/internal/policy/impl/keyguard/CKeyguardServiceDelegate.h"
#include "elastos/droid/internal/widget/CPointerLocationView.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/FactoryTest.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/view/WindowManagerPolicyControl.h"
#include "elastos/droid/widget/CToastHelper.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <sys/reboot.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::IUiModeManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Hardware::Input::IIInputManager;
using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Internal::Policy::IIKeyguardServiceConstants;
using Elastos::Droid::Internal::Policy::Impl::Keyguard::CKeyguardServiceDelegate;
using Elastos::Droid::Internal::Policy::Impl::Keyguard::EIID_IKeyguardServiceDelegateShowListener;
using Elastos::Droid::Internal::Widget::CPointerLocationView;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelperHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelperHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::FactoryTest;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Service::Dreams::EIID_IDreamManagerInternal;
using Elastos::Droid::Service::Dreams::IDreamService;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationSet;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::WindowManagerPolicyControl;
using Elastos::Droid::View::EIID_IInputEventReceiverFactory;
using Elastos::Droid::View::EIID_IOnKeyguardExitResult;
using Elastos::Droid::View::EIID_IWindowManagerInternal;
using Elastos::Droid::View::EIID_IWindowManagerPolicy;
using Elastos::Droid::View::IFallbackAction;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IPointerEventListener;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;

using Elastos::Core::CString;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

static AutoPtr<ArrayOf<Int32> > InitWINDOW_TYPES_WHERE_HOME_DOESNT_WORK()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    array->Set(0, IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    array->Set(1, IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
    return array;
}

// Instead of static block.
static AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > InitApplicationLaunchKeyCategories()
{
    AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > sparseArray = new HashMap<Int32, AutoPtr<IInterface> >();

    AutoPtr<ICharSequence> categoryAppBrowser;
    CString::New(IIntent::CATEGORY_APP_BROWSER, (ICharSequence**)&categoryAppBrowser);
    (*sparseArray)[IKeyEvent::KEYCODE_EXPLORER] = categoryAppBrowser;

    AutoPtr<ICharSequence> categoryAppEmail;
    CString::New(IIntent::CATEGORY_APP_EMAIL, (ICharSequence**)&categoryAppEmail);
    (*sparseArray)[IKeyEvent::KEYCODE_ENVELOPE] = categoryAppEmail;

    AutoPtr<ICharSequence> categoryAppContacts;
    CString::New(IIntent::CATEGORY_APP_CONTACTS, (ICharSequence**)&categoryAppContacts);
    (*sparseArray)[IKeyEvent::KEYCODE_CONTACTS] = categoryAppContacts;

    AutoPtr<ICharSequence> categoryAppCalendar;
    CString::New(IIntent::CATEGORY_APP_CALENDAR, (ICharSequence**)&categoryAppCalendar);
    (*sparseArray)[IKeyEvent::KEYCODE_CALENDAR] = categoryAppCalendar;

    AutoPtr<ICharSequence> categoryAppMusic;
    CString::New(IIntent::CATEGORY_APP_MUSIC, (ICharSequence**)&categoryAppMusic);
    (*sparseArray)[IKeyEvent::KEYCODE_MUSIC] = categoryAppMusic;

    AutoPtr<ICharSequence> categoryAppCalculator;
    CString::New(IIntent::CATEGORY_APP_CALCULATOR, (ICharSequence**)&categoryAppCalculator);
    (*sparseArray)[IKeyEvent::KEYCODE_CALCULATOR] = categoryAppCalculator;

    return sparseArray;
}

static AutoPtr<IAudioAttributes> InitVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributes> audioAttributes;
    AutoPtr<IAudioAttributesBuilder> aaBuilder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aaBuilder);
    aaBuilder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    aaBuilder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    aaBuilder->Build((IAudioAttributes**)&audioAttributes);
    return audioAttributes;
}

static AutoPtr<CRect> InitCRect()
{
    AutoPtr<CRect> tmp;
    CRect::NewByFriend((CRect**)&tmp);
    return tmp;
}

String PhoneWindowManager::SYSTEM_DIALOG_REASON_KEY("reason");
String PhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS("globalactions");
String PhoneWindowManager::SYSTEM_DIALOG_REASON_RECENT_APPS("recentapps");
String PhoneWindowManager::SYSTEM_DIALOG_REASON_HOME_KEY("homekey");
String PhoneWindowManager::SYSTEM_DIALOG_REASON_ASSIST("assist");

const Int32 PhoneWindowManager::SYSTEM_UI_CHANGING_LAYOUT =
        IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION | IView::SYSTEM_UI_FLAG_FULLSCREEN
        | IView::STATUS_BAR_TRANSLUCENT | IView::NAVIGATION_BAR_TRANSLUCENT
        | IView::SYSTEM_UI_TRANSPARENT;

AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > PhoneWindowManager::sApplicationLaunchKeyCategories =
        InitApplicationLaunchKeyCategories();

String PhoneWindowManager::TAG("PhoneWindowManager");
const Boolean PhoneWindowManager::DEBUG = TRUE;
const Boolean PhoneWindowManager::localLOGV = TRUE;
const Boolean PhoneWindowManager::DEBUG_LAYOUT = TRUE;
const Boolean PhoneWindowManager::DEBUG_INPUT = TRUE;
const Boolean PhoneWindowManager::DEBUG_STARTING_WINDOW = TRUE;
const Boolean PhoneWindowManager::DEBUG_WAKEUP = TRUE;
const Boolean PhoneWindowManager::SHOW_STARTING_ANIMATIONS = TRUE;
const Boolean PhoneWindowManager::SHOW_PROCESSES_ON_ALT_MENU = FALSE;

const Boolean PhoneWindowManager::ENABLE_CAR_DOCK_HOME_CAPTURE = TRUE;
const Boolean PhoneWindowManager::ENABLE_DESK_DOCK_HOME_CAPTURE = FALSE;
const Int32 PhoneWindowManager::SHORT_PRESS_POWER_NOTHING = 0;
const Int32 PhoneWindowManager::SHORT_PRESS_POWER_GO_TO_SLEEP = 1;
const Int32 PhoneWindowManager::SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP = 2;
const Int32 PhoneWindowManager::SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP_AND_GO_HOME = 3;

const Int32 PhoneWindowManager::LONG_PRESS_POWER_NOTHING = 0;
const Int32 PhoneWindowManager::LONG_PRESS_POWER_GLOBAL_ACTIONS = 1;
const Int32 PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF = 2;
const Int32 PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM = 3;
const Int32 PhoneWindowManager::LONG_PRESS_HOME_NOTHING = 0;
const Int32 PhoneWindowManager::LONG_PRESS_HOME_RECENT_SYSTEM_UI = 1;
const Int32 PhoneWindowManager::LONG_PRESS_HOME_ASSIST = 2;
const Int32 PhoneWindowManager::DOUBLE_TAP_HOME_NOTHING = 0;
const Int32 PhoneWindowManager::DOUBLE_TAP_HOME_RECENT_SYSTEM_UI = 1;

const Int32 PhoneWindowManager::APPLICATION_MEDIA_SUBLAYER = -2;
const Int32 PhoneWindowManager::APPLICATION_MEDIA_OVERLAY_SUBLAYER = -1;
const Int32 PhoneWindowManager::APPLICATION_PANEL_SUBLAYER = 1;
const Int32 PhoneWindowManager::APPLICATION_SUB_PANEL_SUBLAYER = 2;

AutoPtr<CRect> PhoneWindowManager::mTmpParentFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpDisplayFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpOverscanFrame= InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpContentFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpVisibleFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpDecorFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpStableFrame = InitCRect();
AutoPtr<CRect> PhoneWindowManager::mTmpNavigationFrame = InitCRect();

const Int32 PhoneWindowManager::WAITING_FOR_DRAWN_TIMEOUT = 1000;
const Int32 PhoneWindowManager::DISMISS_KEYGUARD_NONE = 0;
const Int32 PhoneWindowManager::DISMISS_KEYGUARD_START = 1;
const Int32 PhoneWindowManager::DISMISS_KEYGUARD_CONTINUE = 2;
const Int64 PhoneWindowManager::SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS = 150;
const Float PhoneWindowManager::KEYGUARD_SCREENSHOT_CHORD_DELAY_MULTIPLIER = 2.5f;
const Int32 PhoneWindowManager::MSG_ENABLE_POINTER_LOCATION = 1;
const Int32 PhoneWindowManager::MSG_DISABLE_POINTER_LOCATION = 2;
const Int32 PhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK = 3;
const Int32 PhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK = 4;
const Int32 PhoneWindowManager::MSG_KEYGUARD_DRAWN_COMPLETE = 5;
const Int32 PhoneWindowManager::MSG_KEYGUARD_DRAWN_TIMEOUT = 6;
const Int32 PhoneWindowManager::MSG_WINDOW_MANAGER_DRAWN_COMPLETE = 7;
const Int32 PhoneWindowManager::MSG_DISPATCH_SHOW_RECENTS = 9;
const Int32 PhoneWindowManager::MSG_DISPATCH_SHOW_GLOBAL_ACTIONS = 10;
const Int32 PhoneWindowManager::MSG_HIDE_BOOT_MESSAGE = 11;
const Int32 PhoneWindowManager::MSG_LAUNCH_VOICE_ASSIST_WITH_WAKE_LOCK = 12;

const Boolean PhoneWindowManager::PRINT_ANIM = FALSE;
AutoPtr<ArrayOf<Int32> > PhoneWindowManager::WINDOW_TYPES_WHERE_HOME_DOESNT_WORK = InitWINDOW_TYPES_WHERE_HOME_DOESNT_WORK();
AutoPtr<IAudioAttributes> PhoneWindowManager::VIBRATION_ATTRIBUTES = InitVIBRATION_ATTRIBUTES();;

//Boolean PhoneWindowManager::mVolumeMute = FALSE;
//Int32 PhoneWindowManager::mPreVolume = 0;

const Int32 PhoneWindowManager::BRIGHTNESS_STEPS = 10;

PhoneWindowManager::MyWakeGestureListener::MyWakeGestureListener(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
    WakeGestureListener::constructor(context, handler);
}

ECode PhoneWindowManager::MyWakeGestureListener::OnWakeUp()
{
    {
        AutoLock lock(mHost->mLock);
        if (mHost->ShouldEnableWakeGestureLp()) {
            Boolean bTemp;
            mHost->PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::VIRTUAL_KEY, FALSE, &bTemp);
            mHost->mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
        }
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::MyWakeGestureListener--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks++
//==============================================================================
CAR_INTERFACE_IMPL(PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks, Object, ISystemGesturesPointerEventListenerCallbacks);

PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks::SystemGesturesPointerEventListenerCallbacks(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks::OnSwipeFromTop()
{
    if (mHost->mStatusBar != NULL)
    {
        mHost->RequestTransientBars(mHost->mStatusBar);
    }
    return NOERROR;
}

ECode PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks::OnSwipeFromBottom()
{
    if (mHost->mNavigationBar != NULL && mHost->mNavigationBarOnBottom) {
        mHost->RequestTransientBars(mHost->mNavigationBar);
    }
    return NOERROR;
}

ECode PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks::OnSwipeFromRight()
{
    if (mHost->mNavigationBar != NULL && !mHost->mNavigationBarOnBottom) {
        mHost->RequestTransientBars(mHost->mNavigationBar);
    }
    return NOERROR;
}

ECode PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks::OnDebug()
{
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::SystemGesturesPointerEventListenerCallbacks--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::WindowManagerDrawCallbackRunnable++
//==============================================================================

PhoneWindowManager::WindowManagerDrawCallbackRunnable::WindowManagerDrawCallbackRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::WindowManagerDrawCallbackRunnable::Run()
{
    if (DEBUG_WAKEUP) Slogger::I(TAG, "All windows ready for display!");
    Boolean isSucceed;
    mHost->mHandler->SendEmptyMessage(MSG_WINDOW_MANAGER_DRAWN_COMPLETE, &isSucceed);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::WindowManagerDrawCallbackRunnable--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::KeyguardDelegateCallbackShowListener++
//==============================================================================
CAR_INTERFACE_IMPL(PhoneWindowManager::KeyguardDelegateCallbackShowListener, Object, IKeyguardServiceDelegateShowListener);

PhoneWindowManager::KeyguardDelegateCallbackShowListener::KeyguardDelegateCallbackShowListener(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::KeyguardDelegateCallbackShowListener::OnShown(
    /* [in] */ IBinder* windowToken)
{
    if (DEBUG_WAKEUP) Slogger::D(TAG, "mKeyguardDelegate.ShowListener.onShown.");
    Boolean isSucceed;
    mHost->mHandler->SendEmptyMessage(MSG_KEYGUARD_DRAWN_COMPLETE, &isSucceed);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::KeyguardDelegateCallbackShowListener--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult++
//==============================================================================
CAR_INTERFACE_IMPL(PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult, Object, IOnKeyguardExitResult);

PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult::KeyguardDelegateOnKeyguardExitResult(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult::OnKeyguardExitResult(
    /* [in] */ Boolean success)
{
    if (success) {
        //try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->StopAppSwitches();
        //} catch (RemoteException e) {
        //}
        mHost->SendCloseSystemWindows(SYSTEM_DIALOG_REASON_HOME_KEY);
        mHost->StartDockOrHome();
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::KeyguardDelegateOnKeyguardExitResult--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ClearHideNavigationFlagRunnable++
//==============================================================================

PhoneWindowManager::ClearHideNavigationFlagRunnable::ClearHideNavigationFlagRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::ClearHideNavigationFlagRunnable::Run()
{
    {
        AutoPtr<ISynchronize> obj;
        mHost->mWindowManagerFuncs->GetWindowManagerLock((ISynchronize**)&obj);
        AutoLock lock(obj);
        // Clear flags.
        mHost->mForceClearedSystemUiFlags &= ~IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION;
    }
    mHost->mWindowManagerFuncs->ReevaluateStatusBarVisibility();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::ClearHideNavigationFlagRunnable--
//==============================================================================
//==============================================================================
//          ++ PhoneWindowManager::RequestTransientBarsRunnable++
//==============================================================================

PhoneWindowManager::RequestTransientBarsRunnable::RequestTransientBarsRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::RequestTransientBarsRunnable::Run()
{
    mHost->RequestTransientBars(mHost->mNavigationBar);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::RequestTransientBarsRunnable--
//==============================================================================
//==============================================================================
//          ++ PhoneWindowManager::KeyguardDelegateKeyguardDone++
//==============================================================================

PhoneWindowManager::KeyguardDelegateKeyguardDone::KeyguardDelegateKeyguardDone(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::KeyguardDelegateKeyguardDone::Run()
{
    mHost->mKeyguardDelegate->KeyguardDone(FALSE, FALSE);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::KeyguardDelegateKeyguardDone--
//==============================================================================
//==============================================================================
//          ++ PhoneWindowManager::KeyguardDelegateKeyguardDismiss++
//==============================================================================

PhoneWindowManager::KeyguardDelegateKeyguardDismiss::KeyguardDelegateKeyguardDismiss(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::KeyguardDelegateKeyguardDismiss::Run()
{
    mHost->mKeyguardDelegate->Dismiss();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::KeyguardDelegateKeyguardDismiss--
//==============================================================================
//==============================================================================
//          ++ PhoneWindowManager::KeyguardDelegateActivityDrawn++
//==============================================================================

PhoneWindowManager::KeyguardDelegateActivityDrawn::KeyguardDelegateActivityDrawn(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::KeyguardDelegateActivityDrawn::Run()
{
    mHost->mKeyguardDelegate->OnActivityDrawn();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::KeyguardDelegateActivityDrawn--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::SettingsObserver ++
//==============================================================================

PhoneWindowManager::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);
}

ECode PhoneWindowManager::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateSettings();
    mHost->UpdateRotation(FALSE);
    return NOERROR;
}

ECode PhoneWindowManager::SettingsObserver::Observe()
{
    // Observe all users' changes
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::END_BUTTON_BEHAVIOR, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::INCALL_POWER_BUTTON_BEHAVIOR, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::WAKE_GESTURE_ENABLED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::ACCELEROMETER_ROTATION, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::USER_ROTATION, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::SCREEN_OFF_TIMEOUT, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::POINTER_LOCATION, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::DEFAULT_INPUT_METHOD, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::Secure::GetUriFor(ISettingsSecure::IMMERSIVE_MODE_CONFIRMATIONS, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::Global::GetUriFor(ISettingsGlobal::POLICY_CONTROL, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    mHost->UpdateSettings();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::SettingsObserver --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::MyOrientationListener ++
//==============================================================================

PhoneWindowManager::MyOrientationListener::MyOrientationListener(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
    WindowOrientationListener::constructor(context, handler);
}

ECode PhoneWindowManager::MyOrientationListener::OnProposedRotationChanged(
    /* [in] */ Int32 rotation)
{
    if (localLOGV) Slogger::V(PhoneWindowManager::TAG, "onProposedRotationChanged, rotation=" + rotation);
    mHost->UpdateRotation(FALSE);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::MyOrientationListener --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::HideNavInputEventReceiver ++
//==============================================================================

ECode PhoneWindowManager::HideNavInputEventReceiver::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ PhoneWindowManager* host)
{
    mHost = host;
    return InputEventReceiver::constructor(inputChannel, looper);
}

ECode PhoneWindowManager::HideNavInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    Boolean handled = FALSE;
    //try {
    Int32 source = 0;
    if (IMotionEvent::Probe(event)
            && ((event->GetSource(&source), source) & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        const AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        Int32 action = 0;
        if ((motionEvent->GetAction(&action), action) == IMotionEvent::ACTION_DOWN) {
            // When the user taps down, we re-show the nav bar.
            Boolean changed = FALSE;
            {
                AutoPtr<ISynchronize> obj;
                mHost->mWindowManagerFuncs->GetWindowManagerLock((ISynchronize**)&obj);
                AutoLock lock(obj);
                // Any user activity always causes us to show the
                // navigation controls, if they had been hidden.
                // We also clear the low profile and only content
                // flags so that tapping on the screen will atomically
                // restore all currently hidden screen decorations.
                Int32 newVal = mHost->mResettingSystemUiFlags
                        | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | IView::SYSTEM_UI_FLAG_LOW_PROFILE
                        | IView::SYSTEM_UI_FLAG_FULLSCREEN;
                if (mHost->mResettingSystemUiFlags != newVal) {
                    mHost->mResettingSystemUiFlags = newVal;
                    changed = TRUE;
                }
                // We don't allow the system's nav bar to be hidden
                // again for 1 second, to prevent applications from
                // spamming us and keeping it from being shown.
                newVal = mHost->mForceClearedSystemUiFlags |
                        IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION;
                if (mHost->mForceClearedSystemUiFlags != newVal) {
                    mHost->mForceClearedSystemUiFlags = newVal;
                    changed = TRUE;
                    Boolean isSuccess = FALSE;
                    mHost->mHandler->PostDelayed(mHost->mClearHideNavigationFlag, 1000, &isSuccess);
                }
            }
            if (changed) {
                mHost->mWindowManagerFuncs->ReevaluateStatusBarVisibility();
            }
        }
    }
    // } finally {
    FinishInputEvent(event, handled);
    // }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::HideNavInputEventReceiver --
//==============================================================================


//==============================================================================
//          ++ PhoneWindowManager::HideNavInputEventReceiverFactory ++
//==============================================================================
CAR_INTERFACE_IMPL(PhoneWindowManager::HideNavInputEventReceiverFactory, Object, IInputEventReceiverFactory)

PhoneWindowManager::HideNavInputEventReceiverFactory::HideNavInputEventReceiverFactory(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::HideNavInputEventReceiverFactory::CreateInputEventReceiver(
   /* [in] */ IInputChannel* inputChannel,
   /* [in] */ ILooper* looper,
   /* [out] */ IInputEventReceiver** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    AutoPtr<HideNavInputEventReceiver> hner = new HideNavInputEventReceiver();
    hner->constructor(inputChannel, looper, mHost);
    *receiver = (IInputEventReceiver*)hner.Get();
    REFCOUNT_ADD(*receiver);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::HideNavInputEventReceiverFactory --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ScreenshotTimeoutRunnable ++
//==============================================================================

PhoneWindowManager::ScreenshotTimeoutRunnable::ScreenshotTimeoutRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::ScreenshotTimeoutRunnable::Run()
{
    AutoLock lock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection != NULL) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::ScreenshotTimeoutRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::DockBroadReceiver ++
//==============================================================================

PhoneWindowManager::DockBroadReceiver::DockBroadReceiver(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::DockBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (IIntent::ACTION_DOCK_EVENT.Equals((intent->GetAction(&action), action))) {
        Int32 value = 0;
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &value);
        mHost->mDockMode = value;
    } else {
        //try {
        AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::UI_MODE_SERVICE);
        IIUiModeManager* uiModeService = IIUiModeManager::Probe(tmpObj);
        uiModeService->GetCurrentModeType(&mHost->mUiMode);
        //} catch (RemoteException e) {
        //}
    }

    mHost->UpdateRotation(TRUE);
    {
        AutoLock lock(mHost->mLock);
        mHost->UpdateOrientationListenerLp();
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::DockBroadReceiver --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::DreamBroadReceiver ++
//==============================================================================

PhoneWindowManager::DreamBroadReceiver::DreamBroadReceiver(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::DreamBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (IIntent::ACTION_DREAMING_STARTED.Equals((intent->GetAction(&action), action))) {
        if (mHost->mKeyguardDelegate != NULL) {
            mHost->mKeyguardDelegate->OnDreamingStarted();
        }
    } else if (IIntent::ACTION_DREAMING_STOPPED.Equals((intent->GetAction(&action), action))) {
        if (mHost->mKeyguardDelegate != NULL) {
            mHost->mKeyguardDelegate->OnDreamingStopped();
        }
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::DreamBroadReceiver --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::MultiuserBroadReceiver ++
//==============================================================================

PhoneWindowManager::MultiuserBroadReceiver::MultiuserBroadReceiver(
    /*in*/ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::MultiuserBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (IIntent::ACTION_USER_SWITCHED.Equals((intent->GetAction(&action), action))) {
        // tickle the settings observer: this first ensures that we're
        // observing the relevant settings for the newly-active user,
        // and then updates our own bookkeeping based on the now-
        // current user.
        mHost->mSettingsObserver->OnChange(FALSE);

        // force a re-application of focused window sysui visibility.
        // the window may never have been shown for this user
        // e.g. the keyguard when going through the new-user setup flow
        {
            AutoPtr<ISynchronize> obj;
            mHost->mWindowManagerFuncs->GetWindowManagerLock((ISynchronize**)&obj);
            AutoLock lock(obj);
            mHost->mLastSystemUiFlags = 0;
            mHost->UpdateSystemUiVisibilityLw();
        }
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::MultiuserBroadReceiver --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ScreenLockTimeoutRunnable ++
//==============================================================================

PhoneWindowManager::ScreenLockTimeoutRunnable::ScreenLockTimeoutRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::ScreenLockTimeoutRunnable::Run()
{
    AutoLock lock(this);
    // if (localLOGV) Log.v(TAG, "mScreenLockTimeout activating keyguard");
    if (mHost->mKeyguardDelegate != NULL) {
        mHost->mKeyguardDelegate->DoKeyguardTimeout(mOptions);
    }
    mHost->mLockScreenTimerActive = FALSE;
    mOptions = NULL;
    return NOERROR;
}

void PhoneWindowManager::ScreenLockTimeoutRunnable::SetLockOptions(
    /* [in] */ IBundle* options)
{
    mOptions = options;
}

//==============================================================================
//          -- PhoneWindowManager::ScreenLockTimeoutRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::PolicyHandler ++
//==============================================================================

PhoneWindowManager::PolicyHandler::PolicyHandler(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::PolicyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case PhoneWindowManager::MSG_ENABLE_POINTER_LOCATION:
            mHost->EnablePointerLocation();
            break;
        case PhoneWindowManager::MSG_DISABLE_POINTER_LOCATION:
            mHost->DisablePointerLocation();
            break;
        case PhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(obj);
                mHost->DispatchMediaKeyWithWakeLock(keyEvent);
            }
            break;
        case PhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(obj);
                mHost->DispatchMediaKeyRepeatWithWakeLock(keyEvent);
            }
            break;
        case MSG_DISPATCH_SHOW_RECENTS:
            mHost->ShowRecentApps(false);
            break;
        case MSG_DISPATCH_SHOW_GLOBAL_ACTIONS:
            mHost->ShowGlobalActionsInternal();
            break;
        case MSG_KEYGUARD_DRAWN_COMPLETE:
            if (DEBUG_WAKEUP) Slogger::W(TAG, "Setting mKeyguardDrawComplete");
            mHost->FinishKeyguardDrawn();
            break;
        case MSG_KEYGUARD_DRAWN_TIMEOUT:
            Slogger::W(TAG, "Keyguard drawn timeout. Setting mKeyguardDrawComplete");
            mHost->FinishKeyguardDrawn();
            break;
        case MSG_WINDOW_MANAGER_DRAWN_COMPLETE:
            if (DEBUG_WAKEUP) Slogger::W(TAG, "Setting mWindowManagerDrawComplete");
            mHost->FinishWindowsDrawn();
            break;
        case MSG_HIDE_BOOT_MESSAGE:
            mHost->HandleHideBootMessage();
            break;
        case MSG_LAUNCH_VOICE_ASSIST_WITH_WAKE_LOCK:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->LaunchVoiceAssistWithWakeLock(arg1 != 0);
                break;
            }
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::PolicyHandler --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::HDMIUEventObserver ++
//==============================================================================

PhoneWindowManager::HDMIUEventObserver::HDMIUEventObserver(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::HDMIUEventObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    String str;
    event->Get(String("SWITCH_STATE"), &str);
    mHost->SetHdmiPlugged(str.Equals("1"));
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::HDMIUEventObserver --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::PowerLongPressRunnable ++
//==============================================================================

PhoneWindowManager::PowerLongPressRunnable::PowerLongPressRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::PowerLongPressRunnable::Run()
{
    // The context isn't read
    if (mHost->mLongPressOnPowerBehavior < 0) {
        AutoPtr<IResources> resources;
        mHost->mContext->GetResources((IResources**)&resources);
        resources->GetInteger(R::integer::config_longPressOnPowerBehavior,
                &mHost->mLongPressOnPowerBehavior);
    }
    Int32 resolvedBehavior = mHost->mLongPressOnPowerBehavior;
    if (FactoryTest::IsLongPressOnPowerOffEnabled()) {
        resolvedBehavior = PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM;
    }
    switch (resolvedBehavior) {
        case PhoneWindowManager::LONG_PRESS_POWER_NOTHING:
            break;
        case PhoneWindowManager::LONG_PRESS_POWER_GLOBAL_ACTIONS:
            {
                mHost->mPowerKeyHandled = TRUE;
                Boolean value = FALSE;
                if (!(mHost->PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &value), value)) {
                    mHost->PerformAuditoryFeedbackForAccessibilityIfNeed();
                }
                mHost->ShowGlobalActionsInternal();
            }
            break;
        case PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF:
        case PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM:
            {
                mHost->mPowerKeyHandled = TRUE;
                Boolean value = FALSE;
                mHost->PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &value);
                mHost->SendCloseSystemWindows(PhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS);
                mHost->mWindowManagerFuncs->Shutdown(resolvedBehavior == PhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF);
            }
            break;
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::PowerLongPressRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ScreenshotRunnable ++
//==============================================================================

PhoneWindowManager::ScreenshotRunnable::ScreenshotRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::ScreenshotRunnable::Run()
{
    mHost->TakeScreenshot();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::ScreenshotRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::HomeDoubleTapTimeoutRunnable++
//==============================================================================

PhoneWindowManager::HomeDoubleTapTimeoutRunnable::HomeDoubleTapTimeoutRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::HomeDoubleTapTimeoutRunnable::Run()
{
    if (mHost->mHomeDoubleTapPending) {
        mHost->mHomeDoubleTapPending = FALSE;
        mHost->LaunchHomeFromHotKey();
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::HomeDoubleTapTimeoutRunnable--
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::CollapsePanelsRunnable ++
//==============================================================================

PhoneWindowManager::CollapsePanelsRunnable::CollapsePanelsRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::CollapsePanelsRunnable::Run()
{
    ECode ec = NOERROR;
    // try {
    AutoPtr<IIStatusBarService> statusbar = mHost->GetStatusBarService();
    if (statusbar != NULL) {
        ec = statusbar->CollapsePanels();
    }
    // } catch (RemoteException ex) {
        // re-acquire status bar service next time it is needed.
    if (FAILED(ec)) {
        mHost->mStatusBarService = NULL;
    }
    // }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::CollapsePanelsRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ScreenshotHandler ++
//==============================================================================

PhoneWindowManager::ScreenshotHandler::ScreenshotHandler(
    /* [in] */ IServiceConnection *conn,
    /* [in] */ PhoneWindowManager* host)
    : myConn(conn)
    , mHost(host)
{
}

ECode PhoneWindowManager::ScreenshotHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoLock lock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection == myConn) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
        mHost->mHandler->RemoveCallbacks(mHost->mScreenshotTimeout);
    }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::ScreenshotHandler --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::ScreenshotServiceConnection ++
//==============================================================================
CAR_INTERFACE_IMPL(PhoneWindowManager::ScreenshotServiceConnection, Object, IServiceConnection)

PhoneWindowManager::ScreenshotServiceConnection::ScreenshotServiceConnection(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::ScreenshotServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoLock lock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection != this) {
        return NOERROR;
    }

    AutoPtr<IMessenger> messenger;
    CMessenger::New(IIMessenger::Probe(service), (IMessenger**)&messenger);
    AutoPtr<IMessageHelper> messageHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&messageHelper);
    AutoPtr<IMessage> msg;
    messageHelper->Obtain(NULL, 1, (IMessage**)&msg);
    const AutoPtr<IServiceConnection> myConn = this;
    AutoPtr<ILooper> looper;
    mHost->mHandler->GetLooper((ILooper**)&looper);
    AutoPtr<ScreenshotHandler> h = new ScreenshotHandler(myConn, mHost);
    h->constructor(looper);
    AutoPtr<IMessenger> msgTmp;
    CMessenger::New(h, (IMessenger**)&msgTmp);
    msg->SetReplyTo(msgTmp);
    msg->SetArg1(0);
    msg->SetArg2(0);
    Boolean isVisibleLw = FALSE;
    if (mHost->mStatusBar != NULL && (mHost->mStatusBar->IsVisibleLw(&isVisibleLw), isVisibleLw)) {
        msg->SetArg1(1);
    }
    if (mHost->mNavigationBar != NULL && (mHost->mNavigationBar->IsVisibleLw(&isVisibleLw), isVisibleLw)) {
        msg->SetArg2(1);
    }
    // try {
    messenger->Send(msg);
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode PhoneWindowManager::ScreenshotServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::ScreenshotServiceConnection --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::UpdateSettingRunnable ++
//==============================================================================

PhoneWindowManager::UpdateSettingRunnable::UpdateSettingRunnable(
    /* [in] */ PhoneWindowManager* host)
    : mHost(host)
{
}

ECode PhoneWindowManager::UpdateSettingRunnable::Run()
{
    mHost->UpdateSettings();
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::UpdateSettingRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::BootMsgDialog ++
//==============================================================================

ECode PhoneWindowManager::BootMsgDialog::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    return ProgressDialog::constructor(context, theme);
}

ECode PhoneWindowManager::BootMsgDialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::BootMsgDialog::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::BootMsgDialog::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::BootMsgDialog::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::BootMsgDialog::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::BootMsgDialog::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean *result)
{
    *result = TRUE;
    return NOERROR;
}


//==============================================================================
//          ++ PhoneWindowManager::BootMsgRunnable ++
//==============================================================================

PhoneWindowManager::BootMsgRunnable::BootMsgRunnable(
    /* [in] */ PhoneWindowManager* host,
    /* [in] */ ICharSequence* msg)
    : mHost(host)
    , mMsg(msg)
{
}

ECode PhoneWindowManager::BootMsgRunnable::Run()
{
    if (mHost->mBootMsgDialog == NULL) {
        int theme;
        AutoPtr<IPackageManager> pm;
        mHost->mContext->GetPackageManager((IPackageManager**)&pm);
        Boolean bTemp;
        if (pm->HasSystemFeature(IPackageManager::FEATURE_WATCH, &bTemp), bTemp) {
            theme = R::style::Theme_Micro_Dialog_Alert;
        } else if (pm->HasSystemFeature(IPackageManager::FEATURE_TELEVISION, &bTemp), bTemp) {
            theme = R::style::Theme_Leanback_Dialog_Alert;
        } else {
            theme = 0;
        }
        mHost->mBootMsgDialog = new BootMsgDialog();
        mHost->mBootMsgDialog->Init(mHost->mContext, theme);
        mHost->mBootMsgDialog->SetTitle(R::string::android_upgrading_title);
        mHost->mBootMsgDialog->SetProgressStyle(IProgressDialog::STYLE_SPINNER);
        mHost->mBootMsgDialog->SetIndeterminate(TRUE);
        AutoPtr<IWindow> window;
        mHost->mBootMsgDialog->GetWindow((IWindow**)&window);
        window->SetType(
                IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS);
        window->AddFlags(
                IWindowManagerLayoutParams::FLAG_DIM_BEHIND
                | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN);
        window->SetDimAmount(1);
        AutoPtr<IWindowManagerLayoutParams> lp;
        window->GetAttributes((IWindowManagerLayoutParams**)&lp);
        lp->SetScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_NOSENSOR);
        window->SetAttributes(lp);
        mHost->mBootMsgDialog->SetCancelable(FALSE);
        IDialog::Probe(mHost->mBootMsgDialog)->Show();
    }
    mHost->mBootMsgDialog->SetMessage(mMsg);
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::BootMsgRunnable --
//==============================================================================

//==============================================================================
//          ++ PhoneWindowManager::UpdateSystemUiVisibilityRunnable ++
//==============================================================================

PhoneWindowManager::UpdateSystemUiVisibilityRunnable::UpdateSystemUiVisibilityRunnable(
    /* [in] */ Int32 visibility,
    /* [in] */ Boolean needsMenu,
    /* [in] */ PhoneWindowManager* host)
    : mVisibility(visibility)
    , mNeedsMenu(needsMenu)
    , mHost(host)
{
}

ECode PhoneWindowManager::UpdateSystemUiVisibilityRunnable::Run()
{
    // try {
    AutoPtr<IIStatusBarService> statusbar = mHost->GetStatusBarService();
    if (statusbar != NULL) {
        FAIL_GOTO(statusbar->SetSystemUiVisibility(mVisibility, 0xffffffff), EXCEPTION);
        FAIL_GOTO(statusbar->TopAppWindowChanged(mNeedsMenu), EXCEPTION);
    }
    return NOERROR;
    // } catch (RemoteException e) {
        // re-acquire status bar service next time it is needed.
EXCEPTION:
    mHost->mStatusBarService = NULL;
    // }
    return NOERROR;
}

//==============================================================================
//          -- PhoneWindowManager::UpdateSystemUiVisibilityRunnable --
//==============================================================================
CAR_INTERFACE_IMPL_2(PhoneWindowManager, Object, IWindowManagerPolicy, IPhoneWindowManager)

PhoneWindowManager::PhoneWindowManager()
    : mPreloadedRecentApps(FALSE)
    , mEnableShiftMenuBugReports(FALSE)
    , mSafeMode(FALSE)
    , mStatusBarHeight(0)
    , mHasNavigationBar(FALSE)
    , mCanHideNavigationBar(FALSE)
    , mNavigationBarCanMove(FALSE)
    , mNavigationBarOnBottom(FALSE)
    , mBootMessageNeedsHiding(FALSE)
    , mPowerKeyHandled(FALSE)
    , mPendingPowerKeyUpCanceled(FALSE)
    , mRecentsVisible(FALSE)
    , mRecentAppsHeldModifiers(0)
    , mLanguageSwitchKeyPressed(FALSE)
    , mLidState(IWindowManagerPolicyWindowManagerFuncs::LID_ABSENT)
    , mCameraLensCoverState(IWindowManagerPolicyWindowManagerFuncs::CAMERA_LENS_COVER_ABSENT)
    , mHaveBuiltInKeyboard(FALSE)
    , mSystemReady(FALSE)
    , mSystemBooted(FALSE)
    , mHdmiPlugged(FALSE)
    , mUiMode(0)
    , mDockMode(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mLidOpenRotation(0)
    , mCarDockRotation(0)
    , mDeskDockRotation(0)
    , mUndockedHdmiRotation(0)
    , mDemoHdmiRotation(0)
    , mDemoHdmiRotationLock(FALSE)
    , mWakeGestureEnabledSetting(FALSE)
    , mUserRotationMode(IWindowManagerPolicy::USER_ROTATION_FREE)
    , mUserRotation(ISurface::ROTATION_0)
    , mAccelerometerDefault(FALSE)
    , mSupportAutoRotation(FALSE)
    , mAllowAllRotations(-1)
    , mCarDockEnablesAccelerometer(FALSE)
    , mDeskDockEnablesAccelerometer(FALSE)
    , mLidKeyboardAccessibility(0)
    , mLidNavigationAccessibility(0)
    , mLidControlsSleep(FALSE)
    , mShortPressOnPowerBehavior(-1)
    , mLongPressOnPowerBehavior(-1)
    , mAwake(FALSE)
    , mScreenOnEarly(FALSE)
    , mScreenOnFully(FALSE)
    , mKeyguardDrawComplete(FALSE)
    , mWindowManagerDrawComplete(FALSE)
    , mOrientationSensorEnabled(FALSE)
    , mCurrentAppOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED)
    , mHasSoftInput(FALSE)
    , mTranslucentDecorEnabled(TRUE)
    , mPointerLocationMode(0)
    , mOverscanScreenLeft(0)
    , mOverscanScreenTop(0)
    , mOverscanScreenWidth(0)
    , mOverscanScreenHeight(0)
    , mUnrestrictedScreenLeft(0)
    , mUnrestrictedScreenTop(0)
    , mUnrestrictedScreenWidth(0)
    , mUnrestrictedScreenHeight(0)
    , mRestrictedOverscanScreenLeft(0)
    , mRestrictedOverscanScreenTop(0)
    , mRestrictedOverscanScreenWidth(0)
    , mRestrictedOverscanScreenHeight(0)
    , mRestrictedScreenLeft(0)
    , mRestrictedScreenTop(0)
    , mRestrictedScreenWidth(0)
    , mRestrictedScreenHeight(0)
    , mSystemLeft(0)
    , mSystemTop(0)
    , mSystemRight(0)
    , mSystemBottom(0)
    , mStableLeft(0)
    , mStableTop(0)
    , mStableRight(0)
    , mStableBottom(0)
    , mStableFullscreenLeft(0)
    , mStableFullscreenTop(0)
    , mStableFullscreenRight(0)
    , mStableFullscreenBottom(0)
    , mCurLeft(0)
    , mCurTop(0)
    , mCurRight(0)
    , mCurBottom(0)
    , mContentLeft(0)
    , mContentTop(0)
    , mContentRight(0)
    , mContentBottom(0)
    , mVoiceContentLeft(0)
    , mVoiceContentTop(0)
    , mVoiceContentRight(0)
    , mVoiceContentBottom(0)
    , mDockLeft(0)
    , mDockTop(0)
    , mDockRight(0)
    , mDockBottom(0)
    , mDockLayer(0)
    , mStatusBarLayer(0)
    , mLastSystemUiFlags(0)
    , mResettingSystemUiFlags(0)
    , mForceClearedSystemUiFlags(0)
    , mLastFocusNeedsMenu(FALSE)
    , mTopIsFullscreen(FALSE)
    , mForceStatusBar(FALSE)
    , mForceStatusBarFromKeyguard(FALSE)
    , mHideLockScreen(FALSE)
    , mForcingShowNavBar(FALSE)
    , mForcingShowNavBarLayer(-1)
    , mDismissKeyguard(DISMISS_KEYGUARD_NONE)
    , mShowingLockscreen(FALSE)
    , mShowingDream(FALSE)
    , mDreamingLockscreen(FALSE)
    , mHomePressed(FALSE)
    , mHomeConsumed(FALSE)
    , mHomeDoubleTapPending(FALSE)
    , mSearchKeyShortcutPending(FALSE)
    , mConsumeSearchKeyUp(FALSE)
    , mAssistKeyLongPressed(FALSE)
    , mPendingMetaAction(FALSE)
    , mAllowLockscreenWhenOn(FALSE)
    , mLockScreenTimeout(0)
    , mLockScreenTimerActive(FALSE)
    , mEndcallBehavior(0)
    , mIncallPowerBehavior(0)
    , mLandscapeRotation(0)
    , mSeascapeRotation(0)
    , mPortraitRotation(0)
    , mUpsideDownRotation(0)
    , mOverscanLeft(0)
    , mOverscanTop(0)
    , mOverscanRight(0)
    , mOverscanBottom(0)
    , mHavePendingMediaKeyRepeatWithWakeLock(FALSE)
    , mKeyguardHidden(FALSE)
    , mKeyguardDrawnOnce(FALSE)
    , mLongPressOnHomeBehavior(0)
    , mDoubleTapOnHomeBehavior(0)
    , mScreenshotChordEnabled(FALSE)
    , mVolumeDownKeyTriggered(FALSE)
    , mVolumeDownKeyTime(0)
    , mVolumeDownKeyConsumedByScreenshotChord(FALSE)
    , mVolumeUpKeyTriggered(FALSE)
    , mPowerKeyTriggered(FALSE)
    , mPowerKeyTime(0)
    , mCurrentUserId(0)
    , mForceDefaultOrientation(FALSE)
{
}

ECode PhoneWindowManager::constructor()
{
    memset(mNavigationBarHeightForRotation, 0, sizeof(mNavigationBarHeightForRotation));
    memset(mNavigationBarWidthForRotation, 0, sizeof(mNavigationBarWidthForRotation));

    // Init member of anonymous class.
    CHashSet::New((IHashSet**)&mAppsToBeHidden);
    CHashSet::New((IHashSet**)&mAppsThatDismissKeyguard);
    mWindowManagerDrawCallback = new WindowManagerDrawCallbackRunnable(this);
    mKeyguardDelegateCallback = new KeyguardDelegateCallbackShowListener(this);
    mHDMIObserver = new HDMIUEventObserver(this);
    mPowerLongPress = new PowerLongPressRunnable(this);
    mScreenshotRunnable = new ScreenshotRunnable(this);
    mHideNavInputEventReceiverFactory = new HideNavInputEventReceiverFactory(this);
    mScreenshotTimeout = new ScreenshotTimeoutRunnable(this);
    mDockReceiver = new DockBroadReceiver(this);
    mDreamReceiver = new DreamBroadReceiver(this);
    mMultiuserReceiver = new MultiuserBroadReceiver(this);
    mScreenLockTimeout = new ScreenLockTimeoutRunnable(this);

    mFallbackActions = new HashMap<Int32, AutoPtr<IInterface> >();
    CLogDecelerateInterpolator::New(100, 0, (ILogDecelerateInterpolator**)&mLogDecelerateInterpolator);
    CBarController::New(String("StatusBar"),
            IView::STATUS_BAR_TRANSIENT,
            IView::STATUS_BAR_UNHIDE,
            IView::STATUS_BAR_TRANSLUCENT,
            IStatusBarManager::WINDOW_STATUS_BAR,
            IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS,
            (IBarController**)&mStatusBarController);
    CBarController::New(String("NavigationBar"),
                        IView::NAVIGATION_BAR_TRANSIENT,
                        IView::NAVIGATION_BAR_UNHIDE,
                        IView::NAVIGATION_BAR_TRANSLUCENT,
                        IStatusBarManager::WINDOW_NAVIGATION_BAR,
                        IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION,
                        (IBarController**)&mNavigationBarController);
    mHomeDoubleTapTimeoutRunnable = new HomeDoubleTapTimeoutRunnable(this);
    mClearHideNavigationFlag = new ClearHideNavigationFlagRunnable(this);
    mRequestTransientNav = new RequestTransientBarsRunnable(this);
    return NOERROR;
}

AutoPtr<IIStatusBarService> PhoneWindowManager::GetStatusBarService()
{
    AutoLock lock(mServiceAquireLock);
    if (mStatusBarService == NULL) {
        AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::STATUS_BAR_SERVICE);
        mStatusBarService = IIStatusBarService::Probe(tmpObj.Get());
    }
    return mStatusBarService;
}

Boolean PhoneWindowManager::NeedSensorRunningLp()
{
    if (mSupportAutoRotation) {
        if (mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR
                || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR
                || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT
                || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE) {
            // If the application has explicitly requested to follow the
            // orientation, then we need to turn the sensor or.
            return TRUE;
        }
    }

    if ((mCarDockEnablesAccelerometer && mDockMode == IIntent::EXTRA_DOCK_STATE_CAR) ||
        (mDeskDockEnablesAccelerometer && (mDockMode == IIntent::EXTRA_DOCK_STATE_DESK
                || mDockMode == IIntent::EXTRA_DOCK_STATE_LE_DESK
                || mDockMode == IIntent::EXTRA_DOCK_STATE_HE_DESK
        ))) {
            // enable accelerometer if we are docked in a dock that enables accelerometer
            // orientation management,
            return TRUE;
    }

    if (mUserRotationMode == USER_ROTATION_LOCKED) {
        // If the setting for using the sensor by default is enabled, then
        // we will always leave it on.  Note that the user could go to
        // a window that forces an orientation that does not use the
        // sensor and in theory we could turn it off... however, when next
        // turning it on we won't have a good value for the current
        // orientation for a little bit, which can cause orientation
        // changes to lag, so we'd like to keep it always on.  (It will
        // still be turned off when the screen is off.)
        return FALSE;
    }

    return mSupportAutoRotation;
}

void PhoneWindowManager::UpdateOrientationListenerLp()
{
    Boolean bTemp;
    if (!(mOrientationListener->CanDetectOrientation(&bTemp), bTemp)) {
        // If sensor is turned off or nonexistent for some reason
        return;
    }
    // Could have been invoked due to screen turning on or off or
    // change of the currently visible window's orientation
    if (localLOGV)
        Slogger::V(TAG, "mScreenOnEarly=%d, mAwake=%d, mCurrentAppOrientation=%d, mOrientationSensorEnabled=%d",
             mScreenOnEarly, mAwake, mCurrentAppOrientation, mOrientationSensorEnabled);
    Boolean disable = TRUE;
    if (mScreenOnEarly && mAwake) {
        if (NeedSensorRunningLp()) {
            disable = FALSE;
            //enable listener if not already enabled
            if (!mOrientationSensorEnabled) {
                mOrientationListener->Enable();
                if(localLOGV) Slogger::V(TAG, "Enabling listeners");
                mOrientationSensorEnabled = TRUE;
            }
        }
    }
    //check if sensors need to be disabled
    if (disable && mOrientationSensorEnabled) {
        mOrientationListener->Disable();
        if(localLOGV) Slogger::V(TAG, "Disabling listeners");
        mOrientationSensorEnabled = FALSE;
    }
}

void PhoneWindowManager::InterceptPowerKeyDown(
    /* [in] */ Boolean handled)
{
    mPowerKeyHandled = handled;
    if (!handled) {
        Int64 delay;
        AutoPtr<IViewConfiguration> vc = CViewConfiguration::Get(mContext);
        vc->GetDeviceGlobalActionKeyTimeout(&delay);
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mPowerLongPress, delay, &isSuccess);
    }
}

Boolean PhoneWindowManager::InterceptPowerKeyUp(
    /* [in] */ Boolean canceled)
{
    if (!mPowerKeyHandled) {
        mHandler->RemoveCallbacks(mPowerLongPress);
        return !canceled;
    }
    return FALSE;
}

void PhoneWindowManager::CancelPendingPowerKeyAction()
{
    if (!mPowerKeyHandled) {
        mHandler->RemoveCallbacks(mPowerLongPress);
    }
    if (mPowerKeyTriggered) {
        mPendingPowerKeyUpCanceled = TRUE;
    }
}

void PhoneWindowManager::InterceptScreenshotChord()
{
    if (mScreenshotChordEnabled
            && mVolumeDownKeyTriggered && mPowerKeyTriggered && !mVolumeUpKeyTriggered) {
        Int64 now = SystemClock::GetUptimeMillis();
        if (now <= mVolumeDownKeyTime + SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS
                && now <= mPowerKeyTime + SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS) {
            mVolumeDownKeyConsumedByScreenshotChord = TRUE;
            CancelPendingPowerKeyAction();

            Boolean isSuccess = FALSE;
            mHandler->PostDelayed(mScreenshotRunnable, GetScreenshotChordLongPressDelay(), &isSuccess);
        }
    }
}

Int64 PhoneWindowManager::GetScreenshotChordLongPressDelay()
{
    //PFL_EX(": PhoneWindowManager::GetScreenshotChordLongPressDelay()")
    Int64 delay;
    AutoPtr<IViewConfiguration> vc = CViewConfiguration::Get(mContext);
    vc->GetDeviceGlobalActionKeyTimeout(&delay);
    Boolean bTemp;
    if ((mKeyguardDelegate->IsShowing(&bTemp), bTemp)) {
         // Double the time it takes to take a screenshot from the keyguard
         return (Int64) (KEYGUARD_SCREENSHOT_CHORD_DELAY_MULTIPLIER + delay);
     }
    return delay;
}

void PhoneWindowManager::CancelPendingScreenshotChordAction()
{
    mHandler->RemoveCallbacks(mScreenshotRunnable);
}

void PhoneWindowManager::PowerShortPress(
    /* [in] */ Int64 eventTime)
{
    if (mShortPressOnPowerBehavior < 0) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        resources->GetInteger(R::integer::config_shortPressOnPowerBehavior, &mShortPressOnPowerBehavior);
    }

    switch (mShortPressOnPowerBehavior) {
        case SHORT_PRESS_POWER_NOTHING:
            break;
        case SHORT_PRESS_POWER_GO_TO_SLEEP:
            //TODO mPowerManager->GoToSleep(eventTime, IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON, 0);
            break;
        case SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP:
            //TODO mPowerManager->GoToSleep(eventTime, IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON, IPowerManager::GO_TO_SLEEP_FLAG_NO_DOZE);
            break;
        case SHORT_PRESS_POWER_REALLY_GO_TO_SLEEP_AND_GO_HOME:
            //TODO mPowerManager->GoToSleep(eventTime, IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON, IPowerManager::GO_TO_SLEEP_FLAG_NO_DOZE);
            LaunchHomeFromHotKey();
            break;
    }
}

void PhoneWindowManager::ShowGlobalActionsInternal()
{
    SendCloseSystemWindows(SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS);
    if (mGlobalActions == NULL) {
        mGlobalActions = new GlobalActions(mContext, mWindowManagerFuncs);
    }
    const Boolean keyguardShowing = KeyguardIsShowingTq();
    mGlobalActions->ShowDialog(keyguardShowing, IsDeviceProvisioned());
    if (keyguardShowing) {
        // since it took two seconds of long press to bring this up,
        // poke the wake lock so they have some time to see the dialog.
        mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    }
}

Boolean PhoneWindowManager::IsDeviceProvisioned()
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    return Settings::Global::GetInt32(
                contentResolver, ISettingsGlobal::DEVICE_PROVISIONED, 0) != 0;
}

Boolean PhoneWindowManager::IsUserSetupComplete()
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 value;
    Settings::Secure::GetInt32ForUser(
            contentResolver, ISettingsSecure::USER_SETUP_COMPLETE, 0, IUserHandle::USER_CURRENT, &value);
    return value != 0;
}

void PhoneWindowManager::HandleLongPressOnHome()
{
    if (mLongPressOnHomeBehavior != LONG_PRESS_HOME_NOTHING) {
        mHomeConsumed = TRUE;
        Boolean result;
        PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &result);

        if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_SYSTEM_UI) {
            ToggleRecentApps();
        } else if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_ASSIST) {
            LaunchAssistAction();
        }
    }
}

void PhoneWindowManager::HandleDoubleTapOnHome()
{
    if (mDoubleTapOnHomeBehavior == DOUBLE_TAP_HOME_RECENT_SYSTEM_UI) {
        mHomeConsumed = TRUE;
        ToggleRecentApps();
    }
}

void PhoneWindowManager::ReadConfigurationDependentBehaviors()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_longPressOnHomeBehavior, &mLongPressOnHomeBehavior);
    if (mLongPressOnHomeBehavior < LONG_PRESS_HOME_NOTHING ||
            mLongPressOnHomeBehavior > LONG_PRESS_HOME_ASSIST) {
        mLongPressOnHomeBehavior = LONG_PRESS_HOME_NOTHING;
    }

    resources->GetInteger(R::integer::config_doubleTapOnHomeBehavior, &mDoubleTapOnHomeBehavior);
    if (mDoubleTapOnHomeBehavior < DOUBLE_TAP_HOME_NOTHING ||
            mDoubleTapOnHomeBehavior > DOUBLE_TAP_HOME_RECENT_SYSTEM_UI) {
        mDoubleTapOnHomeBehavior = LONG_PRESS_HOME_NOTHING;
    }
}

Boolean PhoneWindowManager::CanHideNavigationBar()
{
    Boolean bTemp;
    return mHasNavigationBar && !(mAccessibilityManager->IsTouchExplorationEnabled(&bTemp), bTemp);
}

void PhoneWindowManager::UpdateWakeGestureListenerLp()
{
    if (ShouldEnableWakeGestureLp()) {
        mWakeGestureListener->RequestWakeUpTrigger();
    } else {
        mWakeGestureListener->CancelWakeUpTrigger();
    }
}

Boolean PhoneWindowManager::ShouldEnableWakeGestureLp()
{
    Boolean bTemp;
    mWakeGestureListener->IsSupported(&bTemp);
    return mWakeGestureEnabledSetting && !mAwake
        && (!mLidControlsSleep || mLidState != IWindowManagerPolicyWindowManagerFuncs::LID_CLOSED)
        && bTemp;
}

void PhoneWindowManager::ReadCameraLensCoverState()
{
    mWindowManagerFuncs->GetCameraLensCoverState(&mCameraLensCoverState);
}

void PhoneWindowManager::AwakenDreams()
{
    AutoPtr<IIDreamManager> dreamManager = GetDreamManager();
    if (dreamManager != NULL) {
        //try {
            dreamManager->Awaken();
        //} catch (RemoteException e) {
            // fine, stay asleep then
        //}
    }
}

ECode PhoneWindowManager::ShowGlobalActions()
{
    mHandler->RemoveMessages(MSG_DISPATCH_SHOW_GLOBAL_ACTIONS);
    Boolean isSucceed;
    mHandler->SendEmptyMessage(MSG_DISPATCH_SHOW_GLOBAL_ACTIONS, &isSucceed);
    return NOERROR;
}

ECode PhoneWindowManager::Init(
    /* [in] */ IContext* context,
    /* [in] */ IIWindowManager* windowManager,
    /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs)
{
    mContext = context;
    mWindowManager = windowManager;
    mWindowManagerFuncs = windowManagerFuncs;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);

    mWindowManagerInternal = IWindowManagerInternal::Probe(LocalServices::GetService(EIID_IWindowManagerInternal));
    mDreamManagerInternal = IDreamManagerInternal::Probe(LocalServices::GetService(EIID_IDreamManagerInternal));

    AutoPtr<PolicyHandler> ph = new PolicyHandler(this);
    ph->constructor();
    mHandler = (IHandler*)ph.Get();
    mWakeGestureListener = new MyWakeGestureListener(mContext, mHandler, this);
    mOrientationListener = new MyOrientationListener(mContext, mHandler, this);
    mSettingsObserver = new SettingsObserver(mHandler, this);
    mSettingsObserver->Observe();
    mShortcutManager = new ShortcutManager(context, mHandler);
    mShortcutManager->Observe();
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_defaultUiModeType, &mUiMode);
    ECode ec = CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mHomeIntent);
    if (FAILED(ec)) {
        return ec;
    }

    mHomeIntent->AddCategory(IIntent::CATEGORY_HOME);
    mHomeIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

    ec = CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mCarDockIntent);
    if (FAILED(ec)) {
        return ec;
    }

    mCarDockIntent->AddCategory(IIntent::CATEGORY_CAR_DOCK);
    mCarDockIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

    ec = CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&mDeskDockIntent);
    if (FAILED(ec)) {
        return ec;
    }

    mDeskDockIntent->AddCategory(IIntent::CATEGORY_DESK_DOCK);
    mDeskDockIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    mPowerManager = IPowerManager::Probe(service);
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("PhoneWindowManager.mBroadcastWakeLock"), (IPowerManagerWakeLock**)&mBroadcastWakeLock);
    String equalsStr2;
    sp->Get(String("ro.debuggable"), &equalsStr2);
    mEnableShiftMenuBugReports = String("1").Equals(equalsStr2);
    resources = NULL;
    mContext->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_supportAutoRotation, &mSupportAutoRotation);

    mLidOpenRotation = ReadRotation(
            R::integer::config_lidOpenRotation);
    mCarDockRotation = ReadRotation(
            R::integer::config_carDockRotation);
    mDeskDockRotation = ReadRotation(
            R::integer::config_deskDockRotation);
    mUndockedHdmiRotation = ReadRotation(R::integer::config_undockedHdmiRotation);
    resources->GetBoolean(R::bool_::config_carDockEnablesAccelerometer,
            &mCarDockEnablesAccelerometer);
    resources->GetBoolean(R::bool_::config_deskDockEnablesAccelerometer,
            &mDeskDockEnablesAccelerometer);
    resources->GetInteger(R::integer::config_lidKeyboardAccessibility,
            &mLidKeyboardAccessibility);
    resources->GetInteger(R::integer::config_lidNavigationAccessibility,
            &mLidNavigationAccessibility);
    resources->GetBoolean(R::bool_::config_lidControlsSleep,
            &mLidControlsSleep);
    resources->GetBoolean(R::bool_::config_enableTranslucentDecor, &mTranslucentDecorEnabled);
    ReadConfigurationDependentBehaviors();

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mAccessibilityManager = IAccessibilityManager::Probe(obj);

    // register for dock events
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IUiModeManager::ACTION_ENTER_CAR_MODE);
    filter->AddAction(IUiModeManager::ACTION_EXIT_CAR_MODE);
    filter->AddAction(IUiModeManager::ACTION_ENTER_DESK_MODE);
    filter->AddAction(IUiModeManager::ACTION_EXIT_DESK_MODE);
    filter->AddAction(IIntent::ACTION_DOCK_EVENT);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mDockReceiver, filter, (IIntent**)&intent);
    if (intent != NULL) {
       // Retrieve current sticky dock event broadcast.
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &mDockMode);
    }

    // register for dream-related broadcasts
    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_DREAMING_STARTED);
    filter->AddAction(IIntent::ACTION_DREAMING_STOPPED);
    AutoPtr<IIntent> intentTmp;
    context->RegisterReceiver(mDreamReceiver, filter, (IIntent**)&intentTmp);

    // register for multiuser-relevant broadcasts
    filter = NULL;
    intentTmp = NULL;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    context->RegisterReceiver(mMultiuserReceiver, filter, (IIntent**)&intentTmp);

    CSystemGesturesPointerEventListener::New(context, new SystemGesturesPointerEventListenerCallbacks(this)
            , (ISystemGesturesPointerEventListener**)&mSystemGestures);
    CImmersiveModeConfirmation::New(mContext, (IImmersiveModeConfirmation**)&mImmersiveModeConfirmation);
    mWindowManagerFuncs->RegisterPointerEventListener(IPointerEventListener::Probe(mSystemGestures));

    service = NULL;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&service);
    mVibrator = IVibrator::Probe(service);

    mLongPressVibePattern = GetLongIntArray(resources,
            R::array::config_longPressVibePattern);
    mVirtualKeyVibePattern = GetLongIntArray(resources,
            R::array::config_virtualKeyVibePattern);
    mKeyboardTapVibePattern = GetLongIntArray(resources,
             R::array::config_keyboardTapVibePattern);
    mClockTickVibePattern = GetLongIntArray(resources,
             R::array::config_clockTickVibePattern);
    mCalendarDateVibePattern = GetLongIntArray(resources,
             R::array::config_calendarDateVibePattern);
    mSafeModeDisabledVibePattern = GetLongIntArray(resources,
            R::array::config_safeModeDisabledVibePattern);
    mSafeModeEnabledVibePattern = GetLongIntArray(resources,
            R::array::config_safeModeEnabledVibePattern);

    resources->GetBoolean(R::bool_::config_enableScreenshotChord, &mScreenshotChordEnabled);

    mGlobalKeyManager = new GlobalKeyManager(mContext);

    // Controls rotation and the like.
    InitializeHdmiState();

    // Match current screen state.
    Boolean isInteractive;
    if (!(mPowerManager->IsInteractive(&isInteractive), isInteractive)) {
        GoingToSleep(IWindowManagerPolicy::OFF_BECAUSE_OF_USER);
    }
    return NOERROR;
}

ECode PhoneWindowManager::SetInitialDisplaySize(
    /* [in] */ IDisplay* display,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density)
{
    // This method might be called before the policy has been fully initialized
    // or for other displays we don't care about.
    Int32 displayId;
    display->GetDisplayId(&displayId);
    if (mContext == NULL || displayId != IDisplay::DEFAULT_DISPLAY) {
        return NOERROR;
    }
    mDisplay = display;

    Int32 shortSize = 0, longSize = 0;
    Boolean tmp = FALSE;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    if (width > height) {
        shortSize = height;
        longSize = width;
        mLandscapeRotation = ISurface::ROTATION_0;
        mSeascapeRotation = ISurface::ROTATION_180;
        if (res->GetBoolean(R::bool_::config_reverseDefaultRotation, &tmp), tmp) {
            mPortraitRotation = ISurface::ROTATION_90;
            mUpsideDownRotation = ISurface::ROTATION_270;
        } else {
            mPortraitRotation = ISurface::ROTATION_270;
            mUpsideDownRotation = ISurface::ROTATION_90;
        }
    } else {
        shortSize = width;
        longSize = height;
        mPortraitRotation = ISurface::ROTATION_0;
        mUpsideDownRotation = ISurface::ROTATION_180;
        if (res->GetBoolean(R::bool_::config_reverseDefaultRotation, &tmp), tmp) {
            mLandscapeRotation = ISurface::ROTATION_270;
            mSeascapeRotation = ISurface::ROTATION_90;
        } else {
            mLandscapeRotation = ISurface::ROTATION_90;
            mSeascapeRotation = ISurface::ROTATION_270;
        }
    }

    res->GetDimensionPixelSize(R::dimen::status_bar_height, &mStatusBarHeight);

    // Height of the navigation bar when presented horizontally at bottom
    Int32 dim = 0;
    res->GetDimensionPixelSize(R::dimen::navigation_bar_height, &dim);
    mNavigationBarHeightForRotation[mPortraitRotation] =
    mNavigationBarHeightForRotation[mUpsideDownRotation] = dim;

    res->GetDimensionPixelSize(R::dimen::navigation_bar_height_landscape, &dim);
    mNavigationBarHeightForRotation[mLandscapeRotation] =
    mNavigationBarHeightForRotation[mSeascapeRotation] = dim;

    // Width of the navigation bar when presented vertically along one side
    res->GetDimensionPixelSize(R::dimen::navigation_bar_width, &dim);
    mNavigationBarWidthForRotation[mPortraitRotation] =
    mNavigationBarWidthForRotation[mUpsideDownRotation] =
    mNavigationBarWidthForRotation[mLandscapeRotation] =
    mNavigationBarWidthForRotation[mSeascapeRotation] = dim;

    // SystemUI (status bar) layout policy
    Int32 shortSizeDp = shortSize * IDisplayMetrics::DENSITY_DEFAULT / density;
    Int32 longSizeDp = longSize * IDisplayMetrics::DENSITY_DEFAULT / density;

    // Allow the navigation bar to move on small devices (phones).
    mNavigationBarCanMove = shortSizeDp < 600;

    res->GetBoolean(R::bool_::config_showNavigationBar, &mHasNavigationBar);
    // Allow a system property to override this. Used by the emulator.
    // See also hasNavigationBar().
    String navBarOverride;
    SystemProperties::Get(String("qemu.hw.mainkeys"), &navBarOverride);
    if (String("1").Equals(navBarOverride)) {
        mHasNavigationBar = FALSE;
    } else if (String("0").Equals(navBarOverride)) {
        mHasNavigationBar = TRUE;
    }

    // For demo purposes, allow the rotation of the HDMI display to be controlled.
    // By default, HDMI locks rotation to landscape.
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String equalsStr;
    sp->Get(String("persist.demo.hdmirotation"), &equalsStr);
    if (String("portrait").Equals(equalsStr) == 0) {
        mDemoHdmiRotation = mPortraitRotation;
    } else {
        mDemoHdmiRotation = mLandscapeRotation;
    }

    SystemProperties::GetBoolean(String("persist.demo.hdmirotationlock"), FALSE, &mDemoHdmiRotationLock);

    // Only force the default orientation if the screen is xlarge, at least 960dp x 720dp, per
    // http://developer.android.com/guide/practices/screens_support.html#range
    Boolean forceDefaultOrientation;
    String forced_orient;
    res->GetBoolean(R::bool_::config_forceDefaultOrientation, &forceDefaultOrientation);
    SystemProperties::Get(String("config.override_forced_orient"), &forced_orient);
    mForceDefaultOrientation = longSizeDp >= 960 && shortSizeDp >= 720 &&
    forceDefaultOrientation &&
    // For debug purposes the next line turns this feature off with:
    // $ adb shell setprop config.override_forced_orient true
    // $ adb shell wm size reset
    !String("true").Equals(forced_orient);

    return NOERROR;
}

ECode PhoneWindowManager::IsDefaultOrientationForced(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mForceDefaultOrientation;
    return NOERROR;
}

ECode PhoneWindowManager::SetDisplayOverscan(
    /* [in] */ IDisplay* display,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 displayId;
    display->GetDisplayId(&displayId);
    if (displayId == IDisplay::DEFAULT_DISPLAY) {
        mOverscanLeft = left;
        mOverscanTop = top;
        mOverscanRight = right;
        mOverscanBottom = bottom;
    }
    return NOERROR;
}

ECode PhoneWindowManager::UpdateSettings()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean updateRotation = FALSE;
    {
        AutoLock lock(mLock);
        Settings::System::GetInt32ForUser(resolver,
                ISettingsSystem::END_BUTTON_BEHAVIOR,
                ISettingsSystem::END_BUTTON_BEHAVIOR_DEFAULT,
                IUserHandle::USER_CURRENT, &mEndcallBehavior);
        Settings::Secure::GetInt32ForUser(resolver,
                ISettingsSecure::INCALL_POWER_BUTTON_BEHAVIOR,
                ISettingsSecure::INCALL_POWER_BUTTON_BEHAVIOR_DEFAULT,
                IUserHandle::USER_CURRENT, &mIncallPowerBehavior);

        // Configure wake gesture.
        Int32 iTemp;
        Boolean wakeGestureEnabledSetting =
            (Settings::Secure::GetInt32ForUser(resolver, ISettingsSecure::WAKE_GESTURE_ENABLED, 0, IUserHandle::USER_CURRENT, &iTemp), iTemp) != 0;
        if (mWakeGestureEnabledSetting != wakeGestureEnabledSetting) {
            mWakeGestureEnabledSetting = wakeGestureEnabledSetting;
            UpdateWakeGestureListenerLp();
        }

        // Configure rotation lock.
        Int32 userRotation;
        Settings::System::GetInt32ForUser(resolver,
                ISettingsSystem::USER_ROTATION, ISurface::ROTATION_0,
                IUserHandle::USER_CURRENT, &userRotation);
        if (mUserRotation != userRotation) {
            mUserRotation = userRotation;
            updateRotation = TRUE;
        }
        Int32 userRotationMode, value;
        Settings::System::GetInt32ForUser(resolver,
                ISettingsSystem::ACCELEROMETER_ROTATION, 0, IUserHandle::USER_CURRENT, &value);
        userRotationMode = value != 0 ? IWindowManagerPolicy::USER_ROTATION_FREE
                : IWindowManagerPolicy::USER_ROTATION_LOCKED;
        if (mUserRotationMode != userRotationMode) {
            mUserRotationMode = userRotationMode;
            updateRotation = TRUE;
            UpdateOrientationListenerLp();
        }

        if (mSystemReady) {
            Int32 pointerLocation;
            Settings::System::GetInt32ForUser(resolver,
                    ISettingsSystem::POINTER_LOCATION, 0, IUserHandle::USER_CURRENT, &pointerLocation);
            if (mPointerLocationMode != pointerLocation) {
                mPointerLocationMode = pointerLocation;

                AutoPtr<IMessage> message;
                Int32 what = pointerLocation != 0 ?
                        MSG_ENABLE_POINTER_LOCATION : MSG_DISABLE_POINTER_LOCATION;
                mHandler->ObtainMessage(what, (IMessage**)&message);
                Boolean isSuccess = FALSE;
                mHandler->SendMessage(message, &isSuccess);
            }
        }
        // use screen off timeout setting as the timeout for the lockscreen
        Settings::System::GetInt32ForUser(resolver,
                ISettingsSystem::SCREEN_OFF_TIMEOUT, 0, IUserHandle::USER_CURRENT, &mLockScreenTimeout);
        String imId;
        Settings::Secure::GetStringForUser(resolver, ISettingsSecure::DEFAULT_INPUT_METHOD,
                IUserHandle::USER_CURRENT, &imId);
        Boolean hasSoftInput = imId != NULL && imId.GetLength() > 0;
        if (mHasSoftInput != hasSoftInput) {
            mHasSoftInput = hasSoftInput;
            updateRotation = TRUE;
        }
        if (mImmersiveModeConfirmation != NULL) {
            mImmersiveModeConfirmation->LoadSetting(mCurrentUserId);
        }
        WindowManagerPolicyControl::ReloadFromSetting(mContext);
    }
    if (updateRotation) {
        UpdateRotation(TRUE);
    }
    return NOERROR;
}

void PhoneWindowManager::EnablePointerLocation()
{
    if (mPointerLocationView == NULL) {
        CPointerLocationView::New(mContext, (IPointerLocationView**)&mPointerLocationView);
        mPointerLocationView->SetPrintCoords(FALSE);

        AutoPtr<IWindowManagerLayoutParams> lp;
        CWindowManagerLayoutParams::New(
                IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::MATCH_PARENT,
                (IWindowManagerLayoutParams**)&lp);
        lp->SetType(IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY);
        lp->SetFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN);
        if (CActivityManager::IsHighEndGfx()) {
            Int32 flags = 0;
            lp->GetFlags(&flags);
            lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);

            Int32 privateFlags = 0;
            lp->GetPrivateFlags(&privateFlags);
            lp->SetPrivateFlags(privateFlags | IWindowManagerLayoutParams::PRIVATE_FLAG_FORCE_HARDWARE_ACCELERATED);
        }
        lp->SetFormat(IPixelFormat::TRANSLUCENT);
        AutoPtr<ICharSequence> title;
        CString::New(String("PointerLocation"), (ICharSequence**)&title);
        lp->SetTitle(title);

        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        AutoPtr<IViewManager> vm = IViewManager::Probe(service);

        Int32 inputFeatures = 0;
        lp->GetInputFeatures(&inputFeatures);
        lp->SetInputFeatures(inputFeatures | IWindowManagerLayoutParams::INPUT_FEATURE_NO_INPUT_CHANNEL);
        vm->AddView(IView::Probe(mPointerLocationView), IViewGroupLayoutParams::Probe(lp));

        mWindowManagerFuncs->RegisterPointerEventListener(IPointerEventListener::Probe(mPointerLocationView));
    }
}

void PhoneWindowManager::DisablePointerLocation()
{
    if (mPointerLocationView != NULL) {
        mWindowManagerFuncs->UnregisterPointerEventListener(IPointerEventListener::Probe(mPointerLocationView));

        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        AutoPtr<IViewManager> vm = IViewManager::Probe(service);

        vm->RemoveView(IView::Probe(mPointerLocationView));
        mPointerLocationView = NULL;
    }
}

Int32 PhoneWindowManager::ReadRotation(
    /* [in] */  Int32 resID)
{
    // try {
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 rotation = 0;
    resources->GetInteger(resID, &rotation);
    switch (rotation) {
        case 0:
            return ISurface::ROTATION_0;
        case 90:
            return ISurface::ROTATION_90;
        case 180:
            return ISurface::ROTATION_180;
        case 270:
            return ISurface::ROTATION_270;
        }
    // } catch (Resources.NotFoundException e) {
    //     // fall through
    // }
    return -1;
}

ECode PhoneWindowManager::CheckAddPermission(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ ArrayOf<Int32>* outAppOp,
    /* [out] */ Int32* addPermisssion)
{
    if (addPermisssion == NULL) {
        return E_INVALID_ARGUMENT;
    }

    (*outAppOp)[0] = IAppOpsManager::OP_NONE;

    Int32 type = 0;
    attrs->GetType(&type);

    if (type < IWindowManagerLayoutParams::FIRST_SYSTEM_WINDOW
        || type > IWindowManagerLayoutParams::LAST_SYSTEM_WINDOW) {
        *addPermisssion = IWindowManagerGlobal::ADD_OKAY;
        return NOERROR;
    }

    String permission;
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_TOAST:
            // XXX right now the app process has complete control over
            // this...  should introduce a token to let the system
            // monitor/control what they are doing.
            (*outAppOp)[0] = IAppOpsManager::OP_TOAST_WINDOW;
            break;
        case IWindowManagerLayoutParams::TYPE_DREAM:
        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        case IWindowManagerLayoutParams::TYPE_WALLPAPER:
        case IWindowManagerLayoutParams::TYPE_PRIVATE_PRESENTATION:
        case IWindowManagerLayoutParams::TYPE_VOICE_INTERACTION:
            // The window manager will check these.
            break;
        case IWindowManagerLayoutParams::TYPE_PHONE:
        case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
            permission = Elastos::Droid::Manifest::permission::SYSTEM_ALERT_WINDOW;
            (*outAppOp)[0] = IAppOpsManager::OP_SYSTEM_ALERT_WINDOW;
            break;
        default:
            permission = Elastos::Droid::Manifest::permission::INTERNAL_SYSTEM_WINDOW;
            break;
    }

    if (!permission.IsNull()) {
        Int32 perm;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(permission, &perm));
        if (perm != IPackageManager::PERMISSION_GRANTED) {
                return IWindowManagerGlobal::ADD_PERMISSION_DENIED;
        }
    }

    *addPermisssion = IWindowManagerGlobal::ADD_OKAY;

    return NOERROR;
}

ECode PhoneWindowManager::CheckShowToOwnerOnly(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* result)
{
    assert(attrs != NULL && result != NULL);
    Int32 type = 0;
    attrs->GetType(&type);
    // If this switch statement is modified, modify the comment in the declarations of
    // the type in {@link WindowManager.LayoutParams} as well.
    switch (type) {
        default: {
            Int32 privateFlags = 0;
            attrs->GetPrivateFlags(&privateFlags);
            // These are the windows that by default are shown only to the user that created
            // them. If this needs to be overridden, set
            // {@link WindowManager.LayoutParams.PRIVATE_FLAG_SHOW_FOR_ALL_USERS} in
            // {@link WindowManager.LayoutParams}. Note that permission
            // {@link android.Manifest.permission.INTERNAL_SYSTEM_WINDOW} is required as well.
            if ((privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS) == 0) {
                *result = TRUE;
                return NOERROR;
            }
            break;
        }

        // These are the windows that by default are shown to all users. However, to
        // protect against spoofing, check permissions below.
        case IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING:
        case IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS:
        case IWindowManagerLayoutParams::TYPE_DISPLAY_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER:
        case IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM:
        case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
        case IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
        case IWindowManagerLayoutParams::TYPE_PHONE:
        case IWindowManagerLayoutParams::TYPE_POINTER:
        case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_SEARCH_BAR:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
        case IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND:
        case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_PRIVATE_PRESENTATION:
            break;
    }

    // Check if third party app has set window to system window type.
    Int32 tmp = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::INTERNAL_SYSTEM_WINDOW,
            &tmp));
    *result = tmp != IPackageManager::PERMISSION_GRANTED;

    return NOERROR;
}

ECode PhoneWindowManager::AdjustWindowParamsLw(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    Int32 type = 0;
    attrs->GetType(&type);
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY:
        {
            // These types of windows can't receive input events.
            Int32 flags = 0;
            attrs->GetFlags(&flags);
            flags |= IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
            flags &= ~IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH;
            attrs->SetFlags(flags);
            break;
        }
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        {
            // If the Keyguard is in a hidden state (occluded by another window), we force to
            // remove the wallpaper and keyguard flag so that any change in-flight after setting
            // the keyguard as occluded wouldn't set these flags again.
            // See {@link #processKeyguardSetHiddenResultLw}.
            if (mKeyguardHidden) {
                Int32 flags = 0;
                attrs->GetFlags(&flags);
                flags &= ~IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER;
                attrs->SetFlags(flags);
                Int32 privateFlags;
                attrs->GetPrivateFlags(&privateFlags);
                privateFlags &= ~IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD;
                attrs->SetPrivateFlags(privateFlags);
            }
            break;
        }
    }

    if (type != IWindowManagerLayoutParams::TYPE_STATUS_BAR) {
        // The status bar is the only window allowed to exhibit keyguard behavior.
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        privateFlags &= ~IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD;
        attrs->SetPrivateFlags(privateFlags);
    }

    Int32 flags = 0;
    attrs->GetFlags(&flags);
    Boolean bTemp;
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->IsHighEndGfx(&bTemp);
    if (bTemp && (flags & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) != 0)
    {
        Int32 subtreeSystemUiVisibility;
        attrs->GetSubtreeSystemUiVisibility(&subtreeSystemUiVisibility);
        subtreeSystemUiVisibility |= IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
        attrs->SetSubtreeSystemUiVisibility(subtreeSystemUiVisibility);
    }

    return NOERROR;
}

void PhoneWindowManager::ReadLidState()
{
    mWindowManagerFuncs->GetLidState(&mLidState);
}

Boolean PhoneWindowManager::IsHidden(
    /* [in] */ Int32 accessibilityMode)
{
    switch (accessibilityMode) {
        case 1:
            return mLidState == IWindowManagerPolicyWindowManagerFuncs::LID_CLOSED;
        case 2:
            return mLidState == IWindowManagerPolicyWindowManagerFuncs::LID_OPEN;
        default:
            return FALSE;
    }
}

ECode PhoneWindowManager::AdjustConfigurationLw(
    /* [in] */ IConfiguration* _config,
    /* [in] */ Int32 keyboardPresence,
    /* [in] */ Int32 navigationPresence)
{
    assert(_config);
    CConfiguration* config = (CConfiguration*)_config;

    mHaveBuiltInKeyboard = (keyboardPresence & PRESENCE_INTERNAL) != 0;

    ReadConfigurationDependentBehaviors();
    ReadLidState();
    ApplyLidSwitchState();

    if (config->mKeyboard == IConfiguration::KEYBOARD_NOKEYS
            || (keyboardPresence == PRESENCE_INTERNAL
                    && IsHidden(mLidKeyboardAccessibility))) {
        config->mHardKeyboardHidden = IConfiguration::HARDKEYBOARDHIDDEN_YES;
        if (!mHasSoftInput) {
            config->mKeyboardHidden = IConfiguration::KEYBOARDHIDDEN_YES;
        }
    }

    if (config->mNavigation == IConfiguration::NAVIGATION_NONAV
            || (navigationPresence == PRESENCE_INTERNAL
                    && IsHidden(mLidNavigationAccessibility))) {
        config->mNavigationHidden = IConfiguration::NAVIGATIONHIDDEN_YES;
    }

    return NOERROR;
}

ECode PhoneWindowManager::WindowTypeToLayerLw(
    /* [in] */ Int32 type,
    /* [out] */ Int32* layer)
{
    VALIDATE_NOT_NULL(layer)

    *layer = 2;
    if (type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW && type <=
        IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW) {
        return NOERROR;
    }

    switch (type) {
    case IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND:
        *layer = 1;
        break;
    case IWindowManagerLayoutParams::TYPE_PRIVATE_PRESENTATION:
        *layer = 2;
        break;
    case IWindowManagerLayoutParams::TYPE_WALLPAPER:
        // wallpaper is at the bottom, though the window manager may move it.
        *layer = 2;
        break;
    case IWindowManagerLayoutParams::TYPE_PHONE:
        *layer = 3;
        break;
    case IWindowManagerLayoutParams::TYPE_SEARCH_BAR:
        *layer = 4;
        break;
    case IWindowManagerLayoutParams::TYPE_VOICE_INTERACTION:
        // voice interaction layer is almost immediately above apps.
        *layer = 5;
        break;
    case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY:
    case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
        *layer = 6;
        break;
    case IWindowManagerLayoutParams::TYPE_TOAST:
        // toasts and the plugged-in battery thing
        *layer = 7;
        break;
    case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        // SIM errors and unlock.  Not sure if this really should be in a high layer.
        *layer = 8;
        break;
    case IWindowManagerLayoutParams::TYPE_DREAM:
        // used for Dreams (screensavers with TYPE_DREAM windows)
        *layer = 9;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
        // like the ANR / app crashed dialogs
        *layer = 10;
        break;
    case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        // on-screen keyboards and other such input method user interfaces go here.
        *layer = 11;
        break;
    case IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG:
        // on-screen keyboards and other such input method user interfaces go here.
        *layer = 12;
        break;
    case IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM:
        *layer = 13;
        break;
    case IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL:
        *layer = 14;
        break;
    case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        *layer = 15;
        break;
    case IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL:
        *layer = 16;
        break;
    case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
        *layer = 17;
        break;
    case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
        // the on-screen volume indicator and controller shown when the user
        // changes the device volume
        *layer = 18;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
        // the on-screen volume indicator and controller shown when the user
        // changes the device volume
        *layer = 19;
        break;
    case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        // the navigation bar, if available, shows atop most things
        *layer = 20;
        break;
    case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
        // some panels (e.g. search) need to show on top of the navigation bar
        *layer = 21;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
        // system-level error dialogs
        *layer = 22;
        break;
    case IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY:
        // used to highlight the magnified portion of a display
        *layer = 23;
        break;
    case IWindowManagerLayoutParams::TYPE_DISPLAY_OVERLAY:
        // used to simulate secondary display devices
        *layer = 24;
        break;
    case IWindowManagerLayoutParams::TYPE_DRAG:
        // the drag layer: input for drag-and-drop is associated with this window,
        // which sits above all other focusable windows
        *layer = 25;
        break;
    case IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY:
        *layer = 26;
        break;
    case IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS:
        *layer = 27;
        break;
    case IWindowManagerLayoutParams::TYPE_POINTER:
        // the (mouse) pointer layer
        *layer = 28;
        break;
    case IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER:
        *layer = 29;
        break;
    }
    // Log.e(TAG, "Unknown window type: " + type);

    return NOERROR;
}

ECode PhoneWindowManager::SubWindowTypeToLayerLw(
    /* [in] */ Int32 type,
    /* [out] */ Int32* layer)
{
    VALIDATE_NOT_NULL(layer)

    switch (type) {
        case IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG:
            *layer = APPLICATION_PANEL_SUBLAYER;
            break;
        case IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA:
            *layer = APPLICATION_MEDIA_SUBLAYER;
            break;
        case IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA_OVERLAY:
            *layer = APPLICATION_MEDIA_OVERLAY_SUBLAYER;
            break;
        case IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL:
            *layer = APPLICATION_SUB_PANEL_SUBLAYER;
            break;
        default:
            // Logger::D(TAG, StringBuffer("Unknown sub-window type: ") + type);
            *layer = 0;
    }

    return NOERROR;
}

ECode PhoneWindowManager::GetMaxWallpaperLayer(
    /* [out] */ Int32* layer)
{
    return WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_STATUS_BAR, layer);
}

ECode PhoneWindowManager::GetAboveUniverseLayer(
    /* [out] */ Int32* layer)
{
    return WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR, layer);
}

ECode PhoneWindowManager::GetNonDecorDisplayWidth(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    if (mHasNavigationBar) {
        // For a basic navigation bar, when we are in portrait mode we place
        // the navigation bar to the bottom.
        if (!mNavigationBarCanMove || fullWidth < fullHeight) {
            *width = fullWidth - mNavigationBarWidthForRotation[rotation];
            return NOERROR;
        }
    }
    *width = fullWidth;
    return NOERROR;
}

ECode PhoneWindowManager::GetNonDecorDisplayHeight(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    if (mHasNavigationBar) {
        // For a basic navigation bar, when we are in portrait mode we place
        // the navigation bar to the bottom.
        if (!mNavigationBarCanMove || fullWidth < fullHeight) {
            *height = fullHeight - mNavigationBarHeightForRotation[rotation];
            return NOERROR;
        }
    }
    *height = fullHeight;
    return NOERROR;
}

ECode PhoneWindowManager::GetConfigDisplayWidth(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* value)
{
    return GetNonDecorDisplayWidth(fullWidth, fullHeight, rotation, value);
}

ECode PhoneWindowManager::GetConfigDisplayHeight(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* value)
{
    // If we don't have a system nav bar, then there is a separate status
    // bar at the top of the display.  We don't count that as part of the
    // fixed decor, since it can hide; however, for purposes of configurations,
    // we do want to exclude it since applications can't generally use that part
    // of the screen.
    return GetNonDecorDisplayHeight(fullWidth, fullHeight, rotation, value);
}

ECode PhoneWindowManager::IsForceHiding(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 privateFlags;
    attrs->GetPrivateFlags(&privateFlags);
    Int32 type;
    attrs->GetType(&type);
    Boolean keyguardHostWindow;
    IsKeyguardHostWindow(attrs, &keyguardHostWindow);
    *result = (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0 ||
    (keyguardHostWindow && IsKeyguardSecureIncludingHidden()) ||
    (type == IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM);

    return NOERROR;
}

ECode PhoneWindowManager::IsKeyguardHostWindow(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 type = 0;
    attrs->GetType(&type);
    *result = type == IWindowManagerLayoutParams::TYPE_STATUS_BAR;

    return NOERROR;
}

ECode PhoneWindowManager::CanBeForceHidden(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* hiden)
{
    VALIDATE_NOT_NULL(hiden)

    Int32 type = 0;
    attrs->GetType(&type);
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        case IWindowManagerLayoutParams::TYPE_WALLPAPER:
        case IWindowManagerLayoutParams::TYPE_DREAM:
        case IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND:
        case IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM:
            *hiden = FALSE;
            break;
        default:
            *hiden = TRUE;
    }

    return NOERROR;
}

ECode PhoneWindowManager::GetWinShowWhenLockedLw(
    /* [out] */ IWindowState** ws)
{
    VALIDATE_NOT_NULL(ws);
    *ws = mWinShowWhenLocked;
    REFCOUNT_ADD(*ws);
    return NOERROR;
}

ECode PhoneWindowManager::AddStartingWindow(
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 theme,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon,
    /* [in] */ Int32 logo,
    /* [in] */ Int32 windowFlags,
    /* [out] */ IView** window)
{
    VALIDATE_NOT_NULL(window);
    *window = NULL;

    if (!SHOW_STARTING_ANIMATIONS) {
        return NOERROR;
    }

    if (packageName.IsNullOrEmpty()) {
        return NOERROR;
    }

    ECode ec;
    //try {
        AutoPtr<IContext> context = mContext;
        if (DEBUG_STARTING_WINDOW) {
            Slogger::D(TAG, "addStartingWindow %s: nonLocalizedLabel=%p  theme=%08x",
                packageName.string(), nonLocalizedLabel, theme);
        }
        Int32 resId = 0;
        if (theme != (context->GetThemeResId(&resId), resId) || labelRes != 0) {
            AutoPtr<IContext> temp;
            ec = context->CreatePackageContext(packageName, 0, (IContext**)&temp);
            if (FAILED(ec)) {
                return ec;
            }

            context = temp;
            context->SetTheme(theme);
        }

        AutoPtr<IPolicyManager> pm;
        ec = CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
        if (FAILED(ec)) {
            return ec;
        }

        AutoPtr<IWindow> win;
        ec = pm->MakeNewWindow(context, (IWindow**)&win);
        if (FAILED(ec)) {
            return ec;
        }

        AutoPtr<ITypedArray> ta;
        win->GetWindowStyle((ITypedArray**)&ta);
        Boolean v1, v2;
        ta->GetBoolean(R::styleable::Window_windowDisablePreview, FALSE, &v1);
        ta->GetBoolean(R::styleable::Window_windowShowWallpaper, FALSE, &v2);
        if (v1 || v2) {
            return NOERROR;
        }

        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        AutoPtr<ICharSequence> title;
        r->GetText(labelRes, nonLocalizedLabel, (ICharSequence**)&title);
        win->SetTitle(title);

        win->SetType(IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING);
        // Force the window flags: this is a fake window, so it is not really
        // touchable or focusable by the user.  We also add in the ALT_FOCUSABLE_IM
        // flag because we do know that the next window will take input
        // focus, so we want to get the IME window up on top of us right away.
        win->SetFlags(windowFlags
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
                windowFlags
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);

        win->SetDefaultIcon(icon);
        win->SetDefaultLogo(logo);


        win->SetLayout(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT);

        AutoPtr<IWindowManagerLayoutParams> lp;
        win->GetAttributes((IWindowManagerLayoutParams**)&lp);

        lp->SetToken(appToken);
        lp->SetPackageName(packageName);

        Int32 windowAnimations = 0;
        ta->GetResourceId(R::styleable::Window_windowAnimationStyle
                , 0, &windowAnimations);
        lp->SetWindowAnimations(windowAnimations);

        Int32 privateFlags = 0;
        lp->GetPrivateFlags(&privateFlags);
        privateFlags |=
                (IWindowManagerLayoutParams::PRIVATE_FLAG_FAKE_HARDWARE_ACCELERATED
                | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);

        Boolean supportsScreen;
        if (compatInfo != NULL && !(compatInfo->SupportsScreen(&supportsScreen), supportsScreen)) {
            privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_COMPATIBLE_WINDOW;
        }
        lp->SetPrivateFlags(privateFlags);

        AutoPtr<ICharSequence> tl;
        CString::New(String("Starting ") + packageName, (ICharSequence**)&tl);
        lp->SetTitle(tl);

        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        AutoPtr<IViewManager> vm = IViewManager::Probe(service);
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);

        AutoPtr<IView> view;
        win->GetDecorView((IView**)&view);

        Boolean isFloating;
        win->IsFloating(&isFloating);
        if (isFloating) {
           // Whoops, there is no way to display an animation/preview
           // of such a thing!  After all that work...  let's skip it.
           // (Note that we must do this here because it is in
           // getDecorView() where the theme is evaluated...  maybe
           // we should peek the floating attribute from the theme
           // earlier.)
           return NOERROR;
        }

        ec = vm->AddView(view, IViewGroupLayoutParams::Probe(lp));
        if (FAILED(ec)) {
            return ec;
        }

        // Only return the view if it was successfully added to the
        // window manager... which we can tell by it having a parent.
        //
        AutoPtr<IViewParent> parent;
        view->GetParent((IViewParent**)&parent);
        *window = parent != NULL ? view : NULL;
        REFCOUNT_ADD(*window);

        if (DEBUG_STARTING_WINDOW) {
            Slogger::V(TAG, "Adding starting window for %s/%p: window %p",
                packageName.string(), appToken, *window);
        }
    //} catch (WindowManagerImpl.BadTokenException e) {
    //    // ignore
    //    Log.w(TAG, appToken + " already running, starting window not displayed");
    //} catch (RuntimeException e) {
    //    // don't crash if something else bad happens, for example a
    //    // failure loading resources because we are loading from an app
    //    // on external storage that has been unmounted.
    //    Log.w(TAG, appToken + " failed creating starting window", e);
    //}
    //finally {
    AutoPtr<IViewParent> parent2;
    if (view != NULL && (view->GetParent((IViewParent**)&parent2), parent2) == NULL) {
        Logger::W(TAG, "view not successfully added to wm, removing view");
        wm->RemoveViewImmediate(view);
    }
    //}

    return NOERROR;
}

ECode PhoneWindowManager::RemoveStartingWindow(
    /* [in] */ IBinder* appToken,
    /* [in] */ IView* window)
{
    if (DEBUG_STARTING_WINDOW) {
        Slogger::V(TAG, "Removing starting window for %p: window %p", appToken, window);
    }

    if (window != NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
        IViewManager::Probe(service)->RemoveView(window);
    }

    return NOERROR;
}

ECode PhoneWindowManager::PrepareAddWindowLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* added)
{
    VALIDATE_NOT_NULL(added)
    Int32 type = 0;
    attrs->GetType(&type);
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
               Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
               String("PhoneWindowManager")));

            // TODO: Need to handle the race condition of the status bar proc
            // dying and coming back before the removeWindowLw cleanup has happened.
            if (mStatusBar != NULL) {
                Boolean live = FALSE;
                if (mStatusBar->IsAlive(&live), live) {
                    *added = IWindowManagerGlobal::ADD_MULTIPLE_SINGLETON;
                }
                return NOERROR;
            }

            mStatusBar = win;
            mStatusBarController->SetWindow(win);
            mKeyguardDelegate->HideScrim();
            break;
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                   Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
                   String("PhoneWindowManager")));
            if (mNavigationBar != NULL) {
                Boolean live = FALSE;
                if (mNavigationBar->IsAlive(&live), live) {
                    return IWindowManagerGlobal::ADD_MULTIPLE_SINGLETON;
                }
            }
            mNavigationBar = win;
            mNavigationBarController->SetWindow(win);
            if (DEBUG_LAYOUT) Slogger::I(TAG, "NAVIGATION BAR: %s", TO_CSTR(mNavigationBar));
            break;
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                    Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
                    String("PhoneWindowManager")));
            break;
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL:
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
               Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
               String("PhoneWindowManager")));
            break;
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL:
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                    Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
                    String("PhoneWindowManager")));
            break;
        case IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM:
            if (mKeyguardScrim != NULL)
            {
                *added = IWindowManagerGlobal::ADD_MULTIPLE_SINGLETON;
                return NOERROR;
            }
            mKeyguardScrim = win;
            break;
    }
    *added = IWindowManagerGlobal::ADD_OKAY;

    return NOERROR;
}

ECode PhoneWindowManager::RemoveWindowLw(
    /* [in] */ IWindowState* win)
{
    if (mStatusBar.Get() == win) {
        mStatusBar = NULL;
        mStatusBarController->SetWindow(NULL);
        mKeyguardDelegate->ShowScrim();
    }
    else if (mKeyguardScrim.Get() == win){
        //Log.v(TAG, "Removing keyguard scrim");
        mKeyguardScrim = NULL;
    }
    if (mNavigationBar.Get() == win) {
        mNavigationBar = NULL;
        mNavigationBarController->SetWindow(NULL);
    }

    return NOERROR;
}

ECode PhoneWindowManager::SelectAnimationLw(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 transit,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    // if (PRINT_ANIM) Log.i(TAG, "selectAnimation in " + win
    //       + ": transit=" + transit);
    if (win == mStatusBar) {
        AutoPtr<IWindowManagerLayoutParams> attrs;
        win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        Boolean isKeyguard = (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0;
        if (transit == TRANSIT_EXIT || transit == TRANSIT_HIDE) {
            if (isKeyguard)
                *id = -1;
            else
                *id = R::anim::dock_top_exit;
            return NOERROR;
        } else if (transit == TRANSIT_ENTER || transit == TRANSIT_SHOW) {
            if (isKeyguard)
                *id = -1;
            else
                *id = R::anim::dock_top_enter;
            return NOERROR;
        }
    }
    else if (win == mNavigationBar) {
        // This can be on either the bottom or the right.
        if (mNavigationBarOnBottom) {
            if (transit == TRANSIT_EXIT || transit == TRANSIT_HIDE) {
                *id = R::anim::dock_bottom_exit;
                return NOERROR;
            }
            else if (transit == TRANSIT_ENTER || transit == TRANSIT_SHOW) {
                *id = R::anim::dock_bottom_enter;
                return NOERROR;
            }
        }
        else {
            if (transit == TRANSIT_EXIT || transit == TRANSIT_HIDE) {
                *id = R::anim::dock_right_exit;
                return NOERROR;
            }
            else if (transit == TRANSIT_ENTER || transit == TRANSIT_SHOW) {
                *id = R::anim::dock_right_enter;
                return NOERROR;
            }
        }
    }

    AutoPtr<IWindowManagerLayoutParams> attrs;
    win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    Int32 type;
    attrs->GetType(&type);

    if (transit == TRANSIT_PREVIEW_DONE) {
        Boolean hasAppShownWindows;
        win->HasAppShownWindows(&hasAppShownWindows);
        if (hasAppShownWindows) {
            if (PRINT_ANIM)
                Logger::I(TAG, "**** STARTING EXIT");
            *id = R::anim::app_starting_exit;
            return NOERROR;
        }
    }
    else if (type == IWindowManagerLayoutParams::TYPE_DREAM && mDreamingLockscreen
            && transit == TRANSIT_ENTER) {
        // Special case: we are animating in a dream, while the keyguard
        // is shown.  We don't want an animation on the dream, because
        // we need it shown immediately with the keyguard animating away
        // to reveal it.
        *id = -1;
        return NOERROR;
    }

    *id = 0;
    return NOERROR;
}

ECode PhoneWindowManager::SelectRotationAnimationLw(
    /* [in] */ ArrayOf<Int32>* anim)
{
    if (PRINT_ANIM)
        Slogger::I(TAG, "selectRotationAnimation mTopFullscreen=0x%p, rotationAnimation="
            , mTopFullscreenOpaqueWindowState.Get());//,  " rotationAnimation="
            //+ (mTopFullscreenOpaqueWindowState == null ?
            //    "0" : mTopFullscreenOpaqueWindowState.getAttrs().rotationAnimation));
    if (mTopFullscreenOpaqueWindowState != NULL && mTopIsFullscreen) {
        AutoPtr<IWindowManagerLayoutParams> winAttrs;
        mTopFullscreenOpaqueWindowState->GetAttrs((IWindowManagerLayoutParams**)&winAttrs);;
        CWindowManagerLayoutParams* wmlParams = (CWindowManagerLayoutParams*)(winAttrs.Get());
        switch (wmlParams->mRotationAnimation) {
            case IWindowManagerLayoutParams::ROTATION_ANIMATION_CROSSFADE:
                (*anim)[0] = R::anim::rotation_animation_xfade_exit;
                (*anim)[1] = R::anim::rotation_animation_enter;
                break;
            case IWindowManagerLayoutParams::ROTATION_ANIMATION_JUMPCUT:
                (*anim)[0] = R::anim::rotation_animation_jump_exit;
                (*anim)[1] = R::anim::rotation_animation_enter;
                break;
            case IWindowManagerLayoutParams::ROTATION_ANIMATION_ROTATE:
            default:
                (*anim)[0] = 0;
                (*anim)[1] = 0;
                break;
        }
    } else {
        (*anim)[0] = 0;
        (*anim)[1] = 0;
    }
    return NOERROR;
}

ECode PhoneWindowManager::CreateForceHideWallpaperExitAnimation(
    /* [in] */ Boolean goingToNotificationShade,
    /* [in] */ Boolean keyguardShowingMedia,
    /* [out] */ IAnimation** anim)
{
    VALIDATE_NOT_NULL(anim)

    *anim = NULL;
    AutoPtr<IAnimationUtils> animUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animUtils);
    if (goingToNotificationShade) {
        return NOERROR;
    } else if (keyguardShowingMedia) {
        return animUtils->LoadAnimation(mContext, R::anim::lock_screen_wallpaper_exit_noop, anim);
    } else {
        return animUtils->LoadAnimation(mContext, R::anim::lock_screen_behind_enter_fade_in, anim);
    }
    return NOERROR;
}


ECode PhoneWindowManager::ShowRecentApps()
{
    mHandler->RemoveMessages(MSG_DISPATCH_SHOW_RECENTS);
    Boolean isSucceed;
    mHandler->SendEmptyMessage(MSG_DISPATCH_SHOW_RECENTS, &isSucceed);
    return NOERROR;
}

ECode PhoneWindowManager::ValidateRotationAnimationLw(
    /* [in] */ Int32 exitAnimId,
    /* [in] */ Int32 enterAnimId,
    /* [in] */ Boolean forceDefault,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (exitAnimId) {
        case R::anim::rotation_animation_xfade_exit:
        case R::anim::rotation_animation_jump_exit:
        {
            // These are the only cases that matter.
            if (forceDefault) {
                *result = FALSE;
                return NOERROR;
            }
            AutoPtr<ArrayOf<Int32> > anim = ArrayOf<Int32>::Alloc(2);
            SelectRotationAnimationLw(anim);
            *result = (exitAnimId == (*anim)[0] && enterAnimId == (*anim)[1]);
            return NOERROR;
        }
        default:
            *result = TRUE;
    }
    return NOERROR;
}

ECode PhoneWindowManager::CreateForceHideEnterAnimation(
    /* [in] */ Boolean onWallpaper,
    /* [in] */ Boolean goingToNotificationShade,
    /* [out] */ IAnimation** anim)
{
    VALIDATE_NOT_NULL(anim);

    AutoPtr<IAnimationUtils> animUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animUtils);
    if (goingToNotificationShade) {
        return animUtils->LoadAnimation(mContext, R::anim::lock_screen_behind_enter_fade_in, anim);
    } else if (onWallpaper) {
        AutoPtr<IAnimation> a;
        animUtils->LoadAnimation(mContext, R::anim::lock_screen_behind_enter_wallpaper, (IAnimation**)&a);
        IAnimationSet* set = IAnimationSet::Probe(a);

        // TODO: Use XML interpolators when we have log interpolators available in XML.
        AutoPtr<IList> animations;
        set->GetAnimations((IList**)&animations);
        AutoPtr<IInterface> obj;
        animations->Get(0, (IInterface**)&obj);
        IAnimation::Probe(obj)->SetInterpolator(IInterpolator::Probe(mLogDecelerateInterpolator));
        obj = NULL;
        animations->Get(1, (IInterface**)&obj);
        IAnimation::Probe(obj)->SetInterpolator(IInterpolator::Probe(mLogDecelerateInterpolator));
        *anim = a;
        REFCOUNT_ADD(*anim);
        return NOERROR;
    } else {
        AutoPtr<IAnimation> a;
        animUtils->LoadAnimation(mContext, R::anim::lock_screen_behind_enter, (IAnimation**)&a);
        IAnimationSet* set = IAnimationSet::Probe(a);

        // TODO: Use XML interpolators when we have log interpolators available in XML.
        AutoPtr<IList> animations;
        set->GetAnimations((IList**)&animations);
        AutoPtr<IInterface> obj;
        animations->Get(0, (IInterface**)&obj);
        IAnimation::Probe(obj)->SetInterpolator(IInterpolator::Probe(mLogDecelerateInterpolator));
        *anim = a;
        REFCOUNT_ADD(*anim);
        return NOERROR;
    }
}

AutoPtr<IIAudioService> PhoneWindowManager::GetAudioService()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    AutoPtr<IIAudioService> audioService = IIAudioService::Probe(obj.Get());
    if (audioService == NULL) {
        Logger::E(TAG, "Unable to find IAudioService interface.");
    }
    return audioService;
}

Boolean PhoneWindowManager::KeyguardOn()
{
    Boolean restricted;
    InKeyguardRestrictedKeyInputMode(&restricted);

    return KeyguardIsShowingTq() || restricted;
}

ECode PhoneWindowManager::InterceptKeyBeforeDispatching(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean keyguardOn = KeyguardOn();

    Int32 action, keyCode, repeatCount, metaState, flags;
    event->GetAction(&action);
    event->GetKeyCode(&keyCode);
    event->GetRepeatCount(&repeatCount);
    event->GetMetaState(&metaState);
    event->GetFlags(&flags);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    Boolean canceled;
    event->IsCanceled(&canceled);

    // if (DEBUG_INPUT) {
    //     Log.d(TAG, "interceptKeyTi keyCode=" + keyCode + " down=" + down + " repeatCount="
    //             + repeatCount + " keyguardOn=" + keyguardOn + " mHomePressed=" + mHomePressed
    //             + " canceled=" + canceled);}

    // If we think we might have a volume down & power key chord on the way
    // but we're not sure, then tell the dispatcher to wait a little while and
    // try again later before dispatching.
    if (mScreenshotChordEnabled && (flags & IKeyEvent::FLAG_FALLBACK) == 0) {
        if (mVolumeDownKeyTriggered && !mPowerKeyTriggered) {
            Int64 now = SystemClock::GetUptimeMillis();
            Int64 timeoutTime = mVolumeDownKeyTime + SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS;
            if (now < timeoutTime) {
                return timeoutTime - now;
            }
        }
        if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN
                && mVolumeDownKeyConsumedByScreenshotChord) {
            if (!down) {
                mVolumeDownKeyConsumedByScreenshotChord = FALSE;
            }
            return -1;
        }
    }

    // Cancel any pending meta actions if we see any other keys being pressed between the down
    // of the meta key and its corresponding up.
    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    Boolean bTemp;
    helper->IsMetaKey(keyCode, &bTemp);
    if (mPendingMetaAction && !bTemp) {
        mPendingMetaAction = FALSE;
    }

    // First we always handle the home key here, so applications
    // can never break it, although if keyguard is on, we do let
    // it handle it, because that gives us the correct 5 second
    // timeout.
    if (keyCode == IKeyEvent::KEYCODE_HOME) {
        // If we have released the home key, and didn't do anything else
        // while it was pressed, then it is time to go home!
        if (!down) {
            CancelPreloadRecentApps();
            mHomePressed = FALSE;

            if (mHomeConsumed) {
                mHomeConsumed = FALSE;
                return -1;
            }

            if (canceled) {
                Logger::I(TAG, "Ignoring HOME; event canceled.");
                return -1;
            }

            // If an incoming call is ringing, HOME is totally disabled.
            // (The user is already on the InCallUI at this point,
            // and his ONLY options are to answer or reject the call.)
            //TODO
            //AutoPtr<ITelecomManager> telecomManager = GetTelecommService();
            //Boolean ringing;
            //if (telecomManager != NULL && (telecomManager->IsRinging(&ringing), ringing)) {
            //    Logger::I(TAG, "Ignoring HOME; there's a ringing incoming call.");
            //    return -1;
            //}
            //

            // Delay handling home if a double-tap is possible.
            if (mDoubleTapOnHomeBehavior != DOUBLE_TAP_HOME_NOTHING) {
                mHandler->RemoveCallbacks(mHomeDoubleTapTimeoutRunnable); // just in case
                mHomeDoubleTapPending = TRUE;
                Boolean isSuccessed;
                AutoPtr<IViewConfigurationHelper> vcHelper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
                Int32 doubleTapTimeout;
                vcHelper->GetDoubleTapTimeout(&doubleTapTimeout);
                mHandler->PostDelayed(mHomeDoubleTapTimeoutRunnable, doubleTapTimeout, &isSuccessed);
                return -1;
            }

            // If there's a dream running then use home to escape the dream
            // but don't actually go home.
            Boolean isDreaming;
            if (mDreamManagerInternal != NULL && (mDreamManagerInternal->IsDreaming(&isDreaming), isDreaming)) {
                mDreamManagerInternal->StopDream(FALSE/*immediate*/);
                return -1;
            }

            // Go home!
            LaunchHomeFromHotKey();
            return -1;
        }

        // If a system window has focus, then it doesn't make sense
        // right now to interact with applications.
        AutoPtr<IWindowManagerLayoutParams> attrs;
        if (win != NULL) {
            win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        }
        if (attrs != NULL) {
            Int32 type;
            attrs->GetType(&type);
            Int32 privateFlags;
            attrs->GetPrivateFlags(&privateFlags);
            if (type == IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM
                    || type == IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG
                    || (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
                // the "app" is keyguard, so give it the key
                return 0;
            }
            Int32 typeCount = WINDOW_TYPES_WHERE_HOME_DOESNT_WORK->GetLength();
            for (Int32 i = 0; i < typeCount; i++) {
                if (type == (*WINDOW_TYPES_WHERE_HOME_DOESNT_WORK)[i]) {
                    // don't do anything, but also don't pass it to the app
                    return -1;
                }
            }
        }

        // Remember that home is pressed and handle special actions.
        if (repeatCount == 0) {
            mHomePressed = TRUE;
            if (mHomeDoubleTapPending) {
                mHomeDoubleTapPending = FALSE;
                mHandler->RemoveCallbacks(mHomeDoubleTapTimeoutRunnable);
                HandleDoubleTapOnHome();
            } else if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_SYSTEM_UI
                    || mDoubleTapOnHomeBehavior == DOUBLE_TAP_HOME_RECENT_SYSTEM_UI) {
                PreloadRecentApps();
            }
        }
        else if ((flags & IKeyEvent::FLAG_LONG_PRESS) != 0) {
            if (!keyguardOn) {
                HandleLongPressOnHome();
            }
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_MENU) {
        // // Hijack modified menu keys for debugging features
        Int32 chordBug = IKeyEvent::META_SHIFT_ON;

        if (down && repeatCount == 0) {
            if (mEnableShiftMenuBugReports && (metaState & chordBug) == chordBug) {
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_BUG_REPORT, (IIntent**)&intent);

                AutoPtr<IUserHandleHelper> helper;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
                AutoPtr<IUserHandle> current;
                helper->GetCURRENT((IUserHandle**)&current);

                mContext->SendOrderedBroadcastAsUser(intent, current,
                        String(NULL), NULL, NULL, 0, String(NULL), NULL);
                return -1;
            }
            else if (SHOW_PROCESSES_ON_ALT_MENU &&
                    (metaState & IKeyEvent::META_ALT_ON) == IKeyEvent::META_ALT_ON) {
                AutoPtr<IIntent> service;
                CIntent::New((IIntent**)&service);
                service->SetClassName(mContext, String("com.android.server.LoadAverageService"));
                AutoPtr<IContentResolver> res;
                mContext->GetContentResolver((IContentResolver**)&res);
                Boolean shown = Settings::Global::GetInt32(
                        res, ISettingsGlobal::SHOW_PROCESSES, 0) != 0;
                if (!shown) {
                    AutoPtr<IComponentName> component;
                    mContext->StartService(service, (IComponentName**)&component);
                } else {
                    Boolean isSuccess = FALSE;
                    mContext->StopService(service, &isSuccess);
                }
                Boolean result;
                Settings::Global::PutInt32(
                        res, ISettingsGlobal::SHOW_PROCESSES, shown ? 0 : 1, &result);
                return -1;
            }
        }
    }
    else if (keyCode == IKeyEvent::KEYCODE_SEARCH) {
        if (down) {
            if (repeatCount == 0) {
                mSearchKeyShortcutPending = TRUE;
                mConsumeSearchKeyUp = FALSE;
            }
        } else {
            mSearchKeyShortcutPending = FALSE;
            if (mConsumeSearchKeyUp) {
                mConsumeSearchKeyUp = FALSE;
                return -1;
            }
        }
        return 0;
    }
    else if (keyCode == IKeyEvent::KEYCODE_APP_SWITCH) {
        if (!keyguardOn) {
            if (down && repeatCount == 0) {
                PreloadRecentApps();
            } else if (!down) {
                ToggleRecentApps();
            }
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_ASSIST) {
        if (down) {
            if (repeatCount == 0) {
                mAssistKeyLongPressed = FALSE;
            }
            else if (repeatCount == 1) {
                mAssistKeyLongPressed = TRUE;
                if (!keyguardOn) {
                    LaunchAssistLongPressAction();
                }
            }
        }
        else {
            if (mAssistKeyLongPressed) {
                mAssistKeyLongPressed = FALSE;
            }
            else {
                if (!keyguardOn) {
                    LaunchAssistAction();
                }
            }
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_VOICE_ASSIST) {
        if (!down) {
            AutoPtr<IIntent> voiceIntent;
            if (!keyguardOn) {
                CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&voiceIntent);
            } else {
                CIntent::New(IRecognizerIntent::ACTION_VOICE_SEARCH_HANDS_FREE, (IIntent**)&voiceIntent);
                voiceIntent->PutExtra(IRecognizerIntent::EXTRA_SECURE, TRUE);
            }
            mContext->StartActivityAsUser(voiceIntent, UserHandle::CURRENT_OR_SELF);
        }
    } else if (keyCode == IKeyEvent::KEYCODE_SYSRQ) {
        if (down && repeatCount == 0) {
            Boolean isSuccess = FALSE;
            mHandler->Post(mScreenshotRunnable, &isSuccess);
        }
        return -1;
    } else if (keyCode == IKeyEvent::KEYCODE_BRIGHTNESS_UP
            || keyCode == IKeyEvent::KEYCODE_BRIGHTNESS_DOWN) {
        if (down) {
            Int32 direction = keyCode == IKeyEvent::KEYCODE_BRIGHTNESS_UP ? 1 : -1;

            // Disable autobrightness if it's on
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            Int32 autobrightness;
            Settings::System::GetInt32ForUser(
                    resolver,
                    ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                    ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL,
                    IUserHandle::USER_CURRENT_OR_SELF,
                    &autobrightness);
            if (autobrightness != 0) {
                Boolean bTemp;
                Settings::System::PutInt32ForUser(
                        resolver,
                        ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                        ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL,
                        IUserHandle::USER_CURRENT_OR_SELF,
                        &bTemp);
            }

            Int32 min;
            mPowerManager->GetMinimumScreenBrightnessSetting(&min);
            Int32 max;
            mPowerManager->GetMaximumScreenBrightnessSetting(&max);
            Int32 step = (max - min + BRIGHTNESS_STEPS - 1) / BRIGHTNESS_STEPS * direction;
            Int32 brightness;
            Int32 screenBrightness;
            mPowerManager->GetDefaultScreenBrightnessSetting(&screenBrightness);
            Settings::System::GetInt32ForUser(
                    resolver,
                    ISettingsSystem::SCREEN_BRIGHTNESS,
                    screenBrightness,
                    IUserHandle::USER_CURRENT_OR_SELF,
                    &brightness);
            brightness += step;
            // Make sure we don't go beyond the limits.
            brightness = Elastos::Core::Math::Min(max, brightness);
            brightness = Elastos::Core::Math::Max(min, brightness);

            Boolean bTemp;
            Settings::System::PutInt32ForUser(resolver,
                    ISettingsSystem::SCREEN_BRIGHTNESS, brightness,
                    IUserHandle::USER_CURRENT_OR_SELF,
                    &bTemp);

            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_SHOW_BRIGHTNESS_DIALOG, (IIntent**)&intent);
            mContext->StartActivityAsUser(intent, UserHandle::CURRENT_OR_SELF);
        }
        return -1;
    } else if ((helper->IsMetaKey(keyCode, &bTemp), bTemp)) {
        if (down) {
            mPendingMetaAction = TRUE;
        } else if (mPendingMetaAction) {
            LaunchAssistAction(IIntent::EXTRA_ASSIST_INPUT_HINT_KEYBOARD);
        }
        return -1;
    }

    // Shortcuts are invoked through Search+key, so intercept those here
    // Any printing key that is chorded with Search should be consumed
    // even if no shortcut was invoked.  This prevents text from being
    // inadvertently inserted when using a keyboard that has built-in macro
    // shortcut keys (that emit Search+x) and some of them are not registered.
    if (mSearchKeyShortcutPending) {
        const AutoPtr<IKeyCharacterMap> kcm;
        event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
        Boolean isPrintingKey = FALSE;
        if (kcm->IsPrintingKey(keyCode, &isPrintingKey), isPrintingKey) {
            mConsumeSearchKeyUp = TRUE;
            mSearchKeyShortcutPending = FALSE;
            if (down && repeatCount == 0 && !keyguardOn) {
                AutoPtr<IIntent> shortcutIntent = mShortcutManager->GetIntent(kcm, keyCode, metaState);
                if (shortcutIntent != NULL) {
                    shortcutIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                    // try {
                    mContext->StartActivityAsUser(shortcutIntent, UserHandle::CURRENT);
                    // } catch (ActivityNotFoundException ex) {
                    //     Slog.w(TAG, "Dropping shortcut key combination because "
                    //             + "the activity to which it is registered was not found: "
                    //             + "SEARCH+" + IKeyEvent::keyCodeToString(keyCode), ex);
                    // }
                    // } else {
                    // Slogger::I(TAG, "Dropping unregistered shortcut key combination: "
                    //         + "SEARCH+" + KeyEvent.keyCodeToString(keyCode));
                }
            }
            return -1;
        }
    }

    // Invoke shortcuts using Meta.
    if (down && repeatCount == 0 && !keyguardOn
            && (metaState & IKeyEvent::META_META_ON) != 0) {
        const AutoPtr<IKeyCharacterMap> kcm;
        event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
        Boolean isPrintingKey = FALSE;
        if (kcm->IsPrintingKey(keyCode, &isPrintingKey), isPrintingKey) {
            AutoPtr<IIntent> shortcutIntent = mShortcutManager->GetIntent(kcm, keyCode,
                    metaState & ~(IKeyEvent::META_META_ON
                    | IKeyEvent::META_META_LEFT_ON | IKeyEvent::META_META_RIGHT_ON));
            if (shortcutIntent != NULL) {
                shortcutIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                // try {
                mContext->StartActivityAsUser(shortcutIntent, UserHandle::CURRENT);
                // } catch (ActivityNotFoundException ex) {
                //     Slog.w(TAG, "Dropping shortcut key combination because "
                //             + "the activity to which it is registered was not found: "
                //             + "META+" + KeyEvent.keyCodeToString(keyCode), ex);
                // }
                return -1;
            }
        }
    }

    // // Handle application launch keys.
    if (down && repeatCount == 0 && !keyguardOn) {
        AutoPtr<IInterface> obj;
        HashMap<Int32, AutoPtr<IInterface> >::Iterator it = sApplicationLaunchKeyCategories->Find(keyCode);
        if (it != sApplicationLaunchKeyCategories->End()) {
            obj = it->mSecond;
        }
        if (obj != NULL) {
            AutoPtr<ICharSequence> pValue = ICharSequence::Probe(obj);
            String category;
            pValue->ToString(&category);

            AutoPtr<IIntent> intent = CIntent::MakeMainSelectorActivity(IIntent::ACTION_MAIN, category);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            // try {
            mContext->StartActivityAsUser(intent, UserHandle::CURRENT);
            // } catch (ActivityNotFoundException ex) {
            //     Slog.w(TAG, "Dropping application launch key because "
            //             + "the activity to which it is registered was not found: "
            //             + "keyCode=" + keyCode + ", category=" + category, ex);
            // }
            return -1;
        }
    }

    // Display task switcher for ALT-TAB.
    if (down && repeatCount == 0 && keyCode == IKeyEvent::KEYCODE_TAB) {
        if (mRecentAppsHeldModifiers == 0 && !keyguardOn) {
            Int32 modifiers;
            event->GetModifiers(&modifiers);
            Int32 shiftlessModifiers = modifiers & ~IKeyEvent::META_SHIFT_MASK;
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            Boolean bval1;
            if ((helper->MetaStateHasModifiers(shiftlessModifiers, IKeyEvent::META_ALT_ON, &bval1), bval1)) {
                mRecentAppsHeldModifiers = shiftlessModifiers;
                ShowRecentApps(TRUE);
                return -1;
            }
        }
    }
    else if (!down && mRecentAppsHeldModifiers != 0
        && (metaState & mRecentAppsHeldModifiers) == 0) {
        mRecentAppsHeldModifiers = 0;
        HideRecentApps(TRUE, FALSE);
    }

    // // Handle keyboard language switching.
    if (down && repeatCount == 0
            && (keyCode == IKeyEvent::KEYCODE_LANGUAGE_SWITCH
                    || (keyCode == IKeyEvent::KEYCODE_SPACE
                            && (metaState & IKeyEvent::META_CTRL_MASK) != 0))) {
        Int32 direction = (metaState & IKeyEvent::META_SHIFT_MASK) != 0 ? -1 : 1;
        Int32 deviceId = 0;
        mWindowManagerFuncs->SwitchKeyboardLayout((IInputEvent::Probe(event)->GetDeviceId(&deviceId), deviceId), direction);
        return -1;
    }
    if (mLanguageSwitchKeyPressed && !down
            && (keyCode == IKeyEvent::KEYCODE_LANGUAGE_SWITCH
                    || keyCode == IKeyEvent::KEYCODE_SPACE)) {
        mLanguageSwitchKeyPressed = FALSE;
        return -1;
    }

    if (mGlobalKeyManager->HandleGlobalKey(mContext, keyCode, event, &bTemp), bTemp) {
        return -1;
    }

    // Reserve all the META modifier combos for system behavior
    if ((metaState & IKeyEvent::META_META_ON) != 0) {
        return -1;
    }

    // Let the application handle the key.
    return 0;
}

ECode PhoneWindowManager::DispatchUnhandledKey(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [out] */ IKeyEvent** keyEvent)
{
    VALIDATE_NOT_NULL(keyEvent);
    // Note: This method is only called if the initial down was unhandled.
    // if (DEBUG_INPUT) {
    //     Slog.d(TAG, "Unhandled key: win=" + win + ", action=" + event.getAction()
    //             + ", flags=" + event.getFlags()
    //             + ", keyCode=" + event.getKeyCode()
    //             + ", scanCode=" + event.getScanCode()
    //             + ", metaState=" + event.getMetaState()
    //             + ", repeatCount=" + event.getRepeatCount()
    //             + ", policyFlags=" + policyFlags);
    // }

    AutoPtr<IKeyEvent> fallbackEvent;
    Int32 flags = 0;
    if (((event->GetFlags(&flags), flags) & IKeyEvent::FLAG_FALLBACK) == 0) {
        AutoPtr<IKeyCharacterMap> kcm;
        event->GetKeyCharacterMap((IKeyCharacterMap**)&kcm);
        Int32 keyCode = 0;
        event->GetKeyCode(&keyCode);
        Int32 metaState = 0;
        event->GetMetaState(&metaState);

        Int32 action = 0;
        event->GetAction(&action);
        Int32 repeatCount = 0;
        event->GetRepeatCount(&repeatCount);
        Boolean initialDown = action == IKeyEvent::ACTION_DOWN
                && repeatCount == 0;

        // Check for fallback actions specified by the key character map.
        AutoPtr<IFallbackAction> fallbackAction;
        if (initialDown) {
            kcm->GetFallbackAction(keyCode, metaState, (IFallbackAction**)&fallbackAction);
        } else {
            HashMap<Int32, AutoPtr<IInterface> >::Iterator iter;
            iter = mFallbackActions->Find(keyCode);
            if (iter != mFallbackActions->End()) {
                AutoPtr<IInterface> obj = iter->mSecond;
                fallbackAction = IFallbackAction::Probe(obj);
            }
        }

        if (fallbackAction != NULL) {
            // if (DEBUG_INPUT) {
            //     Slog.d(TAG, "Fallback: keyCode=" + fallbackAction.keyCode
            //             + " metaState=" + Integer.toHexString(fallbackAction.metaState));
            // }
            Int32 flagsValue = 0;
            Int32 flags = (event->GetFlags(&flagsValue), flagsValue) | IKeyEvent::FLAG_FALLBACK;
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            Int64 downTime, eventTime;
            Int32 deviceId, scanCode, source;
            helper->Obtain(
                    (event->GetDownTime(&downTime), downTime), (IInputEvent::Probe(event)->GetEventTime(&eventTime), eventTime),
                    action, ((CKeyCharacterMap::FallbackAction*)fallbackAction.Get())->mKeyCode,
                    repeatCount, ((CKeyCharacterMap::FallbackAction*)fallbackAction.Get())->mMetaState,
                    (IInputEvent::Probe(event)->GetDeviceId(&deviceId), deviceId), (event->GetScanCode(&scanCode), scanCode),
                    flags, (IInputEvent::Probe(event)->GetSource(&source), source), String(NULL), (IKeyEvent**)&fallbackEvent);

            if (!InterceptFallback(win, fallbackEvent, policyFlags)) {
                IInputEvent::Probe(fallbackEvent)->Recycle();
                fallbackEvent = NULL;
            }

            if (initialDown) {
                (*mFallbackActions)[keyCode] = fallbackAction;
            } else if (action == IKeyEvent::ACTION_UP) {
                HashMap<Int32, AutoPtr<IInterface> >::Iterator itr = mFallbackActions->Find(keyCode);
                if (itr != mFallbackActions->End()) {
                    mFallbackActions->Erase(itr);
                }
                fallbackAction->Recycle();
            }
        }
    }

    // if (DEBUG_INPUT) {
    //     if (fallbackEvent == null) {
    //         Slog.d(TAG, "No fallback.");
    //     } else {
    //         Slog.d(TAG, "Performing fallback: " + fallbackEvent);
    //     }
    // }
    *keyEvent =  fallbackEvent;
    REFCOUNT_ADD(*keyEvent);

    return NOERROR;
}

Boolean PhoneWindowManager::InterceptFallback(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* fallbackEvent,
    /* [in] */ Int32 policyFlags)
{
    Int32 actions = 0;
    InterceptKeyBeforeQueueing(fallbackEvent, policyFlags, &actions);
    if ((actions & IWindowManagerPolicy::ACTION_PASS_TO_USER) != 0) {
        Int64 delayMillis = 0;
        InterceptKeyBeforeDispatching(
                win, fallbackEvent, policyFlags, &delayMillis);
        if (delayMillis == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void PhoneWindowManager::LaunchAssistLongPressAction()
{
    Boolean value = FALSE;
    PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &value);
    SendCloseSystemWindows(SYSTEM_DIALOG_REASON_ASSIST);

    // launch the search activity
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SEARCH_LONG_PRESS, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    // try {
        // TODO: This only stops the factory-installed search manager.
        // Need to formalize an API to handle others
    AutoPtr<ISearchManager> searchManager = GetSearchManager();
    if (searchManager != NULL) {
        searchManager->StopSearch();
    }
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
    mContext->StartActivityAsUser(intent, userHandle);
    // } catch (ActivityNotFoundException e) {
    //     Slog.w(TAG, "No activity to handle assist long press action.", e);
    // }
}

void PhoneWindowManager::LaunchAssistAction()
{
    LaunchAssistAction(String(NULL));
}

void PhoneWindowManager::LaunchAssistAction(
    /* [in] */ const String& hint)
{
    SendCloseSystemWindows(SYSTEM_DIALOG_REASON_ASSIST);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&service);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(service);
    AutoPtr<IIntent> intent;
    searchManager->GetAssistIntent(mContext, TRUE, IUserHandle::USER_CURRENT, (IIntent**)&intent);

    if (intent != NULL) {
        if (!hint.IsNull())
        {
            intent->PutExtra(hint, TRUE);
        }
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_SINGLE_TOP
                | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
        // try {
        mContext->StartActivityAsUser(intent, UserHandle::CURRENT);
        // } catch (ActivityNotFoundException e) {
        //     Slog.w(TAG, "No activity to handle assist action.", e);
        // }
    }
}

AutoPtr<ISearchManager> PhoneWindowManager::GetSearchManager()
{
    if (mSearchManager == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
        mSearchManager = ISearchManager::Probe(obj);
    }
    return mSearchManager;
}

void PhoneWindowManager::PreloadRecentApps()
{
    mPreloadedRecentApps = TRUE;
    //try {
    AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
    ECode ec = NOERROR;
    if (statusbar != NULL) {
        ec = statusbar->PreloadRecentApps();
    }
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "RemoteException when preloading recent apps", e);
    //    // re-acquire status bar service next time it is needed.
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        mStatusBarService = NULL;
    }
    //}
}

void PhoneWindowManager::CancelPreloadRecentApps()
{
    if (mPreloadedRecentApps) {
        mPreloadedRecentApps = FALSE;
        //try {
        AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
        ECode ec = NOERROR;
        if (statusbar != NULL) {
            ec = statusbar->CancelPreloadRecentApps();
        }
        //} catch (RemoteException e) {
        //    Slog.e(TAG, "RemoteException when cancelling recent apps preload", e);
        // re-acquire status bar service next time it is needed.
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            mStatusBarService = NULL;
        }
        //}
    }
}

void PhoneWindowManager::ToggleRecentApps()
{
    mPreloadedRecentApps = FALSE; // preloading no longer needs to be canceled
    //try {
    AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
    ECode ec = NOERROR;
    if (statusbar != NULL) {
        ec = statusbar->ToggleRecentApps();
    }
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "RemoteException when toggling recent apps", e);
    //    // re-acquire status bar service next time it is needed.
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        mStatusBarService = NULL;
    }
    //}
}

void PhoneWindowManager::ShowRecentApps(
    /* [in] */ Boolean triggeredFromAltTab)
{
    mPreloadedRecentApps = FALSE; // preloading no longer needs to be canceled
    //try {
    AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
    ECode ec = NOERROR;
    if (statusbar != NULL) {
        ec = statusbar->ShowRecentApps(triggeredFromAltTab);
    }
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "RemoteException when showing recent apps", e);
    //    // re-acquire status bar service next time it is needed.
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        mStatusBarService = NULL;
    }
    //}
}

void PhoneWindowManager::HideRecentApps(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHome)
{
    mPreloadedRecentApps = FALSE; // preloading no longer needs to be canceled
    //try {
    AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
    ECode ec = NOERROR;
    if (statusbar != NULL) {
        ec = statusbar->HideRecentApps(triggeredFromAltTab, triggeredFromHome);
    }
    //} catch (RemoteException e) {
    //    Slog.e(TAG, "RemoteException when closing recent apps", e);
    //    // re-acquire status bar service next time it is needed.
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        mStatusBarService = NULL;
    }
    //}
}

void PhoneWindowManager::LaunchHomeFromHotKey()
{
    Boolean bTemp;
    if (mKeyguardDelegate != NULL && (mKeyguardDelegate->IsShowingAndNotOccluded(&bTemp), bTemp)) {
        // don't launch home if keyguard showing
    } else if (!mHideLockScreen && (mKeyguardDelegate->IsInputRestricted(&bTemp), bTemp)) {
        // when in keyguard restricted mode, must first verify unlock
        // before launching home
        mKeyguardDelegate->VerifyUnlock(new KeyguardDelegateOnKeyguardExitResult(this));
    }
    else
    {
        // no keyguard stuff to worry about, just launch home!
        // try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->StopAppSwitches();
        // } catch (RemoteException e) {
        // }
        if (mRecentsVisible) {
            // Hide Recents and notify it to launch Home
            AwakenDreams();
            SendCloseSystemWindows(SYSTEM_DIALOG_REASON_HOME_KEY);
            HideRecentApps(FALSE, TRUE);
        } else {
            // Otherwise, just launch Home
            SendCloseSystemWindows(SYSTEM_DIALOG_REASON_HOME_KEY);
            StartDockOrHome();
        }
    }
}

ECode PhoneWindowManager::AdjustSystemUiVisibilityLw(
    /* [in] */ Int32 visibility,
    /* [out] */ Int32* newVisibility)
{
    VALIDATE_NOT_NULL(newVisibility);
    assert(newVisibility != NULL);
    mStatusBarController->AdjustSystemUiVisibilityLw(mLastSystemUiFlags, visibility);
    mNavigationBarController->AdjustSystemUiVisibilityLw(mLastSystemUiFlags, visibility);
    mRecentsVisible = (visibility & IView::RECENT_APPS_VISIBLE) > 0;

    // Reset any bits in mForceClearingStatusBarVisibility that
    // are now clear.
    mResettingSystemUiFlags &= visibility;
    // Clear any bits in the new visibility that are currently being
    // force cleared, before reporting it.
    *newVisibility = visibility & ~mResettingSystemUiFlags
            & ~mForceClearedSystemUiFlags;

    return NOERROR;
}

ECode PhoneWindowManager::GetContentInsetHintLw(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ IRect* contentInset)
{
    assert(attrs != NULL);
    Int32 fl = WindowManagerPolicyControl::GetWindowFlags(NULL, attrs);

    Int32 sysuiVis = WindowManagerPolicyControl::GetSystemUiVisibility(NULL, attrs);
    Int32 subtreeSystemUiVisibility = 0;
    attrs->GetSubtreeSystemUiVisibility(&subtreeSystemUiVisibility);

    Int32 systemUiVisibility = sysuiVis | subtreeSystemUiVisibility;

    if ((fl & (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR))
            == (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR)) {
        Int32 availRight = 0, availBottom = 0;
        if (CanHideNavigationBar() &&
                (systemUiVisibility & IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) != 0) {
            availRight = mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
            availBottom = mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
        } else {
            availRight = mRestrictedScreenLeft + mRestrictedScreenWidth;
            availBottom = mRestrictedScreenTop + mRestrictedScreenHeight;
        }
        if ((systemUiVisibility & IView::SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0) {
            if ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0) {
                contentInset->Set(mStableFullscreenLeft, mStableFullscreenTop,
                        availRight - mStableFullscreenRight,
                        availBottom - mStableFullscreenBottom);
            } else {
                contentInset->Set(mStableLeft, mStableTop,
                        availRight - mStableRight, availBottom - mStableBottom);
            }
        } else if ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0 || (fl & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN) !=0) {
            contentInset->SetEmpty();
        } else if ((systemUiVisibility & (IView::SYSTEM_UI_FLAG_FULLSCREEN
                    | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN)) == 0) {
            contentInset->Set(mCurLeft, mCurTop,
                    availRight - mCurRight, availBottom - mCurBottom);
        } else {
            contentInset->Set(mCurLeft, mCurTop,
                    availRight - mCurRight, availBottom - mCurBottom);
        }
        return NOERROR;
    }

    contentInset->SetEmpty();

    return NOERROR;
}

ECode PhoneWindowManager::BeginLayoutLw(
    /* [in] */ Boolean isDefaultDisplay,
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 displayHeight,
    /* [in] */ Int32 displayRotation)
{
    Int32 overscanLeft, overscanTop, overscanRight, overscanBottom;
    if (isDefaultDisplay) {
        switch (displayRotation) {
            case ISurface::ROTATION_90:
                overscanLeft = mOverscanTop;
                overscanTop = mOverscanRight;
                overscanRight = mOverscanBottom;
                overscanBottom = mOverscanLeft;
                break;
            case ISurface::ROTATION_180:
                overscanLeft = mOverscanRight;
                overscanTop = mOverscanBottom;
                overscanRight = mOverscanLeft;
                overscanBottom = mOverscanTop;
                break;
            case ISurface::ROTATION_270:
                overscanLeft = mOverscanBottom;
                overscanTop = mOverscanLeft;
                overscanRight = mOverscanTop;
                overscanBottom = mOverscanRight;
                break;
            default:
                overscanLeft = mOverscanLeft;
                overscanTop = mOverscanTop;
                overscanRight = mOverscanRight;
                overscanBottom = mOverscanBottom;
                break;
        }
    } else {
        overscanLeft = 0;
        overscanTop = 0;
        overscanRight = 0;
        overscanBottom = 0;
    }
    mOverscanScreenLeft = mRestrictedOverscanScreenLeft = 0;
    mOverscanScreenTop = mRestrictedOverscanScreenTop = 0;
    mOverscanScreenWidth = mRestrictedOverscanScreenWidth = displayWidth;
    mOverscanScreenHeight = mRestrictedOverscanScreenHeight = displayHeight;
    mSystemLeft = 0;
    mSystemTop = 0;
    mSystemRight = displayWidth;
    mSystemBottom = displayHeight;
    mUnrestrictedScreenLeft = overscanLeft;
    mUnrestrictedScreenTop = overscanTop;
    mUnrestrictedScreenWidth = displayWidth - overscanLeft - overscanRight;
    mUnrestrictedScreenHeight = displayHeight - overscanTop - overscanBottom;
    mRestrictedScreenLeft = mUnrestrictedScreenLeft;
    mRestrictedScreenTop = mUnrestrictedScreenTop;
    SystemGesturesPointerEventListener* sgpel = (SystemGesturesPointerEventListener*)(mSystemGestures.Get());
    mRestrictedScreenWidth = sgpel->screenWidth = mUnrestrictedScreenWidth;
    mRestrictedScreenHeight = sgpel->screenHeight = mUnrestrictedScreenHeight;
    mDockLeft = mContentLeft = mVoiceContentLeft = mStableLeft = mStableFullscreenLeft
        = mCurLeft = mUnrestrictedScreenLeft;
    mDockTop = mContentTop = mVoiceContentTop = mStableTop = mStableFullscreenTop
        = mCurTop = mUnrestrictedScreenTop;
    mDockRight = mContentRight = mVoiceContentRight = mStableRight = mStableFullscreenRight
        = mCurRight = displayWidth - overscanRight;
    mDockBottom = mContentBottom = mVoiceContentBottom = mStableBottom = mStableFullscreenBottom
        = mCurBottom = displayHeight - overscanBottom;
    mDockLayer = 0x10000000;
    mStatusBarLayer = -1;

    // start with the current dock rect, which will be (0,0,displayWidth,displayHeight)
    AutoPtr<CRect> pf = mTmpParentFrame;
    AutoPtr<CRect> df = mTmpDisplayFrame;
    AutoPtr<CRect>of = mTmpOverscanFrame;
    AutoPtr<CRect>vf = mTmpVisibleFrame;
    AutoPtr<CRect>dcf = mTmpDecorFrame;
    pf->mLeft = df->mLeft = of->mLeft = vf->mLeft = mDockLeft;
    pf->mTop = df->mTop = of->mTop = vf->mTop = mDockTop;
    pf->mRight = df->mRight = of->mRight = vf->mRight = mDockRight;
    pf->mBottom = df->mBottom = of->mBottom = vf->mBottom = mDockBottom;
    dcf->SetEmpty();  // Decor frame N/A for system bars.

    if (isDefaultDisplay) {
        // For purposes of putting out fake window up to steal focus, we will
        // drive nav being hidden only by whether it is requested.
        const Int32 sysui = mLastSystemUiFlags;
        Boolean navVisible = (sysui & IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0;
        Boolean navTranslucent = (sysui & (IView::NAVIGATION_BAR_TRANSLUCENT | IView::SYSTEM_UI_TRANSPARENT)) != 0;
        Boolean immersive = (sysui & IView::SYSTEM_UI_FLAG_IMMERSIVE) != 0;
        Boolean immersiveSticky = (sysui & IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY) != 0;
        Boolean navAllowedHidden = immersive || immersiveSticky;
        navTranslucent &= !immersiveSticky;  // transient trumps translucent
        Boolean isKeyguardShowing = IsStatusBarKeyguard() && !mHideLockScreen;
        if (!isKeyguardShowing) {
            navTranslucent &= AreTranslucentBarsAllowed();
        }

        // When the navigation bar isn't visible, we put up a fake
        // input window to catch all touch events.  This way we can
        // detect when the user presses anywhere to bring back the nav
        // bar and ensure the application doesn't see the event.
        if (navVisible || navAllowedHidden) {
            if (mHideNavFakeWindow != NULL) {
                mHideNavFakeWindow->Dismiss();
                mHideNavFakeWindow = NULL;
            }
        } else if (mHideNavFakeWindow == NULL) {
            AutoPtr<ILooper> looper;
            mHandler->GetLooper((ILooper**)&looper);

            mWindowManagerFuncs->AddFakeWindow(
                   looper, mHideNavInputEventReceiverFactory,
                   String("hidden nav"), IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER, 0,
                   0, FALSE, FALSE, TRUE, (IFakeWindow**)&mHideNavFakeWindow);
        }

        // For purposes of positioning and showing the nav bar, if we have
        // decided that it can't be hidden (because of the screen aspect ratio),
        // then take that into account.
        navVisible |= !CanHideNavigationBar();

        Boolean updateSysUiVisibility = false;
        if (mNavigationBar != NULL) {
            Boolean transientNavBarShowing;
            mNavigationBarController->IsTransientShowing(&transientNavBarShowing);
            // Force the navigation bar to its appropriate place and
            // size.  We need to do this directly, instead of relying on
            // it to bubble up from the nav bar, because this needs to
            // change atomically with screen rotations.
            mNavigationBarOnBottom = (!mNavigationBarCanMove || displayWidth < displayHeight);
            if (mNavigationBarOnBottom) {
                // It's a system nav bar or a portrait screen; nav bar goes on bottom.
                int top = displayHeight - overscanBottom
                    - mNavigationBarHeightForRotation[displayRotation];
                mTmpNavigationFrame->Set(0, top, displayWidth, displayHeight - overscanBottom);
                mStableBottom = mStableFullscreenBottom = mTmpNavigationFrame->mTop;
                Boolean bTemp = FALSE;
                if (transientNavBarShowing) {
                    mNavigationBarController->SetBarShowingLw(TRUE, &bTemp);
                } else if (navVisible) {
                    mNavigationBarController->SetBarShowingLw(TRUE, &bTemp);
                    mDockBottom = mTmpNavigationFrame->mTop;
                    mRestrictedScreenHeight = mDockBottom - mRestrictedScreenTop;
                    mRestrictedOverscanScreenHeight = mDockBottom - mRestrictedOverscanScreenTop;
                } else {
                    // We currently want to hide the navigation UI.
                    mNavigationBarController->SetBarShowingLw(FALSE, &bTemp);
                }
                bTemp = FALSE;
                if (navVisible && !navTranslucent && !navAllowedHidden
                        && !(mNavigationBar->IsAnimatingLw(&bTemp), bTemp)
                        && !(mNavigationBarController->WasRecentlyTranslucent(&bTemp), bTemp)) {
                    // If the opaque nav bar is currently requested to be visible,
                    // and not in the process of animating on or off, then
                    // we can tell the app that it is covered by it.
                    mSystemBottom = mTmpNavigationFrame->mTop;
                }
            }
            else {
                // Landscape screen; nav bar goes to the right.
                int left = displayWidth - overscanRight
                    - mNavigationBarWidthForRotation[displayRotation];
                mTmpNavigationFrame->Set(left, 0, displayWidth - overscanRight, displayHeight);
                mStableRight = mStableFullscreenRight = mTmpNavigationFrame->mLeft;
                Boolean bTemp;
                if (transientNavBarShowing) {
                    mNavigationBarController->SetBarShowingLw(TRUE, &bTemp);
                } else if (navVisible) {
                    mNavigationBarController->SetBarShowingLw(TRUE, &bTemp);
                    mDockRight = mTmpNavigationFrame->mLeft;
                    mRestrictedScreenWidth = mDockRight - mRestrictedScreenLeft;
                    mRestrictedOverscanScreenWidth = mDockRight - mRestrictedOverscanScreenLeft;
                } else {
                    // We currently want to hide the navigation UI.
                    mNavigationBarController->SetBarShowingLw(FALSE, &bTemp);
                }
                bTemp = FALSE;
                if (navVisible && !navTranslucent && !(mNavigationBar->IsAnimatingLw(&bTemp), bTemp)
                        && !(mNavigationBarController->WasRecentlyTranslucent(&bTemp), bTemp)) {
                    // If the nav bar is currently requested to be visible,
                    // and not in the process of animating on or off, then
                    // we can tell the app that it is covered by it.
                    mSystemRight = mTmpNavigationFrame->mLeft;
                }
            }
            // Make sure the content and current rectangles are updated to
            // account for the restrictions from the navigation bar.
            mContentTop = mVoiceContentTop = mCurTop = mDockTop;
            mContentBottom = mVoiceContentBottom = mCurBottom = mDockBottom;
            mContentLeft = mVoiceContentLeft = mCurLeft = mDockLeft;
            mContentRight = mVoiceContentRight = mCurRight = mDockRight;
            mNavigationBar->GetSurfaceLayer(&mStatusBarLayer);
            // And compute the final frame.
            mNavigationBar->ComputeFrameLw(mTmpNavigationFrame, mTmpNavigationFrame,
                    mTmpNavigationFrame, mTmpNavigationFrame, mTmpNavigationFrame, dcf,
                    mTmpNavigationFrame);
            if (DEBUG_LAYOUT) Slogger::I(TAG, "mNavigationBar frame: xxxxx");// + mTmpNavigationFrame);
            Boolean bTemp;
            if (mNavigationBarController->CheckHiddenLw(&bTemp), bTemp) {
                updateSysUiVisibility = TRUE;
            }
        }
        if (DEBUG_LAYOUT) {
            Slogger::I(TAG, "mDock rect: (%d,%d - %d,%d)", mDockLeft, mDockTop, mDockRight, mDockBottom);
        }

        // decide where the status bar goes ahead of time
        if (mStatusBar != NULL) {
            // apply any navigation bar insets
            pf->mLeft = df->mLeft = of->mLeft = mUnrestrictedScreenLeft;
            pf->mTop = df->mTop = of->mTop = mUnrestrictedScreenTop;
            pf->mRight = df->mRight = of->mRight = mUnrestrictedScreenWidth + mUnrestrictedScreenLeft;
            pf->mBottom = df->mBottom = of->mBottom = mUnrestrictedScreenHeight + mUnrestrictedScreenTop;
            vf->mLeft = mStableLeft;
            vf->mTop = mStableTop;
            vf->mRight = mStableRight;
            vf->mBottom = mStableBottom;

            mStatusBar->GetSurfaceLayer(&mStatusBarLayer);

            // Let the status bar determine its size.
            mStatusBar->ComputeFrameLw(pf, df, vf, vf, vf, dcf, vf);

            // For layout, the status bar is always at the top with our fixed height.
            mStableTop = mUnrestrictedScreenTop + mStatusBarHeight;

            Boolean statusBarTransient = (sysui & IView::STATUS_BAR_TRANSIENT) != 0;
            Boolean statusBarTranslucent = (sysui & (IView::STATUS_BAR_TRANSLUCENT | IView::SYSTEM_UI_TRANSPARENT)) != 0;
            if (!isKeyguardShowing) {
                statusBarTranslucent &= AreTranslucentBarsAllowed();
            }

            // If the status bar is hidden, we don't want to cause
            // windows behind it to scroll.
            Boolean isVisibleLw;
            if ((mStatusBar->IsVisibleLw(&isVisibleLw), isVisibleLw) && !statusBarTransient) {
                // Status bar may go away, so the screen area it occupies
                // is available to apps but just covering them when the
                // status bar is visible.
                mDockTop = mUnrestrictedScreenTop + mStatusBarHeight;

                mContentTop = mVoiceContentTop = mCurTop = mDockTop;
                mContentBottom = mVoiceContentBottom = mCurBottom = mDockBottom;
                mContentLeft = mVoiceContentLeft = mCurLeft = mDockLeft;
                mContentRight = mVoiceContentRight = mCurRight = mDockRight;

                //if (DEBUG_LAYOUT) Slogger::V(TAG, "Status bar: " +
                //        String.format(
                //            "dock=[%d,%d][%d,%d] content=[%d,%d][%d,%d] cur=[%d,%d][%d,%d]",
                //            mDockLeft, mDockTop, mDockRight, mDockBottom,
                //            mContentLeft, mContentTop, mContentRight, mContentBottom,
                //            mCurLeft, mCurTop, mCurRight, mCurBottom));
            }
            isVisibleLw = FALSE;
            Boolean isAnimatingLw;
            Boolean wasRecentlyTranslucent;
            if ((mStatusBar->IsVisibleLw(&isVisibleLw), isVisibleLw)
                    && !(mStatusBar->IsAnimatingLw(&isAnimatingLw), isAnimatingLw)
                    && !statusBarTransient && !statusBarTranslucent
                    && !(mStatusBarController->WasRecentlyTranslucent(&wasRecentlyTranslucent), wasRecentlyTranslucent)) {
                // If the opaque status bar is currently requested to be visible,
                // and not in the process of animating on or off, then
                // we can tell the app that it is covered by it.
                mSystemTop = mUnrestrictedScreenTop + mStatusBarHeight;
            }
            Boolean checkHiddenLw;
            if (mStatusBarController->CheckHiddenLw(&checkHiddenLw), checkHiddenLw) {
                updateSysUiVisibility = TRUE;
            }
        }
        if (updateSysUiVisibility) {
            UpdateSystemUiVisibilityLw();
        }
    }
    return NOERROR;
}

ECode PhoneWindowManager::GetSystemDecorLayerLw(
    /* [out] */ Int32* layer)
{
    VALIDATE_NOT_NULL(layer);
    assert(layer != NULL);
    *layer = 0;

    if (mStatusBar != NULL) return mStatusBar->GetSurfaceLayer(layer);
    if (mNavigationBar != NULL) return mNavigationBar->GetSurfaceLayer(layer);
    return NOERROR;
}

ECode PhoneWindowManager::GetContentRectLw(
    /* [in] */ IRect* r)
{
    r->Set(mContentLeft, mContentTop, mContentRight, mContentBottom);
    return NOERROR;
}

void PhoneWindowManager::SetAttachedWindowFrames(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 fl,
    /* [in] */ Int32 adjust,
    /* [in] */ IWindowState* attached,
    /* [in] */ Boolean insetDecors,
    /* [in] */ IRect* _pf,
    /* [in] */ IRect* _df,
    /* [in] */ IRect* _of,
    /* [in] */ IRect* _cf,
    /* [in] */ IRect* _vf)
{
    CRect* pf = (CRect*)_pf;
    CRect* df = (CRect*)_df;
    CRect* of = (CRect*)_of;
    CRect* cf = (CRect*)_cf;
    CRect* vf = (CRect*)_vf;
    Int32 winLayer, attackedLayer;
    win->GetSurfaceLayer(&winLayer);
    attached->GetSurfaceLayer(&attackedLayer);
    if (winLayer > mDockLayer && attackedLayer < mDockLayer) {
       // Here's a special case: if this attached window is a panel that is
       // above the dock window, and the window it is attached to is below
       // the dock window, then the frames we computed for the window it is
       // attached to can not be used because the dock is effectively part
       // of the underlying window and the attached window is floating on top
       // of the whole thing.  So, we ignore the attached window and explicitly
       // compute the frames that would be appropriate without the dock.
       df->mLeft = of->mLeft = cf->mLeft = vf->mLeft = mDockLeft;
       df->mTop = of->mTop = cf->mTop = vf->mTop = mDockTop;
       df->mRight = of->mRight = cf->mRight = vf->mRight = mDockRight;
       df->mBottom = of->mBottom = cf->mBottom = vf->mBottom = mDockBottom;
    }
    else {
        // The effective display frame of the attached window depends on
        // whether it is taking care of insetting its content.  If not,
        // we need to use the parent's content frame so that the entire
        // window is positioned within that content.  Otherwise we can use
        // the display frame and let the attached window take care of
        // positioning its content appropriately.
        if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE) {
            AutoPtr<IRect> tempRc;
            attached->GetOverscanFrameLw((IRect**)&tempRc);
            cf->Set(tempRc);
        }
        else {
            // If the window is resizing, then we want to base the content
            // frame on our attached content frame to resize...  however,
            // things can be tricky if the attached window is NOT in resize
            // mode, in which case its content frame will be larger.
            // Ungh.  So to deal with that, make sure the content frame
            // we end up using is not covering the IM dock.
            AutoPtr<IRect> tempRc;
            attached->GetContentFrameLw((IRect**)&tempRc);
            cf->Set(tempRc);
            Boolean bTemp;
            attached->IsVoiceInteraction(&bTemp);
            if (bTemp) {
                if (cf->mLeft < mVoiceContentLeft) cf->mLeft = mVoiceContentLeft;
                if (cf->mTop < mVoiceContentTop) cf->mTop = mVoiceContentTop;
                if (cf->mRight > mVoiceContentRight) cf->mRight = mVoiceContentRight;
                if (cf->mBottom > mVoiceContentBottom) cf->mBottom = mVoiceContentBottom;
            } else if (attackedLayer < mDockLayer) {
                if (cf->mLeft < mContentLeft)
                    cf->mLeft = mContentLeft;
                if (cf->mTop < mContentTop)
                    cf->mTop = mContentTop;
                if (cf->mRight > mContentRight)
                    cf->mRight = mContentRight;
                if (cf->mBottom > mContentBottom)
                    cf->mBottom = mContentBottom;
            }
        }
        AutoPtr<IRect> tempRc;
        attached->GetDisplayFrameLw((IRect**)&tempRc);
        df->Set(insetDecors ? tempRc : cf);
        tempRc = NULL;
        attached->GetOverscanFrameLw((IRect**)&tempRc);
        of->Set(insetDecors ? tempRc : cf);
        tempRc = NULL;
        attached->GetVisibleFrameLw((IRect**)&tempRc);
        vf->Set(tempRc);
    }
    // The LAYOUT_IN_SCREEN flag is used to determine whether the attached
    // window should be positioned relative to its parent or the entire
    // screen.
    AutoPtr<IRect> tempRc;
    attached->GetFrameLw((IRect**)&tempRc);
    pf->Set((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN) == 0 ? tempRc : df);
}

void PhoneWindowManager::ApplyStableConstraints(
    /* [in] */ Int32 sysui,
    /* [in] */ Int32 fl,
    /* [in] */ IRect* _r)
{
    assert(_r != NULL);
    CRect* r = (CRect*)_r;
    if ((sysui & IView::SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0) {
        // If app is requesting a stable layout, don't let the
        // content insets go below the stable values.
        if ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0) {
            if (r->mLeft < mStableFullscreenLeft) r->mLeft = mStableFullscreenLeft;
            if (r->mTop < mStableFullscreenTop) r->mTop = mStableFullscreenTop;
            if (r->mRight > mStableFullscreenRight) r->mRight = mStableFullscreenRight;
            if (r->mBottom > mStableFullscreenBottom) r->mBottom = mStableFullscreenBottom;
        } else {
            if (r->mLeft < mStableLeft) r->mLeft = mStableLeft;
            if (r->mTop < mStableTop) r->mTop = mStableTop;
            if (r->mRight > mStableRight) r->mRight = mStableRight;
            if (r->mBottom > mStableBottom) r->mBottom = mStableBottom;
        }
    }
}

ECode PhoneWindowManager::LayoutWindowLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowState* attached)
{
    // we've already done the status bar
    AutoPtr<IWindowManagerLayoutParams> attrs;
    win->GetAttrs((IWindowManagerLayoutParams**)&attrs);;
    Int32 privateFlags;
    attrs->GetPrivateFlags(&privateFlags);
    if ((win == mStatusBar && (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) == 0)
            || win == mNavigationBar) {
        return NOERROR;
    }

    Boolean isDefaultDisplay = FALSE;
    win->IsDefaultDisplay(&isDefaultDisplay);

    Boolean needsToOffsetInputMethodTarget = isDefaultDisplay &&
            (win == mLastInputMethodTargetWindow && mLastInputMethodWindow != NULL);
    if (needsToOffsetInputMethodTarget) {
        // if (DEBUG_LAYOUT) {
        //     Slogger::I(TAG, "Offset ime target window by the last ime window state");
        // }
        OffsetInputMethodWindowLw(mLastInputMethodWindow);
    }
    Int32 fl = WindowManagerPolicyControl::GetWindowFlags(win, attrs);
    Int32 sim = 0;
    attrs->GetSoftInputMode(&sim);

    Int32 sysUiFl = WindowManagerPolicyControl::GetSystemUiVisibility(win, NULL);

    AutoPtr<CRect> pf = mTmpParentFrame;
    AutoPtr<CRect> df = mTmpDisplayFrame;
    AutoPtr<CRect> of = mTmpOverscanFrame;
    AutoPtr<CRect> cf = mTmpContentFrame;
    AutoPtr<CRect> vf = mTmpVisibleFrame;
    AutoPtr<CRect> dcf = mTmpDecorFrame;
    AutoPtr<CRect> sf = mTmpStableFrame;
    dcf->SetEmpty();

    Boolean visible = FALSE;
    Boolean hasNavBar = (isDefaultDisplay && mHasNavigationBar
            && mNavigationBar != NULL && (mNavigationBar->IsVisibleLw(&visible), visible));

    const Int32 adjust = sim & IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST;

    if (isDefaultDisplay) {
        sf->Set(mStableLeft, mStableTop, mStableRight, mStableBottom);
    } else {
        sf->Set(mOverscanLeft, mOverscanTop, mOverscanRight, mOverscanBottom);
    }
    Int32 type = 0;
    attrs->GetType(&type);
    if (!isDefaultDisplay) {
        if (attached != NULL) {
            // If this window is attached to another, our display
            // frame is the same as the one we are attached to.
            SetAttachedWindowFrames(win, fl, adjust, attached, TRUE, pf, df, of, cf, vf);
        }
        else {
            // Give the window full screen.
            pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mOverscanScreenLeft;
            pf->mTop = df->mTop = of->mTop = cf->mTop = mOverscanScreenTop;
            pf->mRight = df->mRight = of->mRight = cf->mRight
                = mOverscanScreenLeft + mOverscanScreenWidth;
            pf->mBottom = df->mBottom = of->mBottom = cf->mBottom
                = mOverscanScreenTop + mOverscanScreenHeight;
        }
    }
    else if (type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD) {
        pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = vf->mLeft = mDockLeft;
        pf->mTop = df->mTop = of->mTop = cf->mTop = vf->mTop = mDockTop;
        pf->mRight = df->mRight = of->mRight = cf->mRight = vf->mRight = mDockRight;
        // IM dock windows layout below the nav bar...
        pf->mBottom = df->mBottom = of->mBottom = mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
        // ...with content insets above the nav bar
        cf->mBottom = vf->mBottom = mStableBottom;
        // IM dock windows always go to the mBottom of the screen.
        attrs->SetGravity(IGravity::BOTTOM);
        win->GetSurfaceLayer(&mDockLayer);
    }
    else if (win == mStatusBar && (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
        pf->mLeft = df->mLeft = of->mLeft = mUnrestrictedScreenLeft;
        pf->mTop = df->mTop = of->mTop = mUnrestrictedScreenTop;
        pf->mRight = df->mRight = of->mRight = mUnrestrictedScreenWidth + mUnrestrictedScreenLeft;
        pf->mBottom = df->mBottom = of->mBottom = mUnrestrictedScreenHeight + mUnrestrictedScreenTop;
        cf->mLeft = vf->mLeft = mStableLeft;
        cf->mTop = vf->mTop = mStableTop;
        cf->mRight = vf->mRight = mStableRight;
        vf->mBottom = mStableBottom;
        cf->mBottom = mContentBottom;
    }
    else {
        // Default policy decor for the default display
        dcf->mLeft = mSystemLeft;
        dcf->mTop = mSystemTop;
        dcf->mRight = mSystemRight;
        dcf->mBottom = mSystemBottom;
        Boolean inheritTranslucentDecor = (privateFlags
                & IWindowManagerLayoutParams::PRIVATE_FLAG_INHERIT_TRANSLUCENT_DECOR) != 0;
        Boolean isAppWindow =
            type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW &&
            type <= IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW;
        Boolean bTemp;
        Boolean topAtRest =
            win == mTopFullscreenOpaqueWindowState && !(win->IsAnimatingLw(&bTemp), bTemp);
        if (isAppWindow && !inheritTranslucentDecor && !topAtRest) {
            if ((sysUiFl & IView::SYSTEM_UI_FLAG_FULLSCREEN) == 0
                    && (fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) == 0
                    && (fl & IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS) == 0
                    && (fl & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) == 0) {
                // Ensure policy decor includes status bar
                dcf->mTop = mStableTop;
            }
            if ((fl & IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION) == 0
                    && (sysUiFl & IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0
                    && (fl & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) == 0) {
                // Ensure policy decor includes navigation bar
                dcf->mBottom = mStableBottom;
                dcf->mRight = mStableRight;
            }
        }
        if ((fl & (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN |
                   IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR))
                == (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR))
        {
            // if (DEBUG_LAYOUT)
            //     Slogger::V(TAG, "layoutWindowLw(" + attrs.getTitle()
            //             + "): IN_SCREEN, INSET_DECOR");
            // This is the case for a normal activity window: we want it
            // to cover all of the screen space, and it can take care of
            // moving its contents to account for screen decorations that
            // intrude into that space.
            if (attached != NULL) {
                // If this window is attached to another, our display
                // frame is the same as the one we are attached to.
                SetAttachedWindowFrames(win, fl, adjust, attached, TRUE, pf, df, of, cf, vf);
            } else {
                if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                        || type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL) {
                    // Status bar panels are the only windows who can go on top of
                    // the status bar.  They are protected by the STATUS_BAR_SERVICE
                    // permission, so they have the same privileges as the status
                    // bar itself.
                    //
                    // However, they should still dodge the navigation bar if it exists.

                    pf->mLeft = df->mLeft = of->mLeft = hasNavBar ? mDockLeft : mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = hasNavBar
                        ? mRestrictedScreenLeft + mRestrictedScreenWidth
                        : mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = df->mBottom = hasNavBar
                        ? mRestrictedScreenTop + mRestrictedScreenHeight
                        : mUnrestrictedScreenTop + mUnrestrictedScreenHeight;

                    // if (DEBUG_LAYOUT)
                    //     Slogger::V(TAG, String.format(
                    //                 "Laying out status bar window: (%d,%d - %d,%d)",
                    //                 pf.left, pf.top, pf.right, pf.bottom));
                } else if((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN) != 0
                          && type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                          && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
                    // Asking to layout into the overscan region, so give it that pure
                    // unrestricted area.
                    pf->mLeft = df->mLeft = of->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = of->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = of->mRight = mOverscanScreenLeft + mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = mOverscanScreenTop + mOverscanScreenHeight;
                } else if (CanHideNavigationBar()
                        && (sysUiFl & IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) != 0
                        && type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                        && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
                    // Asking for layout as if the nav bar is hidden, lets the
                    // application extend into the unrestricted screen area.  We
                    // only do this for application windows to ensure no window that
                    // can be above the nav bar can do this.
                    pf->mLeft = df->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = mOverscanScreenLeft + mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = mOverscanScreenTop + mOverscanScreenHeight;
                    // We need to tell the app about where the frame inside the overscan
                    // is, so it can inset its content by that amount -- it didn't ask
                    // to actually extend itself into the overscan region
                    of->mLeft = mUnrestrictedScreenLeft;
                    of->mTop = mUnrestrictedScreenTop;
                    of->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                    of->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
                } else {
                    pf->mLeft = df->mLeft = mRestrictedOverscanScreenLeft;
                    pf->mTop = df->mTop = mRestrictedOverscanScreenTop;
                    pf->mRight = df->mRight = mRestrictedOverscanScreenLeft + mRestrictedOverscanScreenWidth;
                    pf->mBottom = df->mBottom = mRestrictedOverscanScreenTop + mRestrictedOverscanScreenHeight;
                    // We need to tell the app about where the frame inside the overscan
                    // is, so it can inset its content by that amount -- it didn't ask
                    // to actually extend itself into the overscan region.
                    of->mLeft = mUnrestrictedScreenLeft;
                    of->mTop = mUnrestrictedScreenTop;
                    of->mRight = mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                    of->mBottom = mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
                }

                if ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) == 0) {
                    Boolean isVoiceInteraction;
                    if (win->IsVoiceInteraction(&isVoiceInteraction), isVoiceInteraction) {
                        cf->mLeft = mVoiceContentLeft;
                        cf->mTop = mVoiceContentTop;
                        cf->mRight = mVoiceContentRight;
                        cf->mBottom = mVoiceContentBottom;
                    } else {
                        if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE) {
                            cf->mLeft = mDockLeft;
                            cf->mTop = mDockTop;
                            cf->mRight = mDockRight;
                            cf->mBottom = mDockBottom;
                        } else {
                            cf->mLeft = mContentLeft;
                            cf->mTop = mContentTop;
                            cf->mRight = mContentRight;
                            cf->mBottom = mContentBottom;
                        }
                    }
                } else {
                    // Full screen windows are always given a layout that is as if the
                    // status bar and other transient decors are gone.  This is to avoid
                    // bad states when moving from a window that is not hding the
                    // status bar to one that is.
                    cf->mLeft = mRestrictedScreenLeft;
                    cf->mTop = mRestrictedScreenTop;
                    cf->mRight = mRestrictedScreenLeft + mRestrictedScreenWidth;
                    cf->mBottom = mRestrictedScreenTop + mRestrictedScreenHeight;
                }

                ApplyStableConstraints(sysUiFl, fl, cf);
                if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING) {
                    vf->mLeft = mCurLeft;
                    vf->mTop = mCurTop;
                    vf->mRight = mCurRight;
                    vf->mBottom = mCurBottom;
                } else {
                    vf->Set(cf);
                }
                }
            } else if ((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN) != 0 || (sysUiFl
                        & (IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION)) != 0) {
                // if (DEBUG_LAYOUT)
                //     Slogger::V(TAG, "layoutWindowLw(" + attrs.getTitle() + "): IN_SCREEN");
                // A window that has requested to fill the entire screen just
                // gets everything, period.
                if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                        || type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL) {
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = hasNavBar ? mDockLeft : mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = hasNavBar
                        ? mRestrictedScreenLeft + mRestrictedScreenWidth
                        : mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom = hasNavBar
                        ? mRestrictedScreenTop + mRestrictedScreenHeight
                        : mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
                    // if (DEBUG_LAYOUT) {
                    //     Slogger::V(TAG, String.format(
                    //                 "Laying out IN_SCREEN status bar window: (%d,%d - %d,%d)",
                    //                 pf.left, pf.top, pf.right, pf.bottom));
                    // }
                } else if (type == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR
                        || type == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL) {
                    // The navigation bar has Real Ultimate Power.
                    pf->mLeft = df->mLeft = of->mLeft = mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
                    // if (DEBUG_LAYOUT) {
                    //     Slogger::V(TAG, String.format(
                    //                 "Laying out navigation bar window: (%d,%d - %d,%d)",
                    //                 pf.left, pf.top, pf.right, pf.bottom));
                    // }
                } else if ((type == IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY
                            || type == IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS)
                        && ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0)) {
                    // Fullscreen secure system overlays get what they ask for.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mOverscanScreenLeft + mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom = mOverscanScreenTop + mOverscanScreenHeight;
                } else if (type == IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS
                        || type == IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND) {
                    // Boot progress screen always covers entire display.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mOverscanScreenLeft+mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom
                        = mOverscanScreenTop+mOverscanScreenHeight;
                } else if (type == IWindowManagerLayoutParams::TYPE_WALLPAPER) {
                    // The wallpaper also has Real Ultimate Power, but we want to tell
                    // it about the overscan area.
                    pf->mLeft = df->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = mOverscanScreenLeft + mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = mOverscanScreenTop + mOverscanScreenHeight;
                    of->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
                    of->mTop = cf->mTop = mUnrestrictedScreenTop;
                    of->mRight = cf->mRight = mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                    of->mBottom = cf->mBottom = mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
                } else if ((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_IN_OVERSCAN) != 0
                        && type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                        && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
                    // Asking to layout into the overscan region, so give it that pure
                    // unrestricted area.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mOverscanScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mOverscanScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight
                        = mOverscanScreenLeft + mOverscanScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom
                        = mOverscanScreenTop + mOverscanScreenHeight;
                } else if (CanHideNavigationBar()
                        && (sysUiFl & IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) != 0
                        && (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR
                        || type == IWindowManagerLayoutParams::TYPE_TOAST
                        || (type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                        && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW))) {
                    // Asking for layout as if the nav bar is hidden, lets the
                    // application extend into the unrestricted screen area.  We
                    // only do this for application windows (or toasts) to ensure no window that
                    // can be above the nav bar can do this.
                    // XXX This assumes that an app asking for this will also
                    // ask for layout in only content.  We can't currently figure out
                    // what the screen would be if only laying out to hide the nav bar.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom
                        = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
                } else {
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mRestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mRestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mRestrictedScreenLeft
                        + mRestrictedScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom = mRestrictedScreenTop
                        + mRestrictedScreenHeight;
                }

                ApplyStableConstraints(sysUiFl, fl, cf);

                if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING) {
                    vf->mLeft = mCurLeft;
                    vf->mTop = mCurTop;
                    vf->mRight = mCurRight;
                    vf->mBottom = mCurBottom;
                } else {
                    vf->Set(cf);
                }
            } else if (attached != NULL) {
                // if (DEBUG_LAYOUT)
                //     Slogger::V(TAG, "layoutWindowLw(" + attrs.getTitle() + "): attached to " + attached);
                // A child window should be placed inside of the same visible
                // frame that its parent had.
                SetAttachedWindowFrames(win, fl, adjust, attached, FALSE, pf, df, of, cf, vf);
            } else {
                // if (DEBUG_LAYOUT)
                //     Slogger::V(TAG, "layoutWindowLw(" + attrs.getTitle() + "): normal window");
                // Otherwise, a normal window must be placed inside the content
                // of all screen decorations.
                if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL) {
                    // Status bar panels are the only windows who can go on top of
                    // the status bar.  They are protected by the STATUS_BAR_SERVICE
                    // permission, so they have the same privileges as the status
                    // bar itself.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mRestrictedScreenLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mRestrictedScreenTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mRestrictedScreenLeft + mRestrictedScreenWidth;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom
                        = mRestrictedScreenTop + mRestrictedScreenHeight;
                } else if (type == IWindowManagerLayoutParams::TYPE_TOAST
                            || type == IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT
                            || type == IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY) {
                    // These dialogs are stable to interim decor changes.
                    pf->mLeft = df->mLeft = of->mLeft = cf->mLeft = mStableLeft;
                    pf->mTop = df->mTop = of->mTop = cf->mTop = mStableTop;
                    pf->mRight = df->mRight = of->mRight = cf->mRight = mStableRight;
                    pf->mBottom = df->mBottom = of->mBottom = cf->mBottom = mStableBottom;
                } else {
                    pf->mLeft = mContentLeft;
                    pf->mTop = mContentTop;
                    pf->mRight = mContentRight;
                    pf->mBottom = mContentBottom;
                    Boolean isVoiceInteraction;
                    if (win->IsVoiceInteraction(&isVoiceInteraction), isVoiceInteraction) {
                        df->mLeft = of->mLeft = cf->mLeft = mVoiceContentLeft;
                        df->mTop = of->mTop = cf->mTop = mVoiceContentTop;
                        df->mRight = of->mRight = cf->mRight = mVoiceContentRight;
                        df->mBottom = of->mBottom = cf->mBottom = mVoiceContentBottom;
                    } else if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE) {
                        df->mLeft = of->mLeft = cf->mLeft = mDockLeft;
                        df->mTop = of->mTop = cf->mTop = mDockTop;
                        df->mRight = of->mRight = cf->mRight = mDockRight;
                        df->mBottom = of->mBottom = cf->mBottom = mDockBottom;
                    } else {
                        df->mLeft = of->mLeft = cf->mLeft = mContentLeft;
                        df->mTop = of->mTop = cf->mTop = mContentTop;
                        df->mRight = of->mRight = cf->mRight = mContentRight;
                        df->mBottom = of->mBottom = cf->mBottom = mContentBottom;
                    }
                    if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING) {
                        vf->mLeft = mCurLeft;
                        vf->mTop = mCurTop;
                        vf->mRight = mCurRight;
                        vf->mBottom = mCurBottom;
                    } else {
                        vf->Set(cf);
                    }
                }
            }
    }

    // TYPE_SYSTEM_ERROR is above the NavigationBar so it can't be allowed to extend over it.
    if ((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS) != 0 && type != IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR) {
        df->mLeft = df->mTop = -10000;
        df->mRight = df->mBottom = 10000;
        if (type != IWindowManagerLayoutParams::TYPE_WALLPAPER) {
            of->mLeft = of->mTop = cf->mLeft = cf->mTop = vf->mLeft = vf->mTop = -10000;
            of->mRight = of->mBottom = cf->mRight = cf->mBottom = vf->mRight = vf->mBottom = 10000;
        }
    }

    // if (DEBUG_LAYOUT) Slogger::V(TAG, "Compute frame " + attrs.getTitle()
    //         + ": sim=#" + Integer.toHexString(sim)
    //         + " attach=" + attached + " type=" + attrs.type
    //         + String.format(" flags=0x%08x", fl)
    //         + " pf=" + pf.toShortString() + " df=" + df.toShortString()
    //         + " of=" + of.toShortString()
    //         + " cf=" + cf.toShortString() + " vf=" + vf.toShortString()
    //         + " dcf=" + dcf.toShortString()
    //         + " sf=" + sf.toShortString());

    win->ComputeFrameLw(pf, df, of, cf, vf, dcf, sf);

    // Dock windows carve out the bottom of the screen, so normal windows
    // can't appear underneath them.
    Boolean state = FALSE;
    win->IsVisibleOrBehindKeyguardLw(&visible);
    win->GetGivenInsetsPendingLw(&state);
    if (type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD && visible
            && !state) {
        SetLastInputMethodWindowLw(NULL, NULL);
        OffsetInputMethodWindowLw(win);
    }

    Boolean isVisibleOrBehindKeyguardLw;
    Boolean givenInsetsPendingLw;
    win->IsVisibleOrBehindKeyguardLw(&isVisibleOrBehindKeyguardLw);
    win->GetGivenInsetsPendingLw(&givenInsetsPendingLw);
    if (type == IWindowManagerLayoutParams::TYPE_VOICE_INTERACTION && isVisibleOrBehindKeyguardLw
            && !givenInsetsPendingLw) {
        OffsetVoiceInputWindowLw(win);
    }

    return NOERROR;
}

void PhoneWindowManager::OffsetInputMethodWindowLw(
    /* [in] */ IWindowState* win)
{
    assert(win != NULL);
    AutoPtr<IRect> frame, tmpRect;
    win->GetContentFrameLw((IRect**)&frame);
    Int32 top = 0, tmp = 0;
    frame->GetTop(&top);

    win->GetGivenContentInsetsLw((IRect**)&tmpRect);
    tmpRect->GetTop(&tmp);
    top += tmp;
    if (mContentBottom > top) {
        mContentBottom = top;
    }

    if (mVoiceContentBottom > top) {
        mVoiceContentBottom = top;
    }

    tmpRect = NULL;
    win->GetVisibleFrameLw((IRect**)&tmpRect);
    tmpRect->GetTop(&tmp);
    top = tmp;

    tmpRect = NULL;
    win->GetGivenVisibleInsetsLw((IRect**)&tmpRect);
    tmpRect->GetTop(&tmp);
    top += tmp;
    if (mCurBottom > top) {
        mCurBottom = top;
    }
    // if (DEBUG_LAYOUT) Slogger::V(TAG, "Input method: mDockBottom="
    //         + mDockBottom + " mContentBottom="
    //         + mContentBottom + " mCurBottom=" + mCurBottom);
}

void PhoneWindowManager::OffsetVoiceInputWindowLw(
    /* [in] */ IWindowState* win)
{
    Int32 gravity;
    AutoPtr<IWindowManagerLayoutParams> attrs;
    win->GetAttrs((IWindowManagerLayoutParams**)&attrs);;
    attrs->GetGravity(&gravity);
    switch (gravity & ((IGravity::AXIS_PULL_BEFORE|IGravity::AXIS_PULL_AFTER)
                << IGravity::AXIS_X_SHIFT)) {
        case IGravity::AXIS_PULL_BEFORE<<IGravity::AXIS_X_SHIFT: {
            AutoPtr<IRect> tempRc1, tempRc2;
            win->GetContentFrameLw((IRect**)&tempRc1);
            win->GetGivenContentInsetsLw((IRect**)&tempRc2);
            Int32 fRight, iRight;
            tempRc1->GetRight(&fRight);
            tempRc2->GetRight(&iRight);
            Int32 right = fRight - iRight;
            if (mVoiceContentLeft < right) {
                mVoiceContentLeft = right;
            }
        } break;
        case IGravity::AXIS_PULL_AFTER<<IGravity::AXIS_X_SHIFT: {
            AutoPtr<IRect> tempRc1, tempRc2;
            win->GetContentFrameLw((IRect**)&tempRc1);
            win->GetGivenContentInsetsLw((IRect**)&tempRc2);
            Int32 fleft, ileft;
            tempRc1->GetLeft(&fleft);
            tempRc2->GetLeft(&ileft);
            Int32 left = fleft - ileft;
            if (mVoiceContentRight < left) {
                mVoiceContentRight = left;
            }
        } break;
    }
    switch (gravity&((IGravity::AXIS_PULL_BEFORE|IGravity::AXIS_PULL_AFTER)
                << IGravity::AXIS_Y_SHIFT)) {
        case IGravity::AXIS_PULL_BEFORE<<IGravity::AXIS_Y_SHIFT: {
            AutoPtr<IRect> tempRc1, tempRc2;
            win->GetContentFrameLw((IRect**)&tempRc1);
            win->GetGivenContentInsetsLw((IRect**)&tempRc2);
            Int32 fBottom, iBottom;
            tempRc1->GetBottom(&fBottom);
            tempRc2->GetBottom(&iBottom);
            Int32 bottom = fBottom - iBottom;
            if (mVoiceContentTop < bottom) {
                mVoiceContentTop = bottom;
            }
        } break;
        case IGravity::AXIS_PULL_AFTER<<IGravity::AXIS_Y_SHIFT: {
            AutoPtr<IRect> tempRc1, tempRc2;
            win->GetContentFrameLw((IRect**)&tempRc1);
            win->GetGivenContentInsetsLw((IRect**)&tempRc2);
            Int32 fTop, iTop;
            tempRc1->GetTop(&fTop);
            tempRc2->GetTop(&iTop);
            int top = fTop - iTop;
            if (mVoiceContentBottom < top) {
                mVoiceContentBottom = top;
            }
       } break;
    }
}

ECode PhoneWindowManager::FinishLayoutLw()
{
    return NOERROR;
}

ECode PhoneWindowManager::BeginPostLayoutPolicyLw(
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 displayHeight)
{
    mTopFullscreenOpaqueWindowState = NULL;
    mAppsToBeHidden->Clear();
    mAppsThatDismissKeyguard->Clear();
    mForceStatusBar = FALSE;
    mForceStatusBarFromKeyguard = FALSE;
    mForcingShowNavBar = FALSE;
    mForcingShowNavBarLayer = -1;

    mHideLockScreen = FALSE;
    mAllowLockscreenWhenOn = FALSE;
    mDismissKeyguard = DISMISS_KEYGUARD_NONE;
    mShowingLockscreen = FALSE;
    mShowingDream = FALSE;
    mWinShowWhenLocked = NULL;
    return NOERROR;
}

ECode PhoneWindowManager::ApplyPostLayoutPolicyLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    if (DEBUG_LAYOUT) {
        Boolean bval;
        win->IsVisibleOrBehindKeyguardLw(&bval);
        Slogger::I(TAG, "Win %s: isVisibleOrBehindKeyguardLw=%d", TO_CSTR(win), bval);
    }
    Boolean tmp = FALSE;
    Int32 fl = WindowManagerPolicyControl::GetWindowFlags(win, attrs);
    Int32 type;
    attrs->GetType(&type);
    Boolean isVisibleLw;
    win->IsVisibleLw(&isVisibleLw);
    if (mTopFullscreenOpaqueWindowState == NULL
            && isVisibleLw && type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD)
    {
        mForcingShowNavBar = TRUE;
        win->GetSurfaceLayer(&mForcingShowNavBarLayer);
    }

    Int32 privateFlags;
    attrs->GetPrivateFlags(&privateFlags);
    if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR
        && (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
        mForceStatusBarFromKeyguard = true;
    }

    if (mTopFullscreenOpaqueWindowState == NULL &&
            (win->IsVisibleOrBehindKeyguardLw(&tmp), tmp)
            && !(win->IsGoneForLayoutLw(&tmp), tmp)) {

        if ((fl & IWindowManagerLayoutParams::FLAG_FORCE_NOT_FULLSCREEN) != 0) {
            if ((privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
                mForceStatusBarFromKeyguard = TRUE;
            } else {
                mForceStatusBar = TRUE;
            }
        }

        if (privateFlags == IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) {
            mShowingLockscreen = TRUE;
        }

        Boolean appWindow = type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
            && type < IWindowManagerLayoutParams::FIRST_SYSTEM_WINDOW;
        if (type == IWindowManagerLayoutParams::TYPE_DREAM) {
            // If the lockscreen was showing when the dream started then wait
            // for the dream to draw before hiding the lockscreen.
            if (!mDreamingLockscreen
                    || ((win->IsVisibleLw(&tmp), tmp) && (win->HasDrawnLw(&tmp), tmp))) {
                mShowingDream = TRUE;
                appWindow = TRUE;
            }
        }

        Boolean showWhenLocked = (fl & IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED) != 0;
        Boolean dismissKeyguard = (fl & IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD) != 0;
        Boolean secureKeyguard;
        IsKeyguardSecure(&secureKeyguard);
        if (appWindow) {
            AutoPtr<IApplicationToken> appToken;
            win->GetAppToken((IApplicationToken**)&appToken);
            if (showWhenLocked) {
                // Remove any previous windows with the same appToken.
                mAppsToBeHidden->Remove(appToken.Get());
                mAppsThatDismissKeyguard->Remove(appToken.Get());
                Boolean bTemp;
                if ((mAppsToBeHidden->IsEmpty(&bTemp), bTemp) && IsKeyguardSecureIncludingHidden()) {
                    mWinShowWhenLocked = win;
                    mHideLockScreen = TRUE;
                    mForceStatusBarFromKeyguard = FALSE;
                }
            }
            else if (dismissKeyguard) {
                if (secureKeyguard) {
                    mAppsToBeHidden->Add(appToken.Get());
                }
                else {
                    mAppsToBeHidden->Remove(appToken.Get());
                }

                mAppsThatDismissKeyguard->Add(appToken.Get());
            }
            else {
                mAppsToBeHidden->Add(appToken.Get());
            }

            Int32 x, y, width, height;
            attrs->GetX(&x);
            attrs->GetY(&y);
            IViewGroupLayoutParams::Probe(attrs)->GetWidth(&width);
            IViewGroupLayoutParams::Probe(attrs)->GetHeight(&height);
            if (x == 0 && y == 0
                && width == IViewGroupLayoutParams::MATCH_PARENT
                && height == IViewGroupLayoutParams::MATCH_PARENT) {
                if (DEBUG_LAYOUT) Slogger::V(TAG, "Fullscreen window: %s", TO_CSTR(win));
                mTopFullscreenOpaqueWindowState = win;
                Boolean isEmpty;
                if (!(mAppsThatDismissKeyguard->IsEmpty(&isEmpty), isEmpty) &&
                        mDismissKeyguard == DISMISS_KEYGUARD_NONE) {
                    if (DEBUG_LAYOUT) Slogger::V(TAG, "Setting mDismissKeyguard true by win %s", TO_CSTR(win));
                    mDismissKeyguard = mWinDismissingKeyguard.Get() == win ?  DISMISS_KEYGUARD_CONTINUE : DISMISS_KEYGUARD_START;
                    mWinDismissingKeyguard = win;
                    mForceStatusBarFromKeyguard = mShowingLockscreen && secureKeyguard;
                }
                else if ((mAppsToBeHidden->IsEmpty(&isEmpty), isEmpty) && showWhenLocked) {
                    if (DEBUG_LAYOUT) Slogger::V(TAG, "Setting mHideLockScreen to true by win %s", TO_CSTR(win));
                    mHideLockScreen = TRUE;
                    mForceStatusBarFromKeyguard = FALSE;
                }
                if ((fl & IWindowManagerLayoutParams::FLAG_ALLOW_LOCK_WHILE_SCREEN_ON) != 0) {
                    mAllowLockscreenWhenOn = TRUE;
                }
            }

            if (mWinShowWhenLocked != NULL) {
                AutoPtr<IApplicationToken> token1, token2;
                mWinShowWhenLocked->GetAppToken((IApplicationToken**)&token1);
                win->GetAppToken((IApplicationToken**)&token2);
                if (token1.Get() != token2.Get()) {
                    Boolean result;
                    win->HideLw(FALSE, &result);
                }
            }
        }
    }

    return NOERROR;
}

ECode PhoneWindowManager::FinishPostLayoutPolicyLw(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mWinShowWhenLocked != NULL && mTopFullscreenOpaqueWindowState != NULL) {
        AutoPtr<IApplicationToken> token1, token2;
        mWinShowWhenLocked->GetAppToken((IApplicationToken**)&token1);
        mTopFullscreenOpaqueWindowState->GetAppToken((IApplicationToken**)&token2);
        Boolean bval;
        if (token1 != token2 && (IsKeyguardLocked(&bval), bval)) {
            // A dialog is dismissing the keyguard. Put the wallpaper behind it and hide the
            // fullscreen window.
            // TODO: Make sure FLAG_SHOW_WALLPAPER is restored when dialog is dismissed. Or not.
            AutoPtr<IWindowManagerLayoutParams> attrs;
            mWinShowWhenLocked->GetAttrs((IWindowManagerLayoutParams**)&attrs);;
            Int32 flags;
            attrs->GetFlags(&flags);
            attrs->SetFlags(flags | IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
            mTopFullscreenOpaqueWindowState->HideLw(FALSE, &bval);
            mTopFullscreenOpaqueWindowState = mWinShowWhenLocked;
        }
    }

    Int32 changes = 0;
    Boolean topIsFullscreen = FALSE;

    AutoPtr<IWindowManagerLayoutParams> lp;
    if (mTopFullscreenOpaqueWindowState != NULL) {
        mTopFullscreenOpaqueWindowState->GetAttrs((IWindowManagerLayoutParams**)&lp);
    }

    // If we are not currently showing a dream then remember the current
    // lockscreen state.  We will use this to determine whether the dream
    // started while the lockscreen was showing and remember this state
    // while the dream is showing.
    if (!mShowingDream) {
        mDreamingLockscreen = mShowingLockscreen;
    }

    if (mStatusBar != NULL) {
        if (DEBUG_LAYOUT) {
            Slogger::I(TAG, "force=%d forcefkg=%d top=%s",
                mForceStatusBar, mForceStatusBarFromKeyguard, TO_CSTR(mTopFullscreenOpaqueWindowState));
        }

        Boolean bval;
        if (mForceStatusBar || mForceStatusBarFromKeyguard) {
            if (DEBUG_LAYOUT) Slogger::V(TAG, "Showing status bar: forced");
            if ((mStatusBarController->SetBarShowingLw(TRUE, &bval), bval)) {
                changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
            }
            // Maintain fullscreen layout until incoming animation is complete.
            topIsFullscreen = mTopIsFullscreen && (mStatusBar->IsAnimatingLw(&bval), bval);
            // Transient status bar on the lockscreen is not allowed
            if (mForceStatusBarFromKeyguard && (mStatusBarController->IsTransientShowing(&bval), bval)) {
                Int32 result;
                mStatusBarController->UpdateVisibilityLw(FALSE /*transientAllowed*/,
                        mLastSystemUiFlags, mLastSystemUiFlags, &result);
            }
        }
        else if (mTopFullscreenOpaqueWindowState != NULL) {
            Int32 fl = WindowManagerPolicyControl::GetWindowFlags(NULL, lp);
            // if (localLOGV) {
            //     Slogger::D(TAG, "frame: " + mTopFullscreenOpaqueWindowState.getFrameLw()
            //             + " shown frame: " + mTopFullscreenOpaqueWindowState.getShownFrameLw());
            //     Slogger::D(TAG, "attr: " + mTopFullscreenOpaqueWindowState.getAttrs()
            //             + " lp.flags=0x" + Integer.toHexString(lp.flags));
            // }
            topIsFullscreen = (fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0
                || (mLastSystemUiFlags & IView::SYSTEM_UI_FLAG_FULLSCREEN) != 0;
            // The subtle difference between the window for mTopFullscreenOpaqueWindowState
            // and mTopIsFullscreen is that that mTopIsFullscreen is set only if the window
            // has the FLAG_FULLSCREEN set.  Not sure if there is another way that to be the
            // case though.
            Boolean bTemp;
            if (mStatusBarController->IsTransientShowing(&bTemp), bTemp) {
                if (mStatusBarController->SetBarShowingLw(TRUE, &bTemp), bTemp) {
                    changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
                }
            }
            else if (topIsFullscreen) {
                if (DEBUG_LAYOUT) Slogger::V(TAG, "** HIDING status bar");
                if (mStatusBarController->SetBarShowingLw(FALSE, &bTemp), bTemp) {
                    changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
                }
                else {
                    if (DEBUG_LAYOUT) Slogger::V(TAG, "Status bar already hiding");
                }
            }
            else {
                if (DEBUG_LAYOUT) Slogger::V(TAG, "** SHOWING status bar: top is not fullscreen");
                if (mStatusBarController->SetBarShowingLw(TRUE, &bTemp), bTemp) {
                    changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
                }
            }
        }
    }

    if (mTopIsFullscreen != topIsFullscreen) {
        if (!topIsFullscreen) {
            // Force another layout when status bar becomes fully shown.
            changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
        }
        mTopIsFullscreen = topIsFullscreen;
    }

    // Hide the key guard if a visible window explicitly specifies that it wants to be
    // displayed when the screen is locked.
    if (mKeyguardDelegate != NULL && mStatusBar != NULL) {
        // if (localLOGV) Slogger::V(TAG, "finishPostLayoutPolicyLw: mHideKeyguard="
        //         + mHideLockScreen);
        Boolean bTemp;
        if (mDismissKeyguard != DISMISS_KEYGUARD_NONE && !(IsKeyguardSecure(&bTemp), bTemp)) {
            mKeyguardHidden = TRUE;
            Int32 ival;
            mKeyguardDelegate->SetOccluded(TRUE, &ival);
            if (ProcessKeyguardSetHiddenResultLw(ival)) {
                changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT | IWindowManagerPolicy::FINISH_LAYOUT_REDO_CONFIG | IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER;
            }
            if (mKeyguardDelegate->IsShowing(&bTemp), bTemp) {
                Boolean isSuccess = FALSE;
                mHandler->Post(new KeyguardDelegateKeyguardDone(this), &isSuccess);
            }
        }
        else if (mHideLockScreen) {
            mKeyguardHidden = TRUE;
            Int32 ival;
            mKeyguardDelegate->SetOccluded(TRUE, &ival);
            if (ProcessKeyguardSetHiddenResultLw(ival)) {
                changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT | IWindowManagerPolicy::FINISH_LAYOUT_REDO_CONFIG | IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER;
            }
        }
        else if (mDismissKeyguard != DISMISS_KEYGUARD_NONE) {
            // This is the case of keyguard isSecure() and not mHideLockScreen.
            if (mDismissKeyguard == DISMISS_KEYGUARD_START) {
                // Only launch the next keyguard unlock window once per window.
                mKeyguardHidden = FALSE;
                Int32 ival;
                mKeyguardDelegate->SetOccluded(FALSE, &ival);
                if (ProcessKeyguardSetHiddenResultLw(ival)) {
                    changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT | IWindowManagerPolicy::FINISH_LAYOUT_REDO_CONFIG | IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER;
                }
                Boolean isSuccess = FALSE;
                mHandler->Post(new KeyguardDelegateKeyguardDismiss(this), &isSuccess);
            }
        }
        else {
            mWinDismissingKeyguard = NULL;
            mKeyguardHidden = FALSE;
            Int32 ival;
            mKeyguardDelegate->SetOccluded(FALSE, &ival);
            if (ProcessKeyguardSetHiddenResultLw(ival)) {
                changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT | FINISH_LAYOUT_REDO_CONFIG | FINISH_LAYOUT_REDO_WALLPAPER;
            }
        }
    }

    if ((UpdateSystemUiVisibilityLw()&SYSTEM_UI_CHANGING_LAYOUT) != 0) {
        // If the navigation bar has been hidden or shown, we need to do another
        // layout pass to update that window.
        changes |= IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
    }

    // // update since mAllowLockscreenWhenOn might have changed
    UpdateLockScreenTimeout();
    *result = changes;
    return NOERROR;
}

ECode PhoneWindowManager::AllowAppAnimationsLw(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    if (IsStatusBarKeyguard() || mShowingDream) {
        // If keyguard is currently visible, no reason to animate
        // behind it.
        *allowed = FALSE;
    }
    else {
        *allowed = TRUE;
    }
    return NOERROR;
}

ECode PhoneWindowManager::FocusChangedLw(
    /* [in] */ IWindowState* lastFocus,
    /* [in] */ IWindowState* newFocus,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = 0;

    mFocusedWindow = newFocus;
    if ((UpdateSystemUiVisibilityLw() & SYSTEM_UI_CHANGING_LAYOUT) != 0) {
        // If the navigation bar has been hidden or shown, we need to do another
        // layout pass to update that window.
        *state = IWindowManagerPolicy::FINISH_LAYOUT_REDO_LAYOUT;
    }

    return NOERROR;
}

ECode PhoneWindowManager::NotifyLidSwitchChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Boolean lidOpen)
{
    // lid changed state
    const Int32 newLidState = lidOpen ?
            IWindowManagerPolicyWindowManagerFuncs::LID_OPEN :
            IWindowManagerPolicyWindowManagerFuncs::LID_CLOSED;
    if (newLidState == mLidState) {
        return NOERROR;
    }

    mLidState = newLidState;
    ApplyLidSwitchState();
    UpdateRotation(TRUE);

    if (lidOpen) {
        mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
    }
    else if (!mLidControlsSleep) {
        mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    }
    return NOERROR;
}

ECode PhoneWindowManager::NotifyCameraLensCoverSwitchChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Boolean lensCovered)
{
    Int32 lensCoverState =
        lensCovered ? IWindowManagerPolicyWindowManagerFuncs::CAMERA_LENS_COVERED
                    : IWindowManagerPolicyWindowManagerFuncs::CAMERA_LENS_UNCOVERED;
    if (mCameraLensCoverState == lensCoverState) {
        return NOERROR;
    }
    if (mCameraLensCoverState == IWindowManagerPolicyWindowManagerFuncs::CAMERA_LENS_COVERED &&
            lensCoverState == IWindowManagerPolicyWindowManagerFuncs::CAMERA_LENS_UNCOVERED) {
        AutoPtr<IIntent> intent;
        Boolean bTemp;
        Boolean keyguardActive = mKeyguardDelegate == NULL? FALSE:
                (mKeyguardDelegate->IsShowing(&bTemp), bTemp);
        if (keyguardActive) {
            CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA_SECURE, (IIntent**)&intent);
        } else {
            CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA, (IIntent**)&intent);
        }
        mPowerManager->WakeUp(whenNanos / 1000000);
        mContext->StartActivityAsUser(intent, UserHandle::CURRENT_OR_SELF);
    }
    mCameraLensCoverState = lensCoverState;
    return NOERROR;
}

void PhoneWindowManager::SetHdmiPlugged(
    /* [in] */ Boolean plugged)
{
    if (mHdmiPlugged != plugged) {
        mHdmiPlugged = plugged;
        UpdateRotation(TRUE, TRUE);
        AutoPtr<IIntent> intent;
        CIntent::New(ACTION_HDMI_PLUGGED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent->PutBooleanExtra(EXTRA_HDMI_PLUGGED_STATE, plugged);

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> all;
        helper->GetALL((IUserHandle**)&all);

        mContext->SendStickyBroadcastAsUser(intent, all);
    }
}

void PhoneWindowManager::InitializeHdmiState()
{
    Boolean plugged = FALSE;
    // watch for HDMI plug messages if the hdmi switch exists
    AutoPtr<IFile> file;
    CFile::New(String("/sys/devices/virtual/switch/hdmi/state"), (IFile**)&file);
    Boolean isExists = FALSE;
    if (file->Exists(&isExists), isExists) {
        mHDMIObserver->StartObserving(String("DEVPATH=/devices/virtual/switch/hdmi"));

        const String filename("/sys/class/switch/hdmi/state");
        AutoPtr<IFileReader> reader;
        // try {
        do {
            ECode ec;
            CFileReader::New(filename, (IFileReader**)&reader);
            AutoPtr< ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(15);
            Int32 n = 0;
            ec = IReader::Probe(reader)->Read(buf, &n);
            if (FAILED(ec)) {
                break;
            }
            if (n > 1) {
                Int32 value = 0;
                value  = StringUtils::ParseInt32(String(*buf, 0, n-1), 0);
                //if (FAILED(ec)) {
                //    break;
                //}
                plugged = 0 != value;
            }
        // } catch (IOException ex) {
        //     Slog.w(TAG, "Couldn't read hdmi state from " + filename + ": " + ex);
        // } catch (NumberFormatException ex) {
        //     Slog.w(TAG, "Couldn't read hdmi state from " + filename + ": " + ex);
        } while (FALSE);
        if (reader != NULL) {
            // try {
            AutoPtr<ICloseable> closeable = ICloseable::Probe(reader);
            closeable->Close();
            // } catch (IOException ex) {
        }
    }
    // This dance forces the code in setHdmiPlugged to run.
    // Always do this so the sticky intent is stuck (to false) if there is no hdmi.
    mHdmiPlugged = !plugged;
    SetHdmiPlugged(!mHdmiPlugged);
}

void PhoneWindowManager::TakeScreenshot()
{
    AutoLock lock(mScreenshotLock);
    if (mScreenshotConnection != NULL) {
        return;
    }
    AutoPtr<IComponentName> cn;
    CComponentName::New(String("com.android.systemui"),
        String("com.android.systemui.screenshot.TakeScreenshotService"),
        (IComponentName**)&cn);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(cn);
    AutoPtr<ScreenshotServiceConnection> conn = new ScreenshotServiceConnection(this);
    Boolean isSuccess = FALSE;
    if (mContext->BindServiceAsUser(
            intent, conn, IContext::BIND_AUTO_CREATE, UserHandle::CURRENT,
            &isSuccess), isSuccess)
    {
        mScreenshotConnection = conn;
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mScreenshotTimeout, 10000, &isSuccess);
    }
}

ECode PhoneWindowManager::InterceptKeyBeforeQueueing(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [out] */ Int32* bitwise)
{
    VALIDATE_NOT_NULL(bitwise)
    *bitwise = 0;

    Logger::I(TAG, " >>> InterceptKeyBeforeQueueing %s", TO_CSTR(event));

    if (!mSystemBooted) {
        // If we have not yet booted, don't let key events do anything.
        return NOERROR;
    }

    Boolean interactive = (policyFlags & IWindowManagerPolicy::FLAG_INTERACTIVE) != 0;
    Int32 action = 0, keyCode = 0, flags = 0;
    event->GetAction(&action);
    event->GetKeyCode(&keyCode);
    event->GetFlags(&flags);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    Boolean canceled = FALSE;
    event->IsCanceled(&canceled);

    Boolean isInjected = (policyFlags & IWindowManagerPolicy::FLAG_INJECTED) != 0;

    // If screen is off then we treat the case where the keyguard is open but hidden
    // the same as if it were open and in front.
    // This will prevent any keys other than the power button from waking the screen
    // when the keyguard is hidden by another activity.
    Boolean bTemp;
    Boolean keyguardActive = (mKeyguardDelegate == NULL ? FALSE :
         (interactive ?
             (mKeyguardDelegate->IsShowingAndNotOccluded(&bTemp), bTemp) :
             (mKeyguardDelegate->IsShowing(&bTemp), bTemp)));

    // if (DEBUG_INPUT) {
    //     Log.d(TAG, "interceptKeyTq keycode=" + keyCode
    //             + " interactive=" + interactive + " keyguardActive=" + keyguardActive
    //             + " policyFlags=" + Integer.toHexString(policyFlags));
    // }

    // Basic policy based on screen state and keyguard.
    Int32 result = 0;
    Boolean isWakeKey = (policyFlags & IWindowManagerPolicy::FLAG_WAKE) != 0
                        || (event->IsWakeKey(&bTemp), bTemp);
    if (interactive || (isInjected && !isWakeKey))
    {
        // When the screen is on or if the key is injected pass the key to the application.
        result = IWindowManagerPolicy::ACTION_PASS_TO_USER;
        isWakeKey = FALSE;
    } else if (!interactive && ShouldDispatchInputWhenNonInteractive()) {
        // If we're currently dozing with the screen on and the keyguard showing, pass the key
        // to the application but preserve its wake key status to make sure we still move
        // from dozing to fully interactive if we would normally go from off to fully
        // interactive.
        result = IWindowManagerPolicy::ACTION_PASS_TO_USER;
    } else {
        // When the screen is off and the key is not injected, determine whether
        // to wake the device but don't pass the key to the application.
        result = 0;
        if (isWakeKey && (!down || !IsWakeKeyWhenScreenOff(keyCode))) {
            isWakeKey = FALSE;
        }
    }

    // If the key would be handled globally, just return the result, don't worry about special
    // key processing.
    bTemp = FALSE;
    if (mGlobalKeyManager->ShouldHandleGlobalKey(keyCode, event, &bTemp), bTemp) {
        if (isWakeKey) {
            Int64 eventTime;
            IInputEvent::Probe(event)->GetEventTime(&eventTime);
            mPowerManager->WakeUp(eventTime);
        }
        *bitwise = result;
        return NOERROR;
    }

    Int32 count;
    event->GetRepeatCount(&count);
    Boolean useHapticFeedback = down
        && (policyFlags & IWindowManagerPolicy::FLAG_VIRTUAL) != 0
        && count == 0;

    // Handle special keys.
    switch (keyCode) {
    case IKeyEvent::KEYCODE_VOLUME_DOWN:
    case IKeyEvent::KEYCODE_VOLUME_UP:
    case IKeyEvent::KEYCODE_VOLUME_MUTE:
        {
            if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN) {
                if (down) {
                    if (interactive && !mVolumeDownKeyTriggered
                            && (flags & IKeyEvent::FLAG_FALLBACK) == 0) {
                        mVolumeDownKeyTriggered = TRUE;
                        event->GetDownTime(&mVolumeDownKeyTime);
                        mVolumeDownKeyConsumedByScreenshotChord = FALSE;
                        CancelPendingPowerKeyAction();
                        InterceptScreenshotChord();
                    }
                }
                else {
                    mVolumeDownKeyTriggered = FALSE;
                    CancelPendingScreenshotChordAction();
                }
            }
            else if (keyCode == IKeyEvent::KEYCODE_VOLUME_UP) {
                if (down) {
                    if (interactive && !mVolumeUpKeyTriggered
                            && (flags & IKeyEvent::FLAG_FALLBACK) == 0) {
                        mVolumeUpKeyTriggered = TRUE;
                        CancelPendingPowerKeyAction();
                        CancelPendingScreenshotChordAction();
                    }
                }
                else {
                    mVolumeUpKeyTriggered = FALSE;
                    CancelPendingScreenshotChordAction();
                }
            }

            if (down) {
                //TODO begin
                //AutoPtr<ITelecomManager> telecomManager = GetTelecommService();
                //Boolean bTemp;
                //if (telecomManager != NULL) {
                //    if (telecomManager->IsRinging(&bTemp), bTemp) {
                //        // If an incoming call is ringing, either VOLUME key means
                //        // "silence ringer".  We handle these keys here, rather than
                //        // in the InCallScreen, to make sure we'll respond to them
                //        // even if the InCallScreen hasn't come to the foreground yet.
                //        // Look for the DOWN event here, to agree with the "fallback"
                //        // behavior in the InCallScreen.
                //        Logger::I(TAG, "interceptKeyBeforeQueueing: VOLUME key-down while ringing: Silence ringer!");

                //        // Silence the ringer.  (It's safe to call this
                //        // even if the ringer has already been silenced.)
                //        telecomManager->SilenceRinger();

                //        // And *don't* pass this key thru to the current activity
                //        // (which is probably the InCallScreen.)
                //        result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
                //        break;
                //    }
                //    if ((telecomManager->IsInCall(&bTemp), bTemp)
                //            && (result & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0) {
                //        // If we are in call but we decided not to pass the key to
                //        // the application, handle the volume change here.
                //        AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
                //        CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
                //        AutoPtr<IMediaSessionLegacyHelper> mslHelper;
                //        mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
                //        mslHelper->SendVolumeKeyEvent(event, FALSE);
                //        break;
                //    }
                //}
                //TODO end

                if ((result & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0) {
                    // If we aren't passing to the user and no one else
                    // handled it send it to the session manager to figure
                    // out.
                    AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
                    CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
                    AutoPtr<IMediaSessionLegacyHelper> mslHelper;
                    mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
                    mslHelper->SendVolumeKeyEvent(event, TRUE);
                    break;
                }
            }
        }
        break;

    case IKeyEvent::KEYCODE_ENDCALL:
        {
            result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
            if (down) {
                //TODO begin
                //AutoPtr<ITelecomManager> telecomManager = GetTelecommService();
                //Boolean hungUp = FALSE;
                //if (telecomManager != NULL) {
                //    hungUp = telecomManager->EndCall();
                //}
                //InterceptPowerKeyDown(!interactive || hungUp);
                //TODO end
            }
            else {
                if (InterceptPowerKeyUp(canceled)) {
                    if ((mEndcallBehavior
                        & ISettingsSystem::END_BUTTON_BEHAVIOR_HOME/*0x1*/) != 0) {
                        if (GoHome()) {
                            break;
                        }
                    }

                    if ((mEndcallBehavior
                        & /*0x2*/ISettingsSystem::END_BUTTON_BEHAVIOR_SLEEP) != 0) {
                        Int64 eventTime;
                        IInputEvent::Probe(event)->GetEventTime(&eventTime);
                        //TODO mPowerManager->GoToSleep(eventTime, IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON, 0);
                        isWakeKey = FALSE;
                    }
                }
            }
        }
        break;
    case IKeyEvent::KEYCODE_POWER:
        {
            result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
            if (down) {
                Int64 downTime;
                event->GetDownTime(&downTime);
                Boolean panic = FALSE;
                if (panic)
                {
                    Boolean isSuccess = FALSE;
                    mHandler->Post(mRequestTransientNav, &isSuccess);
                }

                Int32 flags = 0;
                if (interactive && !mPowerKeyTriggered
                        && ((event->GetFlags(&flags), flags) & IKeyEvent::FLAG_FALLBACK) == 0) {
                    mPowerKeyTriggered = TRUE;
                    event->GetDownTime(&mPowerKeyTime);
                    InterceptScreenshotChord();
                }

                //TODO begin no implementation TelecomManager
                //AutoPtr<ITelecomManager> telecomManager = GetTelecommService();
                //Boolean hungUp = FALSE;
                //Boolean bTemp;
                //if (telecomManager != NULL) {
                //    if (telecomManager->IsRinging(&bTemp), bTemp) {
                //        // Pressing Power while there's a ringing incoming
                //        // call should silence the ringer.
                //        telecomManager->SilenceRinger();
                //    } else if ((mIncallPowerBehavior
                //                & ISettingsSecure::INCALL_POWER_BUTTON_BEHAVIOR_HANGUP) != 0
                //            && (telecomManager->IsInCall(&bTemp), bTemp) && interactive) {
                //        // Otherwise, if "Power button ends call" is enabled,
                //        // the Power button will hang up any current active call.
                //        telecomManager->EndCall(&hungUp);
                //    }
                //}

                //InterceptPowerKeyDown(!interactive || hungUp || mVolumeDownKeyTriggered || mVolumeUpKeyTriggered);
                //TODO end
            }
            else {
                mPowerKeyTriggered = FALSE;
                CancelPendingScreenshotChordAction();

                if (InterceptPowerKeyUp(canceled || mPendingPowerKeyUpCanceled)) {
                    if (mScreenOnEarly && !mScreenOnFully) {
                        Slogger::I(TAG, "Suppressed redundant power key press while already in the process of turning the screen on.");
                    } else {
                        Int64 eventTime;
                        IInputEvent::Probe(event)->GetEventTime(&eventTime);
                        PowerShortPress(eventTime);
                    }
                    isWakeKey = FALSE;
                }
                mPendingPowerKeyUpCanceled = FALSE;
            }
        }
        break;
    case IKeyEvent::KEYCODE_SLEEP: {
        result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
        Boolean bTemp;
        mPowerManager->IsInteractive(&bTemp);
        if (!bTemp) {
            useHapticFeedback = FALSE; // suppress feedback if already non-interactive
        }
        Int64 eventTime;
        IInputEvent::Probe(event)->GetEventTime(&eventTime);
        //TODO mPowerManager->GoToSleep(eventTime, IPowerManager::GO_TO_SLEEP_REASON_POWER_BUTTON, 0);
        isWakeKey = FALSE;
        break;
    }
    case IKeyEvent::KEYCODE_WAKEUP: {
        result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
        isWakeKey = TRUE;
        break;
    }
    case IKeyEvent::KEYCODE_MEDIA_PLAY:
    case IKeyEvent::KEYCODE_MEDIA_PAUSE:
    case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
    case IKeyEvent::KEYCODE_HEADSETHOOK:
    case IKeyEvent::KEYCODE_MUTE:
    case IKeyEvent::KEYCODE_MEDIA_STOP:
    case IKeyEvent::KEYCODE_MEDIA_NEXT:
    case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
    case IKeyEvent::KEYCODE_MEDIA_REWIND:
    case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
    case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK:
    {
        AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
        CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
        AutoPtr<IMediaSessionLegacyHelper> mslHelper;
        mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
        Boolean bTemp;
        if (mslHelper->IsGlobalPriorityActive(&bTemp), bTemp) {
            // If the global session is active pass all media keys to it
            // instead of the active window.
            result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
        }
        if ((result & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0) {
            // Only do this if we would otherwise not pass it to the user. In that
            // case, the PhoneWindow class will do the same thing, except it will
            // only do it if the showing app doesn't process the key on its own.
            // Note that we need to make a copy of the key event here because the
            // original key event will be recycled when we return.
            mBroadcastWakeLock->AcquireLock();
            AutoPtr<IMessage> msg;
            AutoPtr<IKeyEvent> keyEvent;
            CKeyEvent::New(event, (IKeyEvent**)&keyEvent);
            mHandler->ObtainMessage(MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK,
            TO_IINTERFACE(keyEvent), (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            msg->SendToTarget();
        }
    }
    break;
    case IKeyEvent::KEYCODE_CALL: {
        if (down) {
            ///TODO begin
            //Boolean bTemp;
            //AutoPtr<ITelecomManager> telecomManager = GetTelecommService();
            //if (telecomManager != NULL) {
            //    if (telecomManager->IsRinging(&bTemp), bTemp) {
            //        Logger::I(TAG, "interceptKeyBeforeQueueing: CALL key-down while ringing: Answer the call!");
            //        telecomManager->AcceptRingingCall();

            //        // And *don't* pass this key thru to the current activity
            //        // (which is presumably the InCallScreen.)
            //        result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
            //    }
            //}
            //TODO end
        }
        break;
    }
    case IKeyEvent::KEYCODE_VOICE_ASSIST: {
        // Only do this if we would otherwise not pass it to the user. In that case,
        // interceptKeyBeforeDispatching would apply a similar but different policy in
        // order to invoke voice assist actions. Note that we need to make a copy of the
        // key event here because the original key event will be recycled when we return.
        if ((result & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0 && !down) {
            mBroadcastWakeLock->AcquireLock();
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MSG_LAUNCH_VOICE_ASSIST_WITH_WAKE_LOCK,
            keyguardActive ? 1 : 0, 0, (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            msg->SendToTarget();
        }
    }
    default:
        break;
    }

    if (useHapticFeedback) {
        Boolean bTemp;
        PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::VIRTUAL_KEY, FALSE, &bTemp);
    }

    if (isWakeKey) {
        Int64 eventTime;
        IInputEvent::Probe(event)->GetEventTime(&eventTime);
        mPowerManager->WakeUp(eventTime);
    }

    *bitwise = result;
    Logger::I(TAG, " <<< InterceptKeyBeforeQueueing %s", TO_CSTR(event));
    return NOERROR;
}

Boolean PhoneWindowManager::IsWakeKeyWhenScreenOff(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        // ignore volume keys unless docked
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
            return mDockMode != IIntent::EXTRA_DOCK_STATE_UNDOCKED;

        // ignore media and camera keys
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK:
        case IKeyEvent::KEYCODE_CAMERA:
            return FALSE;
    }
    return TRUE;
}

Boolean PhoneWindowManager::ShouldDispatchInputWhenNonInteractive()
{
    if (KeyguardIsShowingTq() && mDisplay != NULL)
    {
        Int32 state;
        mDisplay->GetState(&state);
        if (state != IDisplay::STATE_OFF)
            return TRUE;
    }
    return FALSE;
}

void PhoneWindowManager::RequestTransientBars(
    /* [in] */ IWindowState* swipeTarget)
{
    AutoPtr<ISynchronize> obj;
    mWindowManagerFuncs->GetWindowManagerLock((ISynchronize**)&obj);
    AutoLock lock(obj);
    if (!IsUserSetupComplete()) {
        // Swipe-up for navigation bar is disabled during setup
        return;
    }
    Boolean sb;
    mStatusBarController->CheckShowTransientBarLw(&sb);
    Boolean nb;
    mNavigationBarController->CheckShowTransientBarLw(&nb);
    if (sb || nb) {
        IWindowState* barTarget = sb ? mStatusBar : mNavigationBar;
        if (sb ^ nb && barTarget != swipeTarget) {
            if (DEBUG) Slogger::D(TAG, "Not showing transient bar, wrong swipe target");
            return;
        }
        if (sb) mStatusBarController->ShowTransient();
        if (nb) mNavigationBarController->ShowTransient();
        mImmersiveModeConfirmation->ConfirmCurrentPrompt();
        UpdateSystemUiVisibilityLw();
    }
}

void PhoneWindowManager::FinishKeyguardDrawn()
{
    {
        AutoLock lock(mLock);
        if (!mAwake || mKeyguardDrawComplete) {
            return; // spurious
        }

        mKeyguardDrawComplete = TRUE;
        if (mKeyguardDelegate != NULL) {
            mHandler->RemoveMessages(MSG_KEYGUARD_DRAWN_TIMEOUT);
        }
    }

    FinishScreenTurningOn();
}

void PhoneWindowManager::FinishWindowsDrawn()
{
    {
        AutoLock lock(mLock);
        if (!mScreenOnEarly || mWindowManagerDrawComplete) {
            return; // spurious
        }

        mWindowManagerDrawComplete = TRUE;
    }

    FinishScreenTurningOn();
}

void PhoneWindowManager::FinishScreenTurningOn()
{
    AutoPtr<IScreenOnListener> listener;
    Boolean enableScreen;
    {
        AutoLock lock(mLock);
        //if (DEBUG_WAKEUP) Slog.d(TAG,
        //        "finishScreenTurningOn: mAwake=" + mAwake
        //        + ", mScreenOnEarly=" + mScreenOnEarly
        //        + ", mScreenOnFully=" + mScreenOnFully
        //        + ", mKeyguardDrawComplete=" + mKeyguardDrawComplete
        //        + ", mWindowManagerDrawComplete=" + mWindowManagerDrawComplete);

        if (mScreenOnFully || !mScreenOnEarly || !mWindowManagerDrawComplete
                || (mAwake && !mKeyguardDrawComplete)) {
            return; // spurious or not ready yet
        }

        if (DEBUG_WAKEUP) Slogger::I(TAG, "Finished screen turning on...");
        listener = mScreenOnListener;
        mScreenOnListener = NULL;
        mScreenOnFully = TRUE;

        // Remember the first time we draw the keyguard so we know when we're done with
        // the main part of booting and can enable the screen and hide boot messages.
        if (!mKeyguardDrawnOnce && mAwake) {
            mKeyguardDrawnOnce = TRUE;
            enableScreen = TRUE;
            if (mBootMessageNeedsHiding) {
                mBootMessageNeedsHiding = FALSE;
                HideBootMessages();
            }
        } else {
            enableScreen = FALSE;
        }
    }

    if (listener != NULL) {
        listener->OnScreenOn();
    }

    if (enableScreen) {
        //try {
        mWindowManager->EnableScreenIfNeeded();
        //} catch (RemoteException unhandled) {
        //}
    }
}

void PhoneWindowManager::HandleHideBootMessage()
{
    {
        AutoLock lock(mLock);
        if (!mKeyguardDrawnOnce) {
            mBootMessageNeedsHiding = TRUE;
            return; // keyguard hasn't drawn the first time yet, not done booting
        }
    }

    if (mBootMsgDialog != NULL) {
        if (DEBUG_WAKEUP) Slogger::D(TAG, "handleHideBootMessage: dismissing");
        mBootMsgDialog->Dismiss();
        mBootMsgDialog = NULL;
    }
}

ECode PhoneWindowManager::InterceptMotionBeforeQueueingNonInteractive(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Int32 policyFlags,
    /* [out] */ Int32* result)
{
    if ((policyFlags & IWindowManagerPolicy::FLAG_WAKE) != 0) {
        mPowerManager->WakeUp(whenNanos / 1000000);
        *result = 0;
        return NOERROR;
    }
    if (ShouldDispatchInputWhenNonInteractive()) {
        *result = IWindowManagerPolicy::ACTION_PASS_TO_USER;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

void PhoneWindowManager::DispatchMediaKeyWithWakeLock(
    /* [in] */ IKeyEvent* event)
{
    if (DEBUG_INPUT) {
        Slogger::D(TAG, "dispatchMediaKeyWithWakeLock: %p", event);
    }

    if (mHavePendingMediaKeyRepeatWithWakeLock) {
        if (DEBUG_INPUT) {
            Slogger::D(TAG, "dispatchMediaKeyWithWakeLock: canceled repeat");
        }

        mHandler->RemoveMessages(MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK);
        mHavePendingMediaKeyRepeatWithWakeLock = FALSE;
        mBroadcastWakeLock->ReleaseLock(); // pending repeat was holding onto the wake lock
    }

    DispatchMediaKeyWithWakeLockToAudioService(event);

    Int32 action, repeatCount;
    event->GetAction(&action);
    event->GetRepeatCount(&repeatCount);
    if (action == IKeyEvent::ACTION_DOWN && repeatCount == 0) {
        mHavePendingMediaKeyRepeatWithWakeLock = TRUE;

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK, event, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean isSuccess = FALSE;
        mHandler->SendMessageDelayed(msg, CViewConfiguration::GetKeyRepeatTimeout(), &isSuccess);
    }
    else {
        mBroadcastWakeLock->ReleaseLock();
    }
}

void PhoneWindowManager::DispatchMediaKeyRepeatWithWakeLock(
    /* [in] */ IKeyEvent* event)
{
    mHavePendingMediaKeyRepeatWithWakeLock = FALSE;

    Int32 flags;
    event->GetFlags(&flags);
    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    AutoPtr<IKeyEvent> repeatEvent;
    helper->ChangeTimeRepeat(event, SystemClock::GetUptimeMillis(),
        1, flags | IKeyEvent::FLAG_LONG_PRESS, (IKeyEvent**)&repeatEvent);
    if (DEBUG_INPUT) {
        Slogger::D(TAG, "dispatchMediaKeyRepeatWithWakeLock: %p", repeatEvent.Get());
    }

    DispatchMediaKeyWithWakeLockToAudioService(repeatEvent);
    mBroadcastWakeLock->ReleaseLock();
}

void PhoneWindowManager::DispatchMediaKeyWithWakeLockToAudioService(
    /* [in] */ IKeyEvent* event)
{
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
        CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
        AutoPtr<IMediaSessionLegacyHelper> mslHelper;
        mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
        mslHelper->SendMediaButtonEvent(event, TRUE);
    }
}

void PhoneWindowManager::LaunchVoiceAssistWithWakeLock(
    /* [in] */ Boolean keyguardActive)
{
    AutoPtr<IIntent> voiceIntent;
    CIntent::New(IRecognizerIntent::ACTION_VOICE_SEARCH_HANDS_FREE, (IIntent**)&voiceIntent);
    voiceIntent->PutExtra(IRecognizerIntent::EXTRA_SECURE, keyguardActive);
    mContext->StartActivityAsUser(voiceIntent, UserHandle::CURRENT_OR_SELF);
    mBroadcastWakeLock->ReleaseLock();
}

// Called on the PowerManager's Notifier thread.
ECode PhoneWindowManager::GoingToSleep(
    /* [in] */ Int32 why)
{
    //not implemented
    //TODO EventLog.writeEvent(70000, 0);
    if (DEBUG_WAKEUP) Slogger::I(TAG, "Going to sleep...");

    // We must get this work done here because the power manager will drop
    // the wake lock and let the system suspend once this function returns.
    {
        AutoLock lock(mLock);
        mAwake = FALSE;
        mKeyguardDrawComplete = FALSE;
        UpdateWakeGestureListenerLp();
        UpdateOrientationListenerLp();
        UpdateLockScreenTimeout();
    }

    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->OnScreenTurnedOff(why);
    }
    return NOERROR;
}

// Called on the PowerManager's Notifier thread.
ECode PhoneWindowManager::WakingUp()
{
    //TODO EventLog.writeEvent(70000, 1);
    if (DEBUG_WAKEUP) Slogger::I(TAG, "Waking up...");

    // Since goToSleep performs these functions synchronously, we must
    // do the same here.  We cannot post this work to a handler because
    // that might cause it to become reordered with respect to what
    // may happen in a future call to goToSleep.
    {
        AutoLock lock(mLock);
        mAwake = TRUE;
        mKeyguardDrawComplete = FALSE;
        if (mKeyguardDelegate != NULL) {
            mHandler->RemoveMessages(MSG_KEYGUARD_DRAWN_TIMEOUT);
            Boolean result;
            mHandler->SendEmptyMessageDelayed(MSG_KEYGUARD_DRAWN_TIMEOUT, 1000, &result);
        }

        UpdateWakeGestureListenerLp();
        UpdateOrientationListenerLp();
        UpdateLockScreenTimeout();
    }

    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->OnScreenTurnedOn(mKeyguardDelegateCallback);
        // ... eventually calls finishKeyguardDrawn
    } else {
        if (DEBUG_WAKEUP) Slogger::D(TAG, "null mKeyguardDelegate: setting mKeyguardDrawComplete.");
        FinishKeyguardDrawn();
    }
    return NOERROR;
}

ECode PhoneWindowManager::ScreenTurnedOff()
{
    if (DEBUG_WAKEUP) Slogger::I(TAG, "Screen turned off...");

    {
        AutoLock lock(mLock);
        mScreenOnEarly = FALSE;
        mScreenOnFully = FALSE;
        mWindowManagerDrawComplete = FALSE;
        mScreenOnListener = NULL;
        UpdateOrientationListenerLp();
    }
    return NOERROR;
}

ECode PhoneWindowManager::ScreenTurningOn(
    /* [in] */ IScreenOnListener* screenOnListener)
{
    if (DEBUG_WAKEUP) Slogger::I(TAG, "Screen turning on...");

    {
        AutoLock lock(mLock);
        mScreenOnEarly = TRUE;
        mScreenOnFully = FALSE;
        mWindowManagerDrawComplete = FALSE;
        mScreenOnListener = screenOnListener;
        UpdateOrientationListenerLp();
    }

    mWindowManagerInternal->WaitForAllWindowsDrawn(mWindowManagerDrawCallback,
            WAITING_FOR_DRAWN_TIMEOUT);
    // ... eventually calls finishWindowsDrawn
    return NOERROR;
}

ECode PhoneWindowManager::IsScreenOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScreenOnFully;
    return NOERROR;
}

ECode PhoneWindowManager::EnableKeyguard(
    /* [in] */ Boolean enabled)
{
    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->SetKeyguardEnabled(enabled);
    }
    return NOERROR;
}

ECode PhoneWindowManager::ExitKeyguardSecurely(
   /* [in] */ IOnKeyguardExitResult* callback)
{
    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->VerifyUnlock(callback);
    }

    //return E_NOT_IMPLEMENTED;
    return NOERROR;
}

Boolean PhoneWindowManager::KeyguardIsShowingTq()
{
    if (mKeyguardDelegate == NULL) return FALSE;
    Boolean result;
    mKeyguardDelegate->IsShowingAndNotOccluded(&result);
    return result;
}

// Returns true if keyguard is currently locked whether or not it is currently hidden.
Boolean PhoneWindowManager::IsKeyguardSecureIncludingHidden()
{
    Boolean isSecure, isShowing;
    mKeyguardDelegate->IsSecure(&isSecure);
    mKeyguardDelegate->IsShowing(&isShowing);
    return isSecure && isShowing;
}

ECode PhoneWindowManager::IsKeyguardLocked(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    *state = KeyguardOn();
    return NOERROR;
}

ECode PhoneWindowManager::IsKeyguardSecure(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    if (mKeyguardDelegate == NULL) {
        *state = FALSE;
        return NOERROR;
    }
    Boolean result;
    mKeyguardDelegate->IsSecure(&result);
    *state = result;
    return NOERROR;
}

ECode PhoneWindowManager::InKeyguardRestrictedKeyInputMode(
    /* [out] */ Boolean* restricted)
{
    VALIDATE_NOT_NULL(restricted);
    if (mKeyguardDelegate == NULL) {
        *restricted = FALSE;
        return NOERROR;
    }
    return mKeyguardDelegate->IsInputRestricted(restricted);
}

ECode PhoneWindowManager::DismissKeyguardLw()
{
    Boolean bTemp;
    if (mKeyguardDelegate != NULL && (mKeyguardDelegate->IsShowing(&bTemp), bTemp)) {
        Boolean isSuccess = FALSE;
        mHandler->Post(new KeyguardDelegateKeyguardDismiss(this), &isSuccess);
    }
    return NOERROR;
}

ECode PhoneWindowManager::NotifyActivityDrawnForKeyguardLw()
{
    if (mKeyguardDelegate != NULL)
    {
        Boolean isSuccess = FALSE;
        mHandler->Post(new KeyguardDelegateActivityDrawn(this), &isSuccess);
    }
    return NOERROR;
}

ECode PhoneWindowManager::IsKeyguardDrawnLw(
    /* [out] */ Boolean* result)
{
    AutoLock lock(mLock);
    return mKeyguardDrawnOnce;
}

ECode PhoneWindowManager::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->StartKeyguardExitAnimation(startTime, fadeoutDuration);
    }
    return NOERROR;
}

void PhoneWindowManager::SendCloseSystemWindows()
{
    SendCloseSystemWindows(mContext, String(NULL));
}

void PhoneWindowManager::SendCloseSystemWindows(
    /* [in] */ const String& reason)
{
    SendCloseSystemWindows(mContext, reason);
}

void PhoneWindowManager::SendCloseSystemWindows(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason)
{
    if (ActivityManagerNative::IsSystemReady()) {
        // try {
        ActivityManagerNative::GetDefault()->CloseSystemDialogs(reason);
        // } catch (RemoteException e) {
        // }
    }
}

ECode PhoneWindowManager::RotationForOrientationLw(
    /* [in] */ Int32 orientation,
    /* [in] */ Int32 lastRotation,
    /* [out] */ Int32* surfaceRotation)
{
    VALIDATE_NOT_NULL(surfaceRotation);
    // if (FALSE) {
    //     Slogger::V(TAG, "rotationForOrientationLw(orient="
    //                 + orientation + ", last=" + lastRotation
    //                 + "); user=" + mUserRotation + " "
    //                 + ((mUserRotationMode == WindowManagerPolicy.USER_ROTATION_LOCKED)
    //                     ? "USER_ROTATION_LOCKED" : "")
    //                 );
    // }
    if (mForceDefaultOrientation) {
        *surfaceRotation = ISurface::ROTATION_0;
        return NOERROR;
    }

    AutoLock lock(mLock);
    Int32 sensorRotation = -1;
    mOrientationListener->GetProposedRotation(&sensorRotation); // may be -1
    if (sensorRotation < 0) {
        sensorRotation = lastRotation;
    }

    Int32 preferredRotation;
    if (mLidState == IWindowManagerPolicyWindowManagerFuncs::LID_OPEN && mLidOpenRotation >= 0) {
        // Ignore sensor when lid switch is open and rotation is forced.
        preferredRotation = mLidOpenRotation;
    } else if (mDockMode == IIntent::EXTRA_DOCK_STATE_CAR
            && (mCarDockEnablesAccelerometer || mCarDockRotation >= 0)) {
        // Ignore sensor when in car dock unless explicitly enabled.
        // This case can override the behavior of NOSENSOR, and can also
        // enable 180 degree rotation while docked.
        preferredRotation = mCarDockEnablesAccelerometer
                ? sensorRotation : mCarDockRotation;
    } else if ((mDockMode == IIntent::EXTRA_DOCK_STATE_DESK
            || mDockMode == IIntent::EXTRA_DOCK_STATE_LE_DESK
            || mDockMode == IIntent::EXTRA_DOCK_STATE_HE_DESK)
            && (mDeskDockEnablesAccelerometer || mDeskDockRotation >= 0)) {
        // Ignore sensor when in desk dock unless explicitly enabled.
        // This case can override the behavior of NOSENSOR, and can also
        // enable 180 degree rotation while docked.
        preferredRotation = mDeskDockEnablesAccelerometer
                ? sensorRotation : mDeskDockRotation;
    } else if (mHdmiPlugged && mDemoHdmiRotationLock) {
        // Ignore sensor when plugged into HDMI.
        // Note that the dock orientation overrides the HDMI orientation.
        preferredRotation = mDemoHdmiRotation;
    } else if (mHdmiPlugged && mDockMode == IIntent::EXTRA_DOCK_STATE_UNDOCKED
                                && mUndockedHdmiRotation >= 0) {
        // Ignore sensor when plugged into HDMI and an undocked orientation has
        // been specified in the configuration (only for legacy devices without
        // full multi-display support).
        // Note that the dock orientation overrides the HDMI orientation.
        preferredRotation = mUndockedHdmiRotation;
    } else if (orientation == IActivityInfo::SCREEN_ORIENTATION_LOCKED) {
        // Application just wants to remain locked in the last rotation.
        preferredRotation = lastRotation;
    } else if (!mSupportAutoRotation) {
        // If we don't support auto-rotation then bail out here and ignore
        // the sensor and any rotation lock settings.
        preferredRotation = -1;
    } else if ((mUserRotationMode == IWindowManagerPolicy::USER_ROTATION_FREE
                    && (orientation == IActivityInfo::SCREEN_ORIENTATION_USER
                            || orientation == IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED
                            || orientation == IActivityInfo::SCREEN_ORIENTATION_USER_LANDSCAPE
                            || orientation == IActivityInfo::SCREEN_ORIENTATION_USER_PORTRAIT
                            || orientation == IActivityInfo::SCREEN_ORIENTATION_FULL_USER))
            || orientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR
            || orientation == IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR
            || orientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE
            || orientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT) {
        // Otherwise, use sensor only if requested by the application or enabled
        // by default for USER or UNSPECIFIED modes.  Does not apply to NOSENSOR.
        if (mAllowAllRotations < 0) {
            // Can't read this during init() because the context doesn't
            // have display metrics at that time so we cannot determine
            // tablet vs. phone then.
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            Boolean isAllowAllRotations = FALSE;
            resources->GetBoolean(R::bool_::config_allowAllRotations, &isAllowAllRotations);
            mAllowAllRotations = isAllowAllRotations? 1 : 0;

        }
        if (sensorRotation != ISurface::ROTATION_180
                || mAllowAllRotations == 1
                || orientation == IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR
                || orientation == IActivityInfo::SCREEN_ORIENTATION_FULL_USER) {
            preferredRotation = sensorRotation;
        } else {
            preferredRotation = lastRotation;
        }
    } else if (mUserRotationMode == IWindowManagerPolicy::USER_ROTATION_LOCKED
            && orientation != IActivityInfo::SCREEN_ORIENTATION_NOSENSOR) {
        // Apply rotation lock.  Does not apply to NOSENSOR.
        // The idea is that the user rotation expresses a weak preference for the direction
        // of gravity and as NOSENSOR is never affected by gravity, then neither should
        // NOSENSOR be affected by rotation lock (although it will be affected by docks).
        preferredRotation = mUserRotation;
    } else {
        // No overriding preference.
        // We will do exactly what the application asked us to do.
        preferredRotation = -1;
    }

    switch (orientation) {
        case IActivityInfo::SCREEN_ORIENTATION_PORTRAIT:
            // Return portrait unless overridden.
            if (IsAnyPortrait(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            *surfaceRotation = mPortraitRotation;
            return NOERROR;

        case IActivityInfo::SCREEN_ORIENTATION_LANDSCAPE:
            // Return landscape unless overridden.
            if (IsLandscapeOrSeascape(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            *surfaceRotation = mLandscapeRotation;
            return NOERROR;

        case IActivityInfo::SCREEN_ORIENTATION_REVERSE_PORTRAIT:
            // Return reverse portrait unless overridden.
            if (IsAnyPortrait(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            *surfaceRotation = mUpsideDownRotation;
            return NOERROR;

        case IActivityInfo::SCREEN_ORIENTATION_REVERSE_LANDSCAPE:
            // Return seascape unless overridden.
            if (IsLandscapeOrSeascape(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            *surfaceRotation = mSeascapeRotation;
            return NOERROR;

        case IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE:
        case IActivityInfo::SCREEN_ORIENTATION_USER_LANDSCAPE:
            // Return either landscape rotation.
            if (IsLandscapeOrSeascape(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            if (IsLandscapeOrSeascape(lastRotation)) {
                *surfaceRotation = lastRotation;
                return NOERROR;
            }
            *surfaceRotation = mLandscapeRotation;
            return NOERROR;

        case IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT:
        case IActivityInfo::SCREEN_ORIENTATION_USER_PORTRAIT:
            // Return either portrait rotation.
            if (IsAnyPortrait(preferredRotation)) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }
            if (IsAnyPortrait(lastRotation)) {
                *surfaceRotation = lastRotation;
                return NOERROR;
            }
            *surfaceRotation = mPortraitRotation;
            return NOERROR;

        default:
            // For USER, UNSPECIFIED, NOSENSOR, SENSOR and FULL_SENSOR,
            // just return the preferred orientation we already calculated.
            if (preferredRotation >= 0) {
                *surfaceRotation = preferredRotation;
                return NOERROR;
            }

            *surfaceRotation = ISurface::ROTATION_0;
            return NOERROR;
    }
    return NOERROR;
}

ECode PhoneWindowManager::RotationHasCompatibleMetricsLw(
    /* [in] */ Int32 orientation,
    /* [in] */ Int32 rotation,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    *result = TRUE;

    switch (orientation) {
        case IActivityInfo::SCREEN_ORIENTATION_PORTRAIT:
        case IActivityInfo::SCREEN_ORIENTATION_REVERSE_PORTRAIT:
        case IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT: {
            *result = IsAnyPortrait(rotation);
            return NOERROR;
        }

        case IActivityInfo::SCREEN_ORIENTATION_LANDSCAPE:
        case IActivityInfo::SCREEN_ORIENTATION_REVERSE_LANDSCAPE:
        case IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE: {
            *result = IsLandscapeOrSeascape(rotation);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode PhoneWindowManager::SetRotationLw(
    /* [in] */ Int32 rotation)
{
    mOrientationListener->SetCurrentRotation(rotation);
    return NOERROR;
}

Boolean PhoneWindowManager::IsLandscapeOrSeascape(
    /* [in] */ Int32 rotation)
{
    return rotation == mLandscapeRotation || rotation == mSeascapeRotation;
}

Boolean PhoneWindowManager::IsAnyPortrait(
    /* [in] */ Int32 rotation)
{
    return rotation == mPortraitRotation || rotation == mUpsideDownRotation;
}

ECode PhoneWindowManager::GetUserRotationMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);
    Int32 value;
    Settings::System::GetInt32ForUser(res, ISettingsSystem::ACCELEROMETER_ROTATION, 0, IUserHandle::USER_CURRENT, &value);
    *result = value != 0 ?  IWindowManagerPolicy::USER_ROTATION_FREE : IWindowManagerPolicy::USER_ROTATION_LOCKED;
    return NOERROR;
}

ECode PhoneWindowManager::SetUserRotationMode(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 rot)
{
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);

    // mUserRotationMode and mUserRotation will be assigned by the content observer
    Boolean result;
    if (mode == IWindowManagerPolicy::USER_ROTATION_LOCKED) {
        Settings::System::PutInt32ForUser(res,
                ISettingsSystem::USER_ROTATION,
                rot,
                IUserHandle::USER_CURRENT, &result);

        Settings::System::PutInt32ForUser(res,
                ISettingsSystem::ACCELEROMETER_ROTATION,
                0,
                IUserHandle::USER_CURRENT, &result);
    } else {
        Settings::System::PutInt32ForUser(res,
                ISettingsSystem::ACCELEROMETER_ROTATION,
                1,
                IUserHandle::USER_CURRENT, &result);
    }
    return NOERROR;
}

ECode PhoneWindowManager::SetSafeMode(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;
    Boolean isSuccess = FALSE;
    return PerformHapticFeedbackLw(NULL, safeMode
            ? IHapticFeedbackConstants::SAFE_MODE_ENABLED
            : IHapticFeedbackConstants::SAFE_MODE_DISABLED, TRUE, &isSuccess);
}

AutoPtr< ArrayOf<Int64> > PhoneWindowManager::GetLongIntArray(
    /* [in] */ IResources* r,
    /* [in] */ Int32 resid)
{
    AutoPtr< ArrayOf<Int32> > ar;
    r->GetInt32Array(resid, (ArrayOf<Int32>**)&ar);
    if (ar == NULL) {
        return NULL;
    }
    Int32 size = ar->GetLength();
    AutoPtr< ArrayOf<Int64> > out = ArrayOf<Int64>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        (*out)[i] = (*ar)[i];
    }
    return out;
}

AutoPtr<IIDreamManager> PhoneWindowManager::GetDreamManager()
{
    AutoPtr<IIDreamManager> dreamManager;
    AutoPtr<IInterface> tmpObj = ServiceManager::CheckService(IDreamService::DREAM_SERVICE);
    dreamManager = IIDreamManager::Probe(tmpObj.Get());
    return dreamManager;
}

ECode PhoneWindowManager::SystemReady()
{
    CKeyguardServiceDelegate::New(mContext, NULL, (IKeyguardServiceDelegate**)&mKeyguardDelegate);
    mKeyguardDelegate->OnSystemReady();
    ReadCameraLensCoverState();
    UpdateUiMode();

    AutoLock lock(mLock);
    UpdateOrientationListenerLp();
    mSystemReady = TRUE;
    AutoPtr<UpdateSettingRunnable> updateSettingRunnable = new UpdateSettingRunnable(this);
    Boolean isSuccess = FALSE;
    ECode ec = mHandler->Post(updateSettingRunnable, &isSuccess);
    if (FAILED(ec)) {
        Logger::E(TAG, " =========PhoneWindowManager::SystemReady() %08x", ec);
        assert(0 && "TODO");
    }
    return ec;
}

ECode PhoneWindowManager::SystemBooted()
{
    if (mKeyguardDelegate != NULL)
    {
        mKeyguardDelegate->BindService(mContext);
        mKeyguardDelegate->OnBootCompleted();
    }
    {
        AutoLock lock(mLock);
        mSystemBooted = TRUE;
    }
    WakingUp();
    ScreenTurningOn(NULL);
    return NOERROR;
}

ECode PhoneWindowManager::ShowBootMessage(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean always)
{
    AutoPtr<BootMsgRunnable> bootMsgRunnable = new BootMsgRunnable(this, msg);
    Boolean isSuccess = FALSE;
    return mHandler->Post(bootMsgRunnable, & isSuccess);
}

ECode PhoneWindowManager::HideBootMessages()
{
    Boolean isSucceed;
    return mHandler->SendEmptyMessage(MSG_HIDE_BOOT_MESSAGE, &isSucceed);
}

ECode PhoneWindowManager::UserActivity()
{
    // ***************************************
    // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
    // ***************************************
    // THIS IS CALLED FROM DEEP IN THE POWER MANAGER
    // WITH ITS LOCKS HELD.
    //
    // This code must be VERY careful about the locks
    // it acquires.
    // In fact, the current code acquires way too many,
    // and probably has lurking deadlocks.

    AutoLock lock(mScreenLockTimeout);
    if (mLockScreenTimerActive) {
        // reset the timer
        mHandler->RemoveCallbacks(mScreenLockTimeout);
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mScreenLockTimeout, mLockScreenTimeout, &isSuccess);
    }
    return NOERROR;
}

ECode PhoneWindowManager::LockNow(
    /* [in] */ IBundle* options)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::DEVICE_POWER, String(NULL));
    mHandler->RemoveCallbacks(mScreenLockTimeout);

    if (options != NULL) {
        // In case multiple calls are made to lockNow, we don't wipe out the options
        // until the runnable actually executes.
        mScreenLockTimeout->SetLockOptions(options);
    }
    Boolean isSuccess = FALSE;
    return mHandler->Post(mScreenLockTimeout, &isSuccess);
}

void PhoneWindowManager::UpdateLockScreenTimeout()
{
    AutoLock lock(mScreenLockTimeout);
    Boolean bTemp;
    Boolean enable = (mAllowLockscreenWhenOn && mAwake &&
             mKeyguardDelegate != NULL && (mKeyguardDelegate->IsSecure(&bTemp), bTemp));
    if (mLockScreenTimerActive != enable) {
        if (enable) {
            // if (localLOGV) Log.v(TAG, "setting lockscreen timer");
            Boolean isSuccess = FALSE;
            mHandler->PostDelayed(mScreenLockTimeout, mLockScreenTimeout, &isSuccess);
        } else {
            // if (localLOGV) Log.v(TAG, "clearing lockscreen timer");
            mHandler->RemoveCallbacks(mScreenLockTimeout);
        }
        mLockScreenTimerActive = enable;
    }
}

ECode PhoneWindowManager::EnableScreenAfterBoot()
{
    ReadLidState();
    ApplyLidSwitchState();
    UpdateRotation(TRUE);

    return NOERROR;
}

void PhoneWindowManager::ApplyLidSwitchState()
{
    if (mLidState == IWindowManagerPolicyWindowManagerFuncs::LID_CLOSED && mLidControlsSleep) {
        //TODO mPowerManager->GoToSleep(SystemClock::GetUptimeMillis(), IPowerManager::GO_TO_SLEEP_REASON_LID_SWITCH, IPowerManager::GO_TO_SLEEP_FLAG_NO_DOZE);
    }

    {
        AutoLock lock(mLock);
        UpdateWakeGestureListenerLp();
    }
}

void PhoneWindowManager::UpdateUiMode()
{
    if (mUiModeManager == NULL) {
        AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::UI_MODE_SERVICE);
        mUiModeManager = IIUiModeManager::Probe(tmpObj);
        assert(mUiModeManager != NULL);
    }
    //try {
        mUiModeManager->GetCurrentModeType(&mUiMode);
    //} catch (RemoteException e) {
    //}
}

void PhoneWindowManager::UpdateRotation(
    /* [in] */ Boolean alwaysSendConfiguration)
{
    // try {
    //set orientation on WindowManager
    mWindowManager->UpdateRotation(alwaysSendConfiguration, FALSE);
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

void PhoneWindowManager::UpdateRotation(
    /* [in] */ Boolean alwaysSendConfiguration,
    /* [in] */ Boolean forceRelayout)
{
    // try {
    //set orientation on WindowManager
    mWindowManager->UpdateRotation(alwaysSendConfiguration, forceRelayout);
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

AutoPtr<IIntent> PhoneWindowManager::CreateHomeDockIntent()
{
    AutoPtr<IIntent> intent;

    // What home does is based on the mode, not the dock state.  That
    // is, when in car mode you should be taken to car home regardless
    // of whether we are actually in a car dock.
    if (mUiMode == IConfiguration::UI_MODE_TYPE_CAR) {
        if (ENABLE_CAR_DOCK_HOME_CAPTURE) {
            intent = mCarDockIntent;
        }
    } else if (mUiMode == IConfiguration::UI_MODE_TYPE_DESK) {
        if (ENABLE_DESK_DOCK_HOME_CAPTURE) {
            intent = mDeskDockIntent;
        }
    } else if (mUiMode == IConfiguration::UI_MODE_TYPE_WATCH
            && (mDockMode == IIntent::EXTRA_DOCK_STATE_DESK
                || mDockMode == IIntent::EXTRA_DOCK_STATE_HE_DESK
                || mDockMode == IIntent::EXTRA_DOCK_STATE_LE_DESK)) {
        // Always launch dock home from home when watch is docked, if it exists.
        intent = mDeskDockIntent;
    }

    if (intent == NULL) {
        return NULL;
    }

    AutoPtr<IActivityInfo> ai;
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResolveInfo> info;
    pm->ResolveActivityAsUser(
            intent,
            IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            mCurrentUserId, (IResolveInfo**)&info);
    if (info != NULL) {
        info->GetActivityInfo((IActivityInfo**)&ai);
    }

    if (ai != NULL) {
        AutoPtr<IBundle> metaData;
        IPackageItemInfo::Probe(ai)->GetMetaData((IBundle**)&metaData);
        if (metaData != NULL) {
            Boolean bTemp;
            if (metaData->GetBoolean(IIntent::METADATA_DOCK_HOME, &bTemp), bTemp) {
                intent = NULL;
                CIntent::New((IIntent**)&intent);
                String packageName;
                String name;
                IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
                IPackageItemInfo::Probe(ai)->GetName(&name);
                intent->SetClassName(packageName, name);
                return intent;
            }
        }
    }

    return NULL;
}

void PhoneWindowManager::StartDockOrHome()
{
    // We don't have dock home anymore. Home is home. If you lived here, you'd be home by now.
    AwakenDreams();

    AutoPtr<IIntent> dock = CreateHomeDockIntent();
    if (dock != NULL) {
        //try {
        mContext->StartActivityAsUser(dock, UserHandle::CURRENT);
        return;
        //} catch (ActivityNotFoundException e) {
        //}
    }
    mContext->StartActivityAsUser(mHomeIntent, UserHandle::CURRENT);
}

/**
* goes to the home screen
* @return whether it did anything
*/
Boolean PhoneWindowManager::GoHome()
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();

    if (FALSE) {
        // This code always brings home to the front.
        // try {

        am->StopAppSwitches();
        // } catch (RemoteException e) {
        // }
        SendCloseSystemWindows();
        StartDockOrHome();
    }
    else {
        // This code brings home to the front or, if it is already
        // at the front, puts the device to sleep.
        // try {
        String nullStr;
        Int32 testMode;
        SystemProperties::GetInt32(String("persist.sys.uts-test-mode"), 0, &testMode);
        if (testMode == 1) {
            /// Roll back EndcallBehavior as the cupcake design to pass P1 lab entry.
            // Logger::D(TAG, "UTS-TEST-MODE");
        }
        else {
            am->StopAppSwitches();
            SendCloseSystemWindows();
            AutoPtr<IIntent> dock = CreateHomeDockIntent();
            if (dock != NULL) {
                AutoPtr<IContentResolver> resolver;
                mContext->GetContentResolver((IContentResolver**)&resolver);
                String type;
                dock->ResolveTypeIfNeeded(resolver, &type);
                Int32 result;
                am->StartActivityAsUser(NULL, nullStr, dock,
                    type, NULL, nullStr, 0,
                    IActivityManager::START_FLAG_ONLY_IF_NEEDED,
                    NULL, NULL, IUserHandle::USER_CURRENT, &result);
                if (result == IActivityManager::START_RETURN_INTENT_TO_CALLER) {
                    return FALSE;
                }
            }
        }

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        String type;
        mHomeIntent->ResolveTypeIfNeeded(resolver, &type);
        Int32 result;
        am->StartActivityAsUser(NULL, nullStr, mHomeIntent, type, NULL, nullStr, 0,
            IActivityManager::START_FLAG_ONLY_IF_NEEDED, NULL, NULL,
            IUserHandle::USER_CURRENT, &result);
        if (result == IActivityManager::START_RETURN_INTENT_TO_CALLER) {
            return FALSE;
        }
        // } catch (RemoteException ex) {
        //     // bummer, the activity manager, which is in this process, is dead
        // }
    }
    return TRUE;
}

//TODO start TelecomManager is not implemented
//AutoPtr<ITelecomManager> PhoneWindowManager::GetTelecommService()
//{
    // AutoPtr<IInterface> service;
    // mContext->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
//    return ITelecomManager::Probe(service);
//}
//TODO end

ECode PhoneWindowManager::SetCurrentOrientationLw(
    /* [in] */ Int32 newOrientation)
{
    AutoLock lock(mLock);

    if (newOrientation != mCurrentAppOrientation) {
        mCurrentAppOrientation = newOrientation;
        UpdateOrientationListenerLp();
    }

    return NOERROR;
}

void PhoneWindowManager::PerformAuditoryFeedbackForAccessibilityIfNeed()
{
    if (!IsGlobalAccessibilityGestureEnabled()) {
        return;
    }

    AutoPtr<IInterface> audioService;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(audioService);

    Boolean isSilentMode;
    audioManager->IsSilentMode(&isSilentMode);
    if (isSilentMode) {
        return;
    }

    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    AutoPtr<IRingtone> ringTone;
    helper->GetRingtone(mContext, Settings::System::DEFAULT_NOTIFICATION_URI, (IRingtone**)&ringTone);
    ringTone->SetStreamType(IAudioManager::STREAM_MUSIC);
    ringTone->Play();
}

Boolean PhoneWindowManager::IsGlobalAccessibilityGestureEnabled()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 gestureEnabled;
    global->GetInt32(resolver, ISettingsGlobal::ENABLE_ACCESSIBILITY_GLOBAL_GESTURE_ENABLED,
        0, &gestureEnabled);
    return (gestureEnabled == 1);
}

ECode PhoneWindowManager::PerformHapticFeedbackLw(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 effectId,
    /* [in] */ Boolean always,
    /* [out] */ Boolean* isSucceed)
{
    VALIDATE_NOT_NULL(isSucceed);
    Boolean isHasVibrator = FALSE;
    if (!(mVibrator->HasVibrator(&isHasVibrator), isHasVibrator)) {
        *isSucceed = FALSE;
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Boolean hapticsDisabled = FALSE;
    Int32 value;
    Settings::System::GetInt32ForUser(contentResolver,
            ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 0, IUserHandle::USER_CURRENT, &value);
    hapticsDisabled = value == 0;
    if (hapticsDisabled && !always) {
         *isSucceed = FALSE;
         return NOERROR;
    }
    AutoPtr<ArrayOf<Int64> > pattern;
    switch (effectId) {
        case IHapticFeedbackConstants::LONG_PRESS:
            pattern = mLongPressVibePattern;
            break;
        case IHapticFeedbackConstants::VIRTUAL_KEY:
            pattern = mVirtualKeyVibePattern;
            break;
        case IHapticFeedbackConstants::KEYBOARD_TAP:
            pattern = mKeyboardTapVibePattern;
            break;
        case IHapticFeedbackConstants::CLOCK_TICK:
            pattern = mClockTickVibePattern;
            break;
        case IHapticFeedbackConstants::CALENDAR_DATE:
            pattern = mCalendarDateVibePattern;
            break;
        case IHapticFeedbackConstants::SAFE_MODE_DISABLED:
            pattern = mSafeModeDisabledVibePattern;
            break;
        case IHapticFeedbackConstants::SAFE_MODE_ENABLED:
            pattern = mSafeModeEnabledVibePattern;
            break;
        default:
            *isSucceed = FALSE;
           return NOERROR;
    }
    Int32 owningUid;
    String owningPackage;
    if (win != NULL) {
        win->GetOwningUid(&owningUid);
        win->GetOwningPackage(&owningPackage);
    } else {
        owningUid = Process::MyUid();
        mContext->GetOpPackageName(&owningPackage);
    }

    if (pattern->GetLength() == 1) {
        // One-shot vibration
        mVibrator->Vibrate(owningUid, owningPackage, (*pattern)[0], VIBRATION_ATTRIBUTES);
    } else {
        // Pattern vibration
        mVibrator->Vibrate(owningUid, owningPackage, pattern, -1, VIBRATION_ATTRIBUTES);
    }
    *isSucceed = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::KeepScreenOnStartedLw()
{
    return NOERROR;
}

ECode PhoneWindowManager::KeepScreenOnStoppedLw()
{
    Boolean bTemp;
    if (mKeyguardDelegate != NULL && !(mKeyguardDelegate->IsShowingAndNotOccluded(&bTemp), bTemp)) {
        mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    }

    return E_NOT_IMPLEMENTED;
}

Int32 PhoneWindowManager::UpdateSystemUiVisibilityLw()
{
    // If there is no window focused, there will be nobody to handle the events
    // anyway, so just hang on in whatever state we're in until things settle down.
    IWindowState* win = mFocusedWindow != NULL? mFocusedWindow : mTopFullscreenOpaqueWindowState;
    if (win == NULL) {
        return 0;
    }

    AutoPtr<IWindowManagerLayoutParams> tmp;
    win->GetAttrs((IWindowManagerLayoutParams**)&tmp);
    Int32 privateFlags = 0;
    tmp->GetPrivateFlags(&privateFlags);
    if ((privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0 && mHideLockScreen == TRUE) {
        // We are updating at a point where the keyguard has gotten
        // focus, but we were last in a state where the top window is
        // hiding it.  This is probably because the keyguard as been
        // shown while the top window was displayed, so we want to ignore
        // it here because this is just a very transient change and it
        // will quickly lose focus once it correctly gets hidden.
        return 0;
    }

    Int32 uiVisiable = WindowManagerPolicyControl::GetSystemUiVisibility(win, NULL);
    Int32 visibility = uiVisiable & ~mResettingSystemUiFlags;
    visibility &= ~mForceClearedSystemUiFlags;

    if (mForcingShowNavBar) {
        Int32 sl;
        win->GetSurfaceLayer(&sl);
        if (sl < mForcingShowNavBarLayer) {
            Int32 fgs = WindowManagerPolicyControl::AdjustClearableFlags(win, IView::SYSTEM_UI_CLEARABLE_FLAGS);
            visibility &= ~fgs;
        }
    }

    visibility = UpdateSystemBarsLw(win, mLastSystemUiFlags, visibility);
    Int32 diff = visibility ^ mLastSystemUiFlags;
    Boolean needsMenu = FALSE;
    win->GetNeedsMenuLw(mTopFullscreenOpaqueWindowState, &needsMenu);

    AutoPtr<IApplicationToken> appToken;
    win->GetAppToken((IApplicationToken**)&appToken);
    if (diff == 0 && mLastFocusNeedsMenu == needsMenu
            && mFocusedApp == appToken) {
        return 0;
    }

    mLastSystemUiFlags = visibility;
    mLastFocusNeedsMenu = needsMenu;
    mFocusedApp = appToken;

    Boolean result;
    AutoPtr<IRunnable> runnable = new UpdateSystemUiVisibilityRunnable(visibility, needsMenu, this);
    mHandler->Post(runnable, &result);

    return diff;
}

Int32 PhoneWindowManager::UpdateSystemBarsLw(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 oldVis,
    /* [in] */ Int32 vis)
{
    // apply translucent bar vis flags
    IWindowState* transWin = IsStatusBarKeyguard() && !mHideLockScreen
        ? mStatusBar : mTopFullscreenOpaqueWindowState;
    Int32 iTemp = 0;
    mStatusBarController->ApplyTranslucentFlagLw(transWin, vis, oldVis, &iTemp);
    vis = iTemp;
    iTemp = 0;
    mNavigationBarController->ApplyTranslucentFlagLw(transWin, vis, oldVis, &iTemp);
    vis = iTemp;

    // prevent status bar interaction from clearing certain flags
    AutoPtr<IWindowManagerLayoutParams> tmp;
    win->GetAttrs((IWindowManagerLayoutParams**)&tmp);
    Int32 type;
    tmp->GetType(&type);
    Boolean statusBarHasFocus = type == IWindowManagerLayoutParams::TYPE_STATUS_BAR;
    if (statusBarHasFocus && !IsStatusBarKeyguard()) {
        int flags = IView::SYSTEM_UI_FLAG_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_IMMERSIVE
            | IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
        if (mHideLockScreen) {
            flags |= IView::STATUS_BAR_TRANSLUCENT | IView::NAVIGATION_BAR_TRANSLUCENT;
        }
        vis = (vis & ~flags) | (oldVis & flags);
    }

    if (!AreTranslucentBarsAllowed() && transWin != mStatusBar) {
        vis &= ~(IView::NAVIGATION_BAR_TRANSLUCENT | IView::STATUS_BAR_TRANSLUCENT
                | IView::SYSTEM_UI_TRANSPARENT);
    }

    // update status bar
    Boolean immersiveSticky = (vis & IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY) != 0;

    Boolean hideStatusBarWM = FALSE;
    if (mTopFullscreenOpaqueWindowState != NULL) {
        Int32 winFlags = WindowManagerPolicyControl::GetWindowFlags(mTopFullscreenOpaqueWindowState, NULL);
        hideStatusBarWM = (winFlags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0;
    }

    Boolean hideStatusBarSysui = (vis & IView::SYSTEM_UI_FLAG_FULLSCREEN) != 0;
    Boolean hideNavBarSysui = (vis & IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION) != 0;
    Boolean transientStatusBarAllowed = (mStatusBar != NULL) &&
        (hideStatusBarWM || (hideStatusBarSysui && immersiveSticky) || statusBarHasFocus);

    Boolean transientNavBarAllowed =
        mNavigationBar != NULL &&
        hideNavBarSysui && immersiveSticky;

    Boolean bTemp;
    Boolean denyTransientStatus = (mStatusBarController->IsTransientShowRequested(&bTemp), bTemp)
        && !transientStatusBarAllowed && hideStatusBarSysui;
    Boolean denyTransientNav = (mNavigationBarController->IsTransientShowRequested(&bTemp), bTemp)
        && !transientNavBarAllowed;
    if (denyTransientStatus || denyTransientNav) {
        // clear the clearable flags instead
        ClearClearableFlagsLw();
    }

    iTemp = 0;
    mStatusBarController->UpdateVisibilityLw(transientStatusBarAllowed, oldVis, vis, &iTemp);
    vis = iTemp;

    // update navigation bar
    Boolean oldImmersiveMode = IsImmersiveMode(oldVis);
    Boolean newImmersiveMode = IsImmersiveMode(vis);
    if (win != NULL && oldImmersiveMode != newImmersiveMode) {
        String pkg;
        win->GetOwningPackage(&pkg);
        mImmersiveModeConfirmation->ImmersiveModeChanged(pkg, newImmersiveMode,
                IsUserSetupComplete());
    }

    iTemp = 0;
    mNavigationBarController->UpdateVisibilityLw(transientNavBarAllowed, oldVis, vis, &iTemp);
    vis = iTemp;

    return vis;
}

void PhoneWindowManager::ClearClearableFlagsLw()
{
    Int32 newVal = mResettingSystemUiFlags | IView::SYSTEM_UI_CLEARABLE_FLAGS;
    if (newVal != mResettingSystemUiFlags) {
        mResettingSystemUiFlags = newVal;
        mWindowManagerFuncs->ReevaluateStatusBarVisibility();
    }
}

Boolean PhoneWindowManager::IsImmersiveMode(
    /* [in] */ Int32 vis)
{
    Int32 flags = IView::SYSTEM_UI_FLAG_IMMERSIVE | IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
    return mNavigationBar != NULL
        && (vis & IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION) != 0
        && (vis & flags) != 0
        && CanHideNavigationBar();
}

/**
 * @return whether the navigation or status bar can be made translucent
 *
 * This should return true unless touch exploration is not enabled or
 * R.boolean.config_enableTranslucentDecor is false.
 */
Boolean PhoneWindowManager::AreTranslucentBarsAllowed()
{
    Boolean bTemp;
    return mTranslucentDecorEnabled
        && !(mAccessibilityManager->IsTouchExplorationEnabled(&bTemp), bTemp);
}

Boolean PhoneWindowManager::ProcessKeyguardSetHiddenResultLw(
    /* [in] */ Int32 setHiddenResult)
{
    if (setHiddenResult
            == IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_SET_FLAGS) {
        AutoPtr<IWindowManagerLayoutParams> attrs;
        mStatusBar->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        attrs->SetPrivateFlags(privateFlags|IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD);
        Int32 flags;
        attrs->GetFlags(&flags);
        attrs->SetFlags(flags|IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
        return TRUE;
    } else if (setHiddenResult
            == IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_UNSET_FLAGS) {
        AutoPtr<IWindowManagerLayoutParams> attrs;
        mStatusBar->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        attrs->SetPrivateFlags(privateFlags & ~IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD);
        Int32 flags;
        attrs->GetFlags(&flags);
        attrs->SetFlags(flags | ~IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
        return TRUE;
    } else {
        return FALSE;
    }
}

Boolean PhoneWindowManager::IsStatusBarKeyguard()
{
    if (mStatusBar != NULL)
    {
        AutoPtr<IWindowManagerLayoutParams> attrs;
        mStatusBar->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        if (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD)
            return TRUE;
    }
    return FALSE;
}

ECode PhoneWindowManager::HasNavigationBar(
    /* [out] */ Boolean* has)
{
    assert(has != NULL);
    *has = mHasNavigationBar;
    return NOERROR;
}

ECode PhoneWindowManager::SetLastInputMethodWindowLw(
    /* [in] */ IWindowState* ime,
    /* [in] */ IWindowState* target)
{
    mLastInputMethodWindow = ime;
    mLastInputMethodTargetWindow = target;
    return NOERROR;
}

ECode PhoneWindowManager::HasPermanentMenuKey(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

/**
* Specifies whether the device needs a navigation bar (because it has no hardware buttons)
*/
ECode PhoneWindowManager::NeedsNavigationBar(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode PhoneWindowManager::CanMagnifyWindow(
    /* [in] */ Int32 windowType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (windowType) {
        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        case IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY: {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::IsTopLevelWindow(
    /* [in] */ Int32 windowType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (windowType >= IWindowManagerLayoutParams::FIRST_SUB_WINDOW
            && windowType <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
        *result = (windowType == IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG);
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode PhoneWindowManager::GetInputMethodWindowVisibleHeightLw(
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDockBottom - mCurBottom;
    return NOERROR;
}

ECode PhoneWindowManager::SetCurrentUserLw(
    /* [in] */ Int32 newUserId)
{
    mCurrentUserId = newUserId;
    if (mKeyguardDelegate != NULL) {
        mKeyguardDelegate->SetCurrentUser(newUserId);
    }
    if (mStatusBarService != NULL) {
        // try {
            mStatusBarService->SetCurrentUser(newUserId);
        // } catch (RemoteException e) {
        //     // oh well
        // }
    }

    return SetLastInputMethodWindowLw(NULL, NULL);
}

void PhoneWindowManager::Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args)
{
    pw->Print(prefix); pw->Print(String("mSafeMode=")); pw->Print(mSafeMode);
    pw->Print(String(" mSystemReady=")); pw->Print(mSystemReady);
    pw->Print(String(" mSystemBooted=")); pw->Print(mSystemBooted);
    pw->Print(prefix); pw->Print(String("mLidState=")); pw->Print(mLidState);
    pw->Print(String(" mLidOpenRotation=")); pw->Print(mLidOpenRotation);
    pw->Print(String(" mCameraLensCoverState=")); pw->Print(mCameraLensCoverState);
    pw->Print(String(" mHdmiPlugged=")); pw->Print(mHdmiPlugged);
    if (mLastSystemUiFlags != 0 || mResettingSystemUiFlags != 0
            || mForceClearedSystemUiFlags != 0) {
        pw->Print(prefix); pw->Print(String("mLastSystemUiFlags=0x"));
        pw->Print(StringUtils::ToHexString(mLastSystemUiFlags, FALSE));
        pw->Print(String(" mResettingSystemUiFlags=0x"));
        pw->Print(StringUtils::ToHexString(mResettingSystemUiFlags, FALSE));
        pw->Print(String(" mForceClearedSystemUiFlags=0x"));
        pw->Print(StringUtils::ToHexString(mForceClearedSystemUiFlags, FALSE));
    }
    if (mLastFocusNeedsMenu) {
        pw->Print(prefix); pw->Print(String("mLastFocusNeedsMenu="));
        pw->Println(mLastFocusNeedsMenu);
    }
    pw->Print(prefix); pw->Print(String("mWakeGestureEnabledSetting="));
    pw->Println(mWakeGestureEnabledSetting);

    pw->Print(prefix); pw->Print(String("mSupportAutoRotation=")); pw->Println(mSupportAutoRotation);

    pw->Print(prefix); pw->Print(String("mUiMode=")); pw->Print(mUiMode);
    pw->Print(String(" mDockMode=")); pw->Print(mDockMode);
    pw->Print(String(" mCarDockRotation=")); pw->Print(mCarDockRotation);
    pw->Print(String(" mDeskDockRotation=")); pw->Println(mDeskDockRotation);
    pw->Print(prefix); pw->Print(String("mUserRotationMode=")); pw->Print(mUserRotationMode);
    pw->Print(String(" mUserRotation=")); pw->Print(mUserRotation);
    pw->Print(String(" mAllowAllRotations=")); pw->Println(mAllowAllRotations);
    pw->Print(prefix); pw->Print(String("mCurrentAppOrientation=")); pw->Println(mCurrentAppOrientation);
    pw->Print(prefix); pw->Print(String("mCarDockEnablesAccelerometer="));
    pw->Print(mCarDockEnablesAccelerometer);
    pw->Print(String(" mDeskDockEnablesAccelerometer="));
    pw->Println(mDeskDockEnablesAccelerometer);
    pw->Print(prefix); pw->Print(String("mLidKeyboardAccessibility="));
    pw->Print(mLidKeyboardAccessibility);
    pw->Print(String(" mLidNavigationAccessibility=")); pw->Print(mLidNavigationAccessibility);
    pw->Print(String(" mLidControlsSleep=")); pw->Println(mLidControlsSleep);
    pw->Print(prefix);
                    pw->Print(String("mShortPressOnPowerBehavior=")); pw->Print(mShortPressOnPowerBehavior);
                    pw->Print(String(" mLongPressOnPowerBehavior=")); pw->Println(mLongPressOnPowerBehavior);
    pw->Print(prefix); pw->Print(String("mHasSoftInput=")); pw->Println(mHasSoftInput);
    pw->Print(prefix); pw->Print(String("mAwake=")); pw->Println(mAwake);
    pw->Print(prefix); pw->Print(String("mScreenOnEarly=")); pw->Print(mScreenOnEarly);
                       pw->Print(String(" mScreenOnFully=")); pw->Println(mScreenOnFully);
    pw->Print(prefix); pw->Print(String("mKeyguardDrawComplete=")); pw->Print(mKeyguardDrawComplete);
                       pw->Print(String(" mWindowManagerDrawComplete=")); pw->Println(mWindowManagerDrawComplete);
    pw->Print(prefix); pw->Print(String("mOrientationSensorEnabled="));
                       pw->Println(mOrientationSensorEnabled);
    pw->Print(prefix); pw->Print(String("mOverscanScreen=(")); pw->Print(mOverscanScreenLeft);
                       pw->Print(String(",")); pw->Print(mOverscanScreenTop);
                       pw->Print(String(") ")); pw->Print(mOverscanScreenWidth);
                       pw->Print(String("x")); pw->Println(mOverscanScreenHeight);
    if (mOverscanLeft != 0 || mOverscanTop != 0
          || mOverscanRight != 0 || mOverscanBottom != 0) {
        pw->Print(prefix); pw->Print(String("mOverscan left=")); pw->Print(mOverscanLeft);
        pw->Print(String(" top=")); pw->Print(mOverscanTop);
        pw->Print(String(" right=")); pw->Print(mOverscanRight);
        pw->Print(String(" bottom=")); pw->Println(mOverscanBottom);
    }
    pw->Print(prefix); pw->Print(String("mRestrictedOverscanScreen=("));
                      pw->Print(mRestrictedOverscanScreenLeft);
                      pw->Print(String(",")); pw->Print(mRestrictedOverscanScreenTop);
                      pw->Print(String(") ")); pw->Print(mRestrictedOverscanScreenWidth);
                      pw->Print(String("x")); pw->Println(mRestrictedOverscanScreenHeight);
    pw->Print(prefix); pw->Print(String("mUnrestrictedScreen=(")); pw->Print(mUnrestrictedScreenLeft);
    pw->Print(String(",")); pw->Print(mUnrestrictedScreenTop);
    pw->Print(String(") ")); pw->Print(mUnrestrictedScreenWidth);
    pw->Print(String("x")); pw->Println(mUnrestrictedScreenHeight);
    pw->Print(prefix); pw->Print(String("mRestrictedScreen=(")); pw->Print(mRestrictedScreenLeft);
    pw->Print(String(",")); pw->Print(mRestrictedScreenTop);
    pw->Print(String(") ")); pw->Print(mRestrictedScreenWidth);
    pw->Print(String("x")); pw->Println(mRestrictedScreenHeight);
    pw->Print(prefix); pw->Print(String("mStableFullscreen=(")); pw->Print(mStableFullscreenLeft);
    pw->Print(String(",")); pw->Print(mStableFullscreenTop);
    pw->Print(String(")-(")); pw->Print(mStableFullscreenRight);
    pw->Print(String(",")); pw->Print(mStableFullscreenBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mStable=(")); pw->Print(mStableLeft);
    pw->Print(String(",")); pw->Print(mStableTop);
    pw->Print(String(")-(")); pw->Print(mStableRight);
    pw->Print(String(",")); pw->Print(mStableBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mSystem=(")); pw->Print(mSystemLeft);
    pw->Print(String(",")); pw->Print(mSystemTop);
    pw->Print(String(")-(")); pw->Print(mSystemRight);
    pw->Print(String(",")); pw->Print(mSystemBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mCur=(")); pw->Print(mCurLeft);
    pw->Print(String(",")); pw->Print(mCurTop);
    pw->Print(String(")-(")); pw->Print(mCurRight);
    pw->Print(String(",")); pw->Print(mCurBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mContent=(")); pw->Print(mContentLeft);
    pw->Print(String(",")); pw->Print(mContentTop);
    pw->Print(String(")-(")); pw->Print(mContentRight);
    pw->Print(String(",")); pw->Print(mContentBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mVoiceContent=(")); pw->Print(mVoiceContentLeft);
                      pw->Print(String(",")); pw->Print(mVoiceContentTop);
                      pw->Print(String(")-(")); pw->Print(mVoiceContentRight);
                      pw->Print(String(",")); pw->Print(mVoiceContentBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mDock=(")); pw->Print(mDockLeft);
    pw->Print(String(",")); pw->Print(mDockTop);
    pw->Print(String(")-(")); pw->Print(mDockRight);
    pw->Print(String(",")); pw->Print(mDockBottom); pw->Println(String(")"));
    pw->Print(prefix); pw->Print(String("mDockLayer=")); pw->Print(mDockLayer);
    pw->Print(String(" mStatusBarLayer=")); pw->Println(mStatusBarLayer);
    pw->Print(prefix); pw->Print(String("mShowingLockscreen=")); pw->Print(mShowingLockscreen);
    pw->Print(String(" mShowingDream=")); pw->Print(mShowingDream);
    pw->Print(String(" mDreamingLockscreen=")); pw->Println(mDreamingLockscreen);
    if (mLastInputMethodWindow != NULL) {
        pw->Print(prefix); pw->Print(String("mLastInputMethodWindow="));
        //pw->PrintObjectln(mLastInputMethodWindow);
        pw->Print(TO_IINTERFACE(mLastInputMethodWindow));

    }
    if (mLastInputMethodTargetWindow != NULL) {
        pw->Print(prefix); pw->Print(String("mLastInputMethodTargetWindow="));
        //pw->PrintObjectln(mLastInputMethodTargetWindow);
        pw->Print(TO_IINTERFACE(mLastInputMethodTargetWindow));
    }
    if (mStatusBar != NULL) {
        pw->Print(prefix); pw->Print(String("mStatusBar="));
        //pw->PrintObjectln(mStatusBar);
        pw->Print(TO_IINTERFACE(mStatusBar));
        pw->Print(prefix); pw->Print(String("isStatusBarKeyguard="));
        pw->Print(IsStatusBarKeyguard());
    }
    if (mNavigationBar != NULL) {
        pw->Print(prefix); pw->Print(String("mNavigationBar="));
        //pw->PrintObjectln(mNavigationBar);
        pw->Print(TO_IINTERFACE(mNavigationBar));
    }
    if (mFocusedWindow != NULL) {
        pw->Print(prefix); pw->Print(String("mFocusedWindow="));
        //pw->PrintObjectln(mFocusedWindow);
        pw->Print(TO_IINTERFACE(mFocusedWindow));
    }
    if (mFocusedApp != NULL) {
        pw->Print(prefix); pw->Print(String("mFocusedApp="));
        //pw->PrintObjectln(mFocusedApp);
        pw->Print(TO_IINTERFACE(mFocusedApp));
    }
    if (mWinDismissingKeyguard != NULL) {
        pw->Print(prefix); pw->Print(String("mWinDismissingKeyguard="));
        //pw->PrintObjectln(mWinDismissingKeyguard);
        pw->Print(TO_IINTERFACE(mWinDismissingKeyguard));
    }
    if (mTopFullscreenOpaqueWindowState != NULL) {
        pw->Print(prefix); pw->Print(String("mTopFullscreenOpaqueWindowState="));
        //pw->PrintObjectln(mTopFullscreenOpaqueWindowState);
        pw->Print(TO_IINTERFACE(mTopFullscreenOpaqueWindowState));
    }
    if (mForcingShowNavBar) {
        pw->Print(prefix); pw->Print(String("mForcingShowNavBar="));
        pw->Println(mForcingShowNavBar); pw->Print(String("mForcingShowNavBarLayer="));
        pw->Println(mForcingShowNavBarLayer);
    }
    pw->Print(prefix); pw->Print(String("mTopIsFullscreen=")); pw->Print(mTopIsFullscreen);
    pw->Print(String(" mHideLockScreen=")); pw->Println(mHideLockScreen);
    pw->Print(prefix); pw->Print(String("mForceStatusBar=")); pw->Print(mForceStatusBar);
    pw->Print(String(" mForceStatusBarFromKeyguard="));
    pw->Println(mForceStatusBarFromKeyguard);
    pw->Print(prefix); pw->Print(String("mDismissKeyguard=")); pw->Print(mDismissKeyguard);
    pw->Print(String(" mWinDismissingKeyguard=")); pw->Print(TO_IINTERFACE(mWinDismissingKeyguard));//pw->PrintObject(mWinDismissingKeyguard);
    pw->Print(String(" mHomePressed=")); pw->Println(mHomePressed);
    pw->Print(prefix); pw->Print(String("mAllowLockscreenWhenOn=")); pw->Print(mAllowLockscreenWhenOn);
    pw->Print(String(" mLockScreenTimeout=")); pw->Print(mLockScreenTimeout);
    pw->Print(String(" mLockScreenTimerActive=")); pw->Println(mLockScreenTimerActive);
    pw->Print(prefix); pw->Print(String("mEndcallBehavior=")); pw->Print(mEndcallBehavior);
    pw->Print(String(" mIncallPowerBehavior=")); pw->Print(mIncallPowerBehavior);
    pw->Print(String(" mLongPressOnHomeBehavior=")); pw->Println(mLongPressOnHomeBehavior);
    pw->Print(prefix); pw->Print(String("mLandscapeRotation=")); pw->Print(mLandscapeRotation);
    pw->Print(String(" mSeascapeRotation=")); pw->Println(mSeascapeRotation);
    pw->Print(prefix); pw->Print(String("mPortraitRotation=")); pw->Print(mPortraitRotation);
    pw->Print(String(" mUpsideDownRotation=")); pw->Println(mUpsideDownRotation);

    pw->Print(prefix); pw->Print(String("mDemoHdmiRotation=")); pw->Print(mDemoHdmiRotation);
        pw->Print(String(" mDemoHdmiRotationLock=")); pw->Println(mDemoHdmiRotationLock);
    pw->Print(prefix); pw->Print(String("mUndockedHdmiRotation=")); pw->Println(mUndockedHdmiRotation);

    mGlobalKeyManager->Dump(prefix, pw);
    mStatusBarController->Dump(pw, prefix);
    mNavigationBarController->Dump(pw, prefix);
    // WindowManagerPolicyControl::Dump(prefix, pw);

    if (mWakeGestureListener != NULL) {
        mWakeGestureListener->Dump(pw, prefix);
    }
    if (mOrientationListener != NULL) {
        mOrientationListener->Dump(pw, prefix);
    }
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
