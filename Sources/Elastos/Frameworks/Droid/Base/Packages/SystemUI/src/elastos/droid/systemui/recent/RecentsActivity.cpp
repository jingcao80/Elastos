#include "elastos/droid/systemui/recent/RecentsActivity.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CWallpaperManagerHelper;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::App::IWallpaperInfo;
using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::App::IWallpaperManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

static const String TAG("Recent::RecentsActivity");

//-------------------------------------------------------------
// RecentsActivity::IntentReceiver
//-------------------------------------------------------------

RecentsActivity::IntentReceiver::IntentReceiver(
    /* [in] */ RecentsActivity* host)
    : mHost(host)
{}

ECode RecentsActivity::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::I(TAG, " >> IntentReceiver: %s", action.string());
    if (IRecentsActivity::CLOSE_RECENTS_INTENT.Equals(action)) {
        Boolean isShowing;
        mHost->mRecentsPanel->IsShowing(&isShowing);
        if (mHost->mRecentsPanel != NULL && isShowing) {
            if (mHost->mShowing && !mHost->mForeground) {
                // Captures the case right before we transition to another activity
                mHost->mRecentsPanel->Show(FALSE);
            }
        }
    }
    else if (IRecentsActivity::WINDOW_ANIMATION_START_INTENT.Equals(action)) {
        if (mHost->mRecentsPanel != NULL) {
            mHost->mRecentsPanel->OnWindowAnimationStart();
        }
    }
    return NOERROR;
}

//-------------------------------------------------------------
// RecentsActivity::TouchOutsideListener
//-------------------------------------------------------------

CAR_INTERFACE_IMPL(RecentsActivity::TouchOutsideListener, Object, IViewOnTouchListener)

RecentsActivity::TouchOutsideListener::TouchOutsideListener(
    /* [in] */ IStatusBarPanel* panel,
    /* [in] */ RecentsActivity* host)
    : mHost(host)
    , mPanel(panel)
{
}

ECode RecentsActivity::TouchOutsideListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    Int32 action;
    ev->GetAction(&action);
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    Boolean isInContentArea;
    mPanel->IsInContentArea((Int32)x, (Int32)y, &isInContentArea);
    if (action == IMotionEvent::ACTION_OUTSIDE || (action == IMotionEvent::ACTION_DOWN
        && !isInContentArea)) {
        mHost->DismissAndGoHome();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//-------------------------------------------------------------
// RecentsActivity
//-------------------------------------------------------------
const String RecentsActivity::WAS_SHOWING("was_showing");

CAR_INTERFACE_IMPL(RecentsActivity, Activity, IRecentsActivity)

RecentsActivity::RecentsActivity()
    : mShowing(FALSE)
    , mForeground(FALSE)
{
}

ECode RecentsActivity::constructor()
{
    mIntentReceiver = new IntentReceiver(this);
    return Activity::constructor();
}

ECode RecentsActivity::OnPause()
{
    OverridePendingTransition(
        R::anim::recents_return_to_launcher_enter,
        R::anim::recents_return_to_launcher_exit);
    mForeground = FALSE;
    return Activity::OnPause();
}

ECode RecentsActivity::OnStop()
{
    mShowing = FALSE;
    if (mRecentsPanel != NULL) {
        mRecentsPanel->OnUiHidden();
    }
    return Activity::OnStop();
}

void RecentsActivity::UpdateWallpaperVisibility(
    /* [in] */ Boolean visible)
{
    Int32 wpflags = visible ? IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER : 0;
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> params;
    window->GetAttributes((IWindowManagerLayoutParams**)&params);
    Int32 flags;
    params->GetFlags(&flags);
    Int32 curflags = flags
        & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER;
    if (wpflags != curflags) {
        window->SetFlags(wpflags, IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
    }
}

ECode RecentsActivity::ForceOpaqueBackground(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IWallpaperManagerHelper> wmh;
    CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&wmh);
    AutoPtr<IWallpaperManager> wm;
    wmh->GetInstance(context, (IWallpaperManager**)&wm);
    AutoPtr<IWallpaperInfo> wi;
    wm->GetWallpaperInfo((IWallpaperInfo**)&wi);
    *result = wi != NULL;
    return NOERROR;
}

ECode RecentsActivity::OnStart()
{
    Logger::I(TAG, " >> OnStart");

    // Hide wallpaper if it's not a static image
    Boolean b;
    ForceOpaqueBackground(this, &b);
    if (b) {
        UpdateWallpaperVisibility(FALSE);
    }
    else {
        UpdateWallpaperVisibility(TRUE);
    }
    mShowing = TRUE;
    if (mRecentsPanel != NULL) {
        // Call and refresh the recent tasks list in case we didn't preload tasks
        // or in case we don't get an onNewIntent
        mRecentsPanel->RefreshRecentTasksList();
        mRecentsPanel->RefreshViews();
    }
    return Activity::OnStart();
}

ECode RecentsActivity::OnResume()
{
    mForeground = TRUE;
    return Activity::OnResume();
}

ECode RecentsActivity::OnBackPressed()
{
    return DismissAndGoBack();
}

ECode RecentsActivity::DismissAndGoHome()
{
    if (mRecentsPanel != NULL) {
        AutoPtr<IIntent> homeIntent;
        CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&homeIntent);
        homeIntent->AddCategory(IIntent::CATEGORY_HOME);
        homeIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&uh);
        StartActivityAsUser(homeIntent, uh);
        mRecentsPanel->Show(FALSE);
    }
    return NOERROR;
}

ECode RecentsActivity::DismissAndGoBack()
{
    if (mRecentsPanel != NULL) {
        AutoPtr<IInterface> amObj;
        GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&amObj);
        AutoPtr<IActivityManager> am = IActivityManager::Probe(amObj);
        AutoPtr<IList> recentTasks;
        am->GetRecentTasks(2,
            IActivityManager::RECENT_WITH_EXCLUDED |
            IActivityManager::RECENT_IGNORE_UNAVAILABLE |
            IActivityManager::RECENT_INCLUDE_PROFILES,
            (IList**)&recentTasks);
        Int32 size;
        recentTasks->GetSize(&size);
        if (size > 1) {
            AutoPtr<IInterface> obj;
            recentTasks->Get(1, (IInterface**)&obj);
            AutoPtr<IActivityManagerRecentTaskInfo> amrt = IActivityManagerRecentTaskInfo::Probe(obj);
            Int32 persistentId;
            amrt->GetPersistentId(&persistentId);
            Boolean b;
            mRecentsPanel->SimulateClick(persistentId, &b);
            if (b) {
                // recents panel will take care of calling show(false) through simulateClick
                return NOERROR;
            }
        }

        mRecentsPanel->Show(FALSE);
    }

    Finish();
    return NOERROR;
}

ECode RecentsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate: %s", TO_CSTR(savedInstanceState));

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddPrivateFlags(IWindowManagerLayoutParams::PRIVATE_FLAG_INHERIT_TRANSLUCENT_DECOR);
    SetContentView(R::layout::status_bar_recent_panel);

    AutoPtr<IView> view;
    FindViewById(R::id::recents_root, (IView**)&view);
    mRecentsPanel = IRecentsPanelView::Probe(view);
    assert(mRecentsPanel);

    AutoPtr<TouchOutsideListener> tol = new TouchOutsideListener(
        IStatusBarPanel::Probe(mRecentsPanel), this);
    view->SetOnTouchListener(tol);
    view->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE
        | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
        | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);

    AutoPtr<RecentTasksLoader> recentTasksLoader = RecentTasksLoader::GetInstance(this);
    recentTasksLoader->SetRecentsPanel(mRecentsPanel, mRecentsPanel);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 i;
    res->GetInteger(R::integer::config_recent_item_min_alpha, &i);
    mRecentsPanel->SetMinSwipeAlpha(i / 100.0f);
    Boolean b;
    if (savedInstanceState == NULL || (savedInstanceState->GetBoolean(WAS_SHOWING, &b), b)) {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        HandleIntent(intent, (savedInstanceState == NULL));
    }
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mIntentFilter->AddAction(CLOSE_RECENTS_INTENT);
    mIntentFilter->AddAction(WINDOW_ANIMATION_START_INTENT);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mIntentReceiver, mIntentFilter, (IIntent**)&intent);
    return Activity::OnCreate(savedInstanceState);
}

ECode RecentsActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Boolean isShowing;
    mRecentsPanel->IsShowing(&isShowing);
    outState->PutBoolean(WAS_SHOWING, isShowing);
    Logger::I(TAG, " >> OnSaveInstanceState: %s, value:%d", WAS_SHOWING.string(), isShowing);
    return NOERROR;
}

ECode RecentsActivity::OnDestroy()
{
    AutoPtr<RecentTasksLoader> recentTasksLoader = RecentTasksLoader::GetInstance(this);
    recentTasksLoader->SetRecentsPanel(NULL, mRecentsPanel);
    UnregisterReceiver(mIntentReceiver);
    return Activity::OnDestroy();
}

ECode RecentsActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    Logger::I(TAG, " >> OnNewIntent: %s", TO_CSTR(intent));
    HandleIntent(intent, TRUE);
    return NOERROR;
}

void RecentsActivity::HandleIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean checkWaitingForAnimationParam)
{
    Logger::I(TAG, " >>> HandleIntent:: %s", TO_CSTR(intent));
    Activity::OnNewIntent(intent);

    String action;
    intent->GetAction(&action);
    if (TOGGLE_RECENTS_INTENT.Equals(action)) {
        if (mRecentsPanel != NULL) {
            Boolean isShowing;
            mRecentsPanel->IsShowing(&isShowing);
            if (isShowing) {
                DismissAndGoBack();
            }
            else {
                AutoPtr<RecentTasksLoader> recentTasksLoader = RecentTasksLoader::GetInstance(this);
                Boolean b;
                intent->GetBooleanExtra(WAITING_FOR_WINDOW_ANIMATION_PARAM, FALSE, &b);
                Boolean waitingForWindowAnimation = checkWaitingForAnimationParam && b;
                AutoPtr<IArrayList> al;
                recentTasksLoader->GetLoadedTasks((IArrayList**)&al);
                Boolean isFirstScreenful;
                recentTasksLoader->IsFirstScreenful(&isFirstScreenful);
                mRecentsPanel->Show(TRUE, al, isFirstScreenful, waitingForWindowAnimation);
            }
        }
    }
}

Boolean RecentsActivity::IsForeground()
{
    return mForeground;
}

ECode RecentsActivity::IsActivityShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShowing;
    return NOERROR;
}


} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos