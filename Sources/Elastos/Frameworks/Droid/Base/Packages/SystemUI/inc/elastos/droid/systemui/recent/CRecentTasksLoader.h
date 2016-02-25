#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADER_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentTasksLoader.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::SystemUI::Recent::IRecentsPanelView;
using Elastos::Droid::SystemUI::Recent::ITaskDescription;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentTasksLoader)
{
public:
    class PreloadTasksRunnable : public Runnable
    {
    public:
        PreloadTasksRunnable(
            /* [in] */ CRecentTasksLoader* host) : mHost(host)
        {}

        CARAPI Run();

    private:
        CRecentTasksLoader* mHost;
    };

private:
    class BgLoadThread : public ThreadBase
    {
    public:
        BgLoadThread(
            /* [in] */ CRecentTasksLoader* host);

        CARAPI Run();

    private:
        CRecentTasksLoader* mHost;
    };

    class TaskLoaderAsyncTask : public AsyncTask
    {
    public:
        TaskLoaderAsyncTask(
            /* [in] */ IBlockingQueue* queue,
            /* [in] */ CRecentTasksLoader* host)
            : mTasksWaitingForThumbnails(queue)
            , mHost(host)
        {}

    protected:
        CARAPI_(void) OnProgressUpdate(
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI_(AutoPtr<IInterface>) DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params);

    private:
        AutoPtr<IBlockingQueue> mTasksWaitingForThumbnails;
        CRecentTasksLoader* mHost;
    };

    class ThumbnailLoaderAsyncTask : public AsyncTask
    {
    public:
        ThumbnailLoaderAsyncTask(
            /* [in] */ IBlockingQueue* queue,
            /* [in] */ CRecentTasksLoader* host)
            : mTasksWaitingForThumbnails(queue)
            , mHost(host)
        {}

    protected:
        CARAPI_(void) OnProgressUpdate(
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI_(AutoPtr<IInterface>) DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params);

    private:
        AutoPtr<IBlockingQueue> mTasksWaitingForThumbnails;
        CRecentTasksLoader* mHost;
    };

public:
    CRecentTasksLoader();

    ~CRecentTasksLoader();

    CARAPI constructor(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IRecentTasksLoader>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI SetRecentsPanel(
        /* [in] */ IRecentsPanelView* newRecentsPanel,
        /* [in] */ IRecentsPanelView* caller);

    CARAPI_(AutoPtr<IRecentsPanelView>) GetRecentsPanel();

    CARAPI GetDefaultThumbnail(
        /* [out] */ IBitmap** bitmap);

    CARAPI GetDefaultIcon(
        /* [out] */ IBitmap** bitmap);

    CARAPI GetLoadedTasks(
        /* [out, callee] */ IObjectContainer** tasks);

    CARAPI Remove(
        /* [in] */ ITaskDescription* td);

    CARAPI IsFirstScreenful(
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr<ITaskDescription>) CreateTaskDescription(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 persistentTaskId,
        /* [in] */ IIntent* baseIntent,
        /* [in] */ IComponentName* origActivity,
        /* [in] */ ICharSequence* description);

    CARAPI_(void) LoadThumbnailAndIcon(
        /* [in] */ ITaskDescription* td);

    CARAPI_(AutoPtr<IDrawable>) GetFullResDefaultActivityIcon();

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 iconId);

    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI PreloadRecentTasksList();

    CARAPI CancelPreloadingRecentTasksList();

    CARAPI CancelLoadingThumbnailsAndIcons(
        /* [in] */ IRecentsPanelView* caller);

    CARAPI PreloadFirstTask();

    CARAPI CancelPreloadingFirstTask();

    CARAPI GetFirstTask(
        /* [out] */ ITaskDescription** des);

    CARAPI LoadFirstTask(
        /* [out] */ ITaskDescription** des);

    CARAPI LoadTasksInBackground();

    CARAPI LoadTasksInBackground(
        /* [in] */ Boolean zeroeth);

private:
    CARAPI_(Boolean) IsCurrentHomeActivity(
        /* [in] */ IComponentName* component,
        /* [in] */ IActivityInfo* homeInfo);

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ IResolveInfo* info,
        /* [in] */ IPackageManager* packageManager);

    CARAPI_(void) CancelLoadingThumbnailsAndIcons();

    CARAPI_(void) ClearFirstTask();

    CARAPI_(void) LoadThumbnailsAndIconsInBackground(
        /* [in] */ IBlockingQueue* tasksWaitingForThumbnails);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 DISPLAY_TASKS;
    static const Int32 MAX_TASKS;

    AutoPtr<IContext> mContext;
    // RecentsPanelView has it's reference
    AutoPtr<IWeakReference> mRecentsPanel; //IRecentsPanelView*

    Object mFirstTaskLock;
    AutoPtr<ITaskDescription> mFirstTask;
    Boolean mFirstTaskLoaded;

    AutoPtr<TaskLoaderAsyncTask> mTaskLoader;
    AutoPtr<ThumbnailLoaderAsyncTask> mThumbnailLoader;
    AutoPtr<IHandler> mHandler;

    Int32 mIconDpi;
    AutoPtr<IBitmap> mDefaultThumbnailBackground;
    AutoPtr<IBitmap> mDefaultIconBackground;
    Int32 mNumTasksInFirstScreenful;

    Boolean mFirstScreenful;
    AutoPtr<IObjectContainer> mLoadedTasks;

    typedef enum {
        State_LOADING,
        State_LOADED,
        State_CANCELLED
    } State;
    State mState;

    static AutoPtr<IRecentTasksLoader> sInstance;

    AutoPtr<IRunnable> mPreloadTasksRunnable;

    Boolean mPreloadingFirstTask;
    Boolean mCancelPreloadingFirstTask;
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADER_H__
