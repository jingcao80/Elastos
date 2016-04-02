
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SCRIMCONTROLLER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SCRIMCONTROLLER_H__

#include "_SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/DozeParameters.h"
#include "elastos/droid/systemui/statusbar/phone/UnlockMethodCache.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::SystemUI::Doze::IDozeHostPulseCallback;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Controls both the scrim behind the notifications and in front of the notifications (when a
 * security method gets shown).
 */
class ScrimController
    : public Object
    , public IScrimController
    , public IOnPreDrawListener
{
private:
    class PulseIn: public Runnable
    {
    public:
        PulseIn(
            /* [in] */ ScrimController* host);

        // @Override
        CARAPI Run();

    private:
        ScrimController* mHost;
    };

    class PulseInFinished: public Runnable
    {
    public:
        PulseInFinished(
            /* [in] */ ScrimController* host);

        // @Override
        CARAPI Run();

    private:
        ScrimController* mHost;
    };

    class PulseOut: public Runnable
    {
    public:
        PulseOut(
            /* [in] */ ScrimController* host);

        // @Override
        CARAPI Run();

    private:
        ScrimController* mHost;
    };

    class PulseOutFinished: public Runnable
    {
    public:
        PulseOutFinished(
            /* [in] */ ScrimController* host);

        // @Override
        CARAPI Run();

    private:
        ScrimController* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        AnimatorUpdateListener(
            /* [in] */ IScrimView* scrim);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IScrimView> mScrim;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ ScrimController* host,
            /* [in] */ IScrimView* scrim);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        ScrimController* mHost;
        AutoPtr<IScrimView> mScrim;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ ScrimController* host);

        // @Override
        CARAPI Run();

    private:
        ScrimController* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ScrimController(
        /* [in] */ IScrimView* scrimBehind,
        /* [in] */ IScrimView* scrimInFront,
        /* [in] */ Boolean scrimSrcEnabled);

    CARAPI SetKeyguardShowing(
        /* [in] */ Boolean showing);

    CARAPI OnTrackingStarted();

    CARAPI OnExpandingFinished();

    CARAPI SetPanelExpansion(
        /* [in] */ Float fraction);

    CARAPI SetBouncerShowing(
        /* [in] */ Boolean showing);

    CARAPI AnimateKeyguardFadingOut(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration,
        /* [in] */ IRunnable* onAnimationFinished);

    CARAPI AnimateGoingToFullShade(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

    CARAPI SetDozing(
        /* [in] */ Boolean dozing);

    /** When dozing, fade screen contents in and out using the front scrim. */
    CARAPI Pulse(
        /* [in] */ /*@NonNull*/ IDozeHostPulseCallback* callback);

    CARAPI IsPulsing(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPreDraw(
        /* [out] */ Boolean* result);

    CARAPI SetBackDropView(
        /* [in] */ IBackDropView* backDropView);

private:
    CARAPI_(void) CancelPulsing();

    CARAPI_(void) PulseStarted();

    CARAPI_(void) PulseFinished();

    CARAPI_(void) ScheduleUpdate();

    CARAPI_(void) UpdateScrims();

    CARAPI_(void) UpdateScrimKeyguard();

    CARAPI_(void) UpdateScrimNormal();

    CARAPI_(void) SetScrimBehindColor(
        /* [in] */ Float alpha);

    CARAPI_(void) SetScrimInFrontColor(
        /* [in] */ Float alpha);

    CARAPI_(void) SetScrimColor(
        /* [in] */ IScrimView* scrim,
        /* [in] */ Float alpha);

    CARAPI_(void) StartScrimAnimation(
        /* [in] */ IScrimView* scrim,
        /* [in] */ Int32 targetColor);

    CARAPI_(void) UpdateScrimBehindDrawingMode();

public:
    static const Int64 ANIMATION_DURATION;

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Float SCRIM_BEHIND_ALPHA;
    static const Float SCRIM_BEHIND_ALPHA_KEYGUARD;
    static const Float SCRIM_BEHIND_ALPHA_UNLOCKING;
    static const Float SCRIM_IN_FRONT_ALPHA;
    static const Int32 TAG_KEY_ANIM;

    AutoPtr<IScrimView> mScrimBehind;
    AutoPtr<IScrimView> mScrimInFront;
    AutoPtr<UnlockMethodCache> mUnlockMethodCache;
    AutoPtr<DozeParameters> mDozeParameters;

    Boolean mKeyguardShowing;
    Float mFraction;

    Boolean mDarkenWhileDragging;
    Boolean mBouncerShowing;
    Boolean mAnimateChange;
    Boolean mUpdatePending;
    Boolean mExpanding;
    Boolean mAnimateKeyguardFadingOut;
    Int64 mDurationOverride;
    Int64 mAnimationDelay;
    AutoPtr<IRunnable> mOnAnimationFinished;
    Boolean mAnimationStarted;
    Boolean mDozing;
    AutoPtr<IDozeHostPulseCallback> mPulseCallback;
    AutoPtr<IInterpolator> mInterpolator;
    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<IBackDropView> mBackDropView;
    Boolean mScrimSrcEnabled;
    AutoPtr<IRunnable> mPulseIn;
    AutoPtr<IRunnable> mPulseInFinished;
    AutoPtr<IRunnable> mPulseOut;
    AutoPtr<IRunnable> mPulseOutFinished;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SCRIMCONTROLLER_H__
