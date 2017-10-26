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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEW_H__

#include "elastos/droid/systemui/recents/model/TaskStack.h"
#include "elastos/droid/systemui/recents/views/DebugOverlayView.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewFilterAlgorithm.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewLayoutAlgorithm.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewScroller.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewTouchHandler.h"
#include "elastos/droid/systemui/recents/views/TaskView.h"
#include "elastos/droid/systemui/recents/views/ViewPool.h"
#include "elastos/droid/systemui/recents/misc/DozeTrigger.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Utility::IHashSet;
using Elastos::Droid::SystemUI::Recents::Misc::DozeTrigger;
using Elastos::Droid::SystemUI::Recents::Model::IPackageCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStack;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStackCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::TaskStack;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* The visual representation of a task stack view */
class TaskStackView
    : public FrameLayout
    , public ITaskStackView
{
private:
    class Wrapper
        : public Object
        , public ITaskStackCallbacks
        , public ITaskViewCallbacks
        , public ITaskStackViewScrollerCallbacks
        , public IViewPoolConsumer
        , public IPackageCallbacks
    {
    public:
        Wrapper(
            /* [in] */ TaskStackView* host);

        CAR_INTERFACE_DECL()

        /**** TaskStackCallbacks Implementation ****/

        // @Override
        CARAPI OnStackTaskAdded(
            /* [in] */ ITaskStack* stack,
            /* [in] */ ITask* t);

        // @Override
        CARAPI OnStackTaskRemoved(
            /* [in] */ ITaskStack* stack,
            /* [in] */ ITask* removedTask,
            /* [in] */ ITask* newFrontMostTask);

        // @Override
        CARAPI OnStackFiltered(
            /* [in] */ ITaskStack* newStack,
            /* [in] */ IArrayList* curTasks,
            /* [in] */ ITask* filteredTask);

        // @Override
        CARAPI OnStackUnfiltered(
            /* [in] */ ITaskStack* newStack,
            /* [in] */ IArrayList* curTasks);

        /**** ViewPoolConsumer Implementation ****/

        // @Override
        CARAPI CreateView(
            /* [in] */ IContext* context,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI PrepareViewToEnterPool(
            /* [in] */ IInterface* tv);

        // @Override
        CARAPI PrepareViewToLeavePool(
            /* [in] */ IInterface* tv,
            /* [in] */ IInterface* task,
            /* [in] */ Boolean isNewView);

        // @Override
        CARAPI HasPreferredData(
            /* [in] */ IInterface* tv,
            /* [in] */ IInterface* preferredData,
            /* [out] */ Boolean* result);

        /**** TaskViewCallbacks Implementation ****/

        // @Override
        CARAPI OnTaskViewAppIconClicked(
            /* [in] */ ITaskView* tv);

        // @Override
        CARAPI OnTaskViewAppInfoClicked(
            /* [in] */ ITaskView* tv);

        // @Override
        CARAPI OnTaskViewClicked(
            /* [in] */ ITaskView* tv,
            /* [in] */ ITask* task,
            /* [in] */ Boolean lockToTask);

        // @Override
        CARAPI OnTaskViewDismissed(
            /* [in] */ ITaskView* tv);

        // @Override
        CARAPI OnTaskViewClipStateChanged(
            /* [in] */ ITaskView* tv);

        // @Override
        CARAPI OnTaskViewFullScreenTransitionCompleted();

        // @Override
        CARAPI OnTaskViewFocusChanged(
            /* [in] */ ITaskView* tv,
            /* [in] */ Boolean focused);

        /**** TaskStackViewScroller.TaskStackViewScrollerCallbacks ****/

        // @Override
        CARAPI OnScrollChanged(
            /* [in] */ Float p);

        /**** RecentsPackageMonitor.PackageCallbacks Implementation ****/

        // @Override
        CARAPI OnComponentRemoved(
            /* [in] */ IHashSet* cns);

    private:
        TaskStackView* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TO_STRING_IMPL("TaskStackView::AnimatorUpdateListener")

        AnimatorUpdateListener(
            /* [in] */ TaskStackView* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskStackView* mHost;
    };

    class ReturnAllViewsToPoolRunnable : public Runnable
    {
    public:
        TO_STRING_IMPL("TaskStackView::ReturnAllViewsToPoolRunnable")

        ReturnAllViewsToPoolRunnable(
            /* [in] */ TaskStackView* host);

        // @Override
        CARAPI Run();

        CARAPI SetTaskViewExitContext(
            /* [in] */ ViewAnimation::TaskViewExitContext* ctx);

    private:
        TaskStackView* mHost;
        AutoPtr<ViewAnimation::TaskViewExitContext> mTaskViewExitContext;
    };

    class SleepRunnable : public Runnable
    {
    public:
        TO_STRING_IMPL("TaskStackView::SleepRunnable")

        SleepRunnable(
            /* [in] */ TaskStackView* host);

        // @Override
        CARAPI Run();

    private:
        TaskStackView* mHost;
    };

    class PostScrollRunnable : public Runnable
    {
    public:
        TO_STRING_IMPL("TaskStackView::PostScrollRunnable")

        PostScrollRunnable(
            /* [in] */ TaskStackView* host);

        CARAPI Run();

    private:
        TaskStackView* mHost;
    };

    class LastDecrementRunnable : public Runnable
    {
    public:
        TO_STRING_IMPL("TaskStackView::LastDecrementRunnable")

        LastDecrementRunnable(
            /* [in] */ TaskStackView* host);

        CARAPI Run();

        CARAPI SetTaskViewEnterContext(
            /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

    private:
        TaskStackView* mHost;
        AutoPtr<ViewAnimation::TaskViewEnterContext> mTaskViewEnterContext;
    };

    class DeleteTaskRunnable : public Runnable
    {
    public:
        TO_STRING_IMPL("TaskStackView::DeleteTaskRunnable")

        DeleteTaskRunnable(
            /* [in] */ TaskStackView* host,
            /* [in] */ Task* t);

        CARAPI Run();

    private:
        TaskStackView* mHost;
        AutoPtr<Task> mT;
    };

public:
    CAR_INTERFACE_DECL()

    TaskStackView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITaskStack* stack);

    /** Sets the callbacks */
    CARAPI_(void) SetCallbacks(
        /* [in] */ ITaskStackViewCallbacks* cb);

    /** Sets the debug overlay */
    CARAPI_(void) SetDebugOverlay(
        /* [in] */ DebugOverlayView* overlay);

    /** Requests that the views be synchronized with the model */
    CARAPI_(void) RequestSynchronizeStackViewsWithModel();

    CARAPI_(void) RequestSynchronizeStackViewsWithModel(
        /* [in] */ Int32 duration);

    /** Requests that the views clipping be updated. */
    CARAPI_(void) RequestUpdateStackViewsClip();

    /** Finds the child view given a specific task. */
    CARAPI_(AutoPtr<TaskView>) GetChildViewForTask(
        /* [in] */ Task* t);

    /** Returns the stack algorithm for this task stack. */
    CARAPI_(AutoPtr<TaskStackViewLayoutAlgorithm>) GetStackAlgorithm();

    /** Synchronizes the views with the model */
    CARAPI_(Boolean) SynchronizeStackViewsWithModel();

    /** Updates the clip for each of the task views. */
    CARAPI_(void) ClipTaskViews();

    /** The stack insets to apply to the stack contents */
    CARAPI_(void) SetStackInsetRect(
        /* [in] */ IRect* r);

    /** Updates the min and max virtual scroll bounds */
    CARAPI_(void) UpdateMinMaxScroll(
        /* [in] */ Boolean boundScrollToNewMinMax,
        /* [in] */ Boolean launchedWithAltTab,
        /* [in] */ Boolean launchedFromHome);

    /** Returns the scroller. */
    CARAPI_(AutoPtr<TaskStackViewScroller>) GetScroller();

    /** Focuses the task at the specified index in the stack */
    CARAPI_(void) FocusTask(
        /* [in] */ Int32 taskIndex,
        /* [in] */ Boolean scrollToNewPosition);

    /** Focuses the next task in the stack */
    CARAPI_(void) FocusNextTask(
        /* [in] */ Boolean forward);

    /** Dismisses the focused task. */
    CARAPI_(void) DismissFocusedTask();

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ComputeScroll();

    /** Computes the stack and task rects */
    CARAPI_(void) ComputeRects(
        /* [in] */ Int32 windowWidth,
        /* [in] */ Int32 windowHeight,
        /* [in] */ IRect* taskStackBounds,
        /* [in] */ Boolean launchedWithAltTab,
        /* [in] */ Boolean launchedFromHome);

    /**
     * This is ONLY used from AlternateRecentsComponent to update the dummy stack view for purposes
     * of getting the task rect to animate to.
     */
    CARAPI_(void) UpdateMinMaxScrollForStack(
        /* [in] */ TaskStack* stack,
        /* [in] */ Boolean launchedWithAltTab,
        /* [in] */ Boolean launchedFromHome);

    /** Handler for the first layout. */
    CARAPI_(void) OnFirstLayout();

    /** Requests this task stacks to start it's enter-recents animation */
    CARAPI_(void) StartEnterRecentsAnimation(
        /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

    /** Requests this task stacks to start it's exit-recents animation. */
    CARAPI_(void) StartExitToHomeAnimation(
        /* [in] */ ViewAnimation::TaskViewExitContext* ctx);

    /** Animates a task view in this stack as it launches. */
    CARAPI_(void) StartLaunchTaskAnimation(
        /* [in] */ TaskView* tv,
        /* [in] */ IRunnable* r,
        /* [in] */ Boolean lockToTask);

    CARAPI_(Boolean) IsTransformedTouchPointInView(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IView* child);

    /** Pokes the dozer on user interaction. */
    CARAPI_(void) OnUserInteraction();

    /**** TaskStackCallbacks Implementation ****/

    // @Override
    CARAPI OnStackTaskAdded(
        /* [in] */ ITaskStack* stack,
        /* [in] */ ITask* t);

    // @Override
    CARAPI OnStackTaskRemoved(
        /* [in] */ ITaskStack* stack,
        /* [in] */ ITask* removedTask,
        /* [in] */ ITask* newFrontMostTask);

    // @Override
    CARAPI OnStackFiltered(
        /* [in] */ ITaskStack* newStack,
        /* [in] */ IArrayList* curTasks,
        /* [in] */ ITask* filteredTask);

    // @Override
    CARAPI OnStackUnfiltered(
        /* [in] */ ITaskStack* newStack,
        /* [in] */ IArrayList* curTasks);

    /**** ViewPoolConsumer Implementation ****/

    // @Override
    CARAPI CreateView(
        /* [in] */ IContext* context,
        /* [out] */ IInterface** result);

    // @Override
    CARAPI PrepareViewToEnterPool(
        /* [in] */ ITaskView* tv);

    // @Override
    CARAPI PrepareViewToLeavePool(
        /* [in] */ ITaskView* tv,
        /* [in] */ ITask* task,
        /* [in] */ Boolean isNewView);

    // @Override
    CARAPI HasPreferredData(
        /* [in] */ ITaskView* tv,
        /* [in] */ ITask* preferredData,
        /* [out] */ Boolean* result);

    /**** TaskViewCallbacks Implementation ****/

    // @Override
    CARAPI OnTaskViewAppIconClicked(
        /* [in] */ ITaskView* tv);

    // @Override
    CARAPI OnTaskViewAppInfoClicked(
        /* [in] */ ITaskView* tv);

    // @Override
    CARAPI OnTaskViewClicked(
        /* [in] */ ITaskView* tv,
        /* [in] */ ITask* task,
        /* [in] */ Boolean lockToTask);

    // @Override
    CARAPI OnTaskViewDismissed(
        /* [in] */ ITaskView* tv);

    // @Override
    CARAPI OnTaskViewClipStateChanged(
        /* [in] */ ITaskView* tv);

    // @Override
    CARAPI OnTaskViewFullScreenTransitionCompleted();

    // @Override
    CARAPI OnTaskViewFocusChanged(
        /* [in] */ ITaskView* tv,
        /* [in] */ Boolean focused);

    using FrameLayout::OnScrollChanged;

    /**** TaskStackViewScroller.TaskStackViewScrollerCallbacks ****/

    // @Override
    CARAPI OnScrollChanged(
        /* [in] */ Float p);

    /**** RecentsPackageMonitor.PackageCallbacks Implementation ****/

    // @Override
    CARAPI OnComponentRemoved(
        /* [in] */ IHashSet* cns);

protected:
    /**
     * This is called with the full window width and height to allow stack view children to
     * perform the full screen transition down.
     */
    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * This is called with the size of the space not including the top or right insets, or the
     * search bar height in portrait (but including the search bar width in landscape, since we want
     * to draw under it.
     */
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    /**
     * Gets the stack transforms of a list of tasks, and returns the visible range of tasks.
     */
    CARAPI_(Boolean) UpdateStackTransforms(
        /* [in] */ IArrayList* taskTransforms,
        /* [in] */ IArrayList* tasks,
        /* [in] */ Float stackScroll,
        /* [in] */ ArrayOf<Int32>* visibleRangeOut,
        /* [in] */ Boolean boundTranslationsToRect);

    /**
     * Gets the stack transforms of a list of tasks, and returns the visible range of tasks. This
     * call is less optimal than calling UpdateStackTransforms directly.
     */
    CARAPI_(AutoPtr<IArrayList>) GetStackTransforms(
        /* [in] */ IArrayList* tasks,
        /* [in] */ Float stackScroll,
        /* [in] */ ArrayOf<Int32>* visibleRangeOut,
        /* [in] */ Boolean boundTranslationsToRect);

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<TaskStack> mStack;
    AutoPtr<TaskStackViewLayoutAlgorithm> mLayoutAlgorithm;
    AutoPtr<TaskStackViewFilterAlgorithm> mFilterAlgorithm;
    AutoPtr<TaskStackViewScroller> mStackScroller;
    AutoPtr<TaskStackViewTouchHandler> mTouchHandler;
    AutoPtr<ITaskStackViewCallbacks> mCb;
    AutoPtr<ViewPool> mViewPool;
    AutoPtr<IArrayList> mCurrentTaskTransforms;// item is TaskViewTransform;
    AutoPtr<DozeTrigger> mUIDozeTrigger;
    AutoPtr<DebugOverlayView> mDebugOverlay;
    AutoPtr<IRect> mTaskStackBounds;
    Int32 mFocusedTaskIndex;
    Int32 mPrevAccessibilityFocusedIndex;

    // Optimizations
    Int32 mStackViewsAnimationDuration;
    Boolean mStackViewsDirty;
    Boolean mStackViewsClipDirty;
    Boolean mAwaitingFirstLayout;
    Boolean mStartEnterAnimationRequestedAfterLayout;
    Boolean mStartEnterAnimationCompleted;
    AutoPtr<ViewAnimation::TaskViewEnterContext> mStartEnterAnimationContext;
    AutoPtr<ArrayOf<Int32> > mTmpVisibleRange;
    AutoPtr<ArrayOf<Float> > mTmpCoord;
    AutoPtr<IMatrix> mTmpMatrix;
    AutoPtr<IRect> mTmpRect;
    AutoPtr<TaskViewTransform> mTmpTransform;
    AutoPtr<IHashMap> mTmpTaskViewMap; //<Task, TaskView>
    AutoPtr<ILayoutInflater> mInflater;

    // A convenience update listener to request updating clipping of tasks
    AutoPtr<IAnimatorUpdateListener> mRequestUpdateClippingListener;

    // A convenience runnable to return all views to the pool
    AutoPtr<ReturnAllViewsToPoolRunnable> mReturnAllViewsToPoolRunnable;

private:
    AutoPtr<Wrapper> mWrapper;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKSTACKVIEW_H__
