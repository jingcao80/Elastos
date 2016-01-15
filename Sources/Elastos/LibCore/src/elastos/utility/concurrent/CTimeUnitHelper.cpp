
#include "CTimeUnitHelper.h"
#include "TimeUnit.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CAR_INTERFACE_IMPL(CTimeUnitHelper, Singleton, ITimeUnitHelper)

CAR_SINGLETON_IMPL(CTimeUnitHelper)

ECode CTimeUnitHelper::GetNANOSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new NANOSECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMICROSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MICROSECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMILLISECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MILLISECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetSECONDS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new SECONDS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetMINUTES(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new MINUTES();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetHOURS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new HOURS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

ECode CTimeUnitHelper::GetDAYS(
    /* [out] */ ITimeUnit** unit)
{
    VALIDATE_NOT_NULL(unit);
    *unit = new DAYS();
    REFCOUNT_ADD(*unit);
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
