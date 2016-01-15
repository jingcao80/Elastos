#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CDRAWABLEHOLDER_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CDRAWABLEHOLDER_H__

#include "_Elastos_Droid_Widget_Internal_CDrawableHolder.h"
#include "elastos/droid/widget/internal/DrawableHolder.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

CarClass(CDrawableHolder), public DrawableHolder
{
public:

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IBitmapDrawable* drawable);

    CARAPI constructor(
        /* [in] */ IBitmapDrawable* drawable,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI AddAnimTo(
        /* [in] */ Int64 duration,
        /* [in] */ Int64 delay,
        /* [in] */ const String& property,
        /* [in] */ Float toValue,
        /* [in] */ Boolean replace,
        /* [out] */ IObjectAnimator** animator);

    CARAPI RemoveAnimationFor(
        /* [in] */ const String& property);

    CARAPI ClearAnimations();

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

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
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_CDRAWABLEHOLDER_H__
