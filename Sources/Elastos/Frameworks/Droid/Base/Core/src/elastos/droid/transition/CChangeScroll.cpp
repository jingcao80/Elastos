
#include "elastos/droid/transition/CChangeScroll.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CTransitionUtils.h"
#include "elastos/droid/animation/ObjectAnimator.h"

using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Transition::CTransitionUtils;

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CChangeScroll::
//===============================================================
const String CChangeScroll::PROPNAME_SCROLL_X("android:changeScroll:x");
const String CChangeScroll::PROPNAME_SCROLL_Y("android:changeScroll:y");

CAR_OBJECT_IMPL(CChangeScroll)

ECode CChangeScroll::constructor()
{
    return NOERROR;
}

ECode CChangeScroll::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Transition::constructor(context, attrs);
}

ECode CChangeScroll::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    return CaptureValues(transitionValues);
}

ECode CChangeScroll::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    return CaptureValues(transitionValues);
}

ECode CChangeScroll::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ct = (CTransitionValues*)transitionValues;
    Int32 x = 0, y = 0;
    ct->mView->GetScrollX(&x);
    ct->mView->GetScrollY(&y);
    AutoPtr<IInteger32> iX, iY;
    CInteger32::New(x, (IInteger32**)&iX);
    CInteger32::New(y, (IInteger32**)&iY);
    AutoPtr<ICharSequence> pro_X;
    CString::New(PROPNAME_SCROLL_X, (ICharSequence**)&pro_X);
    AutoPtr<ICharSequence> pro_Y;
    CString::New(PROPNAME_SCROLL_Y, (ICharSequence**)&pro_Y);
    ct->mValues->Put(pro_X, iX);
    ct->mValues->Put(pro_Y, iY);
    return NOERROR;
}

ECode CChangeScroll::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    if (startValues == NULL || endValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> sV = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> eV = (CTransitionValues*)endValues;
    AutoPtr<IView> view = eV->mView;

    AutoPtr<ICharSequence> pro_X;
    CString::New(PROPNAME_SCROLL_X, (ICharSequence**)&pro_X);
    AutoPtr<ICharSequence> pro_Y;
    CString::New(PROPNAME_SCROLL_Y, (ICharSequence**)&pro_Y);
    AutoPtr<IInterface> sX, eX, sY, eY;
    sV->mValues->Get(pro_X, (IInterface**)&sX);
    eV->mValues->Get(pro_X, (IInterface**)&eX);
    sV->mValues->Get(pro_Y, (IInterface**)&sY);
    eV->mValues->Get(pro_Y, (IInterface**)&eY);
    Int32 startX, endX, startY, endY;
    IInteger32::Probe(sX)->GetValue(&startX);
    IInteger32::Probe(eX)->GetValue(&endX);
    IInteger32::Probe(sY)->GetValue(&startY);
    IInteger32::Probe(eY)->GetValue(&endY);
    AutoPtr<IAnimator> scrollXAnimator;
    AutoPtr<IAnimator> scrollYAnimator;
    if (startX != endX) {
        view->SetScrollX(startX);
        AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
        (*arr)[0] = startX;
        (*arr)[1] = endX;
        AutoPtr<IObjectAnimator> XAnimator = ObjectAnimator::OfInt32(view, String("scrollX"), arr);
        scrollXAnimator = IAnimator::Probe(XAnimator);
    }
    if (startY != endY) {
        view->SetScrollY(startY);
        AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
        (*arr)[0] = startX;
        (*arr)[1] = endX;
        AutoPtr<IObjectAnimator> YAnimator = ObjectAnimator::OfInt32(view, String("scrollY"), arr);
        scrollYAnimator = IAnimator::Probe(YAnimator);
    }
    AutoPtr<ITransitionUtils> utils;
    CTransitionUtils::AcquireSingleton((ITransitionUtils**)&utils);
    return utils->MergeAnimators(scrollXAnimator, scrollYAnimator, result);
}

ECode CChangeScroll::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransition> changes;
    CChangeScroll::New((ITransition**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CChangeScroll::CloneImpl(
    /* [in] */ ITransition* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
