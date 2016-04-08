#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/model/SpaceNode.h"
#include "../../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerTaskDescription;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::SystemUI::Recents::Constants;
using Elastos::Droid::SystemUI::Recents::Model::IComponentNameKey;
using Elastos::Droid::SystemUI::Recents::Model::ITask;
using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStack;
using Elastos::Droid::SystemUI::Recents::Model::SpaceNode;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::IRunnable;
using Elastos::Core::ISynchronize;
using Elastos::Core::IThread;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IQueue;
using Elastos::Utility::Concurrent::CConcurrentLinkedQueue;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

//=============================
// RecentsTaskLoader::TaskResourceLoadQueue
//=============================

RecentsTaskLoader::TaskResourceLoadQueue::TaskResourceLoadQueue()
{
    CConcurrentLinkedQueue::New((IConcurrentLinkedQueue**)&mQueue);
}

void RecentsTaskLoader::TaskResourceLoadQueue::AddTasks(
    /* [in] */ ICollection* tasks)
{
    AutoPtr<IIterator> it;
    IIterable::Probe(tasks)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ITask> t = ITask::Probe(obj);
        Boolean b;
        if (IQueue::Probe(mQueue)->Contains(t, &b), !b) {
            IQueue::Probe(mQueue)->Add(t);
        }
    }
    synchronized(this) {
        NotifyAll();
    }
}

void RecentsTaskLoader::TaskResourceLoadQueue::AddTask(
    /* [in] */ ITask* t)
{
    Boolean b;
    if (IQueue::Probe(mQueue)->Contains(t, &b), !b) {
        IQueue::Probe(mQueue)->Add(t);
    }
    synchronized(this) {
        NotifyAll();
    }
}

AutoPtr<ITask> RecentsTaskLoader::TaskResourceLoadQueue::NextTask()
{
    AutoPtr<IInterface> obj;
    IQueue::Probe(mQueue)->Poll((IInterface**)&obj);
    AutoPtr<ITask> t = ITask::Probe(obj);
    return t;
}

void RecentsTaskLoader::TaskResourceLoadQueue::RemoveTask(
    /* [in] */ ITask* t)
{
    Boolean b;
    IQueue::Probe(mQueue)->Remove(t, &b);
}

void RecentsTaskLoader::TaskResourceLoadQueue::ClearTasks()
{
    IQueue::Probe(mQueue)->Clear();
}

Boolean RecentsTaskLoader::TaskResourceLoadQueue::IsEmpty()
{
    Boolean isEmpty;
    IQueue::Probe(mQueue)->IsEmpty(&isEmpty);
    return isEmpty;
}

//=============================
// RecentsTaskLoader::TaskResourceLoader
//=============================

RecentsTaskLoader::TaskResourceLoader::TaskResourceLoader(
    /* [in] */ TaskResourceLoadQueue* loadQueue,
    /* [in] */ DrawableLruCache* applicationIconCache,
    /* [in] */ BitmapLruCache* thumbnailCache,
    /* [in] */ IBitmap* defaultThumbnail,
    /* [in] */ IBitmapDrawable* defaultApplicationIcon)
    : mLoadQueue(loadQueue)
    , mApplicationIconCache(applicationIconCache)
    , mThumbnailCache(thumbnailCache)
    , mDefaultThumbnail(defaultThumbnail)
    , mDefaultApplicationIcon(defaultApplicationIcon)
    , mCancelled(FALSE)
    , mWaitingOnLoadQueue(FALSE)
{
    CHandler::New((IHandler**)&mMainThreadHandler);
    CHandlerThread::New(String("Recents-TaskResourceLoader"),
        IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&mLoadThread);
    IThread::Probe(mLoadThread)->Start();
    AutoPtr<ILooper> looper;
    mLoadThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mLoadThreadHandler);
    Boolean b;
    mLoadThreadHandler->Post((IRunnable*)this, &b);
}

void RecentsTaskLoader::TaskResourceLoader::Start(
    /* [in] */ IContext* context)
{
    mContext = context;
    mCancelled = FALSE;
    mSystemServicesProxy = new SystemServicesProxy(context);
    // Notify the load thread to start loading
    synchronized(mLoadThread) {
        ISynchronize::Probe(mLoadThread)->NotifyAll();
    }
}

void RecentsTaskLoader::TaskResourceLoader::Stop()
{
    // Mark as cancelled for the thread to pick up
    mCancelled = TRUE;
    mSystemServicesProxy = NULL;
    // If we are waiting for the load queue for more tasks, then we can just reset the
    // Context now, since nothing is using it
    if (mWaitingOnLoadQueue) {
        mContext = NULL;
    }
}

ECode RecentsTaskLoader::TaskResourceLoader::Run()
{
    while (TRUE) {
        if (mCancelled) {
            // We have to unset the context here, since the background thread may be using it
            // when we call stop()
            mContext = NULL;
            // If we are cancelled, then wait until we are started again
            synchronized(mLoadThread) {
                ECode ec = ISynchronize::Probe(mLoadThread)->Wait();
                if (FAILED(ec)) {
                    return E_INTERRUPTED_EXCEPTION;
                }
            }
        }
        else {
            AutoPtr<SystemServicesProxy> ssp = mSystemServicesProxy;

            // Load the next item from the queue
            const AutoPtr<ITask> t = mLoadQueue->NextTask();
            if (t != NULL) {
                AutoPtr<Task> task = (Task*)t.Get();
                AutoPtr<Task::TaskKey> taskKey = task->mKey;
                AutoPtr<IDrawable> cachedIcon = mApplicationIconCache->Get((ITaskKey*)taskKey);
                AutoPtr<IBitmap> cachedThumbnail = mThumbnailCache->Get((ITaskKey*)taskKey);

                // Load the application icon if it is stale or we haven't cached one yet
                if (cachedIcon == NULL) {
                    AutoPtr<IResources> resources;
                    mContext->GetResources((IResources**)&resources);
                    cachedIcon = GetTaskDescriptionIcon(task->mKey, task->mIcon, task->mIconFilename, ssp,
                        resources);

                    if (cachedIcon == NULL) {
                        AutoPtr<IIntent> baseIntent = taskKey->mBaseIntent;
                        AutoPtr<IComponentName> cn;
                        baseIntent->GetComponent((IComponentName**)&cn);
                        AutoPtr<IActivityInfo> info = ssp->GetActivityInfo(cn, taskKey->mUserId);
                        if (info != NULL) {
                            cachedIcon = ssp->GetActivityIcon(info, taskKey->mUserId);
                        }
                    }

                    if (cachedIcon == NULL) {
                        cachedIcon = IDrawable::Probe(mDefaultApplicationIcon);
                    }

                    // At this point, even if we can't load the icon, we will set the default
                    // icon.
                    mApplicationIconCache->Put(taskKey, cachedIcon);
                }
                // Load the thumbnail if it is stale or we haven't cached one yet
                if (cachedThumbnail == NULL) {
                    cachedThumbnail = ssp->GetTaskThumbnail(taskKey->mId);
                    if (cachedThumbnail != NULL) {
                        cachedThumbnail->SetHasAlpha(FALSE);
                    }
                    else {
                        cachedThumbnail = mDefaultThumbnail;
                    }
                    mThumbnailCache->Put(taskKey, cachedThumbnail);
                }
                if (!mCancelled) {
                    // Notify that the task data has changed
                    const AutoPtr<IDrawable> newIcon = cachedIcon;
                    const AutoPtr<IBitmap> newThumbnail = cachedThumbnail == mDefaultThumbnail
                        ? NULL : cachedThumbnail;
                    AutoPtr<MyRunnable> r = new MyRunnable(t, newIcon, newThumbnail);
                    Boolean b;
                    mMainThreadHandler->Post((IRunnable*)r, &b);
                }
            }

            // If there are no other items in the list, then just wait until something is added
            if (!mCancelled && mLoadQueue->IsEmpty()) {
                synchronized(mLoadQueue) {
                    mWaitingOnLoadQueue = TRUE;
                    ECode ec = mLoadQueue->Wait();
                    if (FAILED(ec)) {
                        return E_INTERRUPTED_EXCEPTION;
                    }
                    mWaitingOnLoadQueue = FALSE;
                }
            }
        }
    }
    return NOERROR;
}

AutoPtr<IDrawable> RecentsTaskLoader::TaskResourceLoader::GetTaskDescriptionIcon(
    /* [in] */ ITaskKey* taskKey,
    /* [in] */ IBitmap* iconBitmap,
    /* [in] */ const String& iconFilename,
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ IResources* res)
{
#if 0 // CActivityManagerTaskDescriptionHelper needed
    AutoPtr<IActivityManagerTaskDescriptionHelper> amth;
    CActivityManagerTaskDescriptionHelper::AcquireSingleton(
        (IActivityManagerTaskDescriptionHelper**)&amth);
    AutoPtr<IBitmap> bm;
    amth->LoadTaskDescriptionIcon(iconFilename, (IBitmap**)&bm);
    AutoPtr<IBitmap> tdIcon = iconBitmap != NULL
            ? iconBitmap
            : bm;
    if (tdIcon != NULL) {
        AutoPtr<IDrawable> drawable;
        CBitmapDrawable::New(res, tdIcon, (IDrawable**)&drawable);
        AutoPtr<Task::TaskKey> _taskKey = (Task::TaskKey*)taskKey.Get();
        return ssp->GetBadgedIcon(drawable, _taskKey->mUserId);
    }
#endif
    return NULL;
}

//=============================
// RecentsTaskLoader::MyRunnable
//=============================

RecentsTaskLoader::MyRunnable::MyRunnable(
    /* [in] */ ITask* t,
    /* [in] */ IDrawable* d,
    /* [in] */ IBitmap* b)
    : mTask(t)
    , mNewIcon(d)
    , mNewThumbnail(b)
{}

ECode RecentsTaskLoader::MyRunnable::Run()
{
    return mTask->NotifyTaskDataLoaded(mNewThumbnail, mNewIcon);
}

//=============================
// RecentsTaskLoader
//=============================

const String RecentsTaskLoader::TAG("RecentsTaskLoader");
AutoPtr<RecentsTaskLoader> RecentsTaskLoader::sInstance;

RecentsTaskLoader::RecentsTaskLoader(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_recents_max_thumbnail_count, &mMaxThumbnailCacheSize);
    resources->GetInteger(R::integer::config_recents_max_icon_count, &mMaxIconCacheSize);

    Int32 iconCacheSize = Constants::DebugFlags::App::DisableBackgroundCache ? 1 :
        mMaxIconCacheSize;
    Int32 thumbnailCacheSize = Constants::DebugFlags::App::DisableBackgroundCache ? 1 :
        mMaxThumbnailCacheSize;

    // Create the default assets
    AutoPtr<IBitmapHelper> bh;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
    AutoPtr<IBitmap> icon;
    bh->CreateBitmap(1, 1, BitmapConfig_ARGB_8888, (IBitmap**)&icon);
    icon->EraseColor(0x00000000);
    bh->CreateBitmap(1, 1, BitmapConfig_ARGB_8888, (IBitmap**)&mDefaultThumbnail);
    mDefaultThumbnail->SetHasAlpha(FALSE);
    mDefaultThumbnail->EraseColor(0xFFffffff);
    CBitmapDrawable::New(resources, icon, (IBitmapDrawable**)&mDefaultApplicationIcon);

    // Initialize the proxy, cache and loaders
    mSystemServicesProxy = new SystemServicesProxy(context);
    mPackageMonitor = new RecentsPackageMonitor();
    mLoadQueue = new TaskResourceLoadQueue();
    mApplicationIconCache = new DrawableLruCache(iconCacheSize);
    mThumbnailCache = new BitmapLruCache(thumbnailCacheSize);
    mActivityLabelCache = new StringLruCache(100);
    mLoader = new TaskResourceLoader(mLoadQueue, mApplicationIconCache, mThumbnailCache,
        mDefaultThumbnail, mDefaultApplicationIcon);
}

AutoPtr<RecentsTaskLoader> RecentsTaskLoader::Initialize(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new RecentsTaskLoader(context);
    }
    return sInstance;
}

AutoPtr<RecentsTaskLoader> RecentsTaskLoader::GetInstance()
{
    return sInstance;
}

ECode RecentsTaskLoader::GetSystemServicesProxy(
    /* [out] */ SystemServicesProxy** ssp)
{
    VALIDATE_NOT_NULL(ssp)
    *ssp = mSystemServicesProxy;
    REFCOUNT_ADD(*ssp)
    return NOERROR;
}

AutoPtr<IList> RecentsTaskLoader::GetRecentTasks(
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ Boolean isTopTaskHome)
{
    AutoPtr<RecentsConfiguration> config = RecentsConfiguration::GetInstance();
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetCURRENT((IUserHandle**)&uh);
    Int32 identifier;
    uh->GetIdentifier(&identifier);
    AutoPtr<IList> tasks = ssp->GetRecentTasks(config->mMaxNumTasksToLoad, identifier, isTopTaskHome);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Reverse(tasks);
    return tasks;
}

AutoPtr<IDrawable> RecentsTaskLoader::GetAndUpdateActivityIcon(
    /* [in] */ ITaskKey* taskKey,
    /* [in] */ IActivityManagerTaskDescription* td,
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ IResources* res,
    /* [in] */ ActivityInfoHandle* infoHandle,
    /* [in] */ Boolean preloadTask)
{
    // Return the cached activity icon if it exists
    AutoPtr<IDrawable> icon = mApplicationIconCache->GetAndInvalidateIfModified(taskKey);
    if (icon != NULL) {
        return icon;
    }

    // If we are preloading this task, continue to load the task description icon or the
    // activity icon
    if (preloadTask) {

        // Return and cache the task description icon if it exists
        AutoPtr<IBitmap> bm;
        td->GetInMemoryIcon((IBitmap**)&bm);
        String ifn;
        td->GetIconFilename(&ifn);
        AutoPtr<IDrawable> tdDrawable = mLoader->GetTaskDescriptionIcon(taskKey, bm,
            ifn, ssp, res);
        if (tdDrawable != NULL) {
            mApplicationIconCache->Put(taskKey, tdDrawable);
            return tdDrawable;
        }

        // Load the icon from the activity info and cache it
        AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)taskKey;
        if (infoHandle->mInfo == NULL) {
            AutoPtr<IIntent> baseIntent = tk->mBaseIntent;
            AutoPtr<IComponentName> cn;
            baseIntent->GetComponent((IComponentName**)&cn);
            infoHandle->mInfo = ssp->GetActivityInfo(cn, tk->mUserId);
        }
        if (infoHandle->mInfo != NULL) {
            icon = ssp->GetActivityIcon(infoHandle->mInfo, tk->mUserId);
            if (icon != NULL) {
                mApplicationIconCache->Put(taskKey, icon);
                return icon;
            }
        }
    }
    // If we couldn't load any icon, return null
    return NULL;
}

String RecentsTaskLoader::GetAndUpdateActivityLabel(
    /* [in] */ ITaskKey* taskKey,
    /* [in] */ IActivityManagerTaskDescription* td,
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ ActivityInfoHandle* infoHandle)
{
    // Return the task description label if it exists
    String label1;
    td->GetLabel(&label1);
    if (td != NULL && !label1.IsNull()) {
        return label1;
    }
    // Return the cached activity label if it exists
    String label = mActivityLabelCache->GetAndInvalidateIfModified(taskKey);
    if (!label.IsNull()) {
        return label;
    }
    // All short paths failed, load the label from the activity info and cache it
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)taskKey;
    AutoPtr<IIntent> baseIntent = tk->mBaseIntent;
    AutoPtr<IComponentName> cn;
    baseIntent->GetComponent((IComponentName**)&cn);
    if (infoHandle->mInfo == NULL) {
        infoHandle->mInfo = ssp->GetActivityInfo(cn, tk->mUserId);
    }
    if (infoHandle->mInfo != NULL) {
        label = ssp->GetActivityLabel(infoHandle->mInfo);
        mActivityLabelCache->Put(taskKey, label);
    }
    else {
        Logger::W(TAG, "Missing ActivityInfo for %s u=%d",TO_CSTR(cn), tk->mUserId);
    }
    return label;
}

Int32 RecentsTaskLoader::GetActivityPrimaryColor(
    /* [in] */ IActivityManagerTaskDescription* td,
    /* [in] */ RecentsConfiguration* config)
{
    Int32 color;
    td->GetPrimaryColor(&color);
    if (td != NULL && color != 0) {
        return color;
    }
    return config->mTaskBarViewDefaultBackgroundColor;
}

AutoPtr<ISpaceNode> RecentsTaskLoader::Reload(
    /* [in] */ IContext* context,
    /* [in] */ Int32 preloadCount,
    /* [in] */ Boolean isTopTaskHome)
{
    AutoPtr<IArrayList> taskKeys;
    CArrayList::New((IArrayList**)&taskKeys);
    AutoPtr<IArrayList> tasksToLoad;
    CArrayList::New((IArrayList**)&tasksToLoad);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ITaskStack> stack = GetTaskStack(mSystemServicesProxy, res,
        -1, preloadCount, TRUE, isTopTaskHome, IList::Probe(taskKeys),
        IList::Probe(tasksToLoad));
    AutoPtr<SpaceNode> root = new SpaceNode();
    root->SetStack(stack);

    // Start the task loader and add all the tasks we need to load
    mLoader->Start(context);
    mLoadQueue->AddTasks(ICollection::Probe(tasksToLoad));

    // Update the package monitor with the list of packages to listen for
    mPackageMonitor->SetTasks(IList::Probe(taskKeys));

    return root;
}

AutoPtr<ITaskStack> RecentsTaskLoader::GetTaskStack(
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ IResources* res,
    /* [in] */ Int32 preloadTaskId,
    /* [in] */ Int32 preloadTaskCount,
    /* [in] */ Boolean loadTaskThumbnails,
    /* [in] */ Boolean isTopTaskHome,
    /* [in] */ IList* taskKeysOut,
    /* [in] */ IList* tasksToLoadOut)
{
    AutoPtr<RecentsConfiguration> config = RecentsConfiguration::GetInstance();
    AutoPtr<IList> tasks = GetRecentTasks(ssp, isTopTaskHome);
    AutoPtr<IHashMap> activityInfoCache;
    CHashMap::New((IHashMap**)&activityInfoCache);
    AutoPtr<IArrayList> tasksToAdd;
    CArrayList::New((IArrayList**)&tasksToAdd);
    AutoPtr<TaskStack> stack = new TaskStack();

    Int32 taskCount;
    tasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> _t;
        tasks->Get(i, (IInterface**)&_t);
        AutoPtr<IActivityManagerRecentTaskInfo> t = IActivityManagerRecentTaskInfo::Probe(_t);

        // Compose the task key
        Int32 persistentId;
        t->GetPersistentId(&persistentId);
        AutoPtr<IIntent> baseIntent;
        t->GetBaseIntent((IIntent**)&baseIntent);
        Int32 userId;
        t->GetUserId(&userId);
        Int64 firstActiveTime;
        t->GetFirstActiveTime(&firstActiveTime);
        Int64 lastActiveTime;
        t->GetLastActiveTime(&lastActiveTime);
        AutoPtr<Task::TaskKey> taskKey = new Task::TaskKey(persistentId, baseIntent, userId,
            firstActiveTime, lastActiveTime);

        // Get an existing activity info handle if possible
        AutoPtr<Task::ComponentNameKey> cnKey = taskKey->GetComponentNameKey();
        AutoPtr<ActivityInfoHandle> infoHandle;
        Boolean hasCachedActivityInfo = FALSE;
        Boolean b1;
        activityInfoCache->ContainsKey((IComponentNameKey*)cnKey, &b1);
        if (b1) {
            AutoPtr<IInterface> _infoHandle;
            activityInfoCache->Get((IComponentNameKey*)cnKey, (IInterface**)&_infoHandle);
            infoHandle = (ActivityInfoHandle*)(IObject::Probe(_infoHandle));
            hasCachedActivityInfo = TRUE;
        }
        else {
            infoHandle = new ActivityInfoHandle();
        }

        // Determine whether to preload this task
        Int32 id;
        t->GetId(&id);
        Boolean preloadTask = FALSE;
        if (preloadTaskId > 0) {
            preloadTask = (id == preloadTaskId);
        }
        else if (preloadTaskCount > 0) {
            preloadTask = (i >= (taskCount - preloadTaskCount));
        }

        // Load the label, icon, and color
        AutoPtr<IActivityManagerTaskDescription> atd;
        t->GetTaskDescription((IActivityManagerTaskDescription**)&atd);
        String activityLabel  = GetAndUpdateActivityLabel(taskKey, atd,
            ssp, infoHandle);
        AutoPtr<IDrawable> activityIcon = GetAndUpdateActivityIcon(taskKey, atd,
            ssp, res, infoHandle, preloadTask);
        Int32 activityColor = GetActivityPrimaryColor(atd, config);

        // Update the activity info cache
        if (!hasCachedActivityInfo && infoHandle->mInfo != NULL) {
            activityInfoCache->Put((IComponentNameKey*)cnKey, (IObject*)infoHandle);
        }

        AutoPtr<IBitmap> bm;
        atd->GetInMemoryIcon((IBitmap**)&bm);
        AutoPtr<IBitmap> icon = atd != NULL ? bm : NULL;
        String ifn;
        atd->GetIconFilename(&ifn);
        String iconFilename = atd != NULL ? ifn : String(NULL);

        // Add the task to the stack
        Int32 affiliatedTaskId, affiliatedTaskColor;
        t->GetAffiliatedTaskId(&affiliatedTaskId);
        t->GetAffiliatedTaskColor(&affiliatedTaskColor);
        AutoPtr<Task> task = new Task(taskKey, (id > -1), affiliatedTaskId, affiliatedTaskColor,
            activityLabel, activityIcon, activityColor, (i == (taskCount - 1)),
            config->mLockToAppEnabled, icon, iconFilename);

        if (preloadTask && loadTaskThumbnails) {
            // Load the thumbnail from the cache if possible
            task->mThumbnail = mThumbnailCache->GetAndInvalidateIfModified(taskKey);
            if (task->mThumbnail == NULL) {
                // Load the thumbnail from the system
                task->mThumbnail = ssp->GetTaskThumbnail(taskKey->mId);
                if (task->mThumbnail != NULL) {
                    task->mThumbnail->SetHasAlpha(FALSE);
                    mThumbnailCache->Put(taskKey, task->mThumbnail);
                }
            }
            if (task->mThumbnail == NULL && tasksToLoadOut != NULL) {
                // Either the task has changed since the last active time, or it was not
                // previously cached, so try and load the task anew.
                tasksToLoadOut->Add((ITask*)task);
            }
        }

        // Add to the list of task keys
        if (taskKeysOut != NULL) {
            taskKeysOut->Add((ITaskKey*)taskKey);
        }
        // Add the task to the stack
        tasksToAdd->Add((ITask*)task);
    }
    stack->SetTasks(IList::Probe(tasksToAdd));
    stack->CreateAffiliatedGroupings(config);
    return stack;
}

void RecentsTaskLoader::LoadTaskData(
    /* [in] */ ITask* t)
{
    AutoPtr<Task> _t = (Task*)t;
    AutoPtr<IDrawable> applicationIcon = mApplicationIconCache->GetAndInvalidateIfModified(_t->mKey);
    AutoPtr<IBitmap> thumbnail = mThumbnailCache->GetAndInvalidateIfModified(_t->mKey);

    // Grab the thumbnail/icon from the cache, if either don't exist, then trigger a reload and
    // use the default assets in their place until they load
    Boolean requiresLoad = (applicationIcon == NULL) || (thumbnail == NULL);
    applicationIcon = applicationIcon != NULL ? applicationIcon.Get() : IDrawable::Probe(mDefaultApplicationIcon);
    if (requiresLoad) {
        mLoadQueue->AddTask(t);
    }
    t->NotifyTaskDataLoaded(thumbnail == mDefaultThumbnail ? NULL : thumbnail, applicationIcon);
}

void RecentsTaskLoader::UnloadTaskData(
    /* [in] */ ITask* t)
{
    mLoadQueue->RemoveTask(t);
    t->NotifyTaskDataUnloaded(NULL, IDrawable::Probe(mDefaultApplicationIcon));
}

void RecentsTaskLoader::DeleteTaskData(
    /* [in] */ ITask* t,
    /* [in] */ Boolean notifyTaskDataUnloaded)
{
    mLoadQueue->RemoveTask(t);
    AutoPtr<Task> _t = (Task*)t;
    mThumbnailCache->Remove(_t->mKey);
    mApplicationIconCache->Remove(_t->mKey);
    if (notifyTaskDataUnloaded) {
        t->NotifyTaskDataUnloaded(NULL, IDrawable::Probe(mDefaultApplicationIcon));
    }
}

void RecentsTaskLoader::StopLoader()
{
    mLoader->Stop();
    mLoadQueue->ClearTasks();
}

void RecentsTaskLoader::RegisterReceivers(
    /* [in] */ IContext* context,
    /* [in] */ IPackageCallbacks* cb)
{
    // Register the broadcast receiver to handle messages related to packages being added/removed
    mPackageMonitor->Register(context, cb);
}

void RecentsTaskLoader::UnregisterReceivers()
{
    mPackageMonitor->Unregister();
}

void RecentsTaskLoader::OnTrimMemory(
    /* [in] */ Int32 level)
{
    switch (level) {
        case IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN: {
            // Stop the loader immediately when the UI is no longer visible
            StopLoader();
            mThumbnailCache->TrimToSize(Elastos::Core::Math::Max(
                Constants::Values::RecentsTaskLoader::PreloadFirstTasksCount,
                mMaxThumbnailCacheSize / 2));
            mApplicationIconCache->TrimToSize(Elastos::Core::Math::Max(
                Constants::Values::RecentsTaskLoader::PreloadFirstTasksCount,
                mMaxIconCacheSize / 2));
            break;
        }
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_MODERATE:
        case IComponentCallbacks2::TRIM_MEMORY_BACKGROUND: {
            // We are leaving recents, so trim the data a bit
            mThumbnailCache->TrimToSize(mMaxThumbnailCacheSize / 2);
            mApplicationIconCache->TrimToSize(mMaxIconCacheSize / 2);
            break;
        }
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_LOW:
        case IComponentCallbacks2::TRIM_MEMORY_MODERATE: {
            // We are going to be low on memory
            mThumbnailCache->TrimToSize(mMaxThumbnailCacheSize / 4);
            mApplicationIconCache->TrimToSize(mMaxIconCacheSize / 4);
            break;
        }
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL:
        case IComponentCallbacks2::TRIM_MEMORY_COMPLETE: {
            // We are low on memory, so release everything
            mThumbnailCache->EvictAll();
            mApplicationIconCache->EvictAll();
            // The cache is small, only clear the label cache when we are critical
            mActivityLabelCache->EvictAll();
            break;
        }
        default:
            break;
    }
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos