
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/systemui/recents/views/ViewPool.h"

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

class TaskStackView;

/* The layout logic for a TaskStackView */
class TaskStackViewFilterAlgorithm : public Object
{
private:
    class EndActionRunnable : public Runnable
    {
    public:
        EndActionRunnable(
            /* [in] */ IHashMap* childViewTransforms,
            /* [in] */ ITaskView* tv,
            /* [in] */ IArrayList* childrenToRemove,
            /* [in] */ Boolean unifyNewViewAnimation,
            /* [in] */ IArrayList* tasks, // item is Task
            /* [in] */ IArrayList* taskTransforms) // item is TaskViewTrancsform
            /* [in] */ TaskStackViewFilterAlgorithm* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IHashMap> mChildViewTransforms;
        AutoPtr<ITaskView> mTv;
        AutoPtr<IArrayList> mChildrenToRemove
        Boolean mUnifyNewViewAnimation;
        AutoPtr<IArrayList> mTasks;
        AutoPtr<IArrayList> mTaskTransforms;
        TaskStackViewFilterAlgorithm* mHost;

    };

public:
    TaskStackViewFilterAlgorithm(
        /* [in] */ RecentsConfiguration* config,
        /* [in] */ TaskStackView* stackView,
        /* [in] */ ViewPool* viewPool);

    /** Orchestrates the animations of the current child views and any new views. */
    CARAPI_(void) StartFilteringAnimation(
        /* [in] */ IArrayList* curTasks, // item is Task
        /* [in] */ IArrayList* curTaskTransforms, // item is TaskViewTransform
        /* [in] */ IArrayList* tasks, // item is Task
        /* [in] */ IArrayList* taskTransforms); // item is TaskViewTrancsform

    /**
     * Creates the animations for all the children views that need to be animated in when we are
     * un/filtering a stack, and returns the duration for these animations.
     */
    CARAPI_(Int32) GetEnterTransformsForFilterAnimation(
        /* [in] */ IArrayList* tasks, // item is Task
        /* [in] */ IArrayList* taskTransforms, // item is TaskViewTransform
        /* [in] */ IHashMap* childViewTransformsOut); // <TaskView, TaskViewTransform>

    /**
     * Creates the animations for all the children views that need to be removed or to move views
     * to their un/filtered position when we are un/filtering a stack, and returns the duration
     * for these animations.
     */
    CARAPI_(Int32) GetExitTransformsForFilterAnimation(
        /* [in] */ IArrayList* curTasks, // item is Task
        /* [in] */ IArrayList* curTaskTransforms, // item is TaskViewTransform
        /* [in] */ IArrayList* tasks, // item is Task
        /* [in] */ IArrayList* taskTransforms, // item is TaskViewTransform
        /* [in] */ IHashMap*  childViewTransformsOut, // <TaskView, TaskViewTransform>
        /* [in] */ IArrayList* childrenToRemoveOut); // item is TaskView

public:
    AutoPtr<RecentsConfiguration> mConfig;
    AutoPtr<TaskStackView> mStackView;
    AutoPtr<ViewPool> mViewPool;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__
