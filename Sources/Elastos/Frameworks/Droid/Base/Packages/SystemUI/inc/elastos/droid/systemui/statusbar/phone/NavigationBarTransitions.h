
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARTRANSITIONS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARTRANSITIONS_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class NavigationBarTransitions: public BarTransitions
{
private:
    class LightsOutListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL();

        LightsOutListener(
            /* [in] */ NavigationBarTransitions* host);

        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

    private:
        NavigationBarTransitions* mHost;
    };

    class AnimatorListener : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener(
            /* [in] */ IView* lowLights);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* a);

    private:
        AutoPtr<IView> mLowLights;
    };

public:
    NavigationBarTransitions(
        /* [in] */ INavigationBarView* view);

    CARAPI_(void) Init(
        /* [in] */ Boolean isVertical);

    CARAPI_(void) SetVertical(
        /* [in] */ Boolean isVertical);

    // @Override
    CARAPI TransitionTo(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate);

    CARAPI_(void) ApplyBackButtonQuiescentAlpha(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate);

protected:
    // @Override
    CARAPI_(void) OnTransition(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

private:
    CARAPI_(void) ApplyMode(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean force);

    CARAPI_(Float) AlphaForMode(
        /* [in] */ Int32 mode);

    static CARAPI_(Float) MaxVisibleQuiescentAlpha(
        /* [in] */ Float max,
        /* [in] */ IView* v);

    CARAPI_(void) SetKeyButtonViewQuiescentAlpha(
        /* [in] */ IView* button,
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate);

    CARAPI_(void) ApplyLightsOut(
        /* [in] */ Boolean lightsOut,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean force);

private:
    static const Int32 CONTENT_FADE_DURATION;

    AutoPtr<INavigationBarView> mView;
    AutoPtr<IIStatusBarService> mBarService;

    Boolean mLightsOut;
    Boolean mVertical;
    Int32 mRequestedMode;
    AutoPtr<IViewOnTouchListener> mLightsOutListener;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARTRANSITIONS_H__
