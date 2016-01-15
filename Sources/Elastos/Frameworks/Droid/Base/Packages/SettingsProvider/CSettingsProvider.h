
#ifndef __CSETTINGSPROVIDER_H__
#define __CSETTINGSPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_CSettingsProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/FileObserver.h"
#include "DatabaseHelper.h"
#include "util/LruCache.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Utility::LruCache;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SettingsProvider {

CarClass(CSettingsProvider) , public ContentProvider
{
private:
    /**
     * Decode a content URL into the table, projection, and arguments
     * used to access the corresponding database rows.
     */
    class SqlArguments : public ElRefBase
    {
    public:
        static CARAPI CreateSqlArguments(
            /* [in] */ IUri* url,
            /* [in] */ const String& where,
            /* [in] */ ArrayOf<String>* args,
            /* [out] */ SqlArguments** sqlArguments);

        static CARAPI CreateSqlArguments(
            /* [in] */ IUri* url,
            /* [out] */ SqlArguments** sqlArguments);

    private:
        SqlArguments() {}

        /** Operate on existing rows. */
        CARAPI Init(
            /* [in] */ IUri* url,
            /* [in] */ const String& where,
            /* [in] */ ArrayOf<String>* args);

        CARAPI Init(
            /* [in] */ IUri* url);

    public:
        String mTable;
        String mWhere;
        AutoPtr< ArrayOf<String> > mArgs;
    };

    class SettingsFileObserver : public FileObserver
    {
    public:
        SettingsFileObserver(
            /* [in] */ Int32 userHandle,
            /* [in] */ const String& path,
            /* [in] */ CSettingsProvider* host);

        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    private:
        AutoPtr<IAtomicBoolean> mIsDirty;
        Int32 mUserHandle;
        String mPath;
        CSettingsProvider* mHost;
    };

    class CachePrefetchThread
        : public ThreadBase
    {
    public:
        CachePrefetchThread(
            /* [in] */ Int32 userHandle,
            /* [in] */ CSettingsProvider* host);

        CARAPI Run();

    private:
        Int32 mUserHandle;
        CSettingsProvider* mHost;
    };

    class SettingsCache : public LruCache<String, AutoPtr<IBundle> >
    {
    public:
        SettingsCache(
            /* [in] */ const String& name);

        /**
         * Is the whole database table slurped into this cache?
         */
        CARAPI_(Boolean) FullyMatchesDisk();

        CARAPI_(void) SetFullyMatchesDisk(
            /* [in] */ Boolean value);

        /**
         * Atomic cache population, conditional on size of value and if
         * we lost a race.
         *
         * @returns a Bundle to send back to the client from call(), even
         *     if we lost the race.
         */
        CARAPI_(AutoPtr<IBundle>) PutIfAbsent(
            /* [in] */ const String& key,
            /* [in] */ const String& value);

        /**
         * Populates a key in a given (possibly-null) cache.
         */
        static CARAPI_(void) Populate(
            /* [in] */ SettingsCache* cache,
            /* [in] */ IContentValues* contentValues);

        CARAPI_(void) Populate(
            /* [in] */ const String& name,
            /* [in] */ const String& value);

        /**
         * For suppressing duplicate/redundant settings inserts early,
         * checking our cache first (but without faulting it in),
         * before going to sqlite with the mutation.
         */
        static CARAPI_(Boolean) IsRedundantSetValue(
            /* [in] */ SettingsCache* cache,
            /* [in] */ const String& name,
            /* [in] */ const String& value);

    protected:
        CARAPI_(void) EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ const String& key,
            /* [in] */ IBundle* oldValue,
            /* [in] */ IBundle* newValue);

    private:
        String mCacheName;
        Boolean mCacheFullyMatchesDisk;  // has the whole database slurped.

        friend class CSettingsProvider;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CSettingsProvider* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CSettingsProvider::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CSettingsProvider* mHost;
    };

public:
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    CARAPI_(void) OnUserRemoved(
        /* [in] */ Int32 userHandle);

    CARAPI_(AutoPtr<SettingsCache>) CacheForTable(
        /* [in] */ Int32 callingUser,
        /* [in] */ const String& tableName);

    /**
     * Used for wiping a whole cache on deletes when we're not
     * sure what exactly was deleted or changed.
     */
    CARAPI_(void) InvalidateCache(
        /* [in] */ Int32 callingUser,
        /* [in] */ const String& tableName);

    /**
     * Fast path that avoids the use of chatty remoted Cursors.
     */
    // @Override
    CARAPI Call(
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* initialValues,
        /* [out] */ Int32* number);

    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** insertedUri);

    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

private:
    static CARAPI_(Boolean) InitGlobalKeys();

    CARAPI_(Boolean) SettingMovedToGlobal(
        /* [in] */ const String& name);

    /**
     * Get the content URI of a row added to a table.
     * @param tableUri of the entire table
     * @param values found in the row
     * @param rowId of the row
     * @return the content URI for this particular row
     */
    CARAPI GetUriFor(
        /* [in] */ IUri* tableUri,
        /* [in] */ IContentValues* values,
        /* [in] */ Int64 rowId,
        /* [out] */ IUri** uri);

    /**
     * Send a notification when a particular content URI changes.
     * Modify the system property used to communicate the version of
     * this table, for tables which have such a property.  (The Settings
     * contract class uses these to provide client-side caches.)
     * @param uri to send notifications for
     */
    CARAPI_(void) SendNotify(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userHandle);

    /**
     * Make sure the caller has permission to write this data.
     * @param args supplied by the caller
     * @throws SecurityException if the caller is forbidden to write.
     */
    CARAPI CheckWritePermissions(
        /* [in] */ SqlArguments* args);

    CARAPI_(void) EstablishDbTracking(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) StartAsyncCachePopulation(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) FullyPopulateCaches(
        /* [in] */ Int32 userHandle);

    // Slurp all values (if sane in number & size) into cache.
    CARAPI_(void) FullyPopulateCache(
        /* [in] */ DatabaseHelper* dbHelper,
        /* [in] */ const String& table,
        /* [in] */ SettingsCache* cache);

    CARAPI EnsureAndroidIdIsSet(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    // Lazy-initialize the settings caches for non-primary users
    CARAPI_(AutoPtr<SettingsCache>) GetOrConstructCache(
        /* [in] */ Int32 callingUser,
        /* [in] */ HashMap<Int32, AutoPtr<SettingsCache> >& which);

    // Lazy initialize the database helper and caches for this user, if necessary
    CARAPI_(AutoPtr<DatabaseHelper>) GetOrEstablishDatabase(
        /* [in] */ Int32 callingUser);

    // Looks up value 'key' in 'table' and returns either a single-pair Bundle,
    // possibly with a null value, or null on failure.
    CARAPI_(AutoPtr<IBundle>) LookupValue(
        /* [in] */ DatabaseHelper* dbHelper,
        /* [in] */ const String& table,
        /* [in] */ SettingsCache* cache,
        /* [in] */ const String& key);

    CARAPI QueryForUser(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* select,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [in] */ const String& sort,
        /* [in] */ Int32 forUser,
        /* [out] */ ICursor** cursor);

    /*
     * Used to parse changes to the value of Settings.Secure.LOCATION_PROVIDERS_ALLOWED.
     * This setting contains a list of the currently enabled location providers.
     * But helper functions in android.providers.Settings can enable or disable
     * a single provider by using a "+" or "-" prefix before the provider name.
     *
     * @returns whether the database needs to be updated or not, also modifying
     *     'initialValues' if needed.
     */
    CARAPI_(Boolean) ParseProviderList(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues);

    // Settings.put*ForUser() always winds up here, so this is where we apply
    // policy around permission to write settings for other users.
    CARAPI InsertForUser(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [in] */ Int32 desiredUserHandle,
        /* [out] */ IUri** outUri);

public:
    /**
     * Settings which need to be treated as global/shared in multi-user environments.
     */
    static HashSet<String> sSecureGlobalKeys;
    static HashSet<String> sSystemGlobalKeys;

protected:
    // Each defined user has their own settings
    HashMap<Int32, AutoPtr<DatabaseHelper> > mOpenHelpers;

private:
    static const String TAG;
    static const Boolean LOCAL_LOGV = FALSE;

    static const String TABLE_SYSTEM;
    static const String TABLE_SECURE;
    static const String TABLE_GLOBAL;
    static const String TABLE_FAVORITES;
    static const String TABLE_OLD_FAVORITES;

    static const AutoPtr< ArrayOf<String> > COLUMN_VALUE;

    // Caches for each user's settings, access-ordered for acting as LRU.
    // Guarded by themselves.
    static const Int32 MAX_CACHE_ENTRIES = 200;
    static HashMap<Int32, AutoPtr<SettingsCache> > sSystemCaches;
    static HashMap<Int32, AutoPtr<SettingsCache> > sSecureCaches;
    static const AutoPtr<SettingsCache> sGlobalCache;

    // The count of how many known (handled by SettingsProvider)
    // database mutations are currently being handled for this user.
    // Used by file observers to not reload the database when it's ourselves
    // modifying it.
    static HashMap<Int32, AutoPtr<IAtomicInteger32> > sKnownMutationsInFlight;



    // Over this size we don't reject loading or saving settings but
    // we do consider them broken/malicious and don't keep them in
    // memory at least:
    static const Int32 MAX_CACHE_ENTRY_SIZE = 500;

    static const AutoPtr<IBundle> NULL_SETTING;

    // Used as a sentinel value in an instance equality test when we
    // want to cache the existence of a key, but not store its value.
    static const AutoPtr<IBundle> TOO_LARGE_TO_CACHE_MARKER;

    static const Boolean HASINITGLOBALKEYS;

    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IBackupManager> mBackupManager;

    // FileObserver for external modifications to the database file.
    // Note that this is for platform developers only with
    // userdebug/eng builds who should be able to tinker with the
    // sqlite database out from under the SettingsProvider, which is
    // normally the exclusive owner of the database.  But we keep this
    // enabled all the time to minimize development-vs-user
    // differences in testing.
    static HashMap<Int32, AutoPtr<SettingsFileObserver> > sObserverInstances;
    static Object sObserverInstancesLock;

    Object mLock;
};

} // namespace SettingsProvider
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif //__CSETTINGSPROVIDER_H__
