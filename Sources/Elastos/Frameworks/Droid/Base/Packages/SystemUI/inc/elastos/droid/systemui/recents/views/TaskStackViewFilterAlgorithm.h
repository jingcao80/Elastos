//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/systemui/recents/views/ViewPool.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
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
            /* [in] */ IArrayList* taskTransforms, // item is TaskViewTrancsform
            /* [in] */ TaskStackViewFilterAlgorithm* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IHashMap> mChildViewTransforms;
        AutoPtr<ITaskView> mTv;
        AutoPtr<IArrayList> mChildrenToRemove;
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
    TaskStackView* mStackView; // TaskStackView has this's reference
    AutoPtr<ViewPool> mViewPool;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEWFILTERALGORITHM_H__
