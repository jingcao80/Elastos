
#include "elastos/droid/database/sqlite/SQLiteDirectCursorDriver.h"
#include "elastos/droid/database/sqlite/CSQLiteQuery.h"
#include "elastos/droid/database/sqlite/CSQLiteCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(SQLiteDirectCursorDriver, Object, ISQLiteCursorDriver)

SQLiteDirectCursorDriver::SQLiteDirectCursorDriver(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sql,
    /* [in] */ const String& editTable,
    /* [in] */ ICancellationSignal* cancellationSignal)
    : mDatabase(db)
    , mEditTable(editTable)
    , mSql(sql)
    , mCancellationSignal(cancellationSignal)
{}

ECode SQLiteDirectCursorDriver::Query(
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ ICursor** cs)
{
    VALIDATE_NOT_NULL(cs)
    *cs = NULL;

    AutoPtr<ISQLiteQuery> query;
    FAIL_RETURN(CSQLiteQuery::New(mDatabase, mSql, NULL, (ISQLiteQuery**)&query));
    AutoPtr<ICursor> cursor;
    //try {
    ECode ec = ISQLiteProgram::Probe(query)->BindAllArgsAsStrings(selectionArgs);
    if (FAILED(ec)) {
        ICloseable::Probe(query)->Close();
        return ec;
    }

    if (factory == NULL) {
        ec = CSQLiteCursor::New(this, mEditTable, query, (ISQLiteCursor**)&cursor);
    }
    else {
        ec = factory->NewCursor(mDatabase, this, mEditTable, query, (ICursor**)&cursor);
    }
    if (FAILED(ec)) {
        ICloseable::Probe(query)->Close();
        return ec;
    }
    // } catch (RuntimeException ex) {
    //     query.close();
    //     throw ex;
    // }
    mQuery = query;
    *cs = cursor;
    REFCOUNT_ADD(*cs)
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::CursorClosed()
{
    // Do nothing
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::SetBindArguments(
    /* [in] */ ArrayOf<String>* bindArgs)
{
    return ISQLiteProgram::Probe(mQuery)->BindAllArgsAsStrings(bindArgs);;
}

ECode SQLiteDirectCursorDriver::CursorDeactivated()
{
    // Do nothing
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::CursorRequeried(
        /* [in] */ ICursor* cursor)
{
    // Do nothing
    return NOERROR;
}

String SQLiteDirectCursorDriver::ToString()
{
    return String("SQLiteDirectCursorDriver: ") + mSql;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
