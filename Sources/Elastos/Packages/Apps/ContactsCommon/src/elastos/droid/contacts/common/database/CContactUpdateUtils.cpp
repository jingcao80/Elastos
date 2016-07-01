
#include "CContactUpdateUtils.h"
#include "ContactUpdateUtils.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

CAR_INTERFACE_IMPL(CContactUpdateUtils, Singleton, IContactUpdateUtils)

CAR_SINGLETON_IMPL(CContactUpdateUtils)

ECode CContactUpdateUtils::SetSuperPrimary(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId)
{
    ContactUpdateUtils::SetSuperPrimary(context, dataId);

    return NOERROR;
}

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos
