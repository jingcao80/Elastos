
#include "elastos/droid/content/CIntentFilterComparison.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentFilterComparison, Object, IIntentFilterComparison)

CAR_OBJECT_IMPL(CIntentFilterComparison)

CIntentFilterComparison::CIntentFilterComparison()
    : mHashCode(0)
{}

ECode CIntentFilterComparison::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentFilterComparison::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    IIntentFilterComparison* cmp = IIntentFilterComparison::Probe(obj);
    if (cmp != NULL) {
        AutoPtr<IIntent> otherIntent;
        FAIL_RETURN(cmp->GetIntent((IIntent**)&otherIntent))
        return mIntent->FilterEquals(otherIntent, isEqual);
    }
    *isEqual = FALSE;
    return NOERROR;
}

ECode CIntentFilterComparison::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mHashCode;
    return NOERROR;
}

ECode CIntentFilterComparison::constructor(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent)
    mIntent = intent;
    FAIL_RETURN(intent->FilterHashCode(&mHashCode));
    return NOERROR;
}

}
}
}

