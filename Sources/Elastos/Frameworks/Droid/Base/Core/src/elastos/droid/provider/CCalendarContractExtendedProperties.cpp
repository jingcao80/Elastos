#include "elastos/droid/provider/CCalendarContractExtendedProperties.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCalendarContractExtendedProperties)

CAR_INTERFACE_IMPL_4(CCalendarContractExtendedProperties, Singleton
    , ICalendarContractExtendedProperties
    , IBaseColumns
    , ICalendarContractExtendedPropertiesColumns
    , ICalendarContractEventsColumns)

ECode CCalendarContractExtendedProperties::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CalendarContractExtendedProperties::GetCONTENT_URI(uri);
}

} //Provider
} //Droid
} //Elastos