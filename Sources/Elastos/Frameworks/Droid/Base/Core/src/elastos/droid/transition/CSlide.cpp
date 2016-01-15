
#include "Elastos.Droid.Content.h"
#include "elastos/droid/transition/CSlide.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CSidePropagation.h"
#include "elastos/droid/transition/TranslationAnimationCreator.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CSlide::CalculateSlideHorizontal::
//===============================================================
CAR_INTERFACE_IMPL(CSlide::CalculateSlideHorizontal, Object, ICalculateSlide)

ECode CSlide::CalculateSlideHorizontal::GetGoneY(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    return view->GetTranslationY(result);
}

//===============================================================
// CSlide::CalculateSlideHorizontalLeft::
//===============================================================
ECode CSlide::CalculateSlideHorizontalLeft::GetGoneX(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float x = 0;
    view->GetTranslationX(&x);
    Int32 w = 0;
    IView::Probe(sceneRoot)->GetWidth(&w);
    *result = x - w;
    return NOERROR;
}

//===============================================================
// CSlide::CalculateSlideHorizontalRight::
//===============================================================
ECode CSlide::CalculateSlideHorizontalRight::GetGoneX(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float x = 0;
    view->GetTranslationX(&x);
    Int32 w = 0;
    IView::Probe(sceneRoot)->GetWidth(&w);
    *result = x + w;
    return NOERROR;
}

//===============================================================
// CSlide::CalculateSlideVertical::
//===============================================================
CAR_INTERFACE_IMPL(CSlide::CalculateSlideVertical, Object, ICalculateSlide)

ECode CSlide::CalculateSlideVertical::GetGoneX(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    return view->GetTranslationX(result);
}

//===============================================================
// CSlide::CalculateSlideVerticalTop::
//===============================================================

ECode CSlide::CalculateSlideVerticalTop::GetGoneY(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float y = 0;
    view->GetTranslationY(&y);
    Int32 h = 0;
    IView::Probe(sceneRoot)->GetHeight(&h);
    *result = y - h;
    return NOERROR;
}

//===============================================================
// CSlide::CalculateSlideVerticalBottom::
//===============================================================

ECode CSlide::CalculateSlideVerticalBottom::GetGoneY(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float y = 0;
    view->GetTranslationY(&y);
    Int32 h = 0;
    IView::Probe(sceneRoot)->GetHeight(&h);
    *result = y + h;
    return NOERROR;
}

//===============================================================
// CSlide::
//===============================================================

const String CSlide::TAG("Slide");
AutoPtr<ITimeInterpolator> CSlide::sDecelerate;
AutoPtr<ITimeInterpolator> CSlide::sAccelerate;
const String CSlide::PROPNAME_SCREEN_POSITION("android:slide:screenPosition");

AutoPtr<ICalculateSlide> CSlide::sCalculateLeft = new CalculateSlideHorizontalLeft();

AutoPtr<ICalculateSlide> CSlide::sCalculateTop = new CalculateSlideVerticalTop();

AutoPtr<ICalculateSlide> CSlide::sCalculateRight = new CalculateSlideHorizontalRight();

AutoPtr<ICalculateSlide> CSlide::sCalculateBottom = new CalculateSlideVerticalBottom();

CAR_OBJECT_IMPL(CSlide)

CAR_INTERFACE_IMPL(CSlide, Visibility, ISlide)

CSlide::CSlide()
{
    CDecelerateInterpolator::New((ITimeInterpolator**)&sDecelerate);
    CAccelerateInterpolator::New((ITimeInterpolator**)&sAccelerate);
}

ECode CSlide::constructor()
{
    SetSlideEdge(IGravity::BOTTOM);
    return NOERROR;
}

ECode CSlide::constructor(
    /* [in] */ Int32 slideEdge)
{
    SetSlideEdge(slideEdge);
    return NOERROR;
}

ECode CSlide::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Visibility::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::Slide),
        ArraySize(R::styleable::Slide));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Int32 edge = 0;
    a->GetInt32(R::styleable::Slide_slideEdge, IGravity::BOTTOM, &edge);
    a->Recycle();
    SetSlideEdge(edge);
    return NOERROR;
}

void CSlide::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    AutoPtr<IView> view = ctv->mView;
    AutoPtr<ArrayOf<Int32> > position = ArrayOf<Int32>::Alloc(2);
    view->GetLocationOnScreen(position);
    AutoPtr<ICharSequence> pro_pos;
    CString::New(PROPNAME_SCREEN_POSITION, (ICharSequence**)&pro_pos);
    // ctv->mValues->Put(pro_pos, position);
}

ECode CSlide::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    Visibility::CaptureStartValues(transitionValues);
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CSlide::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    Visibility::CaptureEndValues(transitionValues);
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CSlide::SetSlideEdge(
    /* [in] */ Int32 slideEdge)
{
    assert(0 && "TODO");
    // switch (slideEdge) {
    //     case IGravity::LEFT:
    //         mSlideCalculator = sCalculateLeft;
    //         break;
    //     case IGravity::TOP:
    //         mSlideCalculator = sCalculateTop;
    //         break;
    //     case IGravity::RIGHT:
    //         mSlideCalculator = sCalculateRight;
    //         break;
    //     case IGravity::BOTTOM:
    //         mSlideCalculator = sCalculateBottom;
    //         break;
    //     default:
    //         return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }
    mSlideEdge = slideEdge;
    AutoPtr<ISidePropagation> propagation;
    CSidePropagation::New((ISidePropagation**)&propagation);
    propagation->SetSide(slideEdge);
    SetPropagation(ITransitionPropagation::Probe(propagation));
    return NOERROR;
}

ECode CSlide::GetSlideEdge(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSlideEdge;
    return NOERROR;
}

ECode CSlide::OnAppear(
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
    AutoPtr<ICharSequence> pro_pos;
    CString::New(PROPNAME_SCREEN_POSITION, (ICharSequence**)&pro_pos);
    AutoPtr<CTransitionValues> cEndValues = (CTransitionValues*)endValues;
    AutoPtr<ArrayOf<Int32> > position;
    assert(0 && "TODO");
//    cEndValues->mValues->Get(pro_pos, (ArrayOf<Int32>**)&position);
    Float endX = 0;
    view->GetTranslationX(&endX);
    Float endY = 0;
    view->GetTranslationY(&endY);
    Float startX = 0;
    mSlideCalculator->GetGoneX(sceneRoot, view, &startX);
    Float startY = 0;
    mSlideCalculator->GetGoneY(sceneRoot, view, &startY);
    AutoPtr<IAnimator> p = TranslationAnimationCreator::CreateAnimation(view, endValues, (*position)[0], (*position)[1],
                    startX, startY, endX, endY, sDecelerate);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSlide::OnDisappear(
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
    AutoPtr<ICharSequence> pro_pos;
    CString::New(PROPNAME_SCREEN_POSITION, (ICharSequence**)&pro_pos);
    AutoPtr<ArrayOf<Int32> > position;
    AutoPtr<CTransitionValues> cStartValues = (CTransitionValues*)startValues;
    assert(0 && "TODO");
//    cStartValues->mValues->Get(pro_pos, (ArrayOf<Int32>**)&position);
    Float startX = 0;
    view->GetTranslationX(&startX);
    Float startY = 0;
    view->GetTranslationY(&startY);
    Float endX = 0;
    mSlideCalculator->GetGoneX(sceneRoot, view, &endX);
    Float endY = 0;
    mSlideCalculator->GetGoneY(sceneRoot, view, &endY);
    AutoPtr<IAnimator> p = TranslationAnimationCreator::CreateAnimation(view, startValues, (*position)[0], (*position)[1],
                    startX, startY, endX, endY, sAccelerate);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSlide::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ISlide> changes;
    CSlide::New((ISlide**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CSlide::CloneImpl(
    /* [in] */ ISlide* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
