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

#ifndef __ELASTOS_DROID_DATABASE_CDATABASEUTILS_H__
#define __ELASTOS_DROID_DATABASE_CDATABASEUTILS_H__

#include "_Elastos_Droid_Database_CDatabaseUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::IO::IPrintStream;
using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Static utility methods for dealing with databases and {@link Cursor}s.
 */
CarClass(CDatabaseUtils)
    , public Singleton
    , public IDatabaseUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Special function for writing an exception result at the header of
     * a parcel, to be used when returning an exception from a transaction.
     * exception will be re-thrown by the function in another process
     * @param reply Parcel to write to
     * @param e The Exception to be written.
     * @see Parcel#writeNoException
     * @see Parcel#writeException
     */
    CARAPI WriteExceptionToParcel(
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
    CARAPI ReadExceptionFromParcel(
        /* [in] */ IParcel* reply);

    CARAPI ReadExceptionWithFileNotFoundExceptionFromParcel(
        /* [in] */ IParcel* reply);

    CARAPI ReadExceptionWithOperationApplicationExceptionFromParcel(
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
    CARAPI BindObjectToProgram(
        /* [in] */ ISQLiteProgram* prog,
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

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
    CARAPI AppendEscapedSQLString(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& sqlString);

    /**
     * SQL-escape a string.
     */
    CARAPI SqlEscapeString(
        /* [in] */ const String& value,
        /* [out] */ String* str);

    /**
     * Appends an Object to an SQL string with the proper escaping, etc.
     */
    CARAPI AppendValueToSql(
        /* [in] */ IStringBuilder* sql,
        /* [in] */ IInterface* value);

    /**
     * Concatenates two SQL WHERE clauses, handling empty or null values.
     */
    CARAPI ConcatenateWhere(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ String* str);

    /**
     * return the collation key
     * @param name
     * @return the collation key
     */
    CARAPI GetCollationKey(
        /* [in] */ const String& name,
        /* [out] */ String* str);

    /**
     * return the collation key in hex format
     * @param name
     * @return the collation key in hex format
     */
    CARAPI GetHexCollationKey(
        /* [in] */ const String& name,
        /* [out] */ String* str);

    /**
     * Prints the contents of a Cursor to System.out. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     */
    CARAPI DumpCursor(
        /* [in] */ ICursor* cursor);

    /**
     * Prints the contents of a Cursor to a PrintSteam. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     * @param stream the stream to print to
     */
    CARAPI DumpCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IPrintStream* stream);

    /**
     * Prints the contents of a Cursor to a StringBuilder. The position
     * is restored after printing.
     *
     * @param cursor the cursor to print
     * @param sb the StringBuilder to print to
     */
    CARAPI DumpCursor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IStringBuilder* sb);

    /**
     * Prints the contents of a Cursor to a String. The position is restored
     * after printing.
     *
     * @param cursor the cursor to print
     * @return a String that contains the dumped cursor
     */
    CARAPI DumpCursorToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ String* str);

    /**
     * Prints the contents of a Cursor's current row to System.out.
     *
     * @param cursor the cursor to print from
     */
    CARAPI DumpCurrentRow(
        /* [in] */ ICursor* cursor);

    /**
     * Prints the contents of a Cursor's current row to a PrintSteam.
     *
     * @param cursor the cursor to print
     * @param stream the stream to print to
     */
    CARAPI DumpCurrentRow(
        /* [in] */ ICursor* cursor,
        /* [in] */ IPrintStream* stream);

    /**
     * Prints the contents of a Cursor's current row to a StringBuilder.
     *
     * @param cursor the cursor to print
     * @param sb the StringBuilder to print to
     */
    CARAPI DumpCurrentRow(
        /* [in] */ ICursor* cursor,
        /* [in] */ IStringBuilder* sb);

    /**
     * Dump the contents of a Cursor's current row to a String.
     *
     * @param cursor the cursor to print
     * @return a String that contains the dumped cursor row
     */
    CARAPI DumpCurrentRowToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ String* str);

    /**
     * Reads a String out of a field in a Cursor and writes it to a Map.
     *
     * @param cursor The cursor to read from
     * @param field The TEXT field to read
     * @param values The {@link ContentValues} to put the value into, with the field as the key
     */
    CARAPI CursorStringToContentValues(
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
    CARAPI CursorStringToInsertHelper(
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
    CARAPI CursorStringToContentValues(
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
    CARAPI CursorInt32ToContentValues(
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
    CARAPI CursorInt32ToContentValues(
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
    CARAPI CursorInt64ToContentValues(
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
    CARAPI CursorInt64ToContentValues(
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
    CARAPI CursorDoubleToCursorValues(
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
    CARAPI CursorDoubleToContentValues(
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
    CARAPI CursorRowToContentValues(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentValues* values);

    /**
     * Query the table for the number of rows in the table.
     * @param db the database the table is in
     * @param table the name of the table to query
     * @return the number of rows in the table
     */
    CARAPI QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [out] */ Int64* result);

    /**
     * Query the table for the number of rows in the table.
     * @param db the database the table is in
     * @param table the name of the table to query
     * @param selection A filter declaring which rows to return,
     *              formatted as an SQL WHERE clause (excluding the WHERE itself).
     *              Passing null will count all rows for the given table
     * @return the number of rows in the table filtered by the selection
     */
    CARAPI QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& selection,
        /* [out] */ Int64* result);

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
    CARAPI QueryNumEntries(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int64* result);

    /**
     * Query the table to check whether a table is empty or not
     * @param db the database the table is in
     * @param table the name of the table to query
     * @return True if the table is empty
     * @hide
     */
    CARAPI QueryIsEmpty(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [out] */ Boolean* result);

    /**
     * Utility method to run the query on the db and return the value in the
     * first column of the first row.
     */
    CARAPI Int64ForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int64* result);

    /**
     * Utility method to run the pre-compiled query and return the value in the
     * first column of the first row.
     */
    CARAPI Int64ForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int64* result);

    /**
     * Utility method to run the query on the db and return the value in the
     * first column of the first row.
     */
    CARAPI StringForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ String* result);

    /**
     * Utility method to run the pre-compiled query and return the value in the
     * first column of the first row.
     */
    CARAPI StringForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ String* result);

    /**
     * Utility method to run the query on the db and return the blob value in the
     * first column of the first row.
     *
     * @return A read-only file descriptor for a copy of the blob value.
     */
    CARAPI BlobFileDescriptorForQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ IParcelFileDescriptor** result);

    /**
     * Utility method to run the pre-compiled query and return the blob value in the
     * first column of the first row.
     *
     * @return A read-only file descriptor for a copy of the blob value.
     */
    CARAPI BlobFileDescriptorForQuery(
        /* [in] */ ISQLiteStatement* prog,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ IParcelFileDescriptor** result);

    /**
     * Reads a String out of a column in a Cursor and writes it to a ContentValues.
     * Adds nothing to the ContentValues if the column isn't present or if its value is null.
     *
     * @param cursor The cursor to read from
     * @param column The column to read
     * @param values The {@link ContentValues} to put the value into
     */
    CARAPI CursorStringToContentValuesIfPresent(
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
    CARAPI CursorInt64ToContentValuesIfPresent(
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
    CARAPI CursorInt16ToContentValuesIfPresent(
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
    CARAPI CursorInt32ToContentValuesIfPresent(
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
    CARAPI CursorFloatToContentValuesIfPresent(
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
    CARAPI CursorDoubleToContentValuesIfPresent(
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
    CARAPI CreateDbFromSqlStatements(
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
    CARAPI GetSqlStatementType(
        /* [in] */ const String& sql,
        /* [out] */ Int32* type);

    /**
     * Appends one set of selection args to another. This is useful when adding a selection
     * argument to a user provided set.
     */
    CARAPI AppendSelectionArgs(
        /* [in] */ ArrayOf<String>* originalValues,
        /* [in] */ ArrayOf<String>* newValues,
        /* [out, callee] */ ArrayOf<String>** args);
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CDATABASEUTILS_H__
