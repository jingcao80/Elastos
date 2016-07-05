#include "elastos/droid/internal/telephony/CCallManagerHelper.h"
#include "elastos/droid/internal/telephony/CallManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallManagerHelper, Singleton, ICallManagerHelper)
CAR_SINGLETON_IMPL(CCallManagerHelper)

ECode CCallManagerHelper::GetInstance(
    /* [out] */ ICallManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CallManager> cm = CallManager::GetInstance();
    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;

}

ECode CCallManagerHelper::IsSamePhone(
    /* [in] */ IPhone* p1,
    /* [in] */ IPhone* p2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CallManager::IsSamePhone(p1, p2);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
