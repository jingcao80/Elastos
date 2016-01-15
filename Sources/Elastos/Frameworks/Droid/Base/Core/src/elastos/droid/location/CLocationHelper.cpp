
#include "elastos/droid/location/CLocationHelper.h"
#include "elastos/droid/location/Location.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CLocationHelper, Singleton, ILocationHelper)

CAR_SINGLETON_IMPL(CLocationHelper)

ECode CLocationHelper::Convert(
    /* [in] */ Double coordinate,
    /* [in] */ Int32 outputType,
    /* [out] */ String* str)
{
    return Location::Convert(coordinate, outputType, str);
}

ECode CLocationHelper::Convert(
    /* [in] */ const String& coordinate,
    /* [out] */ Double* d)
{
    return Location::Convert(coordinate, d);
}

ECode CLocationHelper::DistanceBetween(
    /* [in] */ Double startLatitude,
    /* [in] */ Double startLongitude,
    /* [in] */ Double endLatitude,
    /* [in] */ Double endLongitude,
    /* [in] */ ArrayOf<Float>* results)
{
    return Location::DistanceBetween(startLatitude, startLongitude,endLatitude,endLongitude,results);
}

}
}
}