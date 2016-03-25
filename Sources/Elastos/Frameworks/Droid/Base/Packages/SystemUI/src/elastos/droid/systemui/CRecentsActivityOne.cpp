#include "CRecentsActivityOne.h"
#include <elastos/utility/etl/List.h>
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::App::CWallpaperManagerHelper;
using Elastos::Droid::App::IWallpaperManagerHelper;
using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::App::IWallpaperInfo;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::SystemUI::StatusBar::Tablet::EIID_IStatusBarPanel;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::CRecentTasksLoaderHelper;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoaderHelper;
using Elastos::Droid::SystemUI::Recent::ITaskDescription;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::Recent::EIID_IRecentsActivity;

using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebug;
using Elastos::IO::CFile;
using Elastos::IO::CFileDescriptor;
using Libcore::IO::IOsConstants;
using Libcore::IO::COsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

const String CRecentsActivityOne::WAS_SHOWING("was_showing");

CAR_INTERFACE_IMPL(CRecentsActivityOne::TouchOutsideListener, IViewOnTouchListener)

CRecentsActivityOne::TouchOutsideListener::TouchOutsideListener(
    /* [in] */ CRecentsActivityOne* host,
    /* [in] */ IStatusBarPanel* panel)
    : mHost(host)
    , mPanel(panel)
{}

ECode CRecentsActivityOne::TouchOutsideListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    Int32 action;
    ev->GetAction(&action);
    Boolean b;
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    mPanel->IsInContentArea((Int32) x, (Int32) y, &b);
    if (action == IMotionEvent::ACTION_OUTSIDE
            || (action == IMotionEvent::ACTION_DOWN
            && !b)) {
        mHost->DismissAndGoHome();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

CRecentsActivityOne::IntentReceiver::IntentReceiver(
    /* [in] */ CRecentsActivityOne* host)
    : mHost(host)
{}

ECode CRecentsActivityOne::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IRecentsActivity::CLOSE_RECENTS_INTENT)) {
        Boolean b;
        mHost->mRecentsPanel->IsShowing(&b);
        if (mHost->mRecentsPanel != NULL && b) {
            if (mHost->mShowing && !mHost->mForeground) {
                // Captures the case right before we transition to another activity
                mHost->mRecentsPanel->Show(FALSE);
            }
        }
    }
    else if (action.Equals(IRecentsActivity::WINDOW_ANIMATION_START_INTENT)) {
        if (mHost->mRecentsPanel != NULL) {
            mHost->mRecentsPanel->OnWindowAnimationStart();
        }
    }
    return NOERROR;
}

CRecentsActivityOne::CRecentsActivityOne()
    : mShowing(FALSE)
    , mForeground(FALSE)
{
    mIntentReceiver = new IntentReceiver(this);
}

PInterface CRecentsActivityOne::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IRecentsActivity) {
        return this;
    }
    else return Activity::Probe(riid);
}

UInt32 CRecentsActivityOne::AddRef()
{
    return Activity::AddRef();
}

UInt32 CRecentsActivityOne::Release()
{
    return Activity::Release();
}

ECode CRecentsActivityOne::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IRecentsActivity *)this) {
        *iid = EIID_IRecentsActivity;
        return NOERROR;
    }
    else return Activity::GetInterfaceID(object, iid);
}

ECode CRecentsActivityOne::OnPause()
{
    OverridePendingTransition(
            R::anim::recents_return_to_launcher_enter,
            R::anim::recents_return_to_launcher_exit);
    mForeground = FALSE;
    return Activity::OnPause();
}

ECode CRecentsActivityOne::OnStop()
{
    mShowing = FALSE;
    if (mRecentsPanel != NULL) {
        mRecentsPanel->OnUiHidden();
    }
    return Activity::OnStop();
}

ECode CRecentsActivityOne::OnStart()
{
    // Hide wallpaper if it's not a static image
    if (ForceOpaqueBackground(this)) {
        UpdateWallpaperVisibility(FALSE);
    }
    else {
        UpdateWallpaperVisibility(TRUE);
    }
    mShowing = TRUE;
    if (mRecentsPanel != NULL) {
        mRecentsPanel->RefreshViews();
    }
    return Activity::OnStart();
}

ECode CRecentsActivityOne::OnResume()
{
    mForeground = TRUE;
    return Activity::OnResume();
}

ECode CRecentsActivityOne::OnBackPressed()
{
    return DismissAndGoBack();
}

ECode CRecentsActivityOne::DismissAndGoHome()
{
    if (mRecentsPanel != NULL) {
        AutoPtr<IIntent> homeIntent;
        CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&homeIntent);
        homeIntent->AddCategory(IIntent::CATEGORY_HOME);
        homeIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);

        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&userHandle);
        StartActivityAsUser(homeIntent, userHandle);
        mRecentsPanel->Show(FALSE);
    }
    return NOERROR;
}

ECode CRecentsActivityOne::DismissAndGoBack()
{
    if (mRecentsPanel != NULL) {
        AutoPtr<IActivityManager> am;
        GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

        AutoPtr<IObjectContainer> objectcontainer;
        am->GetRecentTasks(2,
            IActivityManager::RECENT_WITH_EXCLUDED |
            IActivityManager::RECENT_IGNORE_UNAVAILABLE,
            (IObjectContainer**)&objectcontainer);
        if (objectcontainer != NULL) {
            Int32 numTasks;
            objectcontainer->GetObjectCount(&numTasks);
            if (numTasks > 1) {
                AutoPtr<IObjectEnumerator> enumerator;
                objectcontainer->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                Boolean hasNext = FALSE;
                enumerator->MoveNext(&hasNext);
                enumerator->MoveNext(&hasNext);
                AutoPtr<IActivityManagerRecentTaskInfo> recentInfo;
                enumerator->Current((IInterface**)&recentInfo);
                Int32 id;
                recentInfo->GetPersistentId(&id);
                Boolean b;
                mRecentsPanel->SimulateClick(id, &b);
                if (b) {
                    // recents panel will take care of calling show(false) through simulateClick
                    return NOERROR;
                }
            }
        }
        mRecentsPanel->Show(FALSE);
    }
    Finish();
    return NOERROR;
}

ECode CRecentsActivityOne::IsActivityShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mShowing;
    return NOERROR;
}

Boolean CRecentsActivityOne::IsForeground()
{
    return mForeground;
}

Boolean CRecentsActivityOne::ForceOpaqueBackground(
    /* [in] */ IContext* context)
{
    AutoPtr<IWallpaperManagerHelper> helper;
    CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&helper);
    AutoPtr<IWallpaperManager> manager;
    helper->GetInstance(context, (IWallpaperManager**)&manager);
    AutoPtr<IWallpaperInfo> info;
    manager->GetWallpaperInfo((IWallpaperInfo**)&info);
    return info != NULL;
}

ECode CRecentsActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SetContentView(R::layout::status_bar_recent_panel);

    FindViewById(R::id::recents_root, (IView**)&mRecentsPanel);
    AutoPtr<IStatusBarPanel> panel = IStatusBarPanel::Probe(mRecentsPanel);
    AutoPtr<IViewOnTouchListener> l = new TouchOutsideListener(this, panel);
    mRecentsPanel->SetOnTouchListener(l);

    AutoPtr<IRecentTasksLoaderHelper> helper;
    CRecentTasksLoaderHelper::AcquireSingleton((IRecentTasksLoaderHelper**)&helper);
    AutoPtr<IRecentTasksLoader> recentTasksLoader;
    helper->GetInstance(this, (IRecentTasksLoader**)&recentTasksLoader);

    recentTasksLoader->SetRecentsPanel(mRecentsPanel, mRecentsPanel);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 val;
    res->GetInteger(R::integer::config_recent_item_min_alpha, &val);
    mRecentsPanel->SetMinSwipeAlpha(val / 100);

    Boolean b = FALSE;
    if (savedInstanceState == NULL || (savedInstanceState->GetBoolean(WAS_SHOWING, &b), b)) {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        HandleIntent(intent, (savedInstanceState == NULL));
    }

    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mIntentFilter->AddAction(IRecentsActivity::CLOSE_RECENTS_INTENT);
    mIntentFilter->AddAction(IRecentsActivity::WINDOW_ANIMATION_START_INTENT);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mIntentReceiver, mIntentFilter, (IIntent**)&intent);
    return Activity::OnCreate(savedInstanceState);
}

ECode CRecentsActivityOne::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Boolean b;
    mRecentsPanel->IsShowing(&b);
    return outState->PutBoolean(WAS_SHOWING, b);
}

ECode CRecentsActivityOne::OnDestroy()
{
    AutoPtr<IRecentTasksLoaderHelper> helper;
    CRecentTasksLoaderHelper::AcquireSingleton((IRecentTasksLoaderHelper**)&helper);
    AutoPtr<IRecentTasksLoader> recentTasksLoader;
    helper->GetInstance(this, (IRecentTasksLoader**)&recentTasksLoader);

    recentTasksLoader->SetRecentsPanel(NULL, mRecentsPanel);
    UnregisterReceiver(mIntentReceiver);
    return Activity::OnDestroy();
}

ECode CRecentsActivityOne::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    HandleIntent(intent, TRUE);
    return NOERROR;
}

void CRecentsActivityOne::UpdateWallpaperVisibility(
    /* [in] */ Boolean visible)
{
    Int32 wpflags = visible ? IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER : 0;
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    AutoPtr<IWindowManagerLayoutParams> attr;
    win->GetAttributes((IWindowManagerLayoutParams**)&attr);
    Int32 curflags;
    attr->GetFlags(&curflags);
    curflags = curflags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER;
    if (wpflags != curflags) {
        win->SetFlags(wpflags, IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
    }
}

void CRecentsActivityOne::HandleIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean checkWaitingForAnimationParam)
{
    Activity::OnNewIntent(intent);

    String action;
    intent->GetAction(&action);
    if (action.Equals(IRecentsActivity::TOGGLE_RECENTS_INTENT)) {
        if (mRecentsPanel != NULL) {
            Boolean b;
            mRecentsPanel->IsShowing(&b);
            if (b) {
                DismissAndGoBack();
            }
            else {
                AutoPtr<IRecentTasksLoaderHelper> helper;
                CRecentTasksLoaderHelper::AcquireSingleton((IRecentTasksLoaderHelper**)&helper);
                AutoPtr<IRecentTasksLoader> recentTasksLoader;
                helper->GetInstance(this, (IRecentTasksLoader**)&recentTasksLoader);

                intent->GetBooleanExtra(IRecentsActivity::WAITING_FOR_WINDOW_ANIMATION_PARAM, FALSE, &b);
                Boolean waitingForWindowAnimation = checkWaitingForAnimationParam && b;
                recentTasksLoader->IsFirstScreenful(&b);
                AutoPtr<IObjectContainer> tasks;
                recentTasksLoader->GetLoadedTasks((IObjectContainer**)&tasks);
                mRecentsPanel->Show(TRUE, tasks, b, waitingForWindowAnimation);
            }
        }
    }
}


}// namespace SystemUI
}// namespace DevSamples
}// namespace Droid
}// namespace Elastos
