
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "_Elastos.Droid.Core.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Keyguard::IViewMediatorCallback;
using Elastos::Droid::Keyguard::KeyguardHostView::IOnDismissAction;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A class which manages the bouncer on the lockscreen.
 */
class KeyguardBouncer: public Object
{
private:
    class ShowRunnable: public Runnable
    {
    public:
        ShowRunnable(
            /* [in] */ KeyguardBouncer* host);

        // @Override
        CARAPI Run();

    private:
        KeyguardBouncer* mHost;
    };

public:
    KeyguardBouncer(
        /* [in] */ IContext* context,
        /* [in] */ IViewMediatorCallback* callback,
        /* [in] */ ILockPatternUtils* lockPatternUtils,
        /* [in] */ IStatusBarWindowManager* windowManager,
        /* [in] */ IViewGroup* container);

    CARAPI_(void) Show();

    CARAPI_(void) ShowWithDismissAction(
        /* [in] */ IOnDismissAction* r);

    CARAPI_(void) Hide(
        /* [in] */ Boolean destroyView);

    /**
     * See {@link StatusBarKeyguardViewManager#startPreHideAnimation}.
     */
    CARAPI_(void) StartPreHideAnimation(
        /* [in] */ IRunnable* runnable);

    /**
     * Reset the state of the view.
     */
    CARAPI_(void) Reset();

    CARAPI_(void) OnScreenTurnedOff();

    CARAPI_(Int64) GetUserActivityTimeout();

    CARAPI_(Boolean) IsShowing();

    CARAPI_(void) Prepare();

    CARAPI_(Boolean) OnBackPressed();

    /**
     * @return True if and only if the current security method should be shown before showing
     *         the notifications on Keyguard, like SIM PIN/PUK.
     */
    CARAPI_(Boolean) NeedsFullscreenBouncer();

    CARAPI_(Boolean) IsSecure();

    CARAPI_(Boolean) OnMenuPressed();

    CARAPI_(Boolean) InterceptMediaKey(
        /* [in] */ IKeyEvent* event);

private:
    CARAPI_(void) CancelShowRunnable();

    CARAPI_(void) EnsureView();

    CARAPI_(void) InflateView();

    CARAPI_(void) RemoveView();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IViewMediatorCallback> mCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IViewGroup> mContainer;
    AutoPtr<IStatusBarWindowManager> mWindowManager;
    // AutoPtr<IKeyguardViewBase> mKeyguardView;
    AutoPtr<IViewGroup> mRoot;
    Boolean mShowingSoon;
    AutoPtr<IChoreographer> mChoreographer;
    AutoPtr<IRunnable> mShowRunnable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDBOUNCER_H__
