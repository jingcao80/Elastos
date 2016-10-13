
#include "CAttributeTypeAndValueComparator.h"

using Elastos::Core::EIID_IComparator;
using Elastos::IO::EIID_ISerializable;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CAttributeTypeAndValueComparator)
CAR_INTERFACE_IMPL_2(CAttributeTypeAndValueComparator, Object, IComparator, ISerializable)

ECode CAttributeTypeAndValueComparator::Compare(
    /* [in] */ IInterface * atav1,
    /* [in] */ IInterface * atav2,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    if (atav1 == atav2) {
        *result = 0;
        return NOERROR;
    }

    AutoPtr<IObjectIdentifier> type1;
    AutoPtr<IObjectIdentifier> type2;
    IAttributeTypeAndValue::Probe(atav1)->GetType((IObjectIdentifier**)&type1);
    IAttributeTypeAndValue::Probe(atav2)->GetType((IObjectIdentifier**)&type2);
    String kw1;
    type1->GetName(&kw1);
    String kw2;
    type2->GetName(&kw2);
    if (kw1 != NULL && kw2 == NULL) {
        *result = -1;
        return NOERROR;
    }
    if (kw1 == NULL && kw2 != NULL) {
        *result = 1;
        return NOERROR;
    }
    if (kw1 != NULL && kw2 != NULL) {
        *result = kw1.Compare(kw2);
        return NOERROR;
    }

    *result = CompateOids(type1, type2);
    return NOERROR;
}

Int32 CAttributeTypeAndValueComparator::CompateOids(
    /* [in] */ IObjectIdentifier* oid1,
    /* [in] */ IObjectIdentifier* oid2)
{
    if (oid1 == oid2) {
        return 0;
    }

    AutoPtr<ArrayOf<Int32> > ioid1;
    oid1->GetOid((ArrayOf<Int32>**)&ioid1);
    AutoPtr<ArrayOf<Int32> > ioid2;
    oid2->GetOid((ArrayOf<Int32>**)&ioid2);
    Int32 min = ioid1->GetLength() < ioid2->GetLength() ? ioid1->GetLength() : ioid2->GetLength();
    for (Int32 i = 0; i < min; ++i) {
        if ((*ioid1)[i] < (*ioid2)[i]) {
            return -1;
        }
        if ((*ioid1)[i] > (*ioid2)[i]) {
            return 1;
        }
        if ((i + 1) == ioid1->GetLength() && (i + 1) < ioid2->GetLength()) {
            return -1;
        }
        if ((i + 1) < ioid1->GetLength() && (i + 1) == ioid2->GetLength()) {
            return 1;
        }
    }
    return 0;
}

}
}
}
}
}

