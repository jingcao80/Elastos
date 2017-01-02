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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/views/DebugOverlayView.h"
#include "elastos/droid/systemui/recents/views/ViewAnimation.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsOnAnimationStartedListener;
using Elastos::Core::Runnable;
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
    , public ITaskStackViewCallbacks
    , public IPackageCallbacks
{
private:
    class OnAnimationStartedRunnable : public Runnable
    {
    public:
        OnAnimationStartedRunnable(
            /* [in] */ SystemServicesProxy* ssp);

        CARAPI Run();

    private:
        AutoPtr<SystemServicesProxy> mSsp;
    };

    class OnAnimationStartedListener
        : public Object
        , public IActivityOptionsOnAnimationStartedListener
    {
    public:
        OnAnimationStartedListener(
            /* [in] */ SystemServicesProxy* ssp,
            /* [in] */ RecentsView* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationStarted();

    private:
        RecentsView* mHost;
        Boolean mTriggered;
        AutoPtr<SystemServicesProxy> mSsp;
    };

    class LaunchRunnable : public Runnable
    {
    public:
        LaunchRunnable(
            /* [in] */ ITask* task,
            /* [in] */ IActivityOptions* launchOpts,
            /* [in] */ Boolean lockToTask,
            /* [in] */ RecentsView* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<ITask> mTask;
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
        /* [in] */ DebugOverlayView* overlay);

    /** Set/get the bsp root node */
    CARAPI SetTaskStacks(
        /* [in] */ IList* stacks);

    /** Removes all the task stack views from this recents view. */
    CARAPI RemoveAllTaskStacks();

    /** Launches the focused task from the first stack if possible */
    CARAPI_(Boolean) LaunchFocusedTask();

    /** Launches the task that Recents was launched from, if possible */
    CARAPI_(Boolean) LaunchPreviousTask();

    /** Requests all task stacks to start their enter-recents animation */
    CARAPI StartEnterRecentsAnimation(
        /* [in] */ ViewAnimation::TaskViewEnterContext* ctx);

    /** Requests all task stacks to start their exit-recents animation */
    CARAPI StartExitToHomeAnimation(
        /* [in] */ ViewAnimation::TaskViewExitContext* ctx);

    /** Adds the search bar */
    CARAPI SetSearchBar(
        /* [in] */ IView* searchBar);

    /** Returns whether there is currently a search bar */
    CARAPI_(Boolean) HasSearchBar();

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
    CARAPI_(Boolean) UnfilterFilteredStacks();

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
    CARAPI OnMeasure(
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
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_RECENTSVIEW_H__
