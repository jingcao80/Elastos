
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTRUSTDRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTRUSTDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CTrustDrawable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CTrustDrawable), public Drawable
{
private:
    class StateUpdateAnimatorListener: public AnimatorListenerAdapter
    {
    public:
        StateUpdateAnimatorListener(
            /* [in] */ CTrustDrawable* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CTrustDrawable* mHost;
        Boolean mCancelled;
    };

    class AlphaUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AlphaUpdateListener(
            /* [in] */ CTrustDrawable* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CTrustDrawable* mHost;
    };

    class RadiusUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        RadiusUpdateListener(
            /* [in] */ CTrustDrawable* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CTrustDrawable* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CTrustDrawable();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    CARAPI_(void) Start();

    CARAPI_(void) Stop();

    CARAPI_(void) SetTrustManaged(
        /* [in] */ Boolean trustManaged);

private:
    CARAPI_(void) UpdateState(
        /* [in] */ Boolean animate);

    CARAPI_(AutoPtr<IAnimator>) MakeVisibleAnimator();

    CARAPI_(AutoPtr<IAnimator>) MakeEnterAnimator(
        /* [in] */ Float radius,
        /* [in] */ Int32 alpha);

    CARAPI_(AutoPtr<IAnimator>) MakeExitAnimator(
        /* [in] */ Float radius,
        /* [in] */ Int32 alpha);

    CARAPI_(AutoPtr<IAnimator>) MakeAnimators(
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius,
        /* [in] */ Int32 startAlpha,
        /* [in] */ Int32 endAlpha,
        /* [in] */ Int64 duration,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ Boolean repeating,
        /* [in] */ Boolean stateUpdateListener);

    CARAPI_(AutoPtr<IValueAnimator>) ConfigureAnimator(
        /* [in] */ IValueAnimator* animator,
        /* [in] */ Int64 duration,
        /* [in] */ IAnimatorUpdateListener* updateListener,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ Boolean repeating);

private:
    static const Int64 ENTERING_FROM_UNSET_START_DELAY;
    static const Int64 VISIBLE_DURATION;
    static const Int64 EXIT_DURATION;
    static const Int64 ENTER_DURATION;

    static const Int32 ALPHA_VISIBLE_MIN;
    static const Int32 ALPHA_VISIBLE_MAX;

    static const Int32 STATE_UNSET;
    static const Int32 STATE_GONE;
    static const Int32 STATE_ENTERING;
    static const Int32 STATE_VISIBLE;
    static const Int32 STATE_EXITING;

    Int32 mAlpha;
    Boolean mAnimating;

    Int32 mCurAlpha;
    Float mCurInnerRadius;
    AutoPtr<IAnimator> mCurAnimator;
    Int32 mState;
    AutoPtr<IPaint> mPaint;
    Boolean mTrustManaged;

    Float mInnerRadiusVisibleMin;
    Float mInnerRadiusVisibleMax;
    Float mInnerRadiusExit;
    Float mInnerRadiusEnter;
    Float mThickness;

    AutoPtr<IAnimator> mVisibleAnimator;

    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mAccelerateDecelerateInterpolator;

    AutoPtr<IAnimatorUpdateListener> mAlphaUpdateListener;
    AutoPtr<IAnimatorUpdateListener> mRadiusUpdateListener;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CTRUSTDRAWABLE_H__
