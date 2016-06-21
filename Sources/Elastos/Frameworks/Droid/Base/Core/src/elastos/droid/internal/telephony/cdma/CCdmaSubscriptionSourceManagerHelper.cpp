#include "elastos/droid/internal/telephony/cdma/CCdmaSubscriptionSourceManagerHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaSubscriptionSourceManagerHelper, Singleton, ICdmaSubscriptionSourceManagerHelper)
CAR_SINGLETON_IMPL(CCdmaSubscriptionSourceManagerHelper)


ECode CCdmaSubscriptionSourceManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ ICdmaSubscriptionSourceManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaSubscriptionSourceManager> sm = CdmaSubscriptionSourceManager::GetInstance(context, ci, h, what, obj);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSubscriptionSourceManagerHelper::GetDefault(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CdmaSubscriptionSourceManager::GetDefault(context);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
