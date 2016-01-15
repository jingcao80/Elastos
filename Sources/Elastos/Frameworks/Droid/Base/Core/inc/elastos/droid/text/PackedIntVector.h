
#ifndef __ELASTOS_DROID_TEXT_PACKEDINTVECTOR_H__
#define __ELASTOS_DROID_TEXT_PACKEDINTVECTOR_H__

#include "Elastos.Droid.Core.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * PackedIntVector stores a two-dimensional array of integers,
 * optimized for inserting and deleting rows and for
 * offsetting the values in segments of a given column.
 */

class PackedIntVector
    : public Object
{
public:
    /**
     * Creates a new PackedIntVector with the specified width and
     * a height of 0.
     *
     * @param columns the width of the PackedIntVector.
     */
    PackedIntVector(
        /* [in] */ Int32 columns);

    virtual ~PackedIntVector();

    /**
     * Returns the value at the specified row and column.
     *
     * @param row the index of the row to return.
     * @param column the index of the column to return.
     *
     * @return the value stored at the specified position.
     *
     * @throws IndexOutOfBoundsException if the row is out of range
     *         (row &lt; 0 || row >= size()) or the column is out of range
     *         (column &lt; 0 || column >= width()).
     */
    CARAPI_(Int32) GetValue(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column);

    /**
     * Sets the value at the specified row and column.
     *
     * @param row the index of the row to set.
     * @param column the index of the column to set.
     *
     * @throws IndexOutOfBoundsException if the row is out of range
     *         (row &lt; 0 || row >= size()) or the column is out of range
     *         (column &lt; 0 || column >= width()).
     */
    CARAPI_(void) SetValue(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ Int32 value);

    /**
     * Increments all values in the specified column whose row >= the
     * specified row by the specified delta.
     *
     * @param startRow the row at which to begin incrementing.
     *        This may be == size(), which case there is no effect.
     * @param column the index of the column to set.
     *
     * @throws IndexOutOfBoundsException if the row is out of range
     *         (startRow &lt; 0 || startRow > size()) or the column
     *         is out of range (column &lt; 0 || column >= width()).
     */
    CARAPI_(void) AdjustValuesBelow(
        /* [in] */ Int32 startRow,
        /* [in] */ Int32 column,
        /* [in] */ Int32 delta);

    /**
     * Inserts a new row of values at the specified row offset.
     *
     * @param row the row above which to insert the new row.
     *        This may be == size(), which case the new row is added
     *        at the end.
     * @param values the new values to be added.  If this is NULL,
     *        a row of zeroes is added.
     *
     * @throws IndexOutOfBoundsException if the row is out of range
     *         (row &lt; 0 || row > size()) or if the length of the
     *         values array is too small (values.length < width()).
     */
    CARAPI_(void) InsertAt(
        /* [in] */ Int32 row,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Deletes the specified number of rows starting with the specified
     * row.
     *
     * @param row the index of the first row to be deleted.
     * @param count the number of rows to delete.
     *
     * @throws IndexOutOfBoundsException if any of the rows to be deleted
     *         are out of range (row &lt; 0 || count &lt; 0 ||
     *         row + count > size()).
     */
    CARAPI_(void) DeleteAt(
        /* [in] */ Int32 row,
        /* [in] */ Int32 count);

    /**
     * Returns the number of rows in the PackedIntVector.  This number
     * will change as rows are inserted and deleted.
     *
     * @return the number of rows.
     */
    CARAPI_(Int32) Size();

    /**
     * Returns the width of the PackedIntVector.  This number is set
     * at construction and will not change.
     *
     * @return the number of columns.
     */
    CARAPI_(Int32) Width();

private:
    /**
     * Sets the value at the specified row and column.
     * Private internal version: does not check args.
     *
     * @param row the index of the row to set.
     * @param column the index of the column to set.
     *
     */
    CARAPI_(void) SetValueInternal(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ Int32 value);

    /**
     * Grows the value and gap arrays to be large enough to store at least
     * one more than the current number of rows.
     */
    CARAPI_(void) GrowBuffer();

    /**
     * Moves the gap in the values of the specified column to begin at
     * the specified row.
     */
    CARAPI_(void) MoveValueGapTo(
        /* [in] */ Int32 column,
        /* [in] */ Int32 where);

    /**
     * Moves the gap in the row indices to begin at the specified row.
     */
    CARAPI_(void) MoveRowGapTo(
        /* [in] */ Int32 where);

private:
    Int32 mColumns;
    Int32 mRows;

    Int32 mRowGapStart;
    Int32 mRowGapLength;

    AutoPtr< ArrayOf<Int32> > mValues;
    AutoPtr< ArrayOf<Int32> > mValueGap; // starts, followed by lengths

};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_PACKEDINTVECTOR_H__
