
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSACTIVITY_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/recents/AlternateRecentsComponent.h"
#include "elastos/droid/systemui/recents/RecentsAppWidgetHost.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/misc/DebugTrigger.h"
#include "elastos/droid/systemui/recents/misc/ReferenceCountedTrigger.h"
#include "elastos/droid/systemui/recents/views/DebugOverlayView.h"
#include "elastos/droid/systemui/recents/views/RecentsView.h"
#include "elastos/droid/systemui/recents/views/SystemBarScrimViews.h"
#include "elastos/droid/systemui/recents/views/ViewAnimation.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::Recents::AlternateRecentsComponent;
using Elastos::Droid::SystemUI::Recents::IAlternateRecentsComponent;
using Elastos::Droid::SystemUI::Recents::IRecentsAppWidgetHostCallbacks;
using Elastos::Droid::SystemUI::Recents::RecentsAppWidgetHost;
using Elastos::Droid::SystemUI::Recents::RecentsConfiguration;
using Elastos::Droid::SystemUI::Recents::Misc::DebugTrigger;
using Elastos::Droid::SystemUI::Recents::Misc::ReferenceCountedTrigger;
using Elastos::Droid::SystemUI::Recents::Views::DebugOverlayView;
using Elastos::Droid::SystemUI::Recents::Views::IDebugOverlayView;
using Elastos::Droid::SystemUI::Recents::Views::IDebugOverlayViewCallbacks;
using Elastos::Droid::SystemUI::Recents::Views::IRecentsView;
using Elastos::Droid::SystemUI::Recents::Views::IRecentsViewCallbacks;
using Elastos::Droid::SystemUI::Recents::Views::RecentsView;
using Elastos::Droid::SystemUI::Recents::Views::SystemBarScrimViews;
using Elastos::Droid::SystemUI::Recents::Views::ViewAnimation;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewStub;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

/**
 * The main Recents activity that is started from AlternateRecentsComponent.
 */
class RecentsActivity
    : public Activity
    , public IRecentsActivity
{
public:
    class ServiceBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("RecentsActivity::ServiceBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IRecentsActivity* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        RecentsActivity* mHost;
    };

    class SystemBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("RecentsActivity::SystemBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IRecentsActivity* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        RecentsActivity* mHost;
    };

private:
    class InnerCallback
        : public Object
        , public IRecentsViewCallbacks
        , public IDebugOverlayViewCallbacks
        , public IRecentsAppWidgetHostCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCallback(
            /* [in] */ RecentsActivity* host);

        // @Override
        CARAPI OnExitToHomeAnimationTriggered();

        // @Override
        CARAPI OnTaskViewClicked();

        // @Override
        CARAPI OnTaskLaunchFailed();

        // @Override
        CARAPI OnAllTaskViewsDismissed();

        CARAPI OnPrimarySeekBarChanged(
            /* [in] */ Float progress);

        // @Override
        CARAPI OnSecondarySeekBarChanged(
            /* [in] */ Float progress);

        CARAPI RefreshSearchWidget();

    private:
        RecentsActivity* mHost;
    };

    /**
     * A common Runnable to finish Recents either by calling finish() (with a custom animation) or
     * launching Home with some ActivityOptions.  Generally we always launch home when we exit
     * Recents rather than just finishing the activity since we don't know what is behind Recents in
     * the task stack.  The only case where we finish() directly is when we are cancelling the full
     * screen transition from the app.
     */
    class FinishRecentsRunnable
        : public Runnable
    {
    public:
        /**
         * Creates a finish runnable that starts the specified intent, using the given
         * ActivityOptions.
         */
        FinishRecentsRunnable(
            /* [in] */ IIntent* launchIntent,
            /* [in] */ IActivityOptions* opts,
            /* [in] */ RecentsActivity* host);

        // @Override
        CARAPI Run();

    public:
        AutoPtr<IIntent> mLaunchIntent;
        AutoPtr<IActivityOptions> mLaunchOpts;

    private:
        RecentsActivity* mHost;
    };

    class OnDebugModeTriggeredRunnable
        : public Runnable
    {
    public:
        OnDebugModeTriggeredRunnable(
            /* [in] */ RecentsActivity* host);

        CARAPI Run();

    private:
        RecentsActivity* mHost;
    };

    class AppWidgetHostCallbackRunnable
        : public Runnable
    {
    public:
        /**
         * Creates a finish runnable that starts the specified intent, using the given
         * ActivityOptions.
         */
        AppWidgetHostCallbackRunnable(
            /* [in] */ IRecentsAppWidgetHostCallbacks* callback,
            /* [in] */ RecentsActivity* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IRecentsAppWidgetHostCallbacks> mCallback;
        RecentsActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RecentsActivity();

    ~RecentsActivity();

    CARAPI constructor();

    /** Updates the set of recent tasks */
    CARAPI UpdateRecentsTasks(
        /* [in] */ IIntent* launchIntent);

    /** Attempts to allocate and bind the search bar app widget */
    CARAPI BindSearchBarAppWidget();

    /** Creates the search bar app widget view */
    CARAPI AddSearchBarAppWidgetView();

    /** Dismisses recents if we are already visible and the intent is to toggle the recents view */
    CARAPI DismissRecentsToFocusedTaskOrHome(
        /* [in] */ Boolean checkFilteredStackState,
        /* [out] */ Boolean* result);

    /** Dismisses Recents directly to Home. */
    CARAPI DismissRecentsToHomeRaw(
        /* [in] */ Boolean animated);

    /** Dismisses Recents directly to Home if we currently aren't transitioning. */
    CARAPI DismissRecentsToHome(
        /* [in] */ Boolean animated,
        /* [out] */ Boolean* result);

    /** Called with the activity is first created. */
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /** Inflates the debug overlay if debug mode is enabled. */
    CARAPI InflateDebugOverlay();

    CARAPI OnConfigurationChange();

protected:
    // @Override
    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

public:
    // @Override
    CARAPI OnUserInteraction();

    // @Override
    CARAPI OnBackPressed();

    /** Called when debug mode is triggered */
    CARAPI OnDebugModeTriggered();

    /** Called when the enter recents animation is triggered. */
    CARAPI OnEnterAnimationTriggered();

    /**** RecentsView.RecentsViewCallbacks Implementation ****/

    // @Override
    virtual CARAPI OnExitToHomeAnimationTriggered();

    // @Override
    virtual CARAPI OnTaskViewClicked();

    // @Override
    virtual CARAPI OnTaskLaunchFailed();

    // @Override
    virtual CARAPI OnAllTaskViewsDismissed();

    /**** RecentsAppWidgetHost.RecentsAppWidgetHostCallbacks Implementation ****/

    // @Override
    virtual CARAPI RefreshSearchWidget();

    /**** DebugOverlayView.DebugOverlayViewCallbacks ****/

    // @Override
    virtual CARAPI OnPrimarySeekBarChanged(
        /* [in] */ Float progress);

    // @Override
    virtual CARAPI OnSecondarySeekBarChanged(
        /* [in] */ Float progress);

public:
    AutoPtr<RecentsConfiguration> mConfig;
    Boolean mVisible;
    Int64 mLastTabKeyEventTime;

    // Top level views
    AutoPtr<RecentsView> mRecentsView;
    AutoPtr<SystemBarScrimViews> mScrimViews;
    AutoPtr<IViewStub> mEmptyViewStub;
    AutoPtr<IViewStub> mDebugOverlayStub;
    AutoPtr<IView> mEmptyView;
    AutoPtr<IDebugOverlayView> mDebugOverlay;

    // Search AppWidget
    AutoPtr<RecentsAppWidgetHost> mAppWidgetHost;
    AutoPtr<IAppWidgetProviderInfo> mSearchAppWidgetInfo;
    AutoPtr<IAppWidgetHostView> mSearchAppWidgetHostView;

    // Runnables to finish the Recents activity
    AutoPtr<FinishRecentsRunnable> mFinishLaunchHomeRunnable;

    /**
     * Broadcast receiver to handle messages from AlternateRecentsComponent.
     */
    AutoPtr<IBroadcastReceiver> mServiceBroadcastReceiver;

    /**
     * Broadcast receiver to handle messages from the system
     */
    AutoPtr<IBroadcastReceiver> mSystemBroadcastReceiver;

    /**
     * A custom debug trigger to listen for a debug key chord.
     */
    AutoPtr<DebugTrigger> mDebugTrigger;
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSACTIVITY_H__