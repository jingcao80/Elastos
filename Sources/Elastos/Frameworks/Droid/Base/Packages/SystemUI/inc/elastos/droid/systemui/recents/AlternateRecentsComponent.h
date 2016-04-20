
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_ALTERNATERECENTSCOMPONENT_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_ALTERNATERECENTSCOMPONENT_H__

#include "_SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/systemui/recents/views/TaskStackView.h"
#include "elastos/droid/systemui/recents/views/TaskViewTransform.h"

using Elastos::Droid::App::IActivityManagerRunningTaskInfo;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::IRecentsComponentCallbacks;
using Elastos::Droid::SystemUI::Recents::RecentsConfiguration;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::Recents::Views::ITaskViewHeader;
using Elastos::Droid::SystemUI::Recents::Views::TaskStackView;
using Elastos::Droid::SystemUI::Recents::Views::TaskViewTransform;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

/** A proxy implementation for the recents component */
class AlternateRecentsComponent
    : public Object
    , public IActivityOptionsOnAnimationStartedListener
    , public IAlternateRecentsComponent
{
private:
    class MR
        : public Runnable
    {
    public:
        MR(
            /* [in] */ AlternateRecentsComponent* host);

        CARAPI Run();

    private:
        AlternateRecentsComponent* mHost;
    };

    class BR
        : public BroadcastReceiver
    {
    public:
        BR(
            /* [in] */ AlternateRecentsComponent* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AlternateRecentsComponent* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AlternateRecentsComponent(
        /* [in] */ IContext* context);

    CARAPI OnStart();

    CARAPI OnBootCompleted();

    /** Shows the recents */
    CARAPI OnShowRecents(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ IView* statusBarView);

    /** Hides the recents */
    CARAPI OnHideRecents(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    /** Toggles the alternate recents activity */
    CARAPI OnToggleRecents(
        /* [in] */ IView* statusBarView);

    CARAPI OnPreloadRecents();

    CARAPI OnCancelPreloadingRecents();

    CARAPI ShowRelativeAffiliatedTask(
        /* [in] */ Boolean showNextTask);

    CARAPI OnShowNextAffiliatedTask();

    CARAPI OnShowPrevAffiliatedTask();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /** Prepares the header bar layout. */
    CARAPI ReloadHeaderBarLayout();

    /** Gets the top task. */
    CARAPI GetTopMostTask(
        /* [out] */ IActivityManagerRunningTaskInfo** topMostTask);

    /** Returns whether the recents is currently running */
    CARAPI IsRecentsTopMost(
        /* [in] */ IActivityManagerRunningTaskInfo* topTask,
        /* [in] */ IAtomicBoolean* isHomeTopMost,
        /* [out] */ Boolean* result);

    /** Toggles the recents activity */
    CARAPI ToggleRecentsActivity();

    /** Starts the recents activity if it is not already running */
    CARAPI StartRecentsActivity();

    /**
     * Creates the activity options for a unknown state->recents transition.
     */
    CARAPI GetUnknownTransitionActivityOptions(
        /* [out] */ IActivityOptions** ao);

    /**
     * Creates the activity options for a home->recents transition.
     */
    CARAPI GetHomeTransitionActivityOptions(
        /* [in] */ Boolean fromSearchHome,
        /* [out] */ IActivityOptions** ao);

    /**
     * Creates the activity options for an app->recents transition.  If this method sets the static
     * screenshot, then we will use that for the transition.
     */
    CARAPI GetThumbnailTransitionActivityOptions(
        /* [in] */ IActivityManagerRunningTaskInfo* topTask,
        /* [in] */ Boolean isTopTaskHome,
        /* [out] */ IActivityOptions** ao);

    /** Returns the transition rect for the given task id. */
    CARAPI GetThumbnailTransitionTransform(
        /* [in] */ Int32 runningTaskId,
        /* [in] */ Boolean isTopTaskHome,
        /* [in] */ ITask* runningTaskOut,
        /* [out] */ TaskViewTransform** ttt);

    /** Starts the recents activity */
    CARAPI StartRecentsActivity(
        /* [in] */ IActivityManagerRunningTaskInfo* topTask,
        /* [in] */ Boolean isTopTaskHome);

    /** Starts the recents activity */
    CARAPI StartAlternateRecentsActivity(
        /* [in] */ IActivityManagerRunningTaskInfo* topTask,
        /* [in] */ IActivityOptions* opts,
        /* [in] */ const String& extraFlag);

    /** Returns the last screenshot taken, this will be called by the RecentsActivity. */
    static CARAPI GetLastScreenshot(
        /* [out] */ IBitmap** lss);

    /** Recycles the last screenshot taken, this will be called by the RecentsActivity. */
    static CARAPI ConsumeLastScreenshot();

    /** Sets the RecentsComponent callbacks. */
    CARAPI SetRecentsComponentCallback(
        /* [in] */ IRecentsComponentCallbacks* cb);

    /** Notifies the callbacks that the visibility of Recents has changed. */
    static CARAPI NotifyVisibilityChanged(
        /* [in] */ Boolean visible);

    /**** OnAnimationStartedListener Implementation ****/

    // @Override
    CARAPI OnAnimationStarted();

public:
    const static String EXTRA_FROM_HOME;
    const static String EXTRA_FROM_SEARCH_HOME;
    const static String EXTRA_FROM_APP_THUMBNAIL;
    const static String EXTRA_FROM_APP_FULL_SCREENSHOT;
    const static String EXTRA_FROM_TASK_ID;
    const static String EXTRA_TRIGGERED_FROM_ALT_TAB;
    const static String EXTRA_TRIGGERED_FROM_HOME_KEY;

    const static String ACTION_START_ENTER_ANIMATION;
    const static String ACTION_TOGGLE_RECENTS_ACTIVITY;
    const static String ACTION_HIDE_RECENTS_ACTIVITY;

    const static Int32 sMinToggleDelay = 350;

    const static String sToggleRecentsAction;
    const static String sRecentsPackage;
    const static String sRecentsActivity;

    static AutoPtr<IBitmap> sLastScreenshot;
    static AutoPtr<IRecentsComponentCallbacks> sRecentsComponentCallbacks;

    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<SystemServicesProxy> mSystemServicesProxy;
    AutoPtr<IHandler> mHandler;
    Boolean mBootCompleted;
    Boolean mStartAnimationTriggered;

    // Task launching
    AutoPtr<RecentsConfiguration> mConfig;
    AutoPtr<IRect> mWindowRect;
    AutoPtr<IRect> mTaskStackBounds;
    AutoPtr<IRect> mSystemInsets;
    AutoPtr<TaskViewTransform> mTmpTransform;
    Int32 mStatusBarHeight;
    Int32 mNavBarHeight;
    Int32 mNavBarWidth;

    // Header (for transition)
    AutoPtr<ITaskViewHeader> mHeaderBar;
    AutoPtr<TaskStackView> mDummyStackView;

    // Variables to keep track of if we need to start recents after binding
    AutoPtr<IView> mStatusBarView;
    Boolean mTriggeredFromAltTab;
    Int64 mLastToggleTime;
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_ALTERNATERECENTSCOMPONENT_H__