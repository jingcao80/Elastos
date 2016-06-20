
#include "contact/ContactUpdateService.h"
#include "contact/CContactUpdateServiceHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Contact {

CAR_INTERFACE_IMPL(CContactUpdateServiceHelper, Singleton, IContactUpdateServiceHelper);

CAR_SINGLETON_IMPL(CContactUpdateServiceHelper);

ECode CContactUpdateServiceHelper::CreateSetSuperPrimaryIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);

    AutoPtr<IIntent> result = ContactUpdateService::CreateSetSuperPrimaryIntent(
            context, dataId);
    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

} // Contact
} // Dialer
} // Apps
} // Elastos
