#ifndef __ELASTOS_DROID_DATABASE_MATRIXCURSOR_H__
#define __ELASTOS_DROID_DATABASE_MATRIXCURSOR_H__

#include "elastos/droid/database/AbstractCursor.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * A mutable cursor implementation backed by an array of {@code Object}s. Use
 * {@link #newRow()} to add rows. Automatically expands internal capacity
 * as needed.
 */
class MatrixCursor
    : public AbstractCursor
    , public IMatrixCursor
{
public:
    /**
     * Builds a row of values using either of these approaches:
     * <ul>
     * <li>Values can be added with explicit column ordering using
     * {@link #add(Object)}, which starts from the left-most column and adds one
     * column value at a time. This follows the same ordering as the column
     * names specified at cursor construction time.
     * <li>Column and value pairs can be offered for possible inclusion using
     * {@link #add(String, Object)}. If the cursor includes the given column,
     * the value will be set for that column, otherwise the value is ignored.
     * This approach is useful when matching data to a custom projection.
     * </ul>
     * Undefined values are left as {@code null}.
     */
    class RowBuilder
        : public Object
        , public IRowBuilder
    {
    public:
        RowBuilder(
            /* [in] */ Int32 row,
            /* [in] */ MatrixCursor* owner);

        CAR_INTERFACE_DECL()

        /**
         * Sets the next column value in this row.
         *
         * @throws CursorIndexOutOfBoundsException if you try to add too many
         *  values
         * @return this builder to support chaining
         */
        CARAPI Add(
            /* [in] */ IInterface* columnValue);

        /**
         * Offer value for possible inclusion if this cursor defines the given
         * column. Columns not defined by the cursor are silently ignored.
         *
         * @return this builder to support chaining
         */
        CARAPI Add(
            /* [in] */ const String& columnName,
            /* [in] */ IInterface* value);

    private:
        Int32 mRow;
        Int32 mEndIndex;

        Int32 mIndex;
        MatrixCursor* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    MatrixCursor();

    /**
     * Constructs a new cursor with the given initial capacity.
     *
     * @param columnNames names of the columns, the ordering of which
     *  determines column ordering elsewhere in this cursor
     * @param initialCapacity in rows
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<String>* columnNames,
        /* [in] */ Int32 initialCapacity);

    /**
     * Constructs a new cursor.
     *
     * @param columnNames names of the columns, the ordering of which
     *  determines column ordering elsewhere in this cursor
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<String>* columnNames);

    /**
     * Adds a new row to the end and returns a builder for that row. Not safe
     * for concurrent use.
     *
     * @return builder which can be used to set the column values for the new
     *  row
     */
    virtual CARAPI NewRow(
        /* [out] */ IRowBuilder** builder);

    /**
     * Adds a new row to the end with the given column values. Not safe
     * for concurrent use.
     *
     * @throws IllegalArgumentException if {@code columnValues.length !=
     *  columnNames.length}
     * @param columnValues in the same order as the the column names specified
     *  at cursor construction time
     */
    virtual CARAPI AddRow(
        /* [in] */ const ArrayOf<IInterface*>& columnValues);

    /**
     * Adds a new row to the end with the given column values. Not safe
     * for concurrent use.
     *
     * @throws IllegalArgumentException if {@code columnValues.size() !=
     *  columnNames.length}
     * @param columnValues in the same order as the the column names specified
     *  at cursor construction time
     */
    virtual CARAPI AddRow(
        /* [in] */ IIterable* columnValues);

    // AbstractCursor implementation.

    //@Override
    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    //@Override
    virtual CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** names);

    //@Override
    virtual CARAPI GetString(
        /* [in] */ Int32 column,
        /* [out] */ String* str);

    //@Override
    virtual CARAPI GetInt16(
        /* [in] */ Int32 column,
        /* [out] */ Int16* value);

    //@Override
    virtual CARAPI GetInt32(
        /* [in] */ Int32 column,
        /* [out] */ Int32* value);

    //@Override
    virtual CARAPI GetInt64(
        /* [in] */ Int32 column,
        /* [out] */ Int64* value);

    //@Override
    virtual CARAPI GetFloat(
        /* [in] */ Int32 column,
        /* [out] */ Float* value);

    //@Override
    virtual CARAPI GetDouble(
        /* [in] */ Int32 column,
        /* [out] */ Double* value);

    //@Override
    virtual CARAPI GetBlob(
        /* [in] */ Int32 column,
        /* [out, callee] */ ArrayOf<Byte>** blob);

    //@Override
    virtual CARAPI GetType(
        /* [in] */ Int32 column,
        /* [out] */ Int32* type);

    //@Override
    virtual CARAPI IsNull(
        /* [in] */ Int32 column,
        /* [out] */ Boolean* isNull);

private:
    /**
     * Gets value at the given column for the current row.
     */
    CARAPI Get(
        /* [in] */ Int32 column,
        /* [out] */ IInterface** value);

    /** Optimization for {@link ArrayList}. */
//    CARAPI AddRow(
//        /* [in] */ ArrayList<?> columnValues,
//        /* [in] */ Int32 start);

    /** Ensures that this cursor has enough capacity. */
    CARAPI_(void) EnsureCapacity(
        /* [in] */ Int32 size);

private:
    AutoPtr< ArrayOf<String> > mColumnNames;
    AutoPtr< ArrayOf<IInterface*> > mData;
    Int32 mRowCount;
    Int32 mColumnCount;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_MATRIXCURSOR_H__
