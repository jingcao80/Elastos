
#include "elastos/droid/database/sqlite/SQLiteCursor.h"
#include "elastos/droid/database/sqlite/CSQLiteQuery.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const String SQLiteCursor::TAG("SQLiteCursor");
const Int32 SQLiteCursor::NO_COUNT;

CAR_INTERFACE_IMPL(SQLiteCursor, AbstractWindowedCursor, ISQLiteCursor);

SQLiteCursor::SQLiteCursor()
    : mCount(NO_COUNT)
    , mCursorWindowCapacity(0)
{}

ECode SQLiteCursor::constructor(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ISQLiteCursorDriver* driver,
    /* [in] */ const String& editTable,
    /* [in] */ ISQLiteQuery* query)
{
    return constructor(driver, editTable, query);
}

ECode SQLiteCursor::constructor(
    /* [in] */ ISQLiteCursorDriver* driver,
    /* [in] */ const String& editTable,
    /* [in] */ ISQLiteQuery* query)
{
    if (query == NULL) {
        //throw new IllegalArgumentException("query object cannot be null");
        Slogger::E(TAG, "query object cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //if (StrictMode.vmSqliteObjectLeaksEnabled()) {
    //    mStackTrace = new DatabaseObjectNotClosedException().fillInStackTrace();
    //} else {
    //    mStackTrace = null;
    //}
    mDriver = driver;
    mEditTable = editTable;
    mQuery = query;

    mColumns = ((CSQLiteQuery*)query)->GetColumnNames();
    mRowIdColumnIndex = DatabaseUtils::FindRowIdColumnIndex(mColumns);
    return NOERROR;
}

SQLiteCursor::~SQLiteCursor()
{
    //try {
    //    // if the cursor hasn't been closed yet, close it first
    if (mWindow != NULL) {
    //        if (mStackTrace != null) {
    //            String sql = mQuery.getSql();
    //            int len = sql.length();
    //            StrictMode.onSqliteObjectLeaked(
    //                "Finalizing a Cursor that has not been deactivated or closed. " +
    //                "database = " + mQuery.getDatabase().getLabel() +
    //                ", table = " + mEditTable +
    //                ", query = " + sql.substring(0, (len > 1000) ? 1000 : len),
    //               mStackTrace);
    //      }
        Close();
    }
    //} finally {
        //AbstractCursor::Finalize();
    //}
    mColumnNameMap = NULL;
}

ECode SQLiteCursor::GetDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    AutoPtr<ISQLiteDatabase> temp = ((CSQLiteQuery*)mQuery.Get())->GetDatabase();
    *database = temp;
    REFCOUNT_ADD(*database);
    return NOERROR;
}

ECode SQLiteCursor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Make sure the row at newPosition is present in the window
    Int32 startPosition, numRows;
    mWindow->GetStartPosition(&startPosition);
    mWindow->GetNumRows(&numRows);
    if (mWindow == NULL || newPosition < startPosition ||
            newPosition >= (startPosition + numRows)) {
        FillWindow(newPosition);
    }

    *result = TRUE;
    return NOERROR;
}

ECode SQLiteCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mCount == NO_COUNT) {
        FillWindow(0);
    }
    *count = mCount;
    return NOERROR;
}

void SQLiteCursor::FillWindow(
    /* [in] */ Int32 requiredPos)
{
    AutoPtr<ISQLiteDatabase> database;
    String path;
    ASSERT_SUCCEEDED(GetDatabase((ISQLiteDatabase**)&database))
    database->GetPath(&path);
    ClearOrCreateWindow(path);

    ECode ec;
    //try {
    if (mCount == NO_COUNT) {
        Int32 startPos = DatabaseUtils::CursorPickFillWindowStartPosition(requiredPos, 0);
        ec = mQuery->FillWindow(mWindow, startPos, requiredPos, TRUE, &mCount);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            CloseWindow();
            return;
        }
        ec = mWindow->GetNumRows(&mCursorWindowCapacity);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            CloseWindow();
            return;
        }
        //if (Log.isLoggable(TAG, Log.DEBUG)) {
            // StringBuilder sb;
            // sb.Append(String("received count(*) from native_fill_window: "));
            // sb.Append(mCount);
            // Slogger::D(TAG, sb.ToString());
        //}
    }
    else {
        Int32 startPos = DatabaseUtils::CursorPickFillWindowStartPosition(requiredPos, mCursorWindowCapacity);
        Int32 result;
        ec = mQuery->FillWindow(mWindow, startPos, requiredPos, FALSE, &result);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            CloseWindow();
            return;
        }
    }
    // } catch (RuntimeException ex) {
    //         // Close the cursor window if the query failed and therefore will
    //         // not produce any results.  This helps to avoid accidentally leaking
    //         // the cursor window if the client does not correctly handle exceptions
    //         // and fails to close the cursor.
    //         closeWindow();
    //         throw ex;
    // }
}

ECode SQLiteCursor::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex);
    *columnIndex = -1;

    // Create mColumnNameMap on demand
    if (mColumnNameMap == NULL) {
        AutoPtr< ArrayOf<String> > columns = mColumns;
        assert(columns != NULL);
        Int32 columnCount = columns->GetLength();
        AutoPtr< HashMap<String, Int32> > map = new HashMap<String, Int32>(columnCount);
        for (Int32 i = 0; i < columnCount; i++) {
            (*map)[(*columns)[i]] = i;
        }
        mColumnNameMap = map;
    }

    // Hack according to bug 903852
    Int32 periodIndex = columnName.LastIndexOf('.');
    String _columnName = columnName;
    if (periodIndex != -1) {
        // Exception e = new Exception();
        Slogger::E(TAG, "requesting column name with table name -- %s", columnName.string());
        _columnName = columnName.Substring(periodIndex + 1);
    }

    HashMap<String, Int32>::Iterator it = mColumnNameMap->Find(_columnName);
    if (it != mColumnNameMap->End()) {
        *columnIndex = it->mSecond;
    }
    return NOERROR;
}

ECode SQLiteCursor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    VALIDATE_NOT_NULL(columnNames);
    *columnNames = mColumns;
    REFCOUNT_ADD(*columnNames)
    return NOERROR;
}

ECode SQLiteCursor::Deactivate()
{
    FAIL_RETURN(AbstractWindowedCursor::Deactivate());
    return mDriver->CursorDeactivated();
}

ECode SQLiteCursor::Close()
{
    AbstractWindowedCursor::Close();

    ECode ec = NOERROR;
    {    AutoLock syncLock(this);
        ICloseable::Probe(mQuery)->Close();
        ec = mDriver->CursorClosed();
    }
    return ec;
}

ECode SQLiteCursor::Requery(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;

    Boolean isclose;
    if ((IsClosed(&isclose), isclose)) {
        return NOERROR;
    }

    {    AutoLock syncLock(this);
        AutoPtr<ISQLiteDatabase> database = ((CSQLiteQuery*)mQuery.Get())->GetDatabase();
        Boolean isOpened;
        if (database->IsOpen(&isOpened), !isOpened) {
            return NOERROR;
        }

        if (mWindow != NULL) {
            mWindow->Clear();
        }
        mPos = -1;
        mCount = NO_COUNT;

        mDriver->CursorRequeried(this);
    }

    Boolean result;
    ECode ec = AbstractWindowedCursor::Requery(&result);
    if (FAILED(ec)) {
        return ec;
    }

    *succeeded = result;
    return NOERROR;
}

ECode SQLiteCursor::SetWindow(
    /* [in] */ ICursorWindow* window)
{
    FAIL_RETURN(AbstractWindowedCursor::SetWindow(window));
    mCount = NO_COUNT;
    return NOERROR;
}

ECode SQLiteCursor::SetSelectionArguments(
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    return mDriver->SetBindArguments(selectionArgs);
}

} //Sqlite
} //Database
} //Droid
} //Elastos
