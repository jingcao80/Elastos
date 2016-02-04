
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/impl/CPhoneWindowManager.h"
#include "elastos/droid/impl/CPolicyManager.h"
#include "elastos/droid/impl/CRecentApplicationsDialog.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/R.h"
#include "elastos/droid/swextend/gpio/CGpio.h"
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/widget/CToastHelper.h"
#include "elastos/droid/widget/internal/CPointerLocationView.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <sys/reboot.h>

// #include "elastos/droid/provider/Settings.h"

using Elastos::Core::CString;
using Elastos::Core::StringBuffer;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Hardware::Input::IIInputManager;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Swextend::CGpio;
using Elastos::Droid::Swextend::IGpio;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IInputEventReceiverFactory;
using Elastos::Droid::View::IFallbackAction;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::Internal::CPointerLocationView;
using Elastos::Droid::Widget::IToastHelper;


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

String CPhoneWindowManager::SYSTEM_DIALOG_REASON_KEY = String("reason");
String CPhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS = String("globalactions");
String CPhoneWindowManager::SYSTEM_DIALOG_REASON_RECENT_APPS = String("recentapps");
String CPhoneWindowManager::SYSTEM_DIALOG_REASON_HOME_KEY = String("homekey");
String CPhoneWindowManager::SYSTEM_DIALOG_REASON_ASSIST = String("assist");

const Int32 CPhoneWindowManager::SYSTEM_UI_CHANGING_LAYOUT =
        IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION | IView::SYSTEM_UI_FLAG_FULLSCREEN;

AutoPtr<HashMap<Int32, AutoPtr<IInterface> > > CPhoneWindowManager::sApplicationLaunchKeyCategories =
        InitApplicationLaunchKeyCategories();

String CPhoneWindowManager::TAG("PhoneWindowManager");
const Boolean CPhoneWindowManager::DEBUG = FALSE;
const Boolean CPhoneWindowManager::localLOGV = FALSE;
const Boolean CPhoneWindowManager::DEBUG_LAYOUT = FALSE;
const Boolean CPhoneWindowManager::DEBUG_INPUT = FALSE;
const Boolean CPhoneWindowManager::DEBUG_BOOTFAST = FALSE;
const Boolean CPhoneWindowManager::DEBUG_STARTING_WINDOW = FALSE;
const Boolean CPhoneWindowManager::SHOW_STARTING_ANIMATIONS = TRUE;
const Boolean CPhoneWindowManager::SHOW_PROCESSES_ON_ALT_MENU = FALSE;

const Int32 CPhoneWindowManager::WINDOW_TYPE_TEST_MODE = 2099;
const Int32 CPhoneWindowManager::LONG_PRESS_POWER_NOTHING = 0;
const Int32 CPhoneWindowManager::LONG_PRESS_POWER_GLOBAL_ACTIONS = 1;
const Int32 CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF = 2;
const Int32 CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM = 3;
const Int32 CPhoneWindowManager::LONG_PRESS_HOME_NOTHING = 0;
const Int32 CPhoneWindowManager::LONG_PRESS_HOME_RECENT_DIALOG = 1;
const Int32 CPhoneWindowManager::LONG_PRESS_HOME_RECENT_SYSTEM_UI = 2;
const Int32 CPhoneWindowManager::APPLICATION_MEDIA_SUBLAYER = -2;
const Int32 CPhoneWindowManager::APPLICATION_MEDIA_OVERLAY_SUBLAYER = -1;
const Int32 CPhoneWindowManager::APPLICATION_PANEL_SUBLAYER = 1;
const Int32 CPhoneWindowManager::APPLICATION_SUB_PANEL_SUBLAYER = 2;
const Int32 CPhoneWindowManager::RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS = 0;
const Int32 CPhoneWindowManager::RECENT_APPS_BEHAVIOR_EXIT_TOUCH_MODE_AND_SHOW = 1;
const Int32 CPhoneWindowManager::RECENT_APPS_BEHAVIOR_DISMISS = 2;
const Int32 CPhoneWindowManager::RECENT_APPS_BEHAVIOR_DISMISS_AND_SWITCH = 3;

AutoPtr<CRect> CPhoneWindowManager::mTmpParentFrame;
AutoPtr<CRect> CPhoneWindowManager::mTmpDisplayFrame;
AutoPtr<CRect> CPhoneWindowManager::mTmpContentFrame;
AutoPtr<CRect> CPhoneWindowManager::mTmpVisibleFrame;
AutoPtr<CRect> CPhoneWindowManager::mTmpNavigationFrame;
const Int32 CPhoneWindowManager::DISMISS_KEYGUARD_NONE = 0;
const Int32 CPhoneWindowManager::DISMISS_KEYGUARD_START = 1;
const Int32 CPhoneWindowManager::DISMISS_KEYGUARD_CONTINUE = 2;
const Int64 CPhoneWindowManager::SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS = 150;
const Float CPhoneWindowManager::KEYGUARD_SCREENSHOT_CHORD_DELAY_MULTIPLIER = 2.5f;
const Int32 CPhoneWindowManager::MSG_ENABLE_POINTER_LOCATION = 1;
const Int32 CPhoneWindowManager::MSG_DISABLE_POINTER_LOCATION = 2;
const Int32 CPhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK = 3;
const Int32 CPhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK = 4;
const Int32 CPhoneWindowManager::MSG_SHOW_BATTERY_CHARGE = 5;
const Int32 CPhoneWindowManager::MSG_SHOW_BOOT_INIT = 6;

const Boolean CPhoneWindowManager::PRINT_ANIM = FALSE;
AutoPtr<ArrayOf<Int32> > CPhoneWindowManager::WINDOW_TYPES_WHERE_HOME_DOESNT_WORK = InitWINDOW_TYPES_WHERE_HOME_DOESNT_WORK();

Boolean CPhoneWindowManager::mVolumeMute = FALSE;
Int32 CPhoneWindowManager::mPreVolume = 0;

Boolean CPhoneWindowManager::mFullScreenIsEnable = TRUE;
Boolean CPhoneWindowManager::mAlwaysFullScreen = FALSE;

const Int32 CPhoneWindowManager::NF_ID_ENTER_KEY_MOUSE_MODE = 1;

const Int32 CPhoneWindowManager::FLICKER_INTERVAL = 30;
const Char32 CPhoneWindowManager::PORT_TYPE = 'h';
const Int32 CPhoneWindowManager::PORT_NUM = 20;
const Int32 CPhoneWindowManager::ON = 1;
const Int32 CPhoneWindowManager::OFF = 0;


//==============================================================================
//          ++ CPhoneWindowManager::PromptEnterMouseModeRunnable ++
//==============================================================================

CPhoneWindowManager::PromptEnterMouseModeRunnable::PromptEnterMouseModeRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::PromptEnterMouseModeRunnable::Run()
{
    if (mHost->mMouseToast == NULL) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        helper->MakeText(mHost->mContext, R::string::enter_key_mouse_mode,
                IToast::LENGTH_SHORT, (IToast**)&mHost->mMouseToast);
        if (mHost->mMouseToast == NULL) {
            // Logger::W(TAG, "Fail in creating toast.");
        }
        else {
            mHost->mMouseToast->SetGravity(IGravity::CENTER, 0, 0);
        }
    }
    mHost->mMouseToast->SetText(R::string::enter_key_mouse_mode);
    mHost->mMouseToast->Show();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::PromptEnterMouseModeRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::PromptExitMouseModeRunnable ++
//==============================================================================

CPhoneWindowManager::PromptExitMouseModeRunnable::PromptExitMouseModeRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::PromptExitMouseModeRunnable::Run()
{
    if (mHost->mMouseToast != NULL) {
        mHost->mMouseToast->SetText(R::string::exit_key_mouse_mode);
        mHost->mMouseToast->Show();
    }

    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::PromptExitMouseModeRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::SettingsObserver ++
//==============================================================================

CPhoneWindowManager::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CPhoneWindowManager* host)
    : ContentObserver(handler)
    , mHost(host)
{
}

ECode CPhoneWindowManager::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateSettings();
    mHost->UpdateRotation(FALSE);
    return NOERROR;
}

void CPhoneWindowManager::SettingsObserver::Observe()
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
    Settings::System::GetUriFor(String("fancy_rotation_anim"), (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    uri = NULL;
    Settings::System::GetUriFor(ISettingsSystem::BOOT_FAST_ENABLE, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);

    mHost->UpdateSettings();
}

//==============================================================================
//          -- CPhoneWindowManager::SettingsObserver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::MyOrientationListener ++
//==============================================================================

CPhoneWindowManager::MyOrientationListener::MyOrientationListener(
    /* [in] */ IContext* context,
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::MyOrientationListener::OnProposedRotationChanged(
    /* [in] */ Int32 rotation)
{
    // if (localLOGV) Log.v(TAG, "onProposedRotationChanged, rotation=" + rotation);
    mHost->UpdateRotation(FALSE);
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::MyOrientationListener --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::HideNavInputEventReceiver ++
//==============================================================================

CPhoneWindowManager::HideNavInputEventReceiver::HideNavInputEventReceiver(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ CPhoneWindowManager* host)
    : InputEventReceiver(inputChannel, looper)
    , mHost(host)
{
}

ECode CPhoneWindowManager::HideNavInputEventReceiver::OnInputEvent(
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
                AutoLock lock(mHost->mLock);
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
                    AutoPtr<IRunnable> reevaluateRunnable = new ReevaluateRunnable(mHost);
                    Boolean isSuccess = FALSE;
                    mHost->mHandler->PostDelayed(reevaluateRunnable, 1000, &isSuccess);
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
//          -- CPhoneWindowManager::HideNavInputEventReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::HideNavInputEventReceiverFactory ++
//==============================================================================

CPhoneWindowManager::HideNavInputEventReceiverFactory::HideNavInputEventReceiverFactory(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(CPhoneWindowManager::HideNavInputEventReceiverFactory, IInputEventReceiverFactory)

ECode CPhoneWindowManager::HideNavInputEventReceiverFactory::CreateInputEventReceiver(
   /* [in] */ IInputChannel* inputChannel,
   /* [in] */ ILooper* looper,
   /* [out] */ IInputEventReceiver** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    *receiver = new HideNavInputEventReceiver(inputChannel, looper, mHost);
    REFCOUNT_ADD(*receiver);
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::HideNavInputEventReceiverFactory --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ScreenshotTimeoutRunnable ++
//==============================================================================

CPhoneWindowManager::ScreenshotTimeoutRunnable::ScreenshotTimeoutRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ScreenshotTimeoutRunnable::Run()
{
    AutoLock lock(mHost->mScreenshotLock);
    if (mHost->mScreenshotConnection != NULL) {
        mHost->mContext->UnbindService(mHost->mScreenshotConnection);
        mHost->mScreenshotConnection = NULL;
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ScreenshotTimeoutRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::DockBroadReceiver ++
//==============================================================================

CPhoneWindowManager::DockBroadReceiver::DockBroadReceiver(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::DockBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (IIntent::ACTION_DOCK_EVENT.Equals((intent->GetAction(&action), action))) {
        Int32 value = 0;
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &value);
        mHost->mDockMode = value;
    }
    mHost->UpdateRotation(TRUE);
    mHost->UpdateOrientationListenerLp();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::DockBroadReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::DreamBroadReceiver ++
//==============================================================================

CPhoneWindowManager::DreamBroadReceiver::DreamBroadReceiver(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::DreamBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (IIntent::ACTION_DREAMING_STARTED.Equals((intent->GetAction(&action), action))) {
        // TODO: KeyguardMediator is not implement.
        // if (mHost->mKeyguardMediator != NULL) {
        //     mHost->mKeyguardMediator->OnDreamingStarted();
        // }
    } else if (IIntent::ACTION_DREAMING_STOPPED.Equals((intent->GetAction(&action), action))) {
        // if (mHost->mKeyguardMediator != NULL) {
        //     mHost->mKeyguardMediator->OnDreamingStopped();
        // }
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::DreamBroadReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::MultiuserBroadReceiver ++
//==============================================================================

CPhoneWindowManager::MultiuserBroadReceiver::MultiuserBroadReceiver(
    /*in*/ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::MultiuserBroadReceiver::OnReceive(
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
        AutoLock lock(mHost->mLock);
        mHost->mLastSystemUiFlags = 0;
        mHost->UpdateSystemUiVisibilityLw();
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::MultiuserBroadReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ScreenLockTimeoutRunnable ++
//==============================================================================

CPhoneWindowManager::ScreenLockTimeoutRunnable::ScreenLockTimeoutRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ScreenLockTimeoutRunnable::Run()
{
    AutoLock lock(this);
    // if (localLOGV) Log.v(TAG, "mScreenLockTimeout activating keyguard");
    // TODO : KeyguardMediator is not implement.
    // if (mHost->mKeyguardMediator != NULL) {
    //     mHost->mKeyguardMediator->DoKeyguardTimeout(options);
    // }
    mHost->mLockScreenTimerActive = FALSE;
    mOptions = NULL;
    return NOERROR;
}

void CPhoneWindowManager::ScreenLockTimeoutRunnable::SetLockOptions(
    /* [in] */ IBundle* options)
{
    mOptions = options;
}

//==============================================================================
//          -- CPhoneWindowManager::ScreenLockTimeoutRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::PointerLocationInputEventReceiver ++
//==============================================================================

CPhoneWindowManager::PointerLocationInputEventReceiver::PointerLocationInputEventReceiver(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ IPointerLocationView* view)
    : InputEventReceiver(inputChannel, looper)
    , mView(view)
{
}

ECode CPhoneWindowManager::PointerLocationInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    Boolean handled = FALSE;
    // try {
    Int32 source = 0;
    if (IMotionEvent::Probe(event)
            && ((event->GetSource(&source), source) & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        const AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        mView->AddPointerEvent(motionEvent);
        handled = TRUE;
    }
    // } finally {
    FinishInputEvent(event, handled);
    // }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::PointerLocationInputEventReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::PolicyHandler ++
//==============================================================================

CPhoneWindowManager::PolicyHandler::PolicyHandler(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::PolicyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case CPhoneWindowManager::MSG_ENABLE_POINTER_LOCATION:
            mHost->EnablePointerLocation();
            break;
        case CPhoneWindowManager::MSG_DISABLE_POINTER_LOCATION:
            mHost->DisablePointerLocation();
            break;
        case CPhoneWindowManager::MSG_SHOW_BATTERY_CHARGE:
            if (mHost->mBootAnimationView != NULL) {
                Int32 arg1 = 0;
                msg->GetArg1(&arg1);
                mHost->mBootAnimationView->StartShowBatteryCharge(arg1);
            }
            break;
        case CPhoneWindowManager::MSG_SHOW_BOOT_INIT:
            if (mHost->mBootAnimationView != NULL) {
                mHost->mBootAnimationView->ShowBootInitLogo(0);
            }
            break;
        case CPhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(obj);
                mHost->DispatchMediaKeyWithWakeLock(keyEvent);
            }
            break;
        case CPhoneWindowManager::MSG_DISPATCH_MEDIA_KEY_REPEAT_WITH_WAKE_LOCK:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(obj);
                mHost->DispatchMediaKeyRepeatWithWakeLock(keyEvent);
            }
            break;
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::PolicyHandler --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::HDMIUEventObserver ++
//==============================================================================

CPhoneWindowManager::HDMIUEventObserver::HDMIUEventObserver(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::HDMIUEventObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    mHost->SetHdmiPlugged(event->Get(String("SWITCH_STATE")).Equals("1"));
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::HDMIUEventObserver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::FlickerIntentBroadcastReceiver ++
//==============================================================================

CPhoneWindowManager::FlickerIntentBroadcastReceiver::FlickerIntentBroadcastReceiver(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::FlickerIntentBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<IGpio> gpio;
    CGpio::AcquireSingleton((IGpio**)&gpio);
    Int32 result = 0;
    Slogger::D(TAG, "action = %s", action.string());
    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mHost->mFlickerEnable = TRUE;
        gpio->WriteGpio(mHost->PORT_TYPE, mHost->PORT_NUM, mHost->ON, &result);
    } else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mHost->mFlickerEnable = FALSE;

        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep1On);
        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep2Off);
        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep3On);
        gpio->WriteGpio(mHost->PORT_TYPE, mHost->PORT_NUM, mHost->OFF, &result);
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::FlickerIntentBroadcastReceiver --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::Step1OnRunnable ++
//==============================================================================

CPhoneWindowManager::Step1OnRunnable::Step1OnRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::Step1OnRunnable::Run()
{
    if (mHost->mFlickerEnable) {
        AutoPtr<IGpio> gpio;
        CGpio::AcquireSingleton((IGpio**)&gpio);
        Int32 result = 0;
        gpio->WriteGpio(mHost->PORT_TYPE, mHost->PORT_NUM, mHost->ON, &result);

        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep2Off);
        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep3On);

        Boolean isSuccess = FALSE;
        mHost->mFlickerHandler->PostDelayed(mHost->mStep2Off,
                CPhoneWindowManager::FLICKER_INTERVAL, &isSuccess);
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::Step1OnRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::Step2OffRunnable ++
//==============================================================================

CPhoneWindowManager::Step2OffRunnable::Step2OffRunnable(
/* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::Step2OffRunnable::Run()
{
    if (mHost->mFlickerEnable) {
        AutoPtr<IGpio> gpio;
        CGpio::AcquireSingleton((IGpio**)&gpio);
        Int32 result = 0;
        gpio->WriteGpio(mHost->PORT_TYPE, mHost->PORT_NUM, mHost->OFF, &result);

        mHost->mFlickerHandler->RemoveCallbacks(mHost->mStep3On);

        Boolean isSuccess = FALSE;
        mHost->mFlickerHandler->PostDelayed(mHost->mStep3On,
                CPhoneWindowManager::FLICKER_INTERVAL, &isSuccess);
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::Step2OffRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::Step3OnRunnable ++
//==============================================================================

CPhoneWindowManager::Step3OnRunnable::Step3OnRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::Step3OnRunnable::Run()
{
    if (mHost->mFlickerEnable) {
        AutoPtr<IGpio> gpio;
        CGpio::AcquireSingleton((IGpio**)&gpio);
        Int32 result = 0;
        gpio->WriteGpio(mHost->PORT_TYPE, mHost->PORT_NUM, mHost->ON, &result);
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::Step3OnRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::BootFastPowerLongPressRunnable ++
//==============================================================================

CPhoneWindowManager::BootFastPowerLongPressRunnable::BootFastPowerLongPressRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::BootFastPowerLongPressRunnable::Run()
{
    mHost->mBootFastRuning = TRUE;
    mHost->mPowerBootKeyHandled = TRUE;
    Slogger::V(TAG, "mBootFastPowerLongPress Runnable");
    mHost->HideScreen(TRUE);
    mHost->mPowerManager->BootFastWake(SystemClock::GetUptimeMillis());

    // showBootAnimation(3000);
    mHost->HideScreen(FALSE);
    // releaseBootAnimationView();
    mHost->mBootAnimationView->ShowBootInitLogo(0);

    Boolean isSuccess = FALSE;
    AutoPtr<ShowInitLogoRunnable> showInitLogoRunnable = new ShowInitLogoRunnable(mHost);
    mHost->mHandler->PostDelayed(showInitLogoRunnable, 1200, &isSuccess);

    AutoPtr<HideScreenRunnable> hideScreenRunnable = new HideScreenRunnable(mHost);
    mHost->mHandler->PostDelayed(hideScreenRunnable, 1500, &isSuccess);

    AutoPtr<HideBootAnimationRunnable> hideBootAnimationRunnable =
            new HideBootAnimationRunnable(mHost);
    mHost->mHandler->PostDelayed(hideBootAnimationRunnable, 5000, &isSuccess);
    // try {
    ActivityManagerNative::GetDefault()->SendBootFastComplete();
    // } catch (RemoteException e) {
    // }
    // setAirplaneModeState(false);
    mHost->RestoreAirplaneModeState();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::BootFastPowerLongPressRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::PowerLongPressRunnable ++
//==============================================================================

CPhoneWindowManager::PowerLongPressRunnable::PowerLongPressRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::PowerLongPressRunnable::Run()
{
    // The context isn't read
    mHost->mBootFastRuning = TRUE;
    if (mHost->mLongPressOnPowerBehavior < 0) {
        AutoPtr<IResources> resources;
        mHost->mContext->GetResources((IResources**)&resources);
        resources->GetInteger(R::integer::config_longPressOnPowerBehavior,
                &mHost->mLongPressOnPowerBehavior);
    }
    Int32 resolvedBehavior = mHost->mLongPressOnPowerBehavior;
    // Boolean isEnable = FALSE;
    // if (FactoryTest::IsLongPressOnPowerOffEnabled(&isEnable), isEnable) {
    //     resolvedBehavior = CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM;
    // }
    switch (resolvedBehavior) {
        case CPhoneWindowManager::LONG_PRESS_POWER_NOTHING:
            break;
        case CPhoneWindowManager::LONG_PRESS_POWER_GLOBAL_ACTIONS:
            {
                mHost->mPowerKeyHandled = TRUE;
                Boolean value = FALSE;
                if (!(mHost->PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &value), value)) {
                    mHost->PerformAuditoryFeedbackForAccessibilityIfNeed();
                }
                mHost->SendCloseSystemWindows(CPhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS);
                /* modified by Gary. start {{----------------------------------- */
                /* 2013-03-16 */
                /* directly power off when to shut down */
                {
                    Boolean directlyPowerOff = SystemProperties::GetBoolean(String("ro.sw.directlypoweroff"), FALSE);
                    Slogger::D(TAG, "LONG_PRESS_POWER_GLOBAL_ACTIONS() :directlyPowerOff = %d", directlyPowerOff);
                    if (directlyPowerOff) {
                        mHost->mWindowManagerFuncs->Shutdown(FALSE);
                    } else {
                        mHost->ShowGlobalActionsDialog();
                    }
                }
                /* modified by Gary. end   -----------------------------------}} */
            }
            break;
        case CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF:
        case CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF_NO_CONFIRM:
            {
                mHost->mPowerKeyHandled = TRUE;
                Boolean value = FALSE;
                mHost->PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &value);
                mHost->SendCloseSystemWindows(CPhoneWindowManager::SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS);
                /* modified by Gary. start {{----------------------------------- */
                /* 2013-03-16 */
                /* directly power off when to shut down */
                //boolean directlyPowerOff = SystemProperties.getBoolean("ro.sw.directlypoweroff", true);
                {
                    Boolean directlyPowerOff = SystemProperties::GetBoolean(String("ro.sw.directlypoweroff"), FALSE);
                    Slogger::D(TAG, "LONG_PRESS_POWER_SHUT_OFF() :directlyPowerOff = %d", directlyPowerOff);
                    if (directlyPowerOff) {
                        mHost->mWindowManagerFuncs->Shutdown(FALSE);
                    } else {
                        mHost->mWindowManagerFuncs->Shutdown(resolvedBehavior == CPhoneWindowManager::LONG_PRESS_POWER_SHUT_OFF);
                    }
                }
                /* modified by Gary. end   -----------------------------------}} */
            }
            break;
    }
    if (DEBUG_BOOTFAST) {
        Slogger::D(TAG, "shutdown finish out");
    }
    mHost->mBootFastRuning = FALSE;
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::PowerLongPressRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ScreenshotChordLongPressRunnable ++
//==============================================================================

CPhoneWindowManager::ScreenshotChordLongPressRunnable::ScreenshotChordLongPressRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ScreenshotChordLongPressRunnable::Run()
{
    mHost->TakeScreenshot();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ScreenshotChordLongPressRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ShowInitLogoRunnable ++
//==============================================================================

CPhoneWindowManager::ShowInitLogoRunnable::ShowInitLogoRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ShowInitLogoRunnable::Run()
{
    mHost->mBootAnimationView->ShowInitLogo();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ShowInitLogoRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::HideScreenRunnable ++
//==============================================================================

CPhoneWindowManager::HideScreenRunnable::HideScreenRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::HideScreenRunnable::Run()
{
    mHost->mBootAnimationView->HideScreen(TRUE);
    mHost->ShowBootAnimation();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::HideScreenRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::HideBootAnimationRunnable ++
//==============================================================================

CPhoneWindowManager::HideBootAnimationRunnable::HideBootAnimationRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::HideBootAnimationRunnable::Run()
{
    mHost->ReleaseBootAnimationView();
    mHost->HideBootAnimation();
    if (mHost->mWindowManager != NULL) {
        // try {
        mHost->mWindowManager->ThawRotation();
        mHost->mWindowManager->SetEventDispatching(TRUE);
        // }catch (RemoteException e) {
        // }
        mHost->mBootFastRuning = FALSE;
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::HideBootAnimationRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ShowOrHideRecentAppsDialogRunnable ++
//==============================================================================

CPhoneWindowManager::ShowOrHideRecentAppsDialogRunnable::ShowOrHideRecentAppsDialogRunnable(
    /* [in] */ CPhoneWindowManager* host,
    /* [in] */ Int32 behavior)
    : mHost(host)
    , mBehavior(behavior)
{
}

ECode CPhoneWindowManager::ShowOrHideRecentAppsDialogRunnable::Run()
{
    if (mHost->mRecentAppsDialog == NULL) {
        CRecentApplicationsDialog::New(mHost->mContext,
            (IRecentApplicationsDialog**)&mHost->mRecentAppsDialog);
        assert(mHost->mRecentAppsDialog != NULL);
    }

    Boolean isShowing;
    mHost->mRecentAppsDialog->IsShowing(&isShowing);
    if (isShowing) {
        switch (mBehavior) {
            case RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS:
            case RECENT_APPS_BEHAVIOR_DISMISS:
                mHost->mRecentAppsDialog->Dismiss();
                break;
            case RECENT_APPS_BEHAVIOR_DISMISS_AND_SWITCH:
                mHost->mRecentAppsDialog->DismissAndSwitch();
                break;
            case RECENT_APPS_BEHAVIOR_EXIT_TOUCH_MODE_AND_SHOW:
            default:
                break;
        }
    }
    else {
        switch (mBehavior) {
            case RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS:
                mHost->mRecentAppsDialog->Show();
                break;
            case RECENT_APPS_BEHAVIOR_EXIT_TOUCH_MODE_AND_SHOW:
                // try {
                mHost->mWindowManager->SetInTouchMode(FALSE);
                // } catch (RemoteException e) {
                // }
                mHost->mRecentAppsDialog->Show();
                break;
            case RECENT_APPS_BEHAVIOR_DISMISS:
            case RECENT_APPS_BEHAVIOR_DISMISS_AND_SWITCH:
            default:
                break;
        }
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ShowOrHideRecentAppsDialogRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ReevaluateRunnable ++
//==============================================================================

CPhoneWindowManager::ReevaluateRunnable::ReevaluateRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ReevaluateRunnable::Run()
{
    {
        AutoLock lock(mHost->mLock);
        // Clear flags.
        mHost->mForceClearedSystemUiFlags &=
                ~IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION;
    }
    mHost->mWindowManagerFuncs->ReevaluateStatusBarVisibility();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ReevaluateRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::CollapsePanelsRunnable ++
//==============================================================================

CPhoneWindowManager::CollapsePanelsRunnable::CollapsePanelsRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::CollapsePanelsRunnable::Run()
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
//          -- CPhoneWindowManager::CollapsePanelsRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ScreenshotHandler ++
//==============================================================================

CPhoneWindowManager::ScreenshotHandler::ScreenshotHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ IServiceConnection *conn,
    /* [in] */ CPhoneWindowManager* host)
    : myConn(conn)
    , mHost(host)
{
}

ECode CPhoneWindowManager::ScreenshotHandler::HandleMessage(
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
//          -- CPhoneWindowManager::ScreenshotHandler --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ScreenshotServiceConnection ++
//==============================================================================

CPhoneWindowManager::ScreenshotServiceConnection::ScreenshotServiceConnection(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(CPhoneWindowManager::ScreenshotServiceConnection, IServiceConnection)

ECode CPhoneWindowManager::ScreenshotServiceConnection::OnServiceConnected(
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
    AutoPtr<ScreenshotHandler> h = new ScreenshotHandler(looper, myConn, mHost);
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

ECode CPhoneWindowManager::ScreenshotServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ScreenshotServiceConnection --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::AcquireBootRunnable ++
//==============================================================================

CPhoneWindowManager::AcquireBootRunnable::AcquireBootRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::AcquireBootRunnable::Run()
{
    mHost->AcquireBootAnimationView();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::AcquireBootRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::ReleaseBootAVRunnable ++
//==============================================================================

CPhoneWindowManager::ReleaseBootAVRunnable::ReleaseBootAVRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::ReleaseBootAVRunnable::Run()
{
    mHost->ReleaseBootAnimationView();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::ReleaseBootAVRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::UpdateSettingRunnable ++
//==============================================================================

CPhoneWindowManager::UpdateSettingRunnable::UpdateSettingRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::UpdateSettingRunnable::Run()
{
    mHost->UpdateSettings();
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::UpdateSettingRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::BootMsgDialog ++
//==============================================================================

ECode CPhoneWindowManager::_BootMsgDialog::Init(
    /* [in] */ IContext* context)
{
    return ProgressDialog::Init(context);
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return TRUE;
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event)
{
    return TRUE;
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    return TRUE;
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* ev)
{
    return TRUE;
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* ev)
{
    return TRUE;
}

Boolean CPhoneWindowManager::_BootMsgDialog::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    return TRUE;
}

IPROGRESSDIALOG_METHODS_IMPL(CPhoneWindowManager::BootMsgDialog, CPhoneWindowManager::_BootMsgDialog);
IALERTDIALOG_METHODS_IMPL(CPhoneWindowManager::BootMsgDialog, CPhoneWindowManager::_BootMsgDialog);
IDIALOG_METHODS_IMPL(CPhoneWindowManager::BootMsgDialog, CPhoneWindowManager::_BootMsgDialog);
IWINDOWCALLBACK_METHODS_IMPL(CPhoneWindowManager::BootMsgDialog, CPhoneWindowManager::_BootMsgDialog);
IKEYEVENTCALLBACK_METHODS_IMPL(CPhoneWindowManager::BootMsgDialog, CPhoneWindowManager::_BootMsgDialog);

PInterface CPhoneWindowManager::BootMsgDialog::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IProgressDialog*)this;
    }
    else if (Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener == riid) {
        return (IViewOnCreateContextMenuListener*)this;
    }
    else if (Elastos::Droid::View::EIID_IWindowCallback == riid) {
        return (IWindowCallback*)this;
    }
    else if (Elastos::Droid::Content::EIID_IDialogInterface == riid) {
        return (IDialogInterface*)(IProgressDialog*)this;
    }
    else if (EIID_IDialog == riid) {
        return (IDialog*)(IProgressDialog*)this;
    }
    else if (EIID_IAlertDialog == riid) {
        return (IAlertDialog*)(IProgressDialog*)this;
    }
    else if (EIID_IProgressDialog == riid) {
        return (IProgressDialog*)this;
    }
    else if (Elastos::Droid::View::EIID_IKeyEventCallback == riid) {
        return (IKeyEventCallback*)this;
    }

    return NULL;
}

UInt32 CPhoneWindowManager::BootMsgDialog::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CPhoneWindowManager::BootMsgDialog::Release()
{
    return ElRefBase::Release();
}

ECode CPhoneWindowManager::BootMsgDialog::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindowManager::BootMsgDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return _BootMsgDialog::OnCreateContextMenu(menu, v, menuInfo);
}

//==============================================================================
//          -- CPhoneWindowManager::BootMsgDialog --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::BootMsgRunnable ++
//==============================================================================

CPhoneWindowManager::BootMsgRunnable::BootMsgRunnable(
    /* [in] */ CPhoneWindowManager* host,
    /* [in] */ ICharSequence* msg)
    : mHost(host)
    , mMsg(msg)
{
}

ECode CPhoneWindowManager::BootMsgRunnable::Run()
{
    if (mHost->mBootMsgDialog == NULL) {
        mHost->mBootMsgDialog = new BootMsgDialog();
        mHost->mBootMsgDialog->Init(mHost->mContext);
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
        mHost->mBootMsgDialog->Show();
    }
    mHost->mBootMsgDialog->SetMessage(mMsg);
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::BootMsgRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::BootMsgDismissRunnable ++
//==============================================================================

CPhoneWindowManager::BootMsgDismissRunnable::BootMsgDismissRunnable(
    /* [in] */ CPhoneWindowManager* host)
    : mHost(host)
{
}

ECode CPhoneWindowManager::BootMsgDismissRunnable::Run()
{
    if (mHost->mBootMsgDialog != NULL) {
        mHost->mBootMsgDialog->Dismiss();
        mHost->mBootMsgDialog = NULL;
    }
    return NOERROR;
}

//==============================================================================
//          -- CPhoneWindowManager::BootMsgDismissRunnable --
//==============================================================================


//==============================================================================
//          ++ CPhoneWindowManager::UpdateSystemUiVisibilityRunnable ++
//==============================================================================

CPhoneWindowManager::UpdateSystemUiVisibilityRunnable::UpdateSystemUiVisibilityRunnable(
    /* [in] */ Int32 visibility,
    /* [in] */ Boolean needsMenu,
    /* [in] */ CPhoneWindowManager* host)
    : mVisibility(visibility)
    , mNeedsMenu(needsMenu)
    , mHost(host)
{
}

ECode CPhoneWindowManager::UpdateSystemUiVisibilityRunnable::Run()
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
//          -- CPhoneWindowManager::UpdateSystemUiVisibilityRunnable --
//==============================================================================


CPhoneWindowManager::CPhoneWindowManager()
    : mEnableShiftMenuBugReports(FALSE)
    , mHeadless(FALSE)
    , mSafeMode(FALSE)
    , mHasSystemNavBar(FALSE)
    , mStatusBarHeight(0)
    , mHasNavigationBar(FALSE)
    , mCanHideNavigationBar(FALSE)
    , mNavigationBarCanMove(FALSE)
    , mNavigationBarOnBottom(FALSE)
    , mPowerKeyHandled(FALSE)
    , mPowerBootKeyHandled(FALSE)
    , mPendingPowerKeyUpCanceled(FALSE)
    , mRecentAppsDialogHeldModifiers(0)
    , mLanguageSwitchKeyPressed(FALSE)
    , mLidState(IWindowManagerPolicyWindowManagerFuncs::LID_ABSENT)
    , mHaveBuiltInKeyboard(FALSE)
    , mSystemReady(FALSE)
    , mSystemBooted(FALSE)
    , mHdmiPlugged(FALSE)
    , mDockMode(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mLidOpenRotation(0)
    , mCarDockRotation(0)
    , mDeskDockRotation(0)
    , mHdmiRotation(0)
    , mHdmiRotationLock(FALSE)
    , mUserRotationMode(IWindowManagerPolicy::USER_ROTATION_FREE)
    , mUserRotation(ISurface::ROTATION_0)
    , mAccelerometerDefault(FALSE)
    , mBootFastEnable(FALSE)
    , mAllowAllRotations(-1)
    , mCarDockEnablesAccelerometer(FALSE)
    , mDeskDockEnablesAccelerometer(FALSE)
    , mLidKeyboardAccessibility(0)
    , mLidNavigationAccessibility(0)
    , mLidControlsSleep(FALSE)
    , mLongPressOnPowerBehavior(-1)
    , mScreenOnEarly(FALSE)
    , mScreenOnFully(FALSE)
    , mOrientationSensorEnabled(FALSE)
    , mCurrentAppOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED)
    , mHasSoftInput(FALSE)
    , mPointerLocationMode(0)
    , mUnrestrictedScreenLeft(0)
    , mUnrestrictedScreenTop(0)
    , mUnrestrictedScreenWidth(0)
    , mUnrestrictedScreenHeight(0)
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
    , mHomeLongPressed(FALSE)
    , mSearchKeyShortcutPending(FALSE)
    , mConsumeSearchKeyUp(FALSE)
    , mAssistKeyLongPressed(FALSE)
    , mAllowLockscreenWhenOn(FALSE)
    , mLockScreenTimeout(0)
    , mLockScreenTimerActive(FALSE)
    , mEndcallBehavior(0)
    , mIncallPowerBehavior(0)
    , mLandscapeRotation(0)
    , mSeascapeRotation(0)
    , mPortraitRotation(0)
    , mUpsideDownRotation(0)
    , mHavePendingMediaKeyRepeatWithWakeLock(FALSE)
    , mIsExpanded(FALSE)
    , mKeyEnterMouseMode(FALSE)
    , mLongPressOnHomeBehavior(-1)
    , mScreenshotChordEnabled(FALSE)
    , mVolumeDownKeyTriggered(FALSE)
    , mVolumeDownKeyTime(0)
    , mVolumeDownKeyConsumedByScreenshotChord(FALSE)
    , mVolumeUpKeyTriggered(FALSE)
    , mPowerKeyTriggered(FALSE)
    , mPowerKeyTime(0)
    , mBootFastRuning(FALSE)
    , mLeftBtn(-1)
    , mMidBtn(-1)
    , mRightBtn(-1)
    , mLeft(-1)
    , mRight(-1)
    , mTop(-1)
    , mBottom(-1)
    , mFlickerEnable(TRUE)
{
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTmpParentFrame));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTmpDisplayFrame));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTmpContentFrame));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTmpVisibleFrame));
    ASSERT_SUCCEEDED(CRect::NewByFriend((CRect**)&mTmpNavigationFrame));

    memset(mNavigationBarHeightForRotation, 0, sizeof(mNavigationBarHeightForRotation));
    memset(mNavigationBarWidthForRotation, 0, sizeof(mNavigationBarWidthForRotation));

    CHandler::New((IHandler**)&mFlickerHandler);

    // Init member of anonymous class.
    mPromptEnterMouseMode = new PromptEnterMouseModeRunnable(this);
    mPromptExitMouseMode = new PromptExitMouseModeRunnable(this);
    mHDMIObserver = new HDMIUEventObserver(this);
    mFlickerIntentReceiver = new FlickerIntentBroadcastReceiver(this);
    mStep1On = new Step1OnRunnable(this);
    mStep2Off = new Step2OffRunnable(this);
    mStep3On = new Step3OnRunnable(this);
    mBootFastPowerLongPress = new BootFastPowerLongPressRunnable(this);
    mPowerLongPress = new PowerLongPressRunnable(this);
    mScreenshotChordLongPress = new ScreenshotChordLongPressRunnable(this);
    mHideNavInputEventReceiverFactory = new HideNavInputEventReceiverFactory(this);
    mScreenshotTimeout = new ScreenshotTimeoutRunnable(this);
    mDockReceiver = new DockBroadReceiver(this);
    mDreamReceiver = new DreamBroadReceiver(this);
    mMultiuserReceiver = new MultiuserBroadReceiver(this);
    mScreenLockTimeout = new ScreenLockTimeoutRunnable(this);

    mFallbackActions = new HashMap<Int32, AutoPtr<IInterface> >();
}

void CPhoneWindowManager::StartFlicker()
{
    Slogger::D("GpioService", "mFlickerEnable = %d", mFlickerEnable);
    if (mFlickerEnable) {
        AutoPtr<IGpio> gpio;
        CGpio::AcquireSingleton((IGpio**)&gpio);
        Int32 result = 0;
        gpio->WriteGpio(PORT_TYPE, PORT_NUM, OFF, &result);




        Boolean isSuccess = FALSE;
        mFlickerHandler->PostDelayed(mStep1On, FLICKER_INTERVAL, &isSuccess);
    }
}

AutoPtr<IIStatusBarService> CPhoneWindowManager::GetStatusBarService()
{
    AutoLock lock(mServiceAquireLock);
    if (mStatusBarService == NULL) {
        AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::STATUS_BAR_SERVICE);
        mStatusBarService = IIStatusBarService::Probe(tmpObj.Get());
    }
    return mStatusBarService;
}

Boolean CPhoneWindowManager::NeedSensorRunningLp()
{
    if (mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR
            || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR
            || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT
            || mCurrentAppOrientation == IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE) {
            // If the application has explicitly requested to follow the
            // orientation, then we need to turn the sensor or.
            return TRUE;
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

    return TRUE;
}

void CPhoneWindowManager::UpdateOrientationListenerLp()
{
    // TODO : WindowOrientationListener is not implement.
    // if (!mOrientationListener->CanDetectOrientation()) {
        // // If sensor is turned off or nonexistent for some reason
    //     return;
    // }
    // // Could have been invoked due to screen turning on or off or
    // // change of the currently visible window's orientation
    // if (localLOGV) Log.v(TAG, "Screen status="+mScreenOnEarly+
    //         ", current orientation="+mCurrentAppOrientation+
    //         ", SensorEnabled="+mOrientationSensorEnabled);
    Boolean disable = TRUE;
    if (mScreenOnEarly) {
        if (NeedSensorRunningLp()) {
            disable = FALSE;
            //enable listener if not already enabled
            if (!mOrientationSensorEnabled) {
                // TODO : WindowOrientationListener is not implement.
                // mOrientationListener->Enable();
                // if(localLOGV) Log.v(TAG, "Enabling listeners");
                mOrientationSensorEnabled = TRUE;
            }
        }
    }
    //check if sensors need to be disabled
    if (disable && mOrientationSensorEnabled) {
        // TODO : WindowOrientationListener is not implement.
        // mOrientationListener->Disable();
        // if(localLOGV) Log.v(TAG, "Disabling listeners");
        mOrientationSensorEnabled = FALSE;
    }
}

void CPhoneWindowManager::InterceptPowerKeyDown(
    /* [in] */ Boolean handled)
{
    mPowerKeyHandled = handled;
    if (!handled) {
        /* modified by Gary. start {{----------------------------------- */
        /* 2013-03-16 */
        /* short press on power leads to shut down */
        Int64 delay;
        Boolean shortPressLeadShutdown = SystemProperties::GetBoolean(String("ro.sw.shortpressleadshut"), FALSE);
        // Log.d(TAG, "shortPressLeadShutdown = " + shortPressLeadShutdown);
        if (shortPressLeadShutdown) {
            delay = 0;
        } else {
            delay = CViewConfiguration::GetGlobalActionKeyTimeout() + 500;
        }
        /* modified by Gary. end   -----------------------------------}} */
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mPowerLongPress, delay, &isSuccess);
    }
}

void CPhoneWindowManager::ShowBootAnimation()
{
    if (DEBUG) {
        Slogger::D(TAG, "showBootAnimation");
    }
    SystemProperties::Set(String("service.bootanim.exit"), String("0"));
    SystemProperties::Set(String("ctl.start"), String("bootanim"));
}

void CPhoneWindowManager::HideBootAnimation()
{
    if (DEBUG) {
        Slogger::D(TAG, "hideBootAnimation");
    }
    SystemProperties::Set(String("service.bootanim.exit"), String("1"));
    SystemProperties::Set(String("ctl.stop"), String("bootanim"));
}

// void CPhoneWindowManager::SetAirplaneModeState(
//     /* [in] */ Boolean enabled)
// {
    // // TODO: Sets the view to be "awaiting" if not already awaiting

    // Log.d(TAG,"setAirplaneModeState = " + enabled);
    // if(Settings.Global.getInt(mContext.getContentResolver(), Settings.Global.AIRPLANE_MODE_ON,0)==0){
    //     Log.d(TAG,"already not in Airplane mode return");
    //     return;
    // }
    // Settings.Global.putInt(mContext.getContentResolver(), Settings.Global.AIRPLANE_MODE_ON,
    //                                 enabled ? 1 : 0);

    // // Post the intent
    // Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
    // intent.putExtra("state", enabled);
    // mContext.sendBroadcast(intent);
// }

void CPhoneWindowManager::RestoreAirplaneModeState()
{
    Slogger::D(TAG, "restoreAirplaneModeState");
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Boolean enabled_save = Settings::Global::GetInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON_FAST_BOOT, 0) == 1 ? TRUE : FALSE;
    Boolean enabled_now = Settings::Global::GetInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0) == 1 ? TRUE : FALSE;
    if (enabled_save == enabled_now) {
        return;
    }

    Boolean result;
    Settings::Global::PutInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON,
                                    enabled_save ? 1 : 0, &result);
    // Post the intent
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
    intent->PutBooleanExtra(String("state"), enabled_save);
    mContext->SendBroadcast(intent);
}

void CPhoneWindowManager::InterceptBootFastPowerKeyDown(
    /* [in] */ Boolean handled)
{
    // if(DEBUG_BOOTFAST) {
    //     Log.v(TAG, "interceptBootFastPowerKeyDown");
    //     Log.v(TAG, "handled: " +  handled);
    // }

    mPowerBootKeyHandled = handled;
    /*if (!handled)*/ {
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mBootFastPowerLongPress, 1200, &isSuccess);
    }
}

Boolean CPhoneWindowManager::InterceptBootFastPowerKeyUp(
    /* [in] */ Boolean canceled)
{
    // if(DEBUG_BOOTFAST)
    //     Log.v(TAG,"interceptBootFastPowerKeyUp");
    /* if (!mPowerBootKeyHandled) */ {
        mHandler->RemoveCallbacks(mBootFastPowerLongPress);

        return !canceled;
    }
    // return FALSE;
}

Boolean CPhoneWindowManager::InterceptPowerKeyUp(
    /* [in] */ Boolean canceled)
{
    if (!mPowerKeyHandled) {
        mHandler->RemoveCallbacks(mPowerLongPress);
        return !canceled;
    }
    return FALSE;
}

void CPhoneWindowManager::CancelPendingPowerKeyAction()
{
    if (!mPowerKeyHandled) {
        mHandler->RemoveCallbacks(mPowerLongPress);
    }
    if (mPowerKeyTriggered) {
        mPendingPowerKeyUpCanceled = TRUE;
    }
}

void CPhoneWindowManager::InterceptScreenshotChord()
{
    if (mScreenshotChordEnabled
            && mVolumeDownKeyTriggered && mPowerKeyTriggered && !mVolumeUpKeyTriggered) {
        Int64 now = SystemClock::GetUptimeMillis();
        if (now <= mVolumeDownKeyTime + SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS
                && now <= mPowerKeyTime + SCREENSHOT_CHORD_DEBOUNCE_DELAY_MILLIS) {
            mVolumeDownKeyConsumedByScreenshotChord = TRUE;
            CancelPendingPowerKeyAction();

            Boolean isSuccess = FALSE;
            mHandler->PostDelayed(mScreenshotChordLongPress, GetScreenshotChordLongPressDelay(), &isSuccess);
        }
    }
}

Int64 CPhoneWindowManager::GetScreenshotChordLongPressDelay()
{
    PFL_EX("TODO: CPhoneWindowManager::GetScreenshotChordLongPressDelay()")
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator.isShowing()) {
    //     // Double the time it takes to take a screenshot from the keyguard
    //     return (long) (KEYGUARD_SCREENSHOT_CHORD_DELAY_MULTIPLIER *
    //             CViewConfiguration.getGlobalActionKeyTimeout());
    // } else {
    //     return CViewConfiguration.getGlobalActionKeyTimeout();
    // }
    return 0;
}

void CPhoneWindowManager::CancelPendingScreenshotChordAction()
{
    mHandler->RemoveCallbacks(mScreenshotChordLongPress);
}

void CPhoneWindowManager::ShowGlobalActionsDialog()
{
    if (mGlobalActions == NULL) {
        mGlobalActions = new GlobalActions(mContext, mWindowManagerFuncs);
    }
    const Boolean keyguardShowing = KeyguardIsShowingTq();
    mGlobalActions->ShowDialog(keyguardShowing, IsDeviceProvisioned());
    if (keyguardShowing) {
        // since it took two seconds of long press to bring this up,
        // poke the wake lock so they have some time to see the dialog.
        // TODO: KeyguardMediator is not implement.
        // mKeyguardMediator.userActivity();
    }
}

Boolean CPhoneWindowManager::IsDeviceProvisioned()
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    return Settings::Global::GetInt32(
                contentResolver, ISettingsGlobal::DEVICE_PROVISIONED, 0) != 0;
}

void CPhoneWindowManager::HandleLongPressOnHome()
{
    // We can't initialize this in init() since the configuration hasn't been loaded yet.
    if (mLongPressOnHomeBehavior < 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetInteger(R::integer::config_longPressOnHomeBehavior, &mLongPressOnHomeBehavior);
        if (mLongPressOnHomeBehavior < LONG_PRESS_HOME_NOTHING ||
                mLongPressOnHomeBehavior > LONG_PRESS_HOME_RECENT_SYSTEM_UI) {
            mLongPressOnHomeBehavior = LONG_PRESS_HOME_NOTHING;
        }
    }

    if (mLongPressOnHomeBehavior != LONG_PRESS_HOME_NOTHING) {
        Boolean result;
        PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::LONG_PRESS, FALSE, &result);
        SendCloseSystemWindows(SYSTEM_DIALOG_REASON_RECENT_APPS);

        // Eat the longpress so it won't dismiss the recent apps dialog when
        // the user lets go of the home key
        mHomeLongPressed = TRUE;
    }

    if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_DIALOG) {
        ShowOrHideRecentAppsDialog(RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS);
    }
    else if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_SYSTEM_UI) {
        // try {
        ECode ec = NOERROR;
        AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
        if (statusbar != NULL) {
            ec = statusbar->ToggleRecentApps();
        }
        // } catch (RemoteException e) {
        //     Slog.e(TAG, "RemoteException when showing recent apps", e);
        //     // re-acquire status bar service next time it is needed.
        if (FAILED(ec))
            mStatusBarService = NULL;
        // }
    }
}

void CPhoneWindowManager::ShowOrHideRecentAppsDialog(
    /* [in] */ Int32 behavior)
{
    AutoPtr<IRunnable> runnable = new ShowOrHideRecentAppsDialogRunnable(this, behavior);
    Boolean bval;
    mHandler->Post(runnable.Get(), &bval);
}

ECode CPhoneWindowManager::Init(
    /* [in] */ IContext* context,
    /* [in] */ IIWindowManager* windowManager,
    /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs)
{
    mContext = context;
    mWindowManager = windowManager;
    mWindowManagerFuncs = windowManagerFuncs;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String equalsStr;
    sp->Get(String("ro.config.headless"), String("0"), &equalsStr);
    mHeadless = String("1").Equals(equalsStr);
    // if (!mHeadless) {
    //     // don't create KeyguardViewMediator if headless
    //     mKeyguardMediator = new KeyguardViewMediator(context, NULL);
    // }
    mHandler = new PolicyHandler(this);
    //mOrientationListener = new MyOrientationListener(mContext);
    mSettingsObserver = new SettingsObserver(mHandler, this);
    mSettingsObserver->Observe();
    mShortcutManager = new ShortcutManager(context, mHandler);
    mShortcutManager->Observe();
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

    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&mPowerManager);
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("PhoneWindowManager.mBroadcastWakeLock"), (IPowerManagerWakeLock**)&mBroadcastWakeLock);
    String equalsStr2;
    sp->Get(String("ro.debuggable"), &equalsStr2);
    mEnableShiftMenuBugReports = String("1").Equals(equalsStr2);

    mLidOpenRotation = ReadRotation(
            R::integer::config_lidOpenRotation);
    mCarDockRotation = ReadRotation(
            R::integer::config_carDockRotation);
    mDeskDockRotation = ReadRotation(
            R::integer::config_deskDockRotation);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
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

    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&mVibrator);

    mLongPressVibePattern = GetLongIntArray(resources,
            R::array::config_longPressVibePattern);
    mVirtualKeyVibePattern = GetLongIntArray(resources,
            R::array::config_virtualKeyVibePattern);
    mKeyboardTapVibePattern = GetLongIntArray(resources,
             R::array::config_keyboardTapVibePattern);
    mSafeModeDisabledVibePattern = GetLongIntArray(resources,
            R::array::config_safeModeDisabledVibePattern);
    mSafeModeEnabledVibePattern = GetLongIntArray(resources,
            R::array::config_safeModeEnabledVibePattern);

        /* add by Gary. start {{----------------------------------- */
        /* 2011-12-7 */
        /* support the mouse mode */
//      mMouseToast = Toast.makeText(mContext, com.android.internal.R.string.enter_key_mouse_mode, Toast.LENGTH_SHORT);
//      if(mMouseToast == null){
//          Log.w(TAG, "Fail in creating toast.");
//      }else {
//          mMouseToast.setGravity(Gravity.CENTER, 0, 0);
//      }
    mLeftBtn  = SystemProperties::GetInt32(String("ro.softmouse.leftbtn.code"), -1);
    mMidBtn   = SystemProperties::GetInt32(String("ro.softmouse.midbtn.code"), -1);
    mRightBtn = SystemProperties::GetInt32(String("ro.softmouse.rightbtn.code"), -1);
    mLeft     = SystemProperties::GetInt32(String("ro.softmouse.left.code"), -1);
    mRight    = SystemProperties::GetInt32(String("ro.softmouse.right.code"), -1);
    mTop      = SystemProperties::GetInt32(String("ro.softmouse.top.code"), -1);
    mBottom   = SystemProperties::GetInt32(String("ro.softmouse.bottom.code"), -1);
    // Log.d(TAG, "mLeftBtn = " + mLeftBtn + ", mBottom = " + mBottom);
    /* add by Gary. end   -----------------------------------}} */

    resources->GetBoolean(R::bool_::config_enableScreenshotChord, &mScreenshotChordEnabled);

    // Controls rotation and the like.
    InitializeHdmiState();
    mFullScreenIsEnable = SystemProperties::GetBoolean(String("ro.statusbar.inheritfullscn"), TRUE);
    mAlwaysFullScreen = SystemProperties::GetBoolean(String("ro.statusbar.alwayshide"), FALSE);
    Logger::D(TAG, "mFullScreenIsEnable = %d", mFullScreenIsEnable);

    // Match current screen state.
    Boolean isOn;
    if (mPowerManager->IsScreenOn(&isOn), isOn) {
        ScreenTurningOn(NULL);
    }
    else {
        ScreenTurnedOff(IWindowManagerPolicy::OFF_BECAUSE_OF_USER);
    }

    return NOERROR;
}

ECode CPhoneWindowManager::SetInitialDisplaySize(
    /* [in] */ IDisplay* display,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density)
{
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

    String tabeltUI = Build::TABLETUI;

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);

    if (tabeltUI.EqualsIgnoreCase("TRUE")) {
        // tabeltUI=true: "tablet" UI with a single combined status & navigation bar
        mHasSystemNavBar = TRUE;
        mNavigationBarCanMove = FALSE;
    }

    else if (shortSizeDp < 600) {
        // 0-599dp: "phone" UI with a separate status & navigation bar
        mHasSystemNavBar = FALSE;
        mNavigationBarCanMove = TRUE;
    }
    else if (shortSizeDp < 720) {
        // 600+dp: "phone" UI with modifications for larger screens
        mHasSystemNavBar = FALSE;
        mNavigationBarCanMove = FALSE;
    }

    if (!mHasSystemNavBar) {
        res->GetBoolean(R::bool_::config_showNavigationBar, &mHasNavigationBar);
        // Allow a system property to override this. Used by the emulator.
        // See also hasNavigationBar().

        String navBarOverride;
        sp->Get(String("qemu.hw.mainkeys"), &navBarOverride);
        if (String("").Equals(navBarOverride) != 0) {
            if      (navBarOverride.Equals(String("1"))) mHasNavigationBar = FALSE;
            else if (navBarOverride.Equals(String("0"))) mHasNavigationBar = TRUE;
        }
    } else {
        mHasNavigationBar = FALSE;
    }

    if (mHasSystemNavBar) {
        // The system bar is always at the bottom.  If you are watching
        // a video in landscape, we don't need to hide it if we can still
        // show a 16:9 aspect ratio with it.
        Int32 longSizeDp = longSize * IDisplayMetrics::DENSITY_DEFAULT / density;
        Int32 barHeightDp = mNavigationBarHeightForRotation[mLandscapeRotation]
                * IDisplayMetrics::DENSITY_DEFAULT / density;
        Int32 aspect = ((shortSizeDp - barHeightDp) * 16) / longSizeDp;
        // We have computed the aspect ratio with the bar height taken
        // out to be 16:aspect.  If this is less than 9, then hiding
        // the navigation bar will provide more useful space for wide
        // screen movies.
        mCanHideNavigationBar = aspect < 9;
    } else if (mHasNavigationBar) {
        // The navigation bar is at the right in landscape; it seems always
        // useful to hide it for showing a video.
        mCanHideNavigationBar = TRUE;
    } else {
        mCanHideNavigationBar = FALSE;
    }

    // For demo purposes, allow the rotation of the HDMI display to be controlled.
    // By default, HDMI locks rotation to landscape.
    String equalsStr;
    sp->Get(String("persist.demo.hdmirotation"), &equalsStr);
    if (String("portrait").Equals(equalsStr) == 0) {
        mHdmiRotation = mPortraitRotation;
    } else {
        mHdmiRotation = mLandscapeRotation;
    }

    mHdmiRotationLock = SystemProperties::GetBoolean(String("persist.demo.hdmirotationlock"), TRUE);

    return NOERROR;
}

void CPhoneWindowManager::UpdateSettings()
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
        Int32 temValue;
        Settings::System::GetInt32ForUser(resolver, ISettingsSystem::BOOT_FAST_ENABLE, 0,
                IUserHandle::USER_CURRENT, &temValue);
        mBootFastEnable = temValue == 1 ? TRUE : FALSE;
        // if(DEBUG_BOOTFAST)
        //     Log.d(TAG,"update Settings mBootFastEnable = " + mBootFastEnable);
        Boolean hasSoftInput = imId != NULL && imId.GetLength() > 0;
        if (mHasSoftInput != hasSoftInput) {
            mHasSoftInput = hasSoftInput;
            updateRotation = TRUE;
        }
    }
    if (updateRotation) {
        UpdateRotation(TRUE);
    }
}

void CPhoneWindowManager::EnablePointerLocation()
{
    if (mPointerLocationView == NULL) {
        CPointerLocationView::New(mContext, (IPointerLocationView**)&mPointerLocationView);
        mPointerLocationView->SetPrintCoords(FALSE);

        AutoPtr<IWindowManagerLayoutParams> lp;
        CWindowManagerLayoutParams::New(
                IWindowManagerLayoutParams::MATCH_PARENT,
                IWindowManagerLayoutParams::MATCH_PARENT,
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
        AutoPtr<IWindowManager> wm;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);

        Int32 inputFeatures = 0;
        lp->GetInputFeatures(&inputFeatures);
        lp->SetInputFeatures(inputFeatures | IWindowManagerLayoutParams::INPUT_FEATURE_NO_INPUT_CHANNEL);
        AutoPtr<IViewManager> vm = IViewManager::Probe(wm);
        vm->AddView(mPointerLocationView, lp);

        mPointerLocationInputChannel = NULL;
        mWindowManagerFuncs->MonitorInput(
            String("PointerLocationView"), (IInputChannel**)&mPointerLocationInputChannel);
        AutoPtr<ILooperHelper> looperHelper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
        AutoPtr<ILooper> looper;
        looperHelper->MyLooper((ILooper**)&looper);
        mPointerLocationInputEventReceiver =
                new PointerLocationInputEventReceiver(mPointerLocationInputChannel,
                        looper, mPointerLocationView);
    }
}

void CPhoneWindowManager::DisablePointerLocation()
{
    if (mPointerLocationInputEventReceiver != NULL) {
        mPointerLocationInputEventReceiver->Dispose();
        mPointerLocationInputEventReceiver = NULL;
    }

    if (mPointerLocationInputChannel != NULL) {
        mPointerLocationInputChannel->Dispose();
        mPointerLocationInputChannel = NULL;
    }

    if (mPointerLocationView != NULL) {
        AutoPtr<IWindowManager> wm;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);

        wm->RemoveView(mPointerLocationView);
        mPointerLocationView = NULL;
    }
}

void CPhoneWindowManager::AcquireBootAnimationView()
{
    if (mBootAnimationView == NULL) {
        mBootAnimationView = new BootAnimationView(mContext);
        AutoPtr<IWindowManagerLayoutParams> lp;
        CWindowManagerLayoutParams::New(
                IWindowManagerLayoutParams::MATCH_PARENT,
                IWindowManagerLayoutParams::MATCH_PARENT,
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
        CString::New(String("BootAnimationView"), (ICharSequence**)&title);
        lp->SetTitle(title);

        AutoPtr<IWindowManager> wm;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
        Int32 inputFeatures = 0;
        lp->GetInputFeatures(&inputFeatures);
        lp->SetInputFeatures(inputFeatures | IWindowManagerLayoutParams::INPUT_FEATURE_NO_INPUT_CHANNEL);
        AutoPtr<IViewManager> vm = IViewManager::Probe(wm);
        vm->AddView(mBootAnimationView, lp);
        // if(DEBUG_BOOTFAST)
        //     Log.d(TAG,"acquireBootAnimationView finish");
    }
}

void CPhoneWindowManager::ReleaseBootAnimationView()
{
    if (mBootAnimationView != NULL) {
        AutoPtr<IWindowManager> wm;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
        AutoPtr<IViewManager> vm = IViewManager::Probe(wm);
        vm->RemoveView(mBootAnimationView);
        mBootAnimationView = NULL;
    }
    // if(DEBUG_BOOTFAST)
    //     Log.d(TAG,"releaseBootAnimationView finish");
}

Int32 CPhoneWindowManager::ReadRotation(
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

ECode CPhoneWindowManager::CheckAddPermission(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* addPermisssion)
{
    if (addPermisssion == NULL) {
        return E_INVALID_ARGUMENT;
    }

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
            break;
        case IWindowManagerLayoutParams::TYPE_DREAM:
        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        case IWindowManagerLayoutParams::TYPE_WALLPAPER:
            // The window manager will check these.
            break;
        case IWindowManagerLayoutParams::TYPE_PHONE:
        case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
            permission = Elastos::Droid::Manifest::permission::SYSTEM_ALERT_WINDOW;
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

ECode CPhoneWindowManager::CheckShowToOwnerOnly(
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
        case IWindowManagerLayoutParams::TYPE_KEYGUARD:
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

ECode CPhoneWindowManager::AdjustWindowParamsLw(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    Int32 type = 0;
    attrs->GetType(&type);
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_TOAST:
            // These types of windows can't receive input events.
            Int32 flags = 0;
            attrs->GetFlags(&flags);
            flags |= IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
            flags &= ~IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH;
            attrs->SetFlags(flags);
            break;
    }

    return NOERROR;
}

void CPhoneWindowManager::ReadLidState()
{
    mWindowManagerFuncs->GetLidState(&mLidState);
}

Boolean CPhoneWindowManager::IsHidden(
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

Boolean CPhoneWindowManager::IsBuiltInKeyboardVisible()
{
    return mHaveBuiltInKeyboard && !IsHidden(mLidKeyboardAccessibility);
}

ECode CPhoneWindowManager::AdjustConfigurationLw(
    /* [in] */ IConfiguration* _config,
    /* [in] */ Int32 keyboardPresence,
    /* [in] */ Int32 navigationPresence)
{
    assert(_config);
    CConfiguration* config = (CConfiguration*)_config;

    mHaveBuiltInKeyboard = (keyboardPresence & PRESENCE_INTERNAL) != 0;

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

ECode CPhoneWindowManager::WindowTypeToLayerLw(
    /* [in] */ Int32 type,
    /* [out] */ Int32* layer)
{
    if (layer == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *layer = 2;
    if (type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW && type <=
        IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW) {
        return NOERROR;
    }

    switch (type) {
    case IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND:
        *layer = 1;
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
    case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY:
    case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
        *layer = 5;
        break;
    case IWindowManagerLayoutParams::TYPE_TOAST:
        // toasts and the plugged-in battery thing
        *layer = 6;
        break;
    case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        // SIM errors and unlock.  Not sure if this really should be in a high layer.
        *layer = 7;
        break;
    case IWindowManagerLayoutParams::TYPE_DREAM:
        // used for Dreams (screensavers with TYPE_DREAM windows)
        *layer = 8;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
        // like the ANR / app crashed dialogs
        *layer = 9;
        break;
    case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        // on-screen keyboards and other such input method user interfaces go here.
        *layer = 10;
        break;
    case IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG:
        // on-screen keyboards and other such input method user interfaces go here.
        *layer = 11;
        break;
    case IWindowManagerLayoutParams::TYPE_KEYGUARD:
        // the keyguard; nothing on top of these can take focus, since they are
        // responsible for power management when displayed.
        *layer = 12;
        break;
    case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
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
    case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
        // the on-screen volume indicator and controller shown when the user
        // changes the device volume
        *layer = 17;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY:
        // the on-screen volume indicator and controller shown when the user
        // changes the device volume
        *layer = 18;
        break;
    case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        // the navigation bar, if available, shows atop most things
        *layer = 19;
        break;
    case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
        // some panels (e.g. search) need to show on top of the navigation bar
        *layer = 20;
        break;
    case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
        // system-level error dialogs
        *layer = 21;
        break;
    case IWindowManagerLayoutParams::TYPE_MAGNIFICATION_OVERLAY:
        // used to highlight the magnified portion of a display
        *layer = 22;
        break;
    case IWindowManagerLayoutParams::TYPE_DISPLAY_OVERLAY:
        // used to simulate secondary display devices
        *layer = 23;
        break;
    case IWindowManagerLayoutParams::TYPE_DRAG:
        // the drag layer: input for drag-and-drop is associated with this window,
        // which sits above all other focusable windows
        *layer = 24;
        break;
    case IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY:
        *layer = 25;
        break;
    case IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS:
        *layer = 26;
        break;
    case IWindowManagerLayoutParams::TYPE_POINTER:
        // the (mouse) pointer layer
        *layer = 27;
        break;
    case IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER:
        *layer = 28;
        break;
    }
    // Log.e(TAG, "Unknown window type: " + type);

    return NOERROR;
}

ECode CPhoneWindowManager::SubWindowTypeToLayerLw(
    /* [in] */ Int32 type,
    /* [out] */ Int32* layer)
{
    if (layer == NULL) {
        return E_INVALID_ARGUMENT;
    }

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

ECode CPhoneWindowManager::GetMaxWallpaperLayer(
    /* [out] */ Int32* layer)
{
    return WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_STATUS_BAR, layer);
}

ECode CPhoneWindowManager::GetAboveUniverseLayer(
    /* [out] */ Int32* layer)
{
    return WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR, layer);
}

ECode CPhoneWindowManager::HasSystemNavBar(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mHasSystemNavBar;
    return NOERROR;
}

ECode CPhoneWindowManager::GetNonDecorDisplayWidth(
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

ECode CPhoneWindowManager::GetNonDecorDisplayHeight(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* height)
{
    if (mHasSystemNavBar) {
        // For the system navigation bar, we always place it at the bottom.
        *height = fullHeight - mNavigationBarHeightForRotation[rotation];
        return NOERROR;
    }
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

ECode CPhoneWindowManager::GetConfigDisplayWidth(
    /* [in] */ Int32 fullWidth,
    /* [in] */ Int32 fullHeight,
    /* [in] */ Int32 rotation,
    /* [out] */ Int32* value)
{
    return GetNonDecorDisplayWidth(fullWidth, fullHeight, rotation, value);
}

ECode CPhoneWindowManager::GetConfigDisplayHeight(
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
    if (!mHasSystemNavBar) {
        Int32 tmp = 0;
        GetNonDecorDisplayHeight(fullWidth, fullHeight, rotation, &tmp);
        *value = tmp - mStatusBarHeight;
        return NOERROR;
    }

    return GetNonDecorDisplayHeight(fullWidth, fullHeight, rotation, value);
}

ECode CPhoneWindowManager::DoesForceHide(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* hiden)
{
    if (hiden == NULL) {
        return E_INVALID_ARGUMENT;
    }

    Int32 type = 0;
    attrs->GetType(&type);
    *hiden = type == IWindowManagerLayoutParams::TYPE_KEYGUARD;

    return NOERROR;
}

ECode CPhoneWindowManager::CanBeForceHidden(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* hiden)
{
    if (hiden == NULL) {
        return E_INVALID_ARGUMENT;
    }

    Int32 type = 0;
    attrs->GetType(&type);
    switch (type) {
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        case IWindowManagerLayoutParams::TYPE_WALLPAPER:
        case IWindowManagerLayoutParams::TYPE_DREAM:
        case IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND:
        case IWindowManagerLayoutParams::TYPE_KEYGUARD:
            *hiden = FALSE;
            break;
        default:
            *hiden = TRUE;
    }

    return NOERROR;
}

ECode CPhoneWindowManager::AddStartingWindow(
    /* [in] */ IBinder* appToken,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 theme,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon,
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

        // TODO: compatInfo != NULL
        //
        Boolean supportsScreen;
        if (compatInfo != NULL && !(compatInfo->SupportsScreen(&supportsScreen), supportsScreen)) {
            win->AddFlags(IWindowManagerLayoutParams::FLAG_COMPATIBLE_WINDOW);
        }

        win->SetLayout(
            IWindowManagerLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::MATCH_PARENT);

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
        lp->SetPrivateFlags(privateFlags
                | IWindowManagerLayoutParams::PRIVATE_FLAG_FAKE_HARDWARE_ACCELERATED
                | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);

        AutoPtr<ICharSequence> tl;
        CString::New(String("Starting ") + packageName, (ICharSequence**)&tl);
        lp->SetTitle(tl);

        AutoPtr<IWindowManager> wm;
        context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);

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

        ec = wm->AddView(view, lp);
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

    return NOERROR;
}

ECode CPhoneWindowManager::RemoveStartingWindow(
    /* [in] */ IBinder* appToken,
    /* [in] */ IView* window)
{
    if (DEBUG_STARTING_WINDOW) {
        Slogger::V(TAG, "Removing starting window for %p: window %p", appToken, window);
    }

    if (window != NULL) {
        AutoPtr<IWindowManager> wm;
        mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
        wm->RemoveView(window);
    }

    return NOERROR;
}

ECode CPhoneWindowManager::PrepareAddWindowLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* added)
{
    if (added == NULL) {
        return E_INVALID_ARGUMENT;
    }
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
            //if (DEBUG_LAYOUT) Log.i(TAG, "NAVIGATION BAR: " + mNavigationBar);
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
        case IWindowManagerLayoutParams::TYPE_KEYGUARD:
            if (mKeyguard != NULL) {
                *added = IWindowManagerGlobal::ADD_MULTIPLE_SINGLETON;
                return NOERROR;
            }
            mKeyguard = win;
            break;
    }
    *added = IWindowManagerGlobal::ADD_OKAY;

    return NOERROR;
}

ECode CPhoneWindowManager::RemoveWindowLw(
    /* [in] */ IWindowState* win)
{
    if (mStatusBar.Get() == win) {
        mStatusBar = NULL;
    }
    else if (mKeyguard.Get() == win) {
        mKeyguard = NULL;
    }
    else if (mNavigationBar.Get() == win) {
        mNavigationBar = NULL;
    }

    return NOERROR;
}

ECode CPhoneWindowManager::SelectAnimationLw(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 transit,
    /* [out] */ Int32* id)
{
    // if (PRINT_ANIM) Log.i(TAG, "selectAnimation in " + win
    //       + ": transit=" + transit);
    if (win == mStatusBar) {
        if (transit == TRANSIT_EXIT || transit == TRANSIT_HIDE) {
            *id = R::anim::dock_top_exit;
            return NOERROR;
        } else if (transit == TRANSIT_ENTER || transit == TRANSIT_SHOW) {
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

ECode CPhoneWindowManager::CreateForceHideEnterAnimation(
    /* [in] */ Boolean onWallpaper,
    /* [out] */ IAnimation** anim)
{
    VALIDATE_NOT_NULL(anim);
    AutoPtr<IAnimationUtils> animUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animUtils);
    return animUtils->LoadAnimation(mContext, onWallpaper
            ? R::anim::lock_screen_wallpaper_behind_enter
            : R::anim::lock_screen_behind_enter,
            (IAnimation**)&anim);
}

// static ITelephony GetTelephonyService() {
//     return ITelephony.Stub.asInterface(
//             ServiceManager.checkService(Context.TELEPHONY_SERVICE));
// }

AutoPtr<IIAudioService> CPhoneWindowManager::GetAudioService()
{
    // IAudioService audioService = IAudioService.Stub.asInterface(
    //     ServiceManager.checkService(Context.AUDIO_SERVICE));
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    AutoPtr<IIAudioService> audioService = IIAudioService::Probe(obj.Get());
    if (audioService == NULL) {
        Logger::E(TAG, "Unable to find IAudioService interface.");
    }
    return audioService;
}

Boolean CPhoneWindowManager::KeyguardOn()
{
    Boolean restricted;
    InKeyguardRestrictedKeyInputMode(&restricted);

    return KeyguardIsShowingTq() || restricted;
}

ECode CPhoneWindowManager::InterceptKeyBeforeDispatching(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    /* add by chenjd. start {{----------------------------------- */
    /* 20113-3-2 */
    /* flicker led when receive ir signal */
    Int64 consumed = InnerInterceptKeyBeforeDispatching(win, event, policyFlags);

    Int32 action, keyCode;
    event->GetAction(&action);
    event->GetKeyCode(&keyCode);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    Int32 keycode = keyCode;
    // Log.d(TAG, "keycode is " + event.getKeyCode() + " , and POWER is " + KeyEvent.KEYCODE_POWER);
    if (down && keycode != IKeyEvent::KEYCODE_POWER && keycode != IKeyEvent::KEYCODE_UNKNOWN) {
        StartFlicker();
    }

    *result = consumed;
    return NOERROR;
}

Int64 CPhoneWindowManager::InnerInterceptKeyBeforeDispatching(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Boolean keyguardOn = KeyguardOn();

    ECode ec = NOERROR;
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

    if ((flags & IKeyEvent::FLAG_LONG_PRESS) != 0
        && keyCode == IKeyEvent::KEYCODE_HOME
        && ((policyFlags & IWindowManagerPolicy::FLAG_VIRTUAL)==0)) {
        if (!keyguardOn) {
            keyCode = IKeyEvent::KEYCODE_ASSIST;
        }
    }
    else if (mAssistKeyLongPressed == TRUE
        && keyCode == IKeyEvent::KEYCODE_HOME
        && ((policyFlags & IWindowManagerPolicy::FLAG_VIRTUAL)==0)) {
        if (!keyguardOn) {
            keyCode = IKeyEvent::KEYCODE_ASSIST;
        }
    }

    // First we always handle the home key here, so applications
    // can never break it, although if keyguard is on, we do let
    // it handle it, because that gives us the correct 5 second
    // timeout.
    if (keyCode == IKeyEvent::KEYCODE_HOME) {
        // If we have released the home key, and didn't do anything else
        // while it was pressed, then it is time to go home!
        if (!down) {
            Boolean homeWasLongPressed = mHomeLongPressed;
            mHomePressed = FALSE;
            mHomeLongPressed = FALSE;
            if (!homeWasLongPressed) {
                if (mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_SYSTEM_UI) {
                        ec = NOERROR;
    //                 try {
                        AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
                        if (statusbar != NULL) {
                            ec = statusbar->CancelPreloadRecentApps();
                        }
    //                 } catch (RemoteException e) {
                        // re-acquire status bar service next time it is needed.
                        if (FAILED(ec))
                            Slogger::E(TAG, "RemoteException when showing recent apps, ec=%08x", ec);
                            mStatusBarService = NULL;
    //                 }
                }

                mHomePressed = FALSE;
                if (!canceled) {
                    // If an incoming call is ringing, HOME is totally disabled.
                    // (The user is already on the InCallScreen at this point,
                    // and his ONLY options are to answer or reject the call.)
                    Boolean incomingRinging = FALSE;
    //                 try {
    //                     ITelephony telephonyService = getTelephonyService();
    //                     if (telephonyService != NULL) {
    //                         incomingRinging = telephonyService.isRinging();
    //                     }
    //                 } catch (RemoteException ex) {
    //                     Log.w(TAG, "RemoteException from getPhoneInterface()", ex);
    //                 }

                    if (incomingRinging) {
                        // Log.i(TAG, "Ignoring HOME; there's a ringing incoming call.");
                    }
                    else {
                        // If a system window has focus, then it doesn't make sense
                        // right now to interact with applications.
                        AutoPtr<IWindowManagerLayoutParams> attrs;
                        if (win != NULL) {
                            win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
                        }

                        //if window type value is WINDOW_TYPE_TEST_MODE = 2099, catch home event, and dispatch it to view
                        if (attrs != NULL) {
                            Int32 type;
                            attrs->GetType(&type);
                            if (type == WINDOW_TYPE_TEST_MODE) {
                                return 0;
                            }
                        }

                        LaunchHomeFromHotKey();
                    }
                } else {
                    Logger::I(TAG, "Ignoring HOME; event canceled.");
                }
                return -1;
            }
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
            if (type == IWindowManagerLayoutParams::TYPE_KEYGUARD
                    || type == IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG) {
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
        if (down) {
            ECode ec = NOERROR;
            if (!mHomePressed && mLongPressOnHomeBehavior == LONG_PRESS_HOME_RECENT_SYSTEM_UI) {
    //             try {
                    AutoPtr<IIStatusBarService> statusbar = GetStatusBarService();
                    if (statusbar != NULL) {
                        ec = statusbar->PreloadRecentApps();
                    }
    //             } catch (RemoteException e) {
    //                 Slog.e(TAG, "RemoteException when preloading recent apps", e);
                    // re-acquire status bar service next time it is needed.
                    if (FAILED(ec)) mStatusBarService = NULL;
    //             }
            }
            if (repeatCount == 0) {
                mHomePressed = TRUE;
            }
            else if ((flags & IKeyEvent::FLAG_LONG_PRESS) != 0) {
                if (!keyguardOn) {
                    HandleLongPressOnHome();
                }
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
        if (down && repeatCount == 0 && !keyguardOn) {
            ShowOrHideRecentAppsDialog(RECENT_APPS_BEHAVIOR_SHOW_OR_DISMISS);
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
                    // LaunchAssistLongPressAction();
                    LaunchAssistAction();
                }
            }
        }
        else {
            if (mAssistKeyLongPressed) {
                mAssistKeyLongPressed = FALSE;
            }
            else {
                if (!keyguardOn) {
                    // LaunchAssistAction();
                    LaunchAssistLongPressAction();
                }
            }
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_VOLUME_MUTE) {
        // TODO: Java has not this.
        // Log.d(getClass().getName(), "mVolumeMute is: " + mVolumeMute);

        AutoPtr<IInterface> audioService;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(audioService);

        if (down) {
            if (mVolumeMute) {
                // audioManager.setStreamMute(IAudioManager::STREAM_MUSIC, FALSE);
                // int volume = audioManager.getStreamVolume(IAudioManager::STREAM_MUSIC);
                audioManager->SetStreamVolume(IAudioManager::STREAM_MUSIC, mPreVolume,
                    IAudioManager::FLAG_SHOW_UI);
                mVolumeMute = FALSE;
            }
            else {
                // audioManager.setStreamMute(IAudioManager::STREAM_MUSIC, TRUE);
                audioManager->GetStreamVolume(IAudioManager::STREAM_MUSIC, &mPreVolume);
                audioManager->SetStreamVolume(IAudioManager::STREAM_MUSIC, 0,
                    IAudioManager::FLAG_SHOW_UI);
                mVolumeMute = TRUE;
            }
        }

        return 0;
    }
    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-1 */
    /* support the shortcut keys "setup", "expand" and "browser" */
    else if (keyCode == IKeyEvent::KEYCODE_BROWSER || keyCode == IKeyEvent::KEYCODE_EXPLORER) {
        // Log.v(TAG, "it's KEYCODE_BROWSER key and down = " + down);
        if (!down) {
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            AutoPtr<IUri> uri;
            uriHelper->Parse(String("http://www.qiyi.com"), (IUri**)&uri);

            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&intent);
            if (intent == NULL) {
                // Log.d(TAG, "Fail in creating intent.");
                return -1;
            }
            intent->AddCategory(IIntent::CATEGORY_BROWSABLE);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(intent);
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_SETTINGS) {
        // Log.v(TAG, "it's KEYCODE_SETTINGS key and down = " + down);
        if (!down) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<IComponentName> componentName;
            CComponentName::New(String("com.android.settings"), String("com.android.settings.Settings"),
                    (IComponentName**)&componentName);
            intent->SetComponent(componentName);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(intent);
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_EXPAND) {
        // Log.v(TAG, "it's KEYCODE_EXPAND key and down = " + down);
        if (!down) {
            AutoPtr<IStatusBarManager> mStatusBarManager;
            if (mStatusBarManager == NULL) {
                mContext->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&mStatusBarManager);
                if (mStatusBarManager == NULL) {
                    // Log.w(TAG, "Fail in getting STATUS_BAR_SERVICE");
                    return -1;
                }
            }
            if (mIsExpanded) {
                mStatusBarManager->CollapsePanels();
            } else {
                mStatusBarManager->ExpandNotificationsPanel();
            }
            mIsExpanded = !mIsExpanded;
        }
        return -1;
    }
    /* add by Gary. end   -----------------------------------}} */
    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-1 */
    /* support shortcut keys with color keys for some specific websites and apps */
    else if (keyCode == IKeyEvent::KEYCODE_PROG_RED
            ||keyCode == IKeyEvent::KEYCODE_PROG_GREEN
            ||keyCode == IKeyEvent::KEYCODE_PROG_YELLOW
            ||keyCode == IKeyEvent::KEYCODE_PROG_BLUE  ) {
        // Log.v(TAG, "it's key " + keyCode + " and down = " + down);
        if (!down) {
            String name = Settings::System::FindNameByKey(keyCode);
            if (name.IsNull()) {
                // Log.w(TAG, "the key " + keyCode + " has no shortcut.");
                return -1;
            }
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            String str;
            Settings::System::GetString(resolver, name, &str);
            if (str.IsNull()) {
                // Log.w(TAG, "fail in getting " + name + " from SettingProvider");
                return -1;
            }
            // Log.d(TAG, "shortcut path is " + str);
            /* parse string */
            AutoPtr<ArrayOf<String> > strArray;
            StringUtils::Split(str, ISettingsSystem::SHORTCUT_PATH_SEPARATOR, (ArrayOf<String>**)&strArray);
            if (strArray == NULL) {
                // Log.w(TAG, "fail in spliting string " + str + " with substring \"--split--\".");
                return -1;
            }
            AutoPtr<IIntent> intent;
            if ((*strArray)[0].Equals(ISettingsSystem::SHORTCUT_PATH_TYPE_WEBSITE)) {
                AutoPtr<IUriHelper> uriHelper;
                CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
                AutoPtr<IUri> uri;
                uriHelper->Parse((*strArray)[1], (IUri**)&uri);
                CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&intent);
                intent->AddCategory(IIntent::CATEGORY_BROWSABLE);
            }
            else if ((*strArray)[0].Equals(ISettingsSystem::SHORTCUT_PATH_TYPE_APP)) {
                CIntent::New((IIntent**)&intent);
                AutoPtr<IComponentName> componentName;
                CComponentName::New((*strArray)[1], (*strArray)[2], (IComponentName**)&componentName);
                intent->SetComponent(componentName);
            }
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(intent);
        }
        return -1;
    }
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-1 */
    /* support shortcut keys : movie and apps */
    else if (keyCode == IKeyEvent::KEYCODE_MOVIE) {
        // Log.v(TAG, "it's KEYCODE_MOVIE key and down = " + down);
        if (!down) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<IComponentName> componentName;
            CComponentName::New(String("com.softwinner.TvdFileManager"),
                    String("com.softwinner.TvdFileManager.MainUI"),
                    (IComponentName**)&componentName);
            intent->SetComponent(componentName);
            intent->PutExtra(String("media_type"), String("MEDIA_TYPE_VIDEO"));
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(intent);
        }
        return -1;
    }
    else if (keyCode == IKeyEvent::KEYCODE_APPS) {
        // Log.v(TAG, "it's KEYCODE_APPS key and down = " + down);
        if (!down) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<CComponentName> componentName;
            CComponentName::New(String("com.softwinner.launcher"),
                String("com.softwinner.launcher.Launcher"),
                (IComponentName**)&componentName);
            intent->SetComponent(componentName);
            intent->AddCategory(String("com.softwinner.category.app"));
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(intent);
        }
        return -1;
    }
    /* add by Gary. end   -----------------------------------}} */

    /* 2011-12-7 */
    /* support the mouse mode */
    else if (keyCode == IKeyEvent::KEYCODE_MOUSE) {
        // Log.v(TAG, "it's KEYCODE_MOUSE key and down = " + down);
        if (!down) {
            if (mNotifationManager == NULL) {
                mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotifationManager);
                if (mNotifationManager == NULL) {
                    // Log.w(TAG, "Fail in get NotificationManager");
                    return -1;
                }
            }
            if (mNotificationEnterKeyMouseMode == NULL) {
                AutoPtr<IResources> resources;
                mContext->GetResources((IResources**)&resources);
                AutoPtr<ICharSequence> text;
                resources->GetText(R::string::enter_key_mouse_mode, (ICharSequence**)&text);
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 millis;
                system->GetCurrentTimeMillis(&millis);
                CNotification::New(R::drawable::key_mouse,
                    text, millis, (INotification**)&mNotificationEnterKeyMouseMode);

                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);

                AutoPtr<IPendingIntentHelper> helper;
                CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);

                AutoPtr<IPendingIntent> contentIntent;
                helper->GetActivity(mContext, 0, intent, IPendingIntent::FLAG_UPDATE_CURRENT,
                    (IPendingIntent**)&contentIntent);
                AutoPtr<ICharSequence> titleSeq, detailSeq;
                resources->GetText(R::string::title_key_mouse_mode, (ICharSequence**)&titleSeq);
                resources->GetText(R::string::detail_key_mouse_mode, (ICharSequence**)&detailSeq);
                mNotificationEnterKeyMouseMode->SetLatestEventInfo(mContext,
                    titleSeq, detailSeq, contentIntent);
            }
            if (mKeyEnterMouseMode) {
                /* cancel notification */
                mNotifationManager->Cancel(NF_ID_ENTER_KEY_MOUSE_MODE);
                /* show toast */
                mHandler->RemoveCallbacks(mPromptExitMouseMode);
                Boolean isSuccess = FALSE;
                mHandler->Post(mPromptExitMouseMode, &isSuccess);

                // try {
                mWindowManager->KeyExitMouseMode();
                mKeyEnterMouseMode = FALSE;
                // } catch(RemoteException e) {
                //     Log.e(TAG,"key Exit Mouse Mode Failed!\n");
                // }
            } else {
                /* send notification */
                mNotifationManager->Notify(NF_ID_ENTER_KEY_MOUSE_MODE, mNotificationEnterKeyMouseMode);
                /* show toast */
                mHandler->RemoveCallbacks(mPromptEnterMouseMode);
                Boolean isSuccess = FALSE;
                mHandler->Post(mPromptEnterMouseMode, &isSuccess);

                // try {
                mWindowManager->KeyEnterMouseMode();

                AutoPtr<IContentResolver> contentResolver;
                mContext->GetContentResolver((IContentResolver**)&contentResolver);
                Int32 value;
                Settings::System::GetInt32(contentResolver, ISettingsSystem::MOUSE_ADVANCE, 30, &value);
                mWindowManager->KeySetMouseDistance(value);
                mWindowManager->KeySetMouseBtnCode(mLeftBtn, mMidBtn, mRightBtn);
                mWindowManager->KeySetMouseMoveCode(mLeft, mRight, mTop, mBottom);
                mKeyEnterMouseMode = TRUE;
                // } catch(RemoteException e) {
                //     Log.e(TAG,"key enterMouse Mode Failed!\n");
                // }
            }
        }
        return -1;
    }
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* add by Gary. start {{----------------------------------- */
    /* 2012-6-8 */
    /* add a shortcut for screenshot */
    else if (keyCode == IKeyEvent::KEYCODE_SCREENSHOT) {
        // Log.v(TAG, "it's KEYCODE_SCREENSHOT key and down = " + down);
        if (!down) {
            Boolean isSuccess = FALSE;
            mHandler->Post(mScreenshotChordLongPress, &isSuccess);
        }
        return -1;
    }
    /* add by Gary. end   -----------------------------------}} */

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
                    mContext->StartActivity(shortcutIntent);
                    // } catch (ActivityNotFoundException ex) {
                    //     Slog.w(TAG, "Dropping shortcut key combination because "
                    //             + "the activity to which it is registered was not found: "
                    //             + "SEARCH+" + IKeyEvent::keyCodeToString(keyCode), ex);
                    // }
                    // } else {
                    // Slog.i(TAG, "Dropping unregistered shortcut key combination: "
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
                mContext->StartActivity(shortcutIntent);
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
            mContext->StartActivity(intent);
            // } catch (ActivityNotFoundException ex) {
            //     Slog.w(TAG, "Dropping application launch key because "
            //             + "the activity to which it is registered was not found: "
            //             + "keyCode=" + keyCode + ", category=" + category, ex);
            // }
            return -1;
        }
    }

    // Display task switcher for ALT-TAB or Meta-TAB.
    if (down && repeatCount == 0 && keyCode == IKeyEvent::KEYCODE_TAB) {
        if (mRecentAppsDialogHeldModifiers == 0 && !keyguardOn) {
            Int32 modifiers;
            event->GetModifiers(&modifiers);
            Int32 shiftlessModifiers = modifiers & ~IKeyEvent::META_SHIFT_MASK;
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            Boolean bval1, bval2;
            if ((helper->MetaStateHasModifiers(shiftlessModifiers, IKeyEvent::META_ALT_ON, &bval1), bval1)
                || (helper->MetaStateHasModifiers(shiftlessModifiers, IKeyEvent::META_META_ON, &bval2), bval2)) {
                mRecentAppsDialogHeldModifiers = shiftlessModifiers;
                ShowOrHideRecentAppsDialog(RECENT_APPS_BEHAVIOR_EXIT_TOUCH_MODE_AND_SHOW);
                return -1;
            }
        }
    }
    else if (!down && mRecentAppsDialogHeldModifiers != 0
        && (metaState & mRecentAppsDialogHeldModifiers) == 0) {
        mRecentAppsDialogHeldModifiers = 0;
        ShowOrHideRecentAppsDialog(keyguardOn ? RECENT_APPS_BEHAVIOR_DISMISS :
            RECENT_APPS_BEHAVIOR_DISMISS_AND_SWITCH);
    }

    // // Handle keyboard language switching.
    if (down && repeatCount == 0
            && (keyCode == IKeyEvent::KEYCODE_LANGUAGE_SWITCH
                    || (keyCode == IKeyEvent::KEYCODE_SPACE
                            && (metaState & IKeyEvent::META_CTRL_MASK) != 0))) {
        Int32 direction = (metaState & IKeyEvent::META_SHIFT_MASK) != 0 ? -1 : 1;
        Int32 deviceId = 0;
        mWindowManagerFuncs->SwitchKeyboardLayout((event->GetDeviceId(&deviceId), deviceId), direction);
        return -1;
    }
    if (mLanguageSwitchKeyPressed && !down
            && (keyCode == IKeyEvent::KEYCODE_LANGUAGE_SWITCH
                    || keyCode == IKeyEvent::KEYCODE_SPACE)) {
        mLanguageSwitchKeyPressed = FALSE;
        return -1;
    }

    // Let the application handle the key.
    return 0;
}

ECode CPhoneWindowManager::DispatchUnhandledKey(
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
                    (event->GetDownTime(&downTime), downTime), (event->GetEventTime(&eventTime), eventTime),
                    action, ((CKeyCharacterMap::FallbackAction*)fallbackAction.Get())->mKeyCode,
                    repeatCount, ((CKeyCharacterMap::FallbackAction*)fallbackAction.Get())->mMetaState,
                    (event->GetDeviceId(&deviceId), deviceId), (event->GetScanCode(&scanCode), scanCode),
                    flags, (event->GetSource(&source), source), String(NULL), (IKeyEvent**)&fallbackEvent);

            if (!InterceptFallback(win, fallbackEvent, policyFlags)) {
                fallbackEvent->Recycle();
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

Boolean CPhoneWindowManager::InterceptFallback(
    /* [in] */ IWindowState* win,
    /* [in] */ IKeyEvent* fallbackEvent,
    /* [in] */ Int32 policyFlags)
{
    Int32 actions = 0;
    InterceptKeyBeforeQueueing(fallbackEvent, policyFlags, TRUE, &actions);
    if ((actions & ACTION_PASS_TO_USER) != 0) {
        Int64 delayMillis = 0;
        InterceptKeyBeforeDispatching(
                win, fallbackEvent, policyFlags, &delayMillis);
        if (delayMillis == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void CPhoneWindowManager::LaunchAssistLongPressAction()
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

void CPhoneWindowManager::LaunchAssistAction()
{
    SendCloseSystemWindows(SYSTEM_DIALOG_REASON_ASSIST);
    AutoPtr<ISearchManager> searchManager;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&searchManager);
    AutoPtr<IIntent> intent;
    searchManager->GetAssistIntent(mContext, IUserHandle::USER_CURRENT, (IIntent**)&intent);

    if (intent != NULL) {
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_SINGLE_TOP
                | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
        // try {
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
        mContext->StartActivityAsUser(intent, userHandle);
        // } catch (ActivityNotFoundException e) {
        //     Slog.d(TAG, "No activity to handle assist action.", e);
        // }
    }
}

AutoPtr<ISearchManager> CPhoneWindowManager::GetSearchManager()
{
    if (mSearchManager == NULL) {
        mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&mSearchManager);
    }
    return mSearchManager;
}

void CPhoneWindowManager::LaunchHomeFromHotKey()
{
    // TODO: keyguardMediator is not implement.
    // if (mKeyguardMediator != NULL && mKeyguardMediator.isShowingAndNotHidden()) {
    //     // don't launch home if keyguard showing
    // } else if (!mHideLockScreen && mKeyguardMediator.isInputRestricted()) {
    //     // when in keyguard restricted mode, must first verify unlock
    //     // before launching home
    //     mKeyguardMediator.verifyUnlock(new OnKeyguardExitResult() {
    //         public void onKeyguardExitResult(boolean success) {
    //             if (success) {
    //                 try {
    //                     ActivityManagerNative.getDefault().stopAppSwitches();
    //                 } catch (RemoteException e) {
    //                 }
    //                 sendCloseSystemWindows(SYSTEM_DIALOG_REASON_HOME_KEY);
    //                 startDockOrHome();
    //             }
    //         }
    //     });
    // } else
     {
        // no keyguard stuff to worry about, just launch home!
        // try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->StopAppSwitches();
        // } catch (RemoteException e) {
        // }
        SendCloseSystemWindows(SYSTEM_DIALOG_REASON_HOME_KEY);
        StartDockOrHome();
    }
}

ECode CPhoneWindowManager::AdjustSystemUiVisibilityLw(
    /* [in] */ Int32 visibility,
    /* [out] */ Int32* newVisibility)
{
    assert(newVisibility != NULL);
    // Reset any bits in mForceClearingStatusBarVisibility that
    // are now clear.
    mResettingSystemUiFlags &= visibility;
    // Clear any bits in the new visibility that are currently being
    // force cleared, before reporting it.
    *newVisibility = visibility & ~mResettingSystemUiFlags
            & ~mForceClearedSystemUiFlags;

    return NOERROR;
}

ECode CPhoneWindowManager::GetContentInsetHintLw(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ IRect* contentInset)
{
    assert(attrs != NULL);
    Int32 fl = 0;
    attrs->GetFlags(&fl);

    Int32 systemUiVisibility = 0;
    attrs->GetSystemUiVisibility(&systemUiVisibility);
    Int32 subtreeSystemUiVisibility = 0;
    attrs->GetSubtreeSystemUiVisibility(&subtreeSystemUiVisibility);

    systemUiVisibility = systemUiVisibility | subtreeSystemUiVisibility;

    if ((fl & (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR))
            == (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR)) {
        Int32 availRight = 0, availBottom = 0;
        if (mCanHideNavigationBar &&
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
        } else if ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0) {
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

ECode CPhoneWindowManager::BeginLayoutLw(
    /* [in] */ Boolean isDefaultDisplay,
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 displayHeight,
    /* [in] */ Int32 displayRotation)
{
    mUnrestrictedScreenLeft = mUnrestrictedScreenTop = 0;
    mUnrestrictedScreenWidth = displayWidth;
    mUnrestrictedScreenHeight = displayHeight;
    mRestrictedScreenLeft = mRestrictedScreenTop = 0;
    mRestrictedScreenWidth = displayWidth;
    mRestrictedScreenHeight = displayHeight;
    mDockLeft = mContentLeft = mStableLeft = mStableFullscreenLeft
            = mSystemLeft = mCurLeft = 0;
    mDockTop = mContentTop = mStableTop = mStableFullscreenTop
            = mSystemTop = mCurTop = 0;
    mDockRight = mContentRight = mStableRight = mStableFullscreenRight
            = mSystemRight = mCurRight = displayWidth;
    mDockBottom = mContentBottom = mStableBottom = mStableFullscreenBottom
            = mSystemBottom = mCurBottom = displayHeight;
    mDockLayer = 0x10000000;
    mStatusBarLayer = -1;

    // start with the current dock rect, which will be (0,0,displayWidth,displayHeight)
    AutoPtr<CRect> pf = mTmpParentFrame;
    AutoPtr<CRect> df = mTmpDisplayFrame;
    AutoPtr<CRect> vf = mTmpVisibleFrame;
    pf->mLeft = df->mLeft = vf->mLeft = mDockLeft;
    pf->mTop = df->mTop = vf->mTop = mDockTop;
    pf->mRight = df->mRight = vf->mRight = mDockRight;
    pf->mBottom = df->mBottom = vf->mBottom = mDockBottom;

    if (isDefaultDisplay) {
        // For purposes of putting out fake window up to steal focus, we will
        // drive nav being hidden only by whether it is requested.
        Boolean navVisible = (((mLastSystemUiFlags & IView::SYSTEM_UI_FLAG_FULLSCREEN) == 0) && (mTopIsFullscreen == FALSE))||(mFullScreenIsEnable == FALSE);

        if (mAlwaysFullScreen == TRUE) {
            navVisible = FALSE;
        }

        // When the navigation bar isn't visible, we put up a fake
        // input window to catch all touch events.  This way we can
        // detect when the user presses anywhere to bring back the nav
        // bar and ensure the application doesn't see the event.
        if (FALSE) {
            if (navVisible) {
                if (mHideNavFakeWindow != NULL) {
                    mHideNavFakeWindow->Dismiss();
                    mHideNavFakeWindow = NULL;
                }
            } else if (mHideNavFakeWindow == NULL) {
                AutoPtr<ILooper> looper;
                mHandler->GetLooper((ILooper**)&looper);

                mWindowManagerFuncs->AddFakeWindow(
                        looper, mHideNavInputEventReceiverFactory,
                        String("hidden nav"), IWindowManagerLayoutParams::TYPE_HIDDEN_NAV_CONSUMER,
                        0, FALSE, FALSE, TRUE, (IFakeWindow**)&mHideNavFakeWindow);
            }
        }
        else {
            if (mHideNavFakeWindow != NULL) {
                mHideNavFakeWindow->Dismiss();
                mHideNavFakeWindow = NULL;
            }
        }

        // For purposes of positioning and showing the nav bar, if we have
        // decided that it can't be hidden (because of the screen aspect ratio),
        // then take that into account.
        navVisible |= !mCanHideNavigationBar;

        if (mNavigationBar != NULL) {
            // Force the navigation bar to its appropriate place and
            // size.  We need to do this directly, instead of relying on
            // it to bubble up from the nav bar, because this needs to
            // change atomically with screen rotations.
            mNavigationBarOnBottom = (!mNavigationBarCanMove || displayWidth < displayHeight);
            if (mNavigationBarOnBottom) {
                // It's a system nav bar or a portrait screen; nav bar goes on bottom.
                Int32 top = displayHeight - mNavigationBarHeightForRotation[displayRotation];
                mTmpNavigationFrame->Set(0, top, displayWidth, displayHeight);
                mStableBottom = mStableFullscreenBottom = mTmpNavigationFrame->mTop;
                if (navVisible) {
                    Boolean canShown = FALSE;
                    mNavigationBar->ShowLw(TRUE, &canShown);
                    mDockBottom = mTmpNavigationFrame->mTop;
                    mRestrictedScreenHeight = mDockBottom - mDockTop;
                } else {
                    // We currently want to hide the navigation UI.
                    Boolean canHidden = FALSE;
                    mNavigationBar->HideLw(TRUE, &canHidden);
                }

                Boolean isAni = FALSE;
                if (navVisible && !(mNavigationBar->IsAnimatingLw(&isAni), isAni)) {
                    // If the nav bar is currently requested to be visible,
                    // and not in the process of animating on or off, then
                    // we can tell the app that it is covered by it.
                    mSystemBottom = mTmpNavigationFrame->mTop;
                }
            } else {
                // Landscape screen; nav bar goes to the right.
                Int32 left = displayWidth - mNavigationBarWidthForRotation[displayRotation];
                mTmpNavigationFrame->Set(left, 0, displayWidth, displayHeight);
                mStableRight = mStableFullscreenRight = mTmpNavigationFrame->mLeft;

                Boolean tmp = FALSE;
                if (navVisible) {
                    mNavigationBar->ShowLw(TRUE, &tmp);
                    mDockRight = mTmpNavigationFrame->mLeft;
                    mRestrictedScreenWidth = mDockRight - mDockLeft;
                } else {
                    // We currently want to hide the navigation UI.
                    mNavigationBar->HideLw(TRUE, &tmp);
                }

                Boolean isAni = FALSE;
                if (navVisible && !(mNavigationBar->IsAnimatingLw(&isAni), isAni)) {
                    // If the nav bar is currently requested to be visible,
                    // and not in the process of animating on or off, then
                    // we can tell the app that it is covered by it.
                    mSystemRight = mTmpNavigationFrame->mLeft;
                }
            }
            // Make sure the content and current rectangles are updated to
            // account for the restrictions from the navigation bar.
            mContentTop = mCurTop = mDockTop;
            mContentBottom = mCurBottom = mDockBottom;
            mContentLeft = mCurLeft = mDockLeft;
            mContentRight = mCurRight = mDockRight;
            mNavigationBar->GetSurfaceLayer(&mStatusBarLayer);
            // And compute the final frame.
            mNavigationBar->ComputeFrameLw(mTmpNavigationFrame, mTmpNavigationFrame,
                    mTmpNavigationFrame, mTmpNavigationFrame);
            // if (DEBUG_LAYOUT) Log.i(TAG, "mNavigationBar frame: " + mTmpNavigationFrame);
        }
        // if (DEBUG_LAYOUT) Log.i(TAG, String.format("mDock rect: (%d,%d - %d,%d)",
        //         mDockLeft, mDockTop, mDockRight, mDockBottom));

        // decide where the status bar goes ahead of time
        if (mStatusBar != NULL) {
            // apply any navigation bar insets
            pf->mLeft = df->mLeft = mUnrestrictedScreenLeft;
            pf->mTop = df->mTop = mUnrestrictedScreenTop;
            pf->mRight = df->mRight = mUnrestrictedScreenWidth - mUnrestrictedScreenLeft;
            pf->mBottom = df->mBottom = mUnrestrictedScreenHeight - mUnrestrictedScreenTop;
            vf->mLeft = mStableLeft;
            vf->mTop = mStableTop;
            vf->mRight = mStableRight;
            vf->mBottom = mStableBottom;

            mStatusBar->GetSurfaceLayer(&mStatusBarLayer);

            // Let the status bar determine its size.
            mStatusBar->ComputeFrameLw(pf, df, vf, vf);

            // For layout, the status bar is always at the top with our fixed height.
            mStableTop = mUnrestrictedScreenTop + mStatusBarHeight;

            // If the status bar is hidden, we don't want to cause
            // windows behind it to scroll.
            Boolean isv = FALSE;
            if (mStatusBar->IsVisibleLw(&isv), isv) {
                // Status bar may go away, so the screen area it occupies
                // is available to apps but just covering them when the
                // status bar is visible.
                mDockTop = mUnrestrictedScreenTop + mStatusBarHeight;

                mContentTop = mCurTop = mDockTop;
                mContentBottom = mCurBottom = mDockBottom;
                mContentLeft = mCurLeft = mDockLeft;
                mContentRight = mCurRight = mDockRight;

                // if (DEBUG_LAYOUT) Log.v(TAG, "Status bar: " +
                //     String.format(
                //         "dock=[%d,%d][%d,%d] content=[%d,%d][%d,%d] cur=[%d,%d][%d,%d]",
                //         mDockLeft, mDockTop, mDockRight, mDockBottom,
                //         mContentLeft, mContentTop, mContentRight, mContentBottom,
                //         mCurLeft, mCurTop, mCurRight, mCurBottom));
            }

            Boolean isAni = FALSE;
            if (isv && !(mStatusBar->IsAnimatingLw(&isAni), isAni)) {
                // If the status bar is currently requested to be visible,
                // and not in the process of animating on or off, then
                // we can tell the app that it is covered by it.
                mSystemTop = mUnrestrictedScreenTop + mStatusBarHeight;
            }
        }
    }

    return NOERROR;
}

ECode CPhoneWindowManager::GetSystemDecorRectLw(
    /* [in] */ IRect* _systemRect,
    /* [out] */ Int32* layer)
{
    assert(layer != NULL);
    *layer = 0;

    CRect* systemRect = (CRect*)_systemRect;
    systemRect->mLeft = mSystemLeft;
    systemRect->mTop = mSystemTop;
    systemRect->mRight = mSystemRight;
    systemRect->mBottom = mSystemBottom;
    if (mStatusBar != NULL) return mStatusBar->GetSurfaceLayer(layer);
    if (mNavigationBar != NULL) return mNavigationBar->GetSurfaceLayer(layer);
    return NOERROR;
}

void CPhoneWindowManager::SetAttachedWindowFrames(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 fl,
    /* [in] */ Int32 adjust,
    /* [in] */ IWindowState* attached,
    /* [in] */ Boolean insetDecors,
    /* [in] */ IRect* _pf,
    /* [in] */ IRect* _df,
    /* [in] */ IRect* _cf,
    /* [in] */ IRect* _vf)
{
    CRect* pf = (CRect*)_pf;
    CRect* df = (CRect*)_df;
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
       df->mLeft = cf->mLeft = vf->mLeft = mDockLeft;
       df->mTop = cf->mTop = vf->mTop = mDockTop;
       df->mRight = cf->mRight = vf->mRight = mDockRight;
       df->mBottom = cf->mBottom = vf->mBottom = mDockBottom;
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
            attached->GetDisplayFrameLw((IRect**)&tempRc);
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
            if (attackedLayer < mDockLayer) {
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

void CPhoneWindowManager::ApplyStableConstraints(
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

ECode CPhoneWindowManager::LayoutWindowLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ IWindowState* attached)
{
    // we've already done the status bar
    if (win == mStatusBar || win == mNavigationBar) {
        return NOERROR;
    }

    Boolean isDefaultDisplay = FALSE;
    win->IsDefaultDisplay(&isDefaultDisplay);

    Boolean needsToOffsetInputMethodTarget = isDefaultDisplay &&
            (win == mLastInputMethodTargetWindow && mLastInputMethodWindow != NULL);
    if (needsToOffsetInputMethodTarget) {
        // if (DEBUG_LAYOUT) {
        //     Slog.i(TAG, "Offset ime target window by the last ime window state");
        // }
        OffsetInputMethodWindowLw(mLastInputMethodWindow);
    }
    Int32 fl = 0;
    attrs->GetFlags(&fl);
    Int32 sim = 0;
    attrs->GetSoftInputMode(&sim);

    Int32 sysUiFl = 0;
    win->GetSystemUiVisibility(&sysUiFl);

    AutoPtr<CRect> pf = mTmpParentFrame;
    AutoPtr<CRect> df = mTmpDisplayFrame;
    AutoPtr<CRect> cf = mTmpContentFrame;
    AutoPtr<CRect> vf = mTmpVisibleFrame;

    Boolean visible = FALSE;
    Boolean hasNavBar = (isDefaultDisplay && mHasNavigationBar
            && mNavigationBar != NULL && (mNavigationBar->IsVisibleLw(&visible), visible));

    const Int32 adjust = sim & IWindowManagerLayoutParams::SOFT_INPUT_MASK_ADJUST;

    Int32 type = 0;
    attrs->GetType(&type);
    if (!isDefaultDisplay) {
        if (attached != NULL) {
            // If this window is attached to another, our display
            // frame is the same as the one we are attached to.
            SetAttachedWindowFrames(win, fl, adjust, attached, TRUE, pf, df, cf, vf);
        } else {
            // Give the window full screen.
            pf->mLeft = df->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
            pf->mTop = df->mTop = cf->mTop = mUnrestrictedScreenTop;
            pf->mRight = df->mRight = cf->mRight
                    = mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
            pf->mBottom = df->mBottom = cf->mBottom
                    = mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
        }
    } else if (type == IWindowManagerLayoutParams::TYPE_INPUT_METHOD) {
        pf->mLeft = df->mLeft = cf->mLeft = vf->mLeft = mDockLeft;
        pf->mTop = df->mTop = cf->mTop = vf->mTop = mDockTop;
        pf->mRight = df->mRight = cf->mRight = vf->mRight = mDockRight;
        pf->mBottom = df->mBottom = cf->mBottom = vf->mBottom = mDockBottom;
        // IM dock windows always go to the bottom of the screen.
        attrs->SetGravity(IGravity::BOTTOM);
        win->GetSurfaceLayer(&mDockLayer);
    } else {
        if ((fl & (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN |
                    IWindowManagerLayoutParams::FLAG_FULLSCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR))
                == (IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR)
                && (sysUiFl & IView::SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
            // if (DEBUG_LAYOUT)
            //     Log.v(TAG, "layoutWindowLw(" + attrs.getTitle()
            //             + "): IN_SCREEN, INSET_DECOR, !FULLSCREEN");
            // This is the case for a normal activity window: we want it
            // to cover all of the screen space, and it can take care of
            // moving its contents to account for screen decorations that
            // intrude into that space.
            if (attached != NULL) {
                // If this window is attached to another, our display
                // frame is the same as the one we are attached to.
                SetAttachedWindowFrames(win, fl, adjust, attached, TRUE, pf, df, cf, vf);
            } else {
                if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                        || type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL) {
                    // Status bar panels are the only windows who can go on top of
                    // the status bar.  They are protected by the STATUS_BAR_SERVICE
                    // permission, so they have the same privileges as the status
                    // bar itself.
                    //
                    // However, they should still dodge the navigation bar if it exists.

                    pf->mLeft = df->mLeft = hasNavBar ? mDockLeft : mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = hasNavBar
                                        ? mRestrictedScreenLeft + mRestrictedScreenWidth
                                        : mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = hasNavBar
                                          ? mRestrictedScreenTop + mRestrictedScreenHeight
                                          : mUnrestrictedScreenTop + mUnrestrictedScreenHeight;

                    // if (DEBUG_LAYOUT) {
                    //     Log.v(TAG, String.format(
                    //                 "Laying out status bar window: (%d,%d - %d,%d)",
                    //                 pf.left, pf.top, pf.right, pf.bottom));
                    }
                else if (mCanHideNavigationBar
                        && (sysUiFl & IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) != 0
                        && type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                        && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
                    // Asking for layout as if the nav bar is hidden, lets the
                    // application extend into the unrestricted screen area.  We
                    // only do this for application windows to ensure no window that
                    // can be above the nav bar can do this.
                    pf->mLeft = df->mLeft = mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
                } else {
                    pf->mLeft = df->mLeft = mRestrictedScreenLeft;
                    pf->mTop = df->mTop = mRestrictedScreenTop;
                    pf->mRight = df->mRight = mRestrictedScreenLeft+mRestrictedScreenWidth;
                    pf->mBottom = df->mBottom = mRestrictedScreenTop+mRestrictedScreenHeight;
                }

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
            //     Log.v(TAG, "layoutWindowLw(" + attrs.getTitle() + "): IN_SCREEN");
            // A window that has requested to fill the entire screen just
            // gets everything, period.
            if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL
                    || type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL) {
                pf->mLeft = df->mLeft = cf->mLeft = hasNavBar ? mDockLeft : mUnrestrictedScreenLeft;
                pf->mTop = df->mTop = cf->mTop = mUnrestrictedScreenTop;
                pf->mRight = df->mRight = cf->mRight = hasNavBar
                                    ? mRestrictedScreenLeft + mRestrictedScreenWidth
                                    : mUnrestrictedScreenLeft + mUnrestrictedScreenWidth;
                pf->mBottom = df->mBottom = cf->mBottom = hasNavBar
                                      ? mRestrictedScreenTop + mRestrictedScreenHeight
                                      : mUnrestrictedScreenTop + mUnrestrictedScreenHeight;
                // if (DEBUG_LAYOUT) {
                //     Log.v(TAG, String.format(
                //                 "Laying out IN_SCREEN status bar window: (%d,%d - %d,%d)",
                //                 pf.left, pf.top, pf.right, pf.bottom));
                // }
            } else if (type == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR
                    || type == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL) {
                // The navigation bar has Real Ultimate Power.
                pf->mLeft = df->mLeft = mUnrestrictedScreenLeft;
                pf->mTop = df->mTop = mUnrestrictedScreenTop;
                pf->mRight = df->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                pf->mBottom = df->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
                // if (DEBUG_LAYOUT) {
                //     Log.v(TAG, String.format(
                //                 "Laying out navigation bar window: (%d,%d - %d,%d)",
                //                 pf.left, pf.top, pf.right, pf.bottom));
                // }
            } else if ((type == IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY
                            || type == IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS)
                    && ((fl & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0)) {
                // Fullscreen secure system overlays get what they ask for.
                pf->mLeft = df->mLeft = mUnrestrictedScreenLeft;
                pf->mTop = df->mTop = mUnrestrictedScreenTop;
                pf->mRight = df->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                pf->mBottom = df->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
            } else if (type == IWindowManagerLayoutParams::TYPE_BOOT_PROGRESS
                    || type == IWindowManagerLayoutParams::TYPE_UNIVERSE_BACKGROUND) {
                // Boot progress screen always covers entire display.
                pf->mLeft = df->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
                pf->mTop = df->mTop = cf->mTop = mUnrestrictedScreenTop;
                pf->mRight = df->mRight = cf->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                pf->mBottom = df->mBottom = cf->mBottom
                        = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
            } else if (mCanHideNavigationBar
                    && (sysUiFl & IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION) != 0
                    && type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                    && type <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
                // Asking for layout as if the nav bar is hidden, lets the
                // application extend into the unrestricted screen area.  We
                // only do this for application windows to ensure no window that
                // can be above the nav bar can do this.
                // XXX This assumes that an app asking for this will also
                // ask for layout in only content.  We can't currently figure out
                // what the screen would be if only laying out to hide the nav bar.
                pf->mLeft = df->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
                pf->mTop = df->mTop = cf->mTop = mUnrestrictedScreenTop;
                pf->mRight = df->mRight = cf->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                pf->mBottom = df->mBottom = cf->mBottom
                        = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;
            } else {
                pf->mLeft = df->mLeft = cf->mLeft = mRestrictedScreenLeft;
                pf->mTop = df->mTop = cf->mTop = mRestrictedScreenTop;
                pf->mRight = df->mRight = cf->mRight = mRestrictedScreenLeft+mRestrictedScreenWidth;
                pf->mBottom = df->mBottom = cf->mBottom
                        = mRestrictedScreenTop+mRestrictedScreenHeight;

                // Log.d(TAG, "mFullScreenIsEnable = " + mFullScreenIsEnable + ", mAlwaysFullScreen = " + mAlwaysFullScreen);
                if (mFullScreenIsEnable || mAlwaysFullScreen) {  //mFullScreenIsEnable && mStatusBarWillHide && (mStatusBarReqShow == false)
                    pf->mLeft = df->mLeft = cf->mLeft = mUnrestrictedScreenLeft;
                    pf->mTop = df->mTop = cf->mTop = mUnrestrictedScreenTop;
                    pf->mRight = df->mRight = cf->mRight = mUnrestrictedScreenLeft+mUnrestrictedScreenWidth;
                    pf->mBottom = df->mBottom = cf->mBottom = mUnrestrictedScreenTop+mUnrestrictedScreenHeight;

                    // if (DEBUG_LAYOUT) {
                    //     Logger::V(TAG, "Laying out IN_SCREEN5 status bar window: (%d,%d - %d,%d)",
                    //         pf.left, pf.top, pf.right, pf.bottom));
                    // }
                }
                else {
                    pf->mLeft = df->mLeft = cf->mLeft = mRestrictedScreenLeft;
                    pf->mTop = df->mTop = cf->mTop = mRestrictedScreenTop;
                    pf->mRight = df->mRight = cf->mRight = mRestrictedScreenLeft+mRestrictedScreenWidth;
                    pf->mBottom = df->mBottom = cf->mBottom = mRestrictedScreenTop+mRestrictedScreenHeight;

                    // if (DEBUG_LAYOUT) {
                    //     Logger::V(TAG, "Laying out IN_SCREEN6 status bar window: (%d,%d - %d,%d)",
                    //         pf.left, pf.top, pf.right, pf.bottom));
                    // }
                }
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
            //     Log.v(TAG, "layoutWindowLw(" + attrs.getTitle() + "): attached to " + attached);
            // A child window should be placed inside of the same visible
            // frame that its parent had.
            SetAttachedWindowFrames(win, fl, adjust, attached, FALSE, pf, df, cf, vf);
        } else {
            // if (DEBUG_LAYOUT)
            //     Log.v(TAG, "layoutWindowLw(" + attrs.getTitle() + "): normal window");
            // Otherwise, a normal window must be placed inside the content
            // of all screen decorations.
            if (type == IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL) {
                // Status bar panels are the only windows who can go on top of
                // the status bar.  They are protected by the STATUS_BAR_SERVICE
                // permission, so they have the same privileges as the status
                // bar itself.
                pf->mLeft = df->mLeft = cf->mLeft = mRestrictedScreenLeft;
                pf->mTop = df->mTop = cf->mTop = mRestrictedScreenTop;
                pf->mRight = df->mRight = cf->mRight = mRestrictedScreenLeft + mRestrictedScreenWidth;
                pf->mBottom = df->mBottom = cf->mBottom
                        = mRestrictedScreenTop + mRestrictedScreenHeight;
            } else {
                pf->mLeft = mContentLeft;
                pf->mTop = mContentTop;
                pf->mRight = mContentRight;
                pf->mBottom = mContentBottom;
                if (adjust != IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE) {
                    df->mLeft = cf->mLeft = mDockLeft;
                    df->mTop = cf->mTop = mDockTop;
                    df->mRight = cf->mRight = mDockRight;
                    df->mBottom = cf->mBottom = mDockBottom;
                } else {
                    df->mLeft = cf->mLeft = mContentLeft;
                    df->mTop = cf->mTop = mContentTop;
                    df->mRight = cf->mRight = mContentRight;
                    df->mBottom = cf->mBottom = mContentBottom;
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

    if ((fl & IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS) != 0) {
        df->mLeft = df->mTop = cf->mLeft = cf->mTop = vf->mLeft = vf->mTop = -10000;
        df->mRight = df->mBottom = cf->mRight = cf->mBottom = vf->mRight = vf->mBottom = 10000;
    }

    // if (DEBUG_LAYOUT) Log.v(TAG, "Compute frame " + attrs.getTitle()
    //         + ": sim=#" + Integer.toHexString(sim)
    //         + " attach=" + attached + " type=" + attrs.type
    //         + String.format(" flags=0x%08x", fl)
    //         + " pf=" + pf.toShortString() + " df=" + df.toShortString()
    //         + " cf=" + cf.toShortString() + " vf=" + vf.toShortString());

    win->ComputeFrameLw(pf, df, cf, vf);

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

    return NOERROR;
}

void CPhoneWindowManager::OffsetInputMethodWindowLw(
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
    // if (DEBUG_LAYOUT) Log.v(TAG, "Input method: mDockBottom="
    //         + mDockBottom + " mContentBottom="
    //         + mContentBottom + " mCurBottom=" + mCurBottom);
}

ECode CPhoneWindowManager::FinishLayoutLw()
{
    return NOERROR;
}

ECode CPhoneWindowManager::BeginPostLayoutPolicyLw(
    /* [in] */ Int32 displayWidth,
    /* [in] */ Int32 displayHeight)
{
    mTopFullscreenOpaqueWindowState = NULL;
    mForceStatusBar = FALSE;
    mForceStatusBarFromKeyguard = FALSE;
    mForcingShowNavBar = FALSE;
    mForcingShowNavBarLayer = -1;

    mHideLockScreen = FALSE;
    mAllowLockscreenWhenOn = FALSE;
    mDismissKeyguard = DISMISS_KEYGUARD_NONE;
    mShowingLockscreen = FALSE;
    mShowingDream = FALSE;
    return NOERROR;
}

ECode CPhoneWindowManager::ApplyPostLayoutPolicyLw(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    // if (DEBUG_LAYOUT) Slog.i(TAG, "Win " + win + ": isVisibleOrBehindKeyguardLw="
    //         + win.isVisibleOrBehindKeyguardLw());
    Boolean tmp = FALSE;

    AutoPtr<IWindowManagerLayoutParams> winAttrs;
    win->GetAttrs((IWindowManagerLayoutParams**)&winAttrs);;
    Int32 privateFlags;
    winAttrs->GetPrivateFlags(&privateFlags);
    if (mTopFullscreenOpaqueWindowState == NULL
            && (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_FORCE_SHOW_NAV_BAR) != 0) {
        if (mForcingShowNavBarLayer < 0) {
            mForcingShowNavBar = TRUE;
            win->GetSurfaceLayer(&mForcingShowNavBarLayer);
        }
    }

    if (mTopFullscreenOpaqueWindowState == NULL &&
            (win->IsVisibleOrBehindKeyguardLw(&tmp), tmp)
            && !(win->IsGoneForLayoutLw(&tmp), tmp)) {

        Int32 flags = 0, type = 0;
        attrs->GetFlags(&flags);
        attrs->GetType(&type);
        if ((flags & IWindowManagerLayoutParams::FLAG_FORCE_NOT_FULLSCREEN) != 0) {
            if (type == IWindowManagerLayoutParams::TYPE_KEYGUARD) {
                mForceStatusBarFromKeyguard = TRUE;
            } else {
                mForceStatusBar = TRUE;
            }
        }

        if (type == IWindowManagerLayoutParams::TYPE_KEYGUARD) {
            mShowingLockscreen = TRUE;
        }

        Boolean applyWindow = type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
                && type <= IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW;
        if (type == IWindowManagerLayoutParams::TYPE_DREAM) {
            // If the lockscreen was showing when the dream started then wait
            // for the dream to draw before hiding the lockscreen.
            if (!mDreamingLockscreen
                    || ((win->IsVisibleLw(&tmp), tmp) && (win->HasDrawnLw(&tmp), tmp))) {
                mShowingDream = TRUE;
                applyWindow = TRUE;
            }
        }

        Int32 x = 0, y = 0, height = 0, width = 0;
        attrs->GetX(&x);
        attrs->GetY(&y);
        attrs->GetWidth(&width);
        attrs->GetHeight(&height);
        if (applyWindow
                && x == 0 && y == 0
                && width == IWindowManagerLayoutParams::MATCH_PARENT
                && height == IWindowManagerLayoutParams::MATCH_PARENT) {
            // if (DEBUG_LAYOUT) Log.v(TAG, "Fullscreen window: " + win);
            mTopFullscreenOpaqueWindowState = win;
            if ((flags & IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED) != 0) {
                // if (DEBUG_LAYOUT) Log.v(TAG, "Setting mHideLockScreen to TRUE by win " + win);
                mHideLockScreen = TRUE;
                mForceStatusBarFromKeyguard = FALSE;
            }
            if ((flags & IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD) != 0
                    && mDismissKeyguard == DISMISS_KEYGUARD_NONE) {
                // if (DEBUG_LAYOUT) Log.v(TAG, "Setting mDismissKeyguard to TRUE by win " + win);
                mDismissKeyguard = mWinDismissingKeyguard.Get() == win ?
                        DISMISS_KEYGUARD_CONTINUE : DISMISS_KEYGUARD_START;
                mWinDismissingKeyguard = win;
                mForceStatusBarFromKeyguard = FALSE;
            }
            if ((flags & IWindowManagerLayoutParams::FLAG_ALLOW_LOCK_WHILE_SCREEN_ON) != 0) {
                mAllowLockscreenWhenOn = TRUE;
            }
        }
    }

    return NOERROR;
}

ECode CPhoneWindowManager::FinishPostLayoutPolicyLw(
    /* [out] */ Int32* result)
{
    assert(result != NULL);
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
        if (DEBUG_LAYOUT) Logger::I(TAG, "force=%d forcefkg=%d top=%p",
            mForceStatusBar, mForceStatusBarFromKeyguard, mTopFullscreenOpaqueWindowState.Get());

        Boolean bval;
        if (mForceStatusBar || mForceStatusBarFromKeyguard) {
            if (DEBUG_LAYOUT) Logger::V(TAG, "Showing status bar: forced");
            mStatusBar->ShowLw(TRUE, &bval);
            if (bval) changes |= FINISH_LAYOUT_REDO_LAYOUT;
        }
        else if (mTopFullscreenOpaqueWindowState != NULL) {
            // if (localLOGV) {
            //     Logger::D(TAG, "frame: " + mTopFullscreenOpaqueWindowState.getFrameLw()
            //             + " shown frame: " + mTopFullscreenOpaqueWindowState.getShownFrameLw());
            //     Logger::D(TAG, "attr: " + mTopFullscreenOpaqueWindowState.getAttrs()
            //             + " lp.flags=0x" + Integer.toHexString(lp.flags));
            // }
            Int32 flags;
            lp->GetFlags(&flags);
            topIsFullscreen = (flags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0
                    || (mLastSystemUiFlags & IView::SYSTEM_UI_FLAG_FULLSCREEN) != 0;
            // The subtle difference between the window for mTopFullscreenOpaqueWindowState
            // and mTopIsFullscreen is that that mTopIsFullscreen is set only if the window
            // has the FLAG_FULLSCREEN set.  Not sure if there is another way that to be the
            // case though.
            if (topIsFullscreen) {
                if (DEBUG_LAYOUT) Logger::V(TAG, "** HIDING status bar");
                mStatusBar->HideLw(TRUE, &bval);
                if (bval) {
                    changes |= FINISH_LAYOUT_REDO_LAYOUT;

                    AutoPtr<IRunnable> runnable = new CollapsePanelsRunnable(this);
                    Boolean isSuccess = FALSE;
                    mHandler->Post(runnable, &isSuccess);
                } else if (DEBUG_LAYOUT) {
                    // Logger::V(TAG, "Preventing status bar from hiding by policy");
                }
            } else {
                if (DEBUG_LAYOUT) Logger::V(TAG, "** SHOWING status bar: top is not fullscreen");
                mStatusBar->ShowLw(TRUE, &bval);
                if (bval) changes |= FINISH_LAYOUT_REDO_LAYOUT;
            }
        }
    }
    else if (mNavigationBar != NULL) {
        // if (DEBUG_LAYOUT) {
        //     Logger::I(TAG, "mNavigationBar force=%d top=%d",
        //         mForceStatusBar, mTopFullscreenOpaqueWindowState);
        // }
        if (mForceStatusBar) {
            // if (DEBUG_LAYOUT) {
            //     Logger::V(TAG, "Showing status bar: forced");
            // }
            Boolean isShowLw = FALSE;
            if (mNavigationBar->ShowLw(TRUE, &isShowLw), isShowLw) {
                changes |= FINISH_LAYOUT_REDO_LAYOUT;
            }
        }
        else if (mTopFullscreenOpaqueWindowState != NULL) {
            // if (DEBUG_LAYOUT) {
            //     Log.v(TAG, "frame: " + mTopFullscreenOpaqueWindowState.getFrameLw()
            //             + " shown frame: " + mTopFullscreenOpaqueWindowState.getShownFrameLw());
            //     Log.v(TAG, "attr: " + mTopFullscreenOpaqueWindowState.getAttrs()
            //             + " lp.flags=0x" + Integer.toHexString(lp.flags));
            // }
            Int32 lpFlags;
            lp->GetFlags(&lpFlags);
            topIsFullscreen = (lpFlags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) != 0
                    || (mLastSystemUiFlags & IView::SYSTEM_UI_FLAG_FULLSCREEN) != 0;
        }
    }

    mTopIsFullscreen = topIsFullscreen;

    // TODO: KeyguardMediator is not implement.
    // // Hide the key guard if a visible window explicitly specifies that it wants to be
    // // displayed when the screen is locked.
    // if (mKeyguard != NULL) {
    //     // if (localLOGV) Log.v(TAG, "finishPostLayoutPolicyLw: mHideKeyguard="
    //     //         + mHideLockScreen);
    //     if (mDismissKeyguard != DISMISS_KEYGUARD_NONE && !mKeyguardMediator.isSecure()) {
    //         if (mKeyguard.hideLw(TRUE)) {
    //             changes |= FINISH_LAYOUT_REDO_LAYOUT
    //                     | FINISH_LAYOUT_REDO_CONFIG
    //                     | FINISH_LAYOUT_REDO_WALLPAPER;
    //         }
    //         if (mKeyguardMediator.isShowing()) {
    //             mHandler.post(new Runnable() {
    //                 @Override
    //                 public void run() {
    //                     mKeyguardMediator.keyguardDone(FALSE, FALSE);
    //                 }
    //             });
    //         }
    //     } else if (mHideLockScreen) {
    //         if (mKeyguard.hideLw(TRUE)) {
    //             changes |= FINISH_LAYOUT_REDO_LAYOUT
    //                     | FINISH_LAYOUT_REDO_CONFIG
    //                     | FINISH_LAYOUT_REDO_WALLPAPER;
    //         }
    //         mKeyguardMediator.setHidden(TRUE);
    //     } else if (mDismissKeyguard != DISMISS_KEYGUARD_NONE) {
    //         // This is the case of keyguard isSecure() and not mHideLockScreen.
    //         if (mDismissKeyguard == DISMISS_KEYGUARD_START) {
    //             // Only launch the next keyguard unlock window once per window.
    //             if (mKeyguard.showLw(TRUE)) {
    //                 changes |= FINISH_LAYOUT_REDO_LAYOUT
    //                         | FINISH_LAYOUT_REDO_CONFIG
    //                         | FINISH_LAYOUT_REDO_WALLPAPER;
    //             }
    //             mKeyguardMediator.setHidden(FALSE);
    //             mHandler.post(new Runnable() {
    //                 @Override
    //                 public void run() {
    //                     mKeyguardMediator.dismiss();
    //                 }
    //             });
    //         }
    //     } else {
    //         mWinDismissingKeyguard = NULL;
    //         if (mKeyguard.showLw(TRUE)) {
    //             changes |= FINISH_LAYOUT_REDO_LAYOUT
    //                     | FINISH_LAYOUT_REDO_CONFIG
    //                     | FINISH_LAYOUT_REDO_WALLPAPER;
    //         }
    //         mKeyguardMediator.setHidden(FALSE);
    //     }
    // }

    if ((UpdateSystemUiVisibilityLw()&SYSTEM_UI_CHANGING_LAYOUT) != 0) {
        // If the navigation bar has been hidden or shown, we need to do another
        // layout pass to update that window.
        changes |= FINISH_LAYOUT_REDO_LAYOUT;
    }

    // // update since mAllowLockscreenWhenOn might have changed
    UpdateLockScreenTimeout();
    *result = changes;
    return NOERROR;
}

ECode CPhoneWindowManager::AllowAppAnimationsLw(
    /* [out] */ Boolean* allowed)
{
    Boolean isVisibleLw, isAnimatingLw;
    if (mKeyguard != NULL && (mKeyguard->IsVisibleLw(&isVisibleLw), isVisibleLw)
        && !(mKeyguard->IsAnimatingLw(&isAnimatingLw), isAnimatingLw)) {
        // If keyguard is currently visible, no reason to animate
        // behind it.
        *allowed = FALSE;
    }
    else {
        *allowed = TRUE;
    }
    return NOERROR;
}

ECode CPhoneWindowManager::FocusChangedLw(
    /* [in] */ IWindowState* lastFocus,
    /* [in] */ IWindowState* newFocus,
    /* [out] */ Int32* state)
{
    assert(state != NULL);
    *state = 0;

    mFocusedWindow = newFocus;
    if ((UpdateSystemUiVisibilityLw() & SYSTEM_UI_CHANGING_LAYOUT) != 0) {
        // If the navigation bar has been hidden or shown, we need to do another
        // layout pass to update that window.
        *state = FINISH_LAYOUT_REDO_LAYOUT;
    }

    return NOERROR;
}

ECode CPhoneWindowManager::NotifyLidSwitchChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Boolean lidOpen)
{
    // do nothing if headless
    if (mHeadless) {
        return NOERROR;
    }

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
        if (KeyguardIsShowingTq()) {
            // TODO: KeyguardMediator is not implement.
            // mKeyguardMediator.onWakeKeyWhenKeyguardShowingTq(KeyEvent.KEYCODE_POWER);
        } else {
            mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
        }
    } else if (!mLidControlsSleep) {
        mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    }
    return NOERROR;
}

void CPhoneWindowManager::SetHdmiPlugged(
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

void CPhoneWindowManager::InitializeHdmiState()
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
            ec = reader->ReadChars(buf, &n);
            if (FAILED(ec)) {
                break;
            }
            if (n > 1) {
                Int32 value = 0;
                ec = StringUtils::ParseInt32(String(*buf, 0, n-1), &value);
                if (FAILED(ec)) {
                    break;
                }
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

Boolean CPhoneWindowManager::IsMusicActive()
{
    AutoPtr<IInterface> audioService;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(audioService);
    if (audioManager == NULL) {
       Logger::W(TAG, "isMusicActive: couldn't get AudioManager reference");
       return FALSE;
    }
    Boolean result;
    return audioManager->IsMusicActive(&result);
}

void CPhoneWindowManager::HandleVolumeKey(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 keycode)
{
    AutoPtr<IIAudioService> audioService = GetAudioService();
    if (audioService == NULL) {
        return;
    }

    // try {
    // since audio is playing, we shouldn't have to hold a wake lock
    // during the call, but we do it as a precaution for the rare possibility
    // that the music stops right before we call this
    // TODO: Actually handle MUTE.
    mBroadcastWakeLock->AcquireLock();
    ECode ec = audioService->AdjustStreamVolume(
        stream,
        keycode == IKeyEvent::KEYCODE_VOLUME_UP ?
            IAudioManager::ADJUST_RAISE : IAudioManager::ADJUST_LOWER,
        0);
    if (FAILED(ec)) {
        Logger::W(TAG, "IAudioService.adjustStreamVolume() threw RemoteException %08x", ec);
    }
    mBroadcastWakeLock->ReleaseLock();
    // } catch (RemoteException e) {
    //    Log.w(TAG, "IAudioService.adjustStreamVolume() threw RemoteException " + e);
    // } finally {
    //    mBroadcastWakeLock->ReleaseLock();
    // }
}

void CPhoneWindowManager::TakeScreenshot()
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
    if (mContext->BindService(
            intent, conn, IContext::BIND_AUTO_CREATE, IUserHandle::USER_CURRENT,
            &isSuccess), isSuccess) {
        mScreenshotConnection = conn;
        Boolean isSuccess = FALSE;
        mHandler->PostDelayed(mScreenshotTimeout, 10000, &isSuccess);
    }
}

Boolean CPhoneWindowManager::IsMute()
{
    AutoPtr<IIAudioService> audioService = GetAudioService();
    if (audioService == NULL) {
        return FALSE;
    }

    Boolean isMute;
    ECode ec = audioService->IsStreamMute(IAudioManager::STREAM_MUSIC, &isMute);
    if (FAILED(ec)) return FALSE;
    return isMute;
}

void CPhoneWindowManager::HandleMute()
{
    AutoPtr<IIAudioService> audioService = GetAudioService();
    if (audioService == NULL) {
        return;
    }

    // since audio is playing, we shouldn't have to hold a wake lock
    // during the call, but we do it as a precaution for the rare possibility
    // that the music stops right before we call this
    mBroadcastWakeLock->AcquireLock();
    ECode ec = audioService->SetStreamMute(IAudioManager::STREAM_MUSIC, !IsMute(), mCallback);
    if (FAILED(ec)) {
        Logger::W(TAG, "IAudioService.adjustStreamVolume() threw RemoteException %d", ec);
    }

    mBroadcastWakeLock->ReleaseLock();
}

ECode CPhoneWindowManager::InterceptKeyBeforeQueueing(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [in] */ Boolean isScreenOn,
    /* [out] */ Int32* bitwise)
{
    if (bitwise == NULL) {
        return E_INVALID_ARGUMENT;
    }

    Boolean shutdhownPending = SystemProperties::GetInt32(String("sys.start_shutdown"), 0) > 0 ? TRUE : FALSE;
    if (DEBUG_BOOTFAST) {
        Slogger::D(TAG, "mBootFastRuning = %d, shutdhownPending = %d", mBootFastRuning, shutdhownPending);
    }

    if (!mSystemBooted) {
        // If we have not yet booted, don't let key events do anything.
        *bitwise = 0;
        return NOERROR;
    }

    Int32 action = 0, keyCode = 0, flags = 0;
    event->GetAction(&action);
    event->GetKeyCode(&keyCode);
    event->GetFlags(&flags);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    Boolean canceled = FALSE;
    event->IsCanceled(&canceled);

    if (mBootFastEnable) {
        if (mBootFastRuning || shutdhownPending) {
            Slogger::D(TAG, "Shutdown is running or boot animation running");
            *bitwise = 0;
            return NOERROR;
        }
        Boolean isBootFastStatus = FALSE;
        mPowerManager->IsBootFastStatus(&isBootFastStatus);
        if (isBootFastStatus && keyCode != IKeyEvent::KEYCODE_POWER) {
            Slogger::D(TAG, "Not power key return");
            *bitwise = 0;
            return NOERROR;
        }

        if (keyCode == IKeyEvent::KEYCODE_POWER && isBootFastStatus) {
            Boolean isBootFastWake = FALSE;
            if (!(mPowerManager->IsBootFastWakeFromStandby(&isBootFastWake), isBootFastWake)) {
                if (down) {
                    Slogger::D(TAG, "Boot fast key down ");
                    InterceptBootFastPowerKeyDown(isScreenOn);
                    *bitwise = 0;
                    return NOERROR;
                } else {
                    Slogger::D(TAG, "boot fast key up");
                    InterceptBootFastPowerKeyUp(isScreenOn);
                    *bitwise = 0;
                    return NOERROR;
                }
            } else {
                SystemClock::Sleep(100); // wait for PowerManagerService check whether usb or ac connect
                Boolean isScreenOnNow = FALSE;
                if (down && !(mPowerManager->IsScreenOn(&isScreenOnNow), isScreenOnNow)) {
                    Slogger::D(TAG, "Wake from boot fast");
                    mHandler->RemoveCallbacks(mPowerLongPress);
                    mHandler->RemoveCallbacks(mBootFastPowerLongPress);
                    Boolean isSuccess = FALSE;
                    mHandler->Post(mBootFastPowerLongPress, &isSuccess);
                }
                else {
                    if (DEBUG_BOOTFAST) {
                        Slogger::D(TAG, "Not light screen");
                    }
                    *bitwise = 0;
                    return NOERROR;
                }
            }
        }
    }

    Boolean isInjected = (policyFlags & IWindowManagerPolicy::FLAG_INJECTED) != 0;

    // If screen is off then we treat the case where the keyguard is open but hidden
    // the same as if it were open and in front.
    // This will prevent any keys other than the power button from waking the screen
    // when the keyguard is hidden by another activity.
    // TODO: KeyguardMediator is not implement.
    // final boolean keyguardActive = (mKeyguardMediator == NULL ? FALSE :
    //                                     (isScreenOn ?
    //                                         mKeyguardMediator.isShowingAndNotHidden() :
    //                                         mKeyguardMediator.isShowing()));

    Boolean keyguardActive = FALSE;

    if (keyCode == IKeyEvent::KEYCODE_POWER) {
        policyFlags |= IWindowManagerPolicy::FLAG_WAKE;
    }
    Boolean isWakeKey = (policyFlags & (IWindowManagerPolicy::FLAG_WAKE
            | IWindowManagerPolicy::FLAG_WAKE_DROPPED)) != 0;

    // if (DEBUG_INPUT) {
    //     Log.d(TAG, "interceptKeyTq keycode=" + keyCode
    //             + " screenIsOn=" + isScreenOn + " keyguardActive=" + keyguardActive
    //             + " policyFlags=" + Integer.toHexString(policyFlags)
    //             + " isWakeKey=" + isWakeKey);
    // }

    Int32 repeat = 0;
    event->GetRepeatCount(&repeat);
    if (down && (policyFlags & IWindowManagerPolicy::FLAG_VIRTUAL) != 0 && repeat == 0) {
        Boolean isSucceed;
        PerformHapticFeedbackLw(NULL, IHapticFeedbackConstants::VIRTUAL_KEY, FALSE, &isSucceed);
    }

    // Basic policy based on screen state and keyguard.
    // FIXME: This policy isn't quite correct.  We shouldn't care whether the screen
    //        is on or off, really.  We should care about whether the device is in an
    //        interactive state or is in suspend pretending to be "off".
    //        The primary screen might be turned off due to proximity sensor or
    //        because we are presenting media on an auxiliary screen or remotely controlling
    //        the device some other way (which is why we have an exemption here for injected
    //        events).
    //
    Int32 result = 0;
    if ((isScreenOn && !mHeadless) || (isInjected && !isWakeKey)) {
        // When the screen is on or if the key is injected pass the key to the application.
        result = IWindowManagerPolicy::ACTION_PASS_TO_USER;
    }
    else {
        // When the screen is off and the key is not injected, determine whether
        // to wake the device but don't pass the key to the application.
        result = 0;

        if (down && isWakeKey && IsWakeKeyWhenScreenOff(keyCode)) {
            if (keyguardActive) {
                // If the keyguard is showing, let it wake the device when ready.
                // TODO: KeyguardMediator is not implement.
                //mKeyguardMediator.onWakeKeyWhenKeyguardShowingTq(keyCode);
            }
            else {
                // Otherwise, wake the device ourselves.
                result |= IWindowManagerPolicy::ACTION_WAKE_UP;
            }
        }
    }

    // Handle special keys.
    switch (keyCode) {
    case IKeyEvent::KEYCODE_VOLUME_DOWN:
    case IKeyEvent::KEYCODE_VOLUME_UP:
    case IKeyEvent::KEYCODE_VOLUME_MUTE:
        {
            if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN) {
                if (down) {
                    if (isScreenOn && !mVolumeDownKeyTriggered
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
                    if (isScreenOn && !mVolumeUpKeyTriggered
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
                //ITelephony telephonyService = GetTelephonyService();
                //if (telephonyService != NULL) {
                //    try {
                //        if (telephonyService.isRinging()) {
                //            // If an incoming call is ringing, either VOLUME key means
                //            // "silence ringer".  We handle these keys here, rather than
                //            // in the InCallScreen, to make sure we'll respond to them
                //            // even if the InCallScreen hasn't come to the foreground yet.
                //            // Look for the DOWN event here, to agree with the "fallback"
                //            // behavior in the InCallScreen.
                //            Log.i(TAG, "interceptKeyBeforeQueueing:"
                //                + " VOLUME key-down while ringing: Silence ringer!");

                //            // Silence the ringer.  (It's safe to call this
                //            // even if the ringer has already been silenced.)
                //            telephonyService.silenceRinger();

                //            // And *don't* pass this key thru to the current activity
                //            // (which is probably the InCallScreen.)
                //            result &= ~ACTION_PASS_TO_USER;
                //            break;
                //        }
                //        if (telephonyService.isOffhook()
                //            && (result & ACTION_PASS_TO_USER) == 0) {
                //                // If we are in call but we decided not to pass the key to
                //                // the application, handle the volume change here.
                //                handleVolumeKey(IAudioManager::STREAM_VOICE_CALL, keyCode);
                //                break;
                //        }
                //    } catch (RemoteException ex) {
                //        Log.w(TAG, "ITelephony threw RemoteException", ex);
                //    }
                //}

               /* add by Gary. start {{----------------------------------- */
                /* 2011-10-27 */
                /* support mute */
                if (IsMute()) {
                    HandleMute();
                }
                /* add by Gary. end   -----------------------------------}} */

                if (IsMusicActive() && (result
                    & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0) {
                    // If music is playing but we decided not to pass the key to the
                    // application, handle the volume change here.
                    HandleVolumeKey(IAudioManager::STREAM_MUSIC, keyCode);
                }
            }
        }
        break;

    /* add by Gary. start {{----------------------------------- */
    /* 2011-10-27 */
    /* support mute */
    case IKeyEvent::KEYCODE_MUTE: {
        // Log.d(TAG, "key Mute");
        if (down) {
            HandleMute();
        }
        break;
    }
    /* add by Gary. end   -----------------------------------}} */

    case IKeyEvent::KEYCODE_ENDCALL:
        {
            result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
            if (down) {
                //ITelephony telephonyService = getTelephonyService();
                Boolean hungUp = FALSE;
                //if (telephonyService != NULL) {
                //    try {
                //        hungUp = telephonyService.endCall();
                //    } catch (RemoteException ex) {
                //        Log.w(TAG, "ITelephony threw RemoteException", ex);
                //    }
                //}
                InterceptPowerKeyDown(!isScreenOn || hungUp);
            }
            else {
                if (InterceptPowerKeyUp(canceled)) {
                    if ((mEndcallBehavior
                        & 0x1/*Settings.System.END_BUTTON_BEHAVIOR_HOME*/) != 0) {
                        if (GoHome()) {
                            break;
                        }
                    }

                    if ((mEndcallBehavior
                        & 0x2/*Settings.System.END_BUTTON_BEHAVIOR_SLEEP*/) != 0) {
                        result = (result &
                            ~IWindowManagerPolicy::ACTION_WAKE_UP)
                            | IWindowManagerPolicy::ACTION_GO_TO_SLEEP;
                    }
                }
            }
        }
        break;
    case IKeyEvent::KEYCODE_POWER:
        {
            result &= ~IWindowManagerPolicy::ACTION_PASS_TO_USER;
            if (down) {
                Int32 flags = 0;
                if (isScreenOn && !mPowerKeyTriggered
                        && ((event->GetFlags(&flags), flags) & IKeyEvent::FLAG_FALLBACK) == 0) {
                    mPowerKeyTriggered = TRUE;
                    event->GetDownTime(&mPowerKeyTime);
                    InterceptScreenshotChord();
                }

                //ITelephony telephonyService = getTelephonyService();
                Boolean hungUp = FALSE;
                //if (telephonyService != NULL) {
                //    try {
                //        if (telephonyService.isRinging()) {
                //            // Pressing Power while there's a ringing incoming
                //            // call should silence the ringer.
                //            telephonyService.silenceRinger();
                //        } else if ((mIncallPowerBehavior
                //            & Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR_HANGUP) != 0
                //            && telephonyService.isOffhook()) {
                //                // Otherwise, if "Power button ends call" is enabled,
                //                // the Power button will hang up any current active call.
                //                hungUp = telephonyService.endCall();
                //        }
                //    } catch (RemoteException ex) {
                //        Log.w(TAG, "ITelephony threw RemoteException", ex);
                //    }
                //}
                InterceptPowerKeyDown(!isScreenOn || hungUp || mVolumeDownKeyTriggered || mVolumeUpKeyTriggered);
            }
            else {
                mPowerKeyTriggered = FALSE;
                CancelPendingScreenshotChordAction();

                if (InterceptPowerKeyUp(canceled || mPendingPowerKeyUpCanceled)) {
                    result = (result & ~IWindowManagerPolicy::ACTION_WAKE_UP)
                        | IWindowManagerPolicy::ACTION_GO_TO_SLEEP;
                }
                mPendingPowerKeyUpCanceled = FALSE;
            }
        }
        break;
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE: {
            // if (down) {
            //     ITelephony telephonyService = getTelephonyService();
            //     if (telephonyService != NULL) {
            //         try {
            //             if (!telephonyService.isIdle()) {
            //                 // Suppress PLAY/PAUSE toggle when phone is ringing or in-call
            //                 // to avoid music playback.
            //                 break;
            //             }
            //         } catch (RemoteException ex) {
            //             Log.w(TAG, "ITelephony threw RemoteException", ex);
            //         }
            //     }
            // }
        }
        break;
    case IKeyEvent::KEYCODE_HEADSETHOOK:
    // case IKeyEvent::KEYCODE_MUTE:
    case IKeyEvent::KEYCODE_MEDIA_STOP:
    case IKeyEvent::KEYCODE_MEDIA_NEXT:
    case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
    case IKeyEvent::KEYCODE_MEDIA_REWIND:
    case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        {
            if ((result & IWindowManagerPolicy::ACTION_PASS_TO_USER) == 0) {
                // Only do this if we would otherwise not pass it to the user. In that
                // case, the PhoneWindow class will do the same thing, except it will
                // only do it if the showing app doesn't process the key on its own.
                // Note that we need to make a copy of the key event here because the
                // original key event will be recycled when we return.
                mBroadcastWakeLock->AcquireLock();
                // TODO: Handler has not implement obtainMessage.
                // Message msg = mHandler.obtainMessage(MSG_DISPATCH_MEDIA_KEY_WITH_WAKE_LOCK,
                //         new KeyEvent(event));
                // msg.setAsynchronous(TRUE);
                // msg.sendToTarget();
            }
        }
        break;
    case IKeyEvent::KEYCODE_CALL: {
        if (down) {
                //ITelephony telephonyService = getTelephonyService();
                //if (telephonyService != NULL) {
                //    try {
                //        if (telephonyService.isRinging()) {
                //            Log.i(TAG, "interceptKeyBeforeQueueing:"
                //                + " CALL key-down while ringing: Answer the call!");
                //            telephonyService.answerRingingCall();

                //            // And *don't* pass this key thru to the current activity
                //            // (which is presumably the InCallScreen.)
                //            result &= ~ACTION_PASS_TO_USER;
                //        }
                //    } catch (RemoteException ex) {
                //        Log.w(TAG, "ITelephony threw RemoteException", ex);
                //    }
                //}
            // }
        }
        break;
    }
    default:
        break;
    }

    if (mBootFastRuning)
        result &= ~IWindowManagerPolicy::ACTION_GO_TO_SLEEP;

    *bitwise = result;
    return NOERROR;
}

Boolean CPhoneWindowManager::IsWakeKeyWhenScreenOff(
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
        case IKeyEvent::KEYCODE_CAMERA:
            return FALSE;
    }
    return TRUE;
}

ECode CPhoneWindowManager::InterceptMotionBeforeQueueingWhenScreenOff(
    /* [in] */ Int32 policyFlags,
    /* [out] */ Int32* result)
{
    assert(result != NULL);
    *result = 0;

    Boolean isWakeMotion = (policyFlags
            & (IWindowManagerPolicy::FLAG_WAKE | IWindowManagerPolicy::FLAG_WAKE_DROPPED)) != 0;
    if (isWakeMotion) {
        // assert(0);
        Logger::D(TAG, "TODO: keyguardMediator is not implement.");
        // if (mKeyguardMediator != NULL && mKeyguardMediator.isShowing()) {
        //     // If the keyguard is showing, let it decide what to do with the wake motion.
        //     mKeyguardMediator.onWakeMotionWhenKeyguardShowingTq();
        // } else {
        //     // Otherwise, wake the device ourselves.
        //     result |= ACTION_WAKE_UP;
        // }
    }

    return NOERROR;
}

void CPhoneWindowManager::DispatchMediaKeyWithWakeLock(
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
        // TODO: SetAsynchronous is not implement in Message.
        // msg->SetAsynchronous(TRUE);
        Boolean isSuccess = FALSE;
        mHandler->SendMessageDelayed(msg, CViewConfiguration::GetKeyRepeatTimeout(), &isSuccess);
    }
    else {
        mBroadcastWakeLock->ReleaseLock();
    }
}

void CPhoneWindowManager::DispatchMediaKeyRepeatWithWakeLock(
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

void CPhoneWindowManager::DispatchMediaKeyWithWakeLockToAudioService(
    /* [in] */ IKeyEvent* event)
{
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IIAudioService> audioService = GetAudioService();
        if (audioService != NULL) {
            ECode ec = audioService->DispatchMediaKeyEventUnderWakelock(event);
            Logger::E(TAG, "dispatchMediaKeyEvent threw exception %08x", ec);
        }
    }
}

ECode CPhoneWindowManager::ScreenTurnedOff(
    /* [in] */ Int32 why)
{
    // EventLog.writeEvent(70000, 0);
    {
        AutoLock lock(mLock);
        mScreenOnEarly = FALSE;
        mScreenOnFully = FALSE;
    }
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator != null) {
    //     mKeyguardMediator.onScreenTurnedOff(why);
    // }

    {
        AutoLock lock(mLock);
        UpdateOrientationListenerLp();
        UpdateLockScreenTimeout();
    }

    return NOERROR;
}

ECode CPhoneWindowManager::ScreenTurningOn(
    /* [in] */ IScreenOnListener* screenOnListener)
{
    // EventLog.writeEvent(70000, 1);
    // if (FALSE) {
    //     RuntimeException here = new RuntimeException("here");
    //     here.fillInStackTrace();
    //     Slog.i(TAG, "Screen turning on...", here);
    // }

    {
        AutoLock lock(mLock);
        mScreenOnEarly = TRUE;
        UpdateOrientationListenerLp();
        UpdateLockScreenTimeout();
    }

    WaitForKeyguard(screenOnListener);
    return NOERROR;
}

void CPhoneWindowManager::WaitForKeyguard(
    /* [in] */ IScreenOnListener* screenOnListener)
{
    // if (mKeyguardMediator != NULL) {
    //     if (screenOnListener != NULL) {
    //         mKeyguardMediator.onScreenTurnedOn(new KeyguardViewManager.ShowListener() {
    //             @Override
    //             public void onShown(IBinder windowToken) {
    //                 waitForKeyguardWindowDrawn(windowToken, screenOnListener);
    //             }
    //         });
    //         return;
    //     } else {
    //         mKeyguardMediator.onScreenTurnedOn(NULL);
    //     }
    // } else {
    //     Slog.i(TAG, "No keyguard mediator!");
    // }
    FinishScreenTurningOn(screenOnListener);
}

void CPhoneWindowManager::WaitForKeyguardWindowDrawn(
    /* [in] */ IBinder* windowToken,
    /* [in] */ IScreenOnListener* screenOnListener)
{
    if (windowToken != NULL) {
        // try {
        assert(0 && "RemoteCallback is not complete.");
        // if (mWindowManager->WaitForWindowDrawn(
        //         windowToken, new IRemoteCallback.Stub() {
        //     @Override
        //     public void sendResult(Bundle data) {
        //         Slog.i(TAG, "Lock screen displayed!");
        //         finishScreenTurningOn(screenOnListener);
        //     }
        // })) {
        //     return;
        // }
        // } catch (RemoteException ex) {
            // Can't happen in system process.
        // }
    }

    // Slog.i(TAG, "No lock screen!");
    FinishScreenTurningOn(screenOnListener);
}

void CPhoneWindowManager::FinishScreenTurningOn(
    /* [in] */ IScreenOnListener* screenOnListener)
{
    {
        AutoLock lock(mLock);
        mScreenOnFully = TRUE;
    }

    // try {
    mWindowManager->SetEventDispatching(TRUE);
    // } catch (RemoteException unhandled) {
    // }

    if (screenOnListener != NULL) {
        screenOnListener->OnScreenOn();
    }
}

ECode CPhoneWindowManager::IsScreenOnEarly(
    /* [out] */ Boolean* state)
{
    assert(state != NULL);
    *state = mScreenOnEarly;
    return NOERROR;
}

ECode CPhoneWindowManager::IsScreenOnFully(
    /* [out] */ Boolean* state)
{
    assert(state != NULL);
    *state = mScreenOnFully;
    return NOERROR;
}

void CPhoneWindowManager::ShowBootAnimation(
    /* [in] */ Int64 time)
{
    // if(DEBUG_BOOTFAST)
    //     Log.d(TAG,"showBootAnimation time = " + time);
    SystemClock::Sleep(1000);
    mBootAnimationView->ShowBootAnimation();
    SystemClock::Sleep(time);
    mBootAnimationView->HideBootAnimation();
}

ECode CPhoneWindowManager::AcquireBAView()
{
    // if (DEBUG_BOOTFAST)
    //     Slog.d(TAG,"acquireBAView");
    AutoPtr<AcquireBootRunnable> acquireBootRunnable = new AcquireBootRunnable(this);
    Boolean isSuccess = FALSE;
    return mHandler->Post(acquireBootRunnable, &isSuccess);
}

ECode CPhoneWindowManager::ReleaseBAView()
{
    // if(DEBUG_BOOTFAST)
    //     Slog.d(TAG,"releaseBAView");
    AutoPtr<ReleaseBootAVRunnable> releaseBootAVRunnable = new ReleaseBootAVRunnable(this);
    Boolean isSuccess = FALSE;
    return mHandler->Post(releaseBootAVRunnable, &isSuccess);
}

ECode CPhoneWindowManager::HideScreen(
    /* [in] */ Boolean enable)
{
    // if(DEBUG_BOOTFAST)
    //     Log.d(TAG,"hideScreen enable = " + enable);
    if (mBootAnimationView != NULL) {
        mBootAnimationView->HideScreen(enable);
    }
    return NOERROR;
}

ECode CPhoneWindowManager::ShowBootInitLogo(
        /* [in] */ Int32 logo)
{
    // if (DEBUG_BOOTFAST)
    //     Log.d(TAG,"showBootInitLogo ");
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_SHOW_BOOT_INIT, (IMessage**)&message);
    Boolean isSuccess = FALSE;
    return mHandler->SendMessage(message, &isSuccess);
}

ECode CPhoneWindowManager::ShowPowerCharge(
    /* [in] */ Int32 precent)
{
    // if(DEBUG_BOOTFAST)
    //     Log.d(TAG,"showPowerCharge");
    // TODO: Handler has not impelement obtainMessage.
    // mHandler.obtainMessage(MSG_SHOW_BATTERY_CHARGE,precent,0).sendToTarget();
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindowManager::EnableKeyguard(
    /* [in] */ Boolean enabled)
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator != NULL) {
    //     mKeyguardMediator.setKeyguardEnabled(enabled);
    // }
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindowManager::ExitKeyguardSecurely(
   /* [in] */ IOnKeyguardExitResult* callback)
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator != NULL) {
    //     mKeyguardMediator.verifyUnlock(callback);
    // }

    return E_NOT_IMPLEMENTED;
}

Boolean CPhoneWindowManager::KeyguardIsShowingTq()
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator == NULL) return FALSE;
    //return mKeyguardMediator.isShowingAndNotHidden();
    return FALSE;
}

ECode CPhoneWindowManager::IsKeyguardLocked(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    *state = KeyguardOn();
    return NOERROR;
}

ECode CPhoneWindowManager::IsKeyguardSecure(
    /* [out] */ Boolean* state)
{
    assert(state != NULL);
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator == NULL) return FALSE;
    // return mKeyguardMediator.isSecure();
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindowManager::InKeyguardRestrictedKeyInputMode(
    /* [out] */ Boolean* restricted)
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator == NULL) return FALSE;
    // return mKeyguardMediator.isInputRestricted();
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneWindowManager::DismissKeyguardLw()
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator.isShowing()) {
    //     mHandler.post(new Runnable() {
    //         public void run() {
    //             if (mKeyguardMediator.isDismissable()) {
    //                 // Can we just finish the keyguard straight away?
    //                 mKeyguardMediator.keyguardDone(FALSE, TRUE);
    //             } else {
    //                 // ask the keyguard to prompt the user to authenticate if necessary
    //                 mKeyguardMediator.dismiss();
    //             }
    //         }
    //     });
    // }
    return E_NOT_IMPLEMENTED;
}

void CPhoneWindowManager::SendCloseSystemWindows()
{
    SendCloseSystemWindows(mContext, String(NULL));
}

void CPhoneWindowManager::SendCloseSystemWindows(
    /* [in] */ const String& reason)
{
    SendCloseSystemWindows(mContext, reason);
}

void CPhoneWindowManager::SendCloseSystemWindows(
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

ECode CPhoneWindowManager::RotationForOrientationLw(
    /* [in] */ Int32 orientation,
    /* [in] */ Int32 lastRotation,
    /* [out] */ Int32* surfaceRotation)
{
    // if (FALSE) {
    //     Slog.v(TAG, "rotationForOrientationLw(orient="
    //                 + orientation + ", last=" + lastRotation
    //                 + "); user=" + mUserRotation + " "
    //                 + ((mUserRotationMode == WindowManagerPolicy.USER_ROTATION_LOCKED)
    //                     ? "USER_ROTATION_LOCKED" : "")
    //                 );
    // }

    AutoLock lock(mLock);
    Int32 sensorRotation = -1;
    // TODO: WindowOrientationListener is not implement.
    // Int32 sensorRotation = mOrientationListener->GetProposedRotation(); // may be -1
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
    } else if (mHdmiPlugged && mHdmiRotationLock) {
        // Ignore sensor when plugged into HDMI.
        // Note that the dock orientation overrides the HDMI orientation.
        preferredRotation = mHdmiRotation;
    } else if ((mUserRotationMode == IWindowManagerPolicy::USER_ROTATION_FREE
                    && (orientation == IActivityInfo::SCREEN_ORIENTATION_USER
                            || orientation == IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED))
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
                || orientation == IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR) {
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
            // return Surface.ROTATION_0;
            if (SystemProperties::GetInt32(String("ro.sf.hwrotation"), 0) == 270) {
                *surfaceRotation = ISurface::ROTATION_90;
                return NOERROR;
            } else {
                *surfaceRotation = ISurface::ROTATION_0;
                return NOERROR;
            }
    }
    return NOERROR;
}

ECode CPhoneWindowManager::RotationHasCompatibleMetricsLw(
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

ECode CPhoneWindowManager::SetRotationLw(
    /* [in] */ Int32 rotation)
{
    // Todo OrientationListener is not implement.
    // mOrientationListener.setCurrentRotation(rotation);
    return E_NOT_IMPLEMENTED;
}

Boolean CPhoneWindowManager::IsLandscapeOrSeascape(
    /* [in] */ Int32 rotation)
{
    return rotation == mLandscapeRotation || rotation == mSeascapeRotation;
}

Boolean CPhoneWindowManager::IsAnyPortrait(
    /* [in] */ Int32 rotation)
{
    return rotation == mPortraitRotation || rotation == mUpsideDownRotation;
}

ECode CPhoneWindowManager::SetUserRotationMode(
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

ECode CPhoneWindowManager::SetSafeMode(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;
    Boolean isSuccess = FALSE;
    return PerformHapticFeedbackLw(NULL, safeMode
            ? IHapticFeedbackConstants::SAFE_MODE_ENABLED
            : IHapticFeedbackConstants::SAFE_MODE_DISABLED, TRUE, &isSuccess);
}

AutoPtr< ArrayOf<Int64> > CPhoneWindowManager::GetLongIntArray(
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

ECode CPhoneWindowManager::SystemReady()
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator != null) {
    //     // tell the keyguard
    //     mKeyguardMediator.onSystemReady();
    // }
    AutoLock lock(mLock);
    UpdateOrientationListenerLp();
    mSystemReady = TRUE;
    AutoPtr<UpdateSettingRunnable> updateSettingRunnable = new UpdateSettingRunnable(this);
    Boolean isSuccess = FALSE;
    return mHandler->Post(updateSettingRunnable, &isSuccess);
}

ECode CPhoneWindowManager::SystemBooted()
{
    AutoLock lock(mLock);
    mSystemBooted = TRUE;
    return NOERROR;
}

ECode CPhoneWindowManager::ShowBootMessage(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean always)
{
    if (mHeadless) {
        return NOERROR;
    }
    AutoPtr<BootMsgRunnable> bootMsgRunnable = new BootMsgRunnable(this, msg);
    Boolean isSuccess = FALSE;
    return mHandler->Post(bootMsgRunnable, & isSuccess);
}

ECode CPhoneWindowManager::HideBootMessages()
{
    AutoPtr<BootMsgDismissRunnable> bootMsgDismissRunnable =
            new BootMsgDismissRunnable(this);
    Boolean isSuccess = FALSE;
    return mHandler->Post(bootMsgDismissRunnable, &isSuccess);
}

ECode CPhoneWindowManager::UserActivity()
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

ECode CPhoneWindowManager::LockNow(
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

void CPhoneWindowManager::UpdateLockScreenTimeout()
{
    AutoLock lock(mScreenLockTimeout);
    Boolean enable = FALSE;
    // TODO: KeyguardMediator is not implement.
    // Boolean enable = (mAllowLockscreenWhenOn && mScreenOnEarly &&
    //         mKeyguardMediator != NULL && mKeyguardMediator->IsSecure());
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

ECode CPhoneWindowManager::EnableScreenAfterBoot()
{
    ReadLidState();
    ApplyLidSwitchState();
    UpdateRotation(TRUE);

    return NOERROR;
}

void CPhoneWindowManager::ApplyLidSwitchState()
{
    if (mLidState == IWindowManagerPolicyWindowManagerFuncs::LID_CLOSED && mLidControlsSleep) {
        mPowerManager->GoToSleep(SystemClock::GetUptimeMillis());
    }
}

void CPhoneWindowManager::UpdateRotation(
    /* [in] */ Boolean alwaysSendConfiguration)
{
    // try {
    //set orientation on WindowManager
    mWindowManager->UpdateRotation(alwaysSendConfiguration, FALSE);
    // } catch (RemoteException e) {
    //     // Ignore
    // }
}

void CPhoneWindowManager::UpdateRotation(
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

void CPhoneWindowManager::StartDockOrHome()
{
    // We don't have dock home anymore. Home is home. If you lived here, you'd be home by now.
    mContext->StartActivityAsUser(mHomeIntent, UserHandle::CURRENT);
}

/**
* goes to the home screen
* @return whether it did anything
*/
Boolean CPhoneWindowManager::GoHome()
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
        Int32 testMode = SystemProperties::GetInt32(String("persist.sys.uts-test-mode"), 0);
        if (testMode == 1) {
            /// Roll back EndcallBehavior as the cupcake design to pass P1 lab entry.
            // Logger::D(TAG, "UTS-TEST-MODE");
        }
        else {
            am->StopAppSwitches();
            SendCloseSystemWindows();
        }

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        String type, nullStr;
        mHomeIntent->ResolveTypeIfNeeded(resolver, &type);
        Int32 result;
        am->StartActivityAsUser(NULL, mHomeIntent,
            type, NULL, nullStr, 0,
            IActivityManager::START_FLAG_ONLY_IF_NEEDED,
            nullStr, NULL, NULL, IUserHandle::USER_CURRENT, &result);
        if (result == IActivityManager::START_RETURN_INTENT_TO_CALLER) {
            return FALSE;
        }
        // } catch (RemoteException ex) {
        //     // bummer, the activity manager, which is in this process, is dead
        // }
    }
    return TRUE;
}

ECode CPhoneWindowManager::SetCurrentOrientationLw(
    /* [in] */ Int32 newOrientation)
{
    AutoLock lock(mLock);

    if (newOrientation != mCurrentAppOrientation) {
        mCurrentAppOrientation = newOrientation;
        UpdateOrientationListenerLp();
    }

    return NOERROR;
}

void CPhoneWindowManager::PerformAuditoryFeedbackForAccessibilityIfNeed()
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

Boolean CPhoneWindowManager::IsGlobalAccessibilityGestureEnabled()
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

ECode CPhoneWindowManager::PerformHapticFeedbackLw(
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
    Boolean hapticsDisabled;
    Int32 value;
    Settings::System::GetInt32ForUser(contentResolver,
            ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 0, IUserHandle::USER_CURRENT, &value);
    hapticsDisabled = value == 0;
    // TODO: keyguardMediator is not implement.
    // if (!always && (hapticsDisabled || mKeyguardMediator->IsShowingAndNotHidden())) {
    //     return false;
    // }
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
    if (pattern->GetLength() == 1) {
        // One-shot vibration
        mVibrator->Vibrate((*pattern)[0]);
    } else {
        // Pattern vibration
        mVibrator->Vibrate(*pattern, -1);
    }
    *isSucceed = TRUE;
    return NOERROR;
}

ECode CPhoneWindowManager::KeepScreenOnStartedLw()
{
    return NOERROR;
}

ECode CPhoneWindowManager::KeepScreenOnStoppedLw()
{
    // TODO: KeyguardMediator is not implement.
    // if (mKeyguardMediator != NULL && !mKeyguardMediator.isShowingAndNotHidden()) {
    //     long curTime = SystemClock.uptimeMillis();
    //     mPowerManager.userActivity(curTime, FALSE);
    // }

    return E_NOT_IMPLEMENTED;
}

Int32 CPhoneWindowManager::UpdateSystemUiVisibilityLw()
{
    // If there is no window focused, there will be nobody to handle the events
    // anyway, so just hang on in whatever state we're in until things settle down.
    if (mFocusedWindow == NULL) {
        return 0;
    }

    AutoPtr<IWindowManagerLayoutParams> tmp;
    mFocusedWindow->GetAttrs((IWindowManagerLayoutParams**)&tmp);
    Int32 type = 0;
    tmp->GetType(&type);
    if (type == IWindowManagerLayoutParams::TYPE_KEYGUARD && mHideLockScreen == TRUE) {
        // We are updating at a point where the keyguard has gotten
        // focus, but we were last in a state where the top window is
        // hiding it.  This is probably because the keyguard as been
        // shown while the top window was displayed, so we want to ignore
        // it here because this is just a very transient change and it
        // will quickly lose focus once it correctly gets hidden.
        return 0;
    }

    Int32 uiVisiable = 0;
    mFocusedWindow->GetSystemUiVisibility(&uiVisiable);
    Int32 visibility = uiVisiable & ~mResettingSystemUiFlags;
    visibility &= ~mForceClearedSystemUiFlags;

    if (mForcingShowNavBar) {
        Int32 sl;
        mFocusedWindow->GetSurfaceLayer(&sl);
        if (sl < mForcingShowNavBarLayer) {
            visibility &= ~IView::SYSTEM_UI_CLEARABLE_FLAGS;
        }
    }

    Int32 diff = visibility ^ mLastSystemUiFlags;
    Boolean needsMenu = FALSE;
    mFocusedWindow->GetNeedsMenuLw(mTopFullscreenOpaqueWindowState, &needsMenu);

    AutoPtr<IApplicationToken> appToken;
    mFocusedWindow->GetAppToken((IApplicationToken**)&appToken);
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

ECode CPhoneWindowManager::HasNavigationBar(
    /* [out] */ Boolean* has)
{
    assert(has != NULL);
    *has = mHasNavigationBar;
    return NOERROR;
}

ECode CPhoneWindowManager::SetLastInputMethodWindowLw(
    /* [in] */ IWindowState* ime,
    /* [in] */ IWindowState* target)
{
    mLastInputMethodWindow = ime;
    mLastInputMethodTargetWindow = target;
    return NOERROR;
}

ECode CPhoneWindowManager::CanMagnifyWindowLw(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* result)
{
    assert(attrs != NULL && result != NULL);
    Int32 type = 0;
    attrs->GetType(&type);

    switch (type) {
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

ECode CPhoneWindowManager::SetCurrentUserLw(
    /* [in] */ Int32 newUserId)
{
    // TODO
    // if (mKeyguardMediator != NULL) {
    //     mKeyguardMediator.setCurrentUser(newUserId);
    // }
    if (mStatusBarService != NULL) {
        // try {
            mStatusBarService->SetCurrentUser(newUserId);
        // } catch (RemoteException e) {
        //     // oh well
        // }
    }

    return SetLastInputMethodWindowLw(NULL, NULL);
}

ECode CPhoneWindowManager::ShowAssistant()
{
    // mKeyguardMediator.showAssistant();
    return E_NOT_IMPLEMENTED;
}

void CPhoneWindowManager::Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args)
{
    pw->PrintString(prefix); pw->PrintString(String("mSafeMode=")); pw->PrintBoolean(mSafeMode);
            pw->PrintString(String(" mSystemReady=")); pw->PrintBoolean(mSystemReady);
            pw->PrintString(String(" mSystemBooted=")); pw->PrintBooleanln(mSystemBooted);
    pw->PrintString(prefix); pw->PrintString(String("mLidState=")); pw->PrintInt32(mLidState);
            pw->PrintString(String(" mLidOpenRotation=")); pw->PrintInt32(mLidOpenRotation);
            pw->PrintString(String(" mHdmiPlugged=")); pw->PrintBooleanln(mHdmiPlugged);
    if (mLastSystemUiFlags != 0 || mResettingSystemUiFlags != 0
            || mForceClearedSystemUiFlags != 0) {
        pw->PrintString(prefix); pw->PrintString(String("mLastSystemUiFlags=0x"));
                pw->PrintString(StringUtils::Int32ToHexString(mLastSystemUiFlags, FALSE));
                pw->PrintString(String(" mResettingSystemUiFlags=0x"));
                pw->PrintString(StringUtils::Int32ToHexString(mResettingSystemUiFlags, FALSE));
                pw->PrintString(String(" mForceClearedSystemUiFlags=0x"));
                pw->PrintStringln(StringUtils::Int32ToHexString(mForceClearedSystemUiFlags, FALSE));
    }
    if (mLastFocusNeedsMenu) {
        pw->PrintString(prefix); pw->PrintString(String("mLastFocusNeedsMenu="));
                pw->PrintBooleanln(mLastFocusNeedsMenu);
    }
    pw->PrintString(prefix); pw->PrintString(String("mDockMode=")); pw->PrintInt32(mDockMode);
            pw->PrintString(String(" mCarDockRotation=")); pw->PrintInt32(mCarDockRotation);
            pw->PrintString(String(" mDeskDockRotation=")); pw->PrintInt32ln(mDeskDockRotation);
    pw->PrintString(prefix); pw->PrintString(String("mUserRotationMode=")); pw->PrintInt32(mUserRotationMode);
            pw->PrintString(String(" mUserRotation=")); pw->PrintInt32(mUserRotation);
            pw->PrintString(String(" mAllowAllRotations=")); pw->PrintInt32ln(mAllowAllRotations);
    pw->PrintString(prefix); pw->PrintString(String("mCurrentAppOrientation=")); pw->PrintInt32ln(mCurrentAppOrientation);
    pw->PrintString(prefix); pw->PrintString(String("mCarDockEnablesAccelerometer="));
            pw->PrintBoolean(mCarDockEnablesAccelerometer);
            pw->PrintString(String(" mDeskDockEnablesAccelerometer="));
            pw->PrintBooleanln(mDeskDockEnablesAccelerometer);
    pw->PrintString(prefix); pw->PrintString(String("mLidKeyboardAccessibility="));
            pw->PrintInt32(mLidKeyboardAccessibility);
            pw->PrintString(String(" mLidNavigationAccessibility=")); pw->PrintInt32(mLidNavigationAccessibility);
            pw->PrintString(String(" mLidControlsSleep=")); pw->PrintBooleanln(mLidControlsSleep);
    pw->PrintString(prefix); pw->PrintString(String("mLongPressOnPowerBehavior="));
            pw->PrintInt32(mLongPressOnPowerBehavior);
            pw->PrintString(String(" mHasSoftInput=")); pw->PrintBooleanln(mHasSoftInput);
    pw->PrintString(prefix); pw->PrintString(String("mScreenOnEarly=")); pw->PrintBoolean(mScreenOnEarly);
            pw->PrintString(String(" mScreenOnFully=")); pw->PrintBoolean(mScreenOnFully);
            pw->PrintString(String(" mOrientationSensorEnabled=")); pw->PrintBooleanln(mOrientationSensorEnabled);
    pw->PrintString(prefix); pw->PrintString(String("mUnrestrictedScreen=(")); pw->PrintInt32(mUnrestrictedScreenLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mUnrestrictedScreenTop);
            pw->PrintString(String(") ")); pw->PrintInt32(mUnrestrictedScreenWidth);
            pw->PrintString(String("x")); pw->PrintInt32ln(mUnrestrictedScreenHeight);
    pw->PrintString(prefix); pw->PrintString(String("mRestrictedScreen=(")); pw->PrintInt32(mRestrictedScreenLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mRestrictedScreenTop);
            pw->PrintString(String(") ")); pw->PrintInt32(mRestrictedScreenWidth);
            pw->PrintString(String("x")); pw->PrintInt32ln(mRestrictedScreenHeight);
    pw->PrintString(prefix); pw->PrintString(String("mStableFullscreen=(")); pw->PrintInt32(mStableFullscreenLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mStableFullscreenTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mStableFullscreenRight);
            pw->PrintString(String(",")); pw->PrintInt32(mStableFullscreenBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mStable=(")); pw->PrintInt32(mStableLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mStableTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mStableRight);
            pw->PrintString(String(",")); pw->PrintInt32(mStableBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mSystem=(")); pw->PrintInt32(mSystemLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mSystemTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mSystemRight);
            pw->PrintString(String(",")); pw->PrintInt32(mSystemBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mCur=(")); pw->PrintInt32(mCurLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mCurTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mCurRight);
            pw->PrintString(String(",")); pw->PrintInt32(mCurBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mContent=(")); pw->PrintInt32(mContentLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mContentTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mContentRight);
            pw->PrintString(String(",")); pw->PrintInt32(mContentBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mDock=(")); pw->PrintInt32(mDockLeft);
            pw->PrintString(String(",")); pw->PrintInt32(mDockTop);
            pw->PrintString(String(")-(")); pw->PrintInt32(mDockRight);
            pw->PrintString(String(",")); pw->PrintInt32(mDockBottom); pw->PrintStringln(String(")"));
    pw->PrintString(prefix); pw->PrintString(String("mDockLayer=")); pw->PrintInt32(mDockLayer);
            pw->PrintString(String(" mStatusBarLayer=")); pw->PrintInt32ln(mStatusBarLayer);
    pw->PrintString(prefix); pw->PrintString(String("mShowingLockscreen=")); pw->PrintBoolean(mShowingLockscreen);
            pw->PrintString(String(" mShowingDream=")); pw->PrintBoolean(mShowingDream);
            pw->PrintString(String(" mDreamingLockscreen=")); pw->PrintInt32ln(mDreamingLockscreen);
    if (mLastInputMethodWindow != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mLastInputMethodWindow="));
                pw->PrintObjectln(mLastInputMethodWindow);
    }
    if (mLastInputMethodTargetWindow != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mLastInputMethodTargetWindow="));
                pw->PrintObjectln(mLastInputMethodTargetWindow);
    }
    if (mStatusBar != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mStatusBar="));
                pw->PrintObjectln(mStatusBar);
    }
    if (mNavigationBar != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mNavigationBar="));
                pw->PrintObjectln(mNavigationBar);
    }
    if (mKeyguard != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mKeyguard="));
                pw->PrintObjectln(mKeyguard);
    }
    if (mFocusedWindow != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mFocusedWindow="));
                pw->PrintObjectln(mFocusedWindow);
    }
    if (mFocusedApp != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mFocusedApp="));
                pw->PrintObjectln(mFocusedApp);
    }
    if (mWinDismissingKeyguard != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mWinDismissingKeyguard="));
                pw->PrintObjectln(mWinDismissingKeyguard);
    }
    if (mTopFullscreenOpaqueWindowState != NULL) {
        pw->PrintString(prefix); pw->PrintString(String("mTopFullscreenOpaqueWindowState="));
                pw->PrintObjectln(mTopFullscreenOpaqueWindowState);
    }
    if (mForcingShowNavBar) {
        pw->PrintString(prefix); pw->PrintString(String("mForcingShowNavBar="));
                pw->PrintBooleanln(mForcingShowNavBar); pw->PrintString(String("mForcingShowNavBarLayer="));
                pw->PrintInt32ln(mForcingShowNavBarLayer);
    }
    pw->PrintString(prefix); pw->PrintString(String("mTopIsFullscreen=")); pw->PrintBoolean(mTopIsFullscreen);
            pw->PrintString(String(" mHideLockScreen=")); pw->PrintBooleanln(mHideLockScreen);
    pw->PrintString(prefix); pw->PrintString(String("mForceStatusBar=")); pw->PrintBoolean(mForceStatusBar);
            pw->PrintString(String(" mForceStatusBarFromKeyguard="));
            pw->PrintBooleanln(mForceStatusBarFromKeyguard);
    pw->PrintString(prefix); pw->PrintString(String("mDismissKeyguard=")); pw->PrintInt32(mDismissKeyguard);
            pw->PrintString(String(" mWinDismissingKeyguard=")); pw->PrintObject(mWinDismissingKeyguard);
            pw->PrintString(String(" mHomePressed=")); pw->PrintBooleanln(mHomePressed);
    pw->PrintString(prefix); pw->PrintString(String("mAllowLockscreenWhenOn=")); pw->PrintBoolean(mAllowLockscreenWhenOn);
            pw->PrintString(String(" mLockScreenTimeout=")); pw->PrintInt32(mLockScreenTimeout);
            pw->PrintString(String(" mLockScreenTimerActive=")); pw->PrintBooleanln(mLockScreenTimerActive);
    pw->PrintString(prefix); pw->PrintString(String("mEndcallBehavior=")); pw->PrintInt32(mEndcallBehavior);
            pw->PrintString(String(" mIncallPowerBehavior=")); pw->PrintInt32(mIncallPowerBehavior);
            pw->PrintString(String(" mLongPressOnHomeBehavior=")); pw->PrintInt32ln(mLongPressOnHomeBehavior);
    pw->PrintString(prefix); pw->PrintString(String("mLandscapeRotation=")); pw->PrintInt32(mLandscapeRotation);
            pw->PrintString(String(" mSeascapeRotation=")); pw->PrintInt32ln(mSeascapeRotation);
    pw->PrintString(prefix); pw->PrintString(String("mPortraitRotation=")); pw->PrintInt32(mPortraitRotation);
            pw->PrintString(String(" mUpsideDownRotation=")); pw->PrintInt32ln(mUpsideDownRotation);
    pw->PrintString(prefix); pw->PrintString(String("mHdmiRotation=")); pw->PrintInt32(mHdmiRotation);
            pw->PrintString(String(" mHdmiRotationLock=")); pw->PrintBooleanln(mHdmiRotationLock);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
