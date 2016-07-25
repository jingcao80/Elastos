
#include "CountryDetector.h"
#include "CCountryDetectorHelper.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CAR_INTERFACE_IMPL(CCountryDetectorHelper, Singleton, ICountryDetectorHelper)

CAR_SINGLETON_IMPL(CCountryDetectorHelper)

ECode CCountryDetectorHelper::RegisterForLocationUpdates(
    /* [in] */ IContext* context,
    /* [in] */ ILocationManager* locationManager)
{
    CountryDetector::RegisterForLocationUpdates(context, locationManager);
    return NOERROR;
}

ECode CCountryDetectorHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ ICountryDetector** detector)
{
    VALUE_NOT_NULL(detector);
    AutoPtr<ICountryDetector> cd = CountryDetector::GetInstance(context);
    *detector = cd;
    REFCOUNT_ADD(*detector)
    return NOERROR;
}

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos
