
#include "Elastos.Droid.View.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/transition/VisibilityPropagation.h"
#include "elastos/droid/transition/CTransitionValues.h"

#include <elastos/core/Math.h>

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// VisibilityPropagation::
//===============================================================

const String VisibilityPropagation::PROPNAME_VISIBILITY("android:visibilityPropagation:visibility");

const String VisibilityPropagation::PROPNAME_VIEW_CENTER ("android:visibilityPropagation:center");

AutoPtr<ArrayOf<String> > VisibilityPropagation::VISIBILITY_PROPAGATION_VALUES = ArrayOf<String>::Alloc(2);

CAR_INTERFACE_IMPL_2(VisibilityPropagation, Object, ITransitionPropagation, IVisibilityPropagation)

VisibilityPropagation::VisibilityPropagation()
{
    (*VISIBILITY_PROPAGATION_VALUES)[0] = PROPNAME_VISIBILITY;
    (*VISIBILITY_PROPAGATION_VALUES)[1] = PROPNAME_VIEW_CENTER;
}

ECode VisibilityPropagation::CaptureValues(
    /* [in] */ ITransitionValues* values)
{
    AutoPtr<CTransitionValues> cv = (CTransitionValues*)values;
    AutoPtr<IView> view = cv->mView;
    AutoPtr<ICharSequence> pro_vis;
    CString::New(PROPNAME_VISIBILITY, (ICharSequence**)&pro_vis);
    AutoPtr<IInterface> pVis;
    cv->mValues->Get(pro_vis, (IInterface**)&pVis);
    AutoPtr<IInteger32> visibility = IInteger32::Probe(pVis);
    if (visibility == NULL) {
        Int32 v = 0;
        view->GetVisibility(&v);
        CInteger32::New(v, (IInteger32**)&visibility);
    }
    cv->mValues->Put(pro_vis, visibility);
    AutoPtr<ArrayOf<Int32> > loc = ArrayOf<Int32>::Alloc(2);
    view->GetLocationOnScreen(loc);
    Float x, y;
    Int32 w = 0, h = 0;
    view->GetTranslationX(&x);
    (*loc)[0] += Elastos::Core::Math::Round(x);
    view->GetWidth(&w);
    (*loc)[0] += w / 2;
    view->GetTranslationY(&y);
    (*loc)[1] += Elastos::Core::Math::Round(y);
    view->GetHeight(&h);
    (*loc)[1] += h / 2;
    assert(0 && "TODO");
//    cv->mValues->Put(PROPNAME_VIEW_CENTER, loc);
    return NOERROR;
}

ECode VisibilityPropagation::GetPropagationProperties(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = VISIBILITY_PROPAGATION_VALUES;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VisibilityPropagation::GetViewVisibility(
    /* [in] */ ITransitionValues* values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (values == NULL) {
        *result = IView::GONE;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cv = (CTransitionValues*)values;
    AutoPtr<ICharSequence> pro_vis;
    CString::New(PROPNAME_VISIBILITY, (ICharSequence**)&pro_vis);
    AutoPtr<IInterface> pVis;
    cv->mValues->Get(pro_vis, (IInterface**)&pVis);
    AutoPtr<IInteger32> visibility = IInteger32::Probe(pVis);
    if (visibility == NULL) {
        *result = IView::GONE;
        return NOERROR;
    }
    visibility->GetValue(result);
    return NOERROR;
}

ECode VisibilityPropagation::GetViewX(
    /* [in] */ ITransitionValues* values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetViewCoordinate(values, 0);
    return NOERROR;
}

ECode VisibilityPropagation::GetViewY(
    /* [in] */ ITransitionValues* values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetViewCoordinate(values, 1);
    return NOERROR;
}

Int32 VisibilityPropagation::GetViewCoordinate(
    /* [in] */ ITransitionValues* values,
    /* [in] */ Int32 coordinateIndex)
{
    if (values == NULL) {
        return -1;
    }

    AutoPtr<CTransitionValues> cv = (CTransitionValues*)values;
    AutoPtr<ICharSequence> pro_vc;
    CString::New(PROPNAME_VIEW_CENTER, (ICharSequence**)&pro_vc);
    AutoPtr<IInterface> pVc;
    cv->mValues->Get(pro_vc, (IInterface**)&pVc);
    assert(0 && "TODO");
    AutoPtr<ArrayOf<Int32> > coordinates;// = pVc;
    if (coordinates == NULL) {
        return -1;
    }

    return (*coordinates)[coordinateIndex];
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
