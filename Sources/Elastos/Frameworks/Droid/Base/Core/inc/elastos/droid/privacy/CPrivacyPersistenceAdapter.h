
#ifndef __ELASTOS_DROID_PRIVACY_CPRIVACYPERSISTENCEADAPTER_H__
#define __ELASTOS_DROID_PRIVACY_CPRIVACYPERSISTENCEADAPTER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Privacy_CPrivacyPersistenceAdapter.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/RWLock.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::RWLock;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Privacy {

CarClass(CPrivacyPersistenceAdapter)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPrivacyPersistenceAdapter();

    ~CPrivacyPersistenceAdapter();

    CARAPI constructor(
        /* [in] */ IContext * pContext);

    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out]*/ String* value);

    CARAPI SetValue(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [out]*/Boolean* result);

    CARAPI GetSettings(
        /* [in] */ const String& packageName,
        /* [out] */ IPrivacySettings** privacySettings);

    CARAPI SaveSettings(
        /* [in] */ IPrivacySettings* s,
        /* [out] */ Boolean* result);

    CARAPI DeleteSettings(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI PurgeSettings(
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) UpgradeDatabase();

    CARAPI_(Int32) GetDbVersion();

    CARAPI_(Boolean) WriteExternalSettings(
        /* [in] */ const String& settingsName,
        /* [in] */ const String& packageName,
        /* [in] */ IPrivacySettings* s);

    CARAPI Query(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ ArrayOf<String>* columns,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& groupBy,
        /* [in] */ const String& having,
        /* [in] */ const String& orderBy,
        /* [in] */ const String& limit,
        /* [out] */ ICursor** cursor);

    CARAPI RawQuery(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sql,
        /* [out] */ ICursor** cursor);

    CARAPI_(void) DeleteRecursive(
        /* [in] */ IFile* fileOrDirectory);

    CARAPI CreateDatabase();

    CARAPI_(void) CreateSettingsDir();

    CARAPI_(AutoPtr<ISQLiteDatabase>) GetDatabase();

    CARAPI_(void) CloseIdleDatabase();

    static CARAPI_(String) InitCreateTableSettings();
    static CARAPI_(String) InitCreateTableMap();
    static CARAPI_(String) InitCreateTableAllowedContacts();
    static CARAPI_(String) InitInsertVersion();
    static CARAPI_(String) InitInsertEnabled();
    static CARAPI_(String) InitInsertNotificationsEnabled();
    static CARAPI_(AutoPtr< ArrayOf<String> >) InitDatabaseFields();

public:
    static const Int32 DUMMY_UID = -1;

    /**
     * Number of threads currently reading the database Could probably be
     * improved by using 'AtomicInteger'
     */
    static volatile Int32 sDbAccessThreads;
    static Object sDbAccessThreadsLock;
    static volatile Int32 sDbVersion;

    /**
     * Used to save settings for access from core libraries
     */
    static const String SETTINGS_DIRECTORY;

    static const String SETTING_ENABLED;
    static const String SETTING_NOTIFICATIONS_ENABLED;
    static const String SETTING_DB_VERSION;
    static const String VALUE_TRUE;
    static const String VALUE_FALSE;

private:
    static const String TAG;
    static const Int32 RETRY_QUERY_COUNT = 5;
    static const String DATABASE_FILE;
    static const Int32 DATABASE_VERSION = 4;
    static const Boolean LOG_LOCKING = FALSE;
    static const Boolean LOG_OPEN_AND_CLOSE = FALSE;
    static const Boolean LOG_CACHE = FALSE;

    static const Boolean mUseCache = TRUE;
    static const Boolean mAutoCloseDb = FALSE;

    // Used to lock the database during multi-statement operations to prevent
    // internally inconsistent data reads.
    // Multiple locks could be used to improve efficiency (i.e. for different tables)
    // TODO:
    static RWLock sDbLock;
//    static ReadWriteLock sDbLock = new ReentrantReadWriteLock();

    // The default cache size is somewhat arbitrary at the moment
    // It may be valuable to run some analyses to check the average time between something being dropped from cache
    // and being needed again. A recency-weighted LRU would be even better.
    static const Int32 MINIMUM_CACHE_ENTRIES = 0;
    static const Int32 DEFAULT_CACHE_ENTRIES = 20;
    //Because having privacy settings of 'null' has meaning
    static HashMap<String, AutoPtr<IInterface> > mSettingsCache;

    static const String TABLE_SETTINGS;
    static const String TABLE_MAP;
    static const String TABLE_ALLOWED_CONTACTS;
    static const String TABLE_VERSION;

    static const String CREATE_TABLE_SETTINGS;
    static const String CREATE_TABLE_MAP;
    static const String CREATE_TABLE_ALLOWED_CONTACTS;
    static const String INSERT_VERSION;
    static const String INSERT_ENABLED;
    static const String INSERT_NOTIFICATIONS_ENABLED;
    static const AutoPtr< ArrayOf<String> > DATABASE_FIELDS;

    AutoPtr<ISQLiteDatabase> mDb;

    AutoPtr<IContext> mContext;

    Object mLock;
};

} // namespace Privacy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PRIVACY_CPRIVACYPERSISTENCEADAPTER_H__
