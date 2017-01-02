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

#include "CJDBCConnection.h"
#include "CSystem.h"
#include "CDatabaseX.h"
#include "CJDBCStatement.h"
#include "CJDBCDatabaseMetaData.h"
#include "CJDBCPreparedStatement.h"

using Elastos::Core::ISystem;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CAR_OBJECT_IMPL(CJDBCConnection)

CAR_INTERFACE_IMPL_5(CJDBCConnection, Object, IJDBCConnection, IConnection, IWrapper, IBusyHandler, ICloseable)

ECode CJDBCConnection::ClearWarnings()
{
    return NOERROR;
}

ECode CJDBCConnection::Close()
{
    ECode ec = NOERROR;
    ec = Rollback();
    if (FAILED(ec)) {
        return E_SQL_EXCEPTION;
    }

    mIntrans = FALSE;
    if (mDb != NULL) {
        ec = mDb->Close();
        if (FAILED(ec)) {
            return E_SQL_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CJDBCConnection::Commit()
{

    if (mDb == NULL) {
        return E_SQL_EXCEPTION;
    }

    if (!mIntrans) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    ec = mDb->Exec(String("COMMIT"), NULL);
    if (FAILED(ec)) {
        return E_SQL_EXCEPTION;
    }

    mIntrans = FALSE;
    return NOERROR;
}

ECode CJDBCConnection::CreateStatement(
    /* [out] */ IStatement ** ppStatement)
{
    AutoPtr<IJDBCStatement> s;
    CJDBCStatement::New(this, (IJDBCStatement**)&s);
    *ppStatement = IStatement::Probe(s);
    REFCOUNT_ADD(*ppStatement)
    return NOERROR;
}

ECode CJDBCConnection::CreateStatement(
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [out] */ IStatement ** ppStatement)
{
    if (resultSetType != IResultSet::TYPE_FORWARD_ONLY &&
        resultSetType != IResultSet::TYPE_SCROLL_INSENSITIVE &&
        resultSetType != IResultSet::TYPE_SCROLL_SENSITIVE) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    if (resultSetConcurrency != IResultSet::CONCUR_READ_ONLY &&
        resultSetConcurrency != IResultSet::CONCUR_UPDATABLE) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;;
    }

    AutoPtr<IJDBCStatement> s;
    CJDBCStatement::New(this, (IJDBCStatement**)&s);
    *ppStatement = IStatement::Probe(s);
    REFCOUNT_ADD(*ppStatement);
    return NOERROR;
}

ECode CJDBCConnection::CreateStatement(
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [in] */ Int32 resultSetHoldability,
    /* [out] */ IStatement ** ppStatement)
{
    if (resultSetHoldability != IResultSet::HOLD_CURSORS_OVER_COMMIT) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    ECode ec = NOERROR;
    ec = CreateStatement(resultSetType, resultSetConcurrency,ppStatement);
    return ec;
}

ECode CJDBCConnection::GetAutoCommit(
    /* [out] */ Boolean * pAuto)
{
    *pAuto = mAutocommit;
    return NOERROR;
}

ECode CJDBCConnection::GetCatalog(
    /* [out] */ String * pCatelog)
{
    *pCatelog = NULL;
    return NOERROR;
}

ECode CJDBCConnection::GetHoldability(
    /* [out] */ Int32 * pHoldability)
{
    *pHoldability = IResultSet::HOLD_CURSORS_OVER_COMMIT;
    return NOERROR;
}

ECode CJDBCConnection::GetMetaData(
    /* [out] */ Elastos::Sql::IDatabaseMetaData ** ppMetadata)
{
    if (*ppMetadata == NULL) {
        CJDBCDatabaseMetaData::New(this, (IJDBCDatabaseMetaData **)ppMetadata);
    }
    return NOERROR;
}

ECode CJDBCConnection::GetTransactionIsolation(
    /* [out] */ Int32 * pValue)
{
    *pValue = mTrmode;
    return NOERROR;
}

ECode CJDBCConnection::GetTypeMap(
    /* [out] */ IMap ** typemap)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::GetWarnings(
    /* [out] */ ISQLWarning ** ppWarning)
{
    *ppWarning = NULL;
    return NOERROR;
}

ECode CJDBCConnection::IsClosed(
    /* [out] */ Boolean * pIsClosed)
{
    if (mDb == NULL) {
        *pIsClosed = TRUE;
    } else {
        *pIsClosed = FALSE;
    }
    return NOERROR;
}

ECode CJDBCConnection::IsReadOnly(
    /* [out] */ Boolean * pIsReadOnly)
{
    *pIsReadOnly = mReadonly;
    return NOERROR;
}

ECode CJDBCConnection::NativeSQL(
    /* [in] */ const String& sql,
    /* [out] */ String * pNsql)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCConnection::PrepareCall(
    /* [in] */ const String& sql,
    /* [out] */ ICallableStatement ** ppCallableStatement)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCConnection::PrepareCall(
    /* [in] */ const String& sql,
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [out] */ ICallableStatement ** ppCallableStatement)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::PrepareCall(
    /* [in] */ const String& sql,
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [in] */ Int32 resultSetHoldability,
    /* [out] */ ICallableStatement ** ppCallableStatement)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [out] */ Elastos::Sql::IPreparedStatement ** ppPreparedStatement)
{
    assert(0 && "TODO");
    return CJDBCPreparedStatement::New(this, sql, (IJDBCPreparedStatement**) ppPreparedStatement);
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [in] */ Int32 autoGeneratedKeys,
    /* [out] */ Elastos::Sql::IPreparedStatement ** ppPreparedStatement)
{
    if (autoGeneratedKeys != IStatement::NO_GENERATED_KEYS) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    return PrepareStatement(sql, ppPreparedStatement);
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [in] */ const ArrayOf<Int32> & columnIndexes,
    /* [out] */ Elastos::Sql::IPreparedStatement ** ppPreparedStatement)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;;
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [out] */ Elastos::Sql::IPreparedStatement ** ppStatement)
{
    if (resultSetType != IResultSet::TYPE_FORWARD_ONLY &&
        resultSetType != IResultSet::TYPE_SCROLL_INSENSITIVE &&
        resultSetType != IResultSet::TYPE_SCROLL_SENSITIVE) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    if (resultSetConcurrency != IResultSet::CONCUR_READ_ONLY &&
        resultSetConcurrency != IResultSet::CONCUR_UPDATABLE) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    return CJDBCPreparedStatement::New(this, sql, ppStatement);
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [in] */ Int32 resultSetType,
    /* [in] */ Int32 resultSetConcurrency,
    /* [in] */ Int32 resultSetHoldability,
    /* [out] */ Elastos::Sql::IPreparedStatement ** ppPreparedStatement)
{
    if (resultSetHoldability != IResultSet::HOLD_CURSORS_OVER_COMMIT) {
        return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
    }
    return PrepareStatement(sql, resultSetType, resultSetConcurrency, ppPreparedStatement);;
}

ECode CJDBCConnection::ReleaseSavepoint(
    /* [in] */ Elastos::Sql::ISavePoint * pSavepoint)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::Rollback()
{
    if (mDb == NULL) {
        return E_SQL_EXCEPTION;
    }
    if (!mIntrans) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    ec = mDb->Exec(String("ROLLBACK"), NULL);
    mIntrans = FALSE;
    return ec;
}

ECode CJDBCConnection::Rollback(
    /* [in] */ Elastos::Sql::ISavePoint * pSavepoint)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::SetAutoCommit(
    /* [in] */ Boolean autoCommit)
{
    if (autoCommit && mIntrans && mDb != NULL) {
        ECode ec = NOERROR;
        ec = mDb->Exec(String("ROLLBACK"), NULL);
        if (ec != NOERROR)
        {
            return ec;
        } else {
            mIntrans = FALSE;
        }
    }
    mAutocommit = autoCommit;
    return NOERROR;
}

ECode CJDBCConnection::SetCatalog(
    /* [in] */ const String& catalog)
{
    return NOERROR;
}

ECode CJDBCConnection::SetHoldability(
    /* [in] */ Int32 holdability)
{
    if (holdability == IResultSet::HOLD_CURSORS_OVER_COMMIT) {
        return NOERROR;
    }
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::SetReadOnly(
    /* [in] */ Boolean readOnly)
{
    if (mIntrans) {
        return E_SQL_EXCEPTION;
    }

    ECode ec = NOERROR;
    if (readOnly != mReadonly) {
        AutoPtr<IDatabase> dbx;
        ec = Open(readOnly, (IDatabase**)&dbx);
        if (FAILED(ec)) {
            return E_SQL_EXCEPTION;
        }
        ec = mDb->Close();
        if (FAILED(ec)) {
            return E_SQL_EXCEPTION;
        }
        mDb = dbx;
        //dbx = null;
        mReadonly = readOnly;;
    }

    return NOERROR;
}

ECode CJDBCConnection::SetSavepoint(
    /* [out] */ Elastos::Sql::ISavePoint ** ppSavepoint)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::SetSavepoint(
    /* [in] */ const String& name,
    /* [out] */ Elastos::Sql::ISavePoint ** ppSavepoint)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::SetTransactionIsolation(
    /* [in] */ Int32 level)
{
    Boolean value;
    ECode ec = NOERROR;
    mDb->Is3(&value);

    if (value) {
        String flag;
        if (level == IConnection::TRANSACTION_READ_UNCOMMITTED &&
        mTrmode != IConnection::TRANSACTION_READ_UNCOMMITTED) {
        flag = "on";
        } else if (level == IConnection::TRANSACTION_SERIALIZABLE &&
               mTrmode != IConnection::TRANSACTION_SERIALIZABLE) {
        flag = "off";
        }
        if (!flag.IsNull()) {
            ec = mDb->Exec(String(""), NULL);
            //db.exec("PRAGMA read_uncommitted = " + flag + ";", null);
            mTrmode = level;
        } else {
            return E_SQL_EXCEPTION;
        }
    }
    if (level != mTrmode) {
        return E_SQL_EXCEPTION;
    }

    return ec;
}

ECode CJDBCConnection::SetTypeMap(
    /* [in] */ IMap * typemap)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::CreateClob(
    /* [out] */ Elastos::Sql::IClob ** ppClob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::CreateBlob(
    /* [out] */ Elastos::Sql::IBlob ** ppBlob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::CreateNClob(
    /* [out] */ Elastos::Sql::INClob ** ppNclob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::CreateSQLXML(
    /* [out] */ Elastos::Sql::ISQLXML ** ppSqlXML)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::IsValid(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean * pValid)
{
    *pValid = TRUE;
    return NOERROR;
}

ECode CJDBCConnection::SetClientInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return E_SQL_CLINET_INFO_EXCEPTION;
}

ECode CJDBCConnection::GetClientInfo(
    /* [out] */ IProperties** properties)
{
    return NOERROR;
}

ECode CJDBCConnection::Busy(
    /* [in] */ const String& table,
    /* [in] */ Int32 count,
    /* [out] */ Boolean * pValue)
{
    return Busy0(mDb, count, pValue);
}

ECode CJDBCConnection::constructor(
    /* [in] */ const String& url,
    /* [in] */ const String& enc,
    /* [in] */ const String& pwd,
    /* [in] */ const String& drep,
    /* [in] */ const String& vfs)
{
    ECode ec = NOERROR;
    if (url.StartWith("sqlite:/")) {
        mDbfile = url.Substring(0, 8);
    } else if (url.StartWith("jdbc:sqlite:/")) {
        mDbfile = url.Substring(0, 13);
    } else {
        return E_SQL_EXCEPTION;
    }

    this->mUrl = url;
    this->mEnc = enc;
    this->mVfs = vfs;

    ec = Open(mReadonly, (IDatabase**)&mDb);
    if (FAILED(ec)) {
        return E_SQL_EXCEPTION;
    }

    if (!pwd.IsNullOrEmpty()) {
        ec = mDb->Key(pwd);
        if (FAILED(ec)) {
            return E_SQL_EXCEPTION;
        }
    }

    IBusyHandler* hd = IBusyHandler::Probe(this);
    ec = mDb->BusyHandler(hd);
    //ec = mDb->BusyHandler(this);
    if (FAILED(ec)) {
        if (mDb != NULL) {
            mDb->Close();
        }
    }
    if (!drep.IsNull() && (drep.StartWith("j") || drep.StartWith("J"))) {
        mUseJulian = TRUE;
    }

    return NOERROR;
}

ECode CJDBCConnection::Busy0(
    /* [in] */ IDatabase* db,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* value)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    if (count <= 1) {
        system->GetCurrentTimeMillis(&mT0);
    }
    if (db != NULL) {
        Int64 t1;
        system->GetCurrentTimeMillis(&t1);
        if (t1 - mT0 > timeout) {
        *value = FALSE;
        return NOERROR;
        }

        AutoPtr<ISynchronize> sync = ISynchronize::Probe(db);
        //sync->Wait(100);
        //db->Probe(100);
        *value = TRUE;
        return NOERROR;
    }

    *value = FALSE;
    return NOERROR;
}

ECode CJDBCConnection::Open(
    /* [in] */ Boolean readonly,
    /* [out] */ IDatabase** db)
{
    AutoPtr<IDatabase> dbx;
    ECode ec = NOERROR;

    ec = CDatabaseX::New((IDatabase**)&dbx);
#if HAVE_SQLITE3
    ec = dbx->Open(mDbfile, readonly ? SQLITE_OPEN_READONLY :
        (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE), mVfs);
#else
    ec = dbx->Open(mDbfile, readonly ? IConstants::SQLITE_OPEN_READONLY :
        (IConstants::SQLITE_OPEN_READWRITE | IConstants::SQLITE_OPEN_CREATE), mVfs);
#endif

    ec = dbx->SetEncoding(mEnc);

    if (FAILED(ec)) {
        return E_SQL_EXCEPTION;
    }

    Int32 loop = 0;
    while(TRUE) {
        dbx->Exec(String("PRAGMA short_column_names = off;"), NULL);
        dbx->Exec(String("PRAGMA full_column_names = on;"), NULL);
        dbx->Exec(String("PRAGMA empty_result_callbacks = on;"), NULL);
        if (Database::Version().Compare(String("2.6.0")) >= 0) {
            dbx->Exec(String("PRAGMA show_datatypes = on;"), NULL);
        }
        if (FAILED(ec)) {
            Int32 err;
            dbx->LastError(&err);
            Boolean value;
            Busy0(dbx, ++loop, &value);
            Boolean isbusyflag = FALSE;
#if HAVE_SQLITE3
            isbusyflag = err != SQLITE_BUSY;
#else
            isbusyflag = err != IConstants::SQLITE_BUSY;
#endif
            if (isbusyflag || !value) {
                dbx->Close();
            }
            continue;
        }
        break;
    }

    *db = dbx;
    REFCOUNT_ADD(*db);
    return NOERROR;
}

ECode CJDBCConnection::GetSQLiteDatabase(
    /* [out] */ IDatabase** ppDb)
{
    VALIDATE_NOT_NULL(ppDb);
    *ppDb = mDb;
    REFCOUNT_ADD(*ppDb);
    return NOERROR;
}

ECode CJDBCConnection::PrepareStatement(
    /* [in] */ const String& sql,
    /* [in] */ const ArrayOf<String>& columnNames,
    /* [out] */ IPreparedStatement** preparedStatement)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::GetClientInfo(
    /* [in] */ const String& name,
    /* [out] */ String* info)
{
    return E_SQL_EXCEPTION;
}

Boolean CJDBCConnection::Busy3(
   /* [in] */ IDatabase* db,
   /* [in] */ Int32 count)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    if (count <= 1) {
        system->GetCurrentTimeMillis(&mT0);
    }
    if (mDb != NULL) {
        Int64 t1;
        system->GetCurrentTimeMillis(&t1);
        if (t1 - mT0 > timeout) {
            return false;
        }
            return true;
    }
    return false;
}

ECode CJDBCConnection::IsWrapperFor(
    /* [in] */ IInterface * iface,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCConnection::SetClientInfo(
    /* [in] */ IProperties* properties)
{
    return E_SQL_CLINET_INFO_EXCEPTION;
}

ECode CJDBCConnection::CreateArrayOf(
    /* [in] */ const String& typeName,
    /* [in] */ const ArrayOf<IInterface * >& elements,
    /* [out] */ IArray ** iarray)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::CreateStruct(
    /* [in] */ const String& typeName,
    /* [in] */ const ArrayOf<IInterface * >& attributes,
    /* [out] */ IStruct ** istruct)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCConnection::Unwrap(
    /* [in] */ PInterface iface,
    /* [out] */ IInterface ** oface)
{
    return E_SQL_EXCEPTION;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
