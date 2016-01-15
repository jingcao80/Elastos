
#ifndef __ELASTOS_DROID_TRANSITION_CSLIDE_H__
#define __ELASTOS_DROID_TRANSITION_CSLIDE_H__

#include "_Elastos_Droid_Transition_CSlide.h"
#include "elastos/droid/transition/Visibility.h"

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition tracks changes to the visibility of target views in the
 * start and end scenes and moves views in or out from one of the edges of the
 * scene. Visibility is determined by both the
 * {@link View#setVisibility(int)} state of the view as well as whether it
 * is parented in the current view hierarchy. Disappearing Views are
 * limited as described in {@link Visibility#onDisappear(android.view.ViewGroup,
 * TransitionValues, int, TransitionValues, int)}.
 */
CarClass(CSlide)
    , public Visibility
    , public ISlide
{
private:
    class CalculateSlideHorizontal
        : public Object
        , public ICalculateSlide
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetGoneY(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

    class CalculateSlideHorizontalLeft
        : public CalculateSlideHorizontal
    {
    public:
        CARAPI GetGoneX(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

    class CalculateSlideHorizontalRight
        : public CalculateSlideHorizontal
    {
    public:
        CARAPI GetGoneX(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

    class CalculateSlideVertical
        : public Object
        , public ICalculateSlide
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetGoneX(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

    class CalculateSlideVerticalTop
        : public CalculateSlideVertical
    {
    public:
        CARAPI GetGoneY(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

    class CalculateSlideVerticalBottom
        : public CalculateSlideVertical
    {
    public:
        CARAPI GetGoneY(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* view,
            /* [out] */ Float* result);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSlide();

    /**
     * Constructor using the default {@link Gravity#BOTTOM}
     * slide edge direction.
     */
    CARAPI constructor();

    /**
     * Constructor using the provided slide edge direction.
     */
    CARAPI constructor(
        /* [in] */ Int32 slideEdge);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI SetSlideEdge(
        /* [in] */ Int32 slideEdge);

    CARAPI GetSlideEdge(
        /* [out] */ Int32* result);

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
        /* [in] */ ISlide* obj);

private:
    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

private:
    static const String TAG;
    static AutoPtr<ITimeInterpolator> sDecelerate;
    static AutoPtr<ITimeInterpolator> sAccelerate;
    static const String PROPNAME_SCREEN_POSITION;
    AutoPtr<ICalculateSlide> mSlideCalculator;
    Int32 mSlideEdge;

    static AutoPtr<ICalculateSlide> sCalculateLeft;

    static AutoPtr<ICalculateSlide> sCalculateTop;

    static AutoPtr<ICalculateSlide> sCalculateRight;

    static AutoPtr<ICalculateSlide> sCalculateBottom;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CSLIDE_H__
