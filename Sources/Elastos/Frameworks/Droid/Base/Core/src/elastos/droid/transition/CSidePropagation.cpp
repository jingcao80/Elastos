
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/transition/CSidePropagation.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CSidePropagation::
//===============================================================

const String CSidePropagation::TAG("SlidePropagation");

CAR_OBJECT_IMPL(CSidePropagation)

CAR_INTERFACE_IMPL(CSidePropagation, VisibilityPropagation, ISidePropagation)

ECode CSidePropagation::SetSide(
    /* [in] */ Int32 side)
{
    mSide = side;
    return NOERROR;
}

ECode CSidePropagation::SetPropagationSpeed(
    /* [in] */ Float propagationSpeed)
{
    if (propagationSpeed == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPropagationSpeed = propagationSpeed;
    return NOERROR;
}

ECode CSidePropagation::GetStartDelay(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransition* transition,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    if (startValues == NULL && endValues == NULL) {
        *result = 0;
        return NOERROR;
    }
    Int32 directionMultiplier = 1;
    AutoPtr<IRect> epicenter;
    transition->GetEpicenter((IRect**)&epicenter);
    AutoPtr<ITransitionValues> positionValues;
    Int32 sv = 0;
    GetViewVisibility(startValues, &sv);
    if (endValues == NULL || sv == IView::VISIBLE) {
        positionValues = startValues;
        directionMultiplier = -1;
    }
    else {
        positionValues = endValues;
    }

    Int32 viewCenterX = 0;
    GetViewX(positionValues, &viewCenterX);
    Int32 viewCenterY = 0;
    GetViewY(positionValues, &viewCenterY);

    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    IView::Probe(sceneRoot)->GetLocationOnScreen(temp);
    Float x, y;
    IView::Probe(sceneRoot)->GetTranslationX(&x);
    IView::Probe(sceneRoot)->GetTranslationY(&y);
    Int32 left = (*temp)[0] + Elastos::Core::Math::Round(x);
    Int32 top = (*temp)[1] + Elastos::Core::Math::Round(y);
    Int32 w=0, h=0;
    IView::Probe(sceneRoot)->GetWidth(&w);
    IView::Probe(sceneRoot)->GetHeight(&h);
    Int32 right = left + w;
    Int32 bottom = top + h;

    Int32 epicenterX = 0;
    Int32 epicenterY = 0;
    if (epicenter != NULL) {
        epicenter->GetCenterX(&epicenterX);
        epicenter->GetCenterY(&epicenterY);
    }
    else {
        epicenterX = (left + right) / 2;
        epicenterY = (top + bottom) / 2;
    }

    Float distance = Distance(viewCenterX, viewCenterY, epicenterX, epicenterY,
            left, top, right, bottom);
    Float maxDistance = GetMaxDistance(sceneRoot);
    Float distanceFraction = distance/maxDistance;

    Int64 duration = 0;
    transition->GetDuration(&duration);
    if (duration < 0) {
        duration = 300;
    }

    return Elastos::Core::Math::Round(duration * directionMultiplier / mPropagationSpeed * distanceFraction);
}

Int32 CSidePropagation::Distance(
    /* [in] */ Int32 viewX,
    /* [in] */ Int32 viewY,
    /* [in] */ Int32 epicenterX,
    /* [in] */ Int32 epicenterY,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 distance = 0;
    // switch (mSide) {
    //     case IGravity::LEFT:
    //         distance = right - viewX + Elastos::Core::Math::Abs(epicenterY - viewY);
    //         break;
    //     case IGravity::TOP:
    //         distance = bottom - viewY + Elastos::Core::Math::Abs(epicenterX - viewX);
    //         break;
    //     case IGravity::RIGHT:
    //         distance = viewX - left + Elastos::Core::Math::Abs(epicenterY - viewY);
    //         break;
    //     case IGravity::BOTTOM:
    //         distance = viewY - top + Elastos::Core::Math::Abs(epicenterX - viewX);
    //         break;
    // }
    return distance;
}

Int32 CSidePropagation::GetMaxDistance(
    /* [in] */ IViewGroup* sceneRoot)
{
    Int32 w = 0, h = 0;
    switch (mSide) {
        case IGravity::LEFT:
        case IGravity::RIGHT:
            return (IView::Probe(sceneRoot)->GetWidth(&w), w);
        default:
            return (IView::Probe(sceneRoot)->GetHeight(&h), h);
    }
    return -1;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
