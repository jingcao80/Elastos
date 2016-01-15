
#include "CDateHelper.h"
#include "Date.h"

namespace Elastos {
namespace Utility {

CAR_SINGLETON_IMPL(CDateHelper)
CAR_INTERFACE_IMPL(CDateHelper, Singleton, IDateHelper)

ECode CDateHelper::Parse(
    /* [in] */ const String& string,
    /* [out] */ Int64* value)
{
    return Date::Parse(string, value);
}

ECode CDateHelper::UTC(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Date::UTC(year, month, day, hour, minute, second);
    return NOERROR;
}

} // Utility
} // Elastos
