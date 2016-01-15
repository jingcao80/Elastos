#include "elastos/droid/systemui/statusbar/BaseStatusBar.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/CActivityOptionsHelper.h"
#include "elastos/droid/app/CWallpaperManagerHelper.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CPopupMenu.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/internal/CSizeAdaptiveLayoutLayoutParams.h"
#include "elastos/droid/statusbar/CStatusBarIcon.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/systemui/statusbar/CCommandQueue.h"
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"
//#include "elastos/droid/systemui/recent/TaskDescription.h"
#include "elastos/droid/systemui/recent/CRecentTasksLoader.h"


using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::R;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
//using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CWallpaperManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER_INSIDE;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::Internal::ISizeAdaptiveLayoutLayoutParams;
using Elastos::Droid::Widget::Internal::CSizeAdaptiveLayoutLayoutParams;
using Elastos::Droid::StatusBar::IStatusBarIconList;
using Elastos::Droid::StatusBar::IIStatusBar;
using Elastos::Droid::StatusBar::CStatusBarIcon;
using Elastos::Droid::StatusBar::EIID_IIStatusBar;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::Recent::ITaskDescription;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::Recent::CRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const String BaseStatusBar::TAG("StatusBar");
const Boolean BaseStatusBar::DEBUG = FALSE;
const Boolean BaseStatusBar::MULTIUSER_DEBUG = FALSE;

const Int32 BaseStatusBar::MSG_TOGGLE_RECENTS_PANEL = 1020;
const Int32 BaseStatusBar::MSG_CLOSE_RECENTS_PANEL = 1021;
const Int32 BaseStatusBar::MSG_PRELOAD_RECENT_APPS = 1022;
const Int32 BaseStatusBar::MSG_CANCEL_PRELOAD_RECENT_APPS = 1023;
const Int32 BaseStatusBar::MSG_OPEN_SEARCH_PANEL = 1024;
const Int32 BaseStatusBar::MSG_CLOSE_SEARCH_PANEL = 1025;
const Int32 BaseStatusBar::MSG_SHOW_INTRUDER = 1026;
const Int32 BaseStatusBar::MSG_HIDE_INTRUDER = 1027;

const Boolean BaseStatusBar::ENABLE_INTRUDERS = FALSE;

//=============================================================================
//              BaseStatusBar::MyHandler
//=============================================================================
ECode BaseStatusBar::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case BaseStatusBar::MSG_TOGGLE_RECENTS_PANEL:
            if (BaseStatusBar::DEBUG) Slogger::D(BaseStatusBar::TAG, "toggle recents panel");
            mHost->ToggleRecentsActivity();
            break;
        case BaseStatusBar::MSG_CLOSE_RECENTS_PANEL: {
            if (BaseStatusBar::DEBUG) Slogger::D(BaseStatusBar::TAG, "closing recents panel");
            AutoPtr<IIntent> intent;
            CIntent::New(IRecentsActivity::CLOSE_RECENTS_INTENT, (IIntent**)&intent);
            String packageName("SystemUI"); //"com.android.systemui"
            intent->SetPackage(packageName);
            AutoPtr<IUserHandle> userHandle;
            CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
            mHost->mContext->SendBroadcastAsUser(intent, userHandle);
            break;
        }
        case BaseStatusBar::MSG_PRELOAD_RECENT_APPS:
            mHost->PreloadRecentTasksList();
            break;
        case BaseStatusBar::MSG_CANCEL_PRELOAD_RECENT_APPS:
            mHost->CancelPreloadingRecentTasksList();
            break;
        case BaseStatusBar::MSG_OPEN_SEARCH_PANEL:
            if (BaseStatusBar::DEBUG) Slogger::D(BaseStatusBar::TAG, "opening search panel");
            if (mHost->mSearchPanelView != NULL) {
                Boolean isAssistantAvailable;
                mHost->mSearchPanelView->IsAssistantAvailable(&isAssistantAvailable);
                if (isAssistantAvailable) {
                    mHost->mSearchPanelView->Show(TRUE, TRUE);
                }
            }
            break;
        case BaseStatusBar::MSG_CLOSE_SEARCH_PANEL:
            if (BaseStatusBar::DEBUG) Slogger::D(BaseStatusBar::TAG, "closing search panel");
            if (mHost->mSearchPanelView != NULL) {
                Boolean isShowing;
                mHost->mSearchPanelView->IsShowing(&isShowing);
                if (isShowing) {
                    mHost->mSearchPanelView->Show(FALSE, TRUE);
                }
            }
            break;
    }

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::ProvisioningObserver
//=============================================================================

BaseStatusBar::ProvisioningObserver::ProvisioningObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ BaseStatusBar* bar)
    : ContentObserver(handler)
    , mHost(bar)
{}

ECode BaseStatusBar::ProvisioningObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Int32 value = 0;
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    ASSERT_SUCCEEDED(CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal));
    settingsGlobal->GetInt32(resolver,
            ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);

    Boolean provisioned = value != 0;
    if (provisioned != mHost->mDeviceProvisioned) {
        mHost->mDeviceProvisioned = provisioned;
        mHost->UpdateNotificationIcons();
    }
    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::StatusBarRemoteViewsOnClickHandler
//=============================================================================
BaseStatusBar::StatusBarRemoteViewsOnClickHandler::StatusBarRemoteViewsOnClickHandler(
 /* [in] */ BaseStatusBar* bar)
    : mHost(bar)
{
}

ECode BaseStatusBar::StatusBarRemoteViewsOnClickHandler::OnClickHandler(
    /* [in] */ IView* view,
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* fillInIntent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(pendingIntent != NULL);

    // if (DEBUG) {
    //     Slog.v(TAG, "Notification click handler invoked for intent: " + pendingIntent);
    // }
    Boolean isActivity;
    pendingIntent->IsActivity(&isActivity);
    if (isActivity) {
        // try {
        // The intent we are sending is for the application, which
        // won't have permission to immediately start an activity after
        // the user switches to home.  We know it is safe to do at this
        // point, so make sure new activity switches are now allowed.
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->ResumeAppSwitches();
        // Also, notifications can be launched from the lock screen,
        // so dismiss the lock screen when the activity starts.
        am->DismissKeyguardOnNextActivity();
        // } catch (RemoteException e) {
        // }
    }

    RemoteViewsOnClickHandler::OnClickHandler(view, pendingIntent, fillInIntent, result);

    if (isActivity && *result) {
        // close the shade if it was open
        mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
        mHost->VisibilityChanged(FALSE);
    }

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::NotificationBlamePopupOnMenuItemClickListener
//=============================================================================

CAR_INTERFACE_IMPL(BaseStatusBar::NotificationBlamePopupOnMenuItemClickListener, IPopupMenuOnMenuItemClickListener)

BaseStatusBar::NotificationBlamePopupOnMenuItemClickListener::NotificationBlamePopupOnMenuItemClickListener(
    /* [in] */ BaseStatusBar* bar,
    /* [in] */ const String& name)
    : mHost(bar)
    , mPackageName(name)
{
}

ECode BaseStatusBar::NotificationBlamePopupOnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    assert(item != NULL);

    Int32 id;
    item->GetItemId(&id);
    if (id == SystemUIR::id::notification_inspect_item) {
        mHost->StartApplicationDetailsActivity(mPackageName);
        mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
        *result = TRUE;
    }

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::NotificationLongClicker
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::NotificationLongClicker, IViewOnLongClickListener)

BaseStatusBar::NotificationLongClicker::NotificationLongClicker(
    /* [in] */ BaseStatusBar* bar)
    : mHost(bar)
{
}

ECode BaseStatusBar::NotificationLongClicker::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IInterface> tagObj;
    v->GetTag((IInterface**)&tagObj);
    String packageNameF;
    if (ICharSequence::Probe(tagObj) != NULL) {
        ICharSequence::Probe(tagObj)->ToString(&packageNameF);
    }
    if (packageNameF.IsNull()) return NOERROR;

    AutoPtr<IBinder> windowToken;
    v->GetWindowToken((IBinder**)&windowToken);
    if (windowToken == NULL) return NOERROR;

    mHost->mNotificationBlamePopup = NULL;
    CPopupMenu::New(mHost->mContext, v, (IPopupMenu**)&(mHost->mNotificationBlamePopup));
    mHost->mNotificationBlamePopup->Inflate(SystemUIR::menu::notification_popup_menu);
    AutoPtr<NotificationBlamePopupOnMenuItemClickListener> listener =
        new NotificationBlamePopupOnMenuItemClickListener(mHost, packageNameF);
    mHost->mNotificationBlamePopup->SetOnMenuItemClickListener(listener);
    mHost->mNotificationBlamePopup->Show();

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::RecentsPreloadOnTouchListener
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::RecentsPreloadOnTouchListener, IViewOnTouchListener)

BaseStatusBar::RecentsPreloadOnTouchListener::RecentsPreloadOnTouchListener(
    /* [in] */ BaseStatusBar* bar)
    : mHost(bar)
{
}

ECode BaseStatusBar::RecentsPreloadOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    assert(event != NULL && v != NULL);

    Int32 action;
    event->GetAction(&action);
    action &= IMotionEvent::ACTION_MASK;
    if (action == IMotionEvent::ACTION_DOWN) {
        mHost->PreloadRecentTasksList();
    }
    else if (action == IMotionEvent::ACTION_CANCEL) {
        mHost->CancelPreloadingRecentTasksList();
    }
    else if (action == IMotionEvent::ACTION_UP) {
        Boolean pressed;
        if (v->IsPressed(&pressed), !pressed) {
            mHost->CancelPreloadingRecentTasksList();
        }
    }

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::TouchOutsideListener
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::TouchOutsideListener, IViewOnTouchListener)

BaseStatusBar::TouchOutsideListener::TouchOutsideListener(
    /* [in] */ Int32 msg,
    /* [in] */ IStatusBarPanel* panel,
    /* [in] */ BaseStatusBar* bar)
    : mMsg(msg)
    , mPanel(panel)
    , mHost(bar)
{
}

ECode BaseStatusBar::TouchOutsideListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 action;
    event->GetAction(&action);
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);

    Boolean isIn;
    if (action == IMotionEvent::ACTION_OUTSIDE
        || (action == IMotionEvent::ACTION_DOWN
            && (mPanel->IsInContentArea((Int32)x, (Int32)y, &isIn), !isIn))) {
        if (MSG_CLOSE_SEARCH_PANEL == mMsg) {
            Boolean result;
            mHost->mHandler->RemoveMessages(mMsg);
            mHost->mHandler->SendEmptyMessage(mMsg, &result);
        }

        *result = TRUE;
    }

    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::NotificationClicker
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::NotificationClicker, IViewOnClickListener)

BaseStatusBar::NotificationClicker::NotificationClicker(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ BaseStatusBar* bar)
    : mIntent(intent)
    , mPkg(pkg)
    , mTag(tag)
    , mId(id)
    , mHost(bar)
{
}

ECode BaseStatusBar::NotificationClicker::OnClick(
    /* [in] */ IView* v)
{
    // try {
        // The intent we are sending is for the application, which
        // won't have permission to immediately start an activity after
        // the user switches to home.  We know it is safe to do at this
        // point, so make sure new activity switches are now allowed.
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->ResumeAppSwitches();
        // Also, notifications can be launched from the lock screen,
        // so dismiss the lock screen when the activity starts.
        am->DismissKeyguardOnNextActivity();
    // } catch (RemoteException e) {
    // }

    if (mIntent != NULL) {
        Int32 x, y, w, h;
        v->GetLocationOnScreen(&x, &y);
        v->GetWidth(&w);
        v->GetHeight(&h);
        AutoPtr<IRect> rect;
        CRect::New(x, y, x + w, y + h, (IRect**)&rect);

        AutoPtr<IIntent> overlay;
        CIntent::New((IIntent**)&overlay);
        overlay->SetSourceBounds(rect);
        // try {
            mIntent->Send(mHost->mContext, 0, overlay);
        // } catch (PendingIntent.CanceledException e) {
        //     // the stack trace isn't very helpful here.  Just log the exception message.
        //     Slogger::W(TAG, "Sending contentIntent failed: " + e);
        // }

        //TODO
        // AutoPtr<IInterface> kgmObj;
        // mHost->mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&kgmObj);
        // AutoPtr<IKeyguardManager> kgm = IKeyguardManager::Probe(kgmObj.Get());
        // if (kgm != NULL) kgm->ExitKeyguardSecurely(NULL);
    }

    // try {
    if (mHost->mBarService)
        mHost->mBarService->OnNotificationClick(mPkg, mTag, mId);
    // } catch (RemoteException ex) {
    //     // system process is dead if we're here.
    // }

    // close the shade if it was open
    mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    mHost->VisibilityChanged(FALSE);

    // If this click was on the intruder alert, hide that instead
//            mHandler.sendEmptyMessage(MSG_HIDE_INTRUDER);
    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::VetoButtonClicker
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::VetoButtonClicker, IViewOnClickListener)

BaseStatusBar::VetoButtonClicker::VetoButtonClicker(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ BaseStatusBar* bar)
    : mPkg(pkg)
    , mTag(tag)
    , mId(id)
    , mHost(bar)
{
}

ECode BaseStatusBar::VetoButtonClicker::OnClick(
    /* [in] */ IView* v)
{
    assert(v != NULL);
    // Accessibility feedback
    String info;
    mHost->mContext->GetString(SystemUIR::string::accessibility_notification_dismissed, &info);
    AutoPtr<ICharSequence> seq;
    CString::New(info, (ICharSequence**)&seq);
    v->AnnounceForAccessibility(seq);
    // try {
    if (mHost->mBarService)
        mHost->mBarService->OnNotificationClear(mPkg, mTag, mId);
    // } catch (RemoteException ex) {
    //     // system process is dead if we're here.
    // }
    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::StatusBarBroadcastReceiver
//=============================================================================

BaseStatusBar::StatusBarBroadcastReceiver::StatusBarBroadcastReceiver(
    /* [in] */ BaseStatusBar* bar)
    : mHost(bar)
{
}

ECode BaseStatusBar::StatusBarBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &mHost->mCurrentUserId);
        if (TRUE) Slogger::V(TAG, "userId %d is inthe house.", mHost->mCurrentUserId);
        mHost->UserSwitched(mHost->mCurrentUserId);
    }
    return NOERROR;
}

//=============================================================================
//              BaseStatusBar::ActivityOptionsOnAnimationStartedListener
//=============================================================================
CAR_INTERFACE_IMPL(BaseStatusBar::ActivityOptionsOnAnimationStartedListener, IActivityOptionsOnAnimationStartedListener)

BaseStatusBar::ActivityOptionsOnAnimationStartedListener::ActivityOptionsOnAnimationStartedListener(
    /* [in] */ BaseStatusBar* bar)
    : mHost(bar)
{
}

ECode BaseStatusBar::ActivityOptionsOnAnimationStartedListener::OnAnimationStarted()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::WINDOW_ANIMATION_START_INTENT, (IIntent**)&intent);
    String packageName("SystemUI"); //"com.android.systemui"
    intent->SetPackage(packageName);
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
    return mHost->mContext->SendBroadcastAsUser(intent, userHandle);
}

//=============================================================================
//              BaseStatusBar
//=============================================================================

BaseStatusBar::BaseStatusBar()
    : mPanelSlightlyVisible(FALSE)
    , mCurrentUserId(0)
    , mDeviceProvisioned(FALSE)
{
    mHandler = new MyHandler(this);
    mNotificationData = new NotificationData();

    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mProvisioningObserver = new ProvisioningObserver(handler, this);
    mOnClickHandler = new StatusBarRemoteViewsOnClickHandler(this);
    mRecentsPreloadOnTouchListener = new RecentsPreloadOnTouchListener(this);
}

BaseStatusBar::~BaseStatusBar()
{}

AutoPtr<IIStatusBarService> BaseStatusBar::GetStatusBarService()
{
    return mBarService;
}

Boolean BaseStatusBar::IsDeviceProvisioned()
{
    return TRUE;
    // TODO return mDeviceProvisioned;
}

ECode BaseStatusBar::Start()
{
    AutoPtr<IInterface> wmObj;

    assert(mContext != NULL);
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wmObj);
    mWindowManager = IWindowManager::Probe(wmObj.Get());
    assert(mWindowManager != NULL);

    AutoPtr<IWindowManagerGlobal> wmg;
    CWindowManagerGlobal::AcquireSingleton((IWindowManagerGlobal**)&wmg);
    wmg->GetWindowManagerService((IIWindowManager**)&mWindowManagerService);
    assert(mWindowManagerService != NULL);
    mWindowManager->GetDefaultDisplay((IDisplay**)&mDisplay);

    mProvisioningObserver->OnChange(FALSE); // set up
    AutoPtr<IContentResolver> resolver;

    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    ASSERT_SUCCEEDED(CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal));
    AutoPtr<IUri> uri;
    settingsGlobal->GetUriFor(ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&uri);

    //TODO resolver->RegisterContentObserver(uri, TRUE, mProvisioningObserver);

    AutoPtr<IInterface> sbsObj = ServiceManager::GetService(IContext::STATUS_BAR_SERVICE);
    mBarService = IIStatusBarService::Probe(sbsObj.Get());
    assert(mBarService != NULL && "Error: StatusBar Service not started!");

    // Connect in to the status bar manager service
    AutoPtr<IStatusBarIconList> iconList;

    CCommandQueue::New(THIS_PROBE(ICommandQueueCallbacks), (ICommandQueue**)&mCommandQueue);
    AutoPtr<IObjectContainer> notificationKeys, notifications, binders;
    AutoPtr<ArrayOf<Int32> > switches;

    // try {
    if (mBarService != NULL) {
        mBarService->RegisterStatusBar(
            IIStatusBar::Probe(mCommandQueue.Get()),
            (IStatusBarIconList**)&iconList,
            (IObjectContainer**)&notificationKeys,
            (IObjectContainer**)&notifications,
            (ArrayOf<Int32>**)&switches,
            (IObjectContainer**)&binders);
        mCommandQueue->SetIconList(iconList);
    }
    // } catch (RemoteException ex) {
    //    // If the system process isn't there we're doomed anyway.
    // }

    CreateAndAddWindows();

    assert(switches != NULL);
    Disable((*switches)[0]);
    SetSystemUiVisibility((*switches)[1], 0xffffffff);
    TopAppWindowChanged((*switches)[2] != 0);

    // StatusBarManagerService has a back up of IME token and it's restored here.
    assert(binders != NULL);
    Int32 bindersCount;
    binders->GetObjectCount(&bindersCount);
    if (bindersCount > 0) {
        AutoPtr<IObjectEnumerator> emu;
        binders->GetObjectEnumerator((IObjectEnumerator**)&emu);
        Boolean hasNext;
        emu->MoveNext(&hasNext);
        assert(hasNext);
        AutoPtr<IInterface> tmpObj;
        emu->Current((IInterface**)&tmpObj);
        AutoPtr<IBinder> binder = IBinder::Probe(tmpObj.Get());
        SetImeWindowStatus(binder, (*switches)[3], (*switches)[4]);
    }

    SetHardKeyboardStatus((*switches)[5] != 0, (*switches)[6] != 0);

    // Set up the initial icon state
    assert(iconList != NULL);
    Int32 N;
    iconList->Size(&N);
    Int32 viewIndex = 0;
    String slot;
    for (Int32  i = 0; i < N; i++) {
        AutoPtr<IStatusBarIcon> icon;
        iconList->GetIcon(i, (IStatusBarIcon**)&icon);
        if (icon != NULL) {
            String slot;
            iconList->GetSlot(i, &slot);
            AddIcon(slot, i, viewIndex, icon);
            viewIndex++;
        }
    }

    // Set up the initial notification state
    assert(notificationKeys != NULL);
    notificationKeys->GetObjectCount(&N);
    Int32 nCount;
    notifications->GetObjectCount(&nCount);

    if (N == nCount && N > 0) {
        AutoPtr<IObjectEnumerator> keyEmu, nEmu;
        notificationKeys->GetObjectEnumerator((IObjectEnumerator**)&keyEmu);
        notifications->GetObjectEnumerator((IObjectEnumerator**)&nEmu);
        Boolean hasNext;
        while(keyEmu->MoveNext(&hasNext), hasNext) {
            nEmu->MoveNext(&hasNext);
            AutoPtr<IInterface> keyObj, obj;
            keyEmu->Current((IInterface**)&keyObj);
            nEmu->Current((IInterface**)&obj);

            AddNotification(
                IBinder::Probe(keyObj.Get()),
                IStatusBarNotification::Probe(obj.Get()));
        }
    } else {
       // Log.wtf(TAG, "Notification list length mismatch: keys=" + N
       //         + " notifications=" + notifications.size());
    }

    // if (DEBUG) {
    //    Slogger::D(TAG, String.format(
    //            "init: icons=%d disabled=0x%08x lights=0x%08x menu=0x%08x imeButton=0x%08x",
    //           iconList.size(),
    //           switches[0],
    //           switches[1],
    //           switches[2],
    //           switches[3]
    //           ));
    // }

    mCurrentUserId = CActivityManager::GetCurrentUser();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);

    AutoPtr<StatusBarBroadcastReceiver> receiver = new StatusBarBroadcastReceiver(this);
    AutoPtr<IIntent> stickyIntent;
    mContext->RegisterReceiver(receiver, filter, (IIntent**)&stickyIntent);
    return NOERROR;
}

ECode BaseStatusBar::UserSwitched(
    /* [in] */ Int32 newUserId)
{
   // should be overridden
   return NOERROR;
}

Boolean BaseStatusBar::NotificationIsForCurrentUser(
    /* [in] */ IStatusBarNotification* n)
{
   Int32 thisUserId = mCurrentUserId;
   Int32 notificationUserId;
   n->GetUserId(&notificationUserId);
   if (DEBUG && MULTIUSER_DEBUG) {
        String str;
        n->ToString(&str);
        Slogger::V(TAG, "%s: current userid: %d, notification userid: %d, mCurrentUserId: %d",
            str.string(), thisUserId, notificationUserId, mCurrentUserId);
   }
   return notificationUserId == IUserHandle::USER_ALL
           || thisUserId == notificationUserId;
}

AutoPtr<IView> BaseStatusBar::UpdateNotificationVetoButton(
    /* [in] */ IView* row,
    /* [in] */ IStatusBarNotification* n)
{
   AutoPtr<IView> vetoButton;
   row->FindViewById(SystemUIR::id::veto, (IView**)&vetoButton);
   assert(vetoButton != NULL);

   Boolean isClearable;
   if (n->IsClearable(&isClearable), isClearable) {
       String pkg, tag;
       Int32 id;
       n->GetPkg(&pkg);
       n->GetTag(&tag);
       n->GetId(&id);
       AutoPtr<VetoButtonClicker> clicker = new VetoButtonClicker(pkg, tag, id, this);

       vetoButton->SetOnClickListener(clicker);
       vetoButton->SetVisibility(IView::VISIBLE);
   } else {
       vetoButton->SetVisibility(IView::GONE);
   }
   vetoButton->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_NO);
   return vetoButton;
}

ECode BaseStatusBar::ApplyLegacyRowBackground(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */  IView* content)
{
    AutoPtr<INotification> notification;
    sbn->GetNotification((INotification**)&notification);
    AutoPtr<IRemoteViews> contentView;
    notification->GetContentView((IRemoteViews**)&contentView);
    Int32 layoutId;
    contentView->GetLayoutId(&layoutId);
    if (layoutId != R::layout::notification_template_base) {
       Int32 version = 0;
       // try {
       AutoPtr<IPackageManager> pm;
       mContext->GetPackageManager((IPackageManager**)&pm);
       AutoPtr<IApplicationInfo> info;
       String pkg;
       sbn->GetPkg(&pkg);
       pm->GetApplicationInfo(pkg, 0, (IApplicationInfo**)&info);
       info->GetTargetSdkVersion(&version);
       // } catch (NameNotFoundException ex) {
       //     Slog.e(TAG, "Failed looking up ApplicationInfo for " + sbn.pkg, ex);
       // }
       if (version > 0 && version < Build::VERSION_CODES::GINGERBREAD) {
           content->SetBackgroundResource(SystemUIR::drawable::notification_row_legacy_bg);
       } else {
           content->SetBackgroundResource(R::drawable::notification_bg);
       }
   }
   return NOERROR;
}

ECode BaseStatusBar::StartApplicationDetailsActivity(
    /* [in] */ const String& packageName)
{
    AutoPtr<IUri> uri;
    Uri::FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> cn;
    intent->ResolveActivity(pm, (IComponentName**)&cn);
    intent->SetComponent(cn);

    AutoPtr<ITaskStackBuilderHelper> helper;
    assert(0 && "TODO");
    // CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
    AutoPtr<ITaskStackBuilder> builder;
    helper->Create(mContext, (ITaskStackBuilder**)&builder);
    builder->AddNextIntentWithParentStack(intent);
    builder->StartActivities(NULL, UserHandle::CURRENT);
    return NOERROR;
}

AutoPtr<IViewOnLongClickListener> BaseStatusBar::GetNotificationLongClicker()
{
    AutoPtr<IViewOnLongClickListener> clicker = new NotificationLongClicker(this);
    return clicker;
}

ECode BaseStatusBar::DismissPopups()
{
   if (mNotificationBlamePopup != NULL) {
       mNotificationBlamePopup->Dismiss();
       mNotificationBlamePopup = NULL;
   }
   return NOERROR;
}

ECode BaseStatusBar::DismissIntruder()
{
   // pass
   return NOERROR;
}

//@Override
ECode BaseStatusBar::ToggleRecentApps()
{
    Int32 msg = MSG_TOGGLE_RECENTS_PANEL;
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

//@Override
ECode BaseStatusBar::PreloadRecentApps()
{
    Int32 msg = MSG_PRELOAD_RECENT_APPS;
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

//@Override
ECode BaseStatusBar::CancelPreloadRecentApps()
{
    Int32 msg = MSG_CANCEL_PRELOAD_RECENT_APPS;
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

//@Override
ECode BaseStatusBar::ShowSearchPanel()
{
    Int32 msg = MSG_OPEN_SEARCH_PANEL;
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

//@Override
ECode BaseStatusBar::HideSearchPanel()
{
    Int32 msg = MSG_CLOSE_SEARCH_PANEL;
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

ECode BaseStatusBar::UpdateSearchPanel()
{
    // Search Panel
    Boolean visible = FALSE;
    if (mSearchPanelView != NULL) {
        mSearchPanelView->IsShowing(&visible);
        mWindowManager->RemoveView(mSearchPanelView);
    }

    // Provide SearchPanel with a temporary parent to allow layout params to work.
    AutoPtr<ILinearLayout> tmpRoot;
    CLinearLayout::New(mContext, (ILinearLayout**)&tmpRoot);
    AutoPtr<ILayoutInflater> li;
    LayoutInflater::From(mContext, (ILayoutInflater**)&li);
    AutoPtr<IView> spv;
    li->Inflate(SystemUIR::layout::status_bar_search_panel,
        IViewGroup::Probe(tmpRoot.Get()), FALSE, (IView**)&spv);
    mSearchPanelView = ISearchPanelView::Probe(spv.Get());
    assert(mSearchPanelView != NULL);

    AutoPtr<IViewOnTouchListener> listener = new TouchOutsideListener(
        MSG_CLOSE_SEARCH_PANEL, IStatusBarPanel::Probe(mSearchPanelView.Get()), this);
    mSearchPanelView->SetOnTouchListener(listener);
    mSearchPanelView->SetVisibility(IView::GONE);

    AutoPtr<IViewGroupLayoutParams> params;
    mSearchPanelView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<IWindowManagerLayoutParams> lp = GetSearchLayoutParams(IViewGroupLayoutParams::Probe(params.Get()));

    mWindowManager->AddView(mSearchPanelView, lp);
    mSearchPanelView->SetBar(THIS_PROBE(IBaseStatusBar));
    if (visible) {
        mSearchPanelView->Show(TRUE, FALSE);
    }
    return NOERROR;
}

ECode BaseStatusBar::SendCloseSystemWindows(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason)
{
   if (ActivityManagerNative::IsSystemReady()) {
       // try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        am->CloseSystemDialogs(reason);
       // } catch (RemoteException e) {
       // }
   }
   return NOERROR;
}

AutoPtr<IView> BaseStatusBar::GetStatusBarView()
{
    return NULL;
}

ECode BaseStatusBar::ToggleRecentsActivity()
{
    // try {
    AutoPtr<ITaskDescription> firstTask;
    AutoPtr<IRecentTasksLoader> loader;
    loader = CRecentTasksLoader::GetInstance(mContext);
    loader->GetFirstTask((ITaskDescription**)&firstTask);

    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::TOGGLE_RECENTS_INTENT, (IIntent**)&intent);
    String packageName("SystemUI"); //("Elastos.Droid.Core.eco"); //"com.android.systemui"
    String className("SystemUI.CRecentsActivityOne"); //("Elastos.Droid.SystemUI.Recent.RecentsActivity");//"com.android.systemui.recent.RecentsActivity"
    intent->SetClassName(packageName, className);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
    if (firstTask == NULL) {
        // // if (RecentsActivity::ForceOpaqueBackground(mContext)) {
        AutoPtr<IWallpaperManagerHelper> helper;
        CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&helper);
        AutoPtr<IWallpaperManager> manager;
        helper->GetInstance(mContext, (IWallpaperManager**)&manager);
        AutoPtr<IWallpaperInfo> info;
        manager->GetWallpaperInfo((IWallpaperInfo**)&info);
        if (info != NULL) {
            AutoPtr<IActivityOptionsHelper> helper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
            AutoPtr<IActivityOptions> opts;
            helper->MakeCustomAnimation(mContext,
                   SystemUIR::anim::recents_launch_from_launcher_enter,
                   SystemUIR::anim::recents_launch_from_launcher_exit,
                   (IActivityOptions**)&opts);
            AutoPtr<IBundle> bundle;
            opts->ToBundle((IBundle**)&bundle);
            mContext->StartActivityAsUser(intent, bundle, userHandle);
        }
        else {
            // The correct window animation will be applied via the activity's style
            mContext->StartActivityAsUser(intent, userHandle);
        }
    }
    else {
        AutoPtr<IBitmap> first;
        firstTask->GetThumbnail((IBitmap**)&first);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);

        Int32 thumbWidth, thumbHeight;
        res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_width, &thumbWidth);
        res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_height, &thumbHeight);
        if (first == NULL) {
           // throw new RuntimeException("Recents thumbnail is NULL");
           return E_RUNTIME_EXCEPTION;
        }

        Int32 w, h;
        first->GetWidth(&w);
        first->GetHeight(&h);
        if (w != thumbWidth || h != thumbHeight) {
            AutoPtr<IBitmapFactory> factory;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
            AutoPtr<IBitmap> scaledBitmap;
            factory->CreateScaledBitmap(first, thumbWidth, thumbHeight,
                   TRUE, (IBitmap**)&scaledBitmap);
            first = scaledBitmap;
            if (first == NULL) {
                // throw new RuntimeException("Recents thumbnail is NULL");
                return E_RUNTIME_EXCEPTION;
            }
        }

        AutoPtr<IDisplayMetrics> dm;
        CDisplayMetrics::New((IDisplayMetrics**)&dm);
        mDisplay->GetMetrics(dm);
        Int32 wp, hp;
        dm->GetWidthPixels(&wp);
        dm->GetHeightPixels(&hp);
        // calculate it here, but consider moving it elsewhere
        // first, determine which orientation you're in.
        // todo: move the system_bar layouts to sw600dp ?
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        Int32 x, y;

        Int32 orientation;
        config->GetOrientation(&orientation);
        if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
            Int32 appLabelLeftMargin, appLabelWidth, thumbLeftMargin, thumbBgPadding;
            Int32 thumbnailHeight, thumbnailTopMargin, textDescriptionPadding, appLabelTextSize;
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_app_label_left_margin, &appLabelLeftMargin);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_app_label_width, &appLabelWidth);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_left_margin, &thumbLeftMargin);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_bg_padding, &thumbBgPadding);

            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_height, &thumbnailHeight);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_top_margin, &thumbnailTopMargin);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_text_description_padding, &textDescriptionPadding);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_app_label_text_size, &appLabelTextSize);

            Float width = appLabelLeftMargin
                       + appLabelWidth
                       + thumbLeftMargin
                       + thumbWidth
                       + 2 * thumbBgPadding;

           x = (Int32) ((wp - width) / 2.0f + appLabelLeftMargin + appLabelWidth
                   + thumbBgPadding + thumbLeftMargin);
           y = (Int32) (hp - thumbnailHeight - thumbBgPadding);
        }
        else { // if (config.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            Int32 thumbTopMargin, thumbBgPadding, textPadding, labelTextSize, descriptionTextSize;
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_top_margin, &thumbTopMargin);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_bg_padding, &thumbBgPadding);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_text_description_padding, &textPadding);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_app_label_text_size, &labelTextSize);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_app_description_text_size, &descriptionTextSize);

            AutoPtr<IPaint> p;
            CPaint::New((IPaint**)&p);
            p->SetTextSize(labelTextSize);
            AutoPtr<IPaintFontMetricsInt> pfmi;
            p->GetFontMetricsInt((IPaintFontMetricsInt**)&pfmi);
            Int32 top, bottom;
            pfmi->GetTop(&top);
            pfmi->GetBottom(&bottom);
            Int32 labelTextHeight = bottom - top;
            p->SetTextSize(descriptionTextSize);
            Int32 descriptionTextHeight = bottom - top;

            Int32 statusBarHeight;
            res->GetDimensionPixelSize(R::dimen::status_bar_height, &statusBarHeight);
            Int32 recentsItemTopPadding = statusBarHeight;

            Int32 height = thumbTopMargin
                    + thumbHeight
                    + 2 * thumbBgPadding + textPadding + labelTextHeight
                    + recentsItemTopPadding + textPadding + descriptionTextHeight;
            Int32 recentsItemRightPadding, recentsScrollViewRightPadding, thumbWidth;
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_item_padding, &recentsItemRightPadding);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_right_glow_margin, &recentsScrollViewRightPadding);
            res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_recents_thumbnail_width, &thumbWidth);

            x = (Int32) (wp - thumbWidth - thumbBgPadding
                    - recentsItemRightPadding - recentsScrollViewRightPadding);
            y = (Int32) ((hp - statusBarHeight - height) / 2.0f + thumbTopMargin
                    + recentsItemTopPadding + thumbBgPadding + statusBarHeight);
        }

        AutoPtr<IView> statusBarView = GetStatusBarView();
        AutoPtr<IActivityOptionsOnAnimationStartedListener> listener;
        listener = new ActivityOptionsOnAnimationStartedListener(this);

        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        AutoPtr<IActivityOptions> opts;
        helper->MakeThumbnailScaleDownAnimation(
            statusBarView, first, x, y, listener, (IActivityOptions**)&opts);
        intent->PutBooleanExtra(IRecentsActivity::WAITING_FOR_WINDOW_ANIMATION_PARAM, TRUE);
        AutoPtr<IBundle> bundle;
        opts->ToBundle((IBundle**)&bundle);
        mContext->StartActivityAsUser(intent, bundle, userHandle);
    }
    // } catch (ActivityNotFoundException e) {
    //     Log.e(TAG, "Failed to launch RecentAppsIntent", e);
    // }
    return NOERROR;
}

ECode BaseStatusBar::PreloadRecentTasksList()
{
    if (DEBUG) Slogger::D(TAG, "preloading recents");
    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::PRELOAD_INTENT, (IIntent**)&intent);
    String packageName("SystemUI"); //("Elastos.Droid.Core.eco"); //"com.android.systemui"
    String className("SystemUI.CRecentsPreloadReceiver"); //"com.android.systemui.recent.RecentsActivity"
    intent->SetClassName(packageName, className);
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
    mContext->SendBroadcastAsUser(intent, userHandle);

    CRecentTasksLoader::GetInstance(mContext)->PreloadFirstTask();
    return NOERROR;
}

ECode BaseStatusBar::CancelPreloadingRecentTasksList()
{
    if (DEBUG) Slogger::D(TAG, "cancel preloading recents");
    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::CANCEL_PRELOAD_INTENT, (IIntent**)&intent);
    String packageName("SystemUI"); //("Elastos.Droid.Core.eco"); //"com.android.systemui"
    String className("SystemUI.CRecentsPreloadReceiver"); //"com.android.systemui.recent.RecentsActivity"
    intent->SetClassName(packageName, className);
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
    mContext->SendBroadcastAsUser(intent, userHandle);

    CRecentTasksLoader::GetInstance(mContext)->CancelPreloadingFirstTask();
    return NOERROR;
}

ECode BaseStatusBar::HandleShowIntruder()
{
    return NOERROR;
}

ECode BaseStatusBar::HandleHideIntruder()
{
    return NOERROR;
}

ECode BaseStatusBar::WorkAroundBadLayerDrawableOpacity(
    /* [in] */ IView* v)
{
    return NOERROR;
}

Boolean BaseStatusBar::InflateViews(
    /* [in] */ NotificationDataEntry* entry,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 minHeight, maxHeight;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_min_height, &minHeight);
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_max_height, &maxHeight);

    AutoPtr<IStatusBarNotification> sbn = entry->mNotification;
    AutoPtr<INotification> n;
    sbn->GetNotification((INotification**)&n);

    if (DEBUG) {
        String sbnStr;
        sbn->ToString(&sbnStr);
        Slogger::D(TAG, "InflateViews: notification: [%s]", sbnStr.string());
    }

    AutoPtr<IRemoteViews> oneU, large;
    n->GetContentView((IRemoteViews**)&oneU);
    n->GetBigContentView((IRemoteViews**)&large);

    if (oneU == NULL) {
        return FALSE;
    }

    // create the row view
    AutoPtr<IInterface> liObj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&liObj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(liObj.Get());
    AutoPtr<IView> row;
    inflater->Inflate(SystemUIR::layout::status_bar_notification_row,
        parent, FALSE, (IView**)&row);

    // for blaming (see SwipeHelper->SetLongPressListener)
    String pkg, tag;
    Int32 id;
    sbn->GetPkg(&pkg);
    sbn->GetTag(&tag);
    sbn->GetId(&id);

    AutoPtr<ICharSequence> seq;
    CString::New(pkg, (ICharSequence**)&seq);
    row->SetTag(seq);

    WorkAroundBadLayerDrawableOpacity(row);
    AutoPtr<IView> vetoButton = UpdateNotificationVetoButton(row, sbn);
    String contentDes;
    mContext->GetString(SystemUIR::string::accessibility_remove_notification, &contentDes);
    seq = NULL;
    CString::New(contentDes, (ICharSequence**)&seq);
    vetoButton->SetContentDescription(seq);

    // NB: the large icon is now handled entirely by the template

    // bind the click event to the content area
    AutoPtr<IView> tmpObj;
    row->FindViewById(SystemUIR::id::content, (IView**)&tmpObj);
    AutoPtr<IViewGroup> content = IViewGroup::Probe(tmpObj.Get());

    tmpObj = NULL;
    row->FindViewById(SystemUIR::id::adaptive, (IView**)&tmpObj);
    AutoPtr<IViewGroup> adaptive = IViewGroup::Probe(tmpObj.Get());

    content->SetDescendantFocusability(IViewGroup::FOCUS_BLOCK_DESCENDANTS);

    AutoPtr<IPendingIntent> contentIntent;
    n->GetContentIntent((IPendingIntent**)&contentIntent);
    if (contentIntent != NULL) {
        if (DEBUG) {
            Slogger::D(TAG, "InflateViews: SetOnClickListener");
        }
        AutoPtr<IViewOnClickListener> listener = new NotificationClicker(contentIntent, pkg, tag, id, this);
        content->SetOnClickListener(listener);
    }
    else {
        content->SetOnClickListener(NULL);
    }

    // TODO(cwren) normalize variable names with those in updateNotification
    AutoPtr<IView> expandedOneU;
    AutoPtr<IView> expandedLarge;
    // try {
       ECode ec = oneU->Apply(mContext, adaptive, mOnClickHandler, (IView**)&expandedOneU);
       if (ec == (ECode)E_RUNTIME_EXCEPTION) return FALSE;
       if (large != NULL) {
           ec = large->Apply(mContext, adaptive, mOnClickHandler, (IView**)&expandedLarge);
           if (ec == (ECode)E_RUNTIME_EXCEPTION) return FALSE;
       }

    // catch (RuntimeException e) {
    //    final String ident = sbn.pkg + "/0x" + Integer.toHexString(sbn.id);
    //    Slog.e(TAG, "couldn't inflate view for notification " + ident, e);
    //    return FALSE;
    // }

    if (expandedOneU != NULL) {
        AutoPtr<IViewGroupLayoutParams> lp;
        expandedOneU->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        AutoPtr<ISizeAdaptiveLayoutLayoutParams> params;
        CSizeAdaptiveLayoutLayoutParams::New(lp, (ISizeAdaptiveLayoutLayoutParams**)&params);
        params->SetMinHeight(minHeight);
        params->SetMaxHeight(minHeight);

        adaptive->AddView(expandedOneU, IViewGroupLayoutParams::Probe(params));
    }

    if (expandedLarge != NULL) {
        AutoPtr<IViewGroupLayoutParams> lp;
        expandedLarge->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        AutoPtr<ISizeAdaptiveLayoutLayoutParams> params;
        CSizeAdaptiveLayoutLayoutParams::New(lp, (ISizeAdaptiveLayoutLayoutParams**)&params);
        params->SetMinHeight(minHeight + 1);
        params->SetMaxHeight(maxHeight);

        adaptive->AddView(expandedLarge, IViewGroupLayoutParams::Probe(params));
    }
    row->SetDrawingCacheEnabled(TRUE);

    ApplyLegacyRowBackground(sbn, content);

    AutoPtr<IBoolean> bVal;
    CBoolean::New(large != NULL, (IBoolean**)&bVal);
    row->SetTag(SystemUIR::id::expandable_tag, bVal);

    // if (MULTIUSER_DEBUG) {
    //    TextView debug = (TextView) row.findViewById(R.id.debug_info);
    //    if (debug != NULL) {
    //        debug->SetVisibility(View.VISIBLE);
    //        debug->SetText("U " + entry.notification.getUserId());
    //    }
    // }
    entry->mRow = row;
    entry->mContent = content;
    entry->mExpanded = expandedOneU;
    entry->SetLargeView(expandedLarge);

    return TRUE;
}

AutoPtr<IViewOnClickListener> BaseStatusBar::MakeClicker(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    AutoPtr<IViewOnClickListener> clicker = (IViewOnClickListener*)(new NotificationClicker(intent, pkg, tag, id, this));
    return clicker;
}

ECode BaseStatusBar::VisibilityChanged(
    /* [in] */ Boolean visible)
{
   if (mPanelSlightlyVisible != visible) {
       mPanelSlightlyVisible = visible;
       // try {
        if (mBarService != NULL)
            mBarService->OnPanelRevealed();
       // } catch (RemoteException ex) {
       //     // Won't fail unless the world has ended.
       // }
   }
   return NOERROR;
}

ECode BaseStatusBar::HandleNotificationError(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ const String& message)
{
    if (DEBUG) {
        Slogger::W(TAG, "HandleNotificationError %s", message.string());
    }

    RemoveNotification(key);
    // try {
    if (mBarService != NULL) {
        String pkg, tag;
        Int32 id, uid, initialPid;
        n->GetPkg(&pkg);
        n->GetTag(&tag);
        n->GetId(&id);
        n->GetUid(&uid);
        n->GetInitialPid(&initialPid);
        mBarService->OnNotificationError(pkg, tag, id, uid, initialPid, message);
    }
   // } catch (RemoteException ex) {
   //     // The end is nigh.
   // }
   return NOERROR;
}

AutoPtr<IStatusBarNotification> BaseStatusBar::RemoveNotificationViews(
    /* [in] */ IBinder* key)
{
   AutoPtr<NotificationDataEntry> entry = mNotificationData->Remove(key);
   if (entry == NULL) {
       Slogger::W(TAG, "removeNotification for unknown key: %p", key);
       return NULL;
   }
   // Remove the expanded view.
   AutoPtr<IViewParent> vp;
   entry->mRow->GetParent((IViewParent**)&vp);
   AutoPtr<IViewGroup> rowParent = IViewGroup::Probe(vp.Get());
   if (rowParent != NULL) rowParent->RemoveViewInLayout(entry->mRow);
   UpdateExpansionStates();
   UpdateNotificationIcons();

   return entry->mNotification;
}

AutoPtr<IStatusBarIconView> BaseStatusBar::AddNotificationViews(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    if (DEBUG) {
        String str;
        notification->ToString(&str);
        Slogger::D(TAG, "addNotificationViews(key=%p, notification=[%s]",
            key, str.string());
    }

    // Construct the icon.
    String pkg, tmpStr;
    notification->GetPkg(&pkg);
    Int32 id;
    notification->GetId(&id);
    AutoPtr<IUserHandle> user;
    notification->GetUser((IUserHandle**)&user);

    AutoPtr<INotification> n;
    notification->GetNotification((INotification**)&n);
    Int32 icon, iconLevel, number;
    n->GetIcon(&icon);
    n->GetIconLevel(&iconLevel);
    n->GetNumber(&number);
    AutoPtr<ICharSequence> tickerText;
    n->GetTickerText((ICharSequence**)&tickerText);

    StringBuilder sb(pkg);
    sb += "/0x";
    sb += StringUtils::Int32ToHexString(id);

    AutoPtr<IStatusBarIconView> iconView;
    CStatusBarIconView::New(mContext, sb.ToString(), n, (IStatusBarIconView**)&iconView);
    iconView->SetScaleType(ImageViewScaleType_CENTER_INSIDE);

    AutoPtr<IStatusBarIcon> ic;
    CStatusBarIcon::New(pkg, user, icon, iconLevel, number, tickerText, (IStatusBarIcon**)&ic);
    Boolean succeed;
    iconView->Set(ic, &succeed);
    if (!succeed) {
        ic->ToString(&tmpStr);
        StringBuilder tmpSb("Couldn't create icon: ");
        tmpSb += tmpStr;
        HandleNotificationError(key, notification,  tmpSb.ToString());
        return NULL;
    }

    // Construct the expanded view.
    AutoPtr<NotificationDataEntry> entry = new NotificationDataEntry(key, notification, iconView);
    if (!InflateViews(entry, mPile)) {
        notification->ToString(&tmpStr);
        StringBuilder tmpSb("Couldn't expand RemoteViews for: ");
        tmpSb += tmpStr;
        HandleNotificationError(key, notification, tmpSb.ToString());
        return NULL;
    }

    // Add the expanded view and icon.
    Int32 pos = mNotificationData->Add(entry);
    if (DEBUG) {
        String tmp;
        iconView->ToString(&tmp);
        Slogger::D(TAG, "addNotificationViews: added at %d, %s", pos, tmp.string());
    }

    UpdateExpansionStates();
    UpdateNotificationIcons();

    return iconView;
}

Boolean BaseStatusBar::ExpandView(
    /* [in] */ NotificationDataEntry* entry,
    /* [in] */ Boolean expand)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 rowHeight;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_row_min_height, &rowHeight);
    AutoPtr<IViewGroupLayoutParams> params;
    entry->mRow->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<IViewGroupLayoutParams> lp = IViewGroupLayoutParams::Probe(params);

    if (entry->Expandable()) {
        if (DEBUG) Slogger::D(TAG, "setting expanded row height to WRAP_CONTENT");
        lp->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
    }
    else {
        if (DEBUG) Slogger::D(TAG, "setting collapsed row height to %d", rowHeight);
        lp->SetHeight(rowHeight);
    }
    entry->mRow->SetLayoutParams(lp);
    return expand;
}

ECode BaseStatusBar::UpdateExpansionStates()
{
    Int32 N = mNotificationData->Size();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<NotificationDataEntry> entry = mNotificationData->Get(i);
        if (!entry->UserLocked()) {
            if (i == (N-1)) {
                if (DEBUG) Slogger::D(TAG, "expanding top notification at %d", i);
                ExpandView(entry, TRUE);
            }
            else {
                if (!entry->UserExpanded()) {
                    if (DEBUG) Slogger::D(TAG, "collapsing notification at %d", i);
                    ExpandView(entry, FALSE);
                }
                else {
                    if (DEBUG) Slogger::D(TAG, "ignoring user-modified notification at %d", i);
                }
            }
        }
        else {
            if (DEBUG) Slogger::D(TAG, "ignoring notification being held by user at %d", i);
        }
    }
    return NOERROR;
}

 Boolean BaseStatusBar::IsTopNotification(
    /* [in] */ IViewGroup* parent,
    /* [in] */ NotificationDataEntry* entry)
{
    if (parent == NULL)
        return FALSE;

    Int32 index;
    parent->IndexOfChild(entry->mRow, &index);
    return index == 0;
}

ECode BaseStatusBar::UpdateNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    if (DEBUG) {
        String sbnStr;
        notification->ToString(&sbnStr);
        Slogger::D(TAG, "UpdateNotification: key: %p, notification: [%s]",
            key, sbnStr.string());
    }

    ECode ec = NOERROR;
    AutoPtr<NotificationDataEntry> oldEntry = mNotificationData->FindByKey(key);
    if (oldEntry == NULL) {
        Slogger::W(TAG, "updateNotification for unknown key: %p", key);
        return NOERROR;
    }

    AutoPtr<IStatusBarNotification> oldNotification = oldEntry->mNotification;

    // XXX: modify when we do something more intelligent with the two content views

    AutoPtr<INotification> on, n;
    oldNotification->GetNotification((INotification**)&on);
    notification->GetNotification((INotification**)&n);

    AutoPtr<IRemoteViews> oldContentView, contentView, oldBigContentView, bigContentView;
    on->GetContentView((IRemoteViews**)&oldContentView);
    on->GetBigContentView((IRemoteViews**)&oldBigContentView);
    n->GetContentView((IRemoteViews**)&contentView);
    n->GetBigContentView((IRemoteViews**)&bigContentView);

    // if (DEBUG) {
    //     Slogger::D(TAG, "old notification: when=" + oldNotification.notification.when
    //     + " ongoing=" + oldNotification.isOngoing()
    //     + " expanded=" + oldEntry.expanded
    //     + " contentView=" + oldContentView
    //     + " bigContentView=" + oldBigContentView
    //     + " rowParent=" + oldEntry.row.getParent());
    //     Slogger::D(TAG, "new notification: when=" + notification.notification.when
    //     + " ongoing=" + oldNotification.isOngoing()
    //     + " contentView=" + contentView
    //     + " bigContentView=" + bigContentView);
    // }

    // Can we just reapply the RemoteViews in place?  If when didn't change, the order
    // didn't change.

    String pkg, opkg;
    oldContentView->GetPackage(&opkg);
    contentView->GetPackage(&pkg);

    Int32 layoutId, olayoutId;
    contentView->GetLayoutId(&layoutId);
    oldContentView->GetLayoutId(&olayoutId);

    // 1U is never NULL
    Boolean contentsUnchanged = oldEntry->mExpanded != NULL
        && !pkg.IsNull() && !opkg.IsNull() && opkg.Equals(pkg)
        && olayoutId == layoutId;

    // large view may be NULL
    AutoPtr<IView> olv = oldEntry->GetLargeView();
    Boolean bigContentsUnchanged = (olv == NULL && bigContentView == NULL);
    if (!bigContentsUnchanged) {
        bigContentsUnchanged = (olv != NULL && bigContentView != NULL);
        if (bigContentsUnchanged) {
            String bigPkg, oBigPkg;
            oldBigContentView->GetPackage(&oBigPkg);
            bigContentView->GetPackage(&bigPkg);
            Int32 oblayoutId, blayoutId;
            oldBigContentView->GetLayoutId(&oblayoutId);
            bigContentView->GetLayoutId(&blayoutId);
            bigContentsUnchanged = (!bigPkg.IsNull() && !oBigPkg.IsNull()
                && oBigPkg.Equals(bigPkg) && oblayoutId == blayoutId);
        }
    }

    AutoPtr<IViewParent> vp;
    oldEntry->mRow->GetParent((IViewParent**)&vp);
    AutoPtr<IViewGroup> rowParent = IViewGroup::Probe(vp.Get());

    Int64 when, owhen;
    Int32 score, oscore;
    on->GetWhen(&owhen);
    n->GetWhen(&when);
    notification->GetScore(&oscore);
    oldNotification->GetScore(&score);
    Boolean orderUnchanged = (when == owhen) && (score == oscore);
    // score now encompasses/supersedes isOngoing()

    AutoPtr<ICharSequence> tt, ott;
    on->GetTickerText((ICharSequence**)&ott);
    n->GetTickerText((ICharSequence**)&tt);
    Boolean updateTicker = (tt != NULL) && !TextUtils::Equals(tt, ott);
    Boolean isTopAnyway = IsTopNotification(rowParent, oldEntry);
    if (contentsUnchanged && bigContentsUnchanged && (orderUnchanged || isTopAnyway)) {
        if (DEBUG) Slogger::D(TAG, "reusing notification for key: %p", key);
        oldEntry->mNotification = notification;
        // try {
            // Reapply the RemoteViews
            contentView->Reapply(mContext, oldEntry->mExpanded, mOnClickHandler);
            if (bigContentView != NULL && olv != NULL) {
                bigContentView->Reapply(mContext, olv, mOnClickHandler);
            }

            String pkg, tag;
            notification->GetPkg(&pkg);
            notification->GetTag(&tag);
            Int32 id;
            notification->GetId(&id);

            // update the contentIntent
            AutoPtr<IPendingIntent> contentIntent;
            n->GetContentIntent((IPendingIntent**)&contentIntent);
            if (contentIntent != NULL) {
                AutoPtr<IViewOnClickListener> listener = MakeClicker(contentIntent, pkg, tag, id);
                oldEntry->mContent->SetOnClickListener(listener);
            }
            else {
                oldEntry->mContent->SetOnClickListener(NULL);
            }

            AutoPtr<IUserHandle> user;
            notification->GetUser((IUserHandle**)&user);
            Int32 icon, iconLevel, number;
            n->GetIcon(&icon);
            n->GetIconLevel(&iconLevel);
            n->GetNumber(&number);

            // Update the icon.
            AutoPtr<IStatusBarIcon> ic;
            CStatusBarIcon::New(pkg, user, icon, iconLevel, number, tt, (IStatusBarIcon**)&ic);
            Boolean succeed;
            ec = oldEntry->mIcon->Set(ic, &succeed);
            if (!succeed) {
                HandleNotificationError(key, notification, String("Couldn't update icon: "));
                return NOERROR;
            }

            UpdateExpansionStates();
        // } catch (RuntimeException e) {
        // It failed to add cleanly.  Log, and remove the view from the panel.
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            Slogger::W(TAG, "Couldn't reapply views for package %s", pkg.string());
            RemoveNotificationViews(key);
            AddNotificationViews(key, notification);
        }

        // }
    }
    else {
        if (DEBUG) Slogger::D(TAG, "not reusing notification for key: %p", key);
        if (DEBUG) Slogger::D(TAG, "contents was %s", (contentsUnchanged ? "unchanged" : "changed"));
        if (DEBUG) Slogger::D(TAG, "order was %s", (orderUnchanged ? "unchanged" : "changed"));
        if (DEBUG) Slogger::D(TAG, "notification is %s", (isTopAnyway ? "top" : "not top"));

        Boolean wasExpanded = oldEntry->UserExpanded();
        RemoveNotificationViews(key);
        AddNotificationViews(key, notification);
        if (wasExpanded) {
            AutoPtr<NotificationDataEntry> newEntry = mNotificationData->FindByKey(key);
            ExpandView(newEntry, TRUE);
            newEntry->SetUserExpanded(TRUE);
        }
    }

    // Update the veto button accordingly (and as a result, whether this row is
    // swipe-dismissable)
    UpdateNotificationVetoButton(oldEntry->mRow, notification);

    // Is this for you?
    Boolean isForCurrentUser = NotificationIsForCurrentUser(notification);
    if (DEBUG) Slogger::D(TAG, "notification is %sfor you", (isForCurrentUser ? "" : "not "));

    // Restart the ticker if it's still running
    if (updateTicker && isForCurrentUser) {
        HaltTicker();
        Tick(key, notification, FALSE);
    }

    // Recalculate the position of the sliding windows and the titles.
    SetAreThereNotifications();
    UpdateExpandedViewPos(IBaseStatusBar::EXPANDED_LEAVE_ALONE);

    // See if we need to update the intruder.
    if (ENABLE_INTRUDERS && oldNotification == mCurrentlyIntrudingNotification) {
        if (DEBUG) {
            String nstr;
            notification->ToString(&nstr);
            Slogger::D(TAG, "updating the current intruder:[%s]", nstr.string());
        }

        // XXX: this is a hack for Alarms. The real implementation will need to *update*
        // the intruder.
        AutoPtr<IPendingIntent> fullScreenIntent;
        n->GetFullScreenIntent((IPendingIntent**)&fullScreenIntent);
        if (fullScreenIntent == NULL) { // TODO(dsandler): consistent logic with add()
            if (DEBUG) Slogger::D(TAG, "no longer intrudes!");
            Boolean result;
            return mHandler->SendEmptyMessage(MSG_HIDE_INTRUDER, &result);
        }
    }

    return NOERROR;
}

Boolean BaseStatusBar::ShowNotificationEvenIfUnprovisioned(
    /* [in] */ IStatusBarNotification* sbn)
{
    String pkg;
    sbn->GetPkg(&pkg);
    if (pkg.Equals("android")) {
        AutoPtr<INotification> n;
        sbn->GetNotification((INotification**)&n);
        AutoPtr<ArrayOf<String> > kind;
        n->GetKind((ArrayOf<String>**)&kind);
        if (kind != NULL) {
            String aKind;
            for (Int32 i = 0; i < kind->GetLength(); ++i) {
                aKind = (*kind)[i];
                   // IME switcher, created by InputMethodManagerService
               if (aKind.Equals("android.system.imeswitcher")) return TRUE;
               // OTA availability & errors, created by SystemUpdateService
               if (aKind.Equals("android.system.update")) return TRUE;
            }
       }
   }
   return FALSE;
}

Boolean BaseStatusBar::InKeyguardRestrictedInputMode()
{
    return FALSE;
    // TODO
    // AutoPtr<IInterface> kmObj;
    // mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&kmObj);
    // AutoPtr<IKeyguardManager> km = IKeyguardManager::Probe(kmObj.Get());
    // Boolean result = FALSE;
    // if (km != NULL) {
    //     km->InKeyguardRestrictedInputMode(&result);
    // }
    // return result;
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
