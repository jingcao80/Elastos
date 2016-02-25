
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/** Manages the scrims for the various system bars. */
class SystemBarScrimViews : public Object
{
private:
    class StartAcitonRunnable : public Runnable
    {
    public:
        StartAcitonRunnable(
            /* [in] */ IView* view);

        CARAPI Run();

    private:
        AutoPtr<IView> mView;
    };

public:
    SystemBarScrimViews(
        /* [in] */ IActivity* activity,
        /* [in] */ RecentsConfiguration* config);

    /**
     * Prepares the scrim views for animating when entering Recents. This will be called before
     * the first draw.
     */
    CARAPI_(void) PrepareEnterRecentsAnimation();

    /**
     * Starts animating the scrim views when entering Recents.
     */
    CARAPI_(void) StartEnterRecentsAnimation();

    /**
     * Starts animating the scrim views when leaving Recents (either via launching a task, or
     * going home).
     */
    CARAPI_(void) StartExitRecentsAnimation();

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<IView> mStatusBarScrimView;
    AutoPtr<IView> mNavBarScrimView;

    Boolean mHasNavBarScrim;
    Boolean mShouldAnimateStatusBarScrim;
    Boolean mHasStatusBarScrim;
    Boolean mShouldAnimateNavBarScrim;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_SYSTEMBARSCRIMVIEWS_H__
