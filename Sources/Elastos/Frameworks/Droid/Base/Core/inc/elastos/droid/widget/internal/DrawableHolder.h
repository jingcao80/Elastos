
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_DRAWABLEHOLDER_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_DRAWABLEHOLDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Math;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

/**
 * This class is a container for a Drawable with multiple animated properties.
 *
 */
class DrawableHolder
{
public:
    DrawableHolder();

    DrawableHolder(
        /* [in] */ IBitmapDrawable* drawable,
        /* [in] */ Float x = 0.0f,
        /* [in] */ Float y = 0.0f);

    /**
     *
     * Adds an animation that interpolates given property from its current value
     * to the given value.
     *
     * @param duration the duration, in ms.
     * @param delay the delay to start the animation, in ms.
     * @param property the property to animate
     * @param toValue the target value
     * @param replace if true, replace the current animation with this one.
     */
    virtual CARAPI_(AutoPtr<IObjectAnimator>) AddAnimTo(
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay,
        /* [in] */ const String& property,
        /* [in] */ Float toValue,
        /* [in] */ Boolean replace);

    /**
     * Stops all animations for the given property and removes it from the list.
     *
     * @param property
     */
    virtual CARAPI RemoveAnimationFor(
        /* [in] */ const String& property);

    /**
     * Stops all animations and removes them from the list.
     */
    virtual CARAPI ClearAnimations();


    /**
     * Draw this object to the canvas using the properties defined in this class.
     *
     * @param canvas canvas to draw into
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Starts all animations added since the last call to this function.  Used to synchronize
     * animations.
     *
     * @param listener an optional listener to add to the animations. Typically used to know when
     * to invalidate the surface these are being drawn to.
     */
    virtual CARAPI StartAnimations(
        /* [in] */ IAnimatorUpdateListener* listener);


    virtual CARAPI SetX(
        /* [in] */ Float value);

    virtual CARAPI SetY(
        /* [in] */ Float value);

    virtual CARAPI SetScaleX(
        /* [in] */ Float value);

    virtual CARAPI SetScaleY(
        /* [in] */ Float value);

    virtual CARAPI SetAlpha(
        /* [in] */ Float alpha);

    virtual CARAPI_(Float) GetX();

    virtual CARAPI_(Float) GetY();

    virtual CARAPI_(Float) GetScaleX();

    virtual CARAPI_(Float) GetScaleY();

    virtual CARAPI_(Float) GetAlpha();

    virtual CARAPI_(AutoPtr<IBitmapDrawable>) GetDrawable();

    virtual CARAPI_(Int32) GetWidth();

    virtual CARAPI_(Int32) GetHeight();

    virtual CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;
protected:
    CARAPI Init(
        /* [in] */ IBitmapDrawable* drawable,
        /* [in] */ Float x = 0.0f,
        /* [in] */ Float y = 0.0f);
public:
//    static const DecelerateInterpolator EASE_OUT_INTERPOLATOR = new DecelerateInterpolator();

private:
    /**
     * Adds the given animation to the list of animations for this object.
     *
     * @param anim
     * @param overwrite
     * @return
     */
    AutoPtr<IDrawableHolder> AddAnimation(
        /* [in] */ IObjectAnimator* anim,
        /* [in] */ Boolean overwrite);

private:
    static const String TAG;
    static const Boolean DBG = FALSE;
    Float mX;
    Float mY;
    Float mScaleX;
    Float mScaleY;
    AutoPtr<IBitmapDrawable> mDrawable;
    Float mAlpha;
    List<AutoPtr<IAnimator> > mAnimators;
    List<AutoPtr<IObjectAnimator> > mNeedToStart;
    static AutoPtr<IDecelerateInterpolator> EASE_OUT_INTERPOLATOR;
    typedef List<AutoPtr<IAnimator> >::Iterator Iterator;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif
