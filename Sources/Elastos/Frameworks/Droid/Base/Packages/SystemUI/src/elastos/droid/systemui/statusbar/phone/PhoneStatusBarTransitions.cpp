
#include "elastos/droid/systemui/statusbar/phone/PhoneStatusBarTransitions.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimatorHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Float PhoneStatusBarTransitions::ICON_ALPHA_WHEN_NOT_OPAQUE = 1;
const Float PhoneStatusBarTransitions::ICON_ALPHA_WHEN_LIGHTS_OUT_BATTERY_CLOCK = 0.5f;
const Float PhoneStatusBarTransitions::ICON_ALPHA_WHEN_LIGHTS_OUT_NON_BATTERY_CLOCK = 0;
CAR_INTERFACE_IMPL(PhoneStatusBarTransitions, BarTransitions, IPhoneStatusBarTransitions);
PhoneStatusBarTransitions::PhoneStatusBarTransitions(
    /* [in] */ IPhoneStatusBarView* view)
    : BarTransitions(IView::Probe(view), R::drawable::status_background)
{
    mView = view;
    AutoPtr<IContext> ctx;
    IView::Probe(mView)->GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetFraction(R::dimen::status_bar_icon_drawing_alpha, 1, 1, &mIconAlphaWhenOpaque);
}

ECode PhoneStatusBarTransitions::Init()
{
    IView::Probe(mView)->FindViewById(R::id::notification_icon_area, (IView**)&mLeftSide);
    IView::Probe(mView)->FindViewById(R::id::statusIcons, (IView**)&mStatusIcons);
    IView::Probe(mView)->FindViewById(R::id::signal_cluster, (IView**)&mSignalCluster);
    IView::Probe(mView)->FindViewById(R::id::battery, (IView**)&mBattery);
    IView::Probe(mView)->FindViewById(R::id::clock, (IView**)&mClock);

    Int32 mode = 0;
    GetMode(&mode);
    ApplyModeBackground(-1, mode, FALSE /*animate*/);
    ApplyMode(mode, FALSE /*animate*/);
    return NOERROR;
}

ECode PhoneStatusBarTransitions::AnimateTransitionTo(
    /* [in] */ IView* v,
    /* [in] */ Float toAlpha,
    /* [out] */ IObjectAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    Float a = 0;
    v->GetAlpha(&a);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = a;
    (*fvs)[1] = toAlpha;
    return helper->OfFloat(v, String("alpha"), fvs, result);
}

Float PhoneStatusBarTransitions::GetNonBatteryClockAlphaFor(
    /* [in] */ Int32 mode)
{
    return mode == MODE_LIGHTS_OUT ? ICON_ALPHA_WHEN_LIGHTS_OUT_NON_BATTERY_CLOCK
            : !IsOpaque(mode) ? ICON_ALPHA_WHEN_NOT_OPAQUE
            : mIconAlphaWhenOpaque;
}

Float PhoneStatusBarTransitions::GetBatteryClockAlpha(
    /* [in] */ Int32 mode)
{
    return mode == MODE_LIGHTS_OUT ? ICON_ALPHA_WHEN_LIGHTS_OUT_BATTERY_CLOCK
            : GetNonBatteryClockAlphaFor(mode);
}

Boolean PhoneStatusBarTransitions::IsOpaque(
    /* [in] */ Int32 mode)
{
    return !(mode == MODE_SEMI_TRANSPARENT || mode == MODE_TRANSLUCENT
            || mode == MODE_TRANSPARENT);
}

void PhoneStatusBarTransitions::OnTransition(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    BarTransitions::OnTransition(oldMode, newMode, animate);
    ApplyMode(newMode, animate);
}

void PhoneStatusBarTransitions::ApplyMode(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate)
{
    if (mLeftSide == NULL) return; // pre-init
    Float newAlpha = GetNonBatteryClockAlphaFor(mode);
    Float newAlphaBC = GetBatteryClockAlpha(mode);
    if (mCurrentAnimation != NULL) {
        mCurrentAnimation->Cancel();
    }
    if (animate) {
        AutoPtr<IAnimatorSet> anims;
        CAnimatorSet::New((IAnimatorSet**)&anims);

        AutoPtr<ArrayOf<IAnimator*> > values = ArrayOf<IAnimator*>::Alloc(5);
        AutoPtr<IObjectAnimator> obj;
        AnimateTransitionTo(mLeftSide, newAlpha, (IObjectAnimator**)&obj);
        values->Set(0, IAnimator::Probe(obj));
        obj = NULL;
        AnimateTransitionTo(mStatusIcons, newAlpha, (IObjectAnimator**)&obj);
        values->Set(1, IAnimator::Probe(obj));
        obj = NULL;
        AnimateTransitionTo(mSignalCluster, newAlpha, (IObjectAnimator**)&obj);
        values->Set(2, IAnimator::Probe(obj));
        obj = NULL;
        AnimateTransitionTo(mBattery, newAlphaBC, (IObjectAnimator**)&obj);
        values->Set(3, IAnimator::Probe(obj));
        obj = NULL;
        AnimateTransitionTo(mClock, newAlphaBC, (IObjectAnimator**)&obj);
        values->Set(4, IAnimator::Probe(obj));
        anims->PlayTogether(values);
        if (mode == MODE_LIGHTS_OUT) {
            IAnimator::Probe(anims)->SetDuration(LIGHTS_OUT_DURATION);
        }
        IAnimator::Probe(anims)->Start();
        mCurrentAnimation = IAnimator::Probe(anims);
    }
    else {
        mLeftSide->SetAlpha(newAlpha);
        mStatusIcons->SetAlpha(newAlpha);
        mSignalCluster->SetAlpha(newAlpha);
        mBattery->SetAlpha(newAlphaBC);
        mClock->SetAlpha(newAlphaBC);
    }
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
