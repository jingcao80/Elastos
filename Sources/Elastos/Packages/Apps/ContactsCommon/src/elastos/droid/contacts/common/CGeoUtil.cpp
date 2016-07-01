
#include "GeoUtil.h"
#include "CGeoUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CAR_INTERFACE_IMPL(CGeoUtil, Singleton, IGeoUtil)

CAR_SINGLETON_IMPL(CGeoUtil)

ECode CGeoUtil::GetCurrentCountryIso(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
    VALUE_NOT_NULL(result);
    *result = GeoUtil::GetCurrentCountryIso(context);

    return NOERROR;
}

ECode CGeoUtil::GetGeocodedLocationFor(
    /* [in] */ IContext* context,
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* result)
{
     VALUE_NOT_NULL(result);
    *result = GeoUtil::GetGeocodedLocationFor(context, phoneNumber);

    return NOERROR;
}

} // Common
} // Contacts
} // Apps
} // Elastos
