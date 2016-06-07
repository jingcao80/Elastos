#include "elastos/droid/systemui/recents/AlternateRecentsComponent.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/CRecentAnimationEndedReceiver.h"
#include "elastos/droid/systemui/recents/RecentsAppWidgetHost.h"
#include "elastos/droid/systemui/recents/misc/Console.h"
#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/systemui/recents/model/TaskGrouping.h"
#include "elastos/droid/systemui/recents/model/TaskStack.h"
#include "elastos/droid/systemui/recents/views/CTaskStackView.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewLayoutAlgorithm.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewScroller.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::EIID_IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::SystemUI::Recents::Constants;
using Elastos::Droid::SystemUI::Recents::RecentsAppWidgetHost;
using Elastos::Droid::SystemUI::Recents::Misc::Console;
using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStack;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::SystemUI::Recents::Model::TaskGrouping;
using Elastos::Droid::SystemUI::Recents::Model::TaskStack;
using Elastos::Droid::SystemUI::Recents::Views::CTaskStackView;
using Elastos::Droid::SystemUI::Recents::Views::TaskStackView;
using Elastos::Droid::SystemUI::Recents::Views::TaskStackViewLayoutAlgorithm;
using Elastos::Droid::SystemUI::Recents::Views::TaskStackViewScroller;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::View;
using Elastos::Droid::R;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::Core::ISystem;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;

typedef Elastos::Droid::View::View::MeasureSpec MeasureSpec;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

static const String TAG("AlternateRecentsComponent");

//-----------------------------------------------------------------------------------
// AlternateRecentsComponent::OnAnimationStartedRunnable
//-----------------------------------------------------------------------------------

AlternateRecentsComponent::OnAnimationStartedRunnable::OnAnimationStartedRunnable(
    /* [in] */ AlternateRecentsComponent* host)
    : mHost(host)
{}

ECode AlternateRecentsComponent::OnAnimationStartedRunnable::Run()
{
    return mHost->OnAnimationStarted();
}

//-----------------------------------------------------------------------------------
// AlternateRecentsComponent::RecentAnimationEndedReceiver
//-----------------------------------------------------------------------------------

ECode AlternateRecentsComponent::RecentAnimationEndedReceiver::constructor(
    /* [in] */ IAlternateRecentsComponent* host)
{
    BroadcastReceiver::constructor();
    mHost = (AlternateRecentsComponent*)host;
    return NOERROR;
}

// There can be a race condition between the start animation callback and
// the start of the new activity (where we register the receiver that listens
// to this broadcast, so we add our own receiver and if that gets called, then
// we know the activity has not yet started and we can retry sending the broadcast.
ECode AlternateRecentsComponent::RecentAnimationEndedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 resultCode;
    GetResultCode(&resultCode);
    if (resultCode == IActivity::RESULT_OK) {
        mHost->mStartAnimationTriggered = TRUE;
        return NOERROR;
    }

    // Schedule for the broadcast to be sent again after some time
    AutoPtr<IRunnable> mr = new OnAnimationStartedRunnable(mHost);
    Boolean b;
    mHost->mHandler->PostDelayed(mr, 75, &b);
    return NOERROR;
}

//-----------------------------------------------------------------------------------
// AlternateRecentsComponent
//-----------------------------------------------------------------------------------

const Int32 AlternateRecentsComponent::sMinToggleDelay;

const String AlternateRecentsComponent::sToggleRecentsAction("com.android.systemui.recents.SHOW_RECENTS");
const String AlternateRecentsComponent::sRecentsPackage("Elastos.Droid.SystemUI");
const String AlternateRecentsComponent::sRecentsActivity("Elastos.Droid.SystemUI.Recents.CRecentsActivity");
AutoPtr<IBitmap> AlternateRecentsComponent::sLastScreenshot;
AutoPtr<IRecentsComponentCallbacks> AlternateRecentsComponent::sRecentsComponentCallbacks;

CAR_INTERFACE_IMPL_2(AlternateRecentsComponent, Object, IActivityOptionsOnAnimationStartedListener, \
    IAlternateRecentsComponent)

AlternateRecentsComponent::AlternateRecentsComponent(
    /* [in] */ IContext* context)
    : mContext(context)
    , mStatusBarHeight(0)
    , mNavBarHeight(0)
    , mNavBarWidth(0)
    , mTriggeredFromAltTab(FALSE)
    , mLastToggleTime(0)
{
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::Initialize(context);
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    mSystemServicesProxy = new SystemServicesProxy(context);
    CHandler::New((IHandler**)&mHandler);
    CRect::New((IRect**)&mTaskStackBounds);

    CRect::New((IRect**)&mWindowRect);
    CRect::New((IRect**)&mTaskStackBounds);
    CRect::New((IRect**)&mSystemInsets);
    mTmpTransform = new TaskViewTransform();
}

ECode AlternateRecentsComponent::OnStart()
{
    // Initialize some static datastructures
    TaskStackViewLayoutAlgorithm::InitializeCurve();
    // Load the header bar layout
    ReloadHeaderBarLayout();
    // Try and pre-emptively bind the search widget on startup to ensure that we
    // have the right thumbnail bounds to animate to.
    if (Constants::DebugFlags::App::EnableSearchLayout) {
        // If there is no id, then bind a new search app widget
        if (mConfig->mSearchBarAppWidgetId < 0) {
            AutoPtr<RecentsAppWidgetHost> rawh = new RecentsAppWidgetHost(
                mContext, Constants::Values::App::AppWidgetHostId);
            AutoPtr<IAppWidgetHost> host = (IAppWidgetHost*)rawh.Get();
            AutoPtr<IPair> widgetInfo = mSystemServicesProxy->BindSearchAppWidget(host);
            if (widgetInfo != NULL) {
                // Save the app widget id into the settings
                AutoPtr<IInterface> o;
                widgetInfo->GetFirst((IInterface**)&o);
                AutoPtr<IInteger32> i = IInteger32::Probe(o);
                Int32 first;
                i->GetValue(&first);
                mConfig->UpdateSearchBarAppWidgetId(mContext, first);
            }
        }
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnBootCompleted()
{
    mBootCompleted = TRUE;
    return NOERROR;
}

ECode AlternateRecentsComponent::OnShowRecents(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ IView* statusBarView)
{
    mStatusBarView = statusBarView;
    mTriggeredFromAltTab = triggeredFromAltTab;

    ECode ec = StartRecentsActivity();
    if (FAILED(ec)) {
        Console::LogRawError(String("Failed to launch RecentAppsIntent"), ec);
        return E_ACTIVITY_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnHideRecents(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHomeKey)
{
    if (mBootCompleted) {
        AutoPtr<IActivityManagerRunningTaskInfo> amrti;
        GetTopMostTask((IActivityManagerRunningTaskInfo**)&amrti);
        Boolean isRecentsTopMost;
        IsRecentsTopMost(amrti.Get(), (IAtomicBoolean*)NULL, &isRecentsTopMost);
        if (isRecentsTopMost) {
            // Notify recents to hide itself
            AutoPtr<IIntent> intent;
            CIntent::New(IAlternateRecentsComponent::ACTION_HIDE_RECENTS_ACTIVITY, (IIntent**)&intent);
            String pkgn;
            mContext->GetPackageName(&pkgn);
            intent->SetPackage(pkgn);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT |
                IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutBooleanExtra(IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_ALT_TAB, triggeredFromAltTab);
            intent->PutBooleanExtra(IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_HOME_KEY, triggeredFromHomeKey);
            mContext->SendBroadcastAsUser(intent, UserHandle::CURRENT);
        }
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnToggleRecents(
    /* [in] */ IView* statusBarView)
{
    Logger::I(TAG, " >> OnToggleRecents");
    mStatusBarView = statusBarView;
    mTriggeredFromAltTab = FALSE;

    ECode ec = ToggleRecentsActivity();
    if (FAILED(ec)) {
        Console::LogRawError(String("Failed to toggle RecentAppsIntent"), ec);
        return E_ACTIVITY_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnPreloadRecents()
{
    // Do nothing
    return NOERROR;
}

ECode AlternateRecentsComponent::OnCancelPreloadingRecents()
{
    // Do nothing
    return NOERROR;
}

ECode AlternateRecentsComponent::ShowRelativeAffiliatedTask(
    /* [in] */ Boolean showNextTask)
{
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ITaskStack> _stack = loader->GetTaskStack(
        mSystemServicesProxy, res, -1, -1, FALSE, TRUE, NULL, NULL);
    // Return early if there are no tasks
    AutoPtr<TaskStack> stack = (TaskStack*)_stack.Get();
    if (stack->GetTaskCount() == 0) return E_NULL_POINTER_EXCEPTION;

    AutoPtr<IActivityManagerRunningTaskInfo> runningTask;
    GetTopMostTask((IActivityManagerRunningTaskInfo**)&runningTask);
    // Return early if the running task is in the home stack (optimization)
    Int32 id;
    runningTask->GetId(&id);
    if (mSystemServicesProxy->IsInHomeStack(id)) return E_NULL_POINTER_EXCEPTION;

    // Find the task in the recents list
    AutoPtr<IArrayList> tasks = stack->GetTasks();
    AutoPtr<ITask> toTask;
    AutoPtr<IActivityOptions> launchOpts;
    Int32 taskCount;
    tasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> o;
        tasks->Get(i, (IInterface**)&o);
        AutoPtr<ITask> _task = ITask::Probe(o);
        AutoPtr<Task> task = (Task*)_task.Get();
        if (task->mKey->mId == id) {
            AutoPtr<TaskGrouping> group = (TaskGrouping*)(task->mGroup).Get();
            AutoPtr<ITaskKey> toTaskKey;
            AutoPtr<IActivityOptionsHelper> aoh;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
            if (showNextTask) {
                AutoPtr<Task::TaskKey> tk = group->GetNextTaskInGroup(task);
                toTaskKey = (ITaskKey*)tk.Get();
                aoh->MakeCustomAnimation(mContext,
                    R::anim::recents_launch_next_affiliated_task_target,
                    R::anim::recents_launch_next_affiliated_task_source,
                    (IActivityOptions**)&launchOpts);
            }
            else {
                AutoPtr<Task::TaskKey> tk = group->GetPrevTaskInGroup(task);
                toTaskKey = (ITaskKey*)tk.Get();
                aoh->MakeCustomAnimation(mContext,
                    R::anim::recents_launch_prev_affiliated_task_target,
                    R::anim::recents_launch_prev_affiliated_task_source,
                    (IActivityOptions**)&launchOpts);
            }
            if (toTaskKey != NULL) {
                AutoPtr<Task::TaskKey> tk = group->GetPrevTaskInGroup(task);
                AutoPtr<Task> t = stack->FindTaskWithId(tk->mId);
                toTask = (ITask*)t.Get();
            }
            break;
        }
    }

    // Return early if there is no next task
    if (toTask == NULL) {
        if (showNextTask) {
            // XXX: Show the next-task bounce animation
        }
        else {
            // XXX: Show the prev-task bounce animation
        }
        return E_NULL_POINTER_EXCEPTION;
    }

    // Launch the task
    AutoPtr<Task> t = (Task*)toTask.Get();
    if (t->mIsActive) {
        // Bring an active task to the foreground
        mSystemServicesProxy->MoveTaskToFront(t->mKey->mId, launchOpts);
    }
    else {
        mSystemServicesProxy->StartActivityFromRecents(mContext, t->mKey->mId,
                t->mActivityLabel, launchOpts);
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnShowNextAffiliatedTask()
{
    return ShowRelativeAffiliatedTask(TRUE);
}

ECode AlternateRecentsComponent::OnShowPrevAffiliatedTask()
{
    return ShowRelativeAffiliatedTask(FALSE);
}

ECode AlternateRecentsComponent::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Reload the header bar layout
    ReloadHeaderBarLayout();
    sLastScreenshot = NULL;
    return NOERROR;
}

ECode AlternateRecentsComponent::ReloadHeaderBarLayout()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    mWindowRect = mSystemServicesProxy->GetWindowRect();
    res->GetDimensionPixelSize(R::dimen::status_bar_height, &mStatusBarHeight);
    res->GetDimensionPixelSize(R::dimen::navigation_bar_height, &mNavBarHeight);
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::navigation_bar_width, &mNavBarWidth);

    mConfig = RecentsConfiguration::Reinitialize(mContext, mSystemServicesProxy);
    mConfig->UpdateOnConfigurationChange();
    Int32 width, height;
    mWindowRect->GetWidth(&width);
    mWindowRect->GetHeight(&height);
    mConfig->GetTaskStackBounds(width, height, mStatusBarHeight,
            (mConfig->mHasTransposedNavBar ? mNavBarWidth : 0), mTaskStackBounds);
    if (mConfig->mIsLandscape && mConfig->mHasTransposedNavBar) {
        mSystemInsets->Set(0, mStatusBarHeight, mNavBarWidth, 0);
    }
    else {
        mSystemInsets->Set(0, mStatusBarHeight, 0, mNavBarHeight);
    }

    // Inflate the header bar layout so that we can rebind and draw it for the transition
    AutoPtr<TaskStack> stack = new TaskStack();
    CTaskStackView::New(mContext, stack, (ITaskStackView**)&mDummyStackView);
    AutoPtr<TaskStackViewLayoutAlgorithm> algo = ((CTaskStackView*)mDummyStackView.Get())->GetStackAlgorithm();
    AutoPtr<IRect> taskStackBounds;
    CRect::New(mTaskStackBounds, (IRect**)&taskStackBounds);
    Int32 bottom1, bottom2;
    taskStackBounds->GetBottom(&bottom1);
    mSystemInsets->GetBottom(&bottom2);
    taskStackBounds->SetBottom(bottom1 - bottom2);
    algo->ComputeRects(width, height, taskStackBounds);
    AutoPtr<IRect> taskViewSize = algo->GetUntransformedTaskViewSize();
    Int32 taskBarHeight;
    res->GetDimensionPixelSize(R::dimen::recents_task_bar_height, &taskBarHeight);
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::recents_task_view_header, NULL, FALSE, (IView**)&view);
    mHeaderBar = ITaskViewHeader::Probe(view);
    Int32 w;
    taskViewSize->GetWidth(&w);
    IView::Probe(mHeaderBar)->Measure(
            MeasureSpec::MakeMeasureSpec(w, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(taskBarHeight, MeasureSpec::EXACTLY));
    IView::Probe(mHeaderBar)->Layout(0, 0, w, taskBarHeight);
    return NOERROR;
}

ECode AlternateRecentsComponent::GetTopMostTask(
    /* [out] */ IActivityManagerRunningTaskInfo** topMostTask)
{
    VALIDATE_NOT_NULL(topMostTask)
    AutoPtr<SystemServicesProxy> ssp = mSystemServicesProxy;
    AutoPtr<IList> tasks = ssp->GetRunningTasks(1);
    Boolean isEmpty;
    tasks->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<IInterface> o;
        tasks->Get(0, (IInterface**)&o);
        *topMostTask = IActivityManagerRunningTaskInfo::Probe(o);
        REFCOUNT_ADD(*topMostTask)
        return NOERROR;
    }
    *topMostTask = NULL;
    return NOERROR;
}

ECode AlternateRecentsComponent::IsRecentsTopMost(
    /* [in] */ IActivityManagerRunningTaskInfo* topTask,
    /* [in] */ IAtomicBoolean* isHomeTopMost,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<SystemServicesProxy> ssp = mSystemServicesProxy;
    if (topTask != NULL) {
        AutoPtr<IComponentName> topActivity;
        topTask->GetTopActivity((IComponentName**)&topActivity);

        // Check if the front most activity is recents
        String packageName, className;
        topActivity->GetPackageName(&packageName);
        topActivity->GetClassName(&className);
        if (packageName.Equals(sRecentsPackage) && className.Equals(sRecentsActivity)) {
            if (isHomeTopMost != NULL) {
                isHomeTopMost->Set(FALSE);
            }
            *result = TRUE;
            return NOERROR;
        }

        if (isHomeTopMost != NULL) {
            Int32 id;
            topTask->GetId(&id);
            isHomeTopMost->Set(ssp->IsInHomeStack(id));
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode AlternateRecentsComponent::ToggleRecentsActivity()
{
    // If the user has toggled it too quickly, then just eat up the event here (it's better than
    // showing a janky screenshot).
    // NOTE: Ideally, the screenshot mechanism would take the window transform into account
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 currentTimeMillis;
    sys->GetCurrentTimeMillis(&currentTimeMillis);
    if (currentTimeMillis - mLastToggleTime < sMinToggleDelay) {
        return E_NULL_POINTER_EXCEPTION;
    }

    // If Recents is the front most activity, then we should just communicate with it directly
    // to launch the first task or dismiss itself
    AutoPtr<IActivityManagerRunningTaskInfo> topTask;
    GetTopMostTask((IActivityManagerRunningTaskInfo**)&topTask);
    AutoPtr<IAtomicBoolean> isTopTaskHome;
    CAtomicBoolean::New((IAtomicBoolean**)&isTopTaskHome);
    Boolean isRecentsTopMost;
    IsRecentsTopMost(topTask, isTopTaskHome, &isRecentsTopMost);
    Logger::I(TAG, " >> ToggleRecentsActivity : %d", isRecentsTopMost);
    if (isRecentsTopMost) {
        // Notify recents to toggle itself
        AutoPtr<IIntent> intent;
        CIntent::New(IAlternateRecentsComponent::ACTION_TOGGLE_RECENTS_ACTIVITY, (IIntent**)&intent);
        String pn;
        mContext->GetPackageName(&pn);
        intent->SetPackage(pn);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
            | IIntent::FLAG_RECEIVER_FOREGROUND);
        Logger::I(TAG, " >> ToggleRecentsActivity send broadcast: %s", TO_CSTR(intent));
        mContext->SendBroadcastAsUser(intent, UserHandle::CURRENT);
        sys->GetCurrentTimeMillis(&mLastToggleTime);
        return NOERROR;
    }
    else {
        // Otherwise, start the recents activity
        Boolean b;
        isTopTaskHome->Get(&b);
        StartRecentsActivity(topTask, b);
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::StartRecentsActivity()
{
    // Check if the top task is in the home stack, and start the recents activity
    AutoPtr<IActivityManagerRunningTaskInfo> topTask;
    GetTopMostTask((IActivityManagerRunningTaskInfo**)&topTask);
    AutoPtr<IAtomicBoolean> isTopTaskHome;
    CAtomicBoolean::New((IAtomicBoolean**)&isTopTaskHome);
    Boolean isRecentsTopMost;
    IsRecentsTopMost(topTask, isTopTaskHome, &isRecentsTopMost);
    Logger::I(TAG, " >> StartRecentsActivity: isRecentsTopMost: %d", isRecentsTopMost);
    if (!isRecentsTopMost) {
        Boolean b;
        isTopTaskHome->Get(&b);
        StartRecentsActivity(topTask, b);
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::GetUnknownTransitionActivityOptions(
    /* [out] */ IActivityOptions** ao)
{
    VALIDATE_NOT_NULL(ao)
    mStartAnimationTriggered = FALSE;
    AutoPtr<IActivityOptionsHelper> aoh;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
    return aoh->MakeCustomAnimation(mContext,
        R::anim::recents_from_unknown_enter,
        R::anim::recents_from_unknown_exit, mHandler, this, ao);
}

ECode AlternateRecentsComponent::GetHomeTransitionActivityOptions(
    /* [in] */ Boolean fromSearchHome,
    /* [out] */ IActivityOptions** ao)
{
    VALIDATE_NOT_NULL(ao)
    mStartAnimationTriggered = FALSE;
    AutoPtr<IActivityOptionsHelper> aoh;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
    if (fromSearchHome) {
        return aoh->MakeCustomAnimation(mContext,
            R::anim::recents_from_search_launcher_enter,
            R::anim::recents_from_search_launcher_exit, mHandler, this, ao);
    }
    return aoh->MakeCustomAnimation(mContext,
        R::anim::recents_from_launcher_enter,
        R::anim::recents_from_launcher_exit, mHandler, this, ao);
}

ECode AlternateRecentsComponent::GetThumbnailTransitionActivityOptions(
    /* [in] */ IActivityManagerRunningTaskInfo* topTask,
    /* [in] */ Boolean isTopTaskHome,
    /* [out] */ IActivityOptions** ao)
{
    if (Constants::DebugFlags::App::EnableScreenshotAppTransition) {
        // Recycle the last screenshot
        ConsumeLastScreenshot();

        // Take the full screenshot
        sLastScreenshot = mSystemServicesProxy->TakeAppScreenshot();
        if (sLastScreenshot != NULL) {
            mStartAnimationTriggered = FALSE;
            AutoPtr<IActivityOptionsHelper> aoh;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
            return aoh->MakeCustomAnimation(mContext,
                R::anim::recents_from_app_enter,
                R::anim::recents_from_app_exit, mHandler, this, ao);
        }
    }

    // Update the destination rect
    AutoPtr<Task> toTask = new Task();
    Int32 id;
    topTask->GetId(&id);
    AutoPtr<TaskViewTransform> toTransform;
    GetThumbnailTransitionTransform(id, isTopTaskHome, toTask,
        (TaskViewTransform**)&toTransform);
    if (toTransform != NULL && toTask->mKey != NULL) {
        AutoPtr<IRect> toTaskRect = toTransform->mRect;
        Int32 mw, mh;
        IView::Probe(mHeaderBar)->GetMeasuredWidth(&mw);
        IView::Probe(mHeaderBar)->GetMeasuredHeight(&mh);
        Int32 toHeaderWidth = (Int32) (mw * toTransform->mScale);
        Int32 toHeaderHeight = (Int32) (mh * toTransform->mScale);
        AutoPtr<IBitmapHelper> bh;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
        AutoPtr<IBitmap> thumbnail;
        bh->CreateBitmap(toHeaderWidth, toHeaderHeight,
            BitmapConfig_ARGB_8888, (IBitmap**)&thumbnail);
        if (Constants::DebugFlags::App::EnableTransitionThumbnailDebugMode) {
            thumbnail->EraseColor(0xFFff0000);
        }
        else {
            AutoPtr<ICanvas> c;
            CCanvas::New(thumbnail, (ICanvas**)&c);
            c->Scale(toTransform->mScale, toTransform->mScale);
            mHeaderBar->RebindToTask(toTask);
            IView::Probe(mHeaderBar)->Draw(c);
            c->SetBitmap(NULL);
        }

        mStartAnimationTriggered = FALSE;
        AutoPtr<IActivityOptionsHelper> aoh;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
        Int32 left, top, width, height;
        toTaskRect->GetLeft(&left);
        toTaskRect->GetTop(&top);
        toTaskRect->GetWidth(&width);
        toTaskRect->GetHeight(&height);
        return aoh->MakeThumbnailAspectScaleDownAnimation(mStatusBarView,
            thumbnail, left, top, width, height, this, ao);
    }

    // If both the screenshot and thumbnail fails, then just fall back to the default transition
    return GetUnknownTransitionActivityOptions(ao);
}

ECode AlternateRecentsComponent::GetThumbnailTransitionTransform(
    /* [in] */ Int32 runningTaskId,
    /* [in] */ Boolean isTopTaskHome,
    /* [in] */ ITask* runningTaskOut,
    /* [out] */ TaskViewTransform** ttt)
{
    // Get the stack of tasks that we are animating into
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ITaskStack> _stack = loader->GetTaskStack(mSystemServicesProxy,
        res, runningTaskId, -1, FALSE, isTopTaskHome, NULL, NULL);

    AutoPtr<TaskStack> stack = (TaskStack*)_stack.Get();

    if (stack->GetTaskCount() == 0) {
        *ttt = NULL;
        return NOERROR;
    }

    // Find the running task in the TaskStack
    AutoPtr<Task> task;
    AutoPtr<IArrayList> tasks = stack->GetTasks();
    if (runningTaskId != -1) {
        // Otherwise, try and find the task with the
        Int32 taskCount;
        tasks->GetSize(&taskCount);
        for (Int32 i = taskCount - 1; i >= 0; i--) {
            AutoPtr<IInterface> o;
            tasks->Get(i, (IInterface**)&o);
            AutoPtr<ITask> _t = ITask::Probe(o);
            AutoPtr<Task> t = (Task*)_t.Get();
            if (t->mKey->mId == runningTaskId) {
                task = t;
                runningTaskOut->CopyFrom(_t);
                break;
            }
        }
    }
    if (task == NULL) {
        // If no task is specified or we can not find the task just use the front most one
        Int32 size;
        tasks->GetSize(&size);
        AutoPtr<IInterface> o;
        tasks->Get(size - 1, (IInterface**)&o);
        AutoPtr<ITask> _t = ITask::Probe(o);
        task = (Task*)_t.Get();
    }

    // Get the transform for the running task
    ((CTaskStackView*)mDummyStackView.Get())->UpdateMinMaxScrollForStack(stack,
            mTriggeredFromAltTab, isTopTaskHome);
    AutoPtr<TaskStackViewScroller> scroller = ((CTaskStackView*)mDummyStackView.Get())->GetScroller();
    scroller->SetStackScrollToInitialState();
    AutoPtr<TaskStackViewLayoutAlgorithm> va = ((CTaskStackView*)mDummyStackView.Get())->GetStackAlgorithm();
    mTmpTransform = va->GetStackTransform(task,
        scroller->GetStackScroll(), mTmpTransform, NULL);
    *ttt = mTmpTransform;
    REFCOUNT_ADD(*ttt)
    return NOERROR;
}

ECode AlternateRecentsComponent::StartRecentsActivity(
    /* [in] */ IActivityManagerRunningTaskInfo* topTask,
    /* [in] */ Boolean isTopTaskHome)
{
    // If Recents is not the front-most activity and we should animate into it.  If
    // the activity at the root of the top task stack in the home stack, then we just do a
    // simple transition.  Otherwise, we animate to the rects defined by the Recents service,
    // which can differ depending on the number of items in the list.
    AutoPtr<SystemServicesProxy> ssp = mSystemServicesProxy;
    Int32 id;
    UserHandle::CURRENT->GetIdentifier(&id);

    AutoPtr<IList> recentTasks = ssp->GetRecentTasks(3, id, isTopTaskHome);
    Boolean useThumbnailTransition = !isTopTaskHome;
    Boolean hasRecentTasks;
    recentTasks->IsEmpty(&hasRecentTasks);
    hasRecentTasks = !hasRecentTasks;

    if (useThumbnailTransition) {
        // Try starting with a thumbnail transition
        AutoPtr<IActivityOptions> opts;
        GetThumbnailTransitionActivityOptions(topTask, isTopTaskHome, (IActivityOptions**)&opts);
        if (opts != NULL) {
            if (sLastScreenshot != NULL) {
                StartAlternateRecentsActivity(topTask, opts,
                    IAlternateRecentsComponent::EXTRA_FROM_APP_FULL_SCREENSHOT);
            }
            else {
                StartAlternateRecentsActivity(topTask, opts,
                    IAlternateRecentsComponent::EXTRA_FROM_APP_THUMBNAIL);
            }
        }
        else {
            // Fall through below to the non-thumbnail transition
            useThumbnailTransition = FALSE;
        }
    }

    if (!useThumbnailTransition) {
        // If there is no thumbnail transition, but is launching from home into recents, then
        // use a quick home transition and do the animation from home
        if (hasRecentTasks) {
            // Get the home activity info
            String homeActivityPackage = mSystemServicesProxy->GetHomeActivityPackageName();
            // Get the search widget info
            AutoPtr<IAppWidgetProviderInfo> searchWidget;
            String searchWidgetPackage;
            if (mConfig->HasSearchBarAppWidget()) {
                searchWidget = mSystemServicesProxy->GetAppWidgetInfo(mConfig->mSearchBarAppWidgetId);
            }
            else {
                searchWidget = mSystemServicesProxy->ResolveSearchAppWidget();
            }

            if (searchWidget != NULL) {
                AutoPtr<IComponentName> provider;
                searchWidget->GetProvider((IComponentName**)&provider);
                if (provider != NULL) {
                    provider->GetPackageName(&searchWidgetPackage);
                }
            }

            // Determine whether we are coming from a search owned home activity
            Boolean fromSearchHome = FALSE;
            if (!homeActivityPackage.IsNull() && !searchWidgetPackage.IsNull() &&
                    homeActivityPackage.Equals(searchWidgetPackage)) {
                fromSearchHome = TRUE;
            }

            AutoPtr<IActivityOptions> opts;
            GetHomeTransitionActivityOptions(fromSearchHome, (IActivityOptions**)&opts);
            StartAlternateRecentsActivity(topTask, opts,
                fromSearchHome ? IAlternateRecentsComponent::EXTRA_FROM_SEARCH_HOME
                    : IAlternateRecentsComponent::EXTRA_FROM_HOME);
        }
        else {
            // Otherwise we do the normal fade from an unknown source
            AutoPtr<IActivityOptions> opts;
            GetUnknownTransitionActivityOptions((IActivityOptions**)&opts);
            StartAlternateRecentsActivity(topTask, opts, String(NULL));
        }
    }
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mLastToggleTime);
    return NOERROR;
}

ECode AlternateRecentsComponent::StartAlternateRecentsActivity(
    /* [in] */ IActivityManagerRunningTaskInfo* topTask,
    /* [in] */ IActivityOptions* opts,
    /* [in] */ const String& extraFlag)
{
    AutoPtr<IIntent> intent;
    CIntent::New(sToggleRecentsAction, (IIntent**)&intent);
    intent->SetClassName(sRecentsPackage, sRecentsActivity);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
        | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
        | IIntent::FLAG_ACTIVITY_TASK_ON_HOME);
    if (!extraFlag.IsNull()) {
        intent->PutBooleanExtra(extraFlag, TRUE);
    }
    intent->PutBooleanExtra(IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_ALT_TAB, mTriggeredFromAltTab);
    Int32 id;
    topTask->GetId(&id);
    intent->PutExtra(IAlternateRecentsComponent::EXTRA_FROM_TASK_ID, (topTask != NULL) ? id : -1);
    Logger::I(TAG, " >> StartAlternateRecentsActivity: %s", TO_CSTR(intent));
    if (opts != NULL) {
        AutoPtr<IBundle> bundle;
        opts->ToBundle((IBundle**)&bundle);
        mContext->StartActivityAsUser(intent, bundle, UserHandle::CURRENT);
    }
    else {
        mContext->StartActivityAsUser(intent, UserHandle::CURRENT);
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::GetLastScreenshot(
    /* [out] */ IBitmap** lss)
{
    VALIDATE_NOT_NULL(lss)
    *lss = sLastScreenshot;
    REFCOUNT_ADD(*lss)
    return NOERROR;
}

ECode AlternateRecentsComponent::ConsumeLastScreenshot()
{
    if (sLastScreenshot != NULL) {
        sLastScreenshot->Recycle();
        sLastScreenshot = NULL;
    }
    return NOERROR;
}

/** Sets the RecentsComponent callbacks. */
CARAPI AlternateRecentsComponent::SetRecentsComponentCallback(
    /* [in] */ IRecentsComponentCallbacks* cb)
{
    sRecentsComponentCallbacks = cb;
    return NOERROR;
}

ECode AlternateRecentsComponent::NotifyVisibilityChanged(
    /* [in] */ Boolean visible)
{
    if (sRecentsComponentCallbacks != NULL) {
        sRecentsComponentCallbacks->OnVisibilityChanged(visible);
    }
    return NOERROR;
}

ECode AlternateRecentsComponent::OnAnimationStarted()
{
    // Notify recents to start the enter animation
    if (!mStartAnimationTriggered) {
        // There can be a race condition between the start animation callback and
        // the start of the new activity (where we register the receiver that listens
        // to this broadcast, so we add our own receiver and if that gets called, then
        // we know the activity has not yet started and we can retry sending the broadcast.
        AutoPtr<IBroadcastReceiver> fallbackReceiver;
        CRecentAnimationEndedReceiver::New(this, (IBroadcastReceiver**)&fallbackReceiver);

        // Send the broadcast to notify Recents that the animation has started
        AutoPtr<IIntent> intent;
        CIntent::New(IAlternateRecentsComponent::ACTION_START_ENTER_ANIMATION, (IIntent**)&intent);
        String pn;
        mContext->GetPackageName(&pn);
        intent->SetPackage(pn);
        intent->AddFlags(
            IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT |
            IIntent::FLAG_RECEIVER_FOREGROUND);
        mContext->SendOrderedBroadcastAsUser(intent, UserHandle::CURRENT, String(NULL),
            fallbackReceiver, NULL, IActivity::RESULT_CANCELED, String(NULL), NULL);
    }
    return NOERROR;
}

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos