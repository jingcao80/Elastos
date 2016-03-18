#ifndef __ELASTOS_DROID_DATABASE_DATABASEUTILS_H__
#define __ELASTOS_DROID_DATABASE_DATABASEUTILS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintStream;
using Elastos::Text::ICollator;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Static utility methods for dealing with databases and {@link Cursor}s.
 */
class DatabaseUtils
{
public:
    class InsertHelper
        : public Object
        , public IDatabaseInsertHelper
    {
    public:
        InsertHelper()
            : mColumns(NULL)
        {}

        /**
         * @param db the SQLiteDatabase to insert into
         * @param tableName the name of the table to insert into
         */
        CARAPI constructor(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ const String& tableName);

        virtual ~InsertHelper();

        CAR_INTERFACE_DECL()

        /**
         * Returns the index of the specified column. This is index is suitagble for use
         * in calls to bind().
         * @param key the column name
         * @return the index of the column
         */
        virtual CARAPI GetColumnIndex(
            /* [in] */ const String& key,
            /* [out] */ Int32* columnIndex);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindDouble(
            /* [in] */ Int32 index,
            /* [in] */ Double value);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindFloat(
            /* [in] */ Int32 index,
            /* [in] */ Float value);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindInt64(
            /* [in] */ Int32 index,
            /* [in] */ Int64 value);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindInt32(
            /* [in] */ Int32 index,
            /* [in] */ Int32 value);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindBoolean(
            /* [in] */ Int32 index,
            /* [in] */ Boolean value);

        /**
         * Bind null to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         */
        virtual CARAPI BindNull(
            /* [in] */ Int32 index);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        CARAPI BindBytes(
            /* [in] */ Int32 index,
            /* [in] */ ArrayOf<Byte>* value);

        /**
         * Bind the value to an index. A prepareForInsert() or prepareForReplace()
         * without a matching execute() must have already have been called.
         * @param index the index of the slot to which to bind
         * @param value the value to bind
         */
        virtual CARAPI BindString(
            /* [in] */ Int32 index,
            /* [in] */ const String& value);

        /**
         * Performs an insert, adding a new row with the given values.
         * If the table contains conflicting rows, an error is
         * returned.
         *
         * @param values the set of values with which to populate the
         * new row
         *
         * @return the row ID of the newly inserted row, or -1 if an
         * error occurred
         */
        virtual CARAPI Insert(
            /* [in] */ IContentValues* values,
            /* [out] */ Int64* rowId);

        /**
         * Execute the previously prepared insert or replace using the bound values
         * since the last call to prepareForInsert or prepareForReplace.
         *
         * <p>Note that calling bind() and then execute() is not thread-safe. The only thread-safe
         * way to use this class is to call insert() or replace().
         *
         * @return the row ID of the newly inserted row, or -1 if an
         * error occurred
         */
        virtual CARAPI Execute(
            /* [out] */ Int64* value);

        /**
         * Prepare the InsertHelper for an insert. The pattern for this is:
         * <ul>
         * <li>prepareForInsert()
         * <li>bind(index, value);
         * <li>bind(index, value);
         * <li>...
         * <li>bind(index, value);
         * <li>execute();
         * </ul>
         */
        virtual CARAPI PrepareForInsert();

        /**
         * Prepare the InsertHelper for a replace. The pattern for this is:
         * <ul>
         * <li>prepareForReplace()
         * <li>bind(index, value);
         * <li>bind(index, value);
         * <li>...
         * <li>bind(index, value);
         * <li>execute();
         * </ul>
         */
        virtual CARAPI PrepareForReplace();

        /**
         * Performs an insert, adding a new row with the given values.
         * If the table contains conflicting rows, they are deleted
         * and replaced with the new row.
         *
         * @param values the set of values with which to populate the
         * new row
         *
         * @return the row ID of the newly inserted row, or -1 if an
         * error occurred
         */
        virtual CARAPI Replace(
            /* [in] */ IContentValues* values,
            /* [out] */ Int64* rowId);

        /**
         * Close this object and release any resources associated with
         * it.  The behavior of calling <code>insert()</code> after
         * calling this method is undefined.
         */
        virtual CARAPI Close();

    private:
        CARAPI BuildSQL();

        CARAPI_(AutoPtr<ISQLiteStatement>) GetStatement(
            /* [in] */ Boolean allowReplace);

        /**
         * Performs an insert, adding a new row with the given values.
         *
         * @param values the set of values with which  to populate the
         * new row
         * @param allowReplace if true, the statement does "INSERT OR
         *   REPLACE" instead of "INSERT", silently deleting any
         *   previously existing rows that would cause a conflict
         *
         * @return the row ID of the newly inserted row, or -1 if an
         * error occurred
         */
        CARAPI_(Int64) InsertInternal(
            /* [in] */ IContentValues* values,
            /* [in] */ Boolean allowReplace);

    public:
        /**
         * {@hide}
         *
         * These are the columns returned by sqlite's "PRAGMA
         * table_info(...)" command that we depend on.
         */
        static const Int32 TABLE_INFO_PRAGMA_COLUMNNAME_INDEX = 1;

        /**
         * This field was accidentally exposed in earlier versions of the platform
         * so we can hide it but we can't remove it.
         *
         * @hide
         */
        static const Int32 TABLE_INFO_PRAGMA_DEFAULT_INDEX = 4;

    private:
        AutoPtr<ISQLiteDatabase> mDb;
        String mTableName;
        AutoPtr<HashMap<String, Int32> > mColumns;
        String mInsertSQL;
        AutoPtr<ISQLiteStatement> mInsertStatement;
        AutoPtr<ISQLiteStatement> mReplaceStatement;
        AutoPtr<ISQLiteStatement> mPreparedStatement;
        Object mLock;
    };

public:
    /**
     * Special function for writing an exception result at the header of
     * a parcel, to be used when returning an exception from a transaction.
     * exception will be re-thrown by the function in another process
     * @param reply Parcel to write to
     * @param e The Exception to be written.
     * @see Parcel#writeNoException
     * @see Parcel#writeException
     */
    static CARAPI_(void) WriteExceptionToParcel(
        /* [in] */ IParcel* reply,
        /* [in] */ ECode e);

    /**
     * Special function for reading an exception result from the header of
     * a parcel, to be used after receiving the result of a transaction.  This
     * will throw the exception for you if it had been written to the Parcel,
     * otherwise return and let you read the normal result data from the Parcel.
     * @param reply Parcel to read from
     * @see Parcel#writeNoException
     * @see Parcel#readException
     */
    static CARAPI ReadExceptionFromParcel(
        /* [in] */ IParcel* reply);

    static CARAPI ReadExceptionWithFileNotFoundExceptionFromParcel(
        /* [in] */ IParcel* reply);

    static CARAPI ReadExceptionWithOperationApplicationExceptionFromParcel(
        /* [in] */ IParcel* reply);

    /**
     * Binds the given Object to the given SQLiteProgram using the proper
     * typing. For example, bind numbers as longs/doubles, and everything else
     * as a string by call toString() on it.
     *
     * @param prog the program to bind the object to
     * @param index the 1-based index to bind at
     * @param value the value to bind
     */
    static CARAPI BindObjectToProgram(
        /* [in] */ ISQLiteProgram* prog,
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

    /**
     * Returns data type of the given object's value.
     *<p>
     * Returned values are
     * <ul>
     *   <li>{@link Cursor#FIELD_TYPE_NULL}</li>
     *   <li>{@link Cursor#FIELD_TYPE_INTEGER}</li>
     *   <li>{@link Cursor#FIELD_TYPE_FLOAT}</li>
     *   <li>{@link Cursor#FIELD_TYPE_STRING}</li>
     *   <li>{@link Cursor#FIELD_TYPE_BLOB}</li>
     *</ul>
     *</p>
     *
     * @param obj the object whose value type is to be returned
     * @return object value type
     * @hide
     */
    static CARAPI_(Int32) GetTypeOfObject(
        /* [in] */ IInterface* obj);

    /**
     * Fills the specified cursor window by iterating over the contents of the cursor.
     * The window is filled until the cursor is exhausted or the window runs out
     * of space.
     *
     * The original position of the cursor is left unchanged by this operation.
     *
     * @param cursor The cursor that contains the data to put in the window.
     * @param position The start position for filling the window.
     * @param window The window to fill.
     * @hide
     */
    static CARAPI_(void) CursorFillWindow(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ ICursorWindow* window);

    /**
     * Appends an SQL string to the given StringBuilder, including the opening
     * and closing single quotes. Any single quotes internal to sqlString will
     * be escaped.
     *
     * This method is deprecated because we want to encourage everyone
     * to use the "?" binding form.  However, when implementing a
     * ContentProvider, one may want to add WHERE clauses that were
     * not provided by the caller.  Since "?" is a positional form,
     * using it in this case could break the caller because the
     * indexes would be shifted to accomodate the ContentProvider's
     * internal bindings.  In that case, it may be necessary to
     * construct a WHERE clause manually.  This method is useful for
     * those cases.
     *
     * @param sb the StringBuilder that the SQL string will be appended to
     * @param sqlString the raw string to be appended, which may contain single
     *                  quotes
     */
    static CARAPI_(void) AppendEscapedSQLString(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& sqlString);

    /**
     * SQL-escape a string.
     */
    static CARAPI_(String) SqlEscapeString(
        /* [in] */ const String& value);

    /**
     * Appends an Object to an SQL string with the proper escaping, etc.
     */
    static CARAPI_(void) AppendValueToSql(
        /* [in] */ IStringBuilder* sql,
        /* [in] */ IInterface* value);

    /**
     * Concatenates two SQL WHERE clauses, handling empty or null values.
     */
    static CARAPI_(String) ConcatenateWhere(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    /**
     * return the collation key
     * @param name
     * @return the collation key
     */
    static CARAPI_(String) GetCollationKey(
        /* [in] */ const String& name);

    /**
     * return the collation key in hex format
     * @param name
     * @return the collation key in hex format
     */
    static CARAPI_(String) GetHexCollationKey(
        /* [in] */ const String& name);

    /**
     * Prints the contents of a Cursor to System.out. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     */
    static CARAPI_(void) DumpCursor(
        /* [in] */ ICursor* cursor);

    /**
     * Prints the contents of a Cursor to a PrintSteam. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     * @param stream the stream to print to
     */
    static CARAPI_(void) DumpCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IPrintStream* stream);

    /**
     * Prints the contents of a Cursor to a StringBuilder. The position
     * is restored after printing.
     *
     * @param cursor the cursor to print
     * @param sb the StringBuilder to print to
     */
    static CARAPI_(void) DumpCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IStringBuilder* sb);

    /**
     * Prints the contents of a Cursor to a String. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     * @return a String that contains the dumped cursor
     */
    static CARAPI_(String) DumpCursorToString(
        /* [in] */ ICursor* cursor);

    /**
     * Prints the contents of a Cursor's current row to System.out.
     *
     * @param cursor the cursor to print from
     */
    static CARAPI_(void) DumpCurrentRow(
        /* [in] */ ICursor* cursor);

    /**
     * Prints the contents of a Cursor's current row to a PrintSteam.
     *
     * @param cursor the cursor to print
     * @param stream the stream to print to
     */
    static CARAPI_(void) DumpCurrentRow(
        /* [in] */ ICursor* cursor,
        /* [in] */ IPrintStream* stream);

    /**
     * Prints the contents of a Cursor's current row to a StringBuilder.
     *
     * @param cursor the cursor to print
     * @param sb the StringBuilder to print to
     */
    static CARAPI_(void) DumpCurrentRow(
        /* [in] */ ICursor* cursor,
        /* [in] */ IStringBuilder* sb);

    /**
     * Dump the contents of a Cursor's current row to a String.
     *
     * @param cursor the cursor to print
     * @return a String that contains the dumped cursor row
     */
    static CARAPI_(String) DumpCurrentRowToString(
        /* [in] */ ICursor* cursor);

    /**
     * Reads a String out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The TEXT field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     */
    static CARAPI_(void) CursorStringToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values);

    /**
     * Reads a String out of a field in a Cursor and writes it to an InsertHelper.
     *
     * @param cursor The cursor to read from
     * @param field The TEXT field to read
     * @param inserter The InsertHelper to bind into
     * @param index the index of the bind entry in the InsertHelper
     */
    static CARAPI_(void) CursorStringToInsertHelper(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IDatabaseInsertHelper* inserter,
        /* [in] */ Int32 index);

    /**
     * Reads a String out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The TEXT field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     * @param key The key to store the value with in the map
     */
    static CARAPI_(void) CursorStringToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& key);

    /**
     * Reads an Integer out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The INTEGER field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     */
    static CARAPI_(void) CursorInt32ToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values);

    /**
     * Reads a Integer out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The INTEGER field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     * @param key The key to store the value with in the map
     */
    static CARAPI_(void) CursorInt32ToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& key);

    /**
     * Reads a Long out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The INTEGER field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     */
    static CARAPI_(void) CursorInt64ToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values);

    /**
     * Reads a Long out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The INTEGER field to read
     * @param values The {@link ContentValues} to put the value into
     * @param key The key to store the value with in the map
     */
    static CARAPI_(void) CursorInt64ToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& key);

    /**
     * Reads a Double out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The REAL field to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorDoubleToCursorValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values);

    /**
     * Reads a Double out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The REAL field to read
     * @param values The {@link ContentValues} to put the value into
     * @param key The key to store the value with in the map
     */
    static CARAPI_(void) CursorDoubleToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& field,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& key);

    /**
     * Read the entire contents of a cursor row and store them in a ContentValues.
     *
     * @param cursor the cursor to read from.
     * @param values the {@link ContentValues} to put the row into.
     */
    static CARAPI_(void) CursorRowToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values);

    /**
     * Picks a start position for {@link Cursor#fillWindow} such that the
     * window will contain the requested row and a useful range of rows
     * around it.
     *
     * When the data set is too large to fit in a cursor window, seeking the
     * cursor can become a very expensive operation since we have to run the
     * query again when we move outside the bounds of the current window.
     *
     * We try to choose a start position for the cursor window such that
     * 1/3 of the window's capacity is used to hold rows before the requested
     * position and 2/3 of the window's capacity is used to hold rows after the
     * requested position.
     *
     * @param cursorPosition The row index of the row we want to get.
     * @param cursorWindowCapacity The estimated number of rows that can fit in
     * a cursor window, or 0 if unknown.
     * @return The recommended start position, always less than or equal to
     * the requested row.
     * @hide
     */
    static CARAPI_(Int32) CursorPickFillWindowStartPosition(
        /* [in] */ Int32 cursorPosition,
        /* [in] */ Int32 cursorWindowCapacity);

    /**
     * Query the table for the number of rows in the table.
     * @param db the database the table is in
     * @param table the name of the table to query
     * @return the number of rows in the table
     */
    static CARAPI_(Int64) QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table);

    /**
     * Query the table for the number of rows in the table.
     * @param db the database the table is in
     * @param table the name of the table to query
     * @param selection A filter declaring which rows to return,
     *              formatted as an SQL WHERE clause (excluding the WHERE itself).
     *              Passing null will count all rows for the given table
     * @return the number of rows in the table filtered by the selection
     */
    static CARAPI_(Int64) QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& selection);

    /**
     * Query the table for the number of rows in the table.
     * @param db the database the table is in
     * @param table the name of the table to query
     * @param selection A filter declaring which rows to return,
     *              formatted as an SQL WHERE clause (excluding the WHERE itself).
     *              Passing null will count all rows for the given table
     * @param selectionArgs You may include ?s in selection,
     *              which will be replaced by the values from selectionArgs,
     *              in order that they appear in the selection.
     *              The values will be bound as Strings.
     * @return the number of rows in the table filtered by the selection
     */
    static CARAPI_(Int64) QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Query the table to check whether a table is empty or not
     * @param db the database the table is in
     * @param table the name of the table to query
     * @return True if the table is empty
     * @hide
     */
    static CARAPI_(Boolean) QueryIsEmpty(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table);

    /**
     * Utility method to run the query on the db and return the value in the
     * first column of the first row.
     */
    static CARAPI_(Int64) Int64ForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Utility method to run the pre-compiled query and return the value in the
     * first column of the first row.
     */
    static CARAPI_(Int64) Int64ForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Utility method to run the query on the db and return the value in the
     * first column of the first row.
     */
    static CARAPI_(String) StringForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Utility method to run the pre-compiled query and return the value in the
     * first column of the first row.
     */
    static CARAPI_(String) StringForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Utility method to run the query on the db and return the blob value in the
     * first column of the first row.
     *
     * @return A read-only file descriptor for a copy of the blob value.
     */
    static CARAPI_(AutoPtr<IParcelFileDescriptor>) BlobFileDescriptorForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Utility method to run the pre-compiled query and return the blob value in the
     * first column of the first row.
     *
     * @return A read-only file descriptor for a copy of the blob value.
     */
    static CARAPI_(AutoPtr<IParcelFileDescriptor>) BlobFileDescriptorForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Reads a String out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorStringToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Reads a Long out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorInt64ToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Reads a Short out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorInt16ToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Reads a Integer out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorInt32ToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Reads a Float out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorFloatToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Reads a Double out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    static CARAPI_(void) CursorDoubleToContentValuesIfPresent(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& column);

    /**
     * Creates a db and populates it with the sql statements in sqlStatements.
     *
     * @param context the context to use to create the db
     * @param dbName the name of the db to create
     * @param dbVersion the version to set on the db
     * @param sqlStatements the statements to use to populate the db. This should be a single string
     *   of the form returned by sqlite3's <tt>.dump</tt> command (statements separated by
     *   semicolons)
     */
    static CARAPI_(void) CreateDbFromSqlStatements(
        /* [in] */ IContext* context,
        /* [in] */ const String& dbName,
        /* [in] */ Int32 dbVersion,
        /* [in] */ const String& sqlStatements);

    /**
     * Returns one of the following which represent the type of the given SQL statement.
     * <ol>
     *   <li>{@link #STATEMENT_SELECT}</li>
     *   <li>{@link #STATEMENT_UPDATE}</li>
     *   <li>{@link #STATEMENT_ATTACH}</li>
     *   <li>{@link #STATEMENT_BEGIN}</li>
     *   <li>{@link #STATEMENT_COMMIT}</li>
     *   <li>{@link #STATEMENT_ABORT}</li>
     *   <li>{@link #STATEMENT_OTHER}</li>
     * </ol>
     * @param sql the SQL statement whose type is returned by this method
     * @return one of the values listed above
     */
    static CARAPI_(Int32) GetSqlStatementType(
        /* [in] */ const String& sql);

    /**
     * Appends one set of selection args to another. This is useful when adding a selection
     * argument to a user provided set.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) AppendSelectionArgs(
        /* [in] */ ArrayOf<String>* originalValues,
        /* [in] */ ArrayOf<String>* newValues);

    /**
     * Returns column index of "_id" column, or -1 if not found.
     * @hide
     */
    static CARAPI_(Int32) FindRowIdColumnIndex(
        /* [in] */ ArrayOf<String>* columnNames);

private:
    static CARAPI ReadExceptionFromParcel(
        /* [in] */ IParcel* reply,
        /* [in] */ const String& msg,
        /* [in] */ Int32 code);

    static CARAPI_(Int32) GetKeyLen(
        /* [in] */ const ArrayOf<Byte>& arr);

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) GetCollationKeyInBytes(
        /* [in] */ const String& name);

private:
    static const String TAG;

    static const Boolean DEBUG = FALSE;

    static AutoPtr<ICollator> mColl;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_DATABASEUTILS_H__
