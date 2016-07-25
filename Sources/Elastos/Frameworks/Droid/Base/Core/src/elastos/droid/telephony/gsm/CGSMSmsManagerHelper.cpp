#include "elastos/droid/telephony/gsm/CGSMSmsManagerHelper.h"
#include "elastos/droid/telephony/gsm/GSMSmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGSMSmsManagerHelper, Singleton, ISmsManagerHelper);

CAR_SINGLETON_IMPL(CGSMSmsManagerHelper);

ECode CGSMSmsManagerHelper::GetDefault(
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> sm = GSMSmsManager::GetDefault();
    *result = sm;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos
