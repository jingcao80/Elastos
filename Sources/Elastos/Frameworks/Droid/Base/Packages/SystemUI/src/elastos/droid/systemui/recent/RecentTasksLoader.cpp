#include "elastos/droid/app/AppGlobals.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/systemui/recent/ColorDrawableWithDimensions.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"
#include "elastos/droid/systemui/recent/TaskDescription.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//======================================================================
// RecentTasksLoader::PreloadTasksRunnable
//======================================================================

RecentTasksLoader::PreloadTasksRunnable::PreloadTasksRunnable(
    /* [in] */ RecentTasksLoader* host)
    : mHost(host)
{}


ECode RecentTasksLoader::PreloadTasksRunnable::Run()
{
    return mHost->LoadTasksInBackground();
}

//======================================================================
// RecentTasksLoader::BgLoadThread
//======================================================================

RecentTasksLoader::BgLoadThread::BgLoadThread(
    /* [in] */ RecentTasksLoader* host)
    : mHost(host)
{
    Thread::constructor(String("BgLoadThread"));
}

ECode RecentTasksLoader::BgLoadThread::Run()
{
    AutoPtr<ITaskDescription> first;
    mHost->LoadFirstTask((ITaskDescription**)&first);

    synchronized(mHost->mFirstTaskLock) {
        if (mHost->mCancelPreloadingFirstTask) {
            mHost->ClearFirstTask();
        }
        else {
            mHost->mFirstTask = first;
            mHost->mFirstTaskLoaded = TRUE;
        }
        mHost->mPreloadingFirstTask = FALSE;
    }
    return NOERROR;
}

//======================================================================
// RecentTasksLoader::TaskLoaderAsyncTask
//======================================================================

RecentTasksLoader::TaskLoaderAsyncTask::TaskLoaderAsyncTask(
    /* [in] */ IBlockingQueue* queue,
    /* [in] */ RecentTasksLoader* host)
    : mTasksWaitingForThumbnails(queue)
    , mHost(host)
{}

ECode RecentTasksLoader::TaskLoaderAsyncTask::OnProgressUpdate(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (!IsCancelled()) {
        AutoPtr<IArrayList> newTasks = IArrayList::Probe((*values)[0]);
        // do a callback to RecentsPanelView to let it know we have more values
        // how do we let it know we're all done? just always call back twice
        AutoPtr<IRecentsPanelView> recentsPanel;
        mHost->GetRecentsPanel((IRecentsPanelView**)&recentsPanel);
        if (recentsPanel != NULL) {
            recentsPanel->OnTasksLoaded(newTasks, mHost->mFirstScreenful);
        }
        if (mHost->mLoadedTasks == NULL) {
            CArrayList::New((IArrayList**)&(mHost->mLoadedTasks));
        }
        mHost->mLoadedTasks->AddAll(ICollection::Probe(newTasks));

        mHost->mFirstScreenful = FALSE;
    }
    return NOERROR;
}

ECode RecentTasksLoader::TaskLoaderAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    // We load in two stages: first, we update progress with just the first screenful
    // of items. Then, we update with the rest of the items
    Int32 origPri;
    Process::GetThreadPriority(Process::MyTid(), &origPri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);

    AutoPtr<IPackageManager> pm;
    mHost->mContext->GetPackageManager((IPackageManager**)&pm);

    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    IActivityManager* am = IActivityManager::Probe(service);

    AutoPtr<IList> recentTasks;
    am->GetRecentTasks(MAX_TASKS,
        IActivityManager::RECENT_IGNORE_UNAVAILABLE
        | IActivityManager::RECENT_INCLUDE_PROFILES, (IList**)&recentTasks);

    Int32 numTasks;
    recentTasks->GetSize(&numTasks);
    AutoPtr<IIntent> it;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&it);
    it->AddCategory(IIntent::CATEGORY_HOME);
    AutoPtr<IActivityInfo> homeInfo;
    it->ResolveActivityInfo(pm, 0, (IActivityInfo**)&homeInfo);

    Boolean firstScreenful = TRUE;

    AutoPtr<IArrayList> tasks;
    CArrayList::New((IArrayList**)&tasks);

    // skip the first task - assume it's either the home screen or the current activity.
    const Int32 first = 0;
    for (Int32 i = first, index = 0; i < numTasks && (index < MAX_TASKS); ++i) {
        if (IsCancelled()) {
            break;
        }
        AutoPtr<IInterface> obj;
        recentTasks->Get(i, (IInterface**)&obj);
        const AutoPtr<IActivityManagerRecentTaskInfo> recentInfo = IActivityManagerRecentTaskInfo::Probe(obj);

        AutoPtr<IIntent> baseIntent;
        recentInfo->GetBaseIntent((IIntent**)&baseIntent);
        AutoPtr<IIntent> intent;
        CIntent::New(baseIntent, (IIntent**)&intent);

        AutoPtr<IComponentName> origActivity;
        recentInfo->GetOrigActivity((IComponentName**)&origActivity);

        if (origActivity != NULL) {
            intent->SetComponent(origActivity);
        }

        // Don't load the current home activity.
        AutoPtr<IComponentName> component;
        intent->GetComponent((IComponentName**)&component);

        if (mHost->IsCurrentHomeActivity(component, homeInfo)) {
            continue;
        }

        // Don't load ourselves
        String pkn, pkn2;
        component->GetPackageName(&pkn);
        mHost->mContext->GetPackageName(&pkn2);
        if (pkn.Equals(pkn2)) {
            continue;
        }

        Int32 id, persistentId, userId;
        recentInfo->GetId(&id);
        recentInfo->GetPersistentId(&persistentId);
        recentInfo->GetUserId(&userId);
        AutoPtr<ICharSequence> description;
        recentInfo->GetDescription((ICharSequence**)&description);


        AutoPtr<ITaskDescription> item;
        mHost->CreateTaskDescription(id, persistentId, baseIntent,origActivity,
            description, userId, (ITaskDescription**)&item);

        if (item != NULL) {
            while (TRUE) {
                /*ECode ec = */mTasksWaitingForThumbnails->Put(IObject::Probe(item));
                // if (FAILED(ec)) return E_INTERRUPTED_EXCEPTION;
                break;
            }
            tasks->Add(item);
            Int32 sizee;
            tasks->GetSize(&sizee);
            if (firstScreenful && sizee == mHost->mNumTasksInFirstScreenful) {

                AutoPtr<ArrayOf<IInterface* > > aaTasks = ArrayOf<IInterface* >::Alloc(sizee);
                for (Int32 i = 0; i < sizee; i++) {
                    AutoPtr<IInterface> obj;
                    tasks->Get(i, (IInterface**)&obj);
                    aaTasks->Set(i, obj);
                }

                PublishProgress(aaTasks);

                tasks = NULL;
                CArrayList::New((IArrayList**)&tasks);
                firstScreenful = FALSE;
                //break;
            }
            ++index;
        }
    }

    if (!IsCancelled()) {
        Int32 sizee;
        tasks->GetSize(&sizee);
        AutoPtr<ArrayOf<IInterface* > > aaTasks = ArrayOf<IInterface* >::Alloc(sizee);
        for (Int32 i = 0; i < sizee; i++) {
            AutoPtr<IInterface> obj;
            tasks->Get(i, (IInterface**)&obj);
            aaTasks->Set(i, obj);
        }
        PublishProgress(aaTasks);
        if (firstScreenful) {
            // always should publish two updates
            tasks = NULL;
            CArrayList::New((IArrayList**)&tasks);
            Int32 size;
            tasks->GetSize(&size);
            AutoPtr<ArrayOf<IInterface* > > aTasks = ArrayOf<IInterface* >::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                tasks->Get(i, (IInterface**)&obj);
                aTasks->Set(i, obj);
            }
            PublishProgress(aTasks);
        }
    }

    while (TRUE) {
        AutoPtr<TaskDescription> obj = new TaskDescription();
        /*ECode ec = */mTasksWaitingForThumbnails->Put((ITaskDescription*)obj);
        // if (FAILED(ec)) return E_INTERRUPTED_EXCEPTION;
        break;
    }

    Process::SetThreadPriority(origPri);
    *result = NULL;
    return NOERROR;
}

//======================================================================
// RecentTasksLoader::TaskLoaderAsyncTask
//======================================================================

ECode RecentTasksLoader::ThumbnailLoaderAsyncTask::OnProgressUpdate(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (!IsCancelled()) {
        AutoPtr<ITaskDescription> td = ITaskDescription::Probe((*values)[0]);
        Boolean isNull;
        td->IsNull(&isNull);
        if (isNull) { // end sentinel
            mHost->mState = State_LOADED;
        }
        else {
            AutoPtr<IRecentsPanelView> recentsPanel;
            mHost->GetRecentsPanel((IRecentsPanelView**)&recentsPanel);
            if (recentsPanel != NULL) {
                recentsPanel->OnTaskThumbnailLoaded(td);
            }
        }
    }
    return NOERROR;
}

ECode RecentTasksLoader::ThumbnailLoaderAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 origPri;
    Process::GetThreadPriority(Process::MyTid(), &origPri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);

    while (TRUE) {
        if (IsCancelled()) {
            break;
        }
        AutoPtr<ITaskDescription> td;
        while (td == NULL) {
            // try {
            AutoPtr<IInterface> obj;
            mTasksWaitingForThumbnails->Take((IInterface**)&obj);
            td = ITaskDescription::Probe(obj);
            // } catch (InterruptedException e) {
            // }
        }

        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, td);

        Boolean b;
        td->IsNull(&b);
        if (b) { // end sentinel
            PublishProgress(array);
            break;
        }
        mHost->LoadThumbnailAndIcon(td);

        PublishProgress(array);
    }

    Process::SetThreadPriority(origPri);
    *result = NULL;
    return NOERROR;
}

//======================================================================
// RecentTasksLoader
//======================================================================

const String RecentTasksLoader::TAG("RecentTasksLoader");
const Boolean RecentTasksLoader::DEBUG = IPhoneStatusBar::DEBUG || FALSE;
const Int32 RecentTasksLoader::DISPLAY_TASKS = 20;
const Int32 RecentTasksLoader::MAX_TASKS = DISPLAY_TASKS + 1; // allow extra for non-apps
AutoPtr<RecentTasksLoader> RecentTasksLoader::sInstance;

CAR_INTERFACE_IMPL_2(RecentTasksLoader, Object, IViewOnTouchListener, IRecentTasksLoader)

RecentTasksLoader::RecentTasksLoader(
    /* [in] */ IContext* context)
    : mContext(context)
    , mRecentsPanel(NULL)
    , mFirstTaskLoaded(FALSE)
    , mIconDpi(0)
    , mNumTasksInFirstScreenful(Elastos::Core::Math::INT32_MAX_VALUE)
    , mFirstScreenful(FALSE)
    , mState(State_CANCELLED)
    , mPreloadingFirstTask(FALSE)
    , mCancelPreloadingFirstTask(FALSE)
{
    mPreloadTasksRunnable = new PreloadTasksRunnable(this);

    CHandler::New((IHandler**)&mHandler);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    // get the icon size we want -- on tablets, we use bigger icons
    Boolean isTablet;
    res->GetBoolean(R::bool_::config_recents_interface_for_tablets, &isTablet);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    if (isTablet) {
        AutoPtr<IInterface> am;
        context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);
        IActivityManager::Probe(am)->GetLauncherLargeIconDensity(&mIconDpi);
    }
    else {
        dm->GetDensityDpi(&mIconDpi);
    }

    // Render default icon (just a blank image)
    Int32 defaultIconSize;
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::app_icon_size, &defaultIconSize);
    Int32 dpi;
    dm->GetDensityDpi(&dpi);
    Int32 iconSize = (Int32)(defaultIconSize * mIconDpi / dpi);
    mDefaultIconBackground = new ColorDrawableWithDimensions(0x00000000, iconSize, iconSize);

    // Render the default thumbnail background
    Int32 thumbnailWidth;
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::thumbnail_width, &thumbnailWidth);
    Int32 thumbnailHeight;
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::thumbnail_height, &thumbnailHeight);
    Int32 color;
    res->GetColor(R::drawable::status_bar_recents_app_thumbnail_background, &color);

    mDefaultThumbnailBackground =
            new ColorDrawableWithDimensions(color, thumbnailWidth, thumbnailHeight);
}

AutoPtr<RecentTasksLoader> RecentTasksLoader::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new RecentTasksLoader(context);
    }
    return sInstance;
}

ECode RecentTasksLoader::SetRecentsPanel(
    /* [in] */ IRecentsPanelView* newRecentsPanel,
    /* [in] */ IRecentsPanelView* caller)
{
    // Only allow clearing mRecentsPanel if the caller is the current recentsPanel
    AutoPtr<IRecentsPanelView> recentsPanel;
    GetRecentsPanel((IRecentsPanelView**)&recentsPanel);
    if (newRecentsPanel != NULL || caller == recentsPanel.Get()) {
        mRecentsPanel = NULL;
        if (newRecentsPanel != NULL) {
            AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(newRecentsPanel);
            wrs->GetWeakReference((IWeakReference**)&mRecentsPanel);

            newRecentsPanel->NumItemsInOneScreenful(&mNumTasksInFirstScreenful);
        }
    }
    return NOERROR;
}

ECode RecentTasksLoader::GetRecentsPanel(
    /* [out] */ IRecentsPanelView** rp)
{
    VALIDATE_NOT_NULL(rp)
    if (mRecentsPanel != NULL) {
        AutoPtr<IInterface> obj;
        mRecentsPanel->Resolve(EIID_IInterface, (IInterface**)&obj);
        *rp = IRecentsPanelView::Probe(obj);
        REFCOUNT_ADD(*rp)
    }
    return NOERROR;
}

ECode RecentTasksLoader::GetDefaultThumbnail(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mDefaultThumbnailBackground;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode RecentTasksLoader::GetDefaultIcon(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mDefaultIconBackground;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode RecentTasksLoader::GetLoadedTasks(
    /* [out, callee] */ IArrayList** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = mLoadedTasks;
    REFCOUNT_ADD(*tasks);
    return NOERROR;
}

ECode RecentTasksLoader::Remove(
    /* [in] */ ITaskDescription* td)
{
    mLoadedTasks->Remove(td);
    return NOERROR;
}

ECode RecentTasksLoader::IsFirstScreenful(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFirstScreenful;
    return NOERROR;
}

Boolean RecentTasksLoader::IsCurrentHomeActivity(
    /* [in] */ IComponentName* component,
    /* [in] */ IActivityInfo* _homeInfo)
{
    AutoPtr<IActivityInfo> homeInfo = _homeInfo;
    if (homeInfo == NULL) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        intent->AddCategory(IIntent::CATEGORY_HOME);
        intent->ResolveActivityInfo(pm, 0, (IActivityInfo**)&homeInfo);
    }
    if (homeInfo == NULL) return FALSE;

    String name, pkgName, comPkgName, className;
    IPackageItemInfo::Probe(homeInfo)->GetName(&name);
    IPackageItemInfo::Probe(homeInfo)->GetPackageName(&pkgName);
    component->GetPackageName(&comPkgName);
    component->GetClassName(&className);
    return pkgName.Equals(comPkgName) && name.Equals(className);
}

ECode RecentTasksLoader::CreateTaskDescription(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 persistentTaskId,
    /* [in] */ IIntent* baseIntent,
    /* [in] */ IComponentName* origActivity,
    /* [in] */ ICharSequence* description,
    /* [in] */ Int32 userId,
    /* [out] */ ITaskDescription** td)
{
    VALIDATE_NOT_NULL(td)
    AutoPtr<IIntent> intent;
    CIntent::New(baseIntent, (IIntent**)&intent);
    if (origActivity != NULL) {
        intent->SetComponent(origActivity);
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    const AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
    Int32 flags;
    intent->GetFlags(&flags);
    intent->SetFlags((flags & ~IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED)
            | IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IResolveInfo> resolveInfo;
    ECode ec = ipm->ResolveIntent(intent, String(NULL), 0, userId, (IResolveInfo**)&resolveInfo);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    if (resolveInfo != NULL) {
        AutoPtr<IActivityInfo> info;
        resolveInfo->GetActivityInfo((IActivityInfo**)&info);
        AutoPtr<ICharSequence> cs;
        IPackageItemInfo::Probe(info)->LoadLabel(pm, (ICharSequence**)&cs);
        String title;
        cs->ToString(&title);

        if (!title.IsNullOrEmpty()) {
            if (DEBUG) {
                Logger::V(TAG, "creating activity desc for id=%d, label=%s",
                    persistentTaskId, title.string());
            }

            String pkgName;
            IPackageItemInfo::Probe(info)->GetPackageName(&pkgName);
            AutoPtr<TaskDescription> item = new TaskDescription(taskId,
                persistentTaskId, resolveInfo, baseIntent, pkgName, description, userId);
            item->SetLabel(cs);
            *td = item;
            REFCOUNT_ADD(*td)
            return NOERROR;
        }
        else {
            if (DEBUG) Logger::V(TAG, "SKIPPING item %d", persistentTaskId);
        }
    }
    *td = NULL;
    return NOERROR;
}

ECode RecentTasksLoader::LoadThumbnailAndIcon(
    /* [in] */ ITaskDescription* td)
{
    AutoPtr<IInterface> amObj;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&amObj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(am);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<TaskDescription> _td = (TaskDescription*)td;
    Int32 persistentTaskId = _td->mPersistentTaskId;
    const AutoPtr<IBitmap> thumbnail = SystemServicesProxy::GetThumbnail(am, persistentTaskId);
    AutoPtr<IResolveInfo> resolveInfo = _td->mResolveInfo;
    AutoPtr<IDrawable> icon;
    icon = GetFullResIcon(resolveInfo, pm);
    if (_td->mUserId != UserHandle::GetMyUserId()) {
        // Need to badge the icon
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(_td->mUserId, (IUserHandle**)&uh);
        pm->GetUserBadgedIcon(icon, uh, (IDrawable**)&icon);
    }
    if (DEBUG) {
        Logger::V(TAG, "Loaded bitmap for task %s: %s", TO_CSTR(td), TO_CSTR(thumbnail));
    }
    synchronized(td) {
        if (thumbnail != NULL) {
            // td->SetThumbnail(thumbnail);
            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            AutoPtr<IDrawable> bd;
            CBitmapDrawable::New(res, thumbnail, (IDrawable**)&bd);
            _td->SetThumbnail(bd);
        }
        else {
            _td->SetThumbnail(mDefaultThumbnailBackground);
        }
        if (icon != NULL) {
            _td->SetIcon(icon);
        }
        _td->SetLoaded(TRUE);
        _td->Unlock();
    }
    return NOERROR;
}

ECode RecentTasksLoader::GetFullResDefaultActivityIcon(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    AutoPtr<IResources> res;
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    helper->GetSystem((IResources**)&res);
    return GetFullResIcon(res, Elastos::Droid::R::mipmap::sym_def_app_icon, drawable);
}

ECode RecentTasksLoader::GetFullResIcon(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 iconId,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    ECode ec = resources->GetDrawableForDensity(iconId, mIconDpi, drawable);
    if (FAILED(ec)) {
        GetFullResDefaultActivityIcon(drawable);
        return E_RESOURCES_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IDrawable> RecentTasksLoader::GetFullResIcon(
    /* [in] */ IResolveInfo* info,
    /* [in] */ IPackageManager* packageManager)
{
    AutoPtr<IResources> resources;
    // try {
    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    ECode ec = packageManager->GetResourcesForApplication(applicationInfo, (IResources**)&resources);
    if (FAILED(ec)) resources = NULL;
    // } catch (PackageManager.NameNotFoundException e) {
        // resources = null;
    // }
    AutoPtr<IDrawable> drawable;
    if (resources != NULL) {
        Int32 iconId;
        IComponentInfo::Probe(activityInfo)->GetIconResource(&iconId);
        if (iconId != 0) {
            GetFullResIcon(resources, iconId, (IDrawable**)&drawable);
            return drawable;
        }
    }
    GetFullResDefaultActivityIcon((IDrawable**)&drawable);
    return drawable;
}

ECode RecentTasksLoader::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    Int32 action;
    ev->GetAction(&action);
    action = action & IMotionEvent::ACTION_MASK;
    if (action == IMotionEvent::ACTION_DOWN) {
        PreloadRecentTasksList();
    }
    else if (action == IMotionEvent::ACTION_CANCEL) {
        CancelPreloadingRecentTasksList();
    }
    else if (action == IMotionEvent::ACTION_UP) {
        // Remove the preloader if we haven't called it yet
        mHandler->RemoveCallbacks(mPreloadTasksRunnable);
        Boolean b;
        v->IsPressed(&b);
        if (!b) {
            CancelLoadingThumbnailsAndIcons();
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode RecentTasksLoader::PreloadRecentTasksList()
{
    Boolean b;
    mHandler->Post(mPreloadTasksRunnable, &b);
    return NOERROR;
}

ECode RecentTasksLoader::CancelPreloadingRecentTasksList()
{
    CancelLoadingThumbnailsAndIcons();
    mHandler->RemoveCallbacks(mPreloadTasksRunnable);
    return NOERROR;
}

ECode RecentTasksLoader::CancelLoadingThumbnailsAndIcons(
    /* [in] */ IRecentsPanelView* caller)
{
    // Only oblige this request if it comes from the current RecentsPanel
    // (eg when you rotate, the old RecentsPanel request should be ignored)
    AutoPtr<IRecentsPanelView> recentsPanel;
    GetRecentsPanel((IRecentsPanelView**)&recentsPanel);
    if (caller == recentsPanel.Get()) {
        CancelLoadingThumbnailsAndIcons();
    }
    return NOERROR;
}

void RecentTasksLoader::CancelLoadingThumbnailsAndIcons()
{
    AutoPtr<IInterface> _rpv;
    mRecentsPanel->Resolve(EIID_IInterface, (IInterface**)&_rpv);
    AutoPtr<IRecentsPanelView> rpv = IRecentsPanelView::Probe(_rpv);
    Boolean isShowing;
    rpv->IsShowing(&isShowing);
    if (mRecentsPanel != NULL && isShowing) {
        return;
    }

    if (mTaskLoader != NULL) {
        mTaskLoader->Cancel(FALSE);
        mTaskLoader = NULL;
    }
    if (mThumbnailLoader != NULL) {
        mThumbnailLoader->Cancel(FALSE);
        mThumbnailLoader = NULL;
    }
    mLoadedTasks = NULL;

    AutoPtr<IRecentsPanelView> recentsPanel;
    GetRecentsPanel((IRecentsPanelView**)&recentsPanel);
    if (recentsPanel != NULL) {
        recentsPanel->OnTaskLoadingCancelled();
    }
    mFirstScreenful = FALSE;
    mState = State_CANCELLED;
}

void RecentTasksLoader::ClearFirstTask()
{
    synchronized(mFirstTaskLock) {
        mFirstTask = NULL;
        mFirstTaskLoaded = FALSE;
    }
}

ECode RecentTasksLoader::PreloadFirstTask()
{
    AutoPtr<BgLoadThread> bgLoad = new BgLoadThread(this);
    synchronized(mFirstTaskLock) {
        if (!mPreloadingFirstTask) {
            ClearFirstTask();
            mPreloadingFirstTask = TRUE;
            bgLoad->Start();
        }
    }
    return NOERROR;
}

ECode RecentTasksLoader::CancelPreloadingFirstTask()
{
    synchronized(mFirstTaskLock) {
        if (mPreloadingFirstTask) {
            mCancelPreloadingFirstTask = TRUE;
        }
        else {
            ClearFirstTask();
        }
    }
    return NOERROR;
}

ECode RecentTasksLoader::GetFirstTask(
    /* [out] */ ITaskDescription** des)
{
    VALIDATE_NOT_NULL(des);
    while (TRUE) {
        synchronized(mFirstTaskLock) {
            if (mFirstTaskLoaded) {
                *des = mFirstTask;
                REFCOUNT_ADD(*des);
                return NOERROR;
            }
            else if (!mFirstTaskLoaded && !mPreloadingFirstTask) {
                mFirstTask = NULL;
                LoadFirstTask((ITaskDescription**)&mFirstTask);
                mFirstTaskLoaded = TRUE;
                *des = mFirstTask;
                REFCOUNT_ADD(*des);
                return NOERROR;
            }
        }

        ECode ec = Thread::Sleep(3);
        if(FAILED(ec)) return E_INTERRUPTED_EXCEPTION;
    }
    return NOERROR;
}

ECode RecentTasksLoader::LoadFirstTask(
    /* [out] */ ITaskDescription** des)
{
    VALIDATE_NOT_NULL(des);
    *des = NULL;

    AutoPtr<IInterface> amObj;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&amObj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(am);

    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    Int32 identifier;
    uh->GetIdentifier(&identifier);
    AutoPtr<IList> recentTasks;
    am->GetRecentTasksForUser(1,
        IActivityManager::RECENT_IGNORE_UNAVAILABLE | IActivityManager::RECENT_INCLUDE_PROFILES,
        identifier, (IList**)&recentTasks);

    AutoPtr<ITaskDescription> item;
    Int32 size;
    recentTasks->GetSize(&size);
    if (size > 0) {
        AutoPtr<IInterface> obj;
        recentTasks->Get(0, (IInterface**)&obj);

        AutoPtr<IActivityManagerRecentTaskInfo> recentInfo = IActivityManagerRecentTaskInfo::Probe(obj);

        AutoPtr<IIntent> baseIntent;
        recentInfo->GetBaseIntent((IIntent**)&baseIntent);
        AutoPtr<IIntent> intent;
        CIntent::New(baseIntent, (IIntent**)&intent);
        AutoPtr<IComponentName> origActivity;
        recentInfo->GetOrigActivity((IComponentName**)&origActivity);
        if (origActivity != NULL) {
            intent->SetComponent(origActivity);
        }

        // Don't load the current home activity.
        AutoPtr<IComponentName> cn;
        intent->GetComponent((IComponentName**)&cn);
        if (IsCurrentHomeActivity(cn, NULL)) {
            *des = NULL;
            return NOERROR;
        }

        // Don't load ourselves
        String pn, pn2;
        cn->GetPackageName(&pn);
        mContext->GetPackageName(&pn2);
        if (pn.Equals(pn2)) {
            *des = NULL;
            return NOERROR;
        }

        Int32 id, persistentId, userId;
        recentInfo->GetId(&id);
        recentInfo->GetPersistentId(&persistentId);
        recentInfo->GetUserId(&userId);
        AutoPtr<ICharSequence> description;
        recentInfo->GetDescription((ICharSequence**)&description);
        CreateTaskDescription(id,
            persistentId, baseIntent,origActivity, description,userId,
            (ITaskDescription**)&item);
        if (item != NULL) {
            LoadThumbnailAndIcon(item);
        }
        *des = item;
        REFCOUNT_ADD(*des)
        return NOERROR;
    }
    *des = NULL;
    return NOERROR;
}

ECode RecentTasksLoader::LoadTasksInBackground()
{
    return LoadTasksInBackground(FALSE);
}

ECode RecentTasksLoader::LoadTasksInBackground(
    /* [in] */ Boolean zeroeth)
{
    if (mState != State_CANCELLED) {
        return NOERROR;
    }
    mState = State_LOADING;
    mFirstScreenful = TRUE;

    AutoPtr<IBlockingQueue> tasksWaitingForThumbnails;
    CLinkedBlockingQueue::New((IBlockingQueue**)&tasksWaitingForThumbnails);
    mTaskLoader = new TaskLoaderAsyncTask(tasksWaitingForThumbnails, this);
    mTaskLoader->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
    LoadThumbnailsAndIconsInBackground(tasksWaitingForThumbnails);
    return NOERROR;
}

void RecentTasksLoader::LoadThumbnailsAndIconsInBackground(
    /* [in] */ IBlockingQueue* tasksWaitingForThumbnails)
{
    // continually read items from tasksWaitingForThumbnails and load
    // thumbnails and icons for them. finish thread when cancelled or there
    // is a null item in tasksWaitingForThumbnails
    mThumbnailLoader = new ThumbnailLoaderAsyncTask(tasksWaitingForThumbnails, this);
    mThumbnailLoader->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos