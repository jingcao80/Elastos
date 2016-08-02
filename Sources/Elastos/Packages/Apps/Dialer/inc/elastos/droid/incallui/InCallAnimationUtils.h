
#ifndef __ELASTOS_DROID_INCALLUI_INCALLANIMATIONUTILS_H__
#define __ELASTOS_DROID_INCALLUI_INCALLANIMATIONUTILS_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/graphics/drawable/LayerDrawable.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::LayerDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallAnimationUtils
{
private:
    /**
     * Drawable achieving cross-fade, just like TransitionDrawable. We can have
     * call-backs via animator object (see also {@link CrossFadeDrawable#getAnimator()}).
     */
    class CrossFadeDrawable : public LayerDrawable
    {
    public:
        CrossFadeDrawable(
            /* [in] */ ArrayOf<IDrawable*>* layers);

        /**
         * This will be used from ObjectAnimator.
         * Note: this method is protected by proguard.flags so that it won't be removed
         * automatically.
         */
        // @SuppressWarnings("unused")
        CARAPI_(void) SetCrossFadeAlpha(
            /* [in] */ Int32 alpha);

        CARAPI_(AutoPtr<IObjectAnimator>) GetAnimator();

        // @Override
        CARAPI Draw(
            /* [in] */ ICanvas* canvas);

    private:
        AutoPtr<IObjectAnimator> mAnimator;
        Int32 mCrossFadeAlpha;
    };

    class AnimatorListener : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener(
            /* [in] */ IImageView* imageView,
            /* [in] */ IDrawable* from,
            /* [in] */ IDrawable* to)
            : mImageView(imageView)
            , mFrom(from)
            , mTo(to)
        {}

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IImageView> mImageView;
        AutoPtr<IDrawable> mFrom;
        AutoPtr<IDrawable> mTo;
    };

public:
    /**
     * Starts cross-fade animation using TransitionDrawable. Nothing will happen if "from" and "to"
     * are the same.
     */
    static CARAPI_(void) StartCrossFade(
        /* [in] */ IImageView* imageView,
        /* [in] */ IDrawable* from,
        /* [in] */ IDrawable* to);

private:
    static CARAPI_(AutoPtr<CrossFadeDrawable>) NewCrossFadeDrawable(
        /* [in] */ IDrawable* first,
        /* [in] */ IDrawable* second);

private:
    InCallAnimationUtils() {}

public:
    /**
     * Duration for animations in msec, which can be used with
     * {@link ViewPropertyAnimator#setDuration(long)} for example.
     */
    static const Int32 ANIMATION_DURATION = 250;

private:
    static const String TAG;
    /**
     * Turn on when you're interested in fading animation. Intentionally untied from other debug
     * settings.
     */
    static const Boolean FADE_DBG = FALSE;


};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLANIMATIONUTILS_H__
