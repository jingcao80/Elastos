
#include "elastos/droid/transition/CExplode.h"
#include "elastos/droid/transition/CCircularPropagation.h"
#include "elastos/droid/transition/TranslationAnimationCreator.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/utility/FloatMath.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::FloatMath;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CExplode::
//===============================================================
AutoPtr<ITimeInterpolator> CExplode::sDecelerate;// = new DecelerateInterpolator();
AutoPtr<ITimeInterpolator> CExplode::sAccelerate;// = new AccelerateInterpolator();
const String CExplode::TAG("Explode");
const String CExplode::PROPNAME_SCREEN_BOUNDS("android:explode:screenBounds");

CAR_OBJECT_IMPL(CExplode)

ECode CExplode::constructor()
{
    AutoPtr<ICircularPropagation> p;
    CCircularPropagation::New((ICircularPropagation**)&p);
    SetPropagation(ITransitionPropagation::Probe(p));
    return NOERROR;
}

ECode CExplode::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Visibility::constructor(context, attrs);
    AutoPtr<ICircularPropagation> p;
    CCircularPropagation::New((ICircularPropagation**)&p);
    SetPropagation(ITransitionPropagation::Probe(p));
    return NOERROR;
}

void CExplode::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    AutoPtr<IView> view = ctv->mView;
    view->GetLocationOnScreen(mTempLoc);
    Int32 left = (*mTempLoc)[0];
    Int32 top = (*mTempLoc)[1];
    Int32 w = 0, h = 0;
    view->GetWidth(&w);
    view->GetHeight(&h);
    Int32 right = left + w;
    Int32 bottom = top + h;
    AutoPtr<IRect> rt;
    CRect::New(left, top, right, bottom, (IRect**)&rt);
    AutoPtr<ICharSequence> pro_screen_bounds;
    CString::New(PROPNAME_SCREEN_BOUNDS, (ICharSequence**)&pro_screen_bounds);
    ctv->mValues->Put(pro_screen_bounds, rt);
}

ECode CExplode::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    Visibility::CaptureStartValues(transitionValues);
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CExplode::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    Visibility::CaptureEndValues(transitionValues);
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CExplode::OnAppear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    if (endValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_screen_bounds;
    CString::New(PROPNAME_SCREEN_BOUNDS, (ICharSequence**)&pro_screen_bounds);
    AutoPtr<IInterface> bs;
    cEnd->mValues->Get(pro_screen_bounds, (IInterface**)&bs);
    AutoPtr<IRect> bounds = IRect::Probe(bs);
    Float endX = 0.0f;
    view->GetTranslationX(&endX);
    Float endY = 0.0f;
    view->GetTranslationY(&endY);
    CalculateOut(IView::Probe(sceneRoot), bounds, mTempLoc);
    Float startX = endX + (*mTempLoc)[0];
    Float startY = endY + (*mTempLoc)[1];

    AutoPtr<CRect> cBounds = (CRect*)bounds.Get();
    AutoPtr<IAnimator> res = TranslationAnimationCreator::CreateAnimation(view, endValues, cBounds->mLeft, cBounds->mTop,
            startX, startY, endX, endY, sDecelerate);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CExplode::OnDisappear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    if (startValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cStart = (CTransitionValues*)startValues;
    AutoPtr<ICharSequence> pro_screen_bounds;
    CString::New(PROPNAME_SCREEN_BOUNDS, (ICharSequence**)&pro_screen_bounds);
    AutoPtr<IInterface> bs;
    cStart->mValues->Get(pro_screen_bounds, (IInterface**)&bs);
    AutoPtr<IRect> bounds = IRect::Probe(bs);
    AutoPtr<CRect> cBounds = (CRect*)bounds.Get();
    Int32 viewPosX = cBounds->mLeft;
    Int32 viewPosY = cBounds->mTop;
    Float startX = 0.0f;
    view->GetTranslationX(&startX);
    Float startY = 0.0f;
    view->GetTranslationY(&startY);
    Float endX = startX;
    Float endY = startY;
    assert(0 && "TODO");
    AutoPtr<ArrayOf<Int32> > interruptedPosition;// = (Int32[]) startValues->mView->GetTag(R.id.transitionPosition);
    if (interruptedPosition != NULL) {
        // We want to have the end position relative to the interrupted position, not
        // the position it was supposed to start at.
        endX += (*interruptedPosition)[0] - cBounds->mLeft;
        endY += (*interruptedPosition)[1] - cBounds->mTop;
        bounds->OffsetTo((*interruptedPosition)[0], (*interruptedPosition)[1]);
    }
    CalculateOut(IView::Probe(sceneRoot), bounds, mTempLoc);
    endX += (*mTempLoc)[0];
    endY += (*mTempLoc)[1];

    AutoPtr<IAnimator> res = TranslationAnimationCreator::CreateAnimation(view, startValues,
            viewPosX, viewPosY, startX, startY, endX, endY, sAccelerate);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CExplode::CalculateOut(
    /* [in] */ IView* sceneRoot,
    /* [in] */ IRect* bounds,
    /* [in] */ ArrayOf<Int32>* outVector)
{
    sceneRoot->GetLocationOnScreen(mTempLoc);
    Int32 sceneRootX = (*mTempLoc)[0];
    Int32 sceneRootY = (*mTempLoc)[1];
    Int32 focalX;
    Int32 focalY;
    AutoPtr<IRect> epicenter;
    GetEpicenter((IRect**)&epicenter);
    if (epicenter == NULL) {
        Int32 w = 0, h = 0;
        Float x = 0, y = 0;
        sceneRoot->GetWidth(&w);
        sceneRoot->GetTranslationX(&x);
        sceneRoot->GetHeight(&h);
        sceneRoot->GetTranslationY(&y);
        focalX = sceneRootX + (w / 2) + Elastos::Core::Math::Round(x);
        focalY = sceneRootY + (h / 2) + Elastos::Core::Math::Round(y);
    }
    else {
        epicenter->GetCenterX(&focalX);
        epicenter->GetCenterY(&focalY);
    }

    Int32 centerX = 0;
    bounds->GetCenterX(&centerX);
    Int32 centerY = 0;
    bounds->GetCenterY(&centerY);
    Float xVector = centerX - focalX;
    Float yVector = centerY - focalY;

    if (xVector == 0 && yVector == 0) {
        // Random direction when View is centered on focal View.
        xVector = (Float) (Elastos::Core::Math::Random() * 2) - 1;
        yVector = (Float) (Elastos::Core::Math::Random() * 2) - 1;
    }
    Float vectorSize = CalculateDistance(xVector, yVector);
    xVector /= vectorSize;
    yVector /= vectorSize;

    Float maxDistance = CalculateMaxDistance(sceneRoot, focalX - sceneRootX, focalY - sceneRootY);

    (*outVector)[0] = Elastos::Core::Math::Round(maxDistance * xVector);
    (*outVector)[1] = Elastos::Core::Math::Round(maxDistance * yVector);
}

Float CExplode::CalculateMaxDistance(
    /* [in] */ IView* sceneRoot,
    /* [in] */ Int32 focalX,
    /* [in] */ Int32 focalY)
{
    Int32 w = 0, h = 0;
    sceneRoot->GetWidth(&w);
    sceneRoot->GetHeight(&h);
    Int32 maxX = Elastos::Core::Math::Max(focalX, w - focalX);
    Int32 maxY = Elastos::Core::Math::Max(focalY, h - focalY);
    return CalculateDistance(maxX, maxY);
}

Float CExplode::CalculateDistance(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return FloatMath::Sqrt((x * x) + (y * y));
}

ECode CExplode::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransition> changes;
    CExplode::New((ITransition**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CExplode::CloneImpl(
    /* [in] */ ITransition* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
