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

#include "elastos/droid/database/sqlite/SQLiteSession.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const Int32 SQLiteSession::TRANSACTION_MODE_DEFERRED;
const Int32 SQLiteSession::TRANSACTION_MODE_IMMEDIATE;
const Int32 SQLiteSession::TRANSACTION_MODE_EXCLUSIVE;

SQLiteSession::SQLiteSession(
    /* [in] */ SQLiteConnectionPool* connectionPool)
    : mConnectionPool(connectionPool)
    , mConnectionFlags(0)
    , mConnectionUseCount(0)
{
    if (connectionPool == NULL) {
        //throw new IllegalArgumentException("connectionPool must not be null");
        Slogger::E("SQLiteSession", "connectionPool must not be null");
        assert(0);
    }
}

SQLiteSession::~SQLiteSession()
{
}

Boolean SQLiteSession::HasTransaction()
{
    return mTransactionStack != NULL;
}

Boolean SQLiteSession::HasNestedTransaction()
{
    return mTransactionStack != NULL && mTransactionStack->mParent != NULL;
}

Boolean SQLiteSession::HasConnection()
{
    return mConnection != NULL;
}

ECode SQLiteSession::BeginTransaction(
    /* [in] */ Int32 transactionMode,
    /* [in] */ ISQLiteTransactionListener* transactionListener,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    FAIL_RETURN(ThrowIfTransactionMarkedSuccessful())
    return BeginTransactionUnchecked(transactionMode, transactionListener, connectionFlags, cancellationSignal);
}

ECode SQLiteSession::BeginTransactionUnchecked(
    /* [in] */ Int32 transactionMode,
    /* [in] */ ISQLiteTransactionListener* transactionListener,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
    }

    if (mTransactionStack == NULL) {
        FAIL_RETURN(AcquireConnection(String(NULL), connectionFlags, cancellationSignal)) // might throw
    }

    ECode ec = NOERROR;
    AutoPtr<Transaction> transaction;
    //try {
    // Set up the transaction such that we can back out safely
    // in case we fail part way.
    if (mTransactionStack == NULL) {
        // Execute SQL might throw a runtime exception.
        switch (transactionMode) {
            case TRANSACTION_MODE_IMMEDIATE:
                ec = mConnection->Execute(String("BEGIN IMMEDIATE;"), NULL, cancellationSignal); // might throw
                FAIL_GOTO(ec, fail)
                break;
            case TRANSACTION_MODE_EXCLUSIVE:
                ec = mConnection->Execute(String("BEGIN EXCLUSIVE;"), NULL, cancellationSignal); // might throw
                FAIL_GOTO(ec, fail)
                break;
            default:
                ec = mConnection->Execute(String("BEGIN;"), NULL, cancellationSignal); // might throw
                FAIL_GOTO(ec, fail)
                break;
        }
    }

    // Listener might throw a runtime exception.
    if (transactionListener != NULL) {
        //try {
        ec = transactionListener->OnBegin(); // might throw
        //} catch (RuntimeException ex) {
        if (ec == (ECode)E_RUNTIME_EXCEPTION && mTransactionStack == NULL) {
            mConnection->Execute(String("ROLLBACK;"), NULL, NULL); // might throw
            goto fail;
        }
        //}
    }

    // Bookkeeping can't throw, except an OOM, which is just too bad...
    transaction = ObtainTransaction(transactionMode, transactionListener);
    transaction->mParent = mTransactionStack;
    mTransactionStack = transaction;

fail:
    if (mTransactionStack == NULL) {
        FAIL_RETURN(ReleaseConnection()) // might throw
    }
    return ec;
}

ECode SQLiteSession::SetTransactionSuccessful()
{
    FAIL_RETURN(ThrowIfNoTransaction())
    FAIL_RETURN(ThrowIfTransactionMarkedSuccessful())

    mTransactionStack->mMarkedSuccessful = TRUE;
    return NOERROR;
}

ECode SQLiteSession::EndTransaction(
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    FAIL_RETURN(ThrowIfNoTransaction())
    assert(mConnection != NULL);

    return EndTransactionUnchecked(cancellationSignal, FALSE);
}

ECode SQLiteSession::EndTransactionUnchecked(
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [in] */ Boolean yielding)
{
    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
    }

    AutoPtr<Transaction> top = mTransactionStack;
    Boolean successful = (top->mMarkedSuccessful || yielding) && !top->mChildFailed;

    ECode ec = NOERROR;
    AutoPtr<ISQLiteTransactionListener> listener = top->mListener;
    if (listener != NULL) {
        //try {
        if (successful) {
            ec = listener->OnCommit(); // might throw
        }
        else {
            ec = listener->OnRollback(); // might throw
        }
        //} catch (RuntimeException ex) {
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            successful = FALSE;
        }
        //}
    }

    mTransactionStack = top->mParent;
    RecycleTransaction(top);

    if (mTransactionStack != NULL) {
        if (!successful) {
            mTransactionStack->mChildFailed = TRUE;
        }
    }
    else {
        //try {
        if (successful) {
            ec = mConnection->Execute(String("COMMIT;"), NULL, cancellationSignal); // might throw
        }
        else {
            ec = mConnection->Execute(String("ROLLBACK;"), NULL, cancellationSignal); // might throw
        }
        //} finally {
        FAIL_RETURN(ReleaseConnection()) // might throw
        //}
    }

    return ec;
}

ECode SQLiteSession::YieldTransaction(
    /* [in] */ Int64 sleepAfterYieldDelayMillis,
    /* [in] */ Boolean throwIfUnsafe,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (throwIfUnsafe) {
        FAIL_RETURN(ThrowIfNoTransaction())
        FAIL_RETURN(ThrowIfTransactionMarkedSuccessful())
        FAIL_RETURN(ThrowIfNestedTransaction())
    }
    else {
        if (mTransactionStack == NULL || mTransactionStack->mMarkedSuccessful
                || mTransactionStack->mParent != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    assert(mConnection != NULL);

    if (mTransactionStack->mChildFailed) {
        *result = FALSE;
        return NOERROR;
    }

    return YieldTransactionUnchecked(sleepAfterYieldDelayMillis, cancellationSignal, result); // might throw
}


ECode SQLiteSession::YieldTransactionUnchecked(
    /* [in] */ Int64 sleepAfterYieldDelayMillis,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
    }

    Boolean isConnection;
    FAIL_RETURN(mConnectionPool->ShouldYieldConnection(mConnection, mConnectionFlags, &isConnection))
    if (!isConnection) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 transactionMode = mTransactionStack->mMode;
    AutoPtr<ISQLiteTransactionListener> listener = mTransactionStack->mListener;
    Int32 connectionFlags = mConnectionFlags;
    FAIL_RETURN(EndTransactionUnchecked(cancellationSignal, TRUE)) // might throw

    if (sleepAfterYieldDelayMillis > 0) {
        //try {
        Thread::Sleep(sleepAfterYieldDelayMillis);
        //} catch (InterruptedException ex) {
            // we have been interrupted, that's all we need to do
        //}
    }

    FAIL_RETURN(BeginTransactionUnchecked(transactionMode, listener, connectionFlags, cancellationSignal)) // might throw
    *result =  TRUE;
    return NOERROR;
}

ECode SQLiteSession::Prepare(
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [in] */ SQLiteStatementInfo* outStatementInfo)
{
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
    }

    ECode ec = AcquireConnection(sql, connectionFlags, cancellationSignal); // might throw
    if (FAILED(ec)) {
        Slogger::E("SQLiteSession", "Failed to AcquireConnection %s", sql.string());
        return ec;
    }
    //try {
    ec = mConnection->Prepare(sql, outStatementInfo); // might throw
    if (FAILED(ec)) {
        ReleaseConnection();
        Slogger::E("SQLiteSession", "Failed to Prepare %s", sql.string());
        return ec;
    }

    //} finally {
    ec = ReleaseConnection(); // might throw
    if (FAILED(ec)) {
        Slogger::E("SQLiteSession", "Failed to ReleaseConnection %s", sql.string());
    }
    //}
    return ec;
}

ECode SQLiteSession::Execute(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        return NOERROR;
    }

    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->Execute(sql, bindArgs, cancellationSignal); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForInt64(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        *result = 0;
        return NOERROR;
    }
    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForInt64(sql, bindArgs, cancellationSignal, result); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForString(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        *str = NULL;
        return NOERROR;
    }

    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForString(sql, bindArgs, cancellationSignal, str); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForBlobFileDescriptor(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        return NOERROR;
    }

    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForBlobFileDescriptor(sql, bindArgs, cancellationSignal, descriptor); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForChangedRowCount(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        *count = 0;
        return NOERROR;
    }
    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForChangedRowCount(sql, bindArgs, cancellationSignal, count); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForLastInsertedRowId(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        *id = 0;
        return NOERROR;
    }

    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForLastInsertedRowId(sql, bindArgs, cancellationSignal, id); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteForCursorWindow(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICursorWindow* window,
    /* [in] */ Int32 startPos,
    /* [in] */ Int32 requiredPos,
    /* [in] */ Boolean countAllRows,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E("SQLiteSession", "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (window == NULL) {
        //throw new IllegalArgumentException("window must not be null.");
        Slogger::E("SQLiteSession", "window must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isSuccess;
    FAIL_RETURN(ExecuteSpecial(sql, bindArgs, connectionFlags, cancellationSignal, &isSuccess))
    if (isSuccess) {
        window->Clear();
        *result = 0;
        return NOERROR;
    }

    FAIL_RETURN(AcquireConnection(sql, connectionFlags, cancellationSignal)) // might throw
    //try {
    ECode ec = mConnection->ExecuteForCursorWindow(sql, bindArgs,
            window, startPos, requiredPos, countAllRows, cancellationSignal, result); // might throw
    //} finally {
    FAIL_RETURN(ReleaseConnection()) // might throw
    //}
    return ec;
}

ECode SQLiteSession::ExecuteSpecial(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 type = DatabaseUtils::GetSqlStatementType(sql);
    switch (type) {
        case DatabaseUtils_STATEMENT_BEGIN:
            FAIL_RETURN(BeginTransaction(TRANSACTION_MODE_EXCLUSIVE, NULL, connectionFlags, cancellationSignal))
            *result = TRUE;
            return NOERROR;

        case DatabaseUtils_STATEMENT_COMMIT:
            FAIL_RETURN(SetTransactionSuccessful())
            FAIL_RETURN(EndTransaction(cancellationSignal));
            *result = TRUE;
            return NOERROR;

        case DatabaseUtils_STATEMENT_ABORT:
            FAIL_RETURN(EndTransaction(cancellationSignal))
            *result = TRUE;
            return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode SQLiteSession::AcquireConnection(
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (mConnection == NULL) {
        assert(mConnectionUseCount == 0);
        FAIL_RETURN(mConnectionPool->AcquireConnection(
            sql, connectionFlags, cancellationSignal, (SQLiteConnection**)&mConnection)); // might throw
        mConnectionFlags = connectionFlags;
    }
    mConnectionUseCount += 1;
    return NOERROR;
}

ECode SQLiteSession::ReleaseConnection()
{
    assert(mConnection != NULL);
    assert(mConnectionUseCount > 0);
    ECode ec = NOERROR;
    if (--mConnectionUseCount == 0) {
        //try {
        ec = mConnectionPool->ReleaseConnection(mConnection); // might throw
        //} finally {
        mConnection = NULL;
        //}
    }
    return ec;
}

ECode SQLiteSession::ThrowIfNoTransaction()
{
    if (mTransactionStack == NULL) {
        Slogger::E("SQLiteSession", "Cannot perform this operation because there is no current transaction.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SQLiteSession::ThrowIfTransactionMarkedSuccessful()
{
    if (mTransactionStack != NULL && mTransactionStack->mMarkedSuccessful) {
        //throw new IllegalStateException("Cannot perform this operation because "
        //        + "the transaction has already been marked successful.  The only "
        //        + "thing you can do now is call endTransaction().");
        Slogger::E("SQLiteSession", "Cannot perform this operation because the transaction"
            " has already been marked successful.  The only thing you can do now is call endTransaction().");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode SQLiteSession::ThrowIfNestedTransaction()
{
    if (HasNestedTransaction()) {
        //throw new IllegalStateException("Cannot perform this operation because "
        //        + "a nested transaction is in progress.");
        Slogger::E("SQLiteSession", "Cannot perform this operation because a nested transaction is in progress.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<SQLiteSession::Transaction> SQLiteSession::ObtainTransaction(
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteTransactionListener* listener)
{
    AutoPtr<Transaction> transaction = mTransactionPool;
    if (transaction != NULL) {
        mTransactionPool = transaction->mParent;
        transaction->mParent = NULL;
        transaction->mMarkedSuccessful = FALSE;
        transaction->mChildFailed = FALSE;
    }
    else {
        transaction = new Transaction();
    }
    transaction->mMode = mode;
    transaction->mListener = listener;
    return transaction;
}

void SQLiteSession::RecycleTransaction(
    /* [in] */ Transaction* transaction)
{
    transaction->mParent = mTransactionPool;
    transaction->mListener = NULL;
    mTransactionPool = transaction;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
