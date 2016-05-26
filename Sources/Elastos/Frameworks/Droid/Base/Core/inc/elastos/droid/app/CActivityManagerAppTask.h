
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERAPPTASK_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERAPPTASK_H__

#include "_Elastos_Droid_App_CActivityManagerAppTask.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityManagerAppTask)
    , public Object
    , public IActivityManagerAppTask
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIAppTask* task);

    /**
     * Finishes all activities in this task and removes it from the recent tasks list.
     */
    CARAPI FinishAndRemoveTask();

    /**
     * Get the RecentTaskInfo associated with this task.
     *
     * @return The RecentTaskInfo for this task, or null if the task no longer exists.
     */
    CARAPI GetTaskInfo(
        /* [out] */ IActivityManagerRecentTaskInfo** info);

    /**
     * Bring this task to the foreground.  If it contains activities, they will be
     * brought to the foreground with it and their instances re-created if needed.
     * If it doesn't contain activities, the root activity of the task will be
     * re-launched.
     */
    CARAPI MoveToFront();

    /**
     * Start an activity in this task.  Brings the task to the foreground.  If this task
     * is not currently active (that is, its id < 0), then a new activity for the given
     * Intent will be launched as the root of the task and the task brought to the
     * foreground.  Otherwise, if this task is currently active and the Intent does not specify
     * an activity to launch in a new task, then a new activity for the given Intent will
     * be launched on top of the task and the task brought to the foreground.  If this
     * task is currently active and the Intent specifies {@link Intent#FLAG_ACTIVITY_NEW_TASK}
     * or would otherwise be launched in to a new task, then the activity not launched but
     * this task be brought to the foreground and a new intent delivered to the top
     * activity if appropriate.
     *
     * <p>In other words, you generally want to use an Intent here that does not specify
     * {@link Intent#FLAG_ACTIVITY_NEW_TASK} or {@link Intent#FLAG_ACTIVITY_NEW_DOCUMENT},
     * and let the system do the right thing.</p>
     *
     * @param intent The Intent describing the new activity to be launched on the task.
     * @param options Optional launch options.
     *
     * @see Activity#startActivity(android.content.Intent, android.os.Bundle)
     */
    CARAPI StartActivity(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    /**
     * Modify the {@link Intent#FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS} flag in the root
     * Intent of this AppTask.
     *
     * @param exclude If true, {@link Intent#FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS} will
     * be set; otherwise, it will be cleared.
     */
    CARAPI SetExcludeFromRecents(
        /* [in] */ Boolean exclude);

private:
    static const String TAG;
    AutoPtr<IIAppTask> mAppTaskImpl;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERAPPTASK_H__
