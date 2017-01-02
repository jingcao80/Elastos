//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
