#ifndef __ELASTOS_DROID_DATABASE_CCURSORJOINER_H__
#define __ELASTOS_DROID_DATABASE_CCURSORJOINER_H__

#include "_Elastos_Droid_Database_CCursorJoiner.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::EIID_IIterator;

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CCursorJoiner)
    , public Object
    , public ICursorJoiner
    , public IIterable
    , public IIterator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCursorJoiner();

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI HasNext(
        /* [out] */ Boolean* result);

    CARAPI GetNext(
        /* [out] */ IInterface** result);

    CARAPI Remove();

    CARAPI constructor(
        /* [in] */ ICursor* cursorLeft,
        /* [in] */ const ArrayOf<String>& columnNamesLeft,
        /* [in] */ ICursor* cursorRight,
        /* [in] */ const ArrayOf<String>& columnNamesRight);

private:
    /**
     * Lookup the indicies of the each column name and return them in an array.
     * @param cursor the cursor that contains the columns
     * @param columnNames the array of names to lookup
     * @return an array of column indices
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) BuildColumnIndiciesArray(
        /* [in] */ ICursor* cursor,
        /* [in] */ const ArrayOf<String>& columnNames);

    /**
     * Reads the strings from the cursor that are specifed in the columnIndicies
     * array and saves them in values beginning at startingIndex, skipping a slot
     * for each value. If columnIndicies has length 3 and startingIndex is 1, the
     * values will be stored in slots 1, 3, and 5.
     * @param values the String[] to populate
     * @param cursor the cursor from which to read
     * @param columnIndicies the indicies of the values to read from the cursor
     * @param startingIndex the slot in which to start storing values, and must be either 0 or 1.
     */
    static CARAPI_(void) PopulateValues(
        /* [in] */ ArrayOf<String>& values,
        /* [in] */ ICursor* cursor,
        /* [in] */ const ArrayOf<Int32>& columnIndicies,
        /* [in] */ Int32 startingIndex);

    /**
     * Increment the cursors past the rows indicated in the most recent call to next().
     * This will only have an affect once per call to next().
     */
    CARAPI_(void) IncrementCursors();

    /**
     * Compare the values. Values contains n pairs of strings. If all the pairs of strings match
     * then returns 0. Otherwise returns the comparison result of the first non-matching pair
     * of values, -1 if the first of the pair is less than the second of the pair or 1 if it
     * is greater.
     * @param values the n pairs of values to compare
     * @return -1, 0, or 1 as described above.
     */
    static CARAPI CompareStrings(
        /* [in] */ const ArrayOf<String>& values,
        /* [out] */ Int32* result);

private:
    AutoPtr<ICursor> mCursorLeft;
    AutoPtr<ICursor> mCursorRight;
    Boolean mCompareResultIsValid;
    CursorJoinerResult mCompareResult;
    AutoPtr<ArrayOf<Int32> > mColumnsLeft;
    AutoPtr<ArrayOf<Int32> > mColumnsRight;
    AutoPtr<ArrayOf<String> > mValues;
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CCURSORJOINER_H__
