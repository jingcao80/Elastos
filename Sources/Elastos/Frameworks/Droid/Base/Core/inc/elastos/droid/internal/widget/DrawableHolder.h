
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_DRAWABLEHOLDER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_DRAWABLEHOLDER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * This class is a container for a Drawable with multiple animated properties.
 *
 */
class DrawableHolder
    : public Object
    , public IDrawableHolder
    , public IAnimatorListener
{
public:
    CAR_INTERFACE_DECL()

    DrawableHolder();

    ~DrawableHolder();

    CARAPI constructor(
        /* [in] */ IBitmapDrawable* drawable);

    CARAPI constructor(
        /* [in] */ IBitmapDrawable* drawable,
        /* [in] */ Float x,
        /* [in] */ Float y);

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
    CARAPI AddAnimTo(
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay,
        /* [in] */ const String& property,
        /* [in] */ Float toValue,
        /* [in] */ Boolean replace,
        /* [out] */ IObjectAnimator** animator);

    /**
     * Stops all animations for the given property and removes it from the list.
     *
     * @param property
     */
    CARAPI RemoveAnimationFor(
        /* [in] */ const String& property);

    /**
     * Stops all animations and removes them from the list.
     */
    CARAPI ClearAnimations();


    /**
     * Draw this object to the canvas using the properties defined in this class.
     *
     * @param canvas canvas to draw into
     */
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Starts all animations added since the last call to this function.  Used to synchronize
     * animations.
     *
     * @param listener an optional listener to add to the animations. Typically used to know when
     * to invalidate the surface these are being drawn to.
     */
    CARAPI StartAnimations(
        /* [in] */ IAnimatorUpdateListener* listener);

    CARAPI SetX(
        /* [in] */ Float value);

    CARAPI SetY(
        /* [in] */ Float value);

    CARAPI SetScaleX(
        /* [in] */ Float value);

    CARAPI SetScaleY(
        /* [in] */ Float value);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    CARAPI GetX(
        /* [out] */ Float* x);

    CARAPI GetY(
        /* [out] */ Float* y);

    CARAPI GetScaleX(
        /* [out] */ Float* x);

    CARAPI GetScaleY(
        /* [out] */ Float* y);

    CARAPI GetAlpha(
        /* [out] */ Float* alpha);

    CARAPI GetDrawable(
        /* [out] */ IBitmapDrawable** d);

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

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

public:
    static const AutoPtr<IDecelerateInterpolator> EASE_OUT_INTERPOLATOR;

private:
    static const String TAG;
    static const Boolean DBG = FALSE;
    Float mX;
    Float mY;
    Float mScaleX;
    Float mScaleY;
    AutoPtr<IBitmapDrawable> mDrawable;
    Float mAlpha;
    AutoPtr<IArrayList> mAnimators;
    AutoPtr<IArrayList> mNeedToStart;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_DRAWABLEHOLDER_H__
