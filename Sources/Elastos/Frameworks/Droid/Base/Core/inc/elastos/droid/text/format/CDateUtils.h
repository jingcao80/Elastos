#ifndef __ELASTOS_DROID_TEXT_FORMAT_CDATEUTILS_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CDATEUTILS_H__

#include "_Elastos_Droid_Text_Format_CDateUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CDateUtils)
    , public Singleton
    , public IDateUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Assign(
        /* [in] */ ICalendar* rval,
        /* [in] */ ICalendar* lval);

    CARAPI FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags,
        /* [out] */ String* ret);

    CARAPI FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Utility::IFormatter* formatter,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags,
        /* [out] */ Elastos::Utility::IFormatter** ret);

    CARAPI FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Utility::IFormatter* formatter,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags,
        /* [in] */ const String& timeZone,
        /* [out] */ Elastos::Utility::IFormatter** ret);

    CARAPI FormatDateTime(
        /* [in] */ IContext* context,
        /* [in] */ Int64 millis,
        /* [in] */ Int32 flags,
        /* [out] */ String* ret);

    CARAPI FormatDuration(
        /* [in] */ Int64 millis,
        /* [out] */ ICharSequence** rst);

    CARAPI FormatElapsedTime(
        /* [in] */ Int64 elapsedSeconds,
        /* [out] */ String* ret);

    CARAPI FormatElapsedTime(
        /* [in] */ const String& recycle,
        /* [in] */ Int64 elapsedSeconds,
        /* [out] */ String* ret);

    CARAPI FormatSameDayTime(
        /* [in] */ Int64 then,
        /* [in] */ Int64 now,
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [out] */ ICharSequence** ret);

    CARAPI GetAMPMString(
        /* [in] */ Int32 ampm,
        /* [out] */ String* ret);

    CARAPI GetDayOfWeekString(
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 abbrev,
        /* [out] */ String* ret);

    CARAPI GetMonthString(
        /* [in] */ Int32 month,
        /* [in] */ Int32 abbrev,
        /* [out] */ String* ret);

    CARAPI GetRelativeDateTimeString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 time,
        /* [in] */ Int64 minResolution,
        /* [in] */ Int64 transitionResolution,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** ret);

    CARAPI GetRelativeTimeSpanString(
        /* [in] */ Int64 startTime,
        /* [out] */ ICharSequence** ret);

    CARAPI GetRelativeTimeSpanString(
        /* [in] */ Int64 time,
        /* [in] */ Int64 now,
        /* [in] */ Int64 minResolution,
        /* [out] */ ICharSequence** ret);

    CARAPI GetRelativeTimeSpanString(
        /* [in] */ Int64 time,
        /* [in] */ Int64 now,
        /* [in] */ Int64 minResolution,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** ret);

    CARAPI GetRelativeTimeSpanString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 millis,
        /* [in] */ Boolean withPreposition,
        /* [out] */ ICharSequence** ret);

    CARAPI GetRelativeTimeSpanString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 millis,
        /* [out] */ ICharSequence** ret);

    CARAPI GetSameMonthTable(
        /* [out, callee] */ ArrayOf<Int32>** sameMonthTable);

    CARAPI GetSameYearTable(
        /* [out, callee] */ ArrayOf<Int32>** sameYearTable);

    CARAPI GetStandaloneMonthString(
        /* [in] */ Int32 month,
        /* [in] */ Int32 abbrev,
        /* [out] */ String* ret);

    CARAPI IsToday(
        /* [in] */ Int64 when,
        /* [out] */ Boolean* ret);
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_CDATEUTILS_H__
