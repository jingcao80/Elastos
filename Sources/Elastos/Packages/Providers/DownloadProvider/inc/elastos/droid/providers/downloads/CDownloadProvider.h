
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADPROVIDER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADPROVIDER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include "elastos/droid/content/ContentProvider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorOnCloseListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IHashMap;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Allows application to interact with the download manager.
 */
CarClass(CDownloadProvider)
    , public ContentProvider
    , public IDownloadProvider
{
private:
    /**
     * This class encapsulates a SQL where clause and its parameters.  It makes it possible for
     * shared methods (like {@link DownloadProvider#getWhereClause(Uri, String, String[], Int32)})
     * to return both pieces of information, and provides some utility logic to ease piece-by-piece
     * construction of selections.
     */
    class SqlSelection
        : public Object
    {
    public:
        SqlSelection();

        CARAPI_(void) AppendClause(
            /* [in] */ String newClause,
            /* [in] */ ArrayOf<String>* parameters);

        CARAPI_(String) GetSelection();

        CARAPI_(AutoPtr<ArrayOf<String> >) GetParameters();

    public:
        StringBuilder mWhereClause;
        AutoPtr<IList> mParameters;
    };

    /**
     * Creates and updated database on demand when opening it.
     * Helper class to create database the first time the provider is
     * initialized and upgrade it when a new version of the provider needs
     * an updated version of the database.
     */
    class DatabaseHelper
        : public SQLiteOpenHelper
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context);

        /**
         * Creates database the first time we try to open it.
         */
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        /**
         * Updates the database format when a content provider is used
         * with a database that was created with a different format.
         *
         * Note: to support downgrades, creating a table should always drop it first if it already
         * exists.
         */
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldV,
            /* [in] */ Int32 newV);

    private:
        /**
         * Upgrade database from (version - 1) to version.
         */
        CARAPI_(void) UpgradeTo(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 version);

        /**
         * insert() now ensures these four columns are never null for new downloads, so this method
         * makes that true for existing columns, so that code can rely on this assumption.
         */
        CARAPI_(void) FillNullValues(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) FillNullValuesForColumn(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values);

        /**
         * Set all existing downloads to the cache partition to be invisible in the downloads UI.
         */
        CARAPI_(void) MakeCacheDownloadsInvisible(
            /* [in] */ ISQLiteDatabase* db);

        /**
         * Add a column to a table using ALTER TABLE.
         * @param dbTable name of the table
         * @param columnName name of the column to add
         * @param columnDefinition SQL for the column definition
         */
        CARAPI_(void) AddColumn(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ String dbTable,
            /* [in] */ String columnName,
            /* [in] */ String columnDefinition);

        /**
         * Creates the table that'll hold the download information.
         */
        CARAPI_(void) CreateDownloadsTable(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI_(void) CreateHeadersTable(
            /* [in] */ ISQLiteDatabase* db);
    };

    class OnCloseListener
        : public Object
        , public IParcelFileDescriptorOnCloseListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnCloseListener(
            /* [in] */ IFile* f,
            /* [in] */ IUri* u,
            /* [in] */ Boolean b,
            /* [in] */ CDownloadProvider* host);

        CARAPI OnClose(
            /* [in] */ ECode ioe);

    public:
        AutoPtr<IFile> mFile;
        AutoPtr<IUri> mUri;
        Boolean mShouldScan;
        CDownloadProvider* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadProvider();

    /**
     * Initializes the content provider when it is created.
     */
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    /**
     * Returns the content-provider-style MIME types of the various
     * types accessible through this content provider.
     */
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* result);

    /**
     * Inserts a row in the database
     */
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** result);

    /**
     * Starts a database query
     */
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** result);

    /**
     * Updates a row in the database
     */
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    /**
     * Deletes a row in the database
     */
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    /**
     * Remotely opens a file
     */
    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
     * Check that the file URI provided for DESTINATION_FILE_URI is valid.
     */
    CARAPI_(void) CheckFileUriDestination(
        /* [in] */ IContentValues* values);

    /**
     * Apps with the ACCESS_DOWNLOAD_MANAGER permission can access this provider freely, subject to
     * constraints in the rest of the code. Apps without that may still access this provider through
     * the public API, but additional restrictions are imposed. We check those restrictions here.
     *
     * @param values ContentValues provided to insert()
     * @throws SecurityException if the caller has insufficient permissions
     */
    CARAPI_(void) CheckInsertPermissions(
        /* [in] */ IContentValues* values);

    /**
     * Remove column from values, and throw a SecurityException if the value isn't within the
     * specified allowedValues.
     */
    CARAPI_(void) EnforceAllowedValues(
        /* [in] */ IContentValues* values,
        /* [in] */ String column,
        /* [in] */ ArrayOf<IInterface*>* allowedValues);

    CARAPI_(AutoPtr<ICursor>) QueryCleared(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ String selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ String sort);

    CARAPI_(void) LogVerboseQueryInfo(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ String selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ String sort,
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(String) GetDownloadIdFromUri(
        /* [in] */ IUri* uri);

    /**
     * Insert request headers for a download into the DB.
     */
    CARAPI InsertRequestHeaders(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 downloadId,
        /* [in] */ IContentValues* values);

    /**
     * Handle a query for the custom request headers registered for a download.
     */
    CARAPI_(AutoPtr<ICursor>) QueryRequestHeaders(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ IUri* uri);

    /**
     * Delete request headers for downloads matching the given query.
     */
    CARAPI_(void) DeleteRequestHeaders(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ String where,
        /* [in] */ ArrayOf<String>* whereArgs);

    /**
     * @return true if we should restrict the columns readable by this caller
     */
    CARAPI_(Boolean) ShouldRestrictVisibility();

    /**
     * Notify of a change through both URIs (/my_downloads and /all_downloads)
     * @param uri either URI for the changed download(s)
     * @param uriMatch the match ID from {@link #sURIMatcher}
     */
    CARAPI_(void) NotifyContentChanged(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 uriMatch);

    CARAPI_(AutoPtr<SqlSelection>) GetWhereClause(
        /* [in] */ IUri* uri,
        /* [in] */ String where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [in] */ Int32 uriMatch);

    CARAPI_(void) LogVerboseOpenFileInfo(
        /* [in] */ IUri* uri,
        /* [in] */ String mode);

    static CARAPI_(void) CopyInteger(
        /* [in] */ String key,
        /* [in] */ IContentValues* from,
        /* [in] */ IContentValues* to);

    static CARAPI_(void) CopyBoolean(
        /* [in] */ String key,
        /* [in] */ IContentValues* from,
        /* [in] */ IContentValues* to);

    static CARAPI_(void) CopyString(
        /* [in] */ String key,
        /* [in] */ IContentValues* from,
        /* [in] */ IContentValues* to);

    static CARAPI_(void) CopyStringWithDefault(
        /* [in] */ String key,
        /* [in] */ IContentValues* from,
        /* [in] */ IContentValues* to,
        /* [in] */ String defaultValue);

public:
    /** Database filename */
    static String DB_NAME;
    /** Current database version */
    static Int32 DB_VERSION;
    /** Name of table in the database */
    static String DB_TABLE;

    /** MIME type for the entire download list */
    static String DOWNLOAD_LIST_TYPE;
    /** MIME type for an individual download */
    static String DOWNLOAD_TYPE;

    /** URI matcher used to recognize URIs sent by applications */
    static AutoPtr<IUriMatcher> sURIMatcher;
    /** URI matcher constant for the URI of all downloads belonging to the calling UID */
    static const Int32 MY_DOWNLOADS;
    /** URI matcher constant for the URI of an individual download belonging to the calling UID */
    static const Int32 MY_DOWNLOADS_ID;
    /** URI matcher constant for the URI of all downloads in the system */
    static const Int32 ALL_DOWNLOADS;
    /** URI matcher constant for the URI of an individual download */
    static const Int32 ALL_DOWNLOADS_ID;
    /** URI matcher constant for the URI of a download's request headers */
    static Int32 REQUEST_HEADERS_URI;
    /** URI matcher constant for the public URI returned by
     * {@link DownloadManager#getUriForDownloadedFile(long)} if the given downloaded file
     * is publicly accessible.
     */
    static const Int32 PUBLIC_DOWNLOAD_ID;

    /** Different base URIs that could be used to access an individual download */
    static AutoPtr<ArrayOf<IUri*> > BASE_URIS;

    static AutoPtr<ArrayOf<String> > sAppReadableColumnsArray;

    static AutoPtr<IHashSet> sAppReadableColumnsSet;
    static AutoPtr<IHashMap> sColumnsMap;

    static AutoPtr<IList> downloadManagerColumnsList;

    AutoPtr<IHandler> mHandler;

    /** The database that lies underneath this content provider */
    AutoPtr<DatabaseHelper> mOpenHelper;

    /** List of uids that can access the downloads */
    Int32 mSystemUid;
    Int32 mDefContainerUid;

    AutoPtr<ISystemFacade> mSystemFacade;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADPROVIDER_H__
