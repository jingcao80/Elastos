
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/policy/UserSwitcherController.h"
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/database/DataSetObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::Keyguard::AppearAnimationUtils;
using Elastos::Droid::SystemUI::StatusBar::Phone::IKeyguardStatusBarView;
using Elastos::Droid::SystemUI::StatusBar::Phone::INotificationPanelView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewStub;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Manages the user switcher on the Keyguard.
 */
class KeyguardUserSwitcher
    : public Object
    , public IKeyguardUserSwitcher
{
public:
    class Adapter
        : public UserSwitcherController::BaseUserAdapter
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        Adapter(
            /* [in] */ IContext* context,
            /* [in] */ IUserSwitcherController* controller,
            /* [in] */ KeyguardUserSwitcher* host);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<IContext> mContext;
        KeyguardUserSwitcher* mHost;
    };

    class _DataSetObserver: public DataSetObserver
    {
    public:
        _DataSetObserver(
            /* [in] */ KeyguardUserSwitcher* host);

        // @Override
        CARAPI OnChanged();

        TO_STRING_IMPL("KeyguardUserSwitcher::_DataSetObserver")

    private:
        KeyguardUserSwitcher* mHost;
    };

private:
    class BgAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        BgAnimatorListenerAdapter(
            /* [in] */ KeyguardUserSwitcher* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    public:
        KeyguardUserSwitcher* mHost;
    };

    class DisappearAnimationRunnable: public Runnable
    {
    public:
        DisappearAnimationRunnable(
            /* [in] */ KeyguardUserSwitcher* host);

        // @Override
        CARAPI Run();

    private:
        KeyguardUserSwitcher* mHost;
    };

    class AppearAnimationRunnable: public Runnable
    {
    public:
        AppearAnimationRunnable(
            /* [in] */ KeyguardUserSwitcher* host);

        // @Override
        CARAPI Run();

    private:
        KeyguardUserSwitcher* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    KeyguardUserSwitcher(
        /* [in] */ IContext* context,
        /* [in] */ IViewStub* userSwitcher,
        /* [in] */ IKeyguardStatusBarView* statusBarView,
        /* [in] */ INotificationPanelView* panelView,
        /* [in] */ IUserSwitcherController* userSwitcherController);

    CARAPI SetKeyguard(
        /* [in] */ Boolean keyguard,
        /* [in] */ Boolean animate);

    CARAPI Show(
        /* [in] */ Boolean animate);

    CARAPI Hide(
        /* [in] */ Boolean animate);

private:
    /**
     * @return TRUE if the user switcher should be expanded by default on the lock screen.
     * @see android.os.UserManager#isUserSwitcherEnabled()
     */
    CARAPI_(Boolean) ShouldExpandByDefault();

    CARAPI_(void) CancelAnimations();

    CARAPI_(void) StartAppearAnimation();

    CARAPI_(void) StartDisappearAnimation();

    CARAPI_(void) Refresh();

private:
    static const String TAG;
    static const Boolean ALWAYS_ON;

    AutoPtr<IViewGroup> mUserSwitcher;
    AutoPtr<IKeyguardStatusBarView> mStatusBarView;
    AutoPtr<Adapter> mAdapter;
    AutoPtr<AppearAnimationUtils> mAppearAnimationUtils;
    AutoPtr<IKeyguardUserSwitcherScrim> mBackground;
    AutoPtr<IObjectAnimator> mBgAnimator;
    AutoPtr<IUserSwitcherController> mUserSwitcherController;
    AutoPtr<DataSetObserver> mDataSetObserver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHER_H__
