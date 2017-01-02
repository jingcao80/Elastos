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

#ifndef __ELASTOS_DROID_WEBKIT_DATESORTER_H__
#define __ELASTOS_DROID_WEBKIT_DATESORTER_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Sorts dates into the following groups:
 *   Today
 *   Yesterday
 *   seven days ago
 *   one month ago
 *   older than a month ago
 */
class DateSorter
    : public Object
    , public IDateSorter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * @param context Application context
     */
    DateSorter(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @param time time since the Epoch in milliseconds, such as that
     * returned by Calendar.getTimeInMillis()
     * @return an index from 0 to (DAY_COUNT - 1) that identifies which
     * date bin this date belongs to
     */
    CARAPI GetIndex(
        /* [in] */ Int64 time,
        /* [out] */ Int32* index);

    /**
     * @param index date bin index as returned by getIndex()
     * @return string label suitable for display to user
     */
    CARAPI GetLabel(
        /* [in] */ Int32 index,
        /* [out] */ String* label);

    /**
     * @param index date bin index as returned by getIndex()
     * @return date boundary at given index
     */
    CARAPI GetBoundary(
        /* [in] */ Int32 index,
        /* [out] */ Int64* boundary);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    DateSorter();

    CARAPI_(void) Init(
        /* [in] */ IContext* context);

private:
    /**
     * Calcuate 12:00am by zeroing out hour, minute, second, millisecond
     */
    CARAPI_(void) BeginningOfDay(
        /* [in] */ ICalendar* c);

private:
    static const String LOGTAG;
    static const Int32 NUM_DAYS_AGO = 7;
    AutoPtr< ArrayOf<Int64> > mBins;
    AutoPtr< ArrayOf<String> > mLabels;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_DATESORTER_H__
