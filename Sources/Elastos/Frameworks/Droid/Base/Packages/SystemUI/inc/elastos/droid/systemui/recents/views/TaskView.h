
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEW_H__

#include "elastos/droid/systemui/recents/views/AnimateableViewBounds.h"
#include "elastos/droid/systemui/recents/views/TaskViewFooter.h"
#include "elastos/droid/systemui/recents/views/TaskViewHeader.h"
#include "elastos/droid/systemui/recents/views/TaskViewThumbnail.h"
#include "elastos/droid/systemui/recents/views/ViewAnimation.h"
#include "elastos/droid/systemui/recents/model/Task.h"

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::SystemUI::Recents::Model::ITaskCallbacks;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* A task view */
class TaskView
    : public FrameLayout
    , public ITaskView
{
private:
    class Wrapper
        : public Object
        , public ITaskCallbacks
        , public ITaskFooterViewCallbacks
        , public IViewOnClickListener
        , public IViewOnLongClickListener
    {
    public:
        Wrapper(
            /* [in] */ TaskView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnTaskDataLoaded();

        CARAPI OnTaskDataUnloaded();

        /**** TaskViewFooter.TaskFooterViewCallbacks ****/

        CARAPI OnTaskFooterHeightChanged(
            /* [in] */ Int32 height,
            /* [in] */ Int32 maxHeight);

        /**** View.OnClickListener Implementation ****/

        CARAPI OnClick(
            /* [in] */ IView* v);

        /**** View.OnLongClickListener Implementation ****/

        // @Override
        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        TaskView* mHost;
    };

    class TaskProgressAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        TaskProgressAnimatorUpdateListener(
            /* [in] */ TaskView* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskView* mHost;
    };

    class ClipTopAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        ClipTopAnimatorUpdateListener(
            /* [in] */ TaskView* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskView* mHost;
    };

    class MyViewOutlineProvider : public ViewOutlineProvider
    {
    public:
        MyViewOutlineProvider(
            /* [in] */ TaskView* host);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        TaskView* mHost;
    };

    class EndLaunchedFromAppWithScreenshotRunnable : public Runnable
    {
    public:
        EndLaunchedFromAppWithScreenshotRunnable(
            /* [in] */ TaskView* host,
            /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
        AutoPtr<ViewAnimation::TaskViewEnterContext> mCtx;
    };

    class AnimateDimRunnable : public Runnable
    {
    public:
        AnimateDimRunnable(
            /* [in] */ TaskView* host,
            /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
        AutoPtr<ViewAnimation::TaskViewEnterContext> mCtx;
    };

    class EndLaunchedFromAppWithThumbnailRunnable : public Runnable
    {
    public:
        EndLaunchedFromAppWithThumbnailRunnable(
            /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<ViewAnimation::TaskViewEnterContext> mCtx;
    };

    class EnableFocusAnimationsDimRunnable : public Runnable
    {
    public:
        EnableFocusAnimationsDimRunnable(
            /* [in] */ TaskView* host);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
    };

    class EndActionRunnable3 : public Runnable
    {
    public:
        EndActionRunnable3(
            /* [in] */ TaskView* host,
            /* [in] */ IRunnable* r);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
        AutoPtr<IRunnable> mR;
    };

    class DismissTaskRunnable : public Runnable
    {
    public:
        DismissTaskRunnable(
            /* [in] */ TaskView* host);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
    };

    class OnClickRunnable : public Runnable
    {
    public:
        OnClickRunnable(
            /* [in] */ TaskView* host,
            /* [in] */ IView* v);

        // @Override
        CARAPI Run();

    private:
        TaskView* mHost;
        AutoPtr<IView> mV;
    };

public:
    TaskView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /** Set callback */
    CARAPI_(void) SetCallbacks(
        /* [in] */ ITaskViewCallbacks* cb);

    /** Gets the task */
    CARAPI_(AutoPtr<Task>) GetTask();

    /** Returns the view bounds. */
    CARAPI_(AutoPtr<AnimateableViewBounds>) GetViewBounds();

    /** Synchronizes this view's properties with the task's transform */
    CARAPI_(void) UpdateViewPropertiesToTaskTransform(
        /* [in] */ TaskViewTransform* toTransform,
        /* [in] */ Int32 duration);

    CARAPI_(void) UpdateViewPropertiesToTaskTransform(
        /* [in] */ TaskViewTransform* toTransform,
        /* [in] */ Int32 duratio,
        /* [in] */ IAnimatorUpdateListener* updateCallback);

    /** Resets this view's properties */
    CARAPI_(void) ResetViewProperties();

    /**
     * When we are un/filtering, this method will set up the transform that we are animating to,
     * in order to hide the task.
     */
    CARAPI_(void) PrepareTaskTransformForFilterTaskHidden(
        /* [in] */ TaskViewTransform* toTransform);

    /**
     * When we are un/filtering, this method will setup the transform that we are animating from,
     * in order to show the task.
     */
    CARAPI_(void) PrepareTaskTransformForFilterTaskVisible(
        /* [in] */ TaskViewTransform* fromTransform);

    /** Prepares this task view for the enter-recents animations.  This is called earlier in the
     * first layout because the actual animation into recents may take a long time. */
    CARAPI_(void) PrepareEnterRecentsAnimation(
        /* [in] */ Boolean isTaskViewLaunchTargetTask,
        /* [in] */ Boolean occludesLaunchTarget,
        /* [in] */ Int32 offscreenY);

    /** Animates this task view as it enters recents */
    CARAPI_(void) StartEnterRecentsAnimation(
        /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

    /** Animates this task view as it leaves recents by pressing home. */
    CARAPI_(void) StartExitToHomeAnimation(
        /* [in] */ ViewAnimation::TaskViewExitContext* ctx);

    /** Animates this task view as it exits recents */
    CARAPI_(void) StartLaunchTaskAnimation(
        /* [in] */ IRunnable* postAnimRunnable,
        /* [in] */ Boolean isLaunchingTas,
        /* [in] */ Boolean occludesLaunchTarget,
        /* [in] */ Boolean lockToTask);

    /** Animates the deletion of this task view */
    CARAPI_(void) StartDeleteTaskAnimation(
        /* [in] */ IRunnable* r);

    /** Animates this task view if the user does not interact with the stack after a certain time. */
    CARAPI_(void) StartNoUserInteractionAnimation();

    /** Mark this task view that the user does has not interacted with the stack after a certain time. */
    CARAPI_(void) SetNoUserInteractionState();

    /** Dismisses this task. */
    CARAPI_(void) DismissTask();

    /** Sets whether this task view is full screen or not. */
    CARAPI_(void) SetIsFullScreen(
        /* [in] */ Boolean isFullscreen);

    /** Returns whether this task view should currently be drawn as a full screen view. */
    CARAPI_(Boolean) IsFullScreenView();

    /**
     * Returns whether this view should be clipped, or any views below should clip against this
     * view.
     */
    CARAPI_(Boolean) ShouldClipViewInStack();

    /** Sets whether this view should be clipped, or clipped against. */
    CARAPI_(void) SetClipViewInStack(
        /* [in] */ Boolean clip);

    /** Gets the max footer height. */
    CARAPI_(Int32) GetMaxFooterHeight();

    /** Animates the footer into and out of view. */
    CARAPI_(void) AnimateFooterVisibility(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 duration);

    /** Sets the current task progress. */
    CARAPI_(void) SetTaskProgress(
        /* [in] */ Float p);

    /** Returns the current task progress. */
    CARAPI_(Float) GetTaskProgress();

    /** Returns the current dim. */
    CARAPI_(void) SetDim(
        /* [in] */ Int32 dim);

    /** Returns the current dim. */
    CARAPI_(Int32) GetDim();

    /** Animates the dim to the task progress. */
    CARAPI_(void) AnimateDimToProgress(
        /* [in] */ Int32 delay,
        /* [in] */ Int32 duration,
        /* [in] */ IAnimatorListener* postAnimRunnable);

    /** Compute the dim as a function of the scale of this view. */
    CARAPI_(Int32) GetDimFromTaskProgress();

    /** Update the dim as a function of the scale of this view. */
    CARAPI_(void) UpdateDimFromTaskProgress();

    /**** View focus state ****/

    /**
     * Sets the focused task explicitly. We need a separate flag because requestFocus() won't happen
     * if the view is not currently visible, or we are in touch state (where we still want to keep
     * track of focus).
     */
    CARAPI_(void) SetFocusedTask();

    /**
     * Unsets the focused task explicitly.
     */
    CARAPI_(void) UnsetFocusedTask();

    /**
     * Returns whether we have explicitly been focused.
     */
    CARAPI_(Boolean) IsFocusedTask();

    /** Enables all focus animations. */
    CARAPI_(void) EnableFocusAnimations();

    /**** TaskCallbacks Implementation ****/

    /** Binds this task view to the task */
    CARAPI_(void) OnTaskBound(
        /* [in] */ ITask* t);

    CARAPI OnTaskDataLoaded();

    CARAPI OnTaskDataUnloaded();

    /** Enables/disables handling touch on this task view. */
    CARAPI_(void) SetTouchEnabled(
        /* [in] */ Boolean enabled);

    /**** TaskViewFooter.TaskFooterViewCallbacks ****/

    CARAPI OnTaskFooterHeightChanged(
        /* [in] */ Int32 height,
        /* [in] */ Int32 maxHeight);

    /**** View.OnClickListener Implementation ****/

    CARAPI OnClick(
        /* [in] */ IView* v);

    /**** View.OnLongClickListener Implementation ****/

    // @Override
    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * Updates the explicitly focused state when the view focus changes.
     */
    // @Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

public:
    AutoPtr<RecentsConfiguration> mConfig;

    Float mTaskProgress;
    AutoPtr<IAnimator> mTaskProgressAnimator; //IObjectAnimator
    AutoPtr<IAnimator> mDimAnimator; //IObjectAnimator
    Float mMaxDimScale;
    Int32 mDim;
    AutoPtr<IAccelerateInterpolator> mDimInterpolator;
    AutoPtr<IPorterDuffColorFilter> mDimColorFilter;

    AutoPtr<Task> mTask;
    Boolean mTaskDataLoaded;
    Boolean mIsFocused;
    Boolean mFocusAnimationsEnabled;
    Boolean mIsFullScreenView;
    Boolean mClipViewInStack;
    AutoPtr<AnimateableViewBounds> mViewBounds;
    AutoPtr<IPaint> mLayerPaint;

    AutoPtr<IView> mContent;
    AutoPtr<TaskViewThumbnail> mThumbnailView;
    AutoPtr<TaskViewHeader> mHeaderView;
    AutoPtr<TaskViewFooter> mFooterView;
    AutoPtr<IView> mActionButtonView;
    AutoPtr<ITaskViewCallbacks> mCb;

    // Optimizations
    AutoPtr<IAnimatorUpdateListener> mUpdateDimListener;

private:
    AutoPtr<Wrapper> mWrapper;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEW_H__
