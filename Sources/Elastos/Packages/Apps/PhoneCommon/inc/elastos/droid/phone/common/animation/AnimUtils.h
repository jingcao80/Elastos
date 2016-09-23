
#ifndef __ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMUTILS_H__
#define __ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMUTILS_H__

#include "Elastos.Droid.Phone.Common.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Animation {

class AnimUtils
{
private:
    class FadeOutAdapter
        : public AnimatorListenerAdapter
    {
    public:
        FadeOutAdapter(
            /* [in] */ IView* fadeOut,
            /* [in] */ IAnimationCallback* callback)
            : mFadeOut(fadeOut)
            , mCallback(callback)
        {}

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mFadeOut;
        AutoPtr<IAnimationCallback> mCallback;
    };

    class FadeInAdapter
        : public AnimatorListenerAdapter
    {
    public:
        FadeInAdapter(
            /* [in] */ IView* fadeIn,
            /* [in] */ IAnimationCallback* callback)
            : mFadeIn(fadeIn)
            , mCallback(callback)
        {}

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
        AutoPtr<IView> mFadeIn;
        AutoPtr<IAnimationCallback> mCallback;
    };

    class ScaleInAdapter
        : public AnimatorListenerAdapter
    {
    public:
        ScaleInAdapter(
            /* [in] */ IView* view)
            : mView(view)
        {}

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mView;
    };

    class ScaleOutAdapter
        : public AnimatorListenerAdapter
    {
    public:
        ScaleOutAdapter(
            /* [in] */ IView* view)
            : mView(view)
        {}

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mView;
    };

    class UpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        UpdateListener(
            /* [in] */ IView* view,
            /* [in] */ Int32 oldWidth,
            /* [in] */ Int32 oldHeight,
            /* [in] */ Int32 deltaWidth,
            /* [in] */ Int32 deltaHeight)
            : mView(view)
            , mOldWidth(oldWidth)
            , mOldHeight(oldHeight)
            , mDeltaWidth(deltaWidth)
            , mDeltaHeight(deltaHeight)
        {}

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animator);

    private:
        AutoPtr<IView> mView;
        Int32 mOldWidth;
        Int32 mOldHeight;
        Int32 mDeltaWidth;
        Int32 mDeltaHeight;
    };

public:
    static CARAPI CrossFadeViews(
        /* [in] */ IView* fadeIn,
        /* [in] */ IView* fadeOut,
        /* [in] */ Int32 duration);

    static CARAPI FadeOut(
        /* [in] */ IView* fadeOut,
        /* [in] */ Int32 duration);

    static CARAPI FadeOut(
        /* [in] */ IView* fadeOut,
        /* [in] */ Int32 durationMs,
        /* [in] */ IAnimationCallback* callback);

    static CARAPI FadeIn(
        /* [in] */ IView* fadeIn,
        /* [in] */ Int32 durationMs);

    static CARAPI FadeIn(
        /* [in] */ IView* fadeIn,
        /* [in] */ Int32 durationMs,
        /* [in] */ Int32 delay,
        /* [in] */ IAnimationCallback* callback);

    /**
     * Scales in the view from scale of 0 to actual dimensions.
     * @param view The view to scale.
     * @param durationMs The duration of the scaling in milliseconds.
     * @param startDelayMs The delay to applying the scaling in milliseconds.
     */
    static CARAPI ScaleIn(
        /* [in] */ IView* view,
        /* [in] */ Int32 durationMs,
        /* [in] */ Int32 startDelayMs);

    /**
     * Scales out the view from actual dimensions to 0.
     * @param view The view to scale.
     * @param durationMs The duration of the scaling in milliseconds.
     */
    static CARAPI ScaleOut(
        /* [in] */ IView* view,
        /* [in] */ Int32 durationMs);

    /**
     * Animates a view to the new specified dimensions.
     * @param view The view to change the dimensions of.
     * @param newWidth The new width of the view.
     * @param newHeight The new height of the view.
     */
    static CARAPI ChangeDimensions(
        /* [in] */ IView* view,
        /* [in] */ Int32 newWidth,
        /* [in] */ Int32 newHeight);

private:
    static CARAPI_(void) ScaleInternal(
        /* [in] */ IView* view,
        /* [in] */ Int32 startScaleValue,
        /* [in] */ Int32 endScaleValue,
        /* [in] */ Int32 durationMs,
        /* [in] */ Int32 startDelay,
        /* [in] */ AnimatorListenerAdapter* listener,
        /* [in] */ IInterpolator* interpolator);

public:
    static const Int32 DEFAULT_DURATION;
    static const Int32 NO_DELAY;

    static const AutoPtr<IInterpolator> EASE_IN;
    static const AutoPtr<IInterpolator> EASE_OUT;
    static const AutoPtr<IInterpolator> EASE_OUT_EASE_IN;
};

} // namespace Animation
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_ANIMATION_ANIMUTILS_H__
