
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/transition/CCircularPropagation.h"
#include "elastos/droid/utility/FloatMath.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::FloatMath;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CCircularPropagation::
//===============================================================

String CCircularPropagation::TAG = String("CircularPropagation");

CAR_OBJECT_IMPL(CCircularPropagation)

CAR_INTERFACE_IMPL(CCircularPropagation, VisibilityPropagation, ICircularPropagation)

ECode CCircularPropagation::SetPropagationSpeed(
    /* [in] */ Float propagationSpeed)
{
    if (propagationSpeed == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPropagationSpeed = propagationSpeed;
    return NOERROR;
}

ECode CCircularPropagation::GetStartDelay(
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
    AutoPtr<ITransitionValues> positionValues;
    Int32 vv = 0;
    if (endValues == NULL || (GetViewVisibility(startValues, &vv), vv) == IView::VISIBLE) {
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

    AutoPtr<IRect> epicenter;
    transition->GetEpicenter((IRect**)&epicenter);
    Int32 epicenterX;
    Int32 epicenterY;
    if (epicenter != NULL) {
        epicenter->GetCenterX(&epicenterX);
        epicenter->GetCenterY(&epicenterY);
    }
    else {
        AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
        IView::Probe(sceneRoot)->GetLocationOnScreen(temp);
        Int32 w = 0, h = 0;
        Float x = 0, y = 0;
        IView::Probe(sceneRoot)->GetWidth(&w);
        IView::Probe(sceneRoot)->GetTranslationX(&x);
        IView::Probe(sceneRoot)->GetHeight(&h);
        IView::Probe(sceneRoot)->GetTranslationY(&y);
        epicenterX = Elastos::Core::Math::Round((*temp)[0] + (w / 2) + x);
        epicenterY = Elastos::Core::Math::Round((*temp)[1] + (h / 2) + y);
    }
    Float distance = Distance(viewCenterX, viewCenterY, epicenterX, epicenterY);
    Int32 w = 0, h = 0;
    IView::Probe(sceneRoot)->GetWidth(&w);
    IView::Probe(sceneRoot)->GetHeight(&h);
    Float maxDistance = Distance(0, 0, w, h);
    Float distanceFraction = distance/maxDistance;

    Int64 duration = 0;
    transition->GetDuration(&duration);
    if (duration < 0) {
        duration = 300;
    }

    return Elastos::Core::Math::Round(duration * directionMultiplier / mPropagationSpeed * distanceFraction);
}

Float CCircularPropagation::Distance(
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2)
{
    Float x = x2 - x1;
    Float y = y2 - y1;
    return FloatMath::Sqrt((x * x) + (y * y));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
