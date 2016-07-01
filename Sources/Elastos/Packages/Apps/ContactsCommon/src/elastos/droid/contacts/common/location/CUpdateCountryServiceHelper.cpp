
#include "UpdateCountryService.h"
#include "CUpdateCountryServiceHelper.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CAR_INTERFACE_IMPL(CUpdateCountryServiceHelper, Singleton, IUpdateCountryServiceHelper)

CAR_SINGLETON_IMPL(CUpdateCountryServiceHelper)

ECode CUpdateCountryServiceHelper::UpdateCountry(
    /* [in] */ IContext* context,
    /* [in] */ ILocation* location)
{
    UpdateCountryServiceHelper::UpdateCountry(context, location);
    return NOERROR;
}

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos
