
#ifndef __ELASTOS_DROID_TRANSITION_CCHANGECLIPBOUNDS_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGECLIPBOUNDS_H__

#include "_Elastos_Droid_Transition_CChangeClipBounds.h"
#include "elastos/droid/transition/Transition.h"

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * ChangeClipBounds captures the {@link android.view.View#getClipBounds()} before and after the
 * scene change and animates those changes during the transition.
 */
CarClass(CChangeClipBounds)
    , public Transition
{
public:
    CAR_OBJECT_DECL()

    CChangeClipBounds();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetTransitionProperties(
        /* [out, callee] */ ArrayOf<String>** result);

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
        /* [in] */ ITransitionValues* values);

private:
    static const String TAG;

    static const String PROPNAME_CLIP;
    static const String PROPNAME_BOUNDS;

    static AutoPtr<ArrayOf<String> > sTransitionProperties;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGECLIPBOUNDS_H__
