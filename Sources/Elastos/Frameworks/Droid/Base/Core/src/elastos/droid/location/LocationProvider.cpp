
#include "elastos/droid/location/LocationProvider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(LocationProvider, Object, ILocationProvider)

LocationProvider::LocationProvider()
{
}

ECode LocationProvider::constructor(
    /* [in] */ const String& name,
    /* [in] */ IProviderProperties* properties)
{
    Int32 length = name.GetLength();
    if (name.RegionMatches(0, ILocationProvider::BAD_CHARS_REGEX, 0, length)) {
        Logger::E("LocationProvider", "provider name contains illegal character: %s", name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mName = name;
    mProperties = properties;
    return NOERROR;
}

ECode LocationProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode LocationProvider::MeetsCriteria(
    /* [in] */ ICriteria* criteria,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(criteria);
    VALIDATE_NOT_NULL(result);
    *result = PropertiesMeetCriteria(mName, mProperties, criteria);
    return NOERROR;
}

Boolean LocationProvider::PropertiesMeetCriteria(
    /* [in] */ const String& name,
    /* [in] */ IProviderProperties* properties,
    /* [in] */ ICriteria* criteria)
{
    Int32 value, value2;
    Boolean bValue, bValue2;
    if (ILocationManager::PASSIVE_PROVIDER.Equals(name)) {
        // passive provider never matches
        return FALSE;
    }
    if (properties == NULL) {
        // unfortunately this can happen for provider in remote services
        // that have not finished binding yet
        return FALSE;
    }
    if ( (criteria->GetAccuracy(&value), value != ICriteria::Criteria_NO_REQUIREMENT) && (properties->GetAccuracy(&value2), value < value2) ){
        return FALSE;
    }
    if ( (criteria->GetPowerRequirement(&value), value != ICriteria::Criteria_NO_REQUIREMENT) && (properties->GetPowerRequirement(&value2), value < value2) ) {
        return FALSE;
    }
    if ( (criteria->IsAltitudeRequired(&bValue), bValue) && (properties->GetSupportsAltitude(&bValue2), !bValue2) ) {
        return FALSE;
    }
    if ( (criteria->IsSpeedRequired(&bValue), bValue) && (properties->GetSupportsSpeed(&bValue2), !bValue2) ) {
        return FALSE;
    }
    if ( (criteria->IsBearingRequired(&bValue), bValue) && (properties->GetSupportsBearing(&bValue2), !bValue2) ) {
        return FALSE;
    }
    if ( (criteria->IsCostAllowed(&bValue), bValue) && (properties->GetHasMonetaryCost(&bValue2), !bValue2) ) {
        return FALSE;
    }
    return TRUE;
}

ECode LocationProvider::RequiresNetwork(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetRequiresNetwork(result);
}

ECode LocationProvider::RequiresSatellite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetRequiresSatellite(result);
}

ECode LocationProvider::RequiresCell(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetRequiresCell(result);
}

ECode LocationProvider::HasMonetaryCost(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetHasMonetaryCost(result);
}

ECode LocationProvider::SupportsAltitude(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetSupportsAltitude(result);
}

ECode LocationProvider::SupportsSpeed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetSupportsSpeed(result);
}

ECode LocationProvider::SupportsBearing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mProperties->GetSupportsBearing(result);
}

ECode LocationProvider::GetPowerRequirement(
    /* [out] */ Int32* pr)
{
    VALIDATE_NOT_NULL(pr);
    return mProperties->GetPowerRequirement(pr);
}

ECode LocationProvider::GetAccuracy(
    /* [out] */ Int32* accuracy)
{
    VALIDATE_NOT_NULL(accuracy);
    return mProperties->GetAccuracy(accuracy);
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos