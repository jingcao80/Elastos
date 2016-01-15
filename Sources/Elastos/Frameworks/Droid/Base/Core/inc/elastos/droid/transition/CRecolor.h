
#ifndef __ELASTOS_DROID_TRANSITION_CRECOLOR_H__
#define __ELASTOS_DROID_TRANSITION_CRECOLOR_H__

#include "_Elastos_Droid_Transition_CRecolor.h"
#include "elastos/droid/transition/Transition.h"

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition tracks changes during scene changes to the
 * {@link View#setBackground(android.graphics.drawable.Drawable) background}
 * property of its target views (when the background is a
 * {@link ColorDrawable}, as well as the
 * {@link TextView#setTextColor(android.content.res.ColorStateList)
 * color} of the text for target TextViews. If the color changes between
 * scenes, the color change is animated.
 *
 * @hide
 */
CarClass(CRecolor)
    , public Transition
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ ITransition* obj);

private:
    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

private:
    static const String PROPNAME_BACKGROUND;
    static const String PROPNAME_TEXT_COLOR;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CRECOLOR_H__
