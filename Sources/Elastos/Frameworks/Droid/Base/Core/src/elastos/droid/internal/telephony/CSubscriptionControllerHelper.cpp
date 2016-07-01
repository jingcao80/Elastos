
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSubscriptionControllerHelper, Singleton, ISubscriptionControllerHelper)

CAR_SINGLETON_IMPL(CSubscriptionControllerHelper)

ECode CSubscriptionControllerHelper::Init(
    /* [in] */ IPhone* phone,
    /* [out] */ ISubscriptionController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionController> res = SubscriptionController::Init(phone);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSubscriptionControllerHelper::Init(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ ISubscriptionController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionController> res = SubscriptionController::Init(c, ci);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSubscriptionControllerHelper::GetInstance(
    /* [out] */ ISubscriptionController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionController> res = SubscriptionController::GetInstance();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony