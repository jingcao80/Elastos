
#include "elastos/droid/systemui/statusbar/phone/NavigationBarTransitions.h"
#include "../../R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyButtonView;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(NavigationBarTransitions::LightsOutListener, Object, IViewOnTouchListener)
NavigationBarTransitions::LightsOutListener::LightsOutListener(
    /* [in] */ NavigationBarTransitions* host)
    : mHost(host)
{}

ECode NavigationBarTransitions::LightsOutListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        // even though setting the systemUI visibility below will turn these views
        // on, we need them to come up faster so that they can catch this motion
        // event
        mHost->ApplyLightsOut(FALSE, FALSE, FALSE);

        // try {
        mHost->mBarService->SetSystemUiVisibility(0, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
        // } catch (android.os.RemoteException ex) {
        // }
    }
    *result = FALSE;
    return NOERROR;
}

NavigationBarTransitions::AnimatorListener::AnimatorListener(
    /* [in] */ IView* lowLights)
    : mLowLights(lowLights)
{}

ECode NavigationBarTransitions::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* a)
{
    mLowLights->SetVisibility(IView::GONE);
    return NOERROR;
}

const Int32 NavigationBarTransitions::CONTENT_FADE_DURATION = 200;
NavigationBarTransitions::NavigationBarTransitions(
    /* [in] */ INavigationBarView* view)
    : BarTransitions(IView::Probe(view), R::drawable::nav_background)
    , mLightsOut(FALSE)
    , mVertical(FALSE)
    , mRequestedMode(0)
{
    mLightsOutListener = new LightsOutListener(this);
    mView = view;
    mBarService = IIStatusBarService::Probe(ServiceManager::GetService(IContext::STATUS_BAR_SERVICE));
}

void NavigationBarTransitions::Init(
    /* [in] */ Boolean isVertical)
{
    SetVertical(isVertical);
    Int32 mode = 0;
    GetMode(&mode);
    ApplyModeBackground(-1, mode, FALSE /*animate*/);
    ApplyMode(mode, FALSE /*animate*/, TRUE /*force*/);
}

void NavigationBarTransitions::SetVertical(
    /* [in] */ Boolean isVertical)
{
    mVertical = isVertical;
    TransitionTo(mRequestedMode, FALSE /*animate*/);
}

ECode NavigationBarTransitions::TransitionTo(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate)
{
    mRequestedMode = mode;
    if (mVertical && (mode == MODE_TRANSLUCENT || mode == MODE_TRANSPARENT)) {
        // translucent mode not allowed when vertical
        mode = MODE_OPAQUE;
    }
    BarTransitions::TransitionTo(mode, animate);
    return NOERROR;
}

void NavigationBarTransitions::OnTransition(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    BarTransitions::OnTransition(oldMode, newMode, animate);
    ApplyMode(newMode, animate, FALSE /*force*/);
}

void NavigationBarTransitions::ApplyMode(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean force)
{
    // apply to key buttons
    const Float alpha = AlphaForMode(mode);
    AutoPtr<IView> view;
    SetKeyButtonViewQuiescentAlpha((mView->GetHomeButton((IView**)&view), view), alpha, animate);
    view = NULL;
    SetKeyButtonViewQuiescentAlpha((mView->GetRecentsButton((IView**)&view), view), alpha, animate);
    view = NULL;
    SetKeyButtonViewQuiescentAlpha((mView->GetMenuButton((IView**)&view), view), alpha, animate);
    view = NULL;
    SetKeyButtonViewQuiescentAlpha((mView->GetImeSwitchButton((IView**)&view), view), alpha, animate);

    ApplyBackButtonQuiescentAlpha(mode, animate);

    // apply to lights out
    ApplyLightsOut(mode == MODE_LIGHTS_OUT, animate, force);
}

Float NavigationBarTransitions::AlphaForMode(
    /* [in] */ Int32 mode)
{
    const Boolean isOpaque = mode == MODE_OPAQUE || mode == MODE_LIGHTS_OUT;
    return isOpaque ? IKeyButtonView::DEFAULT_QUIESCENT_ALPHA : 1.f;
}

void NavigationBarTransitions::ApplyBackButtonQuiescentAlpha(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate)
{
    Float backAlpha = 0;
    AutoPtr<IView> view;
    backAlpha = MaxVisibleQuiescentAlpha(backAlpha, (mView->GetHomeButton((IView**)&view), view));
    view = NULL;
    backAlpha = MaxVisibleQuiescentAlpha(backAlpha, (mView->GetRecentsButton((IView**)&view), view));
    view = NULL;
    backAlpha = MaxVisibleQuiescentAlpha(backAlpha, (mView->GetMenuButton((IView**)&view), view));
    view = NULL;
    backAlpha = MaxVisibleQuiescentAlpha(backAlpha, (mView->GetImeSwitchButton((IView**)&view), view));
    if (backAlpha > 0) {
        view = NULL;
        mView->GetBackButton((IView**)&view);
        SetKeyButtonViewQuiescentAlpha(view, backAlpha, animate);
    }
}

Float NavigationBarTransitions::MaxVisibleQuiescentAlpha(
    /* [in] */ Float max,
    /* [in] */ IView* v)
{
    Boolean shown = FALSE;
    if (IKeyButtonView::Probe(v) && (v->IsShown(&shown), shown)) {
        Float alpha = 0;
        IKeyButtonView::Probe(v)->GetQuiescentAlpha(&alpha);
        return Elastos::Core::Math::Max(max, alpha);
    }
    return max;
}

void NavigationBarTransitions::SetKeyButtonViewQuiescentAlpha(
    /* [in] */ IView* button,
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate)
{
    if (IKeyButtonView::Probe(button)) {
        IKeyButtonView::Probe(button)->SetQuiescentAlpha(alpha, animate);
    }
}

void NavigationBarTransitions::ApplyLightsOut(
    /* [in] */ Boolean lightsOut,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean force)
{
    if (!force && lightsOut == mLightsOut) return;

    mLightsOut = lightsOut;

    AutoPtr<IView> view;
    mView->GetCurrentView((IView**)&view);
    AutoPtr<IView> navButtons;
    view->FindViewById(R::id::nav_buttons, (IView**)&navButtons);
    AutoPtr<IView> lowLights;
    view->FindViewById(R::id::lights_out, (IView**)&lowLights);

    // ok, everyone, stop it right there
    AutoPtr<IViewPropertyAnimator> a;
    navButtons->Animate((IViewPropertyAnimator**)&a);
    a->Cancel();
    a = NULL;
    lowLights->Animate((IViewPropertyAnimator**)&a);
    a->Cancel();

    const Float navButtonsAlpha = lightsOut ? 0.f : 1.f;
    const Float lowLightsAlpha = lightsOut ? 1.f : 0.f;

    if (!animate) {
        navButtons->SetAlpha(navButtonsAlpha);
        lowLights->SetAlpha(lowLightsAlpha);
        lowLights->SetVisibility(lightsOut ? IView::VISIBLE : IView::GONE);
    }
    else {
        const Int32 duration = lightsOut ? LIGHTS_OUT_DURATION : LIGHTS_IN_DURATION;
        a = NULL;
        navButtons->Animate((IViewPropertyAnimator**)&a);
        a->Alpha(navButtonsAlpha);
        a->SetDuration(duration);
        a->Start();

        lowLights->SetOnTouchListener(mLightsOutListener);
        Int32 v = 0;
        lowLights->GetVisibility(&v);
        if (v == IView::GONE) {
            lowLights->SetAlpha(0.f);
            lowLights->SetVisibility(IView::VISIBLE);
        }

        a = NULL;
        lowLights->Animate((IViewPropertyAnimator**)&a);
        a->Alpha(lowLightsAlpha);
        a->SetDuration(duration);
        AutoPtr<ITimeInterpolator> ta;
        CAccelerateInterpolator::New(2.0f, (ITimeInterpolator**)&ta);
        a->SetInterpolator(ta);
        AutoPtr<AnimatorListener> listener = new AnimatorListener(lowLights);
        a->SetListener(lightsOut ? NULL : listener.Get());
        a->Start();
    }
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
