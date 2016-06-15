#include "elastos/droid/telephony/CSmsManagerHelper.h"
#include "elastos/droid/telephony/SmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsManagerHelper, Singleton, ISmsManagerHelper)
CAR_SINGLETON_IMPL(CSmsManagerHelper)

ECode CSmsManagerHelper::GetDefault(
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetDefault();
    *result = smsManager;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsManagerHelper::GetSmsManagerForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber(subId);
    *result = smsManager;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsManagerHelper::GetDefaultSmsSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmsManager::GetDefaultSmsSubId();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
