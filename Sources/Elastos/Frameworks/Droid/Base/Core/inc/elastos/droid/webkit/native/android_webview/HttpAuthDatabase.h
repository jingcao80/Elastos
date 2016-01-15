#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_HTTPAUTHDATABASE_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_HTTPAUTHDATABASE_H__
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Thread;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
// import android.database.sqlite.SQLiteException;
// import android.util.Log;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * This database is used to support WebView's setHttpAuthUsernamePassword and
 * getHttpAuthUsernamePassword methods, and WebViewDatabase's clearHttpAuthUsernamePassword and
 * hasHttpAuthUsernamePassword methods.
 *
 * While this class is intended to be used as a singleton, this property is not enforced in this
 * layer, primarily for ease of testing. To line up with the classic implementation and behavior,
 * there is no specific handling and reporting when SQL errors occur.
 *
 * Note on thread-safety: As per the classic implementation, most API functions have thread safety
 * provided by the underlying SQLiteDatabase instance. The exception is database opening: this
 * is handled in the dedicated background thread, which also provides a performance gain
 * if triggered early on (e.g. as a side effect of CookieSyncManager.createInstance() call),
 * sufficiently in advance of the first blocking usage of the API.
 */
class HttpAuthDatabase
:public Object
{
private:
    class InnerThread
    : public Thread
    {
    public:
        InnerThread(
            /* [in] */ HttpAuthDatabase* owner,
            /* [in] */ IContext* context,
            /* [in] */ const String& databaseFile);

        CARAPI Run();

    private:
        HttpAuthDatabase* mOwner;
        IContext* mContext;
        String mDatabaseFile;
    };

public:
    /**
     * Create an instance of HttpAuthDatabase for the named file, and kick-off background
     * initialization of that database.
     *
     * @param context the Context to use for opening the database
     * @param databaseFile Name of the file to be initialized.
     */
    HttpAuthDatabase(
        /* [in] */ IContext* context,
        /* [in] */ const String& databaseFile);

    /**
     * Sets the HTTP authentication password. Tuple (HTTPAUTH_HOST_COL, HTTPAUTH_REALM_COL,
     * HTTPAUTH_USERNAME_COL) is unique.
     *
     * @param host the host for the password
     * @param realm the realm for the password
     * @param username the username for the password.
     * @param password the password
     */
    CARAPI_(void) SetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

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
    CARAPI_(AutoPtr<ArrayOf<String> >) GetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm);

    /**
     * Determines if there are any HTTP authentication passwords saved.
     *
     * @return true if there are passwords saved
     */
    CARAPI_(Boolean) HasHttpAuthUsernamePassword();

    /**
     * Clears the HTTP authentication password database.
     */
    CARAPI_(void) ClearHttpAuthUsernamePassword();

private:
    /**
     * Initializes the databases and notifies any callers waiting on waitForInit.
     *
     * @param context the Context to use for opening the database
     * @param databaseFile Name of the file to be initialized.
     */
    CARAPI_(void) InitOnBackgroundThread(
        /* [in] */ IContext* context,
        /* [in] */ const String& databaseFile);

    /**
     * Opens the database, and upgrades it if necessary.
     *
     * @param context the Context to use for opening the database
     * @param databaseFile Name of the file to be initialized.
     */
    CARAPI_(void) InitDatabase(
        /* [in] */ IContext* context,
        /* [in] */ const String& databaseFile);

    CARAPI_(void) CreateTable();

    /**
     * Waits for the background initialization thread to complete and check the database creation
     * status.
     *
     * @return true if the database was initialized, false otherwise
     */
    CARAPI_(Boolean) WaitForInit();

private:
    static AutoPtr<ArrayOf<String> > ID_PROJECTION_Init();
    static const String LOGTAG;

    static const Int32 DATABASE_VERSION = 1;

    AutoPtr<ISQLiteDatabase> mDatabase;

    static const String ID_COL;

    static const AutoPtr<ArrayOf<String> > ID_PROJECTION;

    // column id strings for "httpauth" table
    static const String HTTPAUTH_TABLE_NAME;
    static const String HTTPAUTH_HOST_COL;
    static const String HTTPAUTH_REALM_COL;
    static const String HTTPAUTH_USERNAME_COL;
    static const String HTTPAUTH_PASSWORD_COL;

    /**
     * Initially false until the background thread completes.
     */
    Boolean mInitialized;

    Object mInitializedLock;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_HTTPAUTHDATABASE_H__
