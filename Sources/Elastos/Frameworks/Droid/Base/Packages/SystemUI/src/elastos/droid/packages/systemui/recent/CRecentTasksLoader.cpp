#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/systemui/recent/CRecentTasksLoader.h"
#include "elastos/droid/systemui/recent/CTaskDescription.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/Thread.h>
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>


using Elastos::Core::CString;
using Elastos::Core::Thread;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Droid::R;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::Recent::CTaskDescription;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//======================================================================
// CRecentTasksLoader::PreloadTasksRunnable
//======================================================================
ECode CRecentTasksLoader::PreloadTasksRunnable::Run()
{
    return mHost->LoadTasksInBackground();
}


//======================================================================
// CRecentTasksLoader::BgLoadThread
//======================================================================
CRecentTasksLoader::BgLoadThread::BgLoadThread(
    /* [in] */ CRecentTasksLoader* host)
    : mHost(host)
{
    Thread::constructor(String("BgLoadThread"));
}

ECode CRecentTasksLoader::BgLoadThread::Run()
{
    AutoPtr<ITaskDescription> first;
    mHost->LoadFirstTask((ITaskDescription**)&first);
    {
        AutoLock lock(mHost->mFirstTaskLock);
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
// CRecentTasksLoader::TaskLoaderAsyncTask
//======================================================================
void CRecentTasksLoader::TaskLoaderAsyncTask::OnProgressUpdate(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (!IsCancelled()) {
        AutoPtr<IObjectContainer> newTasks = IObjectContainer::Probe((*values)[0]);
        // do a callback to RecentsPanelView to let it know we have more values
        // how do we let it know we're all done? just always call back twice
        AutoPtr<IRecentsPanelView> recentsPanel = mHost->GetRecentsPanel();
        if (recentsPanel != NULL) {
            recentsPanel->OnTasksLoaded(newTasks, mHost->mFirstScreenful);
        }
        if (mHost->mLoadedTasks == NULL) {
            CObjectContainer::New((IObjectContainer**)&mHost->mLoadedTasks);
        }
        AutoPtr<IObjectEnumerator> enumerator;
        newTasks->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> task;
            enumerator->Current((IInterface**)&task);
            mHost->mLoadedTasks->Add(task);
        }
        mHost->mFirstScreenful = FALSE;
    }
}

AutoPtr<IInterface> CRecentTasksLoader::TaskLoaderAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params)
{
    // We load in two stages: first, we update progress with just the first screenful
    // of items. Then, we update with the rest of the items
    Int32 origPri;
    Process::GetThreadPriority(Process::MyTid(), &origPri);
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    AutoPtr<IPackageManager> pm;
    mHost->mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IActivityManager> am;
    mHost->mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

    AutoPtr<IObjectContainer> objectcontainer;
    am->GetRecentTasks(MAX_TASKS, IActivityManager::RECENT_IGNORE_UNAVAILABLE, (IObjectContainer**)&objectcontainer);
    AutoPtr<IIntent> tmpIntent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&tmpIntent);
    tmpIntent->AddCategory(IIntent::CATEGORY_HOME);
    AutoPtr<IActivityInfo> homeInfo;
    tmpIntent->ResolveActivityInfo(pm, 0, (IActivityInfo**)&homeInfo);

    Boolean firstScreenful = TRUE;
    List<AutoPtr<ITaskDescription> > tasks;

    if (objectcontainer != NULL) {
        AutoPtr<IObjectEnumerator> enumerator;
        objectcontainer->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Int32 index = 0;
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext && (index < MAX_TASKS)) {
            if (IsCancelled()) {
                break;
            }
            AutoPtr<IActivityManagerRecentTaskInfo> recentInfo;
            enumerator->Current((IInterface**)&recentInfo);

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
            String pkgName, packageName;
            component->GetPackageName(&pkgName);
            mHost->mContext->GetPackageName(&packageName);
            if (pkgName.Equals(packageName)) {
                continue;
            }

            Int32 id, persistentId;
            recentInfo->GetId(&id);
            recentInfo->GetPersistentId(&persistentId);
            AutoPtr<ICharSequence> description;
            recentInfo->GetDescription((ICharSequence**)&description);
            AutoPtr<ITaskDescription> item = mHost->CreateTaskDescription(id,
                    persistentId, baseIntent, origActivity, description);

            if (item != NULL) {
                while (TRUE) {
                    // try {
                    ECode ec = mTasksWaitingForThumbnails->Put(item);
                    if (SUCCEEDED(ec)) break;
                    // } catch (InterruptedException e) {
                    // }
                }
                tasks.PushBack(item);
                if (firstScreenful && tasks.GetSize() == mHost->mNumTasksInFirstScreenful) {
                    AutoPtr<IObjectContainer> _tasks;
                    CObjectContainer::New((IObjectContainer**)&_tasks);
                    List<AutoPtr<ITaskDescription> >::Iterator it;
                    for (it = tasks.Begin(); it != tasks.End(); ++it) {
                        _tasks->Add((*it).Get());
                    }
                    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
                    params->Set(0, _tasks);
                    PublishProgress(params);
                    tasks.Clear();
                    firstScreenful = FALSE;
                    //break;
                }
                ++index;
            }
        }
    }

    if (!IsCancelled()) {
        AutoPtr<IObjectContainer> _tasks;
        CObjectContainer::New((IObjectContainer**)&_tasks);
        List<AutoPtr<ITaskDescription> >::Iterator it;
        for (it = tasks.Begin(); it != tasks.End(); ++it) {
            _tasks->Add((*it).Get());
        }
        AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
        params->Set(0, _tasks);
        PublishProgress(params);
        if (firstScreenful) {
            // always should publish two updates
            AutoPtr<IObjectContainer> _tasks;
            CObjectContainer::New((IObjectContainer**)&_tasks);
            AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
            params->Set(0, _tasks);
            PublishProgress(params);
        }
    }

    while (TRUE) {
        // try {
        AutoPtr<ITaskDescription> taskDes;
        CTaskDescription::New((ITaskDescription**)&taskDes);
        ECode ec = mTasksWaitingForThumbnails->Put(taskDes);
        if (SUCCEEDED(ec)) break;
        // } catch (InterruptedException e) {
        // }
    }

    Process::SetThreadPriority(origPri);
    return NULL;
}


//======================================================================
// CRecentTasksLoader::TaskLoaderAsyncTask
//======================================================================
void CRecentTasksLoader::ThumbnailLoaderAsyncTask::OnProgressUpdate(
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
            AutoPtr<IRecentsPanelView> recentsPanel = mHost->GetRecentsPanel();
            if (recentsPanel != NULL) {
                recentsPanel->OnTaskThumbnailLoaded(td);
            }
        }
    }
}

AutoPtr<IInterface> CRecentTasksLoader::ThumbnailLoaderAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params)
{
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
            mTasksWaitingForThumbnails->Take((IInterface**)&td);
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
    return NULL;
}


//======================================================================
// CRecentTasksLoader
//======================================================================
const String CRecentTasksLoader::TAG("CRecentTasksLoader");
const Boolean CRecentTasksLoader::DEBUG = FALSE; // TabletStatusBar.DEBUG || PhoneStatusBar.DEBUG || false;
const Int32 CRecentTasksLoader::DISPLAY_TASKS = 20;
const Int32 CRecentTasksLoader::MAX_TASKS = DISPLAY_TASKS + 1; // allow extra for non-apps
AutoPtr<IRecentTasksLoader> CRecentTasksLoader::sInstance;

CRecentTasksLoader::CRecentTasksLoader()
    : mRecentsPanel(NULL)
    , mFirstTaskLoaded(FALSE)
    , mIconDpi(0)
    , mNumTasksInFirstScreenful(Elastos::Core::Math::INT32_MAX_VALUE)
    , mFirstScreenful(FALSE)
    , mState(State_CANCELLED)
    , mPreloadingFirstTask(FALSE)
    , mCancelPreloadingFirstTask(FALSE)
{
    mPreloadTasksRunnable = new PreloadTasksRunnable(this);
}

CRecentTasksLoader::~CRecentTasksLoader()
{
}

ECode CRecentTasksLoader::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHandler::New((IHandler**)&mHandler);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    // get the icon size we want -- on tablets, we use bigger icons
    Boolean isTablet;
    res->GetBoolean(SystemUIR::bool_::config_recents_interface_for_tablets, &isTablet);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    if (isTablet) {
        AutoPtr<IActivityManager> activityManager;
        context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&activityManager);
        activityManager->GetLauncherLargeIconDensity(&mIconDpi);
    }
    else {
        dm->GetDensityDpi(&mIconDpi);
    }

    // Render default icon (just a blank image)
    Int32 defaultIconSize;
    res->GetDimensionPixelSize(R::dimen::app_icon_size, &defaultIconSize);
    Int32 dpi;
    dm->GetDensityDpi(&dpi);
    Int32 iconSize = (Int32)(defaultIconSize * mIconDpi / dpi);
    CBitmap::CreateBitmap(iconSize, iconSize, BitmapConfig_ARGB_8888, (IBitmap**)&mDefaultIconBackground);

    // Render the default thumbnail background
    Int32 thumbnailWidth;
    res->GetDimensionPixelSize(R::dimen::thumbnail_width, &thumbnailWidth);
    Int32 thumbnailHeight;
    res->GetDimensionPixelSize(R::dimen::thumbnail_height, &thumbnailHeight);
    Int32 color;
    res->GetColor(SystemUIR::drawable::status_bar_recents_app_thumbnail_background, &color);

    CBitmap::CreateBitmap(thumbnailWidth, thumbnailHeight, BitmapConfig_ARGB_8888, (IBitmap**)&mDefaultThumbnailBackground);
    AutoPtr<ICanvas> c;
    CCanvas::New(mDefaultThumbnailBackground, (ICanvas**)&c);
    c->DrawColor(color);
    return NOERROR;
}

AutoPtr<IRecentTasksLoader> CRecentTasksLoader::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        CRecentTasksLoader::New(context, (IRecentTasksLoader**)&sInstance);
    }
    return sInstance;
}

ECode CRecentTasksLoader::SetRecentsPanel(
    /* [in] */ IRecentsPanelView* newRecentsPanel,
    /* [in] */ IRecentsPanelView* caller)
{
    // Only allow clearing mRecentsPanel if the caller is the current recentsPanel
    AutoPtr<IRecentsPanelView> recentsPanel = GetRecentsPanel();
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

AutoPtr<IRecentsPanelView> CRecentTasksLoader::GetRecentsPanel()
{
    AutoPtr<IRecentsPanelView> recentsPanel;
    if (mRecentsPanel != NULL) {
        AutoPtr<IInterface> obj;
        mRecentsPanel->Resolve(EIID_IInterface, (IInterface**)&obj);
        recentsPanel = IRecentsPanelView::Probe(obj);
    }
    return recentsPanel;
}

ECode CRecentTasksLoader::GetDefaultThumbnail(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mDefaultThumbnailBackground;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CRecentTasksLoader::GetDefaultIcon(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mDefaultIconBackground;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CRecentTasksLoader::GetLoadedTasks(
    /* [out, callee] */ IObjectContainer** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    *tasks = mLoadedTasks;
    REFCOUNT_ADD(*tasks);
    return NOERROR;
}

ECode CRecentTasksLoader::Remove(
    /* [in] */ ITaskDescription* td)
{
    mLoadedTasks->Remove(td);
    return NOERROR;
}

ECode CRecentTasksLoader::IsFirstScreenful(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFirstScreenful;
    return NOERROR;
}

Boolean CRecentTasksLoader::IsCurrentHomeActivity(
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
    homeInfo->GetName(&name);
    homeInfo->GetPackageName(&pkgName);
    component->GetPackageName(&comPkgName);
    component->GetClassName(&className);
    return pkgName.Equals(comPkgName) && name.Equals(className);
}

AutoPtr<ITaskDescription> CRecentTasksLoader::CreateTaskDescription(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 persistentTaskId,
    /* [in] */ IIntent* baseIntent,
    /* [in] */ IComponentName* origActivity,
    /* [in] */ ICharSequence* description)
{
    AutoPtr<IIntent> intent;
    CIntent::New(baseIntent, (IIntent**)&intent);
    if (origActivity != NULL) {
        intent->SetComponent(origActivity);
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 flags;
    intent->GetFlags(&flags);
    intent->SetFlags((flags & ~IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED)
            | IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IResolveInfo> resolveInfo;
    pm->ResolveActivity(intent, 0, (IResolveInfo**)&resolveInfo);
    if (resolveInfo != NULL) {
        AutoPtr<IActivityInfo> info;
        resolveInfo->GetActivityInfo((IActivityInfo**)&info);
        AutoPtr<ICharSequence> cs;
        info->LoadLabel(pm, (ICharSequence**)&cs);
        String title;
        cs->ToString(&title);

        if (!title.IsNullOrEmpty()) {
            // if (DEBUG) Log.v(TAG, "creating activity desc for id="
            //         + persistentTaskId + ", label=" + title);

            String pkgName;
            info->GetPackageName(&pkgName);
            AutoPtr<ITaskDescription> item;
            CTaskDescription::New(taskId, persistentTaskId, resolveInfo,
                    baseIntent, pkgName, description, (ITaskDescription**)&item);
            item->SetLabel(cs);

            return item;
        }
        else {
            // if (DEBUG) Log.v(TAG, "SKIPPING item " + persistentTaskId);
        }
    }
    return NULL;
}

void CRecentTasksLoader::LoadThumbnailAndIcon(
    /* [in] */ ITaskDescription* td)
{
    AutoPtr<IActivityManager> am;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 persistentTaskId;
    td->GetPersistentTaskId(&persistentTaskId);
    AutoPtr<IBitmap> thumbnail;
    am->GetTaskTopThumbnail(persistentTaskId, (IBitmap**)&thumbnail);
    AutoPtr<IResolveInfo> resolveInfo;
    td->GetResolveInfo((IResolveInfo**)&resolveInfo);
    AutoPtr<IDrawable> icon;
    icon = GetFullResIcon(resolveInfo, pm);

    // if (DEBUG) Log.v(TAG, "Loaded bitmap for task "
    //         + td + ": " + thumbnail);
    {
        td->Lock();
        if (thumbnail != NULL) {
            td->SetThumbnail(thumbnail);
        }
        else {
            td->SetThumbnail(mDefaultThumbnailBackground);
        }
        if (icon != NULL) {
            td->SetIcon(icon);
        }
        td->SetLoaded(TRUE);
        td->Unlock();
    }
}

AutoPtr<IDrawable> CRecentTasksLoader::GetFullResDefaultActivityIcon()
{
    AutoPtr<IResources> res;
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    helper->GetSystem((IResources**)&res);
    return GetFullResIcon(res, R::mipmap::sym_def_app_icon);
}

AutoPtr<IDrawable> CRecentTasksLoader::GetFullResIcon(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 iconId)
{
    // try {
    AutoPtr<IDrawable> dr;
    ECode ec = resources->GetDrawableForDensity(iconId, mIconDpi, (IDrawable**)&dr);
    if (FAILED(ec)) {
        dr = GetFullResDefaultActivityIcon();
    }
    return dr;
    // } catch (Resources.NotFoundException e) {
    //     return GetFullResDefaultActivityIcon();
    // }
}

AutoPtr<IDrawable> CRecentTasksLoader::GetFullResIcon(
    /* [in] */ IResolveInfo* info,
    /* [in] */ IPackageManager* packageManager)
{
    AutoPtr<IResources> resources;
    // try {
    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    ECode ec = packageManager->GetResourcesForApplication(applicationInfo, (IResources**)&resources);
    if (FAILED(ec)) resources = NULL;
    // } catch (PackageManager.NameNotFoundException e) {
        // resources = null;
    // }
    if (resources != NULL) {
        Int32 iconId;
        activityInfo->GetIconResource(&iconId);
        if (iconId != 0) {
            return GetFullResIcon(resources, iconId);
        }
    }
    return GetFullResDefaultActivityIcon();
}

ECode CRecentTasksLoader::OnTouch(
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

ECode CRecentTasksLoader::PreloadRecentTasksList()
{
    Boolean b;
    mHandler->Post(mPreloadTasksRunnable, &b);
    return NOERROR;
}

ECode CRecentTasksLoader::CancelPreloadingRecentTasksList()
{
    CancelLoadingThumbnailsAndIcons();
    mHandler->RemoveCallbacks(mPreloadTasksRunnable);
    return NOERROR;
}

ECode CRecentTasksLoader::CancelLoadingThumbnailsAndIcons(
    /* [in] */ IRecentsPanelView* caller)
{
    // Only oblige this request if it comes from the current RecentsPanel
    // (eg when you rotate, the old RecentsPanel request should be ignored)
    AutoPtr<IRecentsPanelView> recentsPanel = GetRecentsPanel();
    if (caller == recentsPanel.Get()) {
        CancelLoadingThumbnailsAndIcons();
    }
    return NOERROR;
}

void CRecentTasksLoader::CancelLoadingThumbnailsAndIcons()
{
    if (mTaskLoader != NULL) {
        mTaskLoader->Cancel(FALSE);
        mTaskLoader = NULL;
    }
    if (mThumbnailLoader != NULL) {
        mThumbnailLoader->Cancel(FALSE);
        mThumbnailLoader = NULL;
    }
    mLoadedTasks = NULL;

    AutoPtr<IRecentsPanelView> recentsPanel = GetRecentsPanel();
    if (recentsPanel != NULL) {
        recentsPanel->OnTaskLoadingCancelled();
    }
    mFirstScreenful = FALSE;
    mState = State_CANCELLED;
}

void CRecentTasksLoader::ClearFirstTask()
{
    AutoLock lock(mFirstTaskLock);
    mFirstTask = NULL;
    mFirstTaskLoaded = FALSE;
}

ECode CRecentTasksLoader::PreloadFirstTask()
{
    AutoPtr<BgLoadThread> bgLoad = new BgLoadThread(this);
    AutoLock lock(mFirstTaskLock);
    if (!mPreloadingFirstTask) {
        ClearFirstTask();
        mPreloadingFirstTask = TRUE;
        bgLoad->Start();
    }
    return NOERROR;
}

ECode CRecentTasksLoader::CancelPreloadingFirstTask()
{
    AutoLock lock(mFirstTaskLock);
    if (mPreloadingFirstTask) {
        mCancelPreloadingFirstTask = TRUE;
    }
    else {
        ClearFirstTask();
    }
    return NOERROR;
}

ECode CRecentTasksLoader::GetFirstTask(
    /* [out] */ ITaskDescription** des)
{
    VALIDATE_NOT_NULL(des);
    while (TRUE) {
        AutoLock lock(mFirstTaskLock);
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
        // try {
        Thread::Sleep(3);
        // } catch (InterruptedException e) {
        // }
    }
}

ECode CRecentTasksLoader::LoadFirstTask(
    /* [out] */ ITaskDescription** des)
{
    VALIDATE_NOT_NULL(des);
    *des = NULL;

    AutoPtr<IActivityManager> am;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

    Int32 identifier;
    UserHandle::CURRENT->GetIdentifier(&identifier);
    AutoPtr<IObjectContainer> recentTasks;
    am->GetRecentTasksForUser(1, IActivityManager::RECENT_IGNORE_UNAVAILABLE, identifier, (IObjectContainer**)&recentTasks);
    if (recentTasks == NULL) {
        return NOERROR;
    }

    AutoPtr<ITaskDescription> item;
    Int32 size;
    recentTasks->GetObjectCount(&size);
    if (size > 0) {
        AutoPtr<IObjectEnumerator> enumerator;
        recentTasks->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        enumerator->MoveNext(&hasNext);
        AutoPtr<IActivityManagerRecentTaskInfo> recentInfo;
        enumerator->Current((IInterface**)&recentInfo);

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
        AutoPtr<IComponentName> name;
        intent->GetComponent((IComponentName**)&name);
        if (IsCurrentHomeActivity(name, NULL)) {
            return NOERROR;
        }

        // Don't load ourselves
        String pkgName, ctxPkgName;
        name->GetPackageName(&pkgName);
        mContext->GetPackageName(&ctxPkgName);
        if (pkgName.Equals(ctxPkgName)) {
            return NOERROR;
        }

        Int32 id, persistentId;
        recentInfo->GetId(&id);
        recentInfo->GetPersistentId(&persistentId);
        AutoPtr<ICharSequence> description;
        recentInfo->GetDescription((ICharSequence**)&description);
        item = CreateTaskDescription(id, persistentId, baseIntent, origActivity, description);
        if (item != NULL) {
            LoadThumbnailAndIcon(item);
        }
        *des = item;
        REFCOUNT_ADD(*des);
        return NOERROR;
    }
    return NOERROR;
}

ECode CRecentTasksLoader::LoadTasksInBackground()
{
    return LoadTasksInBackground(FALSE);
}

ECode CRecentTasksLoader::LoadTasksInBackground(
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

void CRecentTasksLoader::LoadThumbnailsAndIconsInBackground(
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
