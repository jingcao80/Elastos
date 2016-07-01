
#include "elastos/droid/internal/telephony/CSubscriptionHelperHelper.h"
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSubscriptionHelperHelper, Singleton, ISubscriptionHelperHelper)

CAR_SINGLETON_IMPL(CSubscriptionHelperHelper)

ECode CSubscriptionHelperHelper::Init(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ ISubscriptionHelper** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionHelper> res = SubscriptionHelper::Init(c, ci);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSubscriptionHelperHelper::GetInstance(
    /* [out] */ ISubscriptionHelper** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionHelper> res = SubscriptionHelper::GetInstance();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony