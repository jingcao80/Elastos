
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/settingsprovider/SettingsProvider.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/Manifest.h"
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IAbstractCursor;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Manifest;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsNameValueTable;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ISet;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

const String SettingsProvider::TAG("SettingsProvider");
const Boolean SettingsProvider::LOCAL_LOGV = FALSE;
const Boolean SettingsProvider::USER_CHECK_THROWS = TRUE;
const String SettingsProvider::TABLE_SYSTEM("system");
const String SettingsProvider::TABLE_SECURE("secure");
const String SettingsProvider::TABLE_GLOBAL("global");
const String SettingsProvider::TABLE_FAVORITES("favorites");
const String SettingsProvider::TABLE_OLD_FAVORITES("old_favorites");

static AutoPtr< ArrayOf<String> > InitColumnValue()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = "value";
    return args;
}
const AutoPtr< ArrayOf<String> > SettingsProvider::COLUMN_VALUE = InitColumnValue();
const Int32 SettingsProvider::MAX_CACHE_ENTRIES;

static AutoPtr<ISparseArray> InitArg()
{
    AutoPtr<ISparseArray> arr;
    CSparseArray::New((ISparseArray**)&arr);
    return arr;
}

static AutoPtr<SettingsProvider::SettingsCache> InitGlabalCache()
{
    AutoPtr<SettingsProvider::SettingsCache> cache = new SettingsProvider::SettingsCache();
    cache->constructor(String("global")/*TABLE_GLOBAL*/);
    return cache;
}

AutoPtr<ISparseArray> SettingsProvider::sSystemCaches = InitArg();
AutoPtr<ISparseArray> SettingsProvider::sSecureCaches = InitArg();
const AutoPtr<SettingsProvider::SettingsCache> SettingsProvider::sGlobalCache = InitGlabalCache();
AutoPtr<ISparseArray> SettingsProvider::sKnownMutationsInFlight = InitArg();
const Int32 SettingsProvider::MAX_CACHE_ENTRY_SIZE;

AutoPtr<IBundle> InitBundle(
    /* [in] */ const String& str)
{
    AutoPtr<IBundleHelper> helper;
    CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
    AutoPtr<IBundle> b;
    helper->ForPair(str, String(NULL), (IBundle**)&b);
    return b;
}
const AutoPtr<IBundle> SettingsProvider::NULL_SETTING = InitBundle(String("value"));
const AutoPtr<IBundle> SettingsProvider::TOO_LARGE_TO_CACHE_MARKER = InitBundle(String("_dummy"));

AutoPtr<IHashSet> SettingsProvider::sSecureGlobalKeys;
AutoPtr<IHashSet> SettingsProvider::sSystemGlobalKeys;

AutoPtr<IMap> SettingsProvider::sRestrictedKeys;

AutoPtr<IHashSet> SettingsProvider::sSecureCloneToManagedKeys;
AutoPtr<IHashSet> SettingsProvider::sSystemCloneToManagedKeys;

const String SettingsProvider::DROPBOX_TAG_USERLOG("restricted_profile_ssaid");

Boolean SettingsProvider::InitGlobalKeys()
{
    // Keys (name column) from the 'secure' table that are now in the owner user's 'global'
    // table, shared across all users
    // These must match Settings.Secure.MOVED_TO_GLOBAL
    CHashSet::New((IHashSet**)&sSecureGlobalKeys);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->GetMovedKeys(sSecureGlobalKeys);

    // Keys from the 'system' table now moved to 'global'
    // These must match Settings.System.MOVED_TO_GLOBAL
    CHashSet::New((IHashSet**)&sSystemGlobalKeys);
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    sys->GetNonLegacyMovedKeys(sSystemGlobalKeys);

    CHashMap::New((IMap**)&sRestrictedKeys);
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsSecure::LOCATION_MODE),
            CoreUtils::Convert(IUserManager::DISALLOW_SHARE_LOCATION));
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsSecure::LOCATION_PROVIDERS_ALLOWED),
            CoreUtils::Convert(IUserManager::DISALLOW_SHARE_LOCATION));
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsSecure::INSTALL_NON_MARKET_APPS),
            CoreUtils::Convert(IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES));
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsGlobal::ADB_ENABLED),
            CoreUtils::Convert(IUserManager::DISALLOW_DEBUGGING_FEATURES));
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsGlobal::PACKAGE_VERIFIER_ENABLE),
            CoreUtils::Convert(IUserManager::ENSURE_VERIFY_APPS));
    sRestrictedKeys->Put(CoreUtils::Convert(ISettingsGlobal::PREFERRED_NETWORK_MODE),
            CoreUtils::Convert(IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS));

    CHashSet::New((IHashSet**)&sSecureCloneToManagedKeys);
    AutoPtr< ArrayOf<String> > args1;
    secure->GetCLONE_TO_MANAGED_PROFILE((ArrayOf<String>**)&args1);
    for (Int32 i = 0; i < args1->GetLength(); i++) {
        sSecureCloneToManagedKeys->Add(CoreUtils::Convert((*args1)[i]));
    }

    CHashSet::New((IHashSet**)&sSystemCloneToManagedKeys);
    AutoPtr< ArrayOf<String> > args2;
    sys->GetCLONE_TO_MANAGED_PROFILE((ArrayOf<String>**)&args2);

    for (Int32 i = 0; i < args2->GetLength(); i++) {
        sSystemCloneToManagedKeys->Add(CoreUtils::Convert((*args2)[i]));
    }

    return TRUE;
}

const Boolean SettingsProvider::HASINITGLOBALKEYS = SettingsProvider::InitGlobalKeys();

AutoPtr<ISparseArray> SettingsProvider::sObserverInstances = InitArg();

//===============================================================================
//                  SettingsProvider::SqlArguments
//===============================================================================

ECode SettingsProvider::SqlArguments::CreateSqlArguments(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* args,
    /* [out] */ SqlArguments** sqlArguments)
{
    AutoPtr<SqlArguments> arguments = new SqlArguments();
    ECode ec = arguments->constructor(url, where, args);
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

ECode SettingsProvider::SqlArguments::CreateSqlArguments(
    /* [in] */ IUri* url,
    /* [out] */ SqlArguments** sqlArguments)
{
    AutoPtr<SqlArguments> arguments = new SqlArguments();
    ECode ec = arguments->constructor(url);
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

ECode SettingsProvider::SqlArguments::constructor(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IList> pathSegments;
    url->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    if (pathSegments && (pathSegments->GetSize(&size), size) == 1) {
        // of the form content://settings/secure, arbitrary where clause
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&mTable);
        if (!DatabaseHelper::IsValidTable(mTable)) {
            // throw new IllegalArgumentException("Bad root path: " + mTable);
            Logger::E(TAG, "Bad root path: %s", mTable.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mWhere = where;
        mArgs = args;
    }
    else if (pathSegments && (pathSegments->GetSize(&size), size) != 2) {
        // throw new IllegalArgumentException("Invalid URI: " + url);
        String str;
        IObject::Probe(url)->ToString(&str);
        Logger::E(TAG, "Invalid URI: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (!TextUtils::IsEmpty(where)) {
        // throw new UnsupportedOperationException("WHERE clause not supported: " + url);
        String str;
        IObject::Probe(url)->ToString(&str);
        Logger::E(TAG, "WHERE clause not supported: %s", str.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    else {
        // of the form content://settings/secure/element_name, no where clause
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&mTable);

        if (!DatabaseHelper::IsValidTable(mTable)) {
            // throw new IllegalArgumentException("Bad root path: " + mTable);
            Logger::E(TAG, "Bad root path: %s", mTable.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (TABLE_SYSTEM.Equals(mTable) || TABLE_SECURE.Equals(mTable) ||
                TABLE_GLOBAL.Equals(mTable)) {
            mWhere = ISettingsNameValueTable::NAME + String("=?");

            obj = NULL;
            pathSegments->Get(1, (IInterface**)&obj);
            String name;
            ICharSequence::Probe(obj)->ToString(&name);

            mArgs = ArrayOf<String>::Alloc(1);
            (*mArgs)[0] = name;
            // Rewrite the table for known-migrated names
            if (TABLE_SYSTEM.Equals(mTable) || TABLE_SECURE.Equals(mTable)) {
                Boolean res1, res2;
                if ((sSecureGlobalKeys->Contains(CoreUtils::Convert(name), &res1), res1)
                        || (sSystemGlobalKeys->Contains(CoreUtils::Convert(name), &res2), res2)) {
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
            mWhere = String("_id=") + StringUtils::ToString(id);
            mArgs = NULL;
        }
    }
    return NOERROR;
}

ECode SettingsProvider::SqlArguments::constructor(
    /* [in] */ IUri* url)
{
    AutoPtr<IList> pathSegments;
    url->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    if (pathSegments && (pathSegments->GetSize(&size), size) == 1) {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&mTable);
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
        Logger::E(TAG, "Invalid URI: %s", TO_CSTR(url));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

//===============================================================================
//                  SettingsProvider::SettingsFileObserver
//===============================================================================

SettingsProvider::SettingsFileObserver::SettingsFileObserver(
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& path,
    /* [in] */ SettingsProvider* host)
    : FileObserver(path, IFileObserver::CLOSE_WRITE | IFileObserver::CREATE | IFileObserver::DELETE | IFileObserver::MOVED_TO | IFileObserver::MODIFY)
    , mUserHandle(userHandle)
    , mPath(path)
    , mHost(host)
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mIsDirty);
}

ECode SettingsProvider::SettingsFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    AutoPtr<IAtomicInteger32> mutationCount;
    {    AutoLock syncLock(mHost);
        AutoPtr<IInterface> obj;
        mHost->sKnownMutationsInFlight->Get(mUserHandle, (IInterface**)&obj);
        mutationCount = IAtomicInteger32::Probe(obj);
    }
    Int32 value;
    if (mutationCount != NULL && (mutationCount->Get(&value), value > 0)) {
        // our own modification.
        return NOERROR;
    }

    Logger::D(TAG, "User %d external modification to %s; event=%d",
            mUserHandle, mPath.string(), event);
    Boolean result;
    if (mIsDirty->CompareAndSet(FALSE, TRUE, &result), !result) {
        // already handled. (we get a few update events
        // during an sqlite write)
        return NOERROR;
    }
    Logger::D(TAG, "User %d updating our caches for %s",
            mUserHandle, mPath.string());
    mHost->FullyPopulateCaches(mUserHandle);
    mIsDirty->Set(FALSE);
    return NOERROR;
}

//===============================================================================
//                  SettingsProvider::CachePrefetchThread
//===============================================================================

SettingsProvider::CachePrefetchThread::CachePrefetchThread(
    /* [in] */ Int32 userHandle,
    /* [in] */ SettingsProvider* host)
    : mUserHandle(userHandle)
    , mHost(host)
{
    Thread::constructor(String("populate-settings-caches"));
}

ECode SettingsProvider::CachePrefetchThread::Run()
{
    mHost->FullyPopulateCaches(mUserHandle);
    return NOERROR;
}

//===============================================================================
//                  SettingsProvider::SettingsCache
//===============================================================================

SettingsProvider::SettingsCache::SettingsCache()
    : mCacheFullyMatchesDisk(FALSE)
{}

ECode SettingsProvider::SettingsCache::constructor(
    /* [in] */ const String& name)
{
    mCacheName = name;
    return LruCache::constructor(MAX_CACHE_ENTRIES);
}

Boolean SettingsProvider::SettingsCache::FullyMatchesDisk()
{
    AutoLock syncLock(this);
    return mCacheFullyMatchesDisk;
}

void SettingsProvider::SettingsCache::SetFullyMatchesDisk(
    /* [in] */ Boolean value)
{
    AutoLock syncLock(this);
    mCacheFullyMatchesDisk = value;
}

ECode SettingsProvider::SettingsCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue)
{
    if (evicted) {
        mCacheFullyMatchesDisk = FALSE;
    }
    return NOERROR;
}

AutoPtr<IBundle> SettingsProvider::SettingsCache::PutIfAbsent(
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
        AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
        AutoPtr<IInterface> valueObj;
        Get(keyObj, (IInterface**)&valueObj);
        if (valueObj == NULL) {
            Put(keyObj, bundle, NULL);
        }
    }
    return bundle;
}

void SettingsProvider::SettingsCache::Populate(
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

void SettingsProvider::SettingsCache::Populate(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    AutoLock syncLock(this);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(name);
    if (value.IsNull() || (Int32)value.GetLength() <= MAX_CACHE_ENTRY_SIZE) {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        AutoPtr<IBundle> bundle;
        helper->ForPair(ISettingsNameValueTable::VALUE, value, (IBundle**)&bundle);
        Put(keyObj, bundle.Get(), NULL);
    }
    else {
        Put(keyObj, TOO_LARGE_TO_CACHE_MARKER.Get(), NULL);
    }
}

Boolean SettingsProvider::SettingsCache::IsRedundantSetValue(
    /* [in] */ SettingsCache* cache,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (cache == NULL) return FALSE;

    {
        AutoLock syncLock(cache);
        AutoPtr<ICharSequence> key = CoreUtils::Convert(name);
        AutoPtr<IInterface> valueObj;
        cache->Get(key, (IInterface**)&valueObj);
        IBundle* bundle = IBundle::Probe(valueObj);
        if (bundle == NULL) return FALSE;
        String oldValue;
        IBaseBundle::Probe(bundle)->GetPairValue(&oldValue);
        if (oldValue.IsNull() && value.IsNull()) return TRUE;
        if ((oldValue.IsNull()) != (value.IsNull())) return FALSE;
        return oldValue.Equals(value);
    }
    return FALSE;
}

//===============================================================================
//                  SettingsProvider::MyBroadcastReceiver
//===============================================================================

SettingsProvider::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ SettingsProvider* host)
    : mHost(host)
{}

ECode SettingsProvider::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 userHandle;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE,
            IUserHandle::USER_OWNER, &userHandle);
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_USER_REMOVED)) {
        if (userHandle != IUserHandle::USER_OWNER) {
            mHost->OnUserRemoved(userHandle);
        }
        else if (action.Equals(IIntent::ACTION_USER_ADDED)) {
            mHost->OnProfilesChanged();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsProvider
//===============================================================================

SettingsProvider::SettingsProvider()
{}

SettingsProvider::~SettingsProvider()
{}

ECode SettingsProvider::constructor()
{
    FAIL_RETURN(ContentProvider::constructor());
    CSparseArray::New((ISparseArray**)&mOpenHelpers);
    return NOERROR;
}

Boolean SettingsProvider::SettingMovedToGlobal(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> key = CoreUtils::Convert(name);
    Boolean res1, res2;
    sSecureGlobalKeys->Contains(key, &res1);
    sSystemGlobalKeys->Contains(key, &res2);
    return res1 || res2;
}

ECode SettingsProvider::GetUriFor(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values,
    /* [in] */ Int64 rowId,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)

    AutoPtr<IList> pathSegments;
    tableUri->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    if (pathSegments && (pathSegments->GetSize(&size), size) != 1) {
        String str;
        IObject::Probe(tableUri)->ToString(&str);
        Logger::E(TAG, "Invalid URI: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Invalid URI: " + tableUri);
    }

    AutoPtr<IInterface> obj;
    pathSegments->Get(0, (IInterface**)&obj);
    String table;
    ICharSequence::Probe(obj)->ToString(&table);

    if (TABLE_SYSTEM.Equals(table) || TABLE_SECURE.Equals(table) ||
            TABLE_GLOBAL.Equals(table)) {
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

void SettingsProvider::SendNotify(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userHandle)
{
    // Update the system property *first*, so if someone is listening for
    // a notification and then using the contract class to get their data,
    // the system property will be updated and they'll get the new data.

    Boolean backedUpDataChanged = FALSE;

    AutoPtr<IList> pathSegments;
    uri->GetPathSegments((IList**)&pathSegments);

    String property, table;
    if (pathSegments) {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&table);
    }
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
        prop->Set(property, StringUtils::ToString(version));
    }

    // Inform the backup manager about a data change
    if (backedUpDataChanged) {
        if (mBackupManager) {
            mBackupManager->DataChanged();
        }
    }
    // Now send the notification through the content framework.

    String notify;
    uri->GetQueryParameter(String("notify"), &notify);
    if (notify.IsNull() || notify.Equals("true")) {
        Int32 notifyTarget = isGlobal ? IUserHandle::USER_ALL : userHandle;
        Int64 oldId = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        resolver->NotifyChange(uri, NULL, TRUE, notifyTarget);
        // } finally {
        Binder::RestoreCallingIdentity(oldId);
        // }
        if (LOCAL_LOGV) {
            Logger::V(TAG, "notifying for %d: %s", notifyTarget, TO_CSTR(uri));
        }
    }
    else {
        if (LOCAL_LOGV) {
            Logger::V(TAG, "notification suppressed: %s", TO_CSTR(uri));
        }
    }
}

ECode SettingsProvider::CheckWritePermissions(
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

ECode SettingsProvider::CheckUserRestrictions(
    /* [in] */ const String& setting,
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    sRestrictedKeys->Get(CoreUtils::Convert(setting), (IInterface**)&obj);
    String userRestriction;
    if (obj != NULL)
        ICharSequence::Probe(obj)->ToString(&userRestriction);

    AutoPtr<IUserHandle> handle;
    CUserHandle::New(userId, (IUserHandle**)&handle);
    Boolean res;
    if (!TextUtils::IsEmpty(userRestriction)
        && (mUserManager->HasUserRestriction(userRestriction, handle, &res), res)) {
        Logger::E(TAG, "Permission denial: user is restricted from changing this setting.");
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException(
        //         "Permission denial: user is restricted from changing this setting.");
    }
    return NOERROR;
}

ECode SettingsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Logger::W(TAG, "TODO: SettingsProvider::OnCreate need CBackupManager!");
    // CBackupManager::New(context, (IBackupManager**)&mBackupManager);

    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    helper->Get(context, (IUserManager**)&mUserManager);
    SetAppOps(IAppOpsManager::OP_NONE, IAppOpsManager::OP_WRITE_SETTINGS);
    EstablishDbTracking(IUserHandle::USER_OWNER);
    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    userFilter->AddAction(IIntent::ACTION_USER_ADDED);
    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, userFilter, (IIntent**)&intent);
    OnProfilesChanged();
    *result = TRUE;
    return NOERROR;
}

void SettingsProvider::OnUserRemoved(
    /* [in] */ Int32 userHandle)
{
    {
        AutoLock syncLock(this);
        // the db file itself will be deleted automatically, but we need to tear down
        // our caches and other internal bookkeeping.
        AutoPtr<IInterface> obj;
        sObserverInstances->Get(userHandle, (IInterface**)&obj);
        AutoPtr<IFileObserver> observer = IFileObserver::Probe(obj);
        if (observer != NULL) {
            observer->StopWatching();
            sObserverInstances->Delete(userHandle);
        }

        mOpenHelpers->Delete(userHandle);
        sSystemCaches->Delete(userHandle);
        sSecureCaches->Delete(userHandle);
        sKnownMutationsInFlight->Delete(userHandle);
        OnProfilesChanged();
    }
}

void SettingsProvider::OnProfilesChanged()
{
    {
        AutoLock syncLock(this);
        mUserManager->GetProfiles(IUserHandle::USER_OWNER, (IList**)&mManagedProfiles);
        if (mManagedProfiles != NULL) {
            // Remove the primary user from the list
            Int32 size;
            mManagedProfiles->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                mManagedProfiles->Get(i, (IInterface**)&obj);
                AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);
                Int32 id;
                info->GetId(&id);
                if (id == IUserHandle::USER_OWNER) {
                    mManagedProfiles->Remove(i);
                }
            }
            // If there are no managed profiles, reset the variable
            mManagedProfiles->GetSize(&size);
            if (size == 0) {
                mManagedProfiles = NULL;
            }
        }
        if (LOCAL_LOGV) {
            Logger::D(TAG, "Managed Profiles = %p", mManagedProfiles.Get());
        }
    }
}

void SettingsProvider::EstablishDbTracking(
    /* [in] */ Int32 userHandle)
{
    if (LOCAL_LOGV) {
        Logger::I(TAG, "Installing settings db helper and caches for user %d", userHandle);
    }

    AutoPtr<DatabaseHelper> dbhelper;

    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mOpenHelpers->Get(userHandle, (IInterface**)&obj);
        dbhelper = (DatabaseHelper*)ISQLiteOpenHelper::Probe(obj);

        if (dbhelper == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            dbhelper = new DatabaseHelper(context, userHandle);
            mOpenHelpers->Append(userHandle, (ISQLiteOpenHelper*)dbhelper);
            AutoPtr<SettingsCache> systemCache = new SettingsCache();
            systemCache->constructor(TABLE_SYSTEM);
            AutoPtr<SettingsCache> secureCache = new SettingsCache();
            secureCache->constructor(TABLE_SECURE);
            sSystemCaches->Append(userHandle, TO_IINTERFACE(systemCache));
            sSecureCaches->Append(userHandle, TO_IINTERFACE(secureCache));
            AutoPtr<IAtomicInteger32> integer;
            CAtomicInteger32::New(0, (IAtomicInteger32**)&integer);
            sKnownMutationsInFlight->Append(userHandle, integer);
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
    {    AutoLock syncLock(sObserverInstances);
        AutoPtr<IInterface> obj;
        sObserverInstances->Get(userHandle, (IInterface**)&obj);
        if (obj == NULL) {
            String path;
            db->GetPath(&path);
            AutoPtr<SettingsFileObserver> observer = new SettingsFileObserver(userHandle, path, this);
            sObserverInstances->Append(userHandle, (IFileObserver*)observer);
            observer->StartWatching();
        }
    }

    Boolean result;
    EnsureAndroidIdIsSet(userHandle, &result);
    StartAsyncCachePopulation(userHandle);
}

void SettingsProvider::StartAsyncCachePopulation(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<CachePrefetchThread> thread = new CachePrefetchThread(userHandle, this);
    thread->Start();
}

void SettingsProvider::FullyPopulateCaches(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DatabaseHelper> dbHelper;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mOpenHelpers->Get(userHandle, (IInterface**)&obj);
        dbHelper = (DatabaseHelper*)ISQLiteOpenHelper::Probe(obj);
    }
    if (dbHelper == NULL) {
        // User is gone.
        return;
    }

    // Only populate the globals cache once, for the owning user
    if (userHandle == IUserHandle::USER_OWNER) {
        FullyPopulateCache(dbHelper, TABLE_GLOBAL, sGlobalCache);
    }

    AutoPtr<IInterface> obj;
    sSecureCaches->Get(userHandle, (IInterface**)&obj);
    AutoPtr<SettingsCache> secureCache = (SettingsCache*)IObject::Probe(obj);
    FullyPopulateCache(dbHelper, TABLE_SECURE, secureCache);

    obj = NULL;
    sSystemCaches->Get(userHandle, (IInterface**)&obj);
    AutoPtr<SettingsCache> systemCache = (SettingsCache*)IObject::Probe(obj);
    FullyPopulateCache(dbHelper, TABLE_SYSTEM, systemCache);
}

void SettingsProvider::FullyPopulateCache(
    /* [in] */ DatabaseHelper* dbHelper,
    /* [in] */ const String& table,
    /* [in] */ SettingsCache* cache)
{
    AutoPtr<ISQLiteDatabase> db;
    dbHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = ISettingsNameValueTable::NAME;
    (*args)[1] = ISettingsNameValueTable::VALUE;
    String nullStr;
    AutoPtr<ICursor> c;
    db->Query(table, args, nullStr, NULL, nullStr, nullStr, nullStr,
        StringUtils::ToString(MAX_CACHE_ENTRIES + 1)/* limit */, (ICursor**)&c);
    // try {
    {
        AutoLock syncLock(cache);
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
    ICloseable::Probe(c)->Close();
    // }
}

ECode SettingsProvider::EnsureAndroidIdIsSet(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> contentUri;
    secure->GetCONTENT_URI((IUri**)&contentUri);
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
        // sanity-check the user before touching the db
        AutoPtr<IUserInfo> user;
        mUserManager->GetUserInfo(userHandle, (IUserInfo**)&user);
        if (user == NULL) {
            // can happen due to races when deleting users; treat as benign
            return NOERROR;
        }

        Logger::E(TAG, "TODO: SecureRandom has not been realized!!!!");
        // SecureRandom random = new SecureRandom();
        String newAndroidIdValue("93ea6006f51130fe");// String newAndroidIdValue = Long.toHexString(random.nextLong());

        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(ISettingsNameValueTable::NAME,
                CoreUtils::Convert(ISettingsSecure::ANDROID_ID));
        values->Put(ISettingsNameValueTable::VALUE,
                CoreUtils::Convert(newAndroidIdValue));
        AutoPtr<IUri> uri;
        ec = InsertForUser(contentUri, values, userHandle, (IUri**)&uri);
        FAIL_GOTO(ec, failed)
        if (uri == NULL) {
            Logger::E(TAG, "Unable to generate new ANDROID_ID for user %d", userHandle);
            ICloseable::Probe(c)->Close();
            return NOERROR;
        }
        Logger::D(TAG, "Generated and saved new ANDROID_ID [%s] for user %d",
                newAndroidIdValue.string(), userHandle);
        // Write a dropbox entry if it's a restricted profile
        Boolean res;
        if (user->IsRestricted(&res), res) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&obj);
            AutoPtr<IDropBoxManager> dbm = IDropBoxManager::Probe(obj);
            if (dbm != NULL && (dbm->IsTagEnabled(DROPBOX_TAG_USERLOG, &res), res)) {
                AutoPtr<ISystem> sys;
                CSystem::AcquireSingleton((ISystem**)&sys);
                Int64 now;
                sys->GetCurrentTimeMillis(&now);
                StringBuilder builder;
                builder += now;
                builder += ",restricted_profile_ssaid,";
                builder += newAndroidIdValue;
                builder += "\n";
                dbm->AddText(DROPBOX_TAG_USERLOG, builder.ToString());
            }
        }
    }
    *result = TRUE;
    // } finally {
    //     c.close();
    // }
failed:
    ICloseable::Probe(c)->Close();
    return ec;
}

AutoPtr<SettingsProvider::SettingsCache> SettingsProvider::GetOrConstructCache(
    /* [in] */ Int32 callingUser,
    /* [in] */ ISparseArray* which)
{
    AutoPtr<DatabaseHelper> helper;
    GetOrEstablishDatabase(callingUser, (DatabaseHelper**)&helper); // ignore return value; we don't need it
    AutoPtr<IInterface> obj;
    which->Get(callingUser, (IInterface**)&obj);
    AutoPtr<SettingsCache> cache = (SettingsCache*)IObject::Probe(obj);
    return cache;
}

ECode SettingsProvider::GetOrEstablishDatabase(
    /* [in] */ Int32 callingUser,
    /* [out] */ DatabaseHelper** databaseHelper)
{
    VALIDATE_NOT_NULL(databaseHelper);
    *databaseHelper = NULL;

    if (callingUser >= IProcess::SYSTEM_UID) {
        if (USER_CHECK_THROWS) {
            Logger::E(TAG, "Uid rather than user handle: %d", callingUser);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Uid rather than user handle: " + callingUser);
        }
        else {
            Logger::E(TAG, "establish db for uid rather than user: %d", callingUser);
            // Slog.wtf(TAG, "establish db for uid rather than user: " + callingUser);
        }
    }

    Int64 oldId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<DatabaseHelper> dbHelper;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mOpenHelpers->Get(callingUser, (IInterface**)&obj);
        dbHelper = (DatabaseHelper*)ISQLiteOpenHelper::Probe(obj);
    }

    if (NULL == dbHelper) {
        EstablishDbTracking(callingUser);
        {
        AutoLock syncLock(this);
            AutoPtr<IInterface> obj;
            mOpenHelpers->Get(callingUser, (IInterface**)&obj);
            dbHelper = (DatabaseHelper*)ISQLiteOpenHelper::Probe(obj);
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(oldId);
    // }
    *databaseHelper = dbHelper;
    REFCOUNT_ADD(*databaseHelper);
    return NOERROR;
}

AutoPtr<SettingsProvider::SettingsCache> SettingsProvider::CacheForTable(
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

void SettingsProvider::InvalidateCache(
    /* [in] */ Int32 callingUser,
    /* [in] */ const String& tableName)
{
    AutoPtr<SettingsCache> cache = CacheForTable(callingUser, tableName);
    if (cache == NULL) {
        return;
    }
    {
        AutoLock syncLock(cache);
        cache->EvictAll();
        cache->mCacheFullyMatchesDisk = FALSE;
    }
}

Boolean SettingsProvider::IsManagedProfile(
    /* [in] */ Int32 callingUser)
{
    {
        AutoLock syncLock(this);
        if (mManagedProfiles == NULL) return FALSE;
        Int32 size;
        mManagedProfiles->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mManagedProfiles->Get(i, (IInterface**)&obj);
            Int32 id;
            IUserInfo::Probe(obj)->GetId(&id);
            if (id == callingUser) {
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}

ECode SettingsProvider::Call(
    /* [in] */ const String& method,
    /* [in] */ const String& request,
    /* [in] */ IBundle* args,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;

    Int32 callingUser = UserHandle::GetCallingUserId();
    if (args != NULL) {
        Int32 reqUser = callingUser;
        args->GetInt32(ISettings::CALL_METHOD_USER_KEY, callingUser, &reqUser);
        if (reqUser != callingUser) {
            Int32 pid = Binder::GetCallingPid();
            Int32 uid = Binder::GetCallingUid();
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
        // Check if this request should be (re)directed to the primary user's db
        if (callingUser != IUserHandle::USER_OWNER
                && ShouldShadowParentProfile(callingUser, sSystemCloneToManagedKeys, request)) {
            callingUser = IUserHandle::USER_OWNER;
        }

        GetOrEstablishDatabase(callingUser, (DatabaseHelper**)&dbHelper);
        AutoPtr<IInterface> obj;
        sSystemCaches->Get(callingUser, (IInterface**)&obj);
        cache = (SettingsCache*)IObject::Probe(obj);
        AutoPtr<IBundle> b = LookupValue(dbHelper, TABLE_SYSTEM, cache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }
    if (ISettings::CALL_METHOD_GET_SECURE.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call(secure:%s) for %d", request.string(), callingUser);
        // Check if this is a setting to be copied from the primary user
        if (ShouldShadowParentProfile(callingUser, sSecureCloneToManagedKeys, request)) {
            // If the request if for location providers and there's a restriction, return none

            AutoPtr<IUserHandle> handle;
            CUserHandle::New(callingUser, (IUserHandle**)&handle);
            Boolean res;
            if (ISettingsSecure::LOCATION_PROVIDERS_ALLOWED.Equals(request)
                    && (mUserManager->HasUserRestriction(
                            IUserManager::DISALLOW_SHARE_LOCATION, handle, &res), res)) {
                AutoPtr<IInterface> obj;
                sSecureCaches->Get(callingUser, (IInterface**)&obj);
                AutoPtr<SettingsCache> sCache = (SettingsCache*)IObject::Probe(obj);
                AutoPtr<IBundle> b = sCache->PutIfAbsent(request, String(""));
                *bundle = b;
                REFCOUNT_ADD(*bundle);
                return NOERROR;
            }
            callingUser = IUserHandle::USER_OWNER;
        }

        GetOrEstablishDatabase(callingUser, (DatabaseHelper**)&dbHelper);
        AutoPtr<IInterface> obj;
        sSecureCaches->Get(callingUser, (IInterface**)&obj);
        cache = (SettingsCache*)IObject::Probe(obj);
        AutoPtr<IBundle> b = LookupValue(dbHelper, TABLE_SECURE, cache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }
    if (ISettings::CALL_METHOD_GET_GLOBAL.Equals(method)) {
        if (LOCAL_LOGV) Logger::V(TAG, "call(global:%s) for %d", request.string(), callingUser);
        // fast path: owner db & cache are immutable after onCreate() so we need not
        // guard on the attempt to look them up
        AutoPtr<DatabaseHelper> dbHelper1;
        GetOrEstablishDatabase(IUserHandle::USER_OWNER, (DatabaseHelper**)&dbHelper1);
        AutoPtr<IBundle> b = LookupValue(dbHelper1, TABLE_GLOBAL, sGlobalCache, request);
        *bundle = b;
        REFCOUNT_ADD(*bundle)
        return NOERROR;
    }

    // Put methods - new value is in the args bundle under the key named by
    // the Settings.NameValueTable.VALUE static.
    String newValue;
    if (args) {
        args->GetString(ISettingsNameValueTable::VALUE, &newValue);
    }

    // Framework can't do automatic permission checking for calls, so we need
    // to do it here.
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 result;
    context->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::WRITE_SETTINGS, &result);
    if (result != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Permission denial: writing to settings requires %1$s",
            Elastos::Droid::Manifest::permission::WRITE_SETTINGS.string());
        return E_SECURITY_EXCEPTION;
    }

    // Also need to take care of app op.
    AutoPtr<IAppOpsManager> manager;
    GetAppOpsManager((IAppOpsManager**)&manager);
    String pkg;
    GetCallingPackage(&pkg);
    Int32 value;
    manager->NoteOp(IAppOpsManager::OP_WRITE_SETTINGS,
            Binder::GetCallingUid(), pkg, &value);
    if (value != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    ECode ec;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(ISettingsNameValueTable::NAME, CoreUtils::Convert(request));
    values->Put(ISettingsNameValueTable::VALUE, CoreUtils::Convert(newValue));
    if (ISettings::CALL_METHOD_PUT_SYSTEM.Equals(method)) {
        if (LOCAL_LOGV) {
            Logger::V(TAG, "call_put(system:%s=%s) for %d", request.string(),
                newValue.string(), callingUser);
        }

        // Extra check for USER_OWNER to optimize for the 99%
        if (callingUser != IUserHandle::USER_OWNER &&
                ShouldShadowParentProfile(callingUser, sSystemCloneToManagedKeys, request)) {
            // Don't write these settings, as they are cloned from the parent profile
            return NOERROR;
        }

        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        AutoPtr<IUri> uri;
        settingsSystem->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);

        // Clone the settings to the managed profiles so that notifications can be sent out
        Boolean res;
        if (callingUser == IUserHandle::USER_OWNER && mManagedProfiles != NULL
                && (sSystemCloneToManagedKeys->Contains(CoreUtils::Convert(request), &res), res)) {
            const Int64 token = Binder::ClearCallingIdentity();
            // try {
            Int32 size;
            mManagedProfiles->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                mManagedProfiles->Get(i, (IInterface**)&obj);
                Int32 id;
                IUserInfo::Probe(obj)->GetId(&id);

                if (LOCAL_LOGV) {
                    Logger::V(TAG, "putting to additional user %d", id);
                }
                AutoPtr<IUri> resultUri;
                InsertForUser(uri, values, id, (IUri**)&resultUri);
            }
            // } finally {
            Binder::RestoreCallingIdentity(token);
            // }
        }

    }
    else if (ISettings::CALL_METHOD_PUT_SECURE.Equals(method)) {
        if (LOCAL_LOGV) {
            Logger::V(TAG, "call_put(secure:%s=%s for %d", request.string(),
                    newValue.string(), callingUser);
        }

        // Extra check for USER_OWNER to optimize for the 99%
        if (callingUser != IUserHandle::USER_OWNER &&
                ShouldShadowParentProfile(callingUser, sSecureCloneToManagedKeys, request)) {
            // Don't write these settings, as they are cloned from the parent profile
            return NOERROR;
        }

        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        AutoPtr<IUri> uri;
        settingsSecure->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);

        // Clone the settings to the managed profiles so that notifications can be sent out
        Boolean res;
        if (callingUser == IUserHandle::USER_OWNER && mManagedProfiles != NULL
                && (sSecureCloneToManagedKeys->Contains(CoreUtils::Convert(request), &res), res)) {
            const Int64 token = Binder::ClearCallingIdentity();
            // try {
            Int32 size;
            mManagedProfiles->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                mManagedProfiles->Get(i, (IInterface**)&obj);
                Int32 id;
                IUserInfo::Probe(obj)->GetId(&id);

                if (LOCAL_LOGV) {
                    Logger::V(TAG, "putting to additional user %d", id);
                }
                // try {
                AutoPtr<IUri> resultUri;
                ec = InsertForUser(uri, values, id, (IUri**)&resultUri);
                if (FAILED(ec)) {
                    // Temporary fix, see b/17450158
                    Logger::W(TAG, "Cannot clone request '%s' with value '%s' to managed profile (id %d)",
                            request.string(), newValue.string(), id);
                }
                // } catch (SecurityException e) {
                //     // Temporary fix, see b/17450158
                //     Slog.w(TAG, "Cannot clone request '" + request + "' with value '"
                //             + newValue + "' to managed profile (id "
                //             + mManagedProfiles.get(i).id + ")", e);
                // }
            }
            // } finally {
            Binder::RestoreCallingIdentity(token);
            // }
        }
    }
    else if (ISettings::CALL_METHOD_PUT_GLOBAL.Equals(method)) {
        if (LOCAL_LOGV) {
            Logger::V(TAG, "call_put(global:%s=%s for %d", request.string(),
                    newValue.string(), callingUser);
        }
        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        AutoPtr<IUri> uri;
        settingsGlobal->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<IUri> value;
        ec = InsertForUser(uri, values, callingUser, (IUri**)&value);
    }
    else {
        Logger::W(TAG, "call() with invalid method: %s", method.string());
    }

    *bundle = NULL;
    return ec;
}

Boolean SettingsProvider::ShouldShadowParentProfile(
    /* [in] */ Int32 userId,
    /* [in] */ IHashSet* keys,
    /* [in] */ const String& name)
{
    Boolean res;
    return IsManagedProfile(userId) && (keys->Contains(CoreUtils::Convert(name), &res), res);
}

AutoPtr<IBundle> SettingsProvider::LookupValue(
    /* [in] */ DatabaseHelper* dbHelper,
    /* [in] */ const String& table,
    /* [in] */ SettingsCache* cache,
    /* [in] */ const String& key)
{
    if (cache == NULL || key.IsNull()) {
        Int32 userId = UserHandle::GetCallingUserId();
        Logger::E(TAG, "cache is null for user %d : key=%s", userId, key.string());
        return NULL;
    }
    {
        AutoLock syncLock(cache);
        AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
        AutoPtr<IInterface> valueObj;
        cache->Get(keyObj, (IInterface**)&valueObj);
        AutoPtr<IBundle> value = IBundle::Probe(valueObj);
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
    String nullStr;
    // try {
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = key;
    ECode ec = db->Query(table, COLUMN_VALUE, String("name=?"), args,
        nullStr, nullStr, nullStr, nullStr, (ICursor**)&cursor);
    if (FAILED(ec)) {
        Logger::W(TAG, "settings lookup error");
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        return NULL;
    }
    Int32 count = 0;
    if (cursor != NULL && (cursor->GetCount(&count), count == 1)) {
        Boolean result;
        cursor->MoveToFirst(&result);
        String value;
        ec = cursor->GetString(0, &value);
        if (FAILED(ec)) Logger::W(TAG, "settings lookup error");
        if (cursor != NULL) ICloseable::Probe(cursor)->Close();
        return cache->PutIfAbsent(key, value);
    }

    cache->PutIfAbsent(key, String(NULL));
    if (cursor != NULL) ICloseable::Probe(cursor)->Close();
    return NULL_SETTING;
}

ECode SettingsProvider::Query(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* select,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)

    Int32 userId = UserHandle::GetCallingUserId();
    return QueryForUser(url, select, where, whereArgs, sort, userId, cursor);
}

ECode SettingsProvider::QueryForUser(
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
        IObject::Probe(url)->ToString(&str);
        Logger::V(TAG, "query(%s) for user %d", str.string(), forUser);
    }
    AutoPtr<SqlArguments> args;
    FAIL_RETURN(SqlArguments::CreateSqlArguments(url, where, whereArgs, (SqlArguments**)&args))
    AutoPtr<DatabaseHelper> dbH;
    GetOrEstablishDatabase(
            TABLE_GLOBAL.Equals(args->mTable) ? IUserHandle::USER_OWNER : forUser,
            (DatabaseHelper**)&dbH);
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
            ICloseable::Probe(c)->Close();
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

ECode SettingsProvider::GetType(
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

ECode SettingsProvider::BulkInsert(
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

    AutoPtr<IAtomicInteger32> mutationCount;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        sKnownMutationsInFlight->Get(callingUser, (IInterface**)&obj);
        mutationCount = IAtomicInteger32::Probe(obj);
    }
    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->IncrementAndGet(&outV);
    }

    AutoPtr<DatabaseHelper> dbH;
    GetOrEstablishDatabase(
            TABLE_GLOBAL.Equals(args->mTable) ? IUserHandle::USER_OWNER : callingUser,
            (DatabaseHelper**)&dbH);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    db->BeginTransaction();
    // try {
    Int32 numValues = values->GetLength();
    for (Int32 i = 0; i < numValues; i++) {
        String str;
        (*values)[i]->GetAsString(ISettingsNameValueTable::NAME, &str);
        FAIL_RETURN(CheckUserRestrictions(str, callingUser));
        Int64 v;
        if (db->Insert(args->mTable, String(NULL), (*values)[i], &v), v < 0) {
            *number = 0;
            db->EndTransaction();
            if (mutationCount != NULL) {
                Int32 outV;
                mutationCount->DecrementAndGet(&outV);
            }
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
    db->EndTransaction();
    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->DecrementAndGet(&outV);
    }
    // }

    SendNotify(uri, callingUser);
    *number = values->GetLength();
    return NOERROR;
}

Boolean SettingsProvider::ParseProviderList(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ Int32 desiredUser)
{
    String value;
    initialValues->GetAsString(ISettingsNameValueTable::VALUE, &value);
    String newProviders;
    if (!value.IsNull() && value.GetLength() > 1) {
        Char32 prefix = value.GetChar(0);
        if (prefix == '+' || prefix == '-') {
            // skip prefix
            value = value.Substring(1);

            // read list of enabled providers into "providers"
            String providers("");
            AutoPtr< ArrayOf<String> > columns = ArrayOf<String>::Alloc(1);
            (*columns)[0] = ISettingsNameValueTable::VALUE;
            StringBuilder builder(ISettingsNameValueTable::NAME);
            builder += "=\'";
            builder += ISettingsSecure::LOCATION_PROVIDERS_ALLOWED;
            builder += "\'";
            String where = builder.ToString();
            AutoPtr<ICursor> cursor;
            QueryForUser(url, columns, where, NULL, String(NULL), desiredUser, (ICursor**)&cursor);
            Int32 count;
            if (cursor != NULL && (cursor->GetCount(&count), count == 1)) {
                // try {
                Boolean result;
                cursor->MoveToFirst(&result);
                cursor->GetString(0, &providers);
                // } finally {
                ICloseable::Probe(cursor)->Close();
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
                initialValues->Put(ISettingsNameValueTable::VALUE,
                        CoreUtils::Convert(newProviders));
            }
        }
    }

    return TRUE;
}

ECode SettingsProvider::Insert(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri)
    Int32 userId = UserHandle::GetCallingUserId();
    return InsertForUser(url, initialValues, userId, insertedUri);
}

ECode SettingsProvider::InsertForUser(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [in] */ Int32 desiredUserHandle,
    /* [out] */ IUri** outUri)
{
    VALIDATE_NOT_NULL(outUri)
    *outUri = NULL;

    Int32 callingUser = UserHandle::GetCallingUserId();
    if (callingUser != desiredUserHandle) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        FAIL_RETURN(context->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
                String("Not permitted to access settings for other users")))
    }

    if (LOCAL_LOGV) {
        String str;
        IObject::Probe(url)->ToString(&str);
        Logger::V(TAG, "insert(%s) for user %d by %d", str.string(),
                desiredUserHandle, callingUser);
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
    initialValues->GetAsString(ISettingsNameValueTable::NAME, &name);
    if (ISettingsSecure::LOCATION_PROVIDERS_ALLOWED.Equals(name)) {
        if (!ParseProviderList(url, initialValues, desiredUserHandle)) {
            *outUri = NULL;
            return NOERROR;
        }
    }

    // If this is an insert() of a key that has been migrated to the global store,
    // redirect the operation to that store
    Boolean res1, res2;
    if (!name.IsNull()) {
        if ((sSecureGlobalKeys->Contains(CoreUtils::Convert(name), &res1), res1)
                || (sSystemGlobalKeys->Contains(CoreUtils::Convert(name), &res2), res2)) {
            if (!TABLE_GLOBAL.Equals(args->mTable)) {
                if (LOCAL_LOGV) Logger::I(TAG, "Rewrite of insert() of now-global key %s", name.string());
            }
            args->mTable = TABLE_GLOBAL;  // next condition will rewrite the user handle
        }
    }

    // Check write permissions only after determining which table the insert will touch
    FAIL_RETURN(CheckWritePermissions(args));

    FAIL_RETURN(CheckUserRestrictions(name, desiredUserHandle));

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
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        sKnownMutationsInFlight->Get(callingUser, (IInterface**)&obj);
        mutationCount = IAtomicInteger32::Probe(obj);
    }

    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->IncrementAndGet(&outV);
    }
    AutoPtr<DatabaseHelper> dbH;
    GetOrEstablishDatabase(desiredUserHandle, (DatabaseHelper**)&dbH);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int64 rowId;
    db->Insert(args->mTable, String(NULL), initialValues, &rowId);
    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->DecrementAndGet(&outV);
    }
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

ECode SettingsProvider::Delete(
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

    AutoPtr<IAtomicInteger32> mutationCount;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        sKnownMutationsInFlight->Get(callingUser, (IInterface**)&obj);
        mutationCount = IAtomicInteger32::Probe(obj);
    }

    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->IncrementAndGet(&outV);
    }

    AutoPtr<DatabaseHelper> dbH;
    GetOrEstablishDatabase(callingUser, (DatabaseHelper**)&dbH);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int32 count;
    FAIL_RETURN(db->Delete(args->mTable, args->mWhere, args->mArgs, &count))
    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->DecrementAndGet(&outV);
    }
    if (count > 0) {
        InvalidateCache(callingUser, args->mTable);  // before we notify
        SendNotify(url, callingUser);
    }
    StartAsyncCachePopulation(callingUser);
    if (LOCAL_LOGV) Logger::V(TAG,  "%s: %d row(s) deleted", args->mTable.string(), count);
    *rowsAffected = count;
    return NOERROR;
}

ECode SettingsProvider::Update(
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
    String str;
    initialValues->GetAsString(ISettingsNameValueTable::NAME, &str);
    FAIL_RETURN(CheckUserRestrictions(str, callingUser));

    AutoPtr<IAtomicInteger32> mutationCount;
    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        sKnownMutationsInFlight->Get(callingUser, (IInterface**)&obj);
        mutationCount = IAtomicInteger32::Probe(obj);
    }

    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->IncrementAndGet(&outV);
    }

    AutoPtr<DatabaseHelper> dbH;
    GetOrEstablishDatabase(callingUser, (DatabaseHelper**)&dbH);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(dbH->GetWritableDatabase((ISQLiteDatabase**)&db))
    Int32 count;
    FAIL_RETURN(db->Update(args->mTable, initialValues, args->mWhere, args->mArgs, &count))
    if (mutationCount != NULL) {
        Int32 outV;
        mutationCount->DecrementAndGet(&outV);
    }
    if (count > 0) {
        InvalidateCache(callingUser, args->mTable);  // before we notify
        SendNotify(url, callingUser);
    }
    StartAsyncCachePopulation(callingUser);
    if (LOCAL_LOGV) Logger::V(TAG,  "%s: %d row(s) deleted", args->mTable.string(), count);
    *rowsAffected = count;
    return NOERROR;
}

ECode SettingsProvider::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)
    *fileDescriptor = NULL;

    /*
     * When a client attempts to openFile the default ringtone or
     * notification setting Uri, we will proxy the call to the current
     * default ringtone's Uri (if it is in the media provider).
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
            // proxy the openFile call to media provider
            String authority;
            soundUri->GetAuthority(&authority);
            if (authority.Equals(IMediaStore::AUTHORITY)) {
                AutoPtr<IContentResolver> resolver;
                return resolver->OpenFileDescriptor(soundUri, mode, fileDescriptor);
            }
        }
    }

    return ContentProvider::OpenFile(uri, mode, fileDescriptor);
}

ECode SettingsProvider::OpenAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    /*
     * When a client attempts to openFile the default ringtone or
     * notification setting Uri, we will proxy the call to the current
     * default ringtone's Uri (if it is in the media provider).
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
            // proxy the openFile call to media provider
            String authority;
            soundUri->GetAuthority(&authority);
            if (authority.Equals(IMediaStore::AUTHORITY)) {
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
} // namespace Droid
} // namespace Elastos
