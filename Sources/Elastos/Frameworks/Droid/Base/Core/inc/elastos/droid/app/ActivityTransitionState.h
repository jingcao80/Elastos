
#ifndef __ELASTOS_DROID_APP_ACTIVITY_TRANSITION_STATE_H__
#define __ELASTOS_DROID_APP_ACTIVITY_TRANSITION_STATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * This class contains all persistence-related functionality for Activity Transitions.
 * Activities start exit and enter Activity Transitions through this class.
 */
class ActivityTransitionState
    : public Object
    , public IActivityTransitionState
{
public:
    CAR_INTERFACE_DECL()

    ActivityTransitionState();

    virtual ~ActivityTransitionState();

    CARAPI constructor();

    CARAPI AddExitTransitionCoordinator(
        /* [in] */ IExitTransitionCoordinator* exitTransitionCoordinator,
        /* [out] */ Int32* result);

    CARAPI ReadState(
        /* [in] */ IBundle* bundle);

    CARAPI SaveState(
        /* [in] */ IBundle* bundle);

    CARAPI SetEnterActivityOptions(
        /* [in] */ IActivity* activity,
        /* [in] */ IActivityOptions* options);

    CARAPI EnterReady(
        /* [in] */ IActivity* activity);

    CARAPI PostponeEnterTransition();

    CARAPI StartPostponedEnterTransition();

    CARAPI OnStop();

    CARAPI OnResume();

    CARAPI Clear();

    CARAPI StartExitBackTransition(
        /* [in] */ IActivity* activity,
        /* [out] */ Boolean* result);

    CARAPI StartExitOutTransition(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* options);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) StartEnter();

    CARAPI_(void) RestoreExitedViews();

private:
    static const String ENTERING_SHARED_ELEMENTS;
    static const String EXITING_MAPPED_FROM;
    static const String EXITING_MAPPED_TO;

    /**
     * The shared elements that the calling Activity has said that they transferred to this
     * Activity.
     */
    AutoPtr<IArrayList> mEnteringNames; //ArrayList<String>

    /**
     * The names of shared elements that were shared to the called Activity.
     */
    AutoPtr<IArrayList> mExitingFrom;//ArrayList<String>

    /**
     * The names of local Views that were shared out, mapped to those elements in mExitingFrom.
     */
    AutoPtr<IArrayList> mExitingTo;//ArrayList<String>

    /**
     * The local Views that were shared out, mapped to those elements in mExitingFrom.
     */
    AutoPtr<IArrayList> mExitingToView;//ArrayList<View>

    /**
     * The ExitTransitionCoordinator used to start an Activity. Used to make the elements restore
     * Visibility of exited Views.
     */
    AutoPtr<IExitTransitionCoordinator> mCalledExitCoordinator;

    /**
     * The ExitTransitionCoordinator used to return to a previous Activity when called with
     * {@link android.app.Activity#finishAfterTransition()}.
     */
    AutoPtr<IExitTransitionCoordinator> mReturnExitCoordinator;

    /**
     * We must be able to cancel entering transitions to stop changing the Window to
     * opaque when we exit before making the Window opaque.
     */
    AutoPtr<IEnterTransitionCoordinator> mEnterTransitionCoordinator;

    /**
     * ActivityOptions used on entering this Activity.
     */
    AutoPtr<IActivityOptions> mEnterActivityOptions;

    /**
     * Has an exit transition been started? If so, we don't want to double-exit.
     */
    Boolean mHasExited;

    /**
     * Postpone painting and starting the enter transition until this is false.
     */
    Boolean mIsEnterPostponed;

    /**
     * Potential exit transition coordinators.
     */
    AutoPtr<HashMap<Int32, AutoPtr<IWeakReference> > > mExitTransitionCoordinators;
    //SparseArray<WeakReference<ExitTransitionCoordinator>> mExitTransitionCoordinators;

    /**
     * Next key for mExitTransitionCoordinator.
     */
    Int32 mExitTransitionCoordinatorsKey;

    Boolean mIsEnterTriggered;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITY_TRANSITION_STATE_H__
