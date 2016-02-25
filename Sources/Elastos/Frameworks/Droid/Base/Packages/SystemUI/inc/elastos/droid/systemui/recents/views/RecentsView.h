
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::SystemUI::Recents::Model::IPackageCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::ITask;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStack;
using Elastos::Utility::IList;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * This view is the the top level layout that contains TaskStacks (which are laid out according
 * to their SpaceNode bounds.
 */
class RecentsView
    : public FrameLayout
    , public IRecentsView
{
private:
    class CallBacks
        : public Object
        , public ITaskStackViewCallbacks,
        , public IPackageCallbacks
    {
    public:
        CallBacks(
            /* [in] */ RecentsView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnTaskViewClicked(
            /* [in] */ ITaskStackView* stackView,
            /* [in] */ ITaskView* tv,
            /* [in] */ ITaskStack* stack,
            /* [in] */ ITask* t,
            /* [in] */ Boolean lockToTask);

        CARAPI OnTaskViewAppInfoClicked(
            /* [in] */ ITask* t);

        CARAPI OnTaskViewDismissed(
            /* [in] */ ITask* t);

        CARAPI OnAllTaskViewsDismissed();

        CARAPI OnTaskStackFilterTriggered();

        CARAPI OnTaskStackUnfilterTriggered();

        CARAPI OnComponentRemoved(
            /* [in] */ IHashSet* cns);

    private:
        RecentsView* mHost;
    };

    class OnAnimationStartedRunnable : public Runnable
    {
    public:
        CARAPI Run();
    };

    class OnAnimationStartedListener
        : public Object
        , public IActivityOptionsOnAnimationStartedListener
    {
    public:
        OnAnimationStartedListener(
            /* [in] */ RecentsView* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationStarted();

    private:
        RemoveView* mHost;
        Boolean mTriggered;
    };

    class LaunchRunnable : public Runnable
    {
    public:
        LaunchRunnable(
            /* [in] */ Task* task,
            /* [in] */ IActivityOptions* launchOpts,
            /* [in] */ Boolean lockToTask,
            /* [in] */ RecentsView* mHost);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<Task> mTask;
        AutoPtr<IActivityOptions> mLaunchOpts;
        Boolean mLockToTask;
        RecentsView* mHost;
    };

public:
    RecentsView();

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

    /** Sets the callbacks */
    CARAPI SetCallbacks(
        /* [in] */ IRecentsViewCallbacks* cb);

    /** Sets the debug overlay */
    CARAPI SetDebugOverlay(
        /* [in] */ IDebugOverlayView* overlay);

    /** Set/get the bsp root node */
    CARAPI SetTaskStacks(
        /* [in] */ IList* stacks);

    /** Removes all the task stack views from this recents view. */
    CARAPI RemoveAllTaskStacks();

    /** Launches the focused task from the first stack if possible */
    CARAPI LaunchFocusedTask(
        /* [out] */ Boolean* result);

    /** Launches the task that Recents was launched from, if possible */
    CARAPI LaunchPreviousTask(
        /* [out] */ Boolean* result);

    /** Requests all task stacks to start their enter-recents animation */
    CARAPI StartEnterRecentsAnimation(
        /* [in] */ ITaskViewEnterContext* ctx);

    /** Requests all task stacks to start their exit-recents animation */
    CARAPI StartExitToHomeAnimation(
        /* [in] */ ITaskViewExitContext* ctx);

    /** Adds the search bar */
    CARAPI SetSearchBar(
        /* [in] */ IView* searchBar);

    /** Returns whether there is currently a search bar */
    CARAPI HasSearchBar(
        /* [out] */ Boolean* result);

    /** Sets the visibility of the search bar */
    CARAPI SetSearchBarVisibility(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** outInsets);

    /** Notifies each task view of the user interaction. */
    CARAPI OnUserInteraction();

    /** Focuses the next task in the first stack view */
    CARAPI FocusNextTask(
        /* [in] */ Boolean forward);

    /** Dismisses the focused task. */
    CARAPI DismissFocusedTask();

    /** Unfilters any filtered stacks */
    CARAPI UnfilterFilteredStacks(
        /* [out] */ Boolean* result);

    /**** TaskStackView.TaskStackCallbacks Implementation ****/

    // @Override
    CARAPI OnTaskViewClicked(
        /* [in] */ ITaskStackView* _stackView,
        /* [in] */ ITaskView* _tv,
        /* [in] */ ITaskStack* stack,
        /* [in] */ ITask* _task,
        /* [in] */ Boolean lockToTask);

    // @Override
    CARAPI OnTaskViewAppInfoClicked(
        /* [in] */ ITask* _t);

    // @Override
    CARAPI OnTaskViewDismissed(
        /* [in] */ ITask* _t);

    // @Override
    CARAPI OnAllTaskViewsDismissed();

    // @Override
    CARAPI OnTaskStackFilterTriggered();

    // @Override
    CARAPI OnTaskStackUnfilterTriggered();

    /**** RecentsPackageMonitor.PackageCallbacks Implementation ****/

    // @Override
    CARAPI OnComponentRemoved(
        /* [in] */ IHashSet* cns);

protected:
    /**
     * This is called with the full size of the window since we are handling our own insets.
     */
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * This is called with the full size of the window since we are handling our own insets.
     */
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

public:
    AutoPtr<RecentsConfiguration> mConfig;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<DebugOverlayView> mDebugOverlay;

    AutoPtr<IList> mStacks; // item is ITaskStack
    AutoPtr<IView> mSearchBar;
    AutoPtr<IRecentsViewCallbacks> mCb;
    Boolean mAlreadyLaunchingTask;

private:
    AutoPtr<CallBacks> mCallBacks;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__
