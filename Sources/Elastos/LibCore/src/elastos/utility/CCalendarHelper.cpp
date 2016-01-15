
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
    /* [out] */ ICalendar ** ppCalenar)
{
    return Calendar::GetInstance(ppCalenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ILocale * pLocale,
    /* [out] */ ICalendar ** ppCalenar)
{
    return Calendar::GetInstance(pLocale, ppCalenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ITimeZone * pTimezone,
    /* [out] */ ICalendar ** ppCalenar)
{
    return Calendar::GetInstance(pTimezone, ppCalenar);
}

ECode CCalendarHelper::GetInstance(
    /* [in] */ ITimeZone * pTimezone,
    /* [in] */ ILocale * pLocale,
    /* [out] */ ICalendar ** ppCalenar)
{
    return Calendar::GetInstance(pTimezone, pLocale, ppCalenar);
}

} // Utility
} // Elastos
