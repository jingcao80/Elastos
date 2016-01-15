
#ifndef __ELASTOS_DROID_TRANSITION_CEXPLODE_H__
#define __ELASTOS_DROID_TRANSITION_CEXPLODE_H__

#include "_Elastos_Droid_Transition_CExplode.h"
#include "elastos/droid/transition/Visibility.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CExplode)
    , public Visibility
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

    CARAPI OnAppear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI OnDisappear(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
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

    CARAPI_(void) CalculateOut(
        /* [in] */ IView* sceneRoot,
        /* [in] */ IRect* bounds,
        /* [in] */ ArrayOf<Int32>* outVector);

    static CARAPI_(Float) CalculateMaxDistance(
        /* [in] */ IView* sceneRoot,
        /* [in] */ Int32 focalX,
        /* [in] */ Int32 focalY);

    static CARAPI_(Float) CalculateDistance(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    static AutoPtr<ITimeInterpolator> sDecelerate;
    static AutoPtr<ITimeInterpolator> sAccelerate;
    static const String TAG;
    static const String PROPNAME_SCREEN_BOUNDS;

    AutoPtr<ArrayOf<Int32> > mTempLoc;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CEXPLODE_H__
