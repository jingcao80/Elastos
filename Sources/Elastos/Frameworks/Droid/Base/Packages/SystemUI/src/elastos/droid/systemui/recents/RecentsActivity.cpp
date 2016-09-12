#include "elastos/droid/systemui/recents/RecentsActivity.h"
#include "elastos/droid/systemui/recents/CRecentsActivitySystemBroadcastReceiver.h"
#include "elastos/droid/systemui/recents/CRecentsActivityServiceBroadcastReceiver.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/systemui/recents/model/TaskStack.h"
#include "R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::SystemUI::Recents::Constants;
using Elastos::Droid::SystemUI::Recents::EIID_IRecentsAppWidgetHostCallbacks;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::Recents::Misc::Utilities;
using Elastos::Droid::SystemUI::Recents::Model::ISpaceNode;
using Elastos::Droid::SystemUI::Recents::Model::ITask;
using Elastos::Droid::SystemUI::Recents::Model::ITaskStack;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::SystemUI::Recents::Model::TaskStack;
using Elastos::Droid::SystemUI::Recents::Views::EIID_IDebugOverlayViewCallbacks;
using Elastos::Droid::SystemUI::Recents::Views::EIID_IRecentsViewCallbacks;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

static const String TAG("SystemUI::Recents::RecentsActivity");

//---------------------------------------------------------------------
// RecentsActivity::FinishRecentsRunnable
//----------------------------------------------------------------------

RecentsActivity::FinishRecentsRunnable::FinishRecentsRunnable(
    /* [in] */ IIntent* launchIntent,
    /* [in] */ IActivityOptions* opts,
    /* [in] */ RecentsActivity* host)
    : mLaunchIntent(launchIntent)
    , mLaunchOpts(opts)
    , mHost(host)
{
}

ECode RecentsActivity::FinishRecentsRunnable::Run()
{
    // Mark Recents as no longer visible
    AlternateRecentsComponent::NotifyVisibilityChanged(FALSE);
    mHost->mVisible = FALSE;
    // Finish Recents
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> current;
    uhh->GetCURRENT((IUserHandle**)&current);
    if (mLaunchIntent != NULL) {
        if (mLaunchOpts != NULL) {
            AutoPtr<IBundle> bundle;
            mLaunchOpts->ToBundle((IBundle**)&bundle);
            mHost->StartActivityAsUser(mLaunchIntent, bundle, current);
        }
        else {
            mHost->StartActivityAsUser(mLaunchIntent, current);
        }
    }
    else {
        mHost->Finish();
        mHost->OverridePendingTransition(R::anim::recents_to_launcher_enter,
            R::anim::recents_to_launcher_exit);
    }
    return NOERROR;
}

//---------------------------------------------------------------------
// RecentsActivity::ServiceBroadcastReceiver
//----------------------------------------------------------------------

ECode RecentsActivity::ServiceBroadcastReceiver::constructor(
    /* [in] */ IRecentsActivity* host)
{
    mHost = (RecentsActivity*)host;
    return BroadcastReceiver::constructor();
}

ECode RecentsActivity::ServiceBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IAlternateRecentsComponent::ACTION_HIDE_RECENTS_ACTIVITY)) {
        // Mark Recents as no longer visible
        AlternateRecentsComponent::NotifyVisibilityChanged(FALSE);
        Boolean bval;
        if (intent->GetBooleanExtra(
            IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_ALT_TAB, FALSE, &bval), bval) {
            // If we are hiding from releasing Alt-Tab, dismiss Recents to the focused app
            mHost->DismissRecentsToFocusedTaskOrHome(FALSE, &bval);
        }
        else if (intent->GetBooleanExtra(
            IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_HOME_KEY, FALSE, &bval), bval) {
            // Otherwise, dismiss Recents to Home
            mHost->DismissRecentsToHome(TRUE, &bval);
        }
        else {
            // Do nothing, another activity is being launched on top of Recents
        }
    }
    else if (action.Equals(IAlternateRecentsComponent::ACTION_TOGGLE_RECENTS_ACTIVITY)) {
        // If we are toggling Recents, then first unfilter any filtered stacks first
        Boolean b;
        mHost->DismissRecentsToFocusedTaskOrHome(TRUE, &b);
    }
    else if (action.Equals(IAlternateRecentsComponent::ACTION_START_ENTER_ANIMATION)) {
        // Try and start the enter animation (or restart it on configuration changed)
        AutoPtr<ReferenceCountedTrigger> t = new ReferenceCountedTrigger();
        t->constructor(context, NULL, NULL, NULL);
        AutoPtr<ViewAnimation::TaskViewEnterContext> tvec = new ViewAnimation::TaskViewEnterContext(t);
        mHost->mRecentsView->StartEnterRecentsAnimation(tvec);
        mHost->OnEnterAnimationTriggered();
        // Notify the fallback receiver that we have successfully got the broadcast
        // See AlternateRecentsComponent.onAnimationStarted()
        SetResultCode(IActivity::RESULT_OK);
    }
    return NOERROR;
}

//---------------------------------------------------------------------
// RecentsActivity::SystemBroadcastReceiver
//----------------------------------------------------------------------
ECode RecentsActivity::SystemBroadcastReceiver::constructor(
    /* [in] */ IRecentsActivity* host)
{
    mHost = (RecentsActivity*)host;
    return BroadcastReceiver::constructor();
}

ECode RecentsActivity::SystemBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        // When the screen turns off, dismiss Recents to Home
        Boolean b;
        mHost->DismissRecentsToHome(FALSE, &b);
    }
    else if (action.Equals(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED)) {
        // When the search activity changes, update the Search widget
        mHost->RefreshSearchWidget();
    }
    return NOERROR;
}

//---------------------------------------------------------------------
// RecentsActivity::OnDebugModeTriggeredRunnable
//----------------------------------------------------------------------

RecentsActivity::OnDebugModeTriggeredRunnable::OnDebugModeTriggeredRunnable(
    /* [in] */ RecentsActivity* host)
    : mHost(host)
{}

ECode RecentsActivity::OnDebugModeTriggeredRunnable::Run()
{
    return mHost->OnDebugModeTriggered();
}

//---------------------------------------------------------------------
// RecentsActivity::AppWidgetHostCallbackRunnable
//----------------------------------------------------------------------

RecentsActivity::AppWidgetHostCallbackRunnable::AppWidgetHostCallbackRunnable(
    /* [in] */ IRecentsAppWidgetHostCallbacks* callback,
    /* [in] */ RecentsActivity* host)
    : mCallback(callback)
    , mHost(host)
{}

ECode RecentsActivity::AppWidgetHostCallbackRunnable::Run()
{
    if (mCallback != NULL) {
        mHost->mAppWidgetHost->StartListening(mCallback);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_3(RecentsActivity::InnerCallback, Object, IRecentsViewCallbacks,
    IDebugOverlayViewCallbacks, IRecentsAppWidgetHostCallbacks)

RecentsActivity::InnerCallback::InnerCallback(
    /* [in] */ RecentsActivity* host)
    : mHost(host)
{
}

// @Override
ECode RecentsActivity::InnerCallback::OnExitToHomeAnimationTriggered()
{
    return mHost->OnExitToHomeAnimationTriggered();
}

// @Override
ECode RecentsActivity::InnerCallback::OnTaskViewClicked()
{
    return mHost->OnTaskViewClicked();
}

// @Override
ECode RecentsActivity::InnerCallback::OnTaskLaunchFailed()
{
    return mHost->OnTaskLaunchFailed();
}

// @Override
ECode RecentsActivity::InnerCallback::OnAllTaskViewsDismissed()
{
    return mHost->OnAllTaskViewsDismissed();
}

ECode RecentsActivity::InnerCallback::OnPrimarySeekBarChanged(
    /* [in] */ Float progress)
{
    return mHost->OnPrimarySeekBarChanged(progress);
}

// @Override
ECode RecentsActivity::InnerCallback::OnSecondarySeekBarChanged(
    /* [in] */ Float progress)
{
    return mHost->OnSecondarySeekBarChanged(progress);
}

ECode RecentsActivity::InnerCallback::RefreshSearchWidget()
{
    return mHost->RefreshSearchWidget();
}

//---------------------------------------------------------------------
// RecentsActivity
//----------------------------------------------------------------------

CAR_INTERFACE_IMPL(RecentsActivity, Activity, IRecentsActivity)

RecentsActivity::RecentsActivity()
    : mVisible(FALSE)
    , mLastTabKeyEventTime(0L)
{
}

RecentsActivity::~RecentsActivity()
{
}

ECode RecentsActivity::constructor()
{
    CRecentsActivityServiceBroadcastReceiver::New(this, (IBroadcastReceiver**)&mServiceBroadcastReceiver);
    CRecentsActivitySystemBroadcastReceiver::New(this, (IBroadcastReceiver**)&mSystemBroadcastReceiver);

    AutoPtr<OnDebugModeTriggeredRunnable> mr = new OnDebugModeTriggeredRunnable(this);
    mDebugTrigger = new DebugTrigger((IRunnable*)mr);
    return Activity::constructor();
}

ECode RecentsActivity::UpdateRecentsTasks(
    /* [in] */ IIntent* launchIntent)
{
    // Update the configuration based on the launch intent
    Boolean fromSearchHome;
    launchIntent->GetBooleanExtra(
        IAlternateRecentsComponent::EXTRA_FROM_SEARCH_HOME, FALSE, &fromSearchHome);
    Boolean b1;
    launchIntent->GetBooleanExtra(
        IAlternateRecentsComponent::EXTRA_FROM_HOME, FALSE, &b1);
    mConfig->mLaunchedFromHome = fromSearchHome || b1;
    launchIntent->GetBooleanExtra(
        IAlternateRecentsComponent::EXTRA_FROM_APP_THUMBNAIL, FALSE,
        &(mConfig->mLaunchedFromAppWithThumbnail));

    launchIntent->GetBooleanExtra(
        IAlternateRecentsComponent::EXTRA_FROM_APP_FULL_SCREENSHOT, FALSE,
        &(mConfig->mLaunchedFromAppWithScreenshot));

    launchIntent->GetInt32Extra(
        IAlternateRecentsComponent::EXTRA_FROM_TASK_ID, -1,
        &(mConfig->mLaunchedToTaskId));

    launchIntent->GetBooleanExtra(
        IAlternateRecentsComponent::EXTRA_TRIGGERED_FROM_ALT_TAB, FALSE,
        &(mConfig->mLaunchedWithAltTab));

    // Load all the tasks
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    AutoPtr<ISpaceNode> root = loader->Reload(this,
        Constants::Values::RecentsTaskLoader::PreloadFirstTasksCount,
        mConfig->mLaunchedFromHome);
    AutoPtr<IArrayList> stacks;
    root->GetStacks((IArrayList**)&stacks);
    Boolean isEmpty;
    stacks->IsEmpty(&isEmpty);
    if (!isEmpty) {
        mRecentsView->SetTaskStacks(IList::Probe(stacks));
    }
    Boolean hasTasks;
    root->HasTasks(&hasTasks);
    mConfig->mLaunchedWithNoRecentTasks = !hasTasks;

    // Create the home intent runnable
    AutoPtr<IIntent> homeIntent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&homeIntent);
    homeIntent->AddCategory(IIntent::CATEGORY_HOME);
    homeIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
        IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    AutoPtr<IActivityOptionsHelper> aoh;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
    AutoPtr<IActivityOptions> ao;
    aoh->MakeCustomAnimation(this,
        fromSearchHome ? R::anim::recents_to_search_launcher_enter :
            R::anim::recents_to_launcher_enter,
        fromSearchHome ? R::anim::recents_to_search_launcher_exit :
            R::anim::recents_to_launcher_exit,
        (IActivityOptions**)&ao);
    mFinishLaunchHomeRunnable = new FinishRecentsRunnable(homeIntent, ao, this);

    // Mark the task that is the launch target
    Int32 taskStackCount;
    stacks->GetSize(&taskStackCount);
    if (mConfig->mLaunchedToTaskId != -1) {
        for (Int32 i = 0; i < taskStackCount; i++) {
            AutoPtr<IInterface> o;
            stacks->Get(i, (IInterface**)&o);
            AutoPtr<ITaskStack> ts = ITaskStack::Probe(o);
            AutoPtr<TaskStack> stack = (TaskStack*)ts.Get();
            AutoPtr<IArrayList> tasks = stack->GetTasks();
            Int32 taskCount;
            tasks->GetSize(&taskCount);
            for (Int32 j = 0; j < taskCount; j++) {
                AutoPtr<IInterface> o2;
                tasks->Get(j, (IInterface**)&o2);
                AutoPtr<ITask> _t = ITask::Probe(o2);
                AutoPtr<Task> t = (Task*)_t.Get();
                if (t->mKey->mId == mConfig->mLaunchedToTaskId) {
                    t->mIsLaunchTarget = TRUE;
                    break;
                }
            }
        }
    }

    // Update the top level view's visibilities
    if (mConfig->mLaunchedWithNoRecentTasks) {
        if (mEmptyView == NULL) {
            mEmptyViewStub->Inflate((IView**)&mEmptyView);
        }
        IView::Probe(mEmptyView)->SetVisibility(IView::VISIBLE);
        mRecentsView->SetSearchBarVisibility(IView::GONE);
    }
    else {
        if (mEmptyView != NULL) {
            IView::Probe(mEmptyView)->SetVisibility(IView::GONE);
        }
        if (mRecentsView->HasSearchBar()) {
            mRecentsView->SetSearchBarVisibility(IView::VISIBLE);
        }
        else {
            AddSearchBarAppWidgetView();
        }
    }

    // Animate the SystemUI scrims into view
    mScrimViews->PrepareEnterRecentsAnimation();
    return NOERROR;
}

ECode RecentsActivity::BindSearchBarAppWidget()
{
    if (Constants::DebugFlags::App::EnableSearchLayout) {
        AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
        AutoPtr<SystemServicesProxy> ssp;
        rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);

        // Reset the host view and widget info
        mSearchAppWidgetHostView = NULL;
        mSearchAppWidgetInfo = NULL;

        // Try and load the app widget id from the settings
        Int32 appWidgetId = mConfig->mSearchBarAppWidgetId;
        if (appWidgetId >= 0) {
            mSearchAppWidgetInfo = ssp->GetAppWidgetInfo(appWidgetId);
            if (mSearchAppWidgetInfo == NULL) {
                // If there is no actual widget associated with that id, then delete it and
                // prepare to bind another app widget in its place
                ssp->UnbindSearchAppWidget(mAppWidgetHost, appWidgetId);
                appWidgetId = -1;
            }
        }

        // If there is no id, then bind a new search app widget
        if (appWidgetId < 0) {
            AutoPtr<IPair> widgetInfo = ssp->BindSearchAppWidget(mAppWidgetHost);
            if (widgetInfo != NULL) {
                // Save the app widget id into the settings
                AutoPtr<IInterface> o1;
                widgetInfo->GetFirst((IInterface**)&o1);
                AutoPtr<IInteger32> _first = IInteger32::Probe(o1);
                Int32 first;
                _first->GetValue(&first);
                mConfig->UpdateSearchBarAppWidgetId(this, first);
                AutoPtr<IInterface> o2;
                widgetInfo->GetSecond((IInterface**)&o2);
                AutoPtr<IAppWidgetProviderInfo> second = IAppWidgetProviderInfo::Probe(o2);
                mSearchAppWidgetInfo = second;
            }
        }
    }
    return NOERROR;
}

ECode RecentsActivity::AddSearchBarAppWidgetView()
{
    if (Constants::DebugFlags::App::EnableSearchLayout) {
        Int32 appWidgetId = mConfig->mSearchBarAppWidgetId;
        if (appWidgetId >= 0) {
            mAppWidgetHost->CreateView(this, appWidgetId,
                mSearchAppWidgetInfo, (IAppWidgetHostView**)&mSearchAppWidgetHostView);
            AutoPtr<IBundle> opts;
            CBundle::New((IBundle**)&opts);
            opts->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
                IAppWidgetProviderInfo::WIDGET_CATEGORY_SEARCHBOX);
            mSearchAppWidgetHostView->UpdateAppWidgetOptions(opts);
            // Set the padding to 0 for this search widget
            IView::Probe(mSearchAppWidgetHostView)->SetPadding(0, 0, 0, 0);
            mRecentsView->SetSearchBar(IView::Probe(mSearchAppWidgetHostView));
        }
        else {
            mRecentsView->SetSearchBar(NULL);
        }
    }
    return NOERROR;
}

ECode RecentsActivity::DismissRecentsToFocusedTaskOrHome(
    /* [in] */ Boolean checkFilteredStackState,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mVisible) {
        // If we currently have filtered stacks, then unfilter those first
        if (checkFilteredStackState &&
            mRecentsView->UnfilterFilteredStacks()) {
            *result = TRUE;
            return NOERROR;
        }
        // If we have a focused Task, launch that Task now
        if (mRecentsView->LaunchFocusedTask()) {
            *result = TRUE;
            return NOERROR;
        }
        // If we launched from Home, then return to Home
        if (mConfig->mLaunchedFromHome) {
            DismissRecentsToHomeRaw(TRUE);
            *result = TRUE;
            return NOERROR;
        }
        // Otherwise, try and return to the Task that Recents was launched from
        if (mRecentsView->LaunchPreviousTask()) {
            *result = TRUE;
            return NOERROR;
        }
        // If none of the other cases apply, then just go Home
        DismissRecentsToHomeRaw(TRUE);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode RecentsActivity::DismissRecentsToHomeRaw(
    /* [in] */ Boolean animated)
{
    if (animated) {
        AutoPtr<ReferenceCountedTrigger> exitTrigger = new ReferenceCountedTrigger();
        exitTrigger->constructor(this, NULL, mFinishLaunchHomeRunnable, NULL);
        AutoPtr<ViewAnimation::TaskViewExitContext> tvec =
            new ViewAnimation::TaskViewExitContext(exitTrigger);
        mRecentsView->StartExitToHomeAnimation(tvec);
    }
    else {
        mFinishLaunchHomeRunnable->Run();
    }
    return NOERROR;
}

ECode RecentsActivity::DismissRecentsToHome(
    /* [in] */ Boolean animated,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mVisible) {
        // Return to Home
        DismissRecentsToHomeRaw(animated);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode RecentsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    // For the non-primary user, ensure that the SystemSericesProxy is initialized
    RecentsTaskLoader::Initialize(this);

    // Initialize the loader and the configuration
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);

    mConfig = RecentsConfiguration::Reinitialize(this, ssp);

    // Initialize the widget host (the host id is static and does not change)
    mAppWidgetHost = new RecentsAppWidgetHost(this, Constants::Values::App::AppWidgetHostId);

    // Set the Recents layout
    SetContentView(R::layout::recents);
    AutoPtr<IView> v;
    FindViewById(R::id::recents_view, (IView**)&v);
    mRecentsView = (RecentsView*)v.Get();

    AutoPtr<InnerCallback> cb = new InnerCallback(this);
    mRecentsView->SetCallbacks(cb);
    mRecentsView->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE |
        IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
        IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);

    AutoPtr<IView> v2;
    FindViewById(R::id::empty_view_stub, (IView**)&v2);
    mEmptyViewStub = IViewStub::Probe(v2);

    AutoPtr<IView> v3;
    FindViewById(R::id::debug_overlay_stub, (IView**)&v3);
    mDebugOverlayStub = IViewStub::Probe(v3);

    mScrimViews = new SystemBarScrimViews((IActivity*)this, mConfig);
    InflateDebugOverlay();

    // Bind the search app widget when we first start up
    BindSearchBarAppWidget();
    // Update the recent tasks
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    UpdateRecentsTasks(intent);

    // Register the broadcast receiver to handle messages when the screen is turned off
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED);
    AutoPtr<IIntent> intent2;
    RegisterReceiver(mSystemBroadcastReceiver, filter, (IIntent**)&intent2);

    // Private API calls to make the shadows look better
    // try {
    Utilities::SetShadowProperty(String("ambientRatio"), StringUtils::ToString(1.5f));
    // } catch (IllegalAccessException e) {
    //     e.printStackTrace();
    // } catch (InvocationTargetException e) {
    //     e.printStackTrace();
    // }

    // Update if we are getting a configuration change
    if (savedInstanceState != NULL) {
        mConfig->UpdateOnConfigurationChange();
        OnConfigurationChange();
    }

    // Start listening for widget package changes if there is one bound, post it since we don't
    // want it stalling the startup
    if (mConfig->mSearchBarAppWidgetId >= 0) {
        AutoPtr<InnerCallback> cb = new InnerCallback(this);
        AutoPtr<AppWidgetHostCallbackRunnable> mr = new AppWidgetHostCallbackRunnable(cb, this);
        Boolean b;
        mRecentsView->Post(mr, &b);
    }
    return NOERROR;
}

ECode RecentsActivity::InflateDebugOverlay()
{
    if (mConfig->mDebugModeEnabled && mDebugOverlay == NULL) {
        // Inflate the overlay and seek bars
        AutoPtr<IView> v;
        mDebugOverlayStub->Inflate((IView**)&v);
        mDebugOverlay = IDebugOverlayView::Probe(v);
        AutoPtr<InnerCallback> cb = new InnerCallback(this);
        mDebugOverlay->SetCallbacks(cb);
        mRecentsView->SetDebugOverlay((DebugOverlayView*)mDebugOverlay.Get());
    }
    return NOERROR;
}

ECode RecentsActivity::OnConfigurationChange()
{
    // Update RecentsConfiguration
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);

    mConfig = RecentsConfiguration::Reinitialize(this, ssp);

    // Try and start the enter animation (or restart it on configuration changed)
    AutoPtr<ReferenceCountedTrigger> t = new ReferenceCountedTrigger();
    t->constructor(this, NULL, NULL, NULL);
    AutoPtr<ViewAnimation::TaskViewEnterContext> tvec =
        new ViewAnimation::TaskViewEnterContext(t);
    mRecentsView->StartEnterRecentsAnimation(tvec);
    OnEnterAnimationTriggered();
    return NOERROR;
}

ECode RecentsActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    Activity::OnNewIntent(intent);
    Activity::SetIntent(intent);

    // Reinitialize the configuration
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);

    RecentsConfiguration::Reinitialize(this, ssp);

    // Clear any debug rects
    if (mDebugOverlay != NULL) {
        mDebugOverlay->Clear();
    }

    // Update the recent tasks
    UpdateRecentsTasks(intent);
    return NOERROR;
}

ECode RecentsActivity::OnStart()
{
    Activity::OnStart();

    // Register the broadcast receiver to handle messages from our service
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IAlternateRecentsComponent::ACTION_HIDE_RECENTS_ACTIVITY);
    filter->AddAction(IAlternateRecentsComponent::ACTION_TOGGLE_RECENTS_ACTIVITY);
    filter->AddAction(IAlternateRecentsComponent::ACTION_START_ENTER_ANIMATION);
    AutoPtr<IIntent> stickyIntent;
    RegisterReceiver(mServiceBroadcastReceiver, filter, (IIntent**)&stickyIntent);

    // Register any broadcast receivers for the task loader
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    rtl->RegisterReceivers(this, (IPackageCallbacks*)mRecentsView.Get());
    return NOERROR;
}

ECode RecentsActivity::OnResume()
{
    Activity::OnResume();

    // Mark Recents as visible
    mVisible = TRUE;
    return NOERROR;
}

ECode RecentsActivity::OnStop()
{
    Activity::OnStop();

    // Remove all the views
    mRecentsView->RemoveAllTaskStacks();

    // Unregister the RecentsService receiver
    UnregisterReceiver(mServiceBroadcastReceiver);

    // Unregister any broadcast receivers for the task loader
    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    rtl->UnregisterReceivers();
    return NOERROR;
}

ECode RecentsActivity::OnDestroy()
{
    Activity::OnDestroy();

    // Unregister the system broadcast receivers
    UnregisterReceiver(mSystemBroadcastReceiver);

    // Stop listening for widget package changes if there was one bound
    Boolean result;
    mAppWidgetHost->IsListening(&result);
    if (result) {
        mAppWidgetHost->StopListening();
    }
    return NOERROR;
}

ECode RecentsActivity::OnTrimMemory(
    /* [in] */ Int32 level)
{
    AutoPtr<RecentsTaskLoader> loader = RecentsTaskLoader::GetInstance();
    if (loader != NULL) {
        loader->OnTrimMemory(level);
    }
    return NOERROR;
}

ECode RecentsActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    switch (keyCode) {
        case IKeyEvent::KEYCODE_TAB: {
            AutoPtr<ISystem> sys;
            CSystem::AcquireSingleton((ISystem**)&sys);
            Int64 value;
            sys->GetCurrentTimeMillis(&value);
            Boolean hasRepKeyTimeElapsed = (value -
                    mLastTabKeyEventTime) > mConfig->mAltTabKeyDelay;
            Int32 repeatCount;
            event->GetRepeatCount(&repeatCount);
            if (repeatCount <= 0 || hasRepKeyTimeElapsed) {
                // Focus the next task in the stack
                Boolean backward;
                event->IsShiftPressed(&backward);
                mRecentsView->FocusNextTask(!backward);
                sys->GetCurrentTimeMillis(&value);
                mLastTabKeyEventTime =value;
            }
            *result = TRUE;
            return NOERROR;
        }
        case IKeyEvent::KEYCODE_DPAD_UP: {
            mRecentsView->FocusNextTask(TRUE);
            *result = TRUE;
            return NOERROR;
        }
        case IKeyEvent::KEYCODE_DPAD_DOWN: {
            mRecentsView->FocusNextTask(FALSE);
            *result = TRUE;
            return NOERROR;
        }
        case IKeyEvent::KEYCODE_DEL:
        case IKeyEvent::KEYCODE_FORWARD_DEL: {
            mRecentsView->DismissFocusedTask();
            *result = TRUE;
            return NOERROR;
        }
        default:
            break;
    }
    // Pass through the debug trigger
    mDebugTrigger->OnKeyEvent(keyCode);
    return Activity::OnKeyDown(keyCode, event, result);
}

ECode RecentsActivity::OnUserInteraction()
{
    return mRecentsView->OnUserInteraction();
}

ECode RecentsActivity::OnBackPressed()
{
    // Test mode where back does not do anything
    if (mConfig->mDebugModeEnabled) return E_NULL_POINTER_EXCEPTION;

    // Dismiss Recents to the focused Task or Home
    Boolean b;
    DismissRecentsToFocusedTaskOrHome(TRUE, &b);
    return NOERROR;
}

ECode RecentsActivity::OnDebugModeTriggered()
{
    if (mConfig->mDeveloperOptionsEnabled) {
        String pkgName;
        GetPackageName(&pkgName);
        AutoPtr<ISharedPreferences> settings;
        GetSharedPreferences(pkgName, 0, (ISharedPreferences**)&settings);
        Boolean b;
        settings->GetBoolean(Constants::Values::App::Key_DebugModeEnabled, FALSE, &b);
        if (b) {
            // Disable the debug mode
            AutoPtr<ISharedPreferencesEditor> editor;
            settings->Edit((ISharedPreferencesEditor**)&editor);
            editor->Remove(Constants::Values::App::Key_DebugModeEnabled);
            editor->Apply();
            mConfig->mDebugModeEnabled = FALSE;
            InflateDebugOverlay();
            mDebugOverlay->Disable();
        }
        else {
            // Enable the debug mode
            AutoPtr<ISharedPreferencesEditor> editor;
            settings->Edit((ISharedPreferencesEditor**)&editor);
            editor->PutBoolean(Constants::Values::App::Key_DebugModeEnabled, TRUE);
            editor->Apply();
            mConfig->mDebugModeEnabled = TRUE;
            InflateDebugOverlay();
            mDebugOverlay->Enable();
        }
        AutoPtr<IToastHelper> th;
        CToastHelper::AcquireSingleton((IToastHelper**)&th);
        StringBuilder sb("Debug mode (");
        sb += Constants::Values::App::DebugModeVersion;
        sb += ") ";
        if (mConfig->mDebugModeEnabled) {
            sb += "Enabled";
        }
        else {
            sb += "Disabled";
        }
        sb += ", please restart Recents now";
        AutoPtr<ICharSequence> cs;
        CString::New(sb.ToString(), (ICharSequence**)&cs);
        AutoPtr<IToast> toast;
        th->MakeText(this, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return NOERROR;
}

ECode RecentsActivity::OnEnterAnimationTriggered()
{
    // Animate the SystemUI scrim views
    mScrimViews->StartEnterRecentsAnimation();
    return NOERROR;
}

ECode RecentsActivity::OnExitToHomeAnimationTriggered()
{
    // Animate the SystemUI scrim views out
    mScrimViews->StartExitRecentsAnimation();
    return NOERROR;
}

ECode RecentsActivity::OnTaskViewClicked()
{
    // Mark recents as no longer visible
    AlternateRecentsComponent::NotifyVisibilityChanged(FALSE);
    mVisible = FALSE;
    return NOERROR;
}

ECode RecentsActivity::OnTaskLaunchFailed()
{
    // Return to Home
    return DismissRecentsToHomeRaw(TRUE);
}

ECode RecentsActivity::OnAllTaskViewsDismissed()
{
    return mFinishLaunchHomeRunnable->Run();
}

ECode RecentsActivity::RefreshSearchWidget()
{
    BindSearchBarAppWidget();
    AddSearchBarAppWidgetView();
    return NOERROR;
}

ECode RecentsActivity::OnPrimarySeekBarChanged(
    /* [in] */ Float progress)
{
    // Do nothing
    return NOERROR;
}

ECode RecentsActivity::OnSecondarySeekBarChanged(
    /* [in] */ Float progress)
{
    // Do nothing
    return NOERROR;
}

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos