
#include "elastos/droid/location/CLocationProviderHelper.h"
#include "elastos/droid/location/LocationProvider.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_SINGLETON_IMPL(CLocationProviderHelper)

CAR_INTERFACE_IMPL(CLocationProviderHelper, Singleton, ILocationProviderHelper)

ECode CLocationProviderHelper::PropertiesMeetCriteria(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties,
        /* [in] */ ICriteria* criteria,
        /* [out] */ Boolean* rst)
{
    *rst = LocationProvider::PropertiesMeetCriteria(name, properties, criteria);
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
