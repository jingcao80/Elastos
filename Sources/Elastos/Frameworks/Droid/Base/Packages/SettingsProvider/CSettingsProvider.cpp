
#include "CSettingsProvider.h"
#include "elastos/droid/text/TextUtils.h"
#include "Manifest.h"
#include <R.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IAbstractCursor;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsNameValueTable;
// using Elastos::Droid::Provider::IDrmStore;
// using Elastos::Droid::Provider::CDrmStore;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SettingsProvider {

ECode CSettingsProvider::SqlArguments::CreateSqlArguments(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* args,
    /* [out] */ SqlArguments** sqlArguments)
{
    AutoPtr<SqlArguments> arguments = new SqlArguments();
    ECode ec = arguments->Init(url, where, args);
    if (SUCCEEDED(ec)) {
        *sqlArguments = arguments;
        REFCOUNT_ADD(*sqlArguments);
        return NOERROR;
    }
    else {
        *sqlArguments = NULL;
        return ec;
    }
}

ECode CSettingsProvider::SqlArguments::CreateSqlArguments(
    /* [in] */ IUri* url,
    /* [out] */ SqlArguments** sqlArguments)
{
    AutoPtr<SqlArguments> arguments = new SqlArguments();
    ECode ec = arguments->Init(url);
    if (SUCCEEDED(ec)) {
        *sqlArguments = arguments;
        REFCOUNT_ADD(*sqlArguments);
        return NOERROR;
    }
    else {
        *sqlArguments = NULL;
        return ec;
    }
}

ECode CSettingsProvider::SqlArguments::Init(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr< ArrayOf<String> > pathSegments;
    url->GetPathSegments((ArrayOf<String>**)&pathSegments);
    if (pathSegments && pathSegments->GetLength() == 1) {
        // of the form content://settings/secure, arbitrary where clause
        mTable = (*pathSegments)[0];
        if (!DatabaseHelper::IsValidTable(mTable)) {
            // throw new IllegalArgumentException("Bad root path: " + mTable);
            Logger::E(TAG, "Bad root path: %s", mTable.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mWhere = where;
        mArgs = args;
    }
    else if (pathSegments && pathSegments->GetLength() != 2) {
        // throw new IllegalArgumentException("Invalid URI: " + url);
        String str;
        url->ToString(&str);
        Logger::E(TAG, "Invalid URI: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (!TextUtils::IsEmpty(where)) {
        // throw new UnsupportedOperationException("WHERE clause not supported: " + url);
        String str;
        url->ToString(&str);
        Logger::E(TAG, "WHERE clause not supported: %s", str.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    else {
        // of the form content://settings/secure/element_name, no where clause
        mTable = (*pathSegments)[0];
        if (!DatabaseHelper::IsValidTable(mTable)) {
            // throw new IllegalArgumentException("Bad root path: " + mTable);
            Logger::E(TAG, "Bad root path: %s", mTable.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (TABLE_SYSTEM.Equals(mTable) || TABLE_SECURE.Equals(mTable) ||
                TABLE_GLOBAL.Equals(mTable)) {
            mWhere = ISettingsNameValueTable::NAME + String("=?");
            String name = (*pathSegments)[1];
            mArgs = ArrayOf<String>::Alloc(1);
            (*mArgs)[0] = name;
            // Rewrite the table for known-migrated names
            if (TABLE_SYSTEM.Equals(mTable) || TABLE_SECURE.Equals(mTable)) {
                if (sSecureGlobalKeys.Find(name) != sSecureGlobalKeys.End()
                        || sSystemGlobalKeys.Find(name) != sSystemGlobalKeys.End()) {
                    mTable = TABLE_GLOBAL;
                }
            }
        }
        else {
            // of the form content://bookmarks/19
            AutoPtr<IContentUris> uris;
            CContentUris::AcquireSingleton((IContentUris**)&uris);
            Int64 id;
            uris->ParseId(url, &id);
            mWhere = String("_id=") + StringUtils::Int64ToString(id);
            mArgs = NULL;
        }
    }
    return NOERROR;
}

ECode CSettingsProvider::SqlArguments::Init(
    /* [in] */ IUri* url)
{
    AutoPtr< ArrayOf<String> > pathSegments;
    url->GetPathSegments((ArrayOf<String>**)&pathSegments);
    if (pathSegments && pathSegments->GetLength() == 1) {
        mTable = (*pathSegments)[0];
        if (!DatabaseHelper::IsValidTable(mTable)) {
            // throw new IllegalArgumentException("Bad root path: " + this.table);
            Logger::E(TAG, "Bad root path: %s", mTable.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mWhere = String(NULL);
        mArgs = NULL;
        return NOERROR;
    }
    else {
        // throw new IllegalArgumentException("Invalid URI: " + url);
        String str;
        url->ToString(&str);
        Logger::E(TAG, "Invalid URI: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}


CSettingsProvider::SettingsFileObserver::SettingsFileObserver(
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& path,
    /* [in] */ CSettingsProvider* host)
    : FileObserver(path, IFileObserver::CLOSE_WRITE | IFileObserver::CREATE | IFileObserver::DELETE | IFileObserver::MOVED_TO | IFileObserver::MODIFY)
    , mUserHandle(userHandle)
    , mPath(path)
    , mHost(host)
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mIsDirty);
}

ECode CSettingsProvider::SettingsFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    HashMap<Int32, AutoPtr<IAtomicInteger32> >::Iterator it = sKnownMutationsInFlight.Find(mUserHandle);
    Int32 modsInFlight = 0;
    if (it != sKnownMutationsInFlight.End()) {
        it->mSecond->Get(&modsInFlight);
    }
    if (modsInFlight > 0) {
        // our own modification.
        return NOERROR;
    }
    Logger::D(TAG, "User %d external modification to %s; event=%d", mUserHandle, mPath.string(), event);
    Boolean result;
    if (mIsDirty->CompareAndSet(FALSE, TRUE, &result), !result) {
        // already handled. (we get a few update events
        // during an sqlite write)
        return NOERROR;
    }
    Logger::D(TAG, "User %d updating our caches for %s", mUserHandle, mPath.string());
    mHost->FullyPopulateCaches(mUserHandle);
    mIsDirty->Set(FALSE);
    return NOERROR;
}


CSettingsProvider::CachePrefetchThread::CachePrefetchThread(
    /* [in] */ Int32 userHandle,
    /* [in] */ CSettingsProvider* host)
    : mUserHandle(userHandle)
    , mHost(host)
{
    Thread::constructor(String("populate-settings-caches"));
}

ECode CSettingsProvider::CachePrefetchThread::Run()
{
    mHost->FullyPopulateCaches(mUserHandle);
    return NOERROR;
}


CSettingsProvider::SettingsCache::SettingsCache(
    /* [in] */ const String& name)
    : LruCache(MAX_CACHE_ENTRIES)
    , mCacheName(name)
    , mCacheFullyMatchesDisk(FALSE)
{}

Boolean CSettingsProvider::SettingsCache::FullyMatchesDisk()
{
    AutoLock lock(this);
    return mCacheFullyMatchesDisk;
}

void CSettingsProvider::SettingsCache::SetFullyMatchesDisk(
    /* [in] */ Boolean value)
{
    AutoLock lock(this);
    mCacheFullyMatchesDisk = value;
}

void CSettingsProvider::SettingsCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ const String& key,
    /* [in] */ IBundle* oldValue,
    /* [in] */ IBundle* newValue)
{
    if (evicted) {
        mCacheFullyMatchesDisk = FALSE;
    }
}

AutoPtr<IBundle> CSettingsProvider::SettingsCache::PutIfAbsent(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IBundle> bundle;
    if (value.IsNull()) {
        bundle = NULL_SETTING;
    }
    else {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        helper->ForPair(String("value"), value, (IBundle**)&bundle);
    }
    if (value.IsNull() || (Int32)value.GetLength() <= MAX_CACHE_ENTRY_SIZE) {
        AutoLock lock(this);
        if (Get(key) == NULL) {
            Put(key, bundle);
        }
    }
    return bundle;
}

void CSettingsProvider::SettingsCache::Populate(
    /* [in] */ SettingsCache* cache,
    /* [in] */ IContentValues* contentValues)
{
    if (cache == NULL) {
        return;
    }
    String name;
    contentValues->GetAsString(ISettingsNameValueTable::NAME, &name);
    if (name.IsNull()) {
        Logger::W(TAG, "null name populating settings cache.");
        return;
    }
    String value;
    contentValues->GetAsString(ISettingsNameValueTable::VALUE, &value);
    cache->Populate(name, value);
}

void CSettingsProvider::SettingsCache::Populate(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    AutoLock lock(this);
    if (value.IsNull() || (Int32)value.GetLength() <= MAX_CACHE_ENTRY_SIZE) {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        AutoPtr<IBundle> bundle;
        helper->ForPair(ISettingsNameValueTable::VALUE, value, (IBundle**)&bundle);
        Put(name, bundle);
    }
    else {
        Put(name, TOO_LARGE_TO_CACHE_MARKER);
    }
}

Boolean CSettingsProvider::SettingsCache::IsRedundantSetValue(
    /* [in] */ SettingsCache* cache,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (cache == NULL) return FALSE;
    AutoLock lock(cache);
    AutoPtr<IBundle> bundle = cache->Get(name);
    if (bundle == NULL) return FALSE;
    String oldValue;
    bundle->GetPairValue(&oldValue);
    if (oldValue.IsNull() && value.IsNull()) return TRUE;
    if ((oldValue.IsNull()) != (value.IsNull())) return FALSE;
    return oldValue.Equals(value);
}


CSettingsProvider::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CSettingsProvider* host)
    : mHost(host)
{}

ECode CSettingsProvider::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent->GetAction(&action), action.Equals(IIntent::ACTION_USER_REMOVED)) {
        Int32 userHandle = IUserHandle::USER_OWNER;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_OWNER, &userHandle);
        if (userHandle != IUserHandle::USER_OWNER) {
            mHost->OnUserRemoved(userHandle);
        }
    }
    return NOERROR;
}


const String CSettingsProvider::TAG("SettingsProvider");
const Boolean CSettingsProvider::LOCAL_LOGV;
const String CSettingsProvider::TABLE_SYSTEM("system");
const String CSettingsProvider::TABLE_SECURE("secure");
const String CSettingsProvider::TABLE_GLOBAL("global");
const String CSettingsProvider::TABLE_FAVORITES("favorites");
const String CSettingsProvider::TABLE_OLD_FAVORITES("old_favorites");

static AutoPtr< ArrayOf<String> > InitColumnValue()
{
    AutoPtr< ArrayOf<String> > temp = ArrayOf<String>::Alloc(1);
    (*temp)[0] = String("value");
    return temp;
}
const AutoPtr< ArrayOf<String> > CSettingsProvider::COLUMN_VALUE = InitColumnValue();
const Int32 CSettingsProvider::MAX_CACHE_ENTRIES;
HashMap<Int32, AutoPtr<CSettingsProvider::SettingsCache> > CSettingsProvider::sSystemCaches;
HashMap<Int32, AutoPtr<CSettingsProvider::SettingsCache> > CSettingsProvider::sSecureCaches;
const AutoPtr<CSettingsProvider::SettingsCache> CSettingsProvider::sGlobalCache = new CSettingsProvider::SettingsCache(TABLE_GLOBAL);
HashMap<Int32, AutoPtr<IAtomicInteger32> > CSettingsProvider::sKnownMutationsInFlight;
const Int32 CSettingsProvider::MAX_CACHE_ENTRY_SIZE;

AutoPtr<IBundle> InitBundle(
    /* [in] */ const String& str)
{
    AutoPtr<IBundleHelper> helper;
    CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
    AutoPtr<IBundle> b;
    helper->ForPair(str, String(NULL), (IBundle**)&b);
    return b;
}
const AutoPtr<IBundle> CSettingsProvider::NULL_SETTING = InitBundle(String("value"));
const AutoPtr<IBundle> CSettingsProvider::TOO_LARGE_TO_CACHE_MARKER = InitBundle(String("_dummy"));

HashSet<String> CSettingsProvider::sSecureGlobalKeys;
HashSet<String> CSettingsProvider::sSystemGlobalKeys;

Boolean CSettingsProvider::InitGlobalKeys()
{
    // Keys (name column) from the 'secure' table that are now in the owner user's 'global'
    // table, shared across all users
    // These must match Settings.Secure.MOVED_TO_GLOBAL
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    secure->GetMovedKeys(container);
    AutoPtr<IObjectEnumerator> enumerator;
    container->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while(enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<ICharSequence> cs;
        enumerator->Current((IInterface**)&cs);
        String str;
        cs->ToString(&str);
        sSecureGlobalKeys.Insert(str);
    }

    // Keys from the 'system' table now moved to 'global'
    // These must match Settings.System.MOVED_TO_GLOBAL
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    AutoPtr<IObjectContainer> container1;
    CObjectContainer::New((IObjectContainer**)&container1);
    sys->GetNonLegacyMovedKeys(container1);
    AutoPtr<IObjectEnumerator> enumerator1;
    container1->GetObjectEnumerator((IObjectEnumerator**)&enumerator1);
    hasNext = FALSE;
    while(enumerator1->MoveNext(&hasNext), hasNext) {
        AutoPtr<ICharSequence> cs;
        enumerator1->Current((IInterface**)&cs);
        String str;
        cs->ToString(&str);
        sSystemGlobalKeys.Insert(str);
    }

    return TRUE;
}
const Boolean CSettingsProvider::HASINITGLOBALKEYS = CSettingsProvider::InitGlobalKeys();

HashMap<Int32, AutoPtr<CSettingsProvider::SettingsFileObserver> > CSettingsProvider::sObserverInstances;
Mutex CSettingsProvider::sObserverInstancesLock;

Boolean CSettingsProvider::SettingMovedToGlobal(
    /* [in] */ const String& name)
{
    return sSecureGlobalKeys.Find(name) != sSecureGlobalKeys.End() || sSystemGlobalKeys.Find(name) != sSystemGlobalKeys.End();
}

ECode CSettingsProvider::GetUriFor(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values,
    /* [in] */ Int64 rowId,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    AutoPtr< ArrayOf<String> > pathSegments;
    tableUri->GetPathSegments((ArrayOf<String>**)&pathSegments);
    if (pathSegments && pathSegments->GetLength() != 1) {
        String str;
        tableUri->ToString(&str);
        Logger::E(TAG, "Invalid URI: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Invalid URI: " + tableUri);
    }
    String table = (*pathSegments)[0];
    if (TABLE_SYSTEM.Equals(table) || TABLE_SECURE.Equals(table) || TABLE_GLOBAL.Equals(table)) {
        String name;
        values->GetAsString(ISettingsNameValueTable::NAME, &name);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        return helper->WithAppendedPath(tableUri, name, uri);
    }
    else {
        AutoPtr<IContentUris> uris;
        CContentUris::AcquireSingleton((IContentUris**)&uris);
        return uris->WithAppendedId(tableUri, rowId, uri);
    }
}

void CSettingsProvider::SendNotify(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userHandle)
{
    // Update the system property *first*, so if someone is listening for
    // a notification and then using the contract class to get their data,
    // the system property will be updated and they'll get the new data.

    Boolean backedUpDataChanged = FALSE;
    AutoPtr< ArrayOf<String> > pathSegments;
    uri->GetPathSegments((ArrayOf<String>**)&pathSegments);
    String property, table;
    if (pathSegments) table = (*pathSegments)[0];
    Boolean isGlobal = table.Equals(TABLE_GLOBAL);
    if (table.Equals(TABLE_SYSTEM)) {
        property = ISettingsSystem::SYS_PROP_SETTING_VERSION;
        backedUpDataChanged = TRUE;
    }
    else if (table.Equals(TABLE_SECURE)) {
        property = ISettingsSecure::SYS_PROP_SETTING_VERSION;
        backedUpDataChanged = TRUE;
    }
    else if (isGlobal) {
        property = ISettingsGlobal::SYS_PROP_SETTING_VERSION;    // this one is global
        backedUpDataChanged = TRUE;
    }

    if (!property.IsNull()) {
        AutoPtr<ISystemProperties> prop;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
        Int64 value;
        prop->GetInt64(property, 0, &value);
        Int64 version = value + 1;
        if (LOCAL_LOGV) Logger::V(TAG, "property: %s=%d", property.string(), version);
        prop->Set(property, StringUtils::Int64ToString(version));
    }

    // Inform the backup manager about a data change
    if (backedUpDataChanged) {
        Logger::W(TAG, "Backup data has changed!!!!!!");
        if (mBackupManager) {
            mBackupManager->DataChanged();
        }
    }
    // Now send the notification through the content framework.

    String notify;
    uri->GetQueryParameter(String("notify"), &notify);
    if (notify.IsNull() || String("true").Equals(notify)) {
        Int32 notifyTarget = isGlobal ? IUserHandle::USER_ALL : userHandle;
        AutoPtr<IBinderHelper> helper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&helper);
        Int64 oldId;
        helper->ClearCallingIdentity(&oldId);
        // try {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        resolver->NotifyChange(uri, NULL, TRUE, notifyTarget);
        // } finally {
        helper->RestoreCallingIdentity(oldId);
        // }
        if (LOCAL_LOGV) {
            String str;
            uri->ToString(&str);
            Logger::V(TAG, "notifying for %d: %p", notifyTarget, str.string());
        }
    }
    else {
        if (LOCAL_LOGV) {
            String str;
            uri->ToString(&str);
            Logger::V(TAG, "notification suppressed: %p", str.string());
        }
    }
}

ECode CSettingsProvider::CheckWritePermissions(
    /* [in] */ SqlArguments* args)
{

    if (TABLE_SECURE.Equals(args->mTable) || TABLE_GLOBAL.Equals(args->mTable)) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        Int32 value;
        FAIL_RETURN(context->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS, &value))
        if (value != IPackageManager::PERMISSION_GRANTED) {
            Logger::E(TAG, "Permission denial: writing to secure settings requires %1$s",
                    Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS.string());
            return E_SECURITY_EXCEPTION;
            // throw new SecurityException(
            //         String.format("Permission denial: writing to secure settings requires %1$s",
            //                       android.Manifest.permission.WRITE_SECURE_SETTINGS));
        }
    }
    return NOERROR;
}

ECode CSettingsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::BACKUP_SERVICE, (IInterface**)&service);
    mBackupManager = IBackupManager::Probe(service);
    assert(mBackupManager != NULL);

    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&mUserManager);

    EstablishDbTracking(IUserHandle::USER_OWNER);

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, userFilter, (IIntent**)&intent);
    *result = TRUE;
    return NOERROR;
}

void CSettingsProvider::OnUserRemoved(
    /* [in] */ Int32 userHandle)
{
    AutoLock lock(mLock);
    // the db file itself will be deleted automatically, but we need to tear down
    // our caches and other internal bookkeeping.
    HashMap<Int32, AutoPtr<SettingsFileObserver> >::Iterator it = sObserverInstances.Find(userHandle);
    AutoPtr<IFileObserver> observer;
    if (it != sObserverInstances.End()) {
        observer = (IFileObserver*)it->mSecond;
    }
    if (observer != NULL) {
        observer->StopWatching();
        sObserverInstances.Erase(it);
    }

    mOpenHelpers.Erase(userHandle);
    sSystemCaches.Erase(userHandle);
    sSecureCaches.Erase(userHandle);
    sKnownMutationsInFlight.Erase(userHandle);
}

void CSettingsProvider::EstablishDbTracking(
    /* [in] */ Int32 userHandle)
{
    if (LOCAL_LOGV) {
        Logger::I(TAG, "Installing settings db helper and caches for user %d", userHandle);
    }

    AutoPtr<DatabaseHelper> dbhelper;

    {
        AutoLock lock(mLock);
        HashMap<Int32, AutoPtr<DatabaseHelper> >::Iterator it = mOpenHelpers.Find(userHandle);
        if (it != mOpenHelpers.End()) {
            dbhelper = it->mSecond;
        }
        if (dbhelper == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            dbhelper = new DatabaseHelper(context, userHandle);
            mOpenHelpers[userHandle] = dbhelper;

            AutoPtr<SettingsCache> systemCache = new SettingsCache(TABLE_SYSTEM);
            AutoPtr<SettingsCache> secureCache = new SettingsCache(TABLE_SECURE);
            sSystemCaches[userHandle] = systemCache;
            sSecureCaches[userHandle] = secureCache;
            AutoPtr<IAtomicInteger32> integer;
            CAtomicInteger32::New(0, (IAtomicInteger32**)&integer);
            sKnownMutationsInFlight[userHandle] = integer;
        }
    }

    // Initialization of the db *outside* the locks.  It's possible that racing
    // threads might wind up here, the second having read the cache entries
    // written by the first, but that's benign: the SQLite helper implementation
    // manages concurrency itself, and it's important that we not run the db
    // initialization with any of our own locks held, so we're fine.
    AutoPtr<ISQLiteDatabase> db;
    dbhelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    // Watch for external modifications to the database files,
    // keeping our caches in sync.  We synchronize the observer set
    // separately, and of course it has to run after the db file
    // itself was set up by the DatabaseHelper.
    {
        AutoLock lock(sObserverInstancesLock);
        if (sObserverInstances.Find(userHandle) == sObserverInstances.End()) {
            String path;
            db->GetPath(&path);
            AutoPtr<SettingsFileObserver> observer = new SettingsFileObserver(userHandle, path, this);
            sObserverInstances[userHandle] = observer;
            observer->StartWatching();
        }
    }

    Boolean result;
    EnsureAndroidIdIsSet(userHandle, &result);

    StartAsyncCachePopulation(userHandle);
}

void CSettingsProvider::StartAsyncCachePopulation(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<CachePrefetchThread> thread = new CachePrefetchThread(userHandle, this);
    thread->Start();
}

void CSettingsProvider::FullyPopulateCaches(
    /* [in] */ Int32 userHandle)
{
    HashMap<Int32, AutoPtr<DatabaseHelper> >::Iterator it = mOpenHelpers.Find(userHandle);
    AutoPtr<DatabaseHelper> dbHelper;
    if (it != mOpenHelpers.End()) {
        dbHelper = it->mSecond;
    }
    // Only populate the globals cache once, for the owning user
    if (userHandle == IUserHandle::USER_OWNER) {
        FullyPopulateCache(dbHelper, TABLE_GLOBAL, sGlobalCache);
    }
    HashMap<Int32, AutoPtr<SettingsCache> >::Iterator cacheIt = sSecureCaches.Find(userHandle);
    AutoPtr<SettingsCache> secureCache;
    if (cacheIt != sSecureCaches.End()) {
        secureCache = cacheIt->mSecond;
    }
    cacheIt = sSystemCaches.Find(userHandle);
    AutoPtr<SettingsCache> systemCache;;
    if (cacheIt != sSystemCaches.End()) {
        systemCache = cacheIt->mSecond;
    }
    FullyPopulateCache(dbHelper, TABLE_SECURE, secureCache);
    FullyPopulateCache(dbHelper, TABLE_SYSTEM, systemCache);
}

void CSettingsProvider::FullyPopulateCache(
    /* [in] */ DatabaseHelper* dbHelper,
    /* [in] */ const String& table,
    /* [in] */ SettingsCache* cache)
{
    AutoPtr<ISQLiteDatabase> db;
    dbHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = ISettingsNameValueTable::NAME;
    (*args)[1] = ISettingsNameValueTable::VALUE;
    AutoPtr<ICursor> c;
    db->Query(table, args, String(NULL), NULL, String(NULL), String(NULL), String(NULL),
            String("") + StringUtils::Int32ToString(MAX_CACHE_ENTRIES + 1), (ICursor**)&c);
    // try {
    {
        AutoLock lock(cache);
        cache->EvictAll();
        cache->SetFullyMatchesDisk(TRUE);  // optimistic
        Int32 rows = 0;
        Boolean hasNext = FALSE;
        while (c->MoveToNext(&hasNext), hasNext) {
            rows++;
            String name, value;
            c->GetString(0, &name);
            c->GetString(1, &value);
            cache->Populate(name, value);
        }
        if (rows > MAX_CACHE_ENTRIES) {
            // Somewhat redundant, as removeEldestEntry() will
            // have already done this, but to be explicit:
            cache->SetFullyMatchesDisk(FALSE);
            Logger::D(TAG, "row count exceeds max cache entries for table %s", table.string());
        }
        if (LOCAL_LOGV) {
            Logger::D(TAG, "cache for settings table '%s' rows=%d; fullycached=%d", table.string(), rows, cache->FullyMatchesDisk());
        }
    }
    // } finally {
    c->Close();
    // }
}

ECode CSettingsProvider::EnsureAndroidIdIsSet(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> contentUri;
    secure->GetContentUri((IUri**)&contentUri);
    AutoPtr< ArrayOf<String> > select = ArrayOf<String>::Alloc(1);
    (*select)[0] = ISettingsNameValueTable::VALUE;
    AutoPtr< ArrayOf<String> > whereArgs = ArrayOf<String>::Alloc(1);
    (*whereArgs)[0] = ISettingsSecure::ANDROID_ID;
    AutoPtr<ICursor> c;
    FAIL_RETURN(QueryForUser(contentUri, select, ISettingsNameValueTable::NAME + String("=?"),
            whereArgs, String(NULL), userHandle, (ICursor**)&c))
    // try {
    String value;
    ECode ec = NOERROR;
    Boolean hasNext = FALSE;
    if (c->MoveToNext(&hasNext), hasNext) {
        ec = c->GetString(0, &value);
        FAIL_GOTO(ec, failed)
    }
    if (value.IsNull()) {
        Logger::E(TAG, "TODO: SecureRandom has not been realized!!!!");
        // SecureRandom random = new SecureRandom();
        String newAndroidIdValue("93ea6006f51130fe");// String newAndroidIdValue = Long.toHexString(random.nextLong());
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        AutoPtr<ICharSequence> cs1, cs2;
        CStringWrapper::New(ISettingsSecure::ANDROID_ID, (ICharSequence**)&cs1);
        CStringWrapper::New(newAndroidIdValue, (ICharSequence**)&cs2);
        values->PutString(ISettingsNameValueTable::NAME, cs1);
        values->PutString(ISettingsNameValueTable::VALUE, cs2);
        AutoPtr<IUri> uri;
        ec = InsertForUser(contentUri, values, userHandle, (IUri**)&uri);
        FAIL_GOTO(ec, failed)
        if (uri == NULL) {
            Logger::E(TAG, "Unable to generate new ANDROID_ID for user %d", userHandle);
            *result = FALSE;
            c->Close();
            return NOERROR;
        }
        Logger::D(TAG, "Generated and saved new ANDROID_ID [%s] for user %d", newAndroidIdValue.string(), userHandle);
    }
    *result = TRUE;
    // } finally {
    //     c.close();
    // }
failed:
    c->Close();
    return ec;
}

AutoPtr<CSettingsProvider::SettingsCache> CSettingsProvider::GetOrConstructCache(
    /* [in] */ Int32 callingUser,
    /* [in] */ HashMap<Int32, AutoPtr<SettingsCache> >& which)
{
    GetOrEstablishDatabase(callingUser); // ignore return value; we don't need it
    AutoPtr<SettingsCache> cache;
    HashMap<Int32, AutoPtr<SettingsCache> >::Iterator it = which.Find(callingUser);
    if (it != which.End()) {
        cache = it->mSecond;
    }
    return cache;
}

AutoPtr<DatabaseHelper> CSettingsProvider::GetOrEstablishDatabase(
    /* [in] */ Int32 callingUser)
{
    AutoPtr<IBinderHelper> helper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&helper);
    Int64 oldId;
    helper->ClearCallingIdentity(&oldId);
    // try {
    AutoPtr<DatabaseHelper> dbHelper;
    HashMap<Int32, AutoPtr<DatabaseHelper> >::Iterator it = mOpenHelpers.Find(callingUser);
    if (it != mOpenHelpers.End()) {
        dbHelper= it->mSecond;
    }
    if (NULL == dbHelper) {
        EstablishDbTracking(callingUser);
        dbHelper = mOpenHelpers[callingUser];//EstablishDbTracking ensure mOpenHelpers containing key of callingUser
    }
    helper->RestoreCallingIdentity(oldId);
    return dbHelper;
    // } finally {
    //     Binder.restoreCallingIdentity(oldId);
    // }
}

AutoPtr<CSettingsProvider::SettingsCache> CSettingsProvider::CacheForTable(
    /* [in] */ Int32 callingUser,
    /* [in] */ const String& tableName)
{
    if (TABLE_SYSTEM.Equals(tableName)) {
        return GetOrConstructCache(callingUser, sSystemCaches);
    }
    if (TABLE_SECURE.Equals(tableName)) {
        return GetOrConstructCache(callingUser, sSecureCaches);
    }
    if (TABLE_GLOBAL.Equals(tableName)) {
        return sGlobalCache;
    }
    return NULL;
}

void CSettingsProvider::InvalidateCache(
    /* [in] */ Int32 callingUser,
    /* [in] */ const String& tableName)
{
    AutoPtr<SettingsCache> cache = CacheForTable(callingUser, tableName);
    if (cache == NULL) {
        return;
    }
    AutoLock lock(cache);
    cache->EvictAll();
    cache->mCacheFullyMatchesDisk = FALSE;
}

ECode CSettingsProvider::Call(
    /* [in] */ const String& method,
    /* [in] */ const String& request,
    /* [in] */ IBundle* args,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUser;
    helper->GetCallingUserId(&callingUser);
    if (args != NULL) {
        Int32 reqUser = callingUser;
        args->GetInt32(ISettings::CALL_METHOD_USER_KEY, callingUser, &reqUser);
        if (reqUser != callingUser) {
            AutoPtr<IBinderHelper> binderHelper;
            CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
            Int32 pid, uid;
            binderHelper->GetCallingPid(&pid);
            binderHelper->GetCallingUid(&uid);
            AutoPtr<IActivityManagerHelper> amHelper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
            amHelper->HandleIncomingUser(pid, uid, reqUser, FALSE, TRUE,
                    String("get/set setting for user"), String(NULL), &callingUser);
            if (LOCAL_LOGV) Logger::V(TAG, "   access setting for user %d", callingUser);
        }
    }

    // Note: we assume that get/put operations for moved-to-global names have already
    // been directed to the new location on the caller side (otherwise we'd fix them
    // up here).
    AutoPtr<DatabaseHelper> dbHelper;
    AutoPtr<SettingsCache> cache;

    // Get methods
    if (ISettings::CALL_METHOD_GET_SYSTEM.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call(system:%s) for %d", request.string(), callingUser);
        dbHelper = GetOrEstablishDatabase(callingUser);
        cache = sSystemCaches[callingUser];// GetOrEstablishDatabase ensure sSystemCaches containing the key of callingUser
        AutoPtr<IBundle> b = LookupValue(dbHelper, TABLE_SYSTEM, cache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }
    if (ISettings::CALL_METHOD_GET_SECURE.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call(secure:%s) for %d", request.string(), callingUser);
        dbHelper = GetOrEstablishDatabase(callingUser);
        cache = sSecureCaches[callingUser];
        AutoPtr<IBundle> b = LookupValue(dbHelper, TABLE_SECURE, cache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }
    if (ISettings::CALL_METHOD_GET_GLOBAL.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call(global:%s) for %d", request.string(), callingUser);
        // fast path: owner db & cache are immutable after onCreate() so we need not
        // guard on the attempt to look them up
        AutoPtr<DatabaseHelper> dbHelper1 = GetOrEstablishDatabase(IUserHandle::USER_OWNER);
        AutoPtr<IBundle> b = LookupValue(dbHelper1, TABLE_GLOBAL, sGlobalCache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }

    // Put methods - new value is in the args bundle under the key named by
    // the Settings.NameValueTable.VALUE static.
    String newValue(NULL);
    if (args) {
        args->GetString(ISettingsNameValueTable::VALUE, &newValue);
    }

    ECode ec;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<ICharSequence> cs1, cs2;
    CStringWrapper::New(request, (ICharSequence**)&cs1);
    CStringWrapper::New(newValue, (ICharSequence**)&cs2);
    values->PutString(ISettingsNameValueTable::NAME, cs1);
    values->PutString(ISettingsNameValueTable::VALUE, cs2);
    if (ISettings::CALL_METHOD_PUT_SYSTEM.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call_put(system:%s=%s) for %d", request.string(), newValue.string(), callingUser);
        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        AutoPtr<IUri> uri;
        settingsSystem->GetContentUri((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);
    }
    else if (ISettings::CALL_METHOD_PUT_SECURE.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call_put(secure:%s=%s for %d", request.string(), newValue.string(), callingUser);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        AutoPtr<IUri> uri;
        settingsSecure->GetContentUri((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);
    }
    else if (ISettings::CALL_METHOD_PUT_GLOBAL.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call_put(global:%s=%s for %d", request.string(), newValue.string(), callingUser);
        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        AutoPtr<IUri> uri;
        settingsGlobal->GetContentUri((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);
    }
    else {
        Logger::W(TAG, "call() with invalid method: %s", method.string());
    }

    *bundle = NULL;
    return ec;
}

AutoPtr<IBundle> CSettingsProvider::LookupValue(
    /* [in] */ DatabaseHelper* dbHelper,
    /* [in] */ const String& table,
    /* [in] */ SettingsCache* cache,
    /* [in] */ const String& key)
{
    if (cache == NULL || key.IsNull()) {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 userId;
        helper->GetCallingUserId(&userId);
        Logger::E(TAG, "cache is null for user %d : key=%s", userId, key.string());
        return NULL;
    }
    {
        AutoLock lock(cache);
        AutoPtr<IBundle> value = cache->Get(key);
        if (value != NULL) {
            if (value != TOO_LARGE_TO_CACHE_MARKER) {
                return value;
            }
            // else we fall through and read the value from disk
        }
        else if (cache->FullyMatchesDisk()) {
            // Fast path (very common).  Don't even try touch disk
            // if we know we've slurped it all in.  Trying to
            // touch the disk would mean waiting for yaffs2 to
            // give us access, which could takes hundreds of
            // milliseconds.  And we're very likely being called
            // from somebody's UI thread...
            return NULL_SETTING;
        }
    }

    AutoPtr<ISQLiteDatabase> db;
    ASSERT_SUCCEEDED(dbHelper->GetReadableDatabase((ISQLiteDatabase**)&db))
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = key;
    ECode ec = db->Query(table, COLUMN_VALUE, String("name=?"), args,
            String(NULL), String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
    if (FAILED(ec)) {
        Logger::W(TAG, "settings lookup error");
        if (cursor != NULL) cursor->Close();
        return NULL;
    }
    Int32 count;
    if (cursor != NULL && cursor->GetCount(&count), count == 1) {
        Boolean result;
        cursor->MoveToFirst(&result);
        String value;
        ec = cursor->GetString(0, &value);
        if (FAILED(ec)) Logger::W(TAG, "settings lookup error");
        if (cursor != NULL) cursor->Close();
        return cache->PutIfAbsent(key, value);
    }
    // } catch (SQLiteException e) {
    //     Log.w(TAG, "settings lookup error", e);
    //     return null;
    // } finally {
    //     if (cursor != null) cursor.close();
    // }
    cache->PutIfAbsent(key, String(NULL));
    if (cursor != NULL) cursor->Close();
    return NULL_SETTING;
}

ECode CSettingsProvider::Query(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* select,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId;
    helper->GetCallingUserId(&userId);
    return QueryForUser(url, select, where, whereArgs, sort, userId, cursor);
}

ECode CSettingsProvider::QueryForUser(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* select,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [in] */ const String& sort,
    /* [in] */ Int32 forUser,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    if (LOCAL_LOGV) {
        String str;
        url->ToString(&str);
        Logger::V(TAG, "query(%s) for user %d", str.string(), forUser);
    }
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, where, whereArgs, (SqlArguments**)&args))
    AutoPtr<DatabaseHelper> dbH = GetOrEstablishDatabase(
            TABLE_GLOBAL.Equals(args->mTable) ? IUserHandle::USER_OWNER : forUser);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetReadableDatabase((ISQLiteDatabase**)&db))

    // The favorites table was moved from this provider to a provider inside Home
    // Home still need to query this table to upgrade from pre-cupcake builds
    // However, a cupcake+ build with no data does not contain this table which will
    // cause an exception in the SQL stack. The following line is a special case to
    // let the caller of the query have a chance to recover and avoid the exception
    if (TABLE_FAVORITES.Equals(args->mTable)) {
        *cursor = NULL;
        return NOERROR;
    }
    else if (TABLE_OLD_FAVORITES.Equals(args->mTable)) {
        args->mTable = TABLE_FAVORITES;
        AutoPtr<ICursor> c;
        db->RawQuery(String("PRAGMA table_info(favorites);"), NULL, (ICursor**)&c);
        if (c != NULL) {
            Int32 count;
            c->GetCount(&count);
            Boolean exists = count > 0;
            c->Close();
            if (!exists) {
                *cursor = NULL;
                return NOERROR;
            }
        }
        else {
            *cursor = NULL;
            return NOERROR;
        }
    }

    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    qb->SetTables(args->mTable);

    AutoPtr<ICursor> ret;
    FAIL_RETURN(qb->Query(db, select, args->mWhere, args->mArgs, String(NULL), String(NULL), sort, (ICursor**)&ret));
    // the default Cursor interface does not support per-user observation
    // try {
    AutoPtr<IAbstractCursor> ac = IAbstractCursor::Probe(ret);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    ECode ec = ac->SetNotificationUri(resolver, url, forUser);
    if (FAILED(ec)) {
        // details of the concrete Cursor implementation have changed and this code has
        // not been updated to match -- complain and fail hard.
        Logger::E(TAG, "Incompatible cursor derivation!");
        return ec;
    }
    // } catch (ClassCastException e) {
    //     // details of the concrete Cursor implementation have changed and this code has
    //     // not been updated to match -- complain and fail hard.
    //     Log.wtf(TAG, "Incompatible cursor derivation!");
    //     throw e;
    // }
    *cursor = ret;
    REFCOUNT_ADD(*cursor)
    return NOERROR;
}

ECode CSettingsProvider::GetType(
    /* [in] */ IUri* url,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    // If SqlArguments supplies a where clause, then it must be an item
    // (because we aren't supplying our own where clause).
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, String(NULL), NULL, (SqlArguments**)&args))
    if (TextUtils::IsEmpty(args->mWhere)) {
        *type = String("vnd.android.cursor.dir/") + args->mTable;
    }
    else {
        *type = String("vnd.android.cursor.item/") + args->mTable;
    }
    return NOERROR;
}

ECode CSettingsProvider::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* values,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUser;
    helper->GetCallingUserId(&callingUser);
    if (LOCAL_LOGV) Logger::V(TAG, "bulkInsert() for user %d", callingUser);
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(uri, (SqlArguments**)&args))
    if (TABLE_FAVORITES.Equals(args->mTable)) {
        *number = 0;
        return NOERROR;
    }
    FAIL_RETURN(CheckWritePermissions(args))
    AutoPtr<SettingsCache> cache = CacheForTable(callingUser, args->mTable);

    HashMap<Int32, AutoPtr<IAtomicInteger32> >::Iterator it = sKnownMutationsInFlight.Find(callingUser);
    AutoPtr<IAtomicInteger32> mutationCount;
    if (it != sKnownMutationsInFlight.End()) {
        mutationCount = it->mSecond;
    }
    Int32 outV;
    mutationCount->IncrementAndGet(&outV);
    AutoPtr<DatabaseHelper> dbH = GetOrEstablishDatabase(
            TABLE_GLOBAL.Equals(args->mTable) ? IUserHandle::USER_OWNER : callingUser);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    db->BeginTransaction();
    // try {
    Int32 numValues = values->GetLength();
    for (Int32 i = 0; i < numValues; i++) {
        Int64 v;
        if (db->Insert(args->mTable, String(NULL), (*values)[i], &v), v < 0) {
            *number = 0;
            db->EndTransaction();
            mutationCount->DecrementAndGet(&outV);
            return NOERROR;
        }
        SettingsCache::Populate(cache, (*values)[i]);
        if (LOCAL_LOGV) {
            String str;
            (*values)[i]->ToString(&str);
            Logger::V(TAG, "%s <- %s", args->mTable.string(), str.string());
        }
    }
    db->SetTransactionSuccessful();
    // } finally {
    //     db.endTransaction();
    //     mutationCount.decrementAndGet();
    // }

    SendNotify(uri, callingUser);
    *number = values->GetLength();
    return NOERROR;
}

Boolean CSettingsProvider::ParseProviderList(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues)
{
    String value;
    initialValues->GetAsString(ISettingsSecure::VALUE, &value);
    String newProviders;
    if (!value.IsNull() && value.GetLength() > 1) {
        Char32 prefix = value.GetChar(0);
        if (prefix == '+' || prefix == '-') {
            // skip prefix
            value = value.Substring(1);

            // read list of enabled providers into "providers"
            String providers("");
            AutoPtr< ArrayOf<String> > columns = ArrayOf<String>::Alloc(1);
            (*columns)[0] = ISettingsSecure::VALUE;
            StringBuilder builder(ISettingsSecure::NAME);
            builder += "=\'";
            builder += ISettingsSecure::LOCATION_PROVIDERS_ALLOWED;
            builder += "\'";
            String where = builder.ToString();
            AutoPtr<ICursor> cursor;
            Query(url, columns, where, NULL, String(NULL), (ICursor**)&cursor);
            Int32 count;
            if (cursor != NULL && cursor->GetCount(&count), count == 1) {
                // try {
                Boolean result;
                cursor->MoveToFirst(&result);
                cursor->GetString(0, &providers);
                // } finally {
                cursor->Close();
                // }
            }

            Int32 index = providers.IndexOf(value);
            Int32 end = index + value.GetLength();
            // check for commas to avoid matching on partial string
            if (index > 0 && providers.GetChar(index - 1) != ',') index = -1;
            if (end < (Int32)providers.GetLength() && providers.GetChar(end) != ',') index = -1;

            if (prefix == '+' && index < 0) {
                // append the provider to the list if not present
                if (providers.GetLength() == 0) {
                    newProviders = value;
                }
                else {
                    StringBuilder buf(providers);
                    buf += ',';
                    buf += value;
                    newProviders = buf.ToString();
                }
            }
            else if (prefix == '-' && index >= 0) {
                // remove the provider from the list if present
                // remove leading or trailing comma
                if (index > 0) {
                    index--;
                }
                else if (end < (Int32)providers.GetLength()) {
                    end++;
                }

                newProviders = providers.Substring(0, index);
                if (end < (Int32)providers.GetLength()) {
                    newProviders += providers.Substring(end);
                }
            }
            else {
                // nothing changed, so no need to update the database
                return FALSE;
            }

            if (!newProviders.IsNull()) {
                AutoPtr<ICharSequence> cs;
                CStringWrapper::New(newProviders, (ICharSequence**)&cs);
                initialValues->PutString(ISettingsSecure::VALUE, cs);
            }
        }
    }

    return TRUE;
}

ECode CSettingsProvider::Insert(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri)
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId;
    helper->GetCallingUserId(&userId);
    return InsertForUser(url, initialValues, userId, insertedUri);
}

ECode CSettingsProvider::InsertForUser(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ Int32 desiredUserHandle,
    /* [out] */ IUri** outUri)
{
    VALIDATE_NOT_NULL(outUri)
    *outUri = NULL;

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUser;
    helper->GetCallingUserId(&callingUser);
    if (callingUser != desiredUserHandle) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        FAIL_RETURN(context->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
                String("Not permitted to access settings for other users")))
    }

    if (LOCAL_LOGV) {
        String str;
        url->ToString(&str);
        Logger::V(TAG, "insert(%s) for user %d by %d", str.string(), desiredUserHandle, callingUser);
    }

    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, (SqlArguments**)&args))
    if (TABLE_FAVORITES.Equals(args->mTable)) {
        *outUri = NULL;
        return NOERROR;
    }

    // Special case LOCATION_PROVIDERS_ALLOWED.
    // Support enabling/disabling a single provider (using "+" or "-" prefix)
    String name;
    initialValues->GetAsString(ISettingsSecure::NAME, &name);
    if (ISettingsSecure::LOCATION_PROVIDERS_ALLOWED.Equals(name)) {
        if (!ParseProviderList(url, initialValues)) {
            *outUri = NULL;
            return NOERROR;
        }
    }

    // If this is an insert() of a key that has been migrated to the global store,
    // redirect the operation to that store
    if (!name.IsNull()) {
        if (sSecureGlobalKeys.Find(name) != sSecureGlobalKeys.End()
                || sSystemGlobalKeys.Find(name) != sSystemGlobalKeys.End()) {
            if (!TABLE_GLOBAL.Equals(args->mTable)) {
                if (LOCAL_LOGV) Logger::I(TAG, "Rewrite of insert() of now-global key %s", name.string());
            }
            args->mTable = TABLE_GLOBAL;  // next condition will rewrite the user handle
        }
    }

    // Check write permissions only after determining which table the insert will touch
    FAIL_RETURN(CheckWritePermissions(args));

    // The global table is stored under the owner, always
    if (TABLE_GLOBAL.Equals(args->mTable)) {
        desiredUserHandle = IUserHandle::USER_OWNER;
    }

    AutoPtr<SettingsCache> cache = CacheForTable(desiredUserHandle, args->mTable);
    String value;
    initialValues->GetAsString(ISettingsNameValueTable::VALUE, &value);
    if (SettingsCache::IsRedundantSetValue(cache, name, value)) {
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        return uriHelper->WithAppendedPath(url, name, outUri);
    }

    AutoPtr<IAtomicInteger32> mutationCount;
    HashMap<Int32, AutoPtr<IAtomicInteger32> >::Iterator it = sKnownMutationsInFlight.Find(desiredUserHandle);
    if (it != sKnownMutationsInFlight.End()) {
        mutationCount = it->mSecond;
    }
    Int32 outV;
    mutationCount->IncrementAndGet(&outV);
    AutoPtr<DatabaseHelper> dbH = GetOrEstablishDatabase(desiredUserHandle);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int64 rowId;
    db->Insert(args->mTable, String(NULL), initialValues, &rowId);
    mutationCount->DecrementAndGet(&outV);
    if (rowId <= 0) {
        *outUri = NULL;
        return NOERROR;
    }

    SettingsCache::Populate(cache, initialValues);  // before we notify

    if (LOCAL_LOGV) {
        String str;
        initialValues->ToString(&str);
        Logger::V(TAG,  + "%s <- %s for user %d", args->mTable.string(), str.string(), desiredUserHandle);
    }
    // Note that we use the original url here, not the potentially-rewritten table name
    AutoPtr<IUri> temp;
    FAIL_RETURN(GetUriFor(url, initialValues, rowId, (IUri**)&temp))
    SendNotify(temp, desiredUserHandle);
    *outUri = url;
    REFCOUNT_ADD(*outUri)
    return NOERROR;
}

ECode CSettingsProvider::Delete(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected)

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUser;
    helper->GetCallingUserId(&callingUser);
    if (LOCAL_LOGV) Logger::V(TAG, "delete() for user %d", callingUser);
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, where, whereArgs, (SqlArguments**)&args))
    if (TABLE_FAVORITES.Equals(args->mTable)) {
        *rowsAffected = 0;
        return NOERROR;
    }
    else if (TABLE_OLD_FAVORITES.Equals(args->mTable)) {
        args->mTable = TABLE_FAVORITES;
    }
    else if (TABLE_GLOBAL.Equals(args->mTable)) {
        callingUser = IUserHandle::USER_OWNER;
    }
    FAIL_RETURN(CheckWritePermissions(args))

    HashMap<Int32, AutoPtr<IAtomicInteger32> >::Iterator it = sKnownMutationsInFlight.Find(callingUser);
    AutoPtr<IAtomicInteger32> mutationCount;
    if (it != sKnownMutationsInFlight.End()) {
        mutationCount = it->mSecond;
    }
    Int32 outV;
    mutationCount->IncrementAndGet(&outV);
    AutoPtr<DatabaseHelper> dbH = GetOrEstablishDatabase(callingUser);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int32 count;
    FAIL_RETURN(db->Delete(args->mTable, args->mWhere, args->mArgs, &count))
    mutationCount->DecrementAndGet(&outV);
    if (count > 0) {
        InvalidateCache(callingUser, args->mTable);  // before we notify
        SendNotify(url, callingUser);
    }
    StartAsyncCachePopulation(callingUser);
    if (LOCAL_LOGV) Logger::V(TAG,  "%s: %d row(s) deleted", args->mTable.string(), count);
    *rowsAffected = count;
    return NOERROR;
}

ECode CSettingsProvider::Update(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected)

    // NOTE: update() is never called by the front-end Settings API, and updates that
    // wind up affecting rows in Secure that are globally shared will not have the
    // intended effect (the update will be invisible to the rest of the system).
    // This should have no practical effect, since writes to the Secure db can only
    // be done by system code, and that code should be using the correct API up front.
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUser;
    helper->GetCallingUserId(&callingUser);
    if (LOCAL_LOGV) Logger::V(TAG, "update() for user %d", callingUser);
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, where, whereArgs, (SqlArguments**)&args))
    if (TABLE_FAVORITES.Equals(args->mTable)) {
        *rowsAffected = 0;
        return NOERROR;
    }
    else if (TABLE_GLOBAL.Equals(args->mTable)) {
        callingUser = IUserHandle::USER_OWNER;
    }
    FAIL_RETURN(CheckWritePermissions(args))

    HashMap<Int32, AutoPtr<IAtomicInteger32> >::Iterator it = sKnownMutationsInFlight.Find(callingUser);
    AutoPtr<IAtomicInteger32> mutationCount;
    if (it != sKnownMutationsInFlight.End()) {
        mutationCount = it->mSecond;
    }
    Int32 outV;
    mutationCount->IncrementAndGet(&outV);
    AutoPtr<DatabaseHelper> dbH = GetOrEstablishDatabase(callingUser);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int32 count;
    FAIL_RETURN(db->Update(args->mTable, initialValues, args->mWhere, args->mArgs, &count))
    mutationCount->DecrementAndGet(&outV);
    if (count > 0) {
        InvalidateCache(callingUser, args->mTable);  // before we notify
        SendNotify(url, callingUser);
    }
    StartAsyncCachePopulation(callingUser);
    if (LOCAL_LOGV) Logger::V(TAG,  "%s: %d row(s) deleted", args->mTable.string(), count);
    *rowsAffected = count;
    return NOERROR;
}

ECode CSettingsProvider::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)
    *fileDescriptor = NULL;

    /*
     * When a client attempts to openFile the default ringtone or
     * notification setting Uri, we will proxy the call to the current
     * default ringtone's Uri (if it is in the DRM or media provider).
     */
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    Int32 ringtoneType;
    helper->GetDefaultType(uri, &ringtoneType);
    // Above call returns -1 if the Uri doesn't match a default type
    if (ringtoneType != -1) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);

        // Get the current value for the default sound
        AutoPtr<IUri> soundUri;
        helper->GetActualDefaultRingtoneUri(context, ringtoneType, (IUri**)&soundUri);

        if (soundUri != NULL) {
            // Only proxy the openFile call to drm or media providers
            String authority;
            soundUri->GetAuthority(&authority);
            Boolean isDrmAuthority = authority.Equals("drm"/*IDrmStore::AUTHORITY*/);
            if (isDrmAuthority || authority.Equals(IMediaStore::AUTHORITY)) {
                if (isDrmAuthority) {
                    // try {
                    // Check DRM access permission here, since once we
                    // do the below call the DRM will be checking our
                    // permission, not our caller's permission
                    assert(0 && "TODO");
                    // AutoPtr<IDrmStore> drmStore;
                    // CDrmStore::AcquireSingleton((IDrmStore**)&drmStore);
                    // drmStore->EnforceAccessDrmPermission(context);
                    // } catch (SecurityException e) {
                    //     throw new FileNotFoundException(e.getMessage());
                    // }
                }

                AutoPtr<IContentResolver> resolver;
                return resolver->OpenFileDescriptor(soundUri, mode, fileDescriptor);
            }
        }
    }

    return ContentProvider::OpenFile(uri, mode, fileDescriptor);
}

ECode CSettingsProvider::OpenAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    /*
     * When a client attempts to openFile the default ringtone or
     * notification setting Uri, we will proxy the call to the current
     * default ringtone's Uri (if it is in the DRM or media provider).
     */
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    Int32 ringtoneType;
    helper->GetDefaultType(uri, &ringtoneType);
    // Above call returns -1 if the Uri doesn't match a default type
    if (ringtoneType != -1) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);

        // Get the current value for the default sound
        AutoPtr<IUri> soundUri;
        helper->GetActualDefaultRingtoneUri(context, ringtoneType, (IUri**)&soundUri);

        if (soundUri != NULL) {
            // Only proxy the openFile call to drm or media providers
            String authority;
            soundUri->GetAuthority(&authority);
            Boolean isDrmAuthority = authority.Equals("drm"/*DrmStore.AUTHORITY*/);
            if (isDrmAuthority || authority.Equals(IMediaStore::AUTHORITY)) {
                if (isDrmAuthority) {
                    // try {
                    // Check DRM access permission here, since once we
                    // do the below call the DRM will be checking our
                    // permission, not our caller's permission
                    Logger::E(TAG, "TODO: DrmStore has not been realized!!!");
                    assert(0);
                    // DrmStore.enforceAccessDrmPermission(context);
                    // } catch (SecurityException e) {
                    //     throw new FileNotFoundException(e.getMessage());
                    // }
                }

                AutoPtr<IParcelFileDescriptor> pfd;
                // try {
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);
                if (SUCCEEDED(resolver->OpenFileDescriptor(soundUri, mode, (IParcelFileDescriptor**)&pfd))) {
                    return CAssetFileDescriptor::New(pfd, 0, -1, fileDescriptor);
                }
                // } catch (FileNotFoundException ex) {
                //     // fall through and open the fallback ringtone below
                // }
            }

            // try {
            AutoPtr<IAssetFileDescriptor> temp;
            if (FAILED(ContentProvider::OpenAssetFile(soundUri, mode, (IAssetFileDescriptor**)&temp))) {
                AutoPtr<IResources> res;
                context->GetResources((IResources**)&res);
                return res->OpenRawResourceFd(Elastos::Droid::R::raw::fallbackring, fileDescriptor);
            }
            *fileDescriptor = temp;
            REFCOUNT_ADD(*fileDescriptor)
            return NOERROR;
            // } catch (FileNotFoundException ex) {
            //     // Since a non-null Uri was specified, but couldn't be opened,
            //     // fall back to the built-in ringtone.
            //     return context.getResources().openRawResourceFd(
            //             com.android.internal.R.raw.fallbackring);
            // }
        }
        // no need to fall through and have openFile() try again, since we
        // already know that will fail.
        return E_FILE_NOT_FOUND_EXCEPTION;
        // throw new FileNotFoundException(); // or return null ?
    }

    // Note that this will end up calling openFile() above.
    return ContentProvider::OpenAssetFile(uri, mode, fileDescriptor);
}

} // namespace SettingsProvider
} // namespace Packages
} // namespace Droid
} // namespace Elastos
