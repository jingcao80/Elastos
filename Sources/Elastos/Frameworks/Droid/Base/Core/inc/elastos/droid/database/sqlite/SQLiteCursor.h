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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECURSOR_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECURSOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/database/AbstractWindowedCursor.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {
/**
 * A Cursor implementation that exposes results from a query on a
 * {@link SQLiteDatabase}.
 *
 * SQLiteCursor is not internally synchronized so code using a SQLiteCursor from multiple
 * threads should perform its own synchronization when using the SQLiteCursor.
 */
class SQLiteCursor
    : public AbstractWindowedCursor
    , public ISQLiteCursor
{
public:
    CAR_INTERFACE_DECL()

    SQLiteCursor();

    /**
     * Execute a query and provide access to its result set through a Cursor
     * interface. For a query such as: {@code SELECT name, birth, phone FROM
     * myTable WHERE ... LIMIT 1,20 ORDER BY...} the column names (name, birth,
     * phone) would be in the projection argument and everything from
     * {@code FROM} onward would be in the params argument. This constructor
     * has package scope.
     *
     * @param db a reference to a Database object that is already constructed
     *     and opened
     * @param editTable the name of the table used for this query
     * @param query the rest of the query terms
     *     cursor is finalized
     */
    CARAPI constructor(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ ISQLiteCursorDriver* driver,
        /* [in] */ const String& editTable,
        /* [in] */ ISQLiteQuery* query);

    /**
     * Execute a query and provide access to its result set through a Cursor
     * interface. For a query such as: {@code SELECT name, birth, phone FROM
     * myTable WHERE ... LIMIT 1,20 ORDER BY...} the column names (name, birth,
     * phone) would be in the projection argument and everything from
     * {@code FROM} onward would be in the params argument.
     *
     * @param editTable the name of the table used for this query
     * @param query the {@link SQLiteQuery} object associated with this cursor object.
     */
    CARAPI constructor(
        /* [in] */ ISQLiteCursorDriver* driver,
        /* [in] */ const String& editTable,
        /* [in] */ ISQLiteQuery* query);

    ~SQLiteCursor();

    /**
     * @return the SQLiteDatabase that this cursor is associated with.
     */
    virtual CARAPI GetDatabase(
        /* [out] */ ISQLiteDatabase** database);

    //@Override
    virtual CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* result);

    //@Override
    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    // @Override
    virtual CARAPI GetColumnIndex(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    // @Override
    virtual CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** columnNames);

    // @Override
    virtual CARAPI Deactivate();

    // @Override
    virtual CARAPI Close();

    // @Override
    virtual CARAPI Requery(
        /* [out] */ Boolean* succeeded);

    // @Override
    virtual CARAPI SetWindow(
        /* [in] */ ICursorWindow* window);

    /**
     * Changes the selection arguments. The new values take effect after a call to requery().
     */
    virtual CARAPI SetSelectionArguments(
        /* [in] */ ArrayOf<String>* selectionArgs);

private:
    using AbstractWindowedCursor::FillWindow;

    CARAPI_(void) FillWindow(
        /* [in] */ Int32 requiredPos);

private:
    static const String TAG;
    static const Int32 NO_COUNT = -1;

    /** The name of the table to edit */
    String mEditTable;

    /** The names of the columns in the rows */
    AutoPtr< ArrayOf<String> > mColumns;

    /** The query object for the cursor */
    AutoPtr<ISQLiteQuery> mQuery;

    /** The compiled query this cursor came from */
    AutoPtr<ISQLiteCursorDriver> mDriver;

    /** The number of rows in the cursor */
    Int32 mCount;

    /** The number of rows that can fit in the cursor window, 0 if unknown */
    Int32 mCursorWindowCapacity;

    /** A mapping of column names to column indices, to speed up lookups */
    AutoPtr<HashMap<String, Int32> > mColumnNameMap;

    /** Used to find out where a cursor was allocated in case it never got released. */
//   Throwable mStackTrace;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECURSOR_H__
