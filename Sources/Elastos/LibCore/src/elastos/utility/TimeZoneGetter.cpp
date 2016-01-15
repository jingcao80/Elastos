
#include "TimeZoneGetter.h"
#include <cutils/properties.h>

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(TimeZoneGetter::DefaultTimeZoneGetter, Object, ITimeZoneGetter);

ECode TimeZoneGetter::DefaultTimeZoneGetter::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);

    char buf[PROP_VALUE_MAX + 1];

    int len = property_get("persist.sys.timezone", buf, "");
    if (len >= 0) {
        *id = String(buf);
    }
    else {
        *id = String("");
    }

    return NOERROR;
}

AutoPtr<ITimeZoneGetter> TimeZoneGetter::sInstance;

AutoPtr<ITimeZoneGetter> TimeZoneGetter::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new DefaultTimeZoneGetter();
    }

    return sInstance;
}

ECode TimeZoneGetter::SetInstance(
    /* [in] */ ITimeZoneGetter* instance)
{
    if (instance == NULL) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    sInstance = instance;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
