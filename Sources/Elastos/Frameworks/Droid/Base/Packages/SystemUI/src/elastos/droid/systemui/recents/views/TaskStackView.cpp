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

#include "elastos/droid/systemui/recents/model/TaskGrouping.h"
#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "elastos/droid/systemui/recents/views/TaskStackView.h"
#include "elastos/droid/systemui/recents/views/CTaskStackViewScroller.h"
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "../R.h"

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::SystemUI::Recents::Model::EIID_IPackageCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::EIID_ITaskStackCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::TaskGrouping;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;
using Elastos::Droid::SystemUI::Recents::Misc::Utilities;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

static const String TAG("TaskStackView");

//=======================================================
// TaskStackView::Wrapper
//=======================================================
CAR_INTERFACE_IMPL_5(TaskStackView::Wrapper, Object, ITaskStackCallbacks, ITaskViewCallbacks,
    ITaskStackViewScrollerCallbacks, IViewPoolConsumer, IPackageCallbacks)

TaskStackView::Wrapper::Wrapper(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::Wrapper::OnStackTaskAdded(
    /* [in] */ ITaskStack* stack,
    /* [in] */ ITask* t)
{
    return mHost->OnStackTaskAdded(stack, t);
}

ECode TaskStackView::Wrapper::OnStackTaskRemoved(
    /* [in] */ ITaskStack* stack,
    /* [in] */ ITask* removedTask,
    /* [in] */ ITask* newFrontMostTask)
{
    return mHost->OnStackTaskRemoved(stack, removedTask, newFrontMostTask);
}

ECode TaskStackView::Wrapper::OnStackFiltered(
    /* [in] */ ITaskStack* newStack,
    /* [in] */ IArrayList* curTasks,
    /* [in] */ ITask* filteredTask)
{
    return mHost->OnStackFiltered(newStack, curTasks, filteredTask);
}

ECode TaskStackView::Wrapper::OnStackUnfiltered(
    /* [in] */ ITaskStack* newStack,
    /* [in] */ IArrayList* curTasks)
{
    return mHost->OnStackUnfiltered(newStack, curTasks);
}

ECode TaskStackView::Wrapper::CreateView(
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)
{
    return mHost->CreateView(context, result);
}

ECode TaskStackView::Wrapper::PrepareViewToEnterPool(
    /* [in] */ IInterface* tv)
{
    return mHost->PrepareViewToEnterPool(ITaskView::Probe(tv));
}

ECode TaskStackView::Wrapper::PrepareViewToLeavePool(
    /* [in] */ IInterface* tv,
    /* [in] */ IInterface* task,
    /* [in] */ Boolean isNewView)
{
    return mHost->PrepareViewToLeavePool(ITaskView::Probe(tv), ITask::Probe(task), isNewView);
}

ECode TaskStackView::Wrapper::HasPreferredData(
    /* [in] */ IInterface* tv,
    /* [in] */ IInterface* preferredData,
    /* [out] */ Boolean* result)
{
    return mHost->HasPreferredData(ITaskView::Probe(tv), ITask::Probe(preferredData), result);
}

ECode TaskStackView::Wrapper::OnTaskViewAppIconClicked(
    /* [in] */ ITaskView* tv)
{
    return mHost->OnTaskViewAppIconClicked(tv);
}

ECode TaskStackView::Wrapper::OnTaskViewAppInfoClicked(
    /* [in] */ ITaskView* tv)
{
    return mHost->OnTaskViewAppInfoClicked(tv);
}

ECode TaskStackView::Wrapper::OnTaskViewClicked(
    /* [in] */ ITaskView* tv,
    /* [in] */ ITask* task,
    /* [in] */ Boolean lockToTask)
{
    return mHost->OnTaskViewClicked(tv, task, lockToTask);
}

ECode TaskStackView::Wrapper::OnTaskViewDismissed(
    /* [in] */ ITaskView* tv)
{
    return mHost->OnTaskViewDismissed(tv);
}

ECode TaskStackView::Wrapper::OnTaskViewClipStateChanged(
    /* [in] */ ITaskView* tv)
{
    return mHost->OnTaskViewClipStateChanged(tv);
}

ECode TaskStackView::Wrapper::OnTaskViewFullScreenTransitionCompleted()
{
    return mHost->OnTaskViewFullScreenTransitionCompleted();
}

ECode TaskStackView::Wrapper::OnTaskViewFocusChanged(
    /* [in] */ ITaskView* tv,
    /* [in] */ Boolean focused)
{
    return mHost->OnTaskViewFocusChanged(tv, focused);
}

ECode TaskStackView::Wrapper::OnScrollChanged(
    /* [in] */ Float p)
{
    return mHost->OnScrollChanged(p);
}


ECode TaskStackView::Wrapper::OnComponentRemoved(
    /* [in] */ IHashSet* cns)
{
    return mHost->OnComponentRemoved(cns);
}

//=======================================================
// TaskStackView::AnimatorUpdateListener
//=======================================================
CAR_INTERFACE_IMPL(TaskStackView::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskStackView::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    mHost->RequestUpdateStackViewsClip();
    return NOERROR;
}

//=======================================================
// TaskStackView::ReturnAllViewsToPoolRunnable
//=======================================================
TaskStackView::ReturnAllViewsToPoolRunnable::ReturnAllViewsToPoolRunnable(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::ReturnAllViewsToPoolRunnable::SetTaskViewExitContext(
    /* [in] */ ViewAnimation::TaskViewExitContext* ctx)
{
    mTaskViewExitContext = ctx;
    return NOERROR;
}

ECode TaskStackView::ReturnAllViewsToPoolRunnable::Run()
{
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        mHost->mViewPool->ReturnViewToPool(v);
        // Also hide the view since we don't need it anymore
        tv->SetVisibility(IView::INVISIBLE);
    }
    mTaskViewExitContext = NULL;
    return NOERROR;
}

//=======================================================
// TaskStackView::SleepRunnable
//=======================================================
TaskStackView::SleepRunnable::SleepRunnable(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::SleepRunnable::Run()
{
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        mHost->GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        tv->StartNoUserInteractionAnimation();
    }
    return NOERROR;
}

//=======================================================
// TaskStackView::PostScrollRunnable
//=======================================================
TaskStackView::PostScrollRunnable::PostScrollRunnable(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::PostScrollRunnable::Run()
{
    AutoPtr<IInterface> item;
    mHost->mStack->GetTasks()->Get(mHost->mFocusedTaskIndex, (IInterface**)&item);
    Task* t = (Task*)ITask::Probe(item);
    AutoPtr<TaskView> tv = mHost->GetChildViewForTask(t);
    if (tv != NULL) {
        tv->SetFocusedTask();
    }
    return NOERROR;
}

//=======================================================
// TaskStackView::LastDecrementRunnable
//=======================================================
TaskStackView::LastDecrementRunnable::LastDecrementRunnable(
    /* [in] */ TaskStackView* host)
    : mHost(host)
{}

ECode TaskStackView::LastDecrementRunnable::SetTaskViewEnterContext(
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
{
    mTaskViewEnterContext = ctx;
    return NOERROR;
}

ECode TaskStackView::LastDecrementRunnable::Run()
{
    mHost->mStartEnterAnimationCompleted = TRUE;
    // Start dozing
    mHost->mUIDozeTrigger->StartDozing();
    // Focus the first view if accessibility is enabled
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    loader->GetSystemServicesProxy((SystemServicesProxy**)&ssp);
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    if (childCount > 0 && ssp->IsTouchExplorationEnabled()) {
        AutoPtr<IView> v;
        mHost->GetChildAt(childCount - 1, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        Boolean res;
        tv->RequestAccessibilityFocus(&res);
        mHost->mPrevAccessibilityFocusedIndex = mHost->mStack->IndexOfTask(tv->GetTask());
    }
    mTaskViewEnterContext = NULL;
    return NOERROR;
}

//=======================================================
// TaskStackView::DeleteTaskRunnable
//=======================================================
TaskStackView::DeleteTaskRunnable::DeleteTaskRunnable(
    /* [in] */ TaskStackView* host,
    /* [in] */ Task* t)
    : mHost(host)
    , mT(t)
{}

ECode TaskStackView::DeleteTaskRunnable::Run()
{
    mHost->mStack->RemoveTask(mT);
    return NOERROR;
}

//=======================================================
// TaskStackView
//=======================================================
CAR_INTERFACE_IMPL(TaskStackView, FrameLayout, ITaskStackView)

TaskStackView::TaskStackView()
    : mFocusedTaskIndex(-1)
    , mPrevAccessibilityFocusedIndex(-1)
    , mStackViewsAnimationDuration(0)
    , mStackViewsDirty(TRUE)
    , mStackViewsClipDirty(TRUE)
    , mAwaitingFirstLayout(TRUE)
    , mStartEnterAnimationRequestedAfterLayout(FALSE)
    , mStartEnterAnimationCompleted(FALSE)
{
}

ECode TaskStackView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITaskStack* stack)
{
    FAIL_RETURN(FrameLayout::constructor(context));

    CArrayList::New((IArrayList**)&mCurrentTaskTransforms);
    CRect::New((IRect**)&mTaskStackBounds);
    mTmpVisibleRange = ArrayOf<Int32>::Alloc(2);
    mTmpCoord = ArrayOf<Float>::Alloc(2);
    CMatrix::New((IMatrix**)&mTmpMatrix);
    CRect::New((IRect**)&mTmpRect);
    mTmpTransform = new TaskViewTransform();
    CHashMap::New((IHashMap**)&mTmpTaskViewMap);
    mRequestUpdateClippingListener = new AnimatorUpdateListener(this);
    mReturnAllViewsToPoolRunnable = new ReturnAllViewsToPoolRunnable(this);
    mWrapper = new Wrapper(this);

    mConfig = RecentsConfiguration::GetInstance();
    mStack = (TaskStack*)stack;
    mStack->SetCallbacks(mWrapper);
    mViewPool = new ViewPool(context, mWrapper);
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    mLayoutAlgorithm = new TaskStackViewLayoutAlgorithm(mConfig);
    mFilterAlgorithm = new TaskStackViewFilterAlgorithm(mConfig, this, mViewPool);

    AutoPtr<ITaskStackViewScroller> scroller;
    CTaskStackViewScroller::New(context, mConfig.Get(), mLayoutAlgorithm.Get(),
        (ITaskStackViewScroller**)&scroller);
    mStackScroller = (TaskStackViewScroller*)scroller.Get();

    mStackScroller->SetCallbacks(mWrapper);
    mTouchHandler = new TaskStackViewTouchHandler(context, this, mConfig, mStackScroller);
    AutoPtr<Runnable> runnable = new SleepRunnable(this);
    mUIDozeTrigger = new DozeTrigger();
    mUIDozeTrigger->constructor(mConfig->mTaskBarDismissDozeDelaySeconds, runnable);
    return NOERROR;
}

/** Sets the callbacks */
void TaskStackView::SetCallbacks(
    /* [in] */ ITaskStackViewCallbacks* cb)
{
    mCb = cb;
}

/** Sets the debug overlay */
void TaskStackView::SetDebugOverlay(
    /* [in] */ DebugOverlayView* overlay)
{
    mDebugOverlay = overlay;
}

/** Requests that the views be synchronized with the model */
void TaskStackView::RequestSynchronizeStackViewsWithModel()
{
    RequestSynchronizeStackViewsWithModel(0);
}

void TaskStackView::RequestSynchronizeStackViewsWithModel(
    /* [in] */ Int32 duration)
{
    if (!mStackViewsDirty) {
        Invalidate();
        mStackViewsDirty = TRUE;
    }
    if (mAwaitingFirstLayout) {
        // Skip the animation if we are awaiting first layout
        mStackViewsAnimationDuration = 0;
    }
    else {
        mStackViewsAnimationDuration = Elastos::Core::Math::Max(mStackViewsAnimationDuration, duration);
    }
}

/** Requests that the views clipping be updated. */
void TaskStackView::RequestUpdateStackViewsClip()
{
    if (!mStackViewsClipDirty) {
        Invalidate();
        mStackViewsClipDirty = TRUE;
    }
}

/** Finds the child view given a specific task. */
AutoPtr<TaskView> TaskStackView::GetChildViewForTask(
    /* [in] */ Task* t)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        if (tv->GetTask().Get() == t) {
            return tv;
        }
    }
    return NULL;
}

/** Returns the stack algorithm for this task stack. */
AutoPtr<TaskStackViewLayoutAlgorithm> TaskStackView::GetStackAlgorithm()
{
    return mLayoutAlgorithm;
}

/**
 * Gets the stack transforms of a list of tasks, and returns the visible range of tasks.
 */
Boolean TaskStackView::UpdateStackTransforms(
    /* [in] */ IArrayList* taskTransforms,
    /* [in] */ IArrayList* tasks,
    /* [in] */ Float stackScroll,
    /* [in] */ ArrayOf<Int32>* visibleRangeOut,
    /* [in] */ Boolean boundTranslationsToRect)
{
    // XXX: We should be intelligent about where to look for the visible stack range using the
    //      current stack scroll.
    // XXX: We should log extra cases like the ones below where we don't expect to hit very often
    // XXX: Print out approximately how many indices we have to go through to find the first visible transform

    Int32 taskTransformCount;
    taskTransforms->GetSize(&taskTransformCount);
    Int32 taskCount;
    tasks->GetSize(&taskCount);
    Int32 frontMostVisibleIndex = -1;
    Int32 backMostVisibleIndex = -1;

    // We can reuse the task transforms where possible to reduce object allocation
    if (taskTransformCount < taskCount) {
        // If there are less transforms than tasks, then add as many transforms as necessary
        for (Int32 i = taskTransformCount; i < taskCount; i++) {
            AutoPtr<TaskViewTransform> tvt = new TaskViewTransform();
            taskTransforms->Add((IObject*)tvt);
        }
    }
    else if (taskTransformCount > taskCount) {
        // If there are more transforms than tasks, then just subset the transform list
        AutoPtr<IList> subList;
        taskTransforms->GetSubList(0, taskCount, (IList**)&subList);
    }

    // Update the stack transforms
    AutoPtr<TaskViewTransform> prevTransform;
    for (Int32 i = taskCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> item, item2;
        tasks->Get(i, (IInterface**)&item);
        taskTransforms->Get(i, (IInterface**)&item2);
        AutoPtr<TaskViewTransform> transform = mLayoutAlgorithm->GetStackTransform(
                (Task*)ITask::Probe(item), stackScroll,
                (TaskViewTransform*)IObject::Probe(item2), prevTransform);
        if (transform->mVisible) {
            if (frontMostVisibleIndex < 0) {
                frontMostVisibleIndex = i;
            }
            backMostVisibleIndex = i;
        }
        else {
            if (backMostVisibleIndex != -1) {
                // We've reached the end of the visible range, so going down the rest of the
                // stack, we can just reset the transforms accordingly
                while (i >= 0) {
                    item2 = NULL;
                    taskTransforms->Get(i, (IInterface**)&item2);
                    ((TaskViewTransform*)IObject::Probe(item2))->Reset();
                    i--;
                }
                break;
            }
        }

        if (boundTranslationsToRect) {
            Int32 bottom;
            mLayoutAlgorithm->mViewRect->GetBottom(&bottom);
            transform->mTranslationY = Elastos::Core::Math::Min(transform->mTranslationY, bottom);
        }
        prevTransform = transform;
    }
    if (visibleRangeOut != NULL) {
        (*visibleRangeOut)[0] = frontMostVisibleIndex;
        (*visibleRangeOut)[1] = backMostVisibleIndex;
    }
    return frontMostVisibleIndex != -1 && backMostVisibleIndex != -1;
}

/**
 * Gets the stack transforms of a list of tasks, and returns the visible range of tasks. This
 * call is less optimal than calling UpdateStackTransforms directly.
 */
AutoPtr<IArrayList> TaskStackView::GetStackTransforms(
    /* [in] */ IArrayList* tasks,
    /* [in] */ Float stackScroll,
    /* [in] */ ArrayOf<Int32>* visibleRangeOut,
    /* [in] */ Boolean boundTranslationsToRect)
{
    AutoPtr<IArrayList> taskTransforms;
    CArrayList::New((IArrayList**)&taskTransforms);
    UpdateStackTransforms(taskTransforms, tasks, stackScroll, visibleRangeOut,
            boundTranslationsToRect);
    return taskTransforms;
}

/** Synchronizes the views with the model */
Boolean TaskStackView::SynchronizeStackViewsWithModel()
{
    if (mStackViewsDirty) {
        AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
        AutoPtr<SystemServicesProxy> ssp;
        loader->GetSystemServicesProxy((SystemServicesProxy**)&ssp);

        // Get all the task transforms
        AutoPtr<IArrayList> tasks = mStack->GetTasks();
        Float stackScroll = mStackScroller->GetStackScroll();
        AutoPtr<ArrayOf<Int32> > visibleRange = mTmpVisibleRange;
        Boolean isValidVisibleRange = UpdateStackTransforms(mCurrentTaskTransforms, tasks,
                stackScroll, visibleRange, FALSE);
        if (mDebugOverlay != NULL) {
            StringBuilder sb;
            sb += "vis[";
            sb += (*visibleRange)[1];
            sb += "-";
            sb += (*visibleRange)[0];
            sb += "]";
            mDebugOverlay->SetText(sb.ToString());
        }

        // Return all the invisible children to the pool
        mTmpTaskViewMap->Clear();
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = childCount - 1; i >= 0; i--) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            TaskView* tv = (TaskView*)ITaskView::Probe(v);
            AutoPtr<Task> task = tv->GetTask();
            Int32 taskIndex = mStack->IndexOfTask(task);
            if ((*visibleRange)[1] <= taskIndex && taskIndex <= (*visibleRange)[0]) {
                mTmpTaskViewMap->Put((ITask*)task, v);
            }
            else {
                mViewPool->ReturnViewToPool(v);
            }
        }

        // Pick up all the newly visible children and update all the existing children
        for (Int32 i = (*visibleRange)[0]; isValidVisibleRange && i >= (*visibleRange)[1]; i--) {
            AutoPtr<IInterface> item1, item2;
            tasks->Get(i, (IInterface**)&item1);
            mCurrentTaskTransforms->Get(i, (IInterface**)&item2);
            Task* task = (Task*)ITask::Probe(item1);
            TaskViewTransform* transform = (TaskViewTransform*)IObject::Probe(item2);
            AutoPtr<IInterface> _tv;
            mTmpTaskViewMap->Get(item1, (IInterface**)&_tv);
            TaskView* tv = (TaskView*)ITaskView::Probe(_tv);
            Int32 taskIndex = mStack->IndexOfTask(task);

            if (tv == NULL) {
                _tv = mViewPool->PickUpViewFromPool(item1, item1);
                tv = (TaskView*)ITaskView::Probe(_tv);

                if (mStackViewsAnimationDuration > 0) {
                    // For items in the list, put them in start animating them from the
                    // approriate ends of the list where they are expected to appear
                    if (Elastos::Core::Math::Compare(transform->mP, 0.0f) <= 0) {
                        mLayoutAlgorithm->GetStackTransform(0.0f, 0.0f, mTmpTransform, NULL); }
                    else {
                        mLayoutAlgorithm->GetStackTransform(1.0f, 0.0f, mTmpTransform, NULL); }
                    tv->UpdateViewPropertiesToTaskTransform(mTmpTransform, 0);
                }
            }

            // Animate the task into place
            item2 = NULL;
            mCurrentTaskTransforms->Get(taskIndex, (IInterface**)&item2);
            tv->UpdateViewPropertiesToTaskTransform((TaskViewTransform*)IObject::Probe(item2),
                    mStackViewsAnimationDuration, mRequestUpdateClippingListener);

            // Request accessibility focus on the next view if we removed the task
            // that previously held accessibility focus
            GetChildCount(&childCount);
            if (childCount > 0 && ssp->IsTouchExplorationEnabled()) {
                AutoPtr<IView> v;
                GetChildAt(childCount - 1, (IView**)&v);
                TaskView* atv = (TaskView*)ITaskView::Probe(v);
                Int32 indexOfTask = mStack->IndexOfTask(atv->GetTask());
                if (mPrevAccessibilityFocusedIndex != indexOfTask) {
                    Boolean res;
                    tv->RequestAccessibilityFocus(&res);
                    mPrevAccessibilityFocusedIndex = indexOfTask;
                }
            }
        }

        // Reset the request-synchronize params
        mStackViewsAnimationDuration = 0;
        mStackViewsDirty = FALSE;
        mStackViewsClipDirty = TRUE;
        return TRUE;
    }
    return FALSE;
}

/** Updates the clip for each of the task views. */
void TaskStackView::ClipTaskViews()
{
    // Update the clip on each task child
    if (Constants::DebugFlags::App::EnableTaskStackClipping) {
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount - 1; i++) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
            AutoPtr<TaskView> nextTv;
            AutoPtr<TaskView> tmpTv;
            Int32 clipBottom = 0;
            if (tv->ShouldClipViewInStack()) {
                // Find the next view to clip against
                Int32 nextIndex = i;
                while (nextIndex < childCount) {
                    v = NULL;
                    GetChildAt(++nextIndex, (IView**)&v);
                    tmpTv = (TaskView*)ITaskView::Probe(v);
                    if (tmpTv != NULL && tmpTv->ShouldClipViewInStack()) {
                        nextTv = tmpTv;
                        break;
                    }
                }

                // Clip against the next view, this is just an approximation since we are
                // stacked and we can make assumptions about the visibility of the this
                // task relative to the ones in front of it.
                if (nextTv != NULL) {
                    // Map the top edge of next task view into the local space of the current
                    // task view to find the clip amount in local space
                    (*mTmpCoord)[0] = (*mTmpCoord)[1] = 0;
                    Utilities::MapCoordInDescendentToSelf(nextTv, this, mTmpCoord, FALSE);
                    Utilities::MapCoordInSelfToDescendent(tv, this, mTmpCoord, mTmpMatrix);
                    Int32 height, top;
                    tv->GetMeasuredHeight(&height);
                    nextTv->GetPaddingTop(&top);
                    clipBottom = (Int32) Elastos::Core::Math::Floor(
                        height - (*mTmpCoord)[1] - top - 1);
                }
            }
            tv->GetViewBounds()->SetClipBottom(clipBottom);
        }
        if (childCount > 0) {
            // The front most task should never be clipped
            AutoPtr<IView> v;
            GetChildAt(childCount - 1, (IView**)&v);
            AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
            tv->GetViewBounds()->SetClipBottom(0);
        }
    }
    mStackViewsClipDirty = FALSE;
}

/** The stack insets to apply to the stack contents */
void TaskStackView::SetStackInsetRect(
    /* [in] */ IRect* r)
{
    mTaskStackBounds->Set(r);
}

/** Updates the min and max virtual scroll bounds */
void TaskStackView::UpdateMinMaxScroll(
    /* [in] */ Boolean boundScrollToNewMinMax,
    /* [in] */ Boolean launchedWithAltTab,
    /* [in] */ Boolean launchedFromHome)
{
    // Compute the min and max scroll values
    mLayoutAlgorithm->ComputeMinMaxScroll(mStack->GetTasks(), launchedWithAltTab, launchedFromHome);
    // Debug logging
    if (boundScrollToNewMinMax) {
        mStackScroller->BoundScroll();
    }
}

/** Returns the scroller. */
AutoPtr<TaskStackViewScroller> TaskStackView::GetScroller()
{
    return mStackScroller;
}

/** Focuses the task at the specified index in the stack */
void TaskStackView::FocusTask(
    /* [in] */ Int32 taskIndex,
    /* [in] */ Boolean scrollToNewPosition)
{
    // Return early if the task is already focused
    if (taskIndex == mFocusedTaskIndex) return;

    if (0 <= taskIndex && taskIndex < mStack->GetTaskCount()) {
        mFocusedTaskIndex = taskIndex;

        // Focus the view if possible, otherwise, focus the view after we scroll into position
        AutoPtr<IInterface> item;
        mStack->GetTasks()->Get(taskIndex, (IInterface**)&item);
        Task* t = (Task*)ITask::Probe(item);
        AutoPtr<TaskView> tv = GetChildViewForTask(t);
        AutoPtr<Runnable> postScrollRunnable;
        if (tv != NULL) {
            tv->SetFocusedTask();
        }
        else {
            postScrollRunnable = new PostScrollRunnable(this);
        }

        // Scroll the view into position (just center it in the curve)
        if (scrollToNewPosition) {
            Float newScroll = mLayoutAlgorithm->GetStackScrollForTask(t) - 0.5f;
            newScroll = mStackScroller->GetBoundedStackScroll(newScroll);
            mStackScroller->AnimateScroll(mStackScroller->GetStackScroll(), newScroll, postScrollRunnable);
        }
        else {
            if (postScrollRunnable != NULL) {
                postScrollRunnable->Run();
            }
        }

    }
}

/** Focuses the next task in the stack */
void TaskStackView::FocusNextTask(
    /* [in] */ Boolean forward)
{
    // Find the next index to focus
    Int32 numTasks = mStack->GetTaskCount();
    if (numTasks == 0) return;

    Int32 nextFocusIndex = numTasks - 1;
    if (0 <= mFocusedTaskIndex && mFocusedTaskIndex < numTasks) {
        nextFocusIndex = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(numTasks - 1,
                mFocusedTaskIndex + (forward ? -1 : 1)));
    }
    FocusTask(nextFocusIndex, TRUE);
}

/** Dismisses the focused task. */
void TaskStackView::DismissFocusedTask()
{
    // Return early if there is no focused task index
    if (mFocusedTaskIndex < 0) return;

    AutoPtr<IInterface> item;
    mStack->GetTasks()->Get(mFocusedTaskIndex, (IInterface**)&item);
    Task* t = (Task*)ITask::Probe(item);
    AutoPtr<TaskView> tv = GetChildViewForTask(t);
    tv->DismissTask();
}

ECode TaskStackView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord* record = IAccessibilityRecord::Probe(event);
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount > 0) {
        AutoPtr<IView> v;
        GetChildAt(0, (IView**)&v);
        AutoPtr<TaskView> backMostTask = (TaskView*)ITaskView::Probe(v);
        v = NULL;
        GetChildAt(childCount - 1, (IView**)&v);
        AutoPtr<TaskView> frontMostTask = (TaskView*)ITaskView::Probe(v);
        record->SetFromIndex(mStack->IndexOfTask(backMostTask->GetTask()));
        record->SetToIndex(mStack->IndexOfTask(frontMostTask->GetTask()));
        record->SetContentDescription(CoreUtils::Convert(
            frontMostTask->GetTask()->mActivityLabel));
    }
    record->SetItemCount(mStack->GetTaskCount());
    Int32 y;
    mStackScroller->mScroller->GetCurrY(&y);
    record->SetScrollY(y);
    record->SetMaxScrollY(mStackScroller->ProgressToScrollRange(mLayoutAlgorithm->mMaxScrollP));
    return NOERROR;
}

ECode TaskStackView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTouchHandler->OnInterceptTouchEvent(ev);
    return NOERROR;
}

ECode TaskStackView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTouchHandler->OnTouchEvent(ev);
    return NOERROR;
}

ECode TaskStackView::ComputeScroll()
{
    mStackScroller->ComputeScroll();
    // Synchronize the views
    SynchronizeStackViewsWithModel();
    ClipTaskViews();
    // Notify accessibility
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SCROLLED);
    return NOERROR;
}

/** Computes the stack and task rects */
void TaskStackView::ComputeRects(
    /* [in] */ Int32 windowWidth,
    /* [in] */ Int32 windowHeight,
    /* [in] */ IRect* taskStackBounds,
    /* [in] */ Boolean launchedWithAltTab,
    /* [in] */ Boolean launchedFromHome)
{
    // Compute the rects in the stack algorithm
    mLayoutAlgorithm->ComputeRects(windowWidth, windowHeight, taskStackBounds);
    // Update the scroll bounds
    UpdateMinMaxScroll(FALSE, launchedWithAltTab, launchedFromHome);
}

/**
 * This is ONLY used from AlternateRecentsComponent to update the dummy stack view for purposes
 * of getting the task rect to animate to.
 */
void TaskStackView::UpdateMinMaxScrollForStack(
    /* [in] */ TaskStack* stack,
    /* [in] */ Boolean launchedWithAltTab,
    /* [in] */ Boolean launchedFromHome)
{
    mStack = stack;
    UpdateMinMaxScroll(FALSE, launchedWithAltTab, launchedFromHome);
}

/**
 * This is called with the full window width and height to allow stack view children to
 * perform the full screen transition down.
 */
ECode TaskStackView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = MeasureSpec::GetSize(heightMeasureSpec);

    // Compute our stack/task rects
    AutoPtr<IRect> taskStackBounds;
    CRect::New(mTaskStackBounds, (IRect**)&taskStackBounds);
    Int32 b1, b2;
    taskStackBounds->GetBottom(&b1);
    mConfig->mSystemInsets->GetBottom(&b2);
    b1 -= b2;
    taskStackBounds->SetBottom(b1);
    ComputeRects(width, height, taskStackBounds, mConfig->mLaunchedWithAltTab,
            mConfig->mLaunchedFromHome);

    // If this is the first layout, then scroll to the front of the stack and synchronize the
    // stack views immediately to load all the views
    if (mAwaitingFirstLayout) {
        mStackScroller->SetStackScrollToInitialState();
        RequestSynchronizeStackViewsWithModel();
        SynchronizeStackViewsWithModel();
    }

    // Measure each of the TaskViews
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        if (tv->IsFullScreenView()) {
            tv->Measure(widthMeasureSpec, heightMeasureSpec);
        }
        else {
            AutoPtr<IDrawable> drawable;
            tv->GetBackground((IDrawable**)&drawable);
            if (drawable != NULL) {
                Boolean res;
                drawable->GetPadding(mTmpRect, &res);
            }
            else {
                mTmpRect->SetEmpty();
            }
            Int32 w, h, left, right, top, bottom;
            mLayoutAlgorithm->mTaskRect->GetWidth(&w);
            mLayoutAlgorithm->mTaskRect->GetHeight(&h);
            mTmpRect->GetLeft(&left);
            mTmpRect->GetRight(&right);
            mTmpRect->GetTop(&top);
            mTmpRect->GetBottom(&bottom);
            tv->Measure(
                MeasureSpec::MakeMeasureSpec(w + left + right, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(h + top + bottom + tv->GetMaxFooterHeight(),
                    MeasureSpec::EXACTLY));
        }
    }

    SetMeasuredDimension(width, height);
    return NOERROR;
}

/**
 * This is called with the size of the space not including the top or right insets, or the
 * search bar height in portrait (but including the search bar width in landscape, since we want
 * to draw under it.
 */
ECode TaskStackView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    // Layout each of the children
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        if (tv->IsFullScreenView()) {
            Int32 width, height;
            tv->GetMeasuredWidth(&width);
            tv->GetMeasuredHeight(&height);
            tv->Layout(left, top, left + width, top + height);
        }
        else {
            AutoPtr<IDrawable> drawable;
            tv->GetBackground((IDrawable**)&drawable);
            if (drawable != NULL) {
                Boolean res;
                drawable->GetPadding(mTmpRect, &res);
            }
            else {
                mTmpRect->SetEmpty();
            }
            Int32 l1, r1, t1, b1;
            mLayoutAlgorithm->mTaskRect->Get(&l1, &t1, &r1, &b1);
            Int32 l2, r2, t2, b2;
            mTmpRect->Get(&l2, &t2, &r2, &b2);
            tv->Layout(l1 - l2, t1 - t2, r1 + r2, b1 + b2 + tv->GetMaxFooterHeight());
        }
    }

    if (mAwaitingFirstLayout) {
        mAwaitingFirstLayout = FALSE;
        OnFirstLayout();
    }
    return NOERROR;
}

/** Handler for the first layout. */
void TaskStackView::OnFirstLayout()
{
    Int32 top1, top2, bottom;
    mLayoutAlgorithm->mTaskRect->GetTop(&top1);
    mLayoutAlgorithm->mViewRect->GetTop(&top2);
    mLayoutAlgorithm->mViewRect->GetBottom(&bottom);
    Int32 offscreenY = bottom - (top1 - top2);

    // Find the launch target task
    AutoPtr<Task> launchTargetTask;
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        AutoPtr<Task> task = tv->GetTask();
        if (task->mIsLaunchTarget) {
            launchTargetTask = task;
            break;
        }
    }

    // Prepare the first view for its enter animation
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        AutoPtr<Task> task = tv->GetTask();
        AutoPtr<TaskGrouping> tg;
        if (launchTargetTask != NULL) {
            tg = (TaskGrouping*)launchTargetTask->mGroup.Get();
        }
        Boolean occludesLaunchTarget = (tg != NULL) && tg->IsTaskAboveTask(task, launchTargetTask);
        tv->PrepareEnterRecentsAnimation(task->mIsLaunchTarget, occludesLaunchTarget, offscreenY);
    }

    // If the enter animation started already and we haven't completed a layout yet, do the
    // enter animation now
    if (mStartEnterAnimationRequestedAfterLayout) {
        StartEnterRecentsAnimation(mStartEnterAnimationContext);
        mStartEnterAnimationRequestedAfterLayout = FALSE;
        mStartEnterAnimationContext = NULL;
    }

    // When Alt-Tabbing, we scroll to and focus the previous task
    if (mConfig->mLaunchedWithAltTab) {
        if (mConfig->mLaunchedFromHome) {
            FocusTask(Elastos::Core::Math::Max(0, mStack->GetTaskCount() - 1), FALSE);
        }
        else {
            FocusTask(Elastos::Core::Math::Max(0, mStack->GetTaskCount() - 2), FALSE);
        }
    }
}

/** Requests this task stacks to start it's enter-recents animation */
void TaskStackView::StartEnterRecentsAnimation(
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
{
    // If we are still waiting to layout, then just defer until then
    if (mAwaitingFirstLayout) {
        mStartEnterAnimationRequestedAfterLayout = TRUE;
        mStartEnterAnimationContext = ctx;
        return;
    }

    if (mStack->GetTaskCount() > 0) {
        // Find the launch target task
        AutoPtr<Task> launchTargetTask;
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = childCount - 1; i >= 0; i--) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            TaskView* tv = (TaskView*)ITaskView::Probe(v);
            AutoPtr<Task> task = tv->GetTask();
            if (task->mIsLaunchTarget) {
                launchTargetTask = task;
                break;
            }
        }

        // Animate all the task views into view
        for (Int32 i = childCount - 1; i >= 0; i--) {
            AutoPtr<IView> v;
            GetChildAt(i, (IView**)&v);
            TaskView* tv = (TaskView*)ITaskView::Probe(v);
            AutoPtr<Task> task = tv->GetTask();
            ctx->mCurrentTaskTransform = new TaskViewTransform();
            ctx->mCurrentStackViewIndex = i;
            ctx->mCurrentStackViewCount = childCount;
            ctx->mCurrentTaskRect = mLayoutAlgorithm->mTaskRect;
            AutoPtr<TaskGrouping> tg;
            if (launchTargetTask != NULL) {
                tg = (TaskGrouping*)launchTargetTask->mGroup.Get();
            }
            ctx->mCurrentTaskOccludesLaunchTarget = (tg != NULL)
                && tg->IsTaskAboveTask(task, launchTargetTask);
            ctx->mUpdateListener = mRequestUpdateClippingListener;
            mLayoutAlgorithm->GetStackTransform(task, mStackScroller->GetStackScroll(),
                ctx->mCurrentTaskTransform, NULL);
            tv->StartEnterRecentsAnimation(ctx);
        }

        // Add a runnable to the post animation ref counter to clear all the views
        AutoPtr<LastDecrementRunnable> runnable = new LastDecrementRunnable(this);
        runnable->SetTaskViewEnterContext(ctx); // holder refcount of ctx here.
        ctx->mPostAnimationTrigger->AddLastDecrementRunnable(runnable);
    }
}

/** Requests this task stacks to start it's exit-recents animation. */
void TaskStackView::StartExitToHomeAnimation(
    /* [in] */ ViewAnimation::TaskViewExitContext* ctx)
{
    // Stop any scrolling
    mStackScroller->StopScroller();
    mStackScroller->StopBoundScrollAnimation();
    // Animate all the task views out of view
    Int32 top1, top2, bottom;
    mLayoutAlgorithm->mTaskRect->GetTop(&top1);
    mLayoutAlgorithm->mViewRect->GetTop(&top2);
    mLayoutAlgorithm->mViewRect->GetBottom(&bottom);
    ctx->mOffscreenTranslationY = bottom - (top1 - top2);

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* tv = (TaskView*)ITaskView::Probe(v);
        tv->StartExitToHomeAnimation(ctx);
    }

    // Add a runnable to the post animation ref counter to clear all the views
    mReturnAllViewsToPoolRunnable->SetTaskViewExitContext(ctx); // holder refcount of ctx here.
    ctx->mPostAnimationTrigger->AddLastDecrementRunnable(mReturnAllViewsToPoolRunnable);
}

/** Animates a task view in this stack as it launches. */
void TaskStackView::StartLaunchTaskAnimation(
    /* [in] */ TaskView* tv,
    /* [in] */ IRunnable* r,
    /* [in] */ Boolean lockToTask)
{
    AutoPtr<Task> launchTargetTask = tv->GetTask();
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        TaskView* t = (TaskView*)ITaskView::Probe(v);
        if (t == tv) {
            t->SetClipViewInStack(FALSE);
            t->StartLaunchTaskAnimation(r, TRUE, TRUE, lockToTask);
        }
        else {
            AutoPtr<TaskGrouping> tg = (TaskGrouping*)(launchTargetTask->mGroup).Get();
            Boolean occludesLaunchTarget = tg->IsTaskAboveTask(
                t->GetTask(), launchTargetTask);
            t->StartLaunchTaskAnimation(NULL, FALSE, occludesLaunchTarget, lockToTask);
        }
    }
}

Boolean TaskStackView::IsTransformedTouchPointInView(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IView* child)
{
    return FrameLayout::IsTransformedTouchPointInView(x, y, child, NULL);
}

/** Pokes the dozer on user interaction. */
void TaskStackView::OnUserInteraction()
{
    // Poke the doze trigger if it is dozing
    mUIDozeTrigger->Poke();
}

/**** TaskStackCallbacks Implementation ****/

ECode TaskStackView::OnStackTaskAdded(
    /* [in] */ ITaskStack* stack,
    /* [in] */ ITask* t)
{
    RequestSynchronizeStackViewsWithModel();
    return NOERROR;
}

ECode TaskStackView::OnStackTaskRemoved(
    /* [in] */ ITaskStack* _stack,
    /* [in] */ ITask* removedTask,
    /* [in] */ ITask* newFrontMostTask)
{
    TaskStack* stack = (TaskStack*)_stack;
    // Remove the view associated with this task, we can't rely on updateTransforms
    // to work here because the task is no longer in the list
    AutoPtr<TaskView> tv = GetChildViewForTask((Task*)removedTask);
    if (tv != NULL) {
        mViewPool->ReturnViewToPool((ITaskView*)tv);
    }

    // Notify the callback that we've removed the task and it can clean up after it
    mCb->OnTaskViewDismissed(removedTask);

    // Get the stack scroll of the task to anchor to (since we are removing something, the front
    // most task will be our anchor task)
    AutoPtr<Task> anchorTask;
    Float prevAnchorTaskScroll = 0.0f;
    Boolean pullStackForward = stack->GetTaskCount() > 0;
    if (pullStackForward) {
        anchorTask = mStack->GetFrontMostTask();
        prevAnchorTaskScroll = mLayoutAlgorithm->GetStackScrollForTask(anchorTask);
    }

    // Update the min/max scroll and animate other task views into their new positions
    UpdateMinMaxScroll(TRUE, mConfig->mLaunchedWithAltTab, mConfig->mLaunchedFromHome);

    // Offset the stack by as much as the anchor task would otherwise move back
    if (pullStackForward) {
        Float anchorTaskScroll = mLayoutAlgorithm->GetStackScrollForTask(anchorTask);
        mStackScroller->SetStackScroll(mStackScroller->GetStackScroll()
            + (anchorTaskScroll - prevAnchorTaskScroll));
        mStackScroller->BoundScroll();
    }

    // Animate all the tasks into place
    RequestSynchronizeStackViewsWithModel(200);

    // Update the new front most task
    if (newFrontMostTask != NULL) {
        AutoPtr<TaskView> frontTv = GetChildViewForTask((Task*)newFrontMostTask);
        if (frontTv != NULL) {
            frontTv->OnTaskBound(newFrontMostTask);
        }
    }

    // If there are no remaining tasks, then either unfilter the current stack, or just close
    // the activity if there are no filtered stacks
    if (mStack->GetTaskCount() == 0) {
        Boolean shouldFinishActivity = TRUE;
        if (mStack->HasFilteredTasks()) {
            mStack->UnfilterTasks();
            shouldFinishActivity = (mStack->GetTaskCount() == 0);
        }
        if (shouldFinishActivity) {
            mCb->OnAllTaskViewsDismissed();
        }
    }
    return NOERROR;
}

ECode TaskStackView::OnStackFiltered(
    /* [in] */ ITaskStack* newStack,
    /* [in] */ IArrayList* curTasks,
    /* [in] */ ITask* filteredTask)
{
    /*
    // Stash the scroll and filtered task for us to restore to when we unfilter
    mStashedScroll = getStackScroll();

    // Calculate the current task transforms
    ArrayList<TaskViewTransform> curTaskTransforms =
            getStackTransforms(curTasks, getStackScroll(), NULL, TRUE);

    // Update the task offsets
    mLayoutAlgorithm->UpdateTaskOffsets(mStack->GetTasks());
    // Scroll the item to the top of the stack (sans-peek) rect so that we can see it better
    UpdateMinMaxScroll(FALSE);
    Float overlapHeight = mLayoutAlgorithm->GetTaskOverlapHeight(); setStackScrollRaw((Int32) (newStack.indexOfTask(filteredTask) * overlapHeight));
    boundScrollRaw();

    // Compute the transforms of the items in the new stack after setting the new scroll
    final ArrayList<Task> tasks = mStack->GetTasks();
    final ArrayList<TaskViewTransform> taskTransforms =
            getStackTransforms(mStack->GetTasks(), getStackScroll(), NULL, TRUE);

    // Animate
    mFilterAlgorithm.startFilteringAnimation(curTasks, curTaskTransforms, tasks, taskTransforms);

    // Notify any callbacks
    mCb->OnTaskStackFilterTriggered();
    */
    return NOERROR;
}

ECode TaskStackView::OnStackUnfiltered(
    /* [in] */ ITaskStack* newStack,
    /* [in] */ IArrayList* curTasks)
{
    /*
    // Calculate the current task transforms
    final ArrayList<TaskViewTransform> curTaskTransforms =
            getStackTransforms(curTasks, getStackScroll(), NULL, TRUE);

    // Update the task offsets
    mLayoutAlgorithm->UpdateTaskOffsets(mStack->GetTasks());
    // Restore the stashed scroll
    UpdateMinMaxScroll(FALSE);
    setStackScrollRaw(mStashedScroll);
    boundScrollRaw();

    // Compute the transforms of the items in the new stack after restoring the stashed scroll
    final ArrayList<Task> tasks = mStack->GetTasks();
    final ArrayList<TaskViewTransform> taskTransforms =
            getStackTransforms(tasks, getStackScroll(), NULL, TRUE);

    // Animate
    mFilterAlgorithm.startFilteringAnimation(curTasks, curTaskTransforms, tasks, taskTransforms);

    // Clear the saved vars
    mStashedScroll = 0;

    // Notify any callbacks
    mCb->OnTaskStackUnfilterTriggered();
    */
    return NOERROR;
}

/**** ViewPoolConsumer Implementation ****/

ECode TaskStackView::CreateView(
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::recents_task_view, this, FALSE, (IView**)&view);
    *result = view.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TaskStackView::PrepareViewToEnterPool(
    /* [in] */ ITaskView* _tv)
{
    TaskView* tv = (TaskView*)_tv;
    AutoPtr<Task> task = tv->GetTask();

    // Clear the accessibility focus for that view
    Boolean res;
    tv->IsAccessibilityFocused(&res);
    if (res) {
        tv->ClearAccessibilityFocus();
    }

    // Report that this tasks's data is no longer being used
    RecentsTaskLoader::GetInstance()->UnloadTaskData(task);

    // Detach the view from the hierarchy
    DetachViewFromParent(tv);

    // Reset the view properties
    tv->ResetViewProperties();
    return NOERROR;
}

ECode TaskStackView::PrepareViewToLeavePool(
    /* [in] */ ITaskView* _tv,
    /* [in] */ ITask* _task,
    /* [in] */ Boolean isNewView)
{
    TaskView* tv = (TaskView*)_tv;
    Task* task = (Task*)_task;
    // Rebind the task and request that this task's data be filled into the TaskView
    tv->OnTaskBound(task);

    // Mark the launch task as fullscreen
    if (Constants::DebugFlags::App::EnableScreenshotAppTransition && mAwaitingFirstLayout) {
        if (task->mIsLaunchTarget) {
            tv->SetIsFullScreen(TRUE);
        }
    }

    // Load the task data
    RecentsTaskLoader::GetInstance()->LoadTaskData(task);

    // Sanity check, the task view should always be clipping against the stack at this point,
    // but just in case, re-enable it here
    tv->SetClipViewInStack(TRUE);

    // If the doze trigger has already fired, then update the state for this task view
    if (mUIDozeTrigger->HasTriggered()) {
        tv->SetNoUserInteractionState();
    }

    // If we've finished the start animation, then ensure we always enable the focus animations
    if (mStartEnterAnimationCompleted) {
        tv->EnableFocusAnimations();
    }

    // Find the index where this task should be placed in the stack
    Int32 insertIndex = -1;
    Int32 taskIndex = mStack->IndexOfTask(task);
    if (taskIndex != -1) {
        Int32 childCount;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<Task> tvTask = ((TaskView*)ITaskView::Probe(view))->GetTask();
            if (taskIndex < mStack->IndexOfTask(tvTask)) {
                insertIndex = i;
                break;
            }
        }
    }

    // Add/attach the view to the hierarchy
    if (isNewView) {
        AddView(tv, insertIndex);

        // Set the callbacks and listeners for this new view
        tv->SetTouchEnabled(TRUE);
        tv->SetCallbacks(mWrapper);
    }
    else {
        AutoPtr<IViewGroupLayoutParams> lp;
        tv->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        AttachViewToParent(tv, insertIndex, lp);
    }
    return NOERROR;
}

ECode TaskStackView::HasPreferredData(
    /* [in] */ ITaskView* tv,
    /* [in] */ ITask* preferredData,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((ITask*)((TaskView*)tv)->GetTask() == preferredData);
    return NOERROR;
}

/**** TaskViewCallbacks Implementation ****/

ECode TaskStackView::OnTaskViewAppIconClicked(
    /* [in] */ ITaskView* tv)
{
    if (Constants::DebugFlags::App::EnableTaskFiltering) {
        if (mStack->HasFilteredTasks()) {
            mStack->UnfilterTasks();
        }
        else {
            mStack->FilterTasks(((TaskView*)tv)->GetTask());
        }
    }
    return NOERROR;
}

ECode TaskStackView::OnTaskViewAppInfoClicked(
    /* [in] */ ITaskView* tv)
{
    if (mCb != NULL) {
        mCb->OnTaskViewAppInfoClicked(((TaskView*)tv)->GetTask());
    }
    return NOERROR;
}

ECode TaskStackView::OnTaskViewClicked(
    /* [in] */ ITaskView* tv,
    /* [in] */ ITask* task,
    /* [in] */ Boolean lockToTask)
{
    // Cancel any doze triggers
    mUIDozeTrigger->StopDozing();

    if (mCb != NULL) {
        mCb->OnTaskViewClicked(this, tv, mStack, task, lockToTask);
    }
    return NOERROR;
}

ECode TaskStackView::OnTaskViewDismissed(
    /* [in] */ ITaskView* _tv)
{
    TaskView* tv = (TaskView*)_tv;
    AutoPtr<Task> task = tv->GetTask();
    Int32 taskIndex = mStack->IndexOfTask(task);
    Boolean taskWasFocused = tv->IsFocusedTask();

    // Announce for accessibility
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(task->mActivityLabel));
    String str;
    context->GetString(R::string::accessibility_recents_item_dismissed, args, &str);
    tv->AnnounceForAccessibility(CoreUtils::Convert(str));
    // Remove the task from the view
    mStack->RemoveTask(task);
    // If the dismissed task was focused, then we should focus the next task in front
    if (taskWasFocused) {
        AutoPtr<IArrayList> tasks = mStack->GetTasks();
        Int32 size;
        tasks->GetSize(&size);
        Int32 nextTaskIndex = Elastos::Core::Math::Min(size - 1, taskIndex);
        if (nextTaskIndex >= 0) {
            AutoPtr<IInterface> item;
            tasks->Get(nextTaskIndex, (IInterface**)&item);
            Task* nextTask = (Task*)ITask::Probe(item);
            AutoPtr<TaskView> nextTv = GetChildViewForTask(nextTask);
            nextTv->SetFocusedTask();
        }
    }
    return NOERROR;
}

ECode TaskStackView::OnTaskViewClipStateChanged(
    /* [in] */ ITaskView* tv)
{
    if (!mStackViewsDirty) {
        Invalidate();
    }
    return NOERROR;
}

ECode TaskStackView::OnTaskViewFullScreenTransitionCompleted()
{
    RequestSynchronizeStackViewsWithModel();
    return NOERROR;
}

ECode TaskStackView::OnTaskViewFocusChanged(
    /* [in] */ ITaskView* tv,
    /* [in] */ Boolean focused)
{
    if (focused) {
        mFocusedTaskIndex = mStack->IndexOfTask(((TaskView*)tv)->GetTask());
    }
    return NOERROR;
}

/**** TaskStackViewScroller.TaskStackViewScrollerCallbacks ****/

ECode TaskStackView::OnScrollChanged(
    /* [in] */ Float p)
{
    mUIDozeTrigger->Poke();
    RequestSynchronizeStackViewsWithModel();
    PostInvalidateOnAnimation();
    return NOERROR;
}

/**** RecentsPackageMonitor.PackageCallbacks Implementation ****/

ECode TaskStackView::OnComponentRemoved(
    /* [in] */ IHashSet* cns)
{
    // For other tasks, just remove them directly if they no longer exist
    AutoPtr<IArrayList> tasks = mStack->GetTasks();
    Int32 size;
    tasks->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> item;
        tasks->Get(i, (IInterface**)&item);
        Task* t = (Task*)ITask::Probe(item);
        AutoPtr<IComponentName> comp;
        t->mKey->mBaseIntent->GetComponent((IComponentName**)&comp);
        Boolean contains;
        cns->Contains(comp, &contains);
        if (contains) {
            AutoPtr<TaskView> tv = GetChildViewForTask(t);
            if (tv != NULL) {
                // For visible children, defer removing the task until after the animation
                AutoPtr<Runnable> runnable = new DeleteTaskRunnable(this, t);
                tv->StartDeleteTaskAnimation(runnable);
            }
            else {
                // Otherwise, remove the task from the stack immediately
                mStack->RemoveTask(t);
            }
        }
    }
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
