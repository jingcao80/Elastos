
#ifndef __ELASTOS_UTILITY_CCALENDARHELPER_H__
#define __ELASTOS_UTILITY_CCALENDARHELPER_H__

#include "_Elastos_Utility_CCalendarHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Utility::ILocale;

namespace Elastos{
namespace Utility{

CarClass(CCalendarHelper)
    , public Singleton
    , public ICalendarHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetInstance(
        /* [out] */ ICalendar ** ppCalenar);

    CARAPI GetInstance(
        /* [in] */ ILocale * pLocale,
        /* [out] */ ICalendar ** ppCalenar);

    CARAPI GetInstance(
        /* [in] */ ITimeZone * pTimezone,
        /* [out] */ ICalendar ** ppCalenar);

    CARAPI GetInstance(
        /* [in] */ ITimeZone * pTimezone,
        /* [in] */ ILocale * pLocale,
        /* [out] */ ICalendar ** ppCalenar);
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CCALENDARHELPER_H__
