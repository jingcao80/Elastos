
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CPhoneAccountHelper.h"
#include "elastos/droid/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CPhoneAccountHelper::
//===============================================================
CAR_SINGLETON_IMPL(CPhoneAccountHelper)

CAR_INTERFACE_IMPL(CPhoneAccountHelper, Singleton, IPhoneAccountHelper)

ECode CPhoneAccountHelper::Builder(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ ICharSequence* label,
    /* [out] */ IPhoneAccountBuilder** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IPhoneAccountBuilder> p = PhoneAccount::_Builder(accountHandle, label);
    *res = p;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos