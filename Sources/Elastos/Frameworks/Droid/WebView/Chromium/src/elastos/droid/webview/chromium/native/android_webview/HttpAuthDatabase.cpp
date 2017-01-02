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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/HttpAuthDatabase.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

using Elastos::IO::ICloseable;
using Elastos::Core::IThread;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//                 HttpAuthDatabase::InnerThread
//===============================================================

HttpAuthDatabase::InnerThread::InnerThread(
    /* [in] */ HttpAuthDatabase* owner,
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseFile)
    : mOwner(owner)
    , mContext(context)
    , mDatabaseFile(databaseFile)
{
}

ECode HttpAuthDatabase::InnerThread::Run()
{
    mOwner->InitOnBackgroundThread(mContext, mDatabaseFile);
    return NOERROR;
}

//===============================================================
//                      HttpAuthDatabase
//===============================================================

AutoPtr<ArrayOf<String> > HttpAuthDatabase::ID_PROJECTION_Init()
{
    AutoPtr<ArrayOf<String> > strArray = ArrayOf<String>::Alloc(1);
    (*strArray)[0] = HttpAuthDatabase::ID_COL;
    return strArray;
}

const String HttpAuthDatabase::LOGTAG("HttpAuthDatabase");
const Int32 HttpAuthDatabase::DATABASE_VERSION;
const String HttpAuthDatabase::ID_COL("_id");
const AutoPtr<ArrayOf<String> > HttpAuthDatabase::ID_PROJECTION = HttpAuthDatabase::ID_PROJECTION_Init();

// column id strings for "httpauth" table
const String HttpAuthDatabase::HTTPAUTH_TABLE_NAME("httpauth");
const String HttpAuthDatabase::HTTPAUTH_HOST_COL("host");
const String HttpAuthDatabase::HTTPAUTH_REALM_COL("realm");
const String HttpAuthDatabase::HTTPAUTH_USERNAME_COL("username");
const String HttpAuthDatabase::HTTPAUTH_PASSWORD_COL("password");

/**
 * Create an instance of HttpAuthDatabase for the named file, and kick-off background
 * initialization of that database.
 *
 * @param context the Context to use for opening the database
 * @param databaseFile Name of the file to be initialized.
 */
HttpAuthDatabase::HttpAuthDatabase(
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseFile)
    : mInitialized(FALSE)
{
    AutoPtr<IThread> thread = new InnerThread(this, context, databaseFile);
    thread->Start();
}

/**
 * Initializes the databases and notifies any callers waiting on waitForInit.
 *
 * @param context the Context to use for opening the database
 * @param databaseFile Name of the file to be initialized.
 */
void HttpAuthDatabase::InitOnBackgroundThread(
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseFile)
{
    AutoLock lock(&mInitializedLock);
    if (mInitialized) {
        return;
    }

    InitDatabase(context, databaseFile);

    // Thread done, notify.
    mInitialized = TRUE;
    mInitializedLock.NotifyAll();
}

/**
 * Opens the database, and upgrades it if necessary.
 *
 * @param context the Context to use for opening the database
 * @param databaseFile Name of the file to be initialized.
 */
void HttpAuthDatabase::InitDatabase(
    /* [in] */ IContext* context,
    /* [in] */ const String& databaseFile)
{
    // try {
    ECode ecode = context->OpenOrCreateDatabase(databaseFile, 0, NULL, (ISQLiteDatabase**)&mDatabase);
    if (FAILED(ecode)) {
        Boolean deleteRes;
        ecode = context->DeleteDatabase(databaseFile, &deleteRes);
        if(deleteRes)
            ecode = context->OpenOrCreateDatabase(databaseFile, 0, NULL, (ISQLiteDatabase**)&mDatabase);
    }
    // } catch (SQLiteException e) {
    //     // try again by deleting the old db and create a new one
    //     if (context.deleteDatabase(databaseFile)) {
    //         mDatabase = context.openOrCreateDatabase(databaseFile, 0, null);
    //     }
    // }

    if (mDatabase == NULL) {
        // Not much we can do to recover at this point
        Logger::E(LOGTAG, "Unable to open or create %s", databaseFile.string());
        return;
    }

    Int32 version;
    mDatabase->GetVersion(&version);
    if (version != DATABASE_VERSION) {
        mDatabase->BeginTransactionNonExclusive();
        // try {
            CreateTable();
            mDatabase->SetTransactionSuccessful();
        // } finally {
        //     mDatabase.endTransaction();
        // }
        mDatabase->EndTransaction();
    }
}

void HttpAuthDatabase::CreateTable()
{
    String sql("CREATE TABLE ");
    sql += HTTPAUTH_TABLE_NAME;
    sql += " (";
    sql += ID_COL;
    sql += " INTEGER PRIMARY KEY, ";
    sql += HTTPAUTH_HOST_COL;
    sql += " TEXT, ";
    sql += HTTPAUTH_REALM_COL;
    sql += " TEXT, ";
    sql += HTTPAUTH_USERNAME_COL;
    sql += " TEXT, ";
    sql += HTTPAUTH_PASSWORD_COL;
    sql += " TEXT,";
    sql += " UNIQUE (";
    sql += HTTPAUTH_HOST_COL;
    sql += ", ";
    sql += HTTPAUTH_REALM_COL;
    sql += ") ON CONFLICT REPLACE);";

    mDatabase->ExecSQL(sql);

    mDatabase->SetVersion(DATABASE_VERSION);
}

/**
 * Waits for the background initialization thread to complete and check the database creation
 * status.
 *
 * @return true if the database was initialized, false otherwise
 */
Boolean HttpAuthDatabase::WaitForInit()
{
    {
        AutoLock lock(&mInitializedLock);
        while (!mInitialized) {
            // try {
            ECode ecode = mInitializedLock.Wait();
            if (FAILED(ecode))
                Logger::E(LOGTAG, "Caught exception while checking initialization, ecode:0x%x", ecode);
            // } catch (InterruptedException e) {
            //     Log.e(LOGTAG, "Caught exception while checking initialization", e);
            // }
        }
    }

    return mDatabase != NULL;
}

/**
 * Sets the HTTP authentication password. Tuple (HTTPAUTH_HOST_COL, HTTPAUTH_REALM_COL,
 * HTTPAUTH_USERNAME_COL) is unique.
 *
 * @param host the host for the password
 * @param realm the realm for the password
 * @param username the username for the password.
 * @param password the password
 */
void HttpAuthDatabase::SetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    if (host.IsNullOrEmpty()|| realm.IsNullOrEmpty() || !WaitForInit()) {
        return;
    }

    AutoPtr<IContentValues> c;
    CContentValues::New((IContentValues**)&c);
    c->Put(HTTPAUTH_HOST_COL, host);
    c->Put(HTTPAUTH_REALM_COL, realm);
    c->Put(HTTPAUTH_USERNAME_COL, username);
    c->Put(HTTPAUTH_PASSWORD_COL, password);
    Int64 rowID;
    mDatabase->Insert(HTTPAUTH_TABLE_NAME, HTTPAUTH_HOST_COL, c, &rowID);
}

/**
 * Retrieves the HTTP authentication username and password for a given host and realm pair. If
 * there are multiple username/password combinations for a host/realm, only the first one will
 * be returned.
 *
 * @param host the host the password applies to
 * @param realm the realm the password applies to
 * @return a String[] if found where String[0] is username (which can be null) and
 *         String[1] is password.  Null is returned if it can't find anything.
 */
AutoPtr<ArrayOf<String> > HttpAuthDatabase::GetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    if (host.IsNullOrEmpty()|| realm.IsNullOrEmpty() || !WaitForInit()) {
        return NULL;
    }

    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(2);
    (*columns)[0] = HTTPAUTH_USERNAME_COL;
    (*columns)[1] = HTTPAUTH_PASSWORD_COL;

    String selection("(");
    selection += HTTPAUTH_HOST_COL;
    selection += " == ?) AND ";
    selection += "(";
    selection += HTTPAUTH_REALM_COL;
    selection += " == ?)";

    AutoPtr<ArrayOf<String> > ret;
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<ArrayOf<String> > hostRealm = ArrayOf<String>::Alloc(2);
        (*hostRealm)[0] = host;
        (*hostRealm)[1] = realm;
        mDatabase->Query(HTTPAUTH_TABLE_NAME, columns, selection,
                hostRealm, String(NULL), String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
        Boolean bMoveToFirst = FALSE;
        cursor->MoveToFirst(&bMoveToFirst);
        if (bMoveToFirst) {
            ret = ArrayOf<String>::Alloc(2);
            String str1, str2;
            Int32 index1, index2;
            cursor->GetColumnIndex(HTTPAUTH_USERNAME_COL, &index1);
            cursor->GetString(index1, &str1);
            cursor->GetColumnIndex(HTTPAUTH_USERNAME_COL, &index2);
            cursor->GetString(index2, &str2);
            (*ret)[0] = str1;
            (*ret)[1] = str2;
        }
    // } catch (IllegalStateException e) {
    //     Log.e(LOGTAG, "getHttpAuthUsernamePassword", e);
    // } finally {
    //     if (cursor != null) cursor.close();
    // }
    AutoPtr<ICloseable> cursorClose = ICloseable::Probe(cursor);
    cursorClose->Close();

    return ret;
}

/**
 * Determines if there are any HTTP authentication passwords saved.
 *
 * @return true if there are passwords saved
 */
Boolean HttpAuthDatabase::HasHttpAuthUsernamePassword()
{
    if (!WaitForInit()) {
        return FALSE;
    }

    AutoPtr<ICursor> cursor;
    Boolean ret = FALSE;
    // try {
        mDatabase->Query(HTTPAUTH_TABLE_NAME, ID_PROJECTION, String(NULL), NULL, String(NULL), String(NULL),
                String(NULL), (ICursor**)&cursor);
        cursor->MoveToFirst(&ret);
    // } catch (IllegalStateException e) {
    //     Log.e(LOGTAG, "hasEntries", e);
    // } finally {
    //     if (cursor != null) cursor.close();
    // }
    AutoPtr<ICloseable> cursorClose = ICloseable::Probe(cursor);
    cursorClose->Close();
    return ret;
}

/**
 * Clears the HTTP authentication password database.
 */
void HttpAuthDatabase::ClearHttpAuthUsernamePassword()
{
    if (!WaitForInit()) {
        return;
    }
    Int32 value;
    mDatabase->Delete(HTTPAUTH_TABLE_NAME, String(NULL), NULL, &value);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
