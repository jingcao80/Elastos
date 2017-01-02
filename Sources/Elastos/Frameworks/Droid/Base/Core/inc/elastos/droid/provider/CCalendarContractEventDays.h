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

#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTDAS_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTDAS_H__

#include "_Elastos_Droid_Provider_CCalendarContractEventDays.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields and helpers for querying for a list of days that contain events.
 */
CarClass(CCalendarContractEventDays)
    , public Singleton
    , public ICalendarContractEventDays
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Retrieves the days with events for the Julian days starting at
     * "startDay" for "numDays". It returns a cursor containing startday and
     * endday representing the max range of days for all events beginning on
     * each startday.This is a blocking function and should not be done on
     * the UI thread.
     *
     * @param cr the ContentResolver
     * @param startDay the first Julian day in the range
     * @param numDays the number of days to load (must be at least 1)
     * @param projection the columns to return in the cursor
     * @return a database cursor containing a list of start and end days for
     *         events
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int32 startDay,
        /* [in] */ Int32 numDays,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

private:
    static const String SELECTION;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTEVENTDAS_H__
