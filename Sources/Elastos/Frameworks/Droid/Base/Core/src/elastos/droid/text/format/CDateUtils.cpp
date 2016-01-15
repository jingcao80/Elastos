
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CAR_INTERFACE_IMPL(CDateUtils, Singleton, IDateUtils)

CAR_SINGLETON_IMPL(CDateUtils)

ECode CDateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = DateUtils::FormatDateRange(context, startMillis, endMillis, flags);
    return NOERROR;
}

ECode CDateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Elastos::Utility::IFormatter* formatter,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags,
    /* [out] */ Elastos::Utility::IFormatter** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = DateUtils::FormatDateRange(context, formatter, startMillis, endMillis, flags);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Elastos::Utility::IFormatter* formatter,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags,
    /* [in] */ const String& timeZone,
    /* [out] */ Elastos::Utility::IFormatter** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = DateUtils::FormatDateRange(context, formatter, startMillis, endMillis, flags, timeZone);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::FormatDuration(
    /* [in] */ Int64 millis,
    /* [out] */ ICharSequence** rst)
{
    VALIDATE_NOT_NULL(rst);
    AutoPtr<ICharSequence> temp = DateUtils::FormatDuration(millis);
    *rst = temp;
    REFCOUNT_ADD(*rst);
    return NOERROR;
}

ECode CDateUtils::FormatDateTime(
    /* [in] */ IContext* context,
    /* [in] */ Int64 millis,
    /* [in] */ Int32 flags,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::FormatDateTime(context, millis, flags));
    return NOERROR;
}

ECode CDateUtils::FormatElapsedTime(
    /* [in] */ Int64 elapsedSeconds,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::FormatElapsedTime(elapsedSeconds));
    return NOERROR;
}

ECode CDateUtils::FormatElapsedTime(
    /* [in] */ const String& recycle,
    /* [in] */ Int64 elapsedSeconds,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::FormatElapsedTime(recycle, elapsedSeconds));
    return NOERROR;
}

ECode CDateUtils::FormatSameDayTime(
    /* [in] */ Int64 then,
    /* [in] */ Int64 now,
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::FormatSameDayTime(then, now, dateStyle, timeStyle);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetAMPMString(
    /* [in] */ Int32 ampm,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::GetAMPMString(ampm));
    return NOERROR;
}

ECode CDateUtils::GetDayOfWeekString(
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 abbrev,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::GetDayOfWeekString(dayOfWeek, abbrev));
    return NOERROR;
}

ECode CDateUtils::GetMonthString(
    /* [in] */ Int32 month,
    /* [in] */ Int32 abbrev,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::GetMonthString(month, abbrev));
    return NOERROR;
}

ECode CDateUtils::GetRelativeDateTimeString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 time,
    /* [in] */ Int64 minResolution,
    /* [in] */ Int64 transitionResolution,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeDateTimeString(c, time, minResolution, transitionResolution, flags);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 startTime,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeTimeSpanString(startTime);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 time,
    /* [in] */ Int64 now,
    /* [in] */ Int64 minResolution,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeTimeSpanString(time, now, minResolution);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 time,
    /* [in] */ Int64 now,
    /* [in] */ Int64 minResolution,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeTimeSpanString(time, now, minResolution, flags);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetRelativeTimeSpanString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 millis,
    /* [in] */ Boolean withPreposition,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeTimeSpanString(c, millis, withPreposition);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetRelativeTimeSpanString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 millis,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> cRet = DateUtils::GetRelativeTimeSpanString(c, millis);
    *ret = cRet;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDateUtils::GetStandaloneMonthString(
    /* [in] */ Int32 month,
    /* [in] */ Int32 abbrev,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::GetStandaloneMonthString(month, abbrev));
    return NOERROR;
}

ECode CDateUtils::IsToday(
    /* [in] */ Int64 when,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (DateUtils::IsToday(when));
    return NOERROR;
}

ECode CDateUtils::GetSameMonthTable(
    /* [out, callee] */ ArrayOf<Int32>** sameMonthTable)
{
    VALIDATE_NOT_NULL(sameMonthTable)
    *sameMonthTable = DateUtils::sSameMonthTable;
    REFCOUNT_ADD(*sameMonthTable)
    return NOERROR;
}

ECode CDateUtils::GetSameYearTable(
    /* [out, callee] */ ArrayOf<Int32>** sameYearTable)
{
    VALIDATE_NOT_NULL(sameYearTable)
    *sameYearTable = DateUtils::sSameYearTable;
    REFCOUNT_ADD(*sameYearTable)
    return NOERROR;
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

