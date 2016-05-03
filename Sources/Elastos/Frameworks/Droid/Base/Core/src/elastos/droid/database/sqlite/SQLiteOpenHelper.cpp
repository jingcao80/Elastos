
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include "elastos/droid/database/sqlite/CSQLiteDatabaseHelper.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>

using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

static const String TAG("SQLiteOpenHelper");
const Boolean SQLiteOpenHelper::DEBUG_STRICT_READONLY = FALSE;

CAR_INTERFACE_IMPL(SQLiteOpenHelper, Object, ISQLiteOpenHelper)

SQLiteOpenHelper::SQLiteOpenHelper()
    : mNewVersion(0)
    , mIsInitializing(FALSE)
    , mEnableWriteAheadLogging(FALSE)
{}

ECode SQLiteOpenHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 version)
{
    return constructor(context, name, factory, version, NULL);
}

ECode SQLiteOpenHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 version,
    /* [in] */ IDatabaseErrorHandler* errorHandler)
{
    if (version < 1) {
        // throw new IllegalArgumentException("Version must be >= 1, was " + version);
        Slogger::E(TAG, "Version must be >= 1, was %d", version);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContext = context;
    mName = name;
    mFactory = factory;
    mNewVersion = version;
    mErrorHandler = errorHandler;
    return NOERROR;
}

ECode SQLiteOpenHelper::GetDatabaseName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode SQLiteOpenHelper::SetWriteAheadLoggingEnabled(
    /* [in] */ Boolean enabled)
{
    synchronized(mLock) {
        if (mEnableWriteAheadLogging != enabled) {
            Boolean isOpen, isReadOnly;
            if (mDatabase != NULL && (mDatabase->IsOpen(&isOpen), isOpen)
                    && (mDatabase->IsReadOnly(&isReadOnly), !isReadOnly)) {
                if (enabled) {
                    Boolean result;
                    FAIL_RETURN(mDatabase->EnableWriteAheadLogging(&result));
                }
                else {
                    FAIL_RETURN(mDatabase->DisableWriteAheadLogging());
                }
            }
            mEnableWriteAheadLogging = enabled;
        }
    }

    return NOERROR;
}

ECode SQLiteOpenHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    ECode ec = NOERROR;
    synchronized(mLock) {
        ec = GetDatabaseLocked(TRUE, database);
    }
    return ec;
}

ECode SQLiteOpenHelper::GetReadableDatabase(
        /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    ECode ec = NOERROR;
    synchronized(mLock) {
        ec =  GetDatabaseLocked(FALSE, database);
    }
    return ec;
}

ECode SQLiteOpenHelper::GetDatabaseLocked(
    /* [in] */ Boolean writable,
    /* [out] */ ISQLiteDatabase** database)
{
    VALIDATE_NOT_NULL(database)
    *database = NULL;
    if (mDatabase != NULL) {
        Boolean isOpen, isReadOnly;
        if(mDatabase->IsOpen(&isOpen), !isOpen) {
            // Darn!  The user closed the database by calling mDatabase.close().
            mDatabase = NULL;
        }
        else if(!writable || (mDatabase->IsReadOnly(&isReadOnly), !isReadOnly)) {
            *database = mDatabase;
            REFCOUNT_ADD(*database)
            return NOERROR;  // The database is already open for business
        }
    }
    if (mIsInitializing) {
        Slogger::E(TAG, "getDatabase called recursively");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = NOERROR;
    AutoPtr<ISQLiteDatabase> db = mDatabase;
    //try {
    mIsInitializing = TRUE;
    if (db != NULL) {
        Boolean isReadOnly;
        if (writable && (db->IsReadOnly(&isReadOnly), isReadOnly)) {
            ASSERT_SUCCEEDED(db->ReopenReadWrite())
        }
    }
    else if (mName.IsNull()) {
        AutoPtr<ISQLiteDatabaseHelper> helper;
        CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
        ASSERT_SUCCEEDED(helper->Create(NULL, (ISQLiteDatabase**)&db))
    }
    else {
        //try {
        if (DEBUG_STRICT_READONLY && !writable) {
            AutoPtr<IFile> file;
            mContext->GetDatabasePath(mName, (IFile**)&file);
            String path;
            file->GetPath(&path);
            AutoPtr<ISQLiteDatabaseHelper> helper;
            CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
            ec = helper->OpenDatabase(path, mFactory, ISQLiteDatabase::OPEN_READONLY, (ISQLiteDatabase**)&db);
        }
        else {
            ec = mContext->OpenOrCreateDatabase(mName, mEnableWriteAheadLogging ?
                IContext::MODE_ENABLE_WRITE_AHEAD_LOGGING : 0, mFactory, (ISQLiteDatabase**)&db);
        }
        //} catch (SQLiteException ex) {
        if (ec == (ECode)E_SQLITE_EXCEPTION) {
            if (writable) {
                mIsInitializing = FALSE;
                if (db != NULL && db != mDatabase) {
                    ICloseable::Probe(db)->Close();
                }
                return ec;
            }
            Slogger::E(TAG, "Couldn't open %s for writing (will try read-only):0x%08x", mName.string(), ec);
            AutoPtr<IFile> file;
            mContext->GetDatabasePath(mName, (IFile**)&file);
            String path;
            file->GetPath(&path);
            AutoPtr<ISQLiteDatabaseHelper> helper;
            CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
            db = NULL;
            ASSERT_SUCCEEDED(helper->OpenDatabase(path, mFactory, ISQLiteDatabase::OPEN_READONLY, (ISQLiteDatabase**)&db))
        }
        //}
    }

    OnConfigure(db);

    Int32 version;
    db->GetVersion(&version);

    if (version != mNewVersion) {
        Boolean isReadOnly;
        if (db->IsReadOnly(&isReadOnly), isReadOnly) {
            Slogger::E(TAG, "Can't upgrade read-only database from version %d to %d: %s",
                version, mNewVersion, mName.string());
            mIsInitializing = FALSE;
            if (db != NULL && db != mDatabase) {
                ICloseable::Probe(db)->Close();
            }
            return E_SQLITE_EXCEPTION;
        }
        db->BeginTransaction();
        //try {
        if (version == 0) {
            OnCreate(db);
        }
        else {
            if (version > mNewVersion) {
                OnDowngrade(db, version, mNewVersion);
            }
            else {
                OnUpgrade(db, version, mNewVersion);
            }
        }
        db->SetVersion(mNewVersion);
        db->SetTransactionSuccessful();
        //} finally {
        db->EndTransaction();
        //}
    }

    OnOpen(db);

    Boolean isReadOnly;
    if (db->IsReadOnly(&isReadOnly), isReadOnly) {
        Slogger::W(TAG, "Opened %s in read-only mode", mName.string());
    }

    mDatabase = db;
    *database = db;
    REFCOUNT_ADD(*database)
//} finally {
    mIsInitializing = FALSE;
    if (db != NULL && db != mDatabase) {
        ICloseable::Probe(db)->Close();
    }
    //}
    return NOERROR;
}

ECode SQLiteOpenHelper::Close()
{
    synchronized(mLock) {
        if (mIsInitializing) {
            //throw new IllegalStateException("Closed during initialization");
            Slogger::E(TAG, "Closed during initialization");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Boolean isOpen;
        if (mDatabase != NULL && (mDatabase->IsOpen(&isOpen), isOpen)) {
            ICloseable::Probe(mDatabase)->Close();
            mDatabase = NULL;
        }
    }
    return NOERROR;
}

ECode SQLiteOpenHelper::OnConfigure(
    /* [in] */ ISQLiteDatabase* db)
{
    return NOERROR;
}

ECode SQLiteOpenHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    // throw new SQLiteException("Can't downgrade database from version " +
    //             oldVersion + " to " + newVersion);
    Slogger::E(TAG, "Can't downgrade database from version %d to %d", oldVersion, newVersion);
    return E_SQLITE_EXCEPTION;
}

ECode SQLiteOpenHelper::OnOpen(
    /* [in] */ ISQLiteDatabase* db)
{
    return NOERROR;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
