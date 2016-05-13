
#include "CCalendarHelper.h"
#include "Calendar.h"

namespace Elastos{
namespace Utility{

CAR_SINGLETON_IMPL(CCalendarHelper)

CAR_INTERFACE_IMPL(CCalendarHelper, Singleton, ICalendarHelper)

ECode CCalendarHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return Calendar::GetAvailableLocales(locales);
}

ECode CCalendarHelper::GetInstance(
    /* [out] */ ICalendar** calenar)
{
    return Calendar::GetInstance(calenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICalendar** calenar)
{
    return Calendar::GetInstance(locale, calenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ITimeZone* timezone,
    /* [out] */ ICalendar** calenar)
{
    return Calendar::GetInstance(timezone, calenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ITimeZone* timezone,
    /* [in] */ ILocale* locale,
    /* [out] */ ICalendar** calenar)
{
    return Calendar::GetInstance(timezone, locale, calenar);
}

} // Utility
} // Elastos
