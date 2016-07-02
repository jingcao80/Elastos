#ifndef __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALMATCHPOSITION_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALMATCHPOSITION_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * Stores information about a range of characters matched in a display name The integers
 * start and end indicate that the range start to end (exclusive) correspond to some characters
 * in the query. Used to highlight certain parts of the contact's display name to indicate that
 * those ranges matched the user's query.
 */
class SmartDialMatchPosition
    : public Object
{
public:
    SmartDialMatchPosition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Used by {@link SmartDialNameMatcher} to advance the positions of a match position found in
     * a sub query.
     *
     * @param inList ArrayList of SmartDialMatchPositions to modify.
     * @param toAdvance Offset to modify by.
     */
    static CARAPI_(void) AdvanceMatchPositions(
        /* [in] */ IArrayList* inList,
        /* [in] */ Int32 toAdvance);

    /**
     * Used mainly for debug purposes. Displays contents of an ArrayList of SmartDialMatchPositions.
     *
     * @param list ArrayList of SmartDialMatchPositions to print out in a human readable fashion.
     */
    static CARAPI_(void) Print(
        /* [in] */ IArrayList* list);

private:
    CARAPI_(void) Advance(
        /* [in] */ Int32 toAdvance);

public:
    Int32 mStart;
    Int32 mEnd;

private:
    static const String TAG;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALMATCHPOSITION_H__
