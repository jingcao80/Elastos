
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
    CAR_INTERFACE_DECL();

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
