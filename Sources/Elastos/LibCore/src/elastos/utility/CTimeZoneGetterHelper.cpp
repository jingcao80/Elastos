
#include "CTimeZoneGetterHelper.h"
#include "TimeZoneGetter.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CTimeZoneGetterHelper, Singleton, ITimeZoneGetterHelper)

CAR_SINGLETON_IMPL(CTimeZoneGetterHelper)

ECode CTimeZoneGetterHelper::GetInstance(
    /* [out] */ ITimeZoneGetter** instance)
{
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ITimeZoneGetter> getter = TimeZoneGetter::GetInstance();
    *instance = getter;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CTimeZoneGetterHelper::SetInstance(
    /* [in] */ ITimeZoneGetter* instance)
{
    return TimeZoneGetter::SetInstance(instance);
}

} // namespace Utility
} // namespace Elastos
