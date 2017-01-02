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

#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/DateSorter.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleDataHelper;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Utility::ICalendar;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {

const String DateSorter::LOGTAG("webkit");
const Int32 DateSorter::NUM_DAYS_AGO;

CAR_INTERFACE_IMPL(DateSorter, Object, IDateSorter);

DateSorter::DateSorter()
{
}

/**
 * @param context Application context
 */
DateSorter::DateSorter(
    /* [in] */ IContext* context)
{
    Init(context);
}

ECode DateSorter::constructor(
    /* [in] */ IContext* context)
{
    Init(context);
    return NOERROR;
}

/**
 * @param context Application context
 */
void DateSorter::Init(
    /* [in] */ IContext* context)
{
    mBins = ArrayOf<Int64>::Alloc(IDateSorter::DAY_COUNT-1);
    mLabels = ArrayOf<String>::Alloc(IDateSorter::DAY_COUNT);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    AutoPtr<ICalendar> c;
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    calendarHelper->GetInstance((ICalendar**)&c);
    BeginningOfDay(c);

    // Create the bins
    c->GetTimeInMillis(&((*mBins)[0])); // Today
    c->Add(ICalendar::DAY_OF_YEAR, -1);
    c->GetTimeInMillis(&((*mBins)[1]));  // Yesterday
    c->Add(ICalendar::DAY_OF_YEAR, -(NUM_DAYS_AGO - 1));
    c->GetTimeInMillis(&((*mBins)[2]));  // Five days ago
    c->Add(ICalendar::DAY_OF_YEAR, NUM_DAYS_AGO); // move back to today
    c->Add(ICalendar::MONTH, -1);
    c->GetTimeInMillis(&((*mBins)[3]));  // One month ago

    // build labels
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    AutoPtr<ILocale> locale;
    configuration->GetLocale((ILocale**)&locale);
    if (locale == NULL) {
        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        localeHelper->GetDefault((ILocale**)&locale);
    }
    AutoPtr<ILocaleDataHelper> localeDataHelper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper);
    AutoPtr<ILocaleData> localeData;
    localeDataHelper->Get(locale, (ILocaleData**)&localeData);
    String today, yesterday;
    localeData->GetToday(&today);
    localeData->GetYesterday(&yesterday);
    (*mLabels)[0] = today;
    (*mLabels)[1] = yesterday;

    Int32 resId = R::plurals::last_num_days;
    String format;
    resources->GetQuantityString(resId, NUM_DAYS_AGO, &format);
    (*mLabels)[2].AppendFormat((const char*)format, NUM_DAYS_AGO);

    context->GetString(R::string::last_month, &((*mLabels)[3]));
    context->GetString(R::string::older, &((*mLabels)[4]));
}

/**
 * @param time time since the Epoch in milliseconds, such as that
 * returned by Calendar.getTimeInMillis()
 * @return an index from 0 to (DAY_COUNT - 1) that identifies which
 * date bin this date belongs to
 */
ECode DateSorter::GetIndex(
    /* [in] */ Int64 time,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    Int32 lastDay = IDateSorter::DAY_COUNT - 1;
    for (Int32 i = 0; i < lastDay; i++) {
        if (time > (*mBins)[i]) return i;
    }

    *index = lastDay;

    return NOERROR;
}

/**
 * @param index date bin index as returned by getIndex()
 * @return string label suitable for display to user
 */
ECode DateSorter::GetLabel(
    /* [in] */ Int32 index,
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);

    if (index < 0 || index >= IDateSorter::DAY_COUNT) {
        *label = String("");
        return NOERROR;
    }

    *label = (*mLabels)[index];

    return NOERROR;
}

/**
 * @param index date bin index as returned by getIndex()
 * @return date boundary at given index
 */
ECode DateSorter::GetBoundary(
    /* [in] */ Int32 index,
    /* [out] */ Int64* boundary)
{
    VALIDATE_NOT_NULL(boundary);

    Int32 lastDay = IDateSorter::DAY_COUNT - 1;
    // Error case
    if (index < 0 || index > lastDay) index = 0;
    // Since this provides a lower boundary on dates that will be included
    // in the given bin, provide the smallest value
    if (index == lastDay) {
        *boundary = Elastos::Core::Math::INT64_MIN_VALUE;
        return NOERROR;
    }

    *boundary = (*mBins)[index];

    return NOERROR;
}

/**
 * Calcuate 12:00am by zeroing out hour, minute, second, millisecond
 */
void DateSorter::BeginningOfDay(
    /* [in] */ ICalendar* c)
{
    c->Set(ICalendar::HOUR_OF_DAY, 0);
    c->Set(ICalendar::MINUTE, 0);
    c->Set(ICalendar::SECOND, 0);
    c->Set(ICalendar::MILLISECOND, 0);
}

ECode DateSorter::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "DateSorter";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
