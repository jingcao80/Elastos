
#include "elastos/droid/transition/CTransitionValues.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionValues::
//===============================================================
CAR_OBJECT_IMPL(CTransitionValues)

CAR_INTERFACE_IMPL(CTransitionValues, Object, ITransitionValues)

ECode CTransitionValues::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ITransitionValues::Probe(other) != NULL) {
        AutoPtr<ITransitionValues> p = ITransitionValues::Probe(other);
        AutoPtr<CTransitionValues> cp = (CTransitionValues*)p.Get();
        if (Object::Equals(mView->Probe(EIID_IInterface), cp->mView->Probe(EIID_IInterface))) {
            if (Object::Equals(mValues->Probe(EIID_IInterface), cp->mValues->Probe(EIID_IInterface))) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CTransitionValues::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc)

    Int32 vHc = 0, vlHc = 0;
//    mView->GetHashCode(&vHc);
    mValues->GetHashCode(&vlHc);
    *hc = 31 * vHc + vlHc;
    return NOERROR;
}

ECode CTransitionValues::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 hc = 0;
    GetHashCode(&hc);
    StringBuilder returnValue("TransitionValues@");
    returnValue += StringUtils::ToHexString(hc);
    returnValue += ":\n";
    returnValue += "    view = ";
    returnValue += mView;
    returnValue += "\n";
    returnValue += "    values:";
    AutoPtr<ISet> st;
    mValues->GetKeySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean b = FALSE;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IInterface> v;
        mValues->Get(p, (IInterface**)&v);
        returnValue += "    ";
        returnValue += p;
        returnValue += ": ";
        returnValue += v;
        returnValue += "\n";
    }
    *result = returnValue.ToString();
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
