
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSTASKLOADER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSTASKLOADER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/systemui/recents/model/BitmapLruCache.h"
#include "elastos/droid/systemui/recents/model/DrawableLruCache.h"
#include "elastos/droid/systemui/recents/model/RecentsPackageMonitor.h"
#include "elastos/droid/systemui/recents/model/StringLruCache.h"

using Elastos::Droid::App::IActivityManagerTaskDescription;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::Recents::Model::BitmapLruCache;
using Elastos::Droid::SystemUI::Recents::Model::DrawableLruCache;
using Elastos::Droid::SystemUI::Recents::Model::IPackageCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::ITask;
using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::RecentsPackageMonitor;
using Elastos::Droid::SystemUI::Recents::Model::StringLruCache;
using Elastos::Utility::ICollection;
using Elastos::Utility::Concurrent::IConcurrentLinkedQueue;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/* Recents task loader
 * NOTE: We should not hold any references to a Context from a static instance */
class RecentsTaskLoader
    : public Object
{
private:
    /** Handle to an ActivityInfo */
    class ActivityInfoHandle
        : public Object
    {
    public:
        AutoPtr<IActivityInfo> mInfo;
    };

    /** A bitmap load queue */
    class TaskResourceLoadQueue
        : public Object
    {
    public:
        AutoPtr<IConcurrentLinkedQueue> mQueue;

    public:
        TaskResourceLoadQueue();

        /** Adds a new task to the load queue */
        CARAPI_(void) AddTasks(
            /* [in] */ ICollection* tasks);

        /** Adds a new task to the load queue */
        CARAPI_(void) AddTask(
            /* [in] */ ITask* t);

        /**
         * Retrieves the next task from the load queue, as well as whether we want that task to be
         * force reloaded.
         */
        CARAPI_(AutoPtr<ITask>) NextTask();

        /** Removes a task from the load queue */
        CARAPI_(void) RemoveTask(
            /* [in] */ ITask* t);

        /** Clears all the tasks from the load queue */
        CARAPI_(void) ClearTasks();

        /** Returns whether the load queue is empty */
        CARAPI_(Boolean) IsEmpty();
    };

    /* Task resource loader */
    class TaskResourceLoader
        : public Runnable
    {
    public:
        /** Constructor, creates a new loading thread that loads task resources in the background */
        TaskResourceLoader(
            /* [in] */ TaskResourceLoadQueue* loadQueue,
            /* [in] */ DrawableLruCache* applicationIconCache,
            /* [in] */ BitmapLruCache* thumbnailCache,
            /* [in] */ IBitmap* defaultThumbnail,
            /* [in] */ IBitmapDrawable* defaultApplicationIcon);

        /** Restarts the loader thread */
        CARAPI_(void) Start(
            /* [in] */ IContext* context);

        /** Requests the loader thread to stop after the current iteration */
        CARAPI_(void) Stop();

        // @Override
        CARAPI Run();

        CARAPI_(AutoPtr<IDrawable>) GetTaskDescriptionIcon(
            /* [in] */ ITaskKey* taskKey,
            /* [in] */ IBitmap* iconBitmap,
            /* [in] */ const String& iconFilename,
            /* [in] */ SystemServicesProxy* ssp,
            /* [in] */ IResources* res);

    public:
        AutoPtr<IContext> mContext;
        AutoPtr<IHandlerThread> mLoadThread;
        AutoPtr<IHandler> mLoadThreadHandler;
        AutoPtr<IHandler> mMainThreadHandler;

        AutoPtr<SystemServicesProxy> mSystemServicesProxy;
        AutoPtr<TaskResourceLoadQueue> mLoadQueue;
        AutoPtr<DrawableLruCache> mApplicationIconCache;
        AutoPtr<BitmapLruCache> mThumbnailCache;
        AutoPtr<IBitmap> mDefaultThumbnail;
        AutoPtr<IBitmapDrawable> mDefaultApplicationIcon;

        Boolean mCancelled;
        Boolean mWaitingOnLoadQueue;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ITask* t,
            /* [in] */ IDrawable* d,
            /* [in] */ IBitmap* b);

        //@Override
        CARAPI Run();

    private:
        AutoPtr<ITask> mTask;
        AutoPtr<IDrawable> mNewIcon;
        AutoPtr<IBitmap> mNewThumbnail;
    };

private:
    /** Private Constructor */
    RecentsTaskLoader(
        /* [in] */ IContext* context);

public:
    /** Initializes the recents task loader */
    static CARAPI_(AutoPtr<RecentsTaskLoader>) Initialize(
        /* [in] */ IContext* context);

    /** Returns the current recents task loader */
    static  CARAPI_(AutoPtr<RecentsTaskLoader>) GetInstance();

    /** Returns the system services proxy */
    CARAPI GetSystemServicesProxy(
        /* [out] */ SystemServicesProxy** ssp);

private:
    /** Gets the list of recent tasks, ordered from back to front. */
    static CARAPI_(AutoPtr<IList>) GetRecentTasks(
        /* [in] */ SystemServicesProxy* ssp,
        /* [in] */ Boolean isTopTaskHome);

public:
    /** Returns the activity icon using as many cached values as we can. */
    CARAPI_(AutoPtr<IDrawable>) GetAndUpdateActivityIcon(
        /* [in] */ ITaskKey* taskKey,
        /* [in] */ IActivityManagerTaskDescription* td,
        /* [in] */ SystemServicesProxy* ssp,
        /* [in] */ IResources* res,
        /* [in] */ ActivityInfoHandle* infoHandle,
        /* [in] */ Boolean preloadTask);

    /** Returns the activity label using as many cached values as we can. */
    CARAPI_(String) GetAndUpdateActivityLabel(
        /* [in] */ ITaskKey* taskKey,
        /* [in] */ IActivityManagerTaskDescription* td,
        /* [in] */ SystemServicesProxy* ssp,
        /* [in] */ ActivityInfoHandle* infoHandle);

    /** Returns the activity's primary color. */
    CARAPI_(Int32) GetActivityPrimaryColor(
        /* [in] */ IActivityManagerTaskDescription* td,
        /* [in] */ RecentsConfiguration* config);

    /** Reload the set of recent tasks */
    CARAPI_(AutoPtr<ISpaceNode>) Reload(
        /* [in] */ IContext* context,
        /* [in] */ Int32 preloadCount,
        /* [in] */ Boolean isTopTaskHome);

    /** Creates a lightweight stack of the current recent tasks, without thumbnails and icons. */
    CARAPI_(AutoPtr<ITaskStack>) GetTaskStack(
        /* [in] */ SystemServicesProxy* ssp,
        /* [in] */ IResources* res,
        /* [in] */ Int32 preloadTaskId,
        /* [in] */ Int32 preloadTaskCount,
        /* [in] */ Boolean loadTaskThumbnails,
        /* [in] */ Boolean isTopTaskHome,
        /* [in] */ IList* taskKeysOut,
        /* [in] */ IList* tasksToLoadOut);

    /** Acquires the task resource data directly from the pool. */
    CARAPI_(void) LoadTaskData(
        /* [in] */ ITask* t);

    /** Releases the task resource data back into the pool. */
    CARAPI_(void) UnloadTaskData(
        /* [in] */ ITask* t);

    /** Completely removes the resource data from the pool. */
    CARAPI_(void) DeleteTaskData(
        /* [in] */ ITask* t,
        /* [in] */ Boolean notifyTaskDataUnloaded);

    /** Stops the task loader and clears all pending tasks */
    CARAPI_(void) StopLoader();

    /** Registers any broadcast receivers. */
    CARAPI_(void) RegisterReceivers(
        /* [in] */ IContext* context,
        /* [in] */ IPackageCallbacks* cb);

    /** Unregisters any broadcast receivers. */
    CARAPI_(void) UnregisterReceivers();

    /**
     * Handles signals from the system, trimming memory when requested to prevent us from running
     * out of memory.
     */
    CARAPI_(void) OnTrimMemory(
        /* [in] */ Int32 level);

private:
    static const String TAG;

public:
    static AutoPtr<RecentsTaskLoader> sInstance;

    AutoPtr<SystemServicesProxy> mSystemServicesProxy;
    AutoPtr<DrawableLruCache> mApplicationIconCache;
    AutoPtr<BitmapLruCache> mThumbnailCache;
    AutoPtr<StringLruCache> mActivityLabelCache;
    AutoPtr<TaskResourceLoadQueue> mLoadQueue;
    AutoPtr<TaskResourceLoader> mLoader;

    AutoPtr<RecentsPackageMonitor> mPackageMonitor;

    Int32 mMaxThumbnailCacheSize;
    Int32 mMaxIconCacheSize;

    AutoPtr<IBitmapDrawable> mDefaultApplicationIcon;
    AutoPtr<IBitmap> mDefaultThumbnail;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSTASKLOADER_H__
