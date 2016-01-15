
#include "CDateIntervalFormat.h"
#include "DateIntervalFormat.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CDateIntervalFormat)

CAR_INTERFACE_IMPL(CDateIntervalFormat, Singleton, IDateIntervalFormat)

ECode CDateIntervalFormat::FormatDateRange(
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags,
    /* [in] */ const String& olsonId,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst);

    *rst = DateIntervalFormat::FormatDateRange(startMs, endMs, flags, olsonId);
    return NOERROR;
}

ECode CDateIntervalFormat::FormatDateRange(
    /* [in] */ ILocale* locale,
    /* [in] */ ITimeZone* tz,
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst);

    *rst = DateIntervalFormat::FormatDateRange(locale, tz, startMs, endMs, flags);
    return NOERROR;
}

} // ICU
} // Libcore
