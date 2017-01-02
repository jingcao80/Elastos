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

#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "elastos/droid/systemui/recents/model/TaskGrouping.h"
#include "elastos/droid/systemui/recents/views/RecentsView.h"
#include "elastos/droid/systemui/recents/views/TaskStackView.h"
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/view/LayoutInflater.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::EIID_IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ALPHA_8;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::SystemUI::Recents::Model::EIID_IPackageCallbacks;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;
using Elastos::Droid::SystemUI::Recents::Model::TaskGrouping;
using Elastos::Droid::SystemUI::Recents::Misc::Utilities;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

//--------------------------------------------------------------
// RecentsView::OnAnimationStartedRunnable
//--------------------------------------------------------------

RecentsView::OnAnimationStartedRunnable::OnAnimationStartedRunnable(
    /* [in] */ SystemServicesProxy* ssp)
    : mSsp(ssp)
{}

ECode RecentsView::OnAnimationStartedRunnable::Run()
{
    mSsp->LockCurrentTask();
    return NOERROR;
}

//--------------------------------------------------------------
// RecentsView::OnAnimationStartedListener
//--------------------------------------------------------------

CAR_INTERFACE_IMPL(RecentsView::OnAnimationStartedListener, Object, IActivityOptionsOnAnimationStartedListener)

RecentsView::OnAnimationStartedListener::OnAnimationStartedListener(
    /* [in] */ SystemServicesProxy* ssp,
    /* [in] */ RecentsView* host)
    : mHost(host)
    , mTriggered(FALSE)
    , mSsp(ssp)
{}

// @Override
ECode RecentsView::OnAnimationStartedListener::OnAnimationStarted()
{
    if (!mTriggered) {
        AutoPtr<Runnable> runnable = new OnAnimationStartedRunnable(mSsp);
        Boolean res;
        mHost->PostDelayed(runnable, 350, &res);
        mTriggered = TRUE;
    }
    return NOERROR;
}

RecentsView::LaunchRunnable::LaunchRunnable(
    /* [in] */ ITask* task,
    /* [in] */ IActivityOptions* launchOpts,
    /* [in] */ Boolean lockToTask,
    /* [in] */ RecentsView* host)
    : mTask(task)
    , mLaunchOpts(launchOpts)
    , mLockToTask(lockToTask)
    , mHost(host)
{}

// @Override
ECode RecentsView::LaunchRunnable::Run()
{
    Task* task = (Task*)mTask.Get();
    AutoPtr<SystemServicesProxy> ssp;
    RecentsTaskLoader::GetInstance()->GetSystemServicesProxy((SystemServicesProxy**)&ssp);
    if (task->mIsActive) {
        // Bring an active task to the foreground
        ssp->MoveTaskToFront(task->mKey->mId, mLaunchOpts);
    }
    else {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        if (ssp->StartActivityFromRecents(context, task->mKey->mId,
                task->mActivityLabel, mLaunchOpts)) {
            if (mLaunchOpts == NULL && mLockToTask) {
                ssp->LockCurrentTask();
            }
        }
        else {
            // Dismiss the task and return the user to home if we fail to
            // launch the task
            mHost->OnTaskViewDismissed(task);
            if (mHost->mCb != NULL) {
                mHost->mCb->OnTaskLaunchFailed();
            }
        }
    }
    return NOERROR;
}

//--------------------------------------------------------------
// RecentsView
//--------------------------------------------------------------

CAR_INTERFACE_IMPL_3(RecentsView, FrameLayout, IRecentsView, ITaskStackViewCallbacks, IPackageCallbacks)

RecentsView::RecentsView()
    : mAlreadyLaunchingTask(FALSE)
{
}

ECode RecentsView::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

ECode RecentsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode RecentsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode RecentsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    mConfig = RecentsConfiguration::GetInstance();
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    return NOERROR;
}

/** Sets the callbacks */
ECode RecentsView::SetCallbacks(
    /* [in] */ IRecentsViewCallbacks* cb)
{
    mCb = cb;
    return NOERROR;
}

/** Sets the debug overlay */
ECode RecentsView::SetDebugOverlay(
    /* [in] */ DebugOverlayView* overlay)
{
    mDebugOverlay = overlay;
    return NOERROR;
}

/** Set/get the bsp root node */
ECode RecentsView::SetTaskStacks(
    /* [in] */ IList* stacks)
{
    // Remove all TaskStackViews (but leave the search bar)
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        if (v != mSearchBar) {
            RemoveViewAt(i);
        }
    }

    // Create and add all the stacks for this partition of space.
    mStacks = stacks;
    Int32 numStacks;
    mStacks->GetSize(&numStacks);
    for (Int32 i = 0; i < numStacks; i++) {
        AutoPtr<IInterface> item;
        mStacks->Get(i, (IInterface**)&item);
        AutoPtr<ITaskStack> stack = ITaskStack::Probe(item);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<TaskStackView> stackView = new TaskStackView();
        stackView->constructor(context, stack);
        stackView->SetCallbacks((ITaskStackViewCallbacks*)this);
        // Enable debug mode drawing
        if (mConfig->mDebugModeEnabled) {
            stackView->SetDebugOverlay(mDebugOverlay);
        }
        AddView(stackView);
    }

    // Reset the launched state
    mAlreadyLaunchingTask = FALSE;
    return NOERROR;
}

/** Removes all the task stack views from this recents view. */
ECode RecentsView::RemoveAllTaskStacks()
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        if (v != mSearchBar) {
            RemoveViewAt(i);
        }
    }
    return NOERROR;
}

/** Launches the focused task from the first stack if possible */
Boolean RecentsView::LaunchFocusedTask()
{
    // Get the first stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            AutoPtr<TaskStack> stack = stackView->mStack;
            // Iterate the stack views and try and find the focused task
            Int32 taskCount;
            stackView->GetChildCount(&taskCount);
            for (Int32 j = 0; j < taskCount; j++) {
                AutoPtr<IView> v;
                stackView->GetChildAt(j, (IView**)&v);
                AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
                AutoPtr<ITask> task = tv->GetTask();
                if (tv->IsFocusedTask()) {
                    OnTaskViewClicked(stackView, tv, stack, task, FALSE);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/** Launches the task that Recents was launched from, if possible */
Boolean RecentsView::LaunchPreviousTask()
{
    // Get the first stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            AutoPtr<TaskStack> stack = stackView->mStack;
            AutoPtr<IArrayList> tasks = stack->GetTasks();

            // Find the launch task in the stack
            Boolean isEmpty;
            tasks->IsEmpty(&isEmpty);
            if (!isEmpty) {
                Int32 taskCount;
                tasks->GetSize(&taskCount);
                for (Int32 j = 0; j < taskCount; j++) {
                    AutoPtr<IInterface> item;
                    tasks->Get(j, (IInterface**)&item);
                    AutoPtr<Task> task = (Task*)ITask::Probe(item);
                    if (task->mIsLaunchTarget) {
                        AutoPtr<ITaskView> tv = stackView->GetChildViewForTask(task);
                        OnTaskViewClicked(stackView, tv, stack, task, FALSE);
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

/** Requests all task stacks to start their enter-recents animation */
ECode RecentsView::StartEnterRecentsAnimation(
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->StartEnterRecentsAnimation(ctx);
        }
    }
    return NOERROR;
}

/** Requests all task stacks to start their exit-recents animation */
ECode RecentsView::StartExitToHomeAnimation(
    /* [in] */ ViewAnimation::TaskViewExitContext* ctx)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->StartExitToHomeAnimation(ctx);
        }
    }

    // Notify of the exit animation
    return mCb->OnExitToHomeAnimationTriggered();
}

/** Adds the search bar */
ECode RecentsView::SetSearchBar(
    /* [in] */ IView* searchBar)
{
    // Create the search bar (and hide it if we have no recent tasks)
    if (Constants::DebugFlags::App::EnableSearchLayout) {
        // Remove the previous search bar if one exists
        Int32 index;
        if (mSearchBar != NULL && (IndexOfChild(mSearchBar, &index), index > -1)) {
            RemoveView(mSearchBar);
        }
        // Add the new search bar
        if (searchBar != NULL) {
            mSearchBar = searchBar;
            AddView(mSearchBar);
        }
    }
    return NOERROR;
}

/** Returns whether there is currently a search bar */
Boolean RecentsView::HasSearchBar()
{
    return mSearchBar != NULL;
}

/** Sets the visibility of the search bar */
ECode RecentsView::SetSearchBarVisibility(
    /* [in] */ Int32 visibility)
{
    if (mSearchBar != NULL) {
        mSearchBar->SetVisibility(visibility);
        // Always bring the search bar to the top
        mSearchBar->BringToFront();
    }
    return NOERROR;
}

/**
 * This is called with the full size of the window since we are handling our own insets.
 */
// @Override
ECode RecentsView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = MeasureSpec::GetSize(heightMeasureSpec);

    // Get the search bar bounds and measure the search bar layout
    if (mSearchBar != NULL) {
        AutoPtr<IRect> searchBarSpaceBounds;
        CRect::New((IRect**)&searchBarSpaceBounds);
        Int32 top;
        mConfig->mSystemInsets->GetTop(&top);
        mConfig->GetSearchBarBounds(width, height, top, searchBarSpaceBounds);
        Int32 width, height;
        searchBarSpaceBounds->GetWidth(&width);
        searchBarSpaceBounds->GetHeight(&height);
        mSearchBar->Measure(MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY));
    }

    AutoPtr<IRect> taskStackBounds;
    CRect::New((IRect**)&taskStackBounds);
    Int32 top, right;
    mConfig->mSystemInsets->GetTop(&top);
    mConfig->mSystemInsets->GetRight(&right);
    mConfig->GetTaskStackBounds(width, height, top, right, taskStackBounds);

    // Measure each TaskStackView with the full width and height of the window since the
    // transition view is a child of that stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        if (child != mSearchBar && (child->GetVisibility(&visibility), visibility != GONE)) {
            AutoPtr<TaskStackView> tsv = (TaskStackView*)ITaskStackView::Probe(child);
            // Set the insets to be the top/left inset + search bounds
            tsv->SetStackInsetRect(taskStackBounds);
            tsv->Measure(widthMeasureSpec, heightMeasureSpec);
        }
    }

    SetMeasuredDimension(width, height);
    return NOERROR;
}

/**
 * This is called with the full size of the window since we are handling our own insets.
 */
// @Override
ECode RecentsView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    // Get the search bar bounds so that we lay it out
    if (mSearchBar != NULL) {
        AutoPtr<IRect> searchBarSpaceBounds;
        CRect::New((IRect**)&searchBarSpaceBounds);
        Int32 top;
        mConfig->mSystemInsets->GetTop(&top);
        Int32 width, height;
        GetMeasuredWidth(&width);
        GetMeasuredHeight(&height);
        mConfig->GetSearchBarBounds(width, height, top, searchBarSpaceBounds);
        Int32 left, right, bottom;
        searchBarSpaceBounds->GetLeft(&left);
        searchBarSpaceBounds->GetRight(&right);
        searchBarSpaceBounds->GetTop(&top);
        searchBarSpaceBounds->GetBottom(&bottom);
        mSearchBar->Layout(left, top, right, bottom);
    }

    // Layout each TaskStackView with the full width and height of the window since the
    // transition view is a child of that stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        if (child != mSearchBar && (child->GetVisibility(&visibility), visibility != GONE)) {
            Int32 width, height;
            child->GetMeasuredWidth(&width);
            child->GetMeasuredHeight(&height);
            child->Layout(left, top, left + width, top + height);
        }
    }
    return NOERROR;
}

// @Override
ECode RecentsView::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** outInsets)
{
    VALIDATE_NOT_NULL(outInsets);
    // Update the configuration with the latest system insets and trigger a relayout
    AutoPtr<IRect> rect;
    insets->GetSystemWindowInsets((IRect**)&rect);
    mConfig->UpdateSystemInsets(rect);
    RequestLayout();
    return insets->ConsumeSystemWindowInsets(outInsets);
}

/** Notifies each task view of the user interaction. */
ECode RecentsView::OnUserInteraction()
{
    // Get the first stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->OnUserInteraction();
        }
    }
    return NOERROR;
}

/** Focuses the next task in the first stack view */
ECode RecentsView::FocusNextTask(
    /* [in] */ Boolean forward)
{
    // Get the first stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->FocusNextTask(forward);
            break;
        }
    }
    return NOERROR;
}

/** Dismisses the focused task. */
ECode RecentsView::DismissFocusedTask()
{
    // Get the first stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->DismissFocusedTask();
            break;
        }
    }
    return NOERROR;
}

/** Unfilters any filtered stacks */
Boolean RecentsView::UnfilterFilteredStacks()
{
    if (mStacks != NULL) {
        // Check if there are any filtered stacks and unfilter them before we back out of Recents
        Boolean stacksUnfiltered = FALSE;
        Int32 numStacks;
        mStacks->GetSize(&numStacks);
        for (Int32 i = 0; i < numStacks; i++) {
            AutoPtr<IInterface> item;
            mStacks->Get(i, (IInterface**)&item);
            AutoPtr<TaskStack> stack = (TaskStack*)ITaskStack::Probe(item);
            if (stack->HasFilteredTasks()) {
                stack->UnfilterTasks();
                stacksUnfiltered = TRUE;
            }
        }
        return stacksUnfiltered;
    }
    return FALSE;
}

/**** TaskStackView.TaskStackCallbacks Implementation ****/

// @Override
ECode RecentsView::OnTaskViewClicked(
    /* [in] */ ITaskStackView* _stackView,
    /* [in] */ ITaskView* _tv,
    /* [in] */ ITaskStack* stack,
    /* [in] */ ITask* _task,
    /* [in] */ Boolean lockToTask)
{
    TaskStackView* stackView = (TaskStackView*)_stackView;
    TaskView* tv = (TaskView*)_tv;
    Task* task = (Task*)_task;
    // Notify any callbacks of the launching of a new task
    if (mCb != NULL) {
        mCb->OnTaskViewClicked();
    }
    // Skip if we are already launching tasks
    if (mAlreadyLaunchingTask) {
        return NOERROR;
    }
    mAlreadyLaunchingTask = TRUE;

    // Upfront the processing of the thumbnail
    AutoPtr<TaskViewTransform> transform = new TaskViewTransform();
    AutoPtr<IView> sourceView;
    Int32 offsetX = 0;
    Int32 offsetY = 0;
    Float stackScroll = stackView->GetScroller()->GetStackScroll();
    if (tv == NULL) {
        // If there is no actual task view, then use the stack view as the source view
        // and then offset to the expected transform rect, but bound this to just
        // outside the display rect (to ensure we don't animate from too far away)
        sourceView = stackView;
        transform = stackView->GetStackAlgorithm()->GetStackTransform(task, stackScroll, transform, NULL);
        transform->mRect->GetLeft(&offsetX);
        mConfig->mDisplayRect->GetHeight(&offsetY);
    }
    else {
        sourceView = tv->mThumbnailView;
        transform = stackView->GetStackAlgorithm()->GetStackTransform(task, stackScroll, transform, NULL);
    }

    // Compute the thumbnail to scale up from
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);
    AutoPtr<IActivityOptions> opts;
    Int32 w, h;
    if (task->mThumbnail != NULL && (task->mThumbnail->GetWidth(&w), w > 0) &&
            (task->mThumbnail->GetHeight(&h), h > 0)) {
        AutoPtr<IBitmapHelper> bHelper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bHelper);
        AutoPtr<IBitmap> b;
        if (tv != NULL) {
            // Disable any focused state before we draw the header
            if (tv->IsFocusedTask()) {
                tv->UnsetFocusedTask();
            }

            Float scale;
            tv->GetScaleX(&scale);
            tv->mHeaderView->GetMeasuredWidth(&w);
            tv->mHeaderView->GetMeasuredHeight(&h);
            Int32 fromHeaderWidth = (Int32) (w * scale);
            Int32 fromHeaderHeight = (Int32) (h * scale);
            bHelper->CreateBitmap(fromHeaderWidth, fromHeaderHeight,
                    BitmapConfig_ARGB_8888, (IBitmap**)&b);
            if (Constants::DebugFlags::App::EnableTransitionThumbnailDebugMode) {
                b->EraseColor(0xFFff0000);
            }
            else {
                AutoPtr<ICanvas> c;
                CCanvas::New(b, (ICanvas**)&c);
                Float scaleX, scaleY;
                tv->GetScaleX(&scaleX);
                tv->GetScaleY(&scaleY);
                c->Scale(scaleX, scaleY);
                tv->mHeaderView->Draw(c);
                c->SetBitmap(NULL);
            }
        }
        else {
            // Notify the system to skip the thumbnail layer by using an ALPHA_8 bitmap
            bHelper->CreateBitmap(1, 1, BitmapConfig_ALPHA_8, (IBitmap**)&b);
        }
        AutoPtr<IActivityOptionsOnAnimationStartedListener> animStartedListener;
        if (lockToTask) {
            animStartedListener = new OnAnimationStartedListener(ssp, this);
        }
        AutoPtr<IActivityOptionsHelper> aoHelper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
        transform->mRect->GetWidth(&w);
        transform->mRect->GetHeight(&h);
        aoHelper->MakeThumbnailAspectScaleUpAnimation(
            sourceView, b, offsetX, offsetY, w, h,
            animStartedListener, (IActivityOptions**)&opts);
    }

    AutoPtr<Runnable> launchRunnable = new LaunchRunnable(task, opts, lockToTask, this);
    Boolean res;
    // Launch the app right away if there is no task view, otherwise, animate the icon out first
    if (tv == NULL) {
        Post(launchRunnable, &res);
    }
    else {
        AutoPtr<TaskGrouping> tg = (TaskGrouping*)(task->mGroup).Get();
        if (!tg->IsFrontMostTask(task)) {
            // For affiliated tasks that are behind other tasks, we must animate the front cards
            // out of view before starting the task transition
            stackView->StartLaunchTaskAnimation(tv, launchRunnable, lockToTask);
        }
        else {
            // Otherwise, we can start the task transition immediately
            stackView->StartLaunchTaskAnimation(tv, NULL, lockToTask);
            PostDelayed(launchRunnable, 17, &res);
        }
    }
    return NOERROR;
}

// @Override
ECode RecentsView::OnTaskViewAppInfoClicked(
    /* [in] */ ITask* _t)
{
    Task* t = (Task*)_t;
    // Create a new task stack with the application info details activity
    AutoPtr<IIntent> baseIntent = t->mKey->mBaseIntent;
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IComponentName> componet;
    baseIntent->GetComponent((IComponentName**)&componet);
    String pkgName;
    componet->GetPackageName(&pkgName);
    AutoPtr<IUri> uri;
    uriHelper->FromParts(String("package"), pkgName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    componet = NULL;
    intent->ResolveActivity(pm, (IComponentName**)&componet);
    intent->SetComponent(componet);
    AutoPtr<ITaskStackBuilderHelper> tsbHelper;
    CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&tsbHelper);
    AutoPtr<ITaskStackBuilder> taskStackBuilder;
    tsbHelper->Create(context, (ITaskStackBuilder**)&taskStackBuilder);
    taskStackBuilder->AddNextIntentWithParentStack(intent);
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(t->mKey->mUserId, (IUserHandle**)&userHandle);
    taskStackBuilder->StartActivities(NULL, userHandle);
    return NOERROR;
}

// @Override
ECode RecentsView::OnTaskViewDismissed(
    /* [in] */ ITask* taskObj)
{
    Task* t = (Task*)taskObj;
    // Remove any stored data from the loader.  We currently don't bother notifying the views
    // that the data has been unloaded because at the point we call onTaskViewDismissed(), the views
    // either don't need to be updated, or have already been removed.
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    loader->DeleteTaskData(t, FALSE);

    // Remove the old task from activity manager
    AutoPtr<SystemServicesProxy> ssp;
    loader->GetSystemServicesProxy((SystemServicesProxy**)&ssp);
    ssp->RemoveTask(t->mKey->mId, Utilities::IsDocument(t->mKey->mBaseIntent));
    return NOERROR;
}

// @Override
ECode RecentsView::OnAllTaskViewsDismissed()
{
    return mCb->OnAllTaskViewsDismissed();
}

// @Override
ECode RecentsView::OnTaskStackFilterTriggered()
{
    // Hide the search bar
    if (mSearchBar != NULL) {
        AutoPtr<IViewPropertyAnimator> animate;
        mSearchBar->Animate((IViewPropertyAnimator**)&animate);
        animate->Alpha(0.0f);
        animate->SetStartDelay(0);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        animate->SetDuration(mConfig->mFilteringCurrentViewsAnimDuration);
        animate->WithLayer();
        animate->Start();
    }
    return NOERROR;
}

// @Override
ECode RecentsView::OnTaskStackUnfilterTriggered()
{
    // Show the search bar
    if (mSearchBar != NULL) {
        AutoPtr<IViewPropertyAnimator> animate;
        mSearchBar->Animate((IViewPropertyAnimator**)&animate);
        animate->Alpha(1.0f);
        animate->SetStartDelay(0);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        animate->SetDuration(mConfig->mFilteringNewViewsAnimDuration);
        animate->WithLayer();
        animate->Start();
    }
    return NOERROR;
}

/**** RecentsPackageMonitor.PackageCallbacks Implementation ****/

// @Override
ECode RecentsView::OnComponentRemoved(
    /* [in] */ IHashSet* cns)
{
    // Propagate this event down to each task stack view
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child != mSearchBar) {
            AutoPtr<TaskStackView> stackView = (TaskStackView*)ITaskStackView::Probe(child);
            stackView->OnComponentRemoved(cns);
        }
    }
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
