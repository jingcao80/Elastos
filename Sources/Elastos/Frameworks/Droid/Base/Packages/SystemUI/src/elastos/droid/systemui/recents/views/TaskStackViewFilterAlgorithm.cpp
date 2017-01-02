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

#include "elastos/droid/systemui/recents/views/TaskStackViewFilterAlgorithm.h"
#include "elastos/droid/systemui/recents/views/TaskStackView.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include <elastos/core/Math.h>

using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

TaskStackViewFilterAlgorithm::EndActionRunnable::EndActionRunnable(
    /* [in] */ IHashMap* childViewTransforms,
    /* [in] */ ITaskView* tv,
    /* [in] */ IArrayList* childrenToRemove,
    /* [in] */ Boolean unifyNewViewAnimation,
    /* [in] */ IArrayList* tasks, // item is Task
    /* [in] */ IArrayList* taskTransforms, // item is TaskViewTrancsform
    /* [in] */ TaskStackViewFilterAlgorithm* host)
    : mChildViewTransforms(childViewTransforms)
    , mTv(tv)
    , mChildrenToRemove(childrenToRemove)
    , mUnifyNewViewAnimation(unifyNewViewAnimation)
    , mTasks(tasks)
    , mTaskTransforms(taskTransforms)
    , mHost(host)
{}

// @Override
ECode TaskStackViewFilterAlgorithm::EndActionRunnable::Run()
{
    mChildViewTransforms->Remove(mTv);
    Boolean isEmpty;
    mChildViewTransforms->IsEmpty(&isEmpty);
    if (isEmpty) {
        // Return all the removed children to the view pool
        Int32 size;
        mChildrenToRemove->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> tv;
            mChildrenToRemove->Get(i, (IInterface**)&tv);
            mHost->mViewPool->ReturnViewToPool(tv);
        }

        if (!mUnifyNewViewAnimation) {
            // For views that are not already visible, animate them in
            mChildViewTransforms->Clear();
            Int32 duration = mHost->GetEnterTransformsForFilterAnimation(mTasks,
                    mTaskTransforms, mChildViewTransforms);
            AutoPtr<ISet> set;
            mChildViewTransforms->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> meo, ko, vo;
                it->GetNext((IInterface**)&meo);
                IMapEntry* me = IMapEntry::Probe(meo);
                me->GetKey((IInterface**)&ko);
                me->GetValue((IInterface**)&vo);
                TaskView* tv = (TaskView*)ITaskView::Probe(ko);
                TaskViewTransform* t = (TaskViewTransform*)IObject::Probe(vo);
                tv->UpdateViewPropertiesToTaskTransform(t, duration);
            }
        }
    }

    return NOERROR;
}

TaskStackViewFilterAlgorithm::TaskStackViewFilterAlgorithm(
    /* [in] */ RecentsConfiguration* config,
    /* [in] */ TaskStackView* stackView,
    /* [in] */ ViewPool* viewPool)
    : mConfig(config)
    , mStackView(stackView)
    , mViewPool(viewPool)
{
}

/** Orchestrates the animations of the current child views and any new views. */
void TaskStackViewFilterAlgorithm::StartFilteringAnimation(
    /* [in] */ IArrayList* curTasks, // item is Task
    /* [in] */ IArrayList* curTaskTransforms, // item is TaskViewTransform
    /* [in] */ IArrayList* tasks, // item is Task
    /* [in] */ IArrayList* taskTransforms) // item is TaskViewTrancsform
{
    // Calculate the transforms to animate out all the existing views if they are not in the
    // new visible range (or to their final positions in the stack if they are)
    AutoPtr<IArrayList> childrenToRemove;
    CArrayList::New((IArrayList**)&childrenToRemove);
    AutoPtr<IHashMap> childViewTransforms;
    CHashMap::New((IHashMap**)&childViewTransforms);
    Int32 duration = GetExitTransformsForFilterAnimation(curTasks, curTaskTransforms, tasks,
            taskTransforms, childViewTransforms, childrenToRemove);

    // If all the current views are in the visible range of the new stack, then don't wait for
    // views to animate out and animate all the new views into their place
    Boolean unifyNewViewAnimation;
    childrenToRemove->IsEmpty(&unifyNewViewAnimation);
    if (unifyNewViewAnimation) {
        Int32 inDuration = GetEnterTransformsForFilterAnimation(tasks, taskTransforms,
                childViewTransforms);
        duration = Elastos::Core::Math::Max(duration, inDuration);
    }

    // Animate all the views to their final transforms
    AutoPtr<ISet> set;
    childViewTransforms->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> meo, ko, vo;
        it->GetNext((IInterface**)&meo);
        IMapEntry* me = IMapEntry::Probe(meo);
        me->GetKey((IInterface**)&ko);
        me->GetValue((IInterface**)&vo);
        TaskView* tv = (TaskView*)ITaskView::Probe(ko);
        TaskViewTransform* t = (TaskViewTransform*)IObject::Probe(vo);

        AutoPtr<IViewPropertyAnimator> animate;
        tv->Animate((IViewPropertyAnimator**)&animate);
        animate->Cancel();
        AutoPtr<Runnable> runnable = new EndActionRunnable(childViewTransforms,
            tv, childrenToRemove, unifyNewViewAnimation, tasks, taskTransforms, this);
        animate->WithEndAction(runnable);
        tv->UpdateViewPropertiesToTaskTransform(t, duration);
    }
}

/**
 * Creates the animations for all the children views that need to be animated in when we are
 * un/filtering a stack, and returns the duration for these animations.
 */
Int32 TaskStackViewFilterAlgorithm::GetEnterTransformsForFilterAnimation(
    /* [in] */ IArrayList* tasks, // item is Task
    /* [in] */ IArrayList* taskTransforms, // item is TaskViewTransform
    /* [in] */ IHashMap* childViewTransformsOut) // <TaskView, TaskViewTransform>
{
    Int32 offset = 0;
    Int32 movement = 0;
    Int32 taskCount;
    tasks->GetSize(&taskCount);
    for (Int32 i = taskCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> item, item2;
        tasks->Get(i, (IInterface**)&item);
        taskTransforms->Get(i, (IInterface**)&item2);
        Task* task = (Task*)ITask::Probe(item);
        TaskViewTransform* toTransform = (TaskViewTransform*)IObject::Probe(item2);
        if (toTransform->mVisible) {
            AutoPtr<TaskView> tv = mStackView->GetChildViewForTask(task);
            if (tv == NULL) {
                // For views that are not already visible, animate them in
                tv = (TaskView*)ITaskView::Probe(mViewPool->PickUpViewFromPool((ITask*)task, (ITask*)task));

                // Compose a new transform to fade and slide the new task in
                AutoPtr<TaskViewTransform> fromTransform = new TaskViewTransform(toTransform);
                tv->PrepareTaskTransformForFilterTaskHidden(fromTransform);
                tv->UpdateViewPropertiesToTaskTransform(fromTransform, 0);

                toTransform->mStartDelay = offset * Constants::Values::TaskStackView::FilterStartDelay;
                childViewTransformsOut->Put((ITaskView*)tv, (IObject*)toTransform);

                // Use the movement of the new views to calculate the duration of the animation
                movement = Elastos::Core::Math::Max(movement,
                    Elastos::Core::Math::Abs(toTransform->mTranslationY - fromTransform->mTranslationY));
                offset++;
            }
        }
    }
    return mConfig->mFilteringNewViewsAnimDuration;
}

/**
 * Creates the animations for all the children views that need to be removed or to move views
 * to their un/filtered position when we are un/filtering a stack, and returns the duration
 * for these animations.
 */
Int32 TaskStackViewFilterAlgorithm::GetExitTransformsForFilterAnimation(
    /* [in] */ IArrayList* curTasks, // item is Task
    /* [in] */ IArrayList* curTaskTransforms, // item is TaskViewTransform
    /* [in] */ IArrayList* tasks, // item is Task
    /* [in] */ IArrayList* taskTransforms, // item is TaskViewTransform
    /* [in] */ IHashMap*  childViewTransformsOut, // <TaskView, TaskViewTransform>
    /* [in] */ IArrayList* childrenToRemoveOut) // item is TaskView
{
    // Animate all of the existing views out of view (if they are not in the visible range in
    // the new stack) or to their final positions in the new stack
    Int32 offset = 0;
    Int32 movement = 0;
    Int32 childCount;
    mStackView->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> view;
        mStackView->GetChildAt(i, (IView**)&view);
        TaskView* tv = (TaskView*)ITaskView::Probe(view);
        AutoPtr<Task> task = tv->GetTask();
        Int32 taskIndex;
        tasks->IndexOf((ITask*)task, &taskIndex);
        AutoPtr<TaskViewTransform> toTransform;

        // If the view is no longer visible, then we should just animate it out
        Boolean willBeInvisible = TRUE;
        AutoPtr<TaskViewTransform> transform;
        if (taskIndex >= 0) {
            AutoPtr<IInterface> item;
            taskTransforms->Get(taskIndex, (IInterface**)&item);
            transform = (TaskViewTransform*)IObject::Probe(item);
            willBeInvisible = !transform->mVisible;
        }
        if (willBeInvisible) {
            if (taskIndex < 0) {
                Int32 index;
                curTasks->IndexOf((ITask*)task, &index);
                AutoPtr<IInterface> item;
                curTaskTransforms->Get(index, (IInterface**)&item);
                toTransform = (TaskViewTransform*)IObject::Probe(item);
            }
            else {
                toTransform = new TaskViewTransform(transform);
            }
            tv->PrepareTaskTransformForFilterTaskVisible(toTransform);
            childrenToRemoveOut->Add((ITaskView*)tv);
        }
        else {
            toTransform = transform;
            // Use the movement of the visible views to calculate the duration of the animation
            Float y;
            tv->GetTranslationY(&y);
            movement = Elastos::Core::Math::Max(movement, Elastos::Core::Math::Abs(
                toTransform->mTranslationY - (Int32)y));
        }

        toTransform->mStartDelay = offset * Constants::Values::TaskStackView::FilterStartDelay;
        childViewTransformsOut->Put((ITaskView*)tv, (IObject*)toTransform);
        offset++;
    }
    return mConfig->mFilteringCurrentViewsAnimDuration;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
