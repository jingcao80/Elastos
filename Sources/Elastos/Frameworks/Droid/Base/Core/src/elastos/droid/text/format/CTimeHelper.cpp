#include "elastos/droid/text/format/CTimeHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/format/CTime.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CAR_SINGLETON_IMPL(CTimeHelper)

CAR_INTERFACE_IMPL(CTimeHelper, Singleton, ITimeHelper)

ECode CTimeHelper::Compare(
    /* [in] */ ITime* a,
    /* [in] */ ITime* b,
    /* [out] */ Int32* ret)
{
    return CTime::Compare(a, b, ret);
}

ECode CTimeHelper::GetCurrentTimezone(
    /* [out] */ String * ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetCurrentTimezone();
    return NOERROR;
}

ECode CTimeHelper::IsEpoch(
    /* [in] */ ITime* time,
    /* [out] */ Boolean * ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::IsEpoch(time);
    return NOERROR;
}

ECode CTimeHelper::GetJulianDay(
    /* [in] */ Int64 millis,
    /* [in] */ Int64 gmtoff,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetJulianDay(millis, gmtoff);
    return NOERROR;
}

ECode CTimeHelper::GetWeeksSinceEpochFromJulianDay(
    /* [in] */ Int32 julianDay,
    /* [in] */ Int32 firstDayOfWeek,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetWeeksSinceEpochFromJulianDay(julianDay, firstDayOfWeek);
    return NOERROR;
}

ECode CTimeHelper::GetJulianMondayFromWeeksSinceEpoch(
    /* [in] */ Int32 week,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CTime::GetJulianMondayFromWeeksSinceEpoch(week);
    return NOERROR;
}





} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

