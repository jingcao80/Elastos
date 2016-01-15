
#include <elastos/droid/server/content/SyncStorageEngine.h>
#include <elastos/droid/server/content/SyncOperation.h>
#include <elastos/droid/server/content/SyncManager.h>
#include <elastos/droid/server/content/SyncQueue.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/R.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.External.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
// using Elastos::Droid::Accounts::CAccount;
// using Elastos::Droid::Accounts::CAccountAndUser;
using Elastos::Droid::Content::CSyncInfo;
using Elastos::Droid::Content::CSyncStatusInfo;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IPeriodicSync;
using Elastos::Droid::Content::CPeriodicSync;
using Elastos::Droid::Content::IContentResolverHelper;
using Elastos::Droid::Content::CContentResolverHelper;
using Elastos::Droid::Content::ISyncRequest;
using Elastos::Droid::Content::ISyncRequestBuilder;
using Elastos::Droid::Content::CSyncRequestBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabaseHelper;
using Elastos::Droid::Database::Sqlite::CSQLiteDatabaseHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Server::Content::PendingOperation;
using Elastos::Droid::Server::Content::AuthorityInfo;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::ISet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IRandom;
using Elastos::Utility::CRandom;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {


const Int64 SyncStorageEngine::MILLIS_IN_4WEEKS = ((Int64)1000) * 60 * 60 * 24 * 7 * 4;
const Int32 SyncStorageEngine::EVENT_START = 0;
const Int32 SyncStorageEngine::EVENT_STOP = 1;

AutoPtr<ArrayOf<String> > InitEVENTS()
{
    AutoPtr<ArrayOf<String> > events = ArrayOf<String>::Alloc(2);
    events->Set(0, String("START"));
    events->Set(1, String("STOP"));
    return events;
}
const AutoPtr<ArrayOf<String> > SyncStorageEngine::EVENTS = InitEVENTS();

const Int32 SyncStorageEngine::SOURCE_SERVER = 0;

const Int32 SyncStorageEngine::SOURCE_LOCAL = 1;
const Int32 SyncStorageEngine::SOURCE_POLL = 2;
const Int32 SyncStorageEngine::SOURCE_USER = 3;
const Int32 SyncStorageEngine::SOURCE_PERIODIC = 4;
const Int32 SyncStorageEngine::SOURCE_SERVICE = 5;

const Int64 SyncStorageEngine::NOT_IN_BACKOFF_MODE = -1;

AutoPtr<ArrayOf<String> > InitSOURCES()
{
    AutoPtr<ArrayOf<String> > events = ArrayOf<String>::Alloc(6);
    events->Set(0, String("SERVER"));
    events->Set(1, String("LOCAL"));
    events->Set(2, String("POLL"));
    events->Set(3, String("USER"));
    events->Set(4, String("PERIODIC"));
    events->Set(5, String("SERVICE"));
    return events;
}
const AutoPtr< ArrayOf<String> > SyncStorageEngine::SOURCES = InitSOURCES();

const String SyncStorageEngine::MESG_SUCCESS("success");
const String SyncStorageEngine::MESG_CANCELED("canceled");

const Int32 SyncStorageEngine::MAX_HISTORY = 100;

const Int32 SyncStorageEngine::STATUS_FILE_END = 0;
const Int32 SyncStorageEngine::STATUS_FILE_ITEM = 100;
const Int32 SyncStorageEngine::PENDING_OPERATION_VERSION = 3;

const Int32 SyncStorageEngine::STATISTICS_FILE_END = 0;
const Int32 SyncStorageEngine::STATISTICS_FILE_ITEM_OLD = 100;
const Int32 SyncStorageEngine::STATISTICS_FILE_ITEM = 101;

const Int32 SyncStorageEngine::PENDING_FINISH_TO_WRITE = 4;

const String SyncStorageEngine::XML_ATTR_VERSION("version");
const String SyncStorageEngine::XML_ATTR_AUTHORITYID("authority_id");
const String SyncStorageEngine::XML_ATTR_SOURCE("source");
const String SyncStorageEngine::XML_ATTR_EXPEDITED("expedited");
const String SyncStorageEngine::XML_ATTR_REASON("reason");

const String SyncStorageEngine::TAG("SyncManager");
const String SyncStorageEngine::TAG_FILE("SyncManagerFile");

const String SyncStorageEngine::XML_ATTR_NEXT_AUTHORITY_ID("nextAuthorityId");
const String SyncStorageEngine::XML_ATTR_LISTEN_FOR_TICKLES("listen-for-tickles");
const String SyncStorageEngine::XML_ATTR_SYNC_RANDOM_OFFSET("offsetInSeconds");
const String SyncStorageEngine::XML_ATTR_ENABLED("enabled");
const String SyncStorageEngine::XML_ATTR_USER("user");
const String SyncStorageEngine::XML_TAG_LISTEN_FOR_TICKLES("listenForTickles");

const Int64 SyncStorageEngine::DEFAULT_POLL_FREQUENCY_SECONDS = 60 * 60 * 24; // One day
const Double SyncStorageEngine::DEFAULT_FLEX_PERCENT_SYNC = 0.04;
const Int64 SyncStorageEngine::DEFAULT_MIN_FLEX_ALLOWED_SECS = 5;

const Int32 SyncStorageEngine::MSG_WRITE_STATUS = 1;
const Int64 SyncStorageEngine::WRITE_STATUS_DELAY = 1000*60*10; // 10 minutes

const Int32 SyncStorageEngine::MSG_WRITE_STATISTICS = 2;
const Int64 SyncStorageEngine::WRITE_STATISTICS_DELAY = 1000*60*30; // 1/2 hour

const Boolean SyncStorageEngine::SYNC_ENABLED_DEFAULT = FALSE;

// the version of the accounts xml file format
const Int32 SyncStorageEngine::ACCOUNTS_VERSION = 2;

AutoPtr<HashMap<String, String> > InitAuthorityRenames()
{
    AutoPtr<HashMap<String, String> > map = new HashMap<String, String>();
    (*map)[String("contacts")] = String("com.android.contacts");
    (*map)[String("calendar")] = String("com.android.calendar");
    return map;
}
AutoPtr<HashMap<String, String> > SyncStorageEngine::sAuthorityRenames = InitAuthorityRenames();

AutoPtr<SyncStorageEngine> SyncStorageEngine::sSyncStorageEngine;


//=========================================================================================
// DayStats
//=========================================================================================
DayStats::DayStats(
    /* [in] */ Int32 day)
    : mDay(day)
    , mSuccessCount(0)
    , mSuccessTime(0)
    , mFailureCount(0)
    , mFailureTime(0)
{
}

//=========================================================================================
// PendingOperation
//=========================================================================================
PendingOperation::PendingOperation(
    /* [in] */ AuthorityInfo* authority,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 source,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean expedited)
    : mReason(reason)
    , mSyncSource(source)
    , mExpedited(expedited)
{
    mTarget = authority->mTarget;
    if (extras != NULL) {
        CBundle::New(extras, (IBundle**)&mExtras);
    }
    else {
        mExtras = NULL;
    }
    mAuthorityId = authority->mIdent;
}

PendingOperation::PendingOperation(
    /* [in] */ PendingOperation* other)
{
    mReason = other->mReason;
    mSyncSource = other->mSyncSource;
    mTarget = other->mTarget;
    mExtras = other->mExtras;
    mAuthorityId = other->mAuthorityId;
    mExpedited = other->mExpedited;
}

ECode PendingOperation::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    PendingOperation* o = (PendingOperation*)IObject::Probe(other);
    return mTarget->MatchesSpec(o->mTarget);
}

ECode PendingOperation::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("service=");
    sb += Object::ToString(mTarget->mService);
    sb += " user=";
    sb += mTarget->mUserId;
    sb += " auth=";
    sb += Object::ToString(mTarget);
    sb += " account=";
    sb += Object::ToString(mTarget->mAccount);
    sb += " src=";
    sb += mSyncSource;
    sb += " extras=";
    sb += Object::ToString(mExtras);
    *str = sb.ToString();
    return NOERROR;
}

//=========================================================================================
// AccountInfo
//=========================================================================================
AccountInfo::AccountInfo(
    /* [in] */ IAccountAndUser* accountAndUser)
    : mAccountAndUser(accountAndUser)
{
}

//=========================================================================================
// EndPoint
//=========================================================================================

const AutoPtr<EndPoint> EndPoint::USER_ALL_PROVIDER_ALL_ACCOUNTS_ALL = new EndPoint(NULL, String(NULL), IUserHandle::USER_ALL);

CAR_INTERFACE_IMPL(EndPoint, Object, ISyncStorageEngineEndPoint)

EndPoint::EndPoint(
    /* [in] */ IComponentName* service,
    /* [in] */ Int32 userId)
    : mService(service)
    , mUserId(userId)
    , mTarget_service(TRUE)
    , mTarget_provider(FALSE)
{
}

EndPoint::EndPoint(
    /* [in] */ IAccount* account,
    /* [in] */ const String& provider,
    /* [in] */ Int32 userId)
    : mAccount(account)
    , mUserId(userId)
    , mProvider(provider)
    , mTarget_service(FALSE)
    , mTarget_provider(TRUE)
{
}

ECode EndPoint::MatchesSpec(
    /* [in] */ ISyncStorageEngineEndPoint* spec,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MatchesSpec(spec);
    return NOERROR;
}

Boolean EndPoint::MatchesSpec(
    /* [in] */ ISyncStorageEngineEndPoint* spec)
{
    EndPoint* other = (EndPoint*)spec;
    if (mUserId != other->mUserId
            && mUserId != IUserHandle::USER_ALL
            && other->mUserId != IUserHandle::USER_ALL) {
        return FALSE;
    }
    if (mTarget_service && other->mTarget_service) {
        return Object::Equals(mService, other->mService);
    }
    else if (mTarget_provider && other->mTarget_provider) {
        Boolean accountsMatch;
        if (other->mAccount == NULL) {
            accountsMatch = TRUE;
        }
        else {
            accountsMatch = Object::Equals(mAccount, other->mAccount);
        }
        Boolean providersMatch;
        if (other->mProvider == NULL) {
            providersMatch = TRUE;
        }
        else {
            providersMatch = mProvider.Equals(other->mProvider);
        }
        return accountsMatch && providersMatch;
    }

    return FALSE;
}

ECode EndPoint::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    if (mTarget_provider) {
        if (mAccount == NULL) {
            sb.Append("ALL ACCS");
        }
        else {
            String info;
            mAccount->GetName(&info);
            sb.Append(info);
        }
        sb.Append("/");
        if (mProvider == NULL) {
            sb.Append("ALL PDRS");
        }
        else {
            sb.Append(mProvider);
        }
    }
    else if (mTarget_service) {
        String pkgName, clsName;
        mService->GetPackageName(&pkgName);
        mService->GetClassName(&clsName);
        sb.Append(pkgName);
        sb.Append("/");
        sb.Append(clsName);
    }
    else {
        sb.Append("invalid target");
    }
    sb.Append(":u");
    sb.Append(mUserId);
    *str = sb.ToString();
    return NOERROR;
}

//=========================================================================================
// AuthorityInfo
//=========================================================================================

AuthorityInfo::AuthorityInfo(
    /* [in] */ AuthorityInfo* toCopy)
{
    mTarget = toCopy->mTarget;
    mIdent = toCopy->mIdent;
    mEnabled = toCopy->mEnabled;
    mSyncable = toCopy->mSyncable;
    mBackoffTime = toCopy->mBackoffTime;
    mBackoffDelay = toCopy->mBackoffDelay;
    mDelayUntil = toCopy->mDelayUntil;

    List<AutoPtr<IPeriodicSync> >::Iterator it = toCopy->mPeriodicSyncs.Begin();
    for (; it != toCopy->mPeriodicSyncs.End(); ++it) {
        // Still not a perfect copy, because we are just copying the mappings.
        AutoPtr<IPeriodicSync> newSync;
        CPeriodicSync::New((*it).Get(), (IPeriodicSync**)&newSync);
        mPeriodicSyncs.PushBack(newSync);
    }
}

AuthorityInfo::AuthorityInfo(
    /* [in] */ EndPoint* info,
    /* [in] */ Int32 id)
    : mIdent(id)
    , mEnabled(FALSE)
    , mSyncable(0)
    , mBackoffTime(0)
    , mDelayUntil(0)
{
    mTarget = info;
    mEnabled = info->mTarget_provider ? SyncStorageEngine::SYNC_ENABLED_DEFAULT : TRUE;
    // Service is active by default,
    if (info->mTarget_service) {
        mSyncable = 1;
    }
    DefaultInitialisation();
}

void AuthorityInfo::DefaultInitialisation()
{
    mSyncable = -1; // default to "unknown"
    mBackoffTime = -1; // if < 0 then we aren't in backoff mode
    mBackoffDelay = -1; // if < 0 then we aren't in backoff mode
    AutoPtr<IPeriodicSync> defaultSync;
    // Old version is one sync a day. Empty bundle gets replaced by any addPeriodicSync()
    // call.
    if (mTarget->mTarget_provider) {
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        CPeriodicSync::New(mTarget->mAccount, mTarget->mProvider,
            bundle, SyncStorageEngine::DEFAULT_POLL_FREQUENCY_SECONDS,
            SyncStorageEngine::CalculateDefaultFlexTime(SyncStorageEngine::DEFAULT_POLL_FREQUENCY_SECONDS),
            (IPeriodicSync**)&defaultSync);
        mPeriodicSyncs.PushBack(defaultSync);
    }
}

ECode AuthorityInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(Object::ToString(mTarget));
    sb += ", enabled=";
    sb += mEnabled;
    sb += ", syncable=";
    sb += mSyncable;
    sb += ", backoff=";
    sb += mBackoffTime;
    sb += ", delay=";
    sb += mDelayUntil;
    *str = sb.ToString();
    return NOERROR;
}

//=========================================================================================
// SyncHistoryItem
//=========================================================================================

SyncHistoryItem::SyncHistoryItem()
    : mAuthorityId(0)
    , mHistoryId(0)
    , mEventTime(0)
    , mElapsedTime(0)
    , mSource(0)
    , mEvent(0)
    , mUpstreamActivity(0)
    , mDownstreamActivity(0)
    , mInitialization(FALSE)
    , mReason(0)
{}

//=========================================================================================
// SyncStorageEngine
//=========================================================================================

SyncStorageEngine::SyncStorageEngine(
    /* [in] */ IContext* context,
    /* [in] */ IFile* dataDir)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mChangeListeners);

    mNextAuthorityId = 0;

    // We keep 4 weeks of stats.
    mDayStats = ArrayOf<DayStats*>::Alloc(7 * 4);
    mYear = 0;
    mYearInDays = 0;
    mSyncRandomOffset = 0;
    mNumPendingFinished = 0;
    mNextHistoryId = 0;

    mContext = context;
    sSyncStorageEngine = this;

    AutoPtr<ICalendarHelper> calHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calHelper);
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("GMT+0"), (ITimeZone**)&tz);
    calHelper->GetInstance(tz, (ICalendar**)&mCal);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_syncstorageengine_masterSyncAutomatically,
        &mDefaultMasterSyncAutomatically);

    AutoPtr<IFile> systemDir, syncDir;
    CFile::New(dataDir, String("system"), (IFile**)&systemDir);
    CFile::New(systemDir, String("sync"), (IFile**)&syncDir);
    Boolean bval;
    syncDir->Mkdirs(&bval);

    MaybeDeleteLegacyPendingInfoLocked(syncDir);

    AutoPtr<IFile> f;
    CFile::New(syncDir, String("accounts.xml"), (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mAccountInfoFile);

    f = NULL;
    CFile::New(syncDir, String("status.bin"), (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mStatusFile);

    f = NULL;
    CFile::New(syncDir, String("pending.xml"), (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mPendingFile);

    f = NULL;
    CFile::New(syncDir, String("stats.bin"), (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mStatisticsFile);

    ReadAccountInfoLocked();
    ReadStatusLocked();
    ReadPendingOperationsLocked();
    ReadStatisticsLocked();
    ReadAndDeleteLegacyAccountInfoLocked();
    WriteAccountInfoLocked();
    WriteStatusLocked();
    WritePendingOperationsLocked();
    WriteStatisticsLocked();
}

AutoPtr<SyncStorageEngine> SyncStorageEngine::NewTestInstance(
    /* [in] */ IContext* context)
{
    AutoPtr<IFile> filesDir;
    context->GetFilesDir((IFile**)&filesDir);
    return new SyncStorageEngine(context, filesDir);
}

void SyncStorageEngine::Init(
    /* [in] */ IContext* context)
{
    if (sSyncStorageEngine != NULL) {
        return;
    }

    // This call will return the correct directory whether Encrypted File Systems is
    // enabled or not.
    AutoPtr<IFile> dataDir;
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    environment->GetSecureDataDirectory((IFile**)&dataDir);
    sSyncStorageEngine = new SyncStorageEngine(context, dataDir);
}

AutoPtr<SyncStorageEngine> SyncStorageEngine::GetSingleton()
{
    if (sSyncStorageEngine == NULL) {
        Logger::E(TAG, "not initialized");
        return NULL;
    }
    return sSyncStorageEngine;
}

void SyncStorageEngine::SetOnSyncRequestListener(
    /* [in] */ IOnSyncRequestListener* listener)
{
    if (mSyncRequestListener == NULL) {
        mSyncRequestListener = listener;
    }
}

ECode SyncStorageEngine::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == MSG_WRITE_STATUS) {
        synchronized(mAuthoritiesLock) {
            WriteStatusLocked();
        }
    }
    else if (what == MSG_WRITE_STATISTICS) {
        synchronized(mAuthoritiesLock) {
            WriteStatisticsLocked();
        }
    }
    return NOERROR;
}

Int32 SyncStorageEngine::GetSyncRandomOffset()
{
    return mSyncRandomOffset;
}

void SyncStorageEngine::AddStatusChangeListener(
    /* [in] */ Int32 mask,
    /* [in] */ IISyncStatusObserver* callback)
{
    synchronized(mAuthoritiesLock) {
        AutoPtr<IInteger32> ii = CoreUtils::Convert(mask);
        Boolean result;
        mChangeListeners->Register(
            callback, ii.Get(), &result);
    }
}

void SyncStorageEngine::RemoveStatusChangeListener(
    /* [in] */ IISyncStatusObserver* callback)
{
    synchronized(mAuthoritiesLock) {
        Boolean result;
        mChangeListeners->Unregister(callback, &result);
    }
}

Int64 SyncStorageEngine::CalculateDefaultFlexTime(
    /* [in] */ Int64 syncTimeSeconds)
{
    if (syncTimeSeconds < DEFAULT_MIN_FLEX_ALLOWED_SECS) {
        // Small enough sync request time that we don't add flex time - developer probably
        // wants to wait for an operation to occur before syncing so we honour the
        // request time.
        return 0;
    }
    else if (syncTimeSeconds < DEFAULT_POLL_FREQUENCY_SECONDS) {
        return (Int64) (syncTimeSeconds * DEFAULT_FLEX_PERCENT_SYNC);
    }
    else {
        // Large enough sync request time that we cap the flex time.
        return (Int64) (DEFAULT_POLL_FREQUENCY_SECONDS * DEFAULT_FLEX_PERCENT_SYNC);
    }
}

void SyncStorageEngine::ReportChange(
    /* [in] */ Int32 which)
{
    AutoPtr< List<AutoPtr<IISyncStatusObserver> > > reports;
    synchronized(mAuthoritiesLock) {
        Int32 i;
        mChangeListeners->BeginBroadcast(&i);
        while (i > 0) {
            i--;
            AutoPtr<IInterface> obj;
            mChangeListeners->GetBroadcastCookie(i, (IInterface**)&obj);
            IInteger32* mask = IInteger32::Probe(obj);
            Int32 ival;
            mask->GetValue(&ival);
            if ((which & ival) == 0) {
                continue;
            }
            if (reports == NULL) {
                reports = new List<AutoPtr<IISyncStatusObserver> >(i);
            }

            obj = NULL;
            mChangeListeners->GetBroadcastItem(i, (IInterface**)&obj);
            AutoPtr<IISyncStatusObserver> sso = IISyncStatusObserver::Probe(obj);
            reports->PushBack(sso);
        }
        mChangeListeners->FinishBroadcast();
    }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ReportChange " + which + " to: " + reports);
    // }

    if (reports != NULL) {
        Int32 i = reports->GetSize();
        while (i > 0) {
            i--;
            // try {
            AutoPtr<IISyncStatusObserver> sso = (*reports)[i];
            sso->OnStatusChanged(which);
            // } catch (RemoteException e) {
            //     // The remote callback list will take care of this for us.
            // }
        }
    }
}

Boolean SyncStorageEngine::GetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName)
{
    synchronized(mAuthoritiesLock) {
        if (account != NULL) {
            AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
            AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(
                ep, String("GetSyncAutomatically"));
            return authority != NULL && authority->mEnabled;
        }

        AuthorityInfo* authorityInfo;
        AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
        // TODO reserse loop
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator rit;
        for (rit = mAuthorities.Begin(); rit != mAuthorities.End(); ++rit) {
            authorityInfo = rit->mSecond;
            if (authorityInfo->mTarget->MatchesSpec(ep) && authorityInfo->mEnabled) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void SyncStorageEngine::SetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName,
    /* [in] */ Boolean sync)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::D(TAG, "SetSyncAutomatically: " +  account + " provider " + providerName
    //             + ", user " + userId + " -> " + sync);
    // }

    synchronized(mAuthoritiesLock) {
        AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
        AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(
             ep, -1 /* ident */, FALSE);
        if (authority->mEnabled == sync) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::D(TAG, "SetSyncAutomatically: already set to " + sync + ", doing nothing");
            // }
            return;
        }
        authority->mEnabled = sync;
        WriteAccountInfoLocked();
    }

    if (sync) {
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        RequestSync(account, userId, SyncOperation::REASON_SYNC_AUTO, providerName, bundle);
    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
}

Int32 SyncStorageEngine::GetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName)
{
    synchronized(mAuthoritiesLock) {
        if (account != NULL) {
            AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
            AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(
                ep, String("get authority syncable"));
            if (authority == NULL) {
                return -1;
            }
            return authority->mSyncable;
        }

        AuthorityInfo* authorityInfo;
        AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
        // TODO reserse loop
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator rit;
        for (rit = mAuthorities.Begin(); rit != mAuthorities.End(); ++rit) {
            authorityInfo = rit->mSecond;
            if (authorityInfo->mTarget != NULL
                    && authorityInfo->mTarget->mProvider.Equals(providerName)) {
                return authorityInfo->mSyncable;
            }
        }
    }
    return -1;
}

void SyncStorageEngine::SetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 syncable)
{
    AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
    SetSyncableStateForEndPoint(ep, syncable);
}

Boolean SyncStorageEngine::GetIsTargetServiceActive(
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId)
{
    synchronized(mAuthoritiesLock) {
        if (cname != NULL) {
            AutoPtr<EndPoint> ep = new EndPoint(cname, userId);
            AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(
                ep, String("get service active"));
            if (authority == NULL) {
                return FALSE;
            }
            return (authority->mSyncable == 1);
        }
    }
    return FALSE;
}

void SyncStorageEngine::SetIsTargetServiceActive(
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean active)
{
    AutoPtr<EndPoint> ep = new EndPoint(cname, userId);
    SetSyncableStateForEndPoint(ep, active ? 1 : 0);
}

void SyncStorageEngine::SetSyncableStateForEndPoint(
    /* [in] */ EndPoint* target,
    /* [in] */ Int32 syncable)
{
    AutoPtr<AuthorityInfo> aInfo;
    synchronized(mAuthoritiesLock) {
        aInfo = GetOrCreateAuthorityLocked(target, -1, FALSE);
        if (syncable > 1) {
            syncable = 1;
        }
        else if (syncable < -1) {
            syncable = -1;
        }
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::D(TAG, "SetIsSyncable: " + aInfo.toString() + " -> " + syncable);
        // }
        if (aInfo->mSyncable == syncable) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::D(TAG, "SetIsSyncable: already set to " + syncable + ", doing nothing");
            // }
            return;
        }
        aInfo->mSyncable = syncable;
        WriteAccountInfoLocked();
    }
    if (syncable > 0) {
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        RequestSync(aInfo, SyncOperation::REASON_IS_SYNCABLE, bundle);
    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
}

AutoPtr< Pair<Int64, Int64> > SyncStorageEngine::GetBackoff(
    /* [in] */ EndPoint* info)
{
    AutoPtr< Pair<Int64, Int64> > result;
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(info, String("GetBackoff"));
        if (authority.Get() != NULL) {
            assert(0 && "TODO");
            result = new Pair<Int64, Int64>(authority->mBackoffTime, authority->mBackoffDelay);
        }
    }
    return result;
}

void SyncStorageEngine::SetBackoff(
    /* [in] */ EndPoint* info,
    /* [in] */ Int64 nextSyncTime,
    /* [in] */ Int64 nextDelay)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "SetBackoff: " + info
    //             + " -> nextSyncTime " + nextSyncTime + ", nextDelay " + nextDelay);
    // }
    Boolean changed;
    synchronized(mAuthoritiesLock) {
        if (info->mTarget_provider
                && (info->mAccount == NULL || info->mProvider == NULL)) {
            // Do more work for a provider sync if the provided info has specified all
            // accounts/providers.
            changed = SetBackoffLocked(
                    info->mAccount /* may be NULL */,
                    info->mUserId,
                    info->mProvider /* may be NULL */,
                    nextSyncTime, nextDelay);
        }
        else {
            AutoPtr<AuthorityInfo> authorityInfo = GetOrCreateAuthorityLocked(info, -1 /* ident */, TRUE);
            if (authorityInfo->mBackoffTime == nextSyncTime && authorityInfo->mBackoffDelay == nextDelay) {
                changed = FALSE;
            }
            else {
                authorityInfo->mBackoffTime = nextSyncTime;
                authorityInfo->mBackoffDelay = nextDelay;
                changed = TRUE;
            }
        }
    }
    if (changed) {
        ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
    }
}

Boolean SyncStorageEngine::SetBackoffLocked(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName,
    /* [in] */ Int64 nextSyncTime,
    /* [in] */ Int64 nextDelay)
{
    Boolean changed = FALSE;
    HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator it;
    HashMap<String, AutoPtr<AuthorityInfo> >::Iterator ait;
    AccountInfo* accountInfo;
    AuthorityInfo* authorityInfo;
    for (it = mAccounts.Begin(); it != mAccounts.End(); ++it) {
        accountInfo = it->mSecond;
        if (account != NULL) {
            AutoPtr<IAccount> act;
            accountInfo->mAccountAndUser->GetAccount((IAccount**)&act);
            Int32 uid;
            accountInfo->mAccountAndUser->GetUserId(&uid);
            if (account != NULL && !Object::Equals(account, act)
                && userId != uid) {
                continue;
            }
        }


        for (ait = accountInfo->mAuthorities.Begin(); ait != accountInfo->mAuthorities.End(); ++ait) {
            authorityInfo = ait->mSecond;

            if (!providerName.IsNull()
                    && !providerName.Equals(authorityInfo->mTarget->mProvider)) {
                continue;
            }
            if (authorityInfo->mBackoffTime != nextSyncTime
                    || authorityInfo->mBackoffDelay != nextDelay) {
                authorityInfo->mBackoffTime = nextSyncTime;
                authorityInfo->mBackoffDelay = nextDelay;
                changed = TRUE;
            }
        }
    }
    return changed;
}

void SyncStorageEngine::ClearAllBackoffsLocked(
    /* [in] */ SyncQueue* syncQueue)
{
    Boolean changed = FALSE;
    synchronized(mAuthoritiesLock) {
        // Clear backoff for all sync adapters.
        AccountInfo* accountInfo;
        AuthorityInfo* authorityInfo;
        IComponentName* service;
        HashMap<Int32, AutoPtr<AuthorityInfo> >* aInfos;
        HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator it;
        HashMap<String, AutoPtr<AuthorityInfo> >::Iterator ait;
        ServiceMapIterator sit;
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt;
        for (it = mAccounts.Begin(); it != mAccounts.End(); ++it) {
            accountInfo = it->mSecond;

            for (ait = accountInfo->mAuthorities.Begin(); ait != accountInfo->mAuthorities.End(); ++ait) {
                authorityInfo = ait->mSecond;

                if (authorityInfo->mBackoffTime != NOT_IN_BACKOFF_MODE
                        || authorityInfo->mBackoffDelay != NOT_IN_BACKOFF_MODE) {
                    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                    //     Log.v(TAG, "ClearAllBackoffsLocked:"
                    //             + " authority:" + authorityInfo->mTarget
                    //             + " account:" + accountInfo->mAccountAndUser->mAccount.name
                    //             + " user:" + accountInfo->mAccountAndUser->mUserId
                    //             + " backoffTime was: " + authorityInfo->mBackoffTime
                    //             + " backoffDelay was: " + authorityInfo->mBackoffDelay);
                    // }
                    authorityInfo->mBackoffTime = NOT_IN_BACKOFF_MODE;
                    authorityInfo->mBackoffDelay = NOT_IN_BACKOFF_MODE;
                    changed = TRUE;
                }
            }
        }
        // Clear backoff for all sync services.
        for (sit = mServices.Begin(); sit != mServices.End(); ++sit) {
            service = sit->mFirst;
            aInfos = sit->mSecond;
            for (authIt = aInfos->Begin(); authIt != aInfos->End(); ++authIt) {
                authorityInfo = authIt->mSecond;
                if (authorityInfo->mBackoffTime != NOT_IN_BACKOFF_MODE
                        || authorityInfo->mBackoffDelay != NOT_IN_BACKOFF_MODE) {
                    authorityInfo->mBackoffTime = NOT_IN_BACKOFF_MODE;
                    authorityInfo->mBackoffDelay = NOT_IN_BACKOFF_MODE;
                }
            }
            syncQueue->ClearBackoffs();
        }
    }

    if (changed) {
        ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
    }
}

Int64 SyncStorageEngine::GetDelayUntilTime(
    /* [in] */ EndPoint* info)
{
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(info, String("getDelayUntil"));
        if (authority != NULL) {
            return authority->mDelayUntil;
        }
    }
    return 0;
}

void SyncStorageEngine::SetDelayUntilTime(
    /* [in] */ EndPoint* info,
    /* [in] */ Int64 delayUntil)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "setDelayUntil: " + info
    //             + " -> delayUntil " + delayUntil);
    // }
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(info, -1, TRUE);
        if (authority->mDelayUntil == delayUntil) {
            return;
        }
        authority->mDelayUntil = delayUntil;
    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
}

void SyncStorageEngine::UpdateOrAddPeriodicSync(
    /* [in] */ EndPoint* info,
    /* [in] */ Int64 period,
    /* [in] */ Int64 flextime,
    /* [in] */ IBundle* extras)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "addPeriodicSync: " + info
    //             + " -> period " + period + ", flex " + flextime + ", extras "
    //             + extras.toString());
    // }
    synchronized(mAuthoritiesLock) {
        if (period <= 0) {
            Logger::E(TAG, "period < 0, should never happen in UpdateOrAddPeriodicSync");
        }
        if (extras == NULL) {
            Logger::E(TAG, "NULL extras, should never happen in UpdateOrAddPeriodicSync:");
        }

        AutoPtr<IPeriodicSync> toUpdate;
        if (info->mTarget_provider) {
            CPeriodicSync::New(info->mAccount,
                info->mProvider, extras, period, flextime, (IPeriodicSync**)&toUpdate);
        }
        else {
            WriteAccountInfoLocked();
            WriteStatusLocked();
            return;
        }

        AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(info, -1, FALSE);
        // add this periodic sync if an equivalent periodic doesn't already exist.
        Boolean alreadyPresent = FALSE;
        Int64 p, ft;
        for (Int32 i = 0, N = authority->mPeriodicSyncs.GetSize(); i < N; i++) {
            IPeriodicSync* syncInfo = authority->mPeriodicSyncs[i];
            AutoPtr<IBundle> b;
            syncInfo->GetExtras((IBundle**)&b);
            if (SyncManager::SyncExtrasEquals(b, extras, TRUE /* includeSyncSettings*/)) {
                syncInfo->GetPeriod(&p);
                syncInfo->GetPeriod(&ft);
                if (period == p && flextime == ft) {
                    // Absolutely the same.
                    WriteAccountInfoLocked();
                    WriteStatusLocked();
                    return;
                }
                authority->mPeriodicSyncs[i] = toUpdate;
                alreadyPresent = TRUE;
                break;
            }
        }

        // If we added an entry to the periodicSyncs array also add an entry to
        // the periodic syncs status to correspond to it.
        if (!alreadyPresent) {
            authority->mPeriodicSyncs.PushBack(toUpdate);
            AutoPtr<ISyncStatusInfo> status = GetOrCreateSyncStatusLocked(authority->mIdent);
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 ms;
            system->GetCurrentTimeMillis(&ms);
            // A new periodic sync is initialised as already having been run.
            status->SetPeriodicSyncTime(authority->mPeriodicSyncs.GetSize() - 1, ms);
        }

        WriteAccountInfoLocked();
        WriteStatusLocked();

    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
}

void SyncStorageEngine::RemovePeriodicSync(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(info, -1, FALSE);
        // Remove any periodic syncs that match the target and extras.
        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it = mSyncStatus.Find(authority->mIdent);
        AutoPtr<ISyncStatusInfo> status = it->mSecond;
        Boolean changed = FALSE;
        List<AutoPtr<IPeriodicSync> >::Iterator iterator;
        Int32 i = 0;
        for (iterator = authority->mPeriodicSyncs.Begin(); iterator != authority->mPeriodicSyncs.End(); ++iterator, ++i) {
            AutoPtr<IPeriodicSync> syncInfo = *iterator;
            AutoPtr<IBundle> b;
            syncInfo->GetExtras((IBundle**)&b);
            if (SyncManager::SyncExtrasEquals(b, extras, TRUE /* includeSyncSettings */)) {
                iterator = authority->mPeriodicSyncs.Erase(iterator);
                changed = TRUE;
                // If we removed an entry from the periodicSyncs array also
                // remove the corresponding entry from the status
                if (status != NULL) {
                    status->RemovePeriodicSyncTime(i);
                }
                else {
                    Logger::E(TAG, "Tried removing sync status on remove periodic sync but did not find it.");
                }
            } else {
                i++;
            }
        }
        if (!changed) {
            WriteAccountInfoLocked();
            WriteStatusLocked();
            return;
        }

        WriteAccountInfoLocked();
        WriteStatusLocked();
    }

    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
}

AutoPtr<List<AutoPtr<IPeriodicSync> > > SyncStorageEngine::GetPeriodicSyncs(
    /* [in] */ EndPoint* info)
{
    AutoPtr<List<AutoPtr<IPeriodicSync> > > syncs = new List<AutoPtr<IPeriodicSync> >();
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authorityInfo = GetAuthorityLocked(info, String("GetPeriodicSyncs"));
        if (authorityInfo != NULL) {
            List<AutoPtr<IPeriodicSync> >::Iterator it;
            IPeriodicSync* item;
            for (it = authorityInfo->mPeriodicSyncs.Begin(); it != authorityInfo->mPeriodicSyncs.End(); ++it) {
                item = *it;
                // Copy and send out. Necessary for thread-safety although it's parceled.
                AutoPtr<IPeriodicSync> sync;
                CPeriodicSync::New(item, (IPeriodicSync**)&sync);
                syncs->PushBack(sync);
            }
        }
    }
    return syncs;
}

void SyncStorageEngine::SetMasterSyncAutomatically(
    /* [in] */ Boolean flag,
    /* [in] */ Int32 userId)
{
    synchronized(mAuthoritiesLock) {
        HashMap<Int32, Boolean>::Iterator it = mMasterSyncAutomatically.Find(userId);
        if (it != mMasterSyncAutomatically.End() && it->mSecond == flag) {
            return;
        }

        mMasterSyncAutomatically[userId] = flag;
        WriteAccountInfoLocked();
    }
    if (flag) {
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        RequestSync(NULL, userId, SyncOperation::REASON_MASTER_SYNC_AUTO, String(NULL), bundle);
    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS);
    AutoPtr<IContentResolverHelper> crHelper;
    CContentResolverHelper::AcquireSingleton((IContentResolverHelper**)&crHelper);
    AutoPtr<IIntent> intent;
    assert(0 && "TODO");
    // crHelper->GetACTION_SYNC_CONN_STATUS_CHANGED((IIntent**)&intent);
    mContext->SendBroadcast(intent);
}

Boolean SyncStorageEngine::GetMasterSyncAutomatically(
    /* [in] */ Int32 userId)
{
    Boolean bval = FALSE;
    synchronized(mAuthoritiesLock) {
        HashMap<Int32, Boolean>::Iterator it = mMasterSyncAutomatically.Find(userId);
        if (it != mMasterSyncAutomatically.End()) {
            bval = it->mSecond;
        }
        else {
            bval = mDefaultMasterSyncAutomatically;
        }

    }
    return bval;
}

AutoPtr<AuthorityInfo> SyncStorageEngine::GetAuthority(
    /* [in] */ Int32 authorityId)
{
    AutoPtr<AuthorityInfo> ai;
    synchronized(mAuthoritiesLock) {
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it = mAuthorities.Find(authorityId);
        if (it != mAuthorities.End()) {
            ai = it->mSecond;
        }
    }
    return ai;
}

Boolean SyncStorageEngine::IsSyncActive(
    /* [in] */ EndPoint* info)
{
    synchronized(mAuthoritiesLock) {
        AutoPtr<List<AutoPtr<ISyncInfo> > > list = GetCurrentSyncs(info->mUserId);
        List<AutoPtr<ISyncInfo> >::Iterator it;
        Int32 id;
        for (it = list->Begin(); it != list->End(); ++it) {
            ISyncInfo* syncInfo = *it;
            syncInfo->GetAuthorityId(&id);
            AutoPtr<AuthorityInfo> ainfo = GetAuthority(id);
            if (ainfo != NULL && ainfo->mTarget->MatchesSpec(info)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

AutoPtr<PendingOperation> SyncStorageEngine::InsertIntoPending(
    /* [in] */ SyncOperation* op)
{
    AutoPtr<PendingOperation> pop;
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "InsertIntoPending: authority=" + op->mTarget
        //             + " extras=" + op->mExtras);
        // }
        AutoPtr<EndPoint> info = op->mTarget;
        AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(info,
            -1 /* desired identifier */, TRUE /* write accounts to storage */);
        if (authority == NULL) {
            return NULL;
        }

        pop = new PendingOperation(authority, op->mReason, op->mSyncSource, op->mExtras, op->IsExpedited());
        mPendingOperations.PushBack(pop);
        AppendPendingOperationLocked(pop);

        AutoPtr<ISyncStatusInfo> status = GetOrCreateSyncStatusLocked(authority->mIdent);
        status->SetPending(TRUE);
    }
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_PENDING);
    return pop;
}

Boolean SyncStorageEngine::DeleteFromPending(
    /* [in] */ PendingOperation* op)
{
    Boolean res = FALSE;
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "DeleteFromPending: account=" + op.toString());
        // }
        AutoPtr<PendingOperation> tmp = op;
        List<AutoPtr<PendingOperation> >::Iterator it;
        it = Find(mPendingOperations.Begin(), mPendingOperations.End(), tmp);
        if (it != mPendingOperations.End()) {
            mPendingOperations.Erase(it);

            if (mPendingOperations.GetSize() == 0
                    || mNumPendingFinished >= PENDING_FINISH_TO_WRITE) {
                WritePendingOperationsLocked();
                mNumPendingFinished = 0;
            }
            else {
                mNumPendingFinished++;
            }

            AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(op->mTarget, String("DeleteFromPending"));
            if (authority != NULL) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, "removing - " + authority.toString());
                // }
                Int32 N = mPendingOperations.GetSize();
                Boolean morePending = FALSE;
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<PendingOperation> cur = mPendingOperations[i];
                    if (Object::Equals(cur, op)) {
                        morePending = TRUE;
                        break;
                    }
                }

                if (!morePending) {
                    // if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "no more pending!");
                    AutoPtr<ISyncStatusInfo> status = GetOrCreateSyncStatusLocked(authority->mIdent);
                    status->SetPending(FALSE);
                }
            }
            res = TRUE;
        }
    }

    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_PENDING);
    return res;
}

AutoPtr<List<AutoPtr<PendingOperation> > > SyncStorageEngine::GetPendingOperations()
{
    AutoPtr<List<AutoPtr<PendingOperation> > > list;
    synchronized(mAuthoritiesLock) {
        list = new List<AutoPtr<PendingOperation> >(mPendingOperations);
    }
    return list;
}

Int32 SyncStorageEngine::GetPendingOperationCount()
{
    Int32 size = 0;
    synchronized(mAuthoritiesLock) {
        size = mPendingOperations.GetSize();
    }
    return size;
}

static Boolean Contains(
    /* [in] */ ArrayOf<IAccount*>* accounts,
    /* [in] */ IAccount* account)
{
    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        if (Object::Equals(account, (*accounts)[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

void SyncStorageEngine::DoDatabaseCleanup(
    /* [in] */ ArrayOf<IAccount*>* accounts,
    /* [in] */ Int32 userId)
{
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "Updating for new accounts...");
        // }
        HashMap<Int32, AutoPtr<AuthorityInfo> > removing;
        HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator accIt;
        HashMap<String, AutoPtr<AuthorityInfo> >::Iterator authIt;
        for (accIt = mAccounts.Begin(); accIt != mAccounts.End();) {
            AutoPtr<AccountInfo> acc = accIt->mSecond;
            AutoPtr<IAccount> account;
            acc->mAccountAndUser->GetAccount((IAccount**)&account);
            Int32 id;
            acc->mAccountAndUser->GetUserId(&id);
            if (!Contains(accounts, account)  && id == userId) {
                // This account no longer exists...
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, "Account removed: " + acc->mAccountAndUser);
                // }
                for (authIt = acc->mAuthorities.Begin(); authIt != acc->mAuthorities.End(); ++authIt) {
                    AutoPtr<AuthorityInfo> auth = authIt->mSecond;
                    removing[auth->mIdent] = auth;
                }

                mAccounts.Erase(accIt++);
            }
            else {
                ++accIt;
            }
        }

        // Clean out all data structures.
        // TODO reserse loop
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it;
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator rit;
        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator syncRit;
        List< AutoPtr<SyncHistoryItem> >::ReverseIterator histRit;
        for (rit = removing.Begin(); rit != removing.End(); ++rit) {
            Int32 ident = rit->mFirst;

            it = mAuthorities.Find(ident);
            if (it != mAuthorities.End()) {
                mAuthorities.Erase(it);
            }

            for (syncRit = mSyncStatus.Begin(); syncRit != mSyncStatus.End(); ) {
                if (syncRit->mFirst == ident) {
                    mSyncStatus.Erase(syncRit++);
                }
                else {
                    ++syncRit;
                }
            }

            for (histRit = mSyncHistory.RBegin(); histRit != mSyncHistory.REnd();) {
                if ((*histRit)->mAuthorityId == ident) {
                    histRit = List< AutoPtr<SyncHistoryItem> >::ReverseIterator(mSyncHistory.Erase(--(histRit.GetBase())));
                }
                else {
                    ++histRit;
                }
            }
        }

        WriteAccountInfoLocked();
        WriteStatusLocked();
        WritePendingOperationsLocked();
        WriteStatisticsLocked();
    }
}

AutoPtr<ISyncInfo> SyncStorageEngine::AddActiveSync(
    /* [in] */ IActiveSyncContext* activeSyncContextObj)
{
    SyncManager::ActiveSyncContext* activeSyncContext = (SyncManager::ActiveSyncContext*)activeSyncContextObj;
    AutoPtr<ISyncInfo> syncInfo;
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "setActiveSync: account="
        //         + " auth=" + activeSyncContext.mSyncOperation->mTarget
        //         + " src=" + activeSyncContext.mSyncOperation.syncSource
        //         + " extras=" + activeSyncContext.mSyncOperation->mExtras);
        // }
        AutoPtr<EndPoint> info = activeSyncContext->mSyncOperation->mTarget;
        AutoPtr<AuthorityInfo> authorityInfo = GetOrCreateAuthorityLocked(
            info,
            -1 /* assign a new identifier if creating a new target */,
            TRUE /* write to storage if this results in a change */);
        CSyncInfo::New(
            authorityInfo->mIdent,
            authorityInfo->mTarget->mAccount,
            authorityInfo->mTarget->mProvider,
            activeSyncContext->mStartTime,
            (ISyncInfo**)&syncInfo);

        AutoPtr<List<AutoPtr<ISyncInfo> > > list = GetCurrentSyncs(authorityInfo->mTarget->mUserId);
        list->PushBack(syncInfo);
    }
    ReportActiveChange();
    return syncInfo;
}

void SyncStorageEngine::RemoveActiveSync(
    /* [in] */ ISyncInfo* syncInfo,
    /* [in] */ Int32 userId)
{
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "RemoveActiveSync: account=" + syncInfo->mAccount
        //             + " user=" + userId
        //             + " auth=" + syncInfo.authority);
        // }

        AutoPtr<List<AutoPtr<ISyncInfo> > > list = GetCurrentSyncs(userId);
        AutoPtr<ISyncInfo> obj = syncInfo;
        List<AutoPtr<ISyncInfo> >::Iterator it = Find(list->Begin(), list->End(), obj);
        if (it != list->End()) {
            list->Erase(it);
        }
    }

    ReportActiveChange();
}

void SyncStorageEngine::ReportActiveChange()
{
    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_ACTIVE);
}

Int64 SyncStorageEngine::InsertStartSyncEvent(
    /* [in] */ SyncOperation* op,
    /* [in] */ Int64 now)
{
    Int64 id;
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "InsertStartSyncEvent: " + op);
        // }
        AutoPtr<AuthorityInfo> authority = GetAuthorityLocked(op->mTarget, String("InsertStartSyncEvent"));
        if (authority == NULL) {
            return -1;
        }
        AutoPtr<SyncHistoryItem> item = new SyncHistoryItem();
        item->mInitialization = op->IsInitialization();
        item->mAuthorityId = authority->mIdent;
        item->mHistoryId = mNextHistoryId++;
        if (mNextHistoryId < 0) mNextHistoryId = 0;
        item->mEventTime = now;
        item->mSource = op->mSyncSource;
        item->mReason = op->mReason;
        item->mExtras = op->mExtras;
        item->mEvent = EVENT_START;
        mSyncHistory.PushFront(item);
        while (mSyncHistory.GetSize() > MAX_HISTORY) {
            mSyncHistory.PopBack();
        }
        id = item->mHistoryId;
        // if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "returning historyId " + id);
    }

    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_STATUS);
    return id;
}

void SyncStorageEngine::StopSyncEvent(
    /* [in] */ Int64 historyId,
    /* [in] */ Int64 elapsedTime,
    /* [in] */ const String& resultMessage,
    /* [in] */ Int64 downstreamActivity,
    /* [in] */ Int64 upstreamActivity)
{
    synchronized(mAuthoritiesLock) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "StopSyncEvent: historyId=" + historyId);
        // }
        AutoPtr<SyncHistoryItem> item;
        List< AutoPtr<SyncHistoryItem> >::ReverseIterator histRit;
        for (histRit = mSyncHistory.RBegin(); histRit != mSyncHistory.REnd();) {
            if ((*histRit)->mAuthorityId == historyId) {
                item = (*histRit);
                break;
            }
        }
        if (item == NULL) {
            Logger::W(TAG, "StopSyncEvent: no history for id %lld", historyId);
            return;
        }

        item->mElapsedTime = elapsedTime;
        item->mEvent = EVENT_STOP;
        item->mMsg = resultMessage;
        item->mDownstreamActivity = downstreamActivity;
        item->mUpstreamActivity = upstreamActivity;

        AutoPtr<ISyncStatusInfo> status = GetOrCreateSyncStatusLocked(item->mAuthorityId);
        Int32 ival;
        Int64 lval;
        status->GetNumSyncs(&ival);
        status->SetNumSyncs(ival + 1);
        status->GetTotalElapsedTime(&lval);
        status->SetTotalElapsedTime(lval + elapsedTime);

        switch (item->mSource) {
            case SOURCE_LOCAL:
                status->GetNumSourceLocal(&ival);
                status->SetNumSourceLocal(ival + 1);
                break;
            case SOURCE_POLL:
                status->GetNumSourcePoll(&ival);
                status->SetNumSourcePoll(ival + 1);
                break;
            case SOURCE_USER:
                status->GetNumSourceUser(&ival);
                status->SetNumSourceUser(ival + 1);
                break;
            case SOURCE_SERVER:
                status->GetNumSourceServer(&ival);
                status->SetNumSourceServer(ival + 1);
                break;
            case SOURCE_PERIODIC:
                status->GetNumSourcePeriodic(&ival);
                status->SetNumSourcePeriodic(ival + 1);
                break;
        }

        Boolean writeStatisticsNow = FALSE;
        Int32 day = GetCurrentDayLocked();
        if ((*mDayStats)[0] == NULL) {
            AutoPtr<DayStats> ds = new DayStats(day);
            mDayStats->Set(0, ds);
        }
        else if (day != (*mDayStats)[0]->mDay) {
            mDayStats->Copy(1, mDayStats, 0, mDayStats->GetLength() - 1);
            AutoPtr<DayStats> ds = new DayStats(day);
            mDayStats->Set(0, ds);
            writeStatisticsNow = TRUE;
        }
        else if ((*mDayStats)[0] == NULL) {
        }

        AutoPtr<DayStats> ds = (*mDayStats)[0];

        Int64 lastSyncTime = (item->mEventTime + elapsedTime);
        Boolean writeStatusNow = FALSE;
        if (MESG_SUCCESS.Equals(resultMessage)) {
            // - if successful, update the successful columns
            Int64 lval2;
            status->GetLastSuccessTime(&lval);
            status->GetLastFailureTime(&lval2);
            if (lval == 0 || lval2 != 0) {
                writeStatusNow = TRUE;
            }
            status->SetLastSuccessTime(lastSyncTime);
            status->SetLastSuccessSource(item->mSource);
            status->SetLastFailureTime(0);
            status->SetLastFailureSource(-1);
            status->SetLastFailureMesg(String(NULL));
            status->SetInitialFailureTime(0);
            ds->mSuccessCount++;
            ds->mSuccessTime += elapsedTime;
        }
        else if (!MESG_CANCELED.Equals(resultMessage)) {
            status->GetLastFailureTime(&lval);
            if (lval == 0) {
                writeStatusNow = TRUE;
            }
            status->SetLastFailureTime(lastSyncTime);
            status->SetLastFailureSource(item->mSource);
            status->SetLastFailureMesg(resultMessage);

            status->GetInitialFailureTime(&lval);
            if (lval == 0) {
                status->SetInitialFailureTime(lastSyncTime);
            }
            ds->mFailureCount++;
            ds->mFailureTime += elapsedTime;
        }

        Boolean bval, bval1, bval2;
        HasMessages(MSG_WRITE_STATUS, &bval1);
        HasMessages(MSG_WRITE_STATISTICS, &bval2);
        if (writeStatusNow) {
            WriteStatusLocked();
        }
        else if (!bval1) {
            AutoPtr<IMessage> msg;
            ObtainMessage(MSG_WRITE_STATUS, (IMessage**)&msg);
            SendMessageDelayed(msg, WRITE_STATUS_DELAY, &bval);
        }

        if (writeStatisticsNow) {
            WriteStatisticsLocked();
        }
        else if (!bval2) {
            AutoPtr<IMessage> msg;
            ObtainMessage(MSG_WRITE_STATISTICS, (IMessage**)&msg);
            SendMessageDelayed(msg, WRITE_STATISTICS_DELAY, &bval);
        }
    }

    ReportChange(IContentResolver::SYNC_OBSERVER_TYPE_STATUS);
}

AutoPtr<List<AutoPtr<ISyncInfo> > > SyncStorageEngine::GetCurrentSyncs(
    /* [in] */ Int32 userId)
{
    AutoPtr<List<AutoPtr<ISyncInfo> > > list;
    synchronized(mAuthoritiesLock) {
        list = GetCurrentSyncsLocked(userId);
    }
    return list;
}

AutoPtr<IList> SyncStorageEngine::GetCurrentSyncsCopy(
    /* [in] */ Int32 userId)
{
    AutoPtr<IList> syncsCopy;
    CArrayList::New((IList**)&syncsCopy);

    synchronized(mAuthoritiesLock) {
        AutoPtr<List<AutoPtr<ISyncInfo> > > syncs = GetCurrentSyncsLocked(userId);
        List<AutoPtr<ISyncInfo> >::Iterator it;
        for (it = syncs->Begin(); it != syncs->End(); ++it) {
            AutoPtr<ISyncInfo> si;
            CSyncInfo::New((*it).Get(), (ISyncInfo**)&si);
            syncsCopy->Add(TO_IINTERFACE(si));
        }
    }
    return syncsCopy;
}

AutoPtr<List<AutoPtr<ISyncInfo> > > SyncStorageEngine::GetCurrentSyncsLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<List<AutoPtr<ISyncInfo> > > syncs;
    HashMap<Int32, AutoPtr<List<AutoPtr<ISyncInfo> > > >::Iterator it;
    it = mCurrentSyncs.Find(userId);
    if (it != mCurrentSyncs.End()) {
        syncs = it->mSecond;
        if (syncs == NULL) {
            syncs = new List<AutoPtr<ISyncInfo> >();
            mCurrentSyncs[userId] = syncs;
        }
    }

    return syncs;
}

AutoPtr<List<AutoPtr<ISyncStatusInfo> > > SyncStorageEngine::GetSyncStatus()
{
    AutoPtr<List<AutoPtr<ISyncStatusInfo> > > ops;
    synchronized(mAuthoritiesLock) {
        Int32 N = mSyncStatus.GetSize();
        ops = new List<AutoPtr<ISyncStatusInfo> >(N);

        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it;
        for (it = mSyncStatus.Begin(); it != mSyncStatus.End(); ++it) {
            ops->PushBack(it->mSecond);
        }
    }
    return ops;
}

AutoPtr<AuthoritySyncStatusPair> SyncStorageEngine::GetCopyOfAuthorityWithSyncStatus(
    /* [in] */ EndPoint* info)
{
    synchronized(mAuthoritiesLock) {
        AutoPtr<AuthorityInfo> authorityInfo = GetOrCreateAuthorityLocked(
            info,
            -1 /* assign a new identifier if creating a new target */,
            TRUE /* write to storage if this results in a change */);
        return CreateCopyPairOfAuthorityWithSyncStatusLocked(authorityInfo);
    }
    return NULL;
}

AutoPtr<List<AutoPtr<AuthoritySyncStatusPair> > > SyncStorageEngine::GetCopyOfAllAuthoritiesWithSyncStatus()
{
    AutoPtr<List<AutoPtr<AuthoritySyncStatusPair> > > infos;
    synchronized(mAuthoritiesLock) {
        infos = new List<AutoPtr<AuthoritySyncStatusPair> >(mAuthorities.GetSize());
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it;
        for (it = mAuthorities.Begin(); it != mAuthorities.End(); ++it) {
            AutoPtr<AuthoritySyncStatusPair> pair = CreateCopyPairOfAuthorityWithSyncStatusLocked(it->mSecond);
            infos->PushBack(pair);
        }
    }
    return infos;
}

AutoPtr<ISyncStatusInfo> SyncStorageEngine::GetStatusByAuthority(
    /* [in] */ EndPoint* info)
{
    if (info->mTarget_provider && (info->mAccount == NULL || info->mProvider == NULL)) {
        return NULL;
    }
    else if (info->mTarget_service && info->mService == NULL) {
        return NULL;
    }

    synchronized(mAuthoritiesLock) {
        ISyncStatusInfo* cur;
        AutoPtr<AuthorityInfo> ainfo;
        Int32 ival;
        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it;
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt;
        for (it = mSyncStatus.Begin(); it != mSyncStatus.End(); ++it) {
            cur = it->mSecond;
            cur->GetAuthorityId(&ival);
            authIt = mAuthorities.Find(ival);
            if (authIt != mAuthorities.End()) {
                ainfo = authIt->mSecond;
                if (ainfo.Get() != NULL && ainfo->mTarget->MatchesSpec(info)) {
                    return cur;
                }
            }
        }
    }
    return NULL;
}

Boolean SyncStorageEngine::IsSyncPending(
    /* [in] */ EndPoint* info)
{
    synchronized(mAuthoritiesLock) {
        ISyncStatusInfo* cur;
        Int32 ival;
        Boolean bval;
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt;
        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it;
        for (it = mSyncStatus.Begin(); it != mSyncStatus.End(); ++it) {
            cur = it->mSecond;
            cur->GetAuthorityId(&ival);
            authIt = mAuthorities.Find(ival);
            if (authIt == mAuthorities.End()) {
                continue;
            }

            AutoPtr<AuthorityInfo> ainfo = authIt->mSecond;
            if (ainfo == NULL) {
                continue;
            }
            if (!ainfo->mTarget->MatchesSpec(info)) {
                continue;
            }
            cur->GetPending(&bval);
            if (bval) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

AutoPtr<List<AutoPtr<SyncHistoryItem> > > SyncStorageEngine::GetSyncHistory()
{
    AutoPtr<List<AutoPtr<SyncHistoryItem> > > items;
    synchronized(mAuthoritiesLock) {
        items = new List<AutoPtr<SyncHistoryItem> >(mSyncHistory);
    }
    return items;
}

AutoPtr<ArrayOf<DayStats*> > SyncStorageEngine::GetDayStatistics()
{
    AutoPtr<ArrayOf<DayStats*> > ds;
    synchronized(mAuthoritiesLock) {
        ds = mDayStats->Clone();
    }
    return ds;
}

AutoPtr<AuthoritySyncStatusPair> SyncStorageEngine::CreateCopyPairOfAuthorityWithSyncStatusLocked(
    /* [in] */ AuthorityInfo* authorityInfo)
{
    AutoPtr<ISyncStatusInfo> syncStatusInfo = GetOrCreateSyncStatusLocked(authorityInfo->mIdent);
    AutoPtr<AuthorityInfo> ai = new AuthorityInfo(authorityInfo);
    AutoPtr<ISyncStatusInfo> ssi;
    CSyncStatusInfo::New(syncStatusInfo, (ISyncStatusInfo**)&ssi);

    AutoPtr<AuthoritySyncStatusPair> pair = new AuthoritySyncStatusPair(ai, ssi);
    return pair;
}

Int32 SyncStorageEngine::GetCurrentDayLocked()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 ms;
    system->GetCurrentTimeMillis(&ms);
    mCal->SetTimeInMillis(ms);
    Int32 dayOfYear;
    mCal->Get(ICalendar::DAY_OF_YEAR, &dayOfYear);
    Int32 year;
    mCal->Get(ICalendar::YEAR, &year);

    if (mYear != year) {
        mYear =year;
        mCal->Clear();
        mCal->Set(ICalendar::YEAR, mYear);
        mCal->GetTimeInMillis(&ms);
        mYearInDays = (Int32)(ms/86400000);
    }
    return dayOfYear + mYearInDays;
}

AutoPtr<AuthorityInfo> SyncStorageEngine::GetAuthorityLocked(
    /* [in] */ EndPoint* info,
    /* [in] */ const String& tag)
{
    if (info->mTarget_service) {
        ServiceMapIterator it = mServices.Find(info->mService);
        AutoPtr<HashMap<Int32, AutoPtr<AuthorityInfo> > > aInfo;
        if (it != mServices.End()) {
            aInfo = it->mSecond;
        }

        AutoPtr<AuthorityInfo> authority;
        if (aInfo != NULL) {
            HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it = aInfo->Find(info->mUserId);
            if (it != aInfo->End()) {
                authority = it->mSecond;
            }
        }

        if (authority == NULL) {
            if (tag != NULL) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, tag + " No authority info found for " + info->mService + " for"
                //             + " user " + info->mUserId);
                // }
            }
            return NULL;
        }
        return authority;
    }
    else if (info->mTarget_provider){
        AutoPtr<IAccountAndUser> au;
        assert(0 && "TODO");
        // CAccountAndUser::New(info->mAccount, info->mUserId, (IAccountAndUser**)&au);
        HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator it = mAccounts.Find(au);
        AutoPtr<AccountInfo> accountInfo;
        if (it != mAccounts.End()) {
            accountInfo = it->mSecond;
        }
        if (accountInfo == NULL) {
            if (tag != NULL) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, tag + ": unknown account " + au);
                // }
            }
            return NULL;
        }

        HashMap<String, AutoPtr<AuthorityInfo> >::Iterator aiIt = accountInfo->mAuthorities.Find(info->mProvider);
        AutoPtr<AuthorityInfo> authority;
        if (aiIt != accountInfo->mAuthorities.End()) {
            authority = aiIt->mSecond;
        }
        if (authority == NULL) {
            if (tag != NULL) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, tag + ": unknown provider " + info->mProvider);
                // }
            }
            return NULL;
        }
        return authority;
    }

    Logger::E(TAG, "%s Authority : %s, invalid target",
        tag.string(), Object::ToString(info).string());
    return NULL;
}

AutoPtr<AuthorityInfo> SyncStorageEngine::GetOrCreateAuthorityLocked(
    /* [in] */ EndPoint* info,
    /* [in] */ Int32 ident,
    /* [in] */ Boolean doWrite)
{
    AutoPtr<AuthorityInfo> authority;
    if (info->mTarget_service) {
        ServiceMapIterator it = mServices.Find(info->mService);
        AutoPtr<HashMap<Int32, AutoPtr<AuthorityInfo> > > aInfo;
        if (it == mServices.End()) {
            aInfo = new HashMap<Int32, AutoPtr<AuthorityInfo> >();
            mServices[info->mService] = aInfo;
        }

        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator aiIt = aInfo->Find(info->mUserId);
        if (aiIt != aInfo->End()) {
            authority = aiIt->mSecond;
        }
        if (authority == NULL) {
            authority = CreateAuthorityLocked(info, ident, doWrite);
            (*aInfo)[info->mUserId] = authority;
        }
    }
    else if (info->mTarget_provider) {
        AutoPtr<IAccountAndUser> au;
        assert(0 && "TODO");
        // CAccountAndUser::New(info->mAccount, info->mUserId, (IAccountAndUser**)&au);

        AutoPtr<AccountInfo> account;
        HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator it = mAccounts.Find(au);
        if (it != mAccounts.End()) {
            account = it->mSecond;
        }

        if (account == NULL) {
            account = new AccountInfo(au);
            mAccounts[au] = account;
        }

        HashMap<String, AutoPtr<AuthorityInfo> >::Iterator aiIt = account->mAuthorities.Find(info->mProvider);
        if (aiIt != account->mAuthorities.End()) {
            authority = aiIt->mSecond;
        }
        if (authority == NULL) {
            authority = CreateAuthorityLocked(info, ident, doWrite);
            account->mAuthorities[info->mProvider] = authority;
        }
    }
    return authority;
}

AutoPtr<AuthorityInfo> SyncStorageEngine::CreateAuthorityLocked(
    /* [in] */ EndPoint* info,
    /* [in] */ Int32 ident,
    /* [in] */ Boolean doWrite)
{
    AutoPtr<AuthorityInfo> authority;
    if (ident < 0) {
        ident = mNextAuthorityId;
        mNextAuthorityId++;
        doWrite = TRUE;
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "created a new AuthorityInfo for " + info);
    // }
    authority = new AuthorityInfo(info, ident);
    mAuthorities[ident] = authority;
    if (doWrite) {
        WriteAccountInfoLocked();
    }
    return authority;
}

void SyncStorageEngine::RemoveAuthority(
    /* [in] */ EndPoint* info)
{
    synchronized(mAuthoritiesLock) {
        if (info->mTarget_provider) {
            RemoveAuthorityLocked(info->mAccount, info->mUserId, info->mProvider, TRUE /* doWrite */);
        }
        else {
            ServiceMapIterator it = mServices.Find(info->mService);

            if (it != mServices.End()) {
                AutoPtr<HashMap<Int32, AutoPtr<AuthorityInfo> > > aInfos = it->mSecond;
                if (aInfos != NULL) {
                    HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authId = aInfos->Find(info->mUserId);
                    if (authId != aInfos->End()) {
                        AutoPtr<AuthorityInfo> authorityInfo = authId->mSecond;
                        if (authorityInfo != NULL) {
                            HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator ait;
                            ait = mAuthorities.Find(authorityInfo->mIdent);
                            if (ait != mAuthorities.End()) {
                                mAuthorities.Erase(ait);
                            }

                            aInfos->Erase(authId);
                            WriteAccountInfoLocked();
                        }
                    }
                }
            }
        }
    }
}

void SyncStorageEngine::RemoveAuthorityLocked(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& authorityName,
    /* [in] */ Boolean doWrite)
{
    AutoPtr<IAccountAndUser> aau;
    assert(0 && "TODO");
    // CAccountAndUser::New(account, userId, (IAccountAndUser**)&aau);
    HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> >::Iterator accIt;
    accIt = mAccounts.Find(aau);
    AutoPtr<AccountInfo> accountInfo;
    if (accIt != mAccounts.End()) {
        accountInfo = accIt->mSecond;
    }

    if (accountInfo != NULL) {
        HashMap<String, AutoPtr<AuthorityInfo> >::Iterator authIt = accountInfo->mAuthorities.Find(authorityName);
        AutoPtr<AuthorityInfo> authorityInfo;
        if (authIt != accountInfo->mAuthorities.End()) {
            authorityInfo = authIt->mSecond;
            accountInfo->mAuthorities.Erase(authIt);
        }

        if (authorityInfo != NULL) {
            HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it = mAuthorities.Find(authorityInfo->mIdent);
            if (it != mAuthorities.End()) {
                mAuthorities.Erase(it);
            }

            if (doWrite) {
                WriteAccountInfoLocked();
            }
        }
    }
}

void SyncStorageEngine::SetPeriodicSyncTime(
    /* [in] */ Int32 authorityId,
    /* [in] */ IPeriodicSync* targetPeriodicSync,
    /* [in] */ Int64 when)
{
    Boolean found = FALSE;
    AutoPtr<AuthorityInfo> authorityInfo;
    synchronized(mAuthoritiesLock) {
        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it = mAuthorities.Find(authorityId);
        if (it != mAuthorities.End()) {
            authorityInfo = it->mSecond;
        }

        IPeriodicSync* periodicSync;
        List<AutoPtr<IPeriodicSync> >::Iterator psIt;
        HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator ssiIt;
        Int32 i = 0;
        for (psIt = authorityInfo->mPeriodicSyncs.Begin(); psIt != authorityInfo->mPeriodicSyncs.End(); ++psIt) {
            periodicSync = *psIt;
            if (Object::Equals(targetPeriodicSync, periodicSync)) {
                ssiIt = mSyncStatus.Find(authorityId);
                assert(ssiIt != mSyncStatus.End());
                ssiIt->mSecond->SetPeriodicSyncTime(i++, when);
                found = TRUE;
                break;
            }
        }
    }
    if (!found) {
        Logger::W(TAG, "Ignoring SetPeriodicSyncTime request for a sync that does not exist. Authority: %s",
            Object::ToString(authorityInfo->mTarget).string());
    }
}

AutoPtr<ISyncStatusInfo> SyncStorageEngine::GetOrCreateSyncStatusLocked(
    /* [in] */ Int32 authorityId)
{
    HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it = mSyncStatus.Find(authorityId);
    AutoPtr<ISyncStatusInfo> status;
    if (it != mSyncStatus.End()) {
        status = it->mSecond;
    }

    if (status == NULL) {
        CSyncStatusInfo::New(authorityId, (ISyncStatusInfo**)&status);
        mSyncStatus[authorityId] = status;
    }
    return status;
}

void SyncStorageEngine::WriteAllState()
{
    synchronized(mAuthoritiesLock) {
        // Account info is always written so no need to do it here.

        if (mNumPendingFinished > 0) {
            // Only write these if they are out of date.
            WritePendingOperationsLocked();
        }

        // Just always write these...  they are likely out of date.
        WriteStatusLocked();
        WriteStatisticsLocked();
    }
}

void SyncStorageEngine::ClearAndReadState()
{
    synchronized(mAuthoritiesLock) {
        mAuthorities.Clear();
        mAccounts.Clear();
        mServices.Clear();
        mPendingOperations.Clear();
        mSyncStatus.Clear();
        mSyncHistory.Clear();

        ReadAccountInfoLocked();
        ReadStatusLocked();
        ReadPendingOperationsLocked();
        ReadStatisticsLocked();
        ReadAndDeleteLegacyAccountInfoLocked();
        WriteAccountInfoLocked();
        WriteStatusLocked();
        WritePendingOperationsLocked();
        WriteStatisticsLocked();
    }
}

void SyncStorageEngine::ReadAccountInfoLocked()
{
    Int32 highestAuthorityId = -1;
    AutoPtr<IFileInputStream> fis;
    AutoPtr<IXmlPullParser> parser;
    Int32 eventType, version, id, depth;
    String tagName, listen, versionString, nextIdString, offsetString;
    AutoPtr<AuthorityInfo> authority;
    AutoPtr<IPeriodicSync> periodicSync;

    ECode ec = NOERROR;
    String nullStr;
    ec = mAccountInfoFile->OpenRead((IFileInputStream**)&fis);
    FAIL_GOTO(ec, _EXIT_)

    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG_FILE, "Reading " + mAccountInfoFile.getBaseFile());
    // }
    FAIL_GOTO(Xml::NewPullParser((IXmlPullParser**)&parser), _EXIT_)

    FAIL_GOTO(parser->SetInput(IInputStream::Probe(fis), String(NULL)), _EXIT_)
    FAIL_GOTO(parser->GetEventType(&eventType), _EXIT_)
    while (eventType != IXmlPullParser::START_TAG &&
            eventType != IXmlPullParser::END_DOCUMENT) {
        FAIL_GOTO(parser->Next(&eventType), _EXIT_)
    }
    if (eventType == IXmlPullParser::END_DOCUMENT) {
        Logger::I(TAG, "No initial accounts");
        return;
    }

    FAIL_GOTO(parser->GetName(&tagName), _EXIT_)

    if (tagName.Equals("accounts")) {
        FAIL_GOTO(parser->GetAttributeValue(nullStr, XML_ATTR_LISTEN_FOR_TICKLES, &listen), _EXIT_)
        FAIL_GOTO(parser->GetAttributeValue(nullStr, String("version"), &versionString), _EXIT_)

        version = versionString.IsNull() ? 0 : StringUtils::ParseInt32(versionString);

        FAIL_GOTO(parser->GetAttributeValue(nullStr, XML_ATTR_NEXT_AUTHORITY_ID, &nextIdString), _EXIT_)

        id = nextIdString.IsNull() ? 0 : StringUtils::ParseInt32(nextIdString);
        mNextAuthorityId = Elastos::Core::Math::Max(mNextAuthorityId, id);

        FAIL_GOTO(parser->GetAttributeValue(nullStr, XML_ATTR_SYNC_RANDOM_OFFSET, &offsetString), _EXIT_)
        mSyncRandomOffset = offsetString.IsNull() ? 0 : StringUtils::ParseInt32(offsetString);

        if (mSyncRandomOffset == 0) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 ms;
            system->GetCurrentTimeMillis(&ms);
            AutoPtr<IRandom> random;
            CRandom::New(ms, (IRandom**)&random);
            random->NextInt32(86400, &mSyncRandomOffset);
        }

        mMasterSyncAutomatically[0] = listen.IsNull() || StringUtils::ParseBoolean(listen);
        FAIL_GOTO(parser->Next(&eventType), _EXIT_)

        do {
            if (eventType == IXmlPullParser::START_TAG) {
                FAIL_GOTO(parser->GetName(&tagName), _EXIT_)
                FAIL_GOTO(parser->GetDepth(&depth), _EXIT_)
                if (depth == 2) {
                    if (tagName.Equals("authority")) {
                        authority = ParseAuthority(parser, version);
                        periodicSync = NULL;
                        if (authority->mIdent > highestAuthorityId) {
                            highestAuthorityId = authority->mIdent;
                        }
                    }
                    else if (XML_TAG_LISTEN_FOR_TICKLES.Equals(tagName)) {
                        ParseListenForTickles(parser);
                    }
                }
                else if (depth == 3) {
                    if (tagName.Equals("periodicSync") && authority != NULL) {
                        periodicSync = ParsePeriodicSync(parser, authority);
                    }
                }
                else if (depth == 4 && periodicSync != NULL) {
                    if (tagName.Equals("extra")) {
                        AutoPtr<IBundle> b;
                        periodicSync->GetExtras((IBundle**)&b);
                        ParseExtra(parser, b);
                    }
                }
            }
            FAIL_GOTO(parser->Next(&eventType), _EXIT_)
        }
        while (eventType != IXmlPullParser::END_DOCUMENT);
    }

_EXIT_:

    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG, "Error reading accounts: E_XML_PULL_PARSER_EXCEPTION");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        if (fis == NULL) Logger::I(TAG, "No initial accounts");
        else Logger::W(TAG, "Error reading accounts: E_IO_EXCEPTION");

        if (fis != NULL) {
            ICloseable::Probe(fis)->Close();
        }
    }

    mNextAuthorityId = Elastos::Core::Math::Max(highestAuthorityId + 1, mNextAuthorityId);
    if (fis != NULL) {
        ICloseable::Probe(fis)->Close();
    }

    if (FAILED(ec)) return;

    MaybeMigrateSettingsForRenamedAuthorities();
}

void SyncStorageEngine::MaybeDeleteLegacyPendingInfoLocked(
    /* [in] */ IFile* syncDir)
{
    AutoPtr<IFile> file;
    CFile::New(syncDir, String("pending.bin"), (IFile**)&file);
    Boolean exist;
    file->Exists(&exist);
    if (!exist) {
        return;
    }
    else {
        file->Delete();
    }
}

Boolean SyncStorageEngine::MaybeMigrateSettingsForRenamedAuthorities()
{
    Boolean writeNeeded = FALSE;

    List<AutoPtr<AuthorityInfo> > authoritiesToRemove;
    HashMap<String, String>::Iterator strIt;
    HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator mit;
    for (mit = mAuthorities.Begin(); mit != mAuthorities.End(); ++mit) {
        AutoPtr<AuthorityInfo> authority = mit->mSecond;
        // skip this authority if it doesn't target a provider
        if (authority->mTarget->mTarget_service) {
            continue;
        }
        // skip this authority if it isn't one of the renamed ones
        strIt = sAuthorityRenames->Find(authority->mTarget->mProvider);
        if (strIt == sAuthorityRenames->End()) {
            continue;
        }
        String newAuthorityName = strIt->mSecond;
        if (newAuthorityName.IsNull()) {
            continue;
        }

        // remember this authority so we can remove it later. we can't remove it
        // now without messing up this loop iteration
        authoritiesToRemove.PushBack(authority);

        // this authority isn't enabled, no need to copy it to the new authority name since
        // the default is "disabled"
        if (!authority->mEnabled) {
            continue;
        }

        // if we already have a record of this new authority then don't copy over the settings
        AutoPtr<EndPoint> newInfo =
            new EndPoint(authority->mTarget->mAccount,
                newAuthorityName,
                authority->mTarget->mUserId);
        if (GetAuthorityLocked(newInfo, String("cleanup")) != NULL) {
            continue;
        }

        AutoPtr<AuthorityInfo> newAuthority =
            GetOrCreateAuthorityLocked(newInfo, -1 /* ident */, FALSE /* doWrite */);
        newAuthority->mEnabled = TRUE;
        writeNeeded = TRUE;
    }

    AuthorityInfo* authorityInfo;
    List<AutoPtr<AuthorityInfo> >::Iterator lit;
    for (lit = authoritiesToRemove.Begin(); lit != authoritiesToRemove.End(); ++lit) {
        authorityInfo = *lit;

        RemoveAuthorityLocked(
            authorityInfo->mTarget->mAccount,
            authorityInfo->mTarget->mUserId,
            authorityInfo->mTarget->mProvider,
            FALSE /* doWrite */);
        writeNeeded = TRUE;
    }

    return writeNeeded;
}

void SyncStorageEngine::ParseListenForTickles(
    /* [in] */ IXmlPullParser* parser)
{
    String user;
    parser->GetAttributeValue(String(NULL), XML_ATTR_USER, &user);
    Int32 userId;
    ECode ec = StringUtils::Parse(user, &userId);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "error parsing the user for listen-for-tickles");
    }
    else if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        Logger::E(TAG, "the user in listen-for-tickles is NULL");
    }

    String enabled;
    parser->GetAttributeValue(String(NULL), XML_ATTR_ENABLED, &enabled);
    Boolean listen = enabled.IsNull() || StringUtils::ParseBoolean(enabled);
    mMasterSyncAutomatically[userId] = listen;
}

AutoPtr<AuthorityInfo> SyncStorageEngine::ParseAuthority(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 version)
{
    AutoPtr<AuthorityInfo> authority;
    String idStr, nullStr;
    parser->GetAttributeValue(nullStr, String("id"), &idStr);

    Int32 id = -1;
    ECode ec = StringUtils::Parse(idStr, &id);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "error parsing the id of the authority");
        id = -1;
    }
    else if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        Logger::E(TAG, "the id of the authority is NULL");
        id = -1;
    }

    if (id >= 0) {
        String authorityName, enabled, syncable, accountName, accountType, user, packageName, className;
        parser->GetAttributeValue(nullStr, String("authority"), &authorityName);
        parser->GetAttributeValue(nullStr, XML_ATTR_ENABLED, &enabled);
        parser->GetAttributeValue(nullStr, String("syncable"), &syncable);
        parser->GetAttributeValue(nullStr, String("account"), &accountName);
        parser->GetAttributeValue(nullStr, String("type"), &accountType);
        parser->GetAttributeValue(nullStr, XML_ATTR_USER, &user);
        parser->GetAttributeValue(nullStr, String("package"), &packageName);
        parser->GetAttributeValue(nullStr, String("class"), &className);
        Int32 userId = user.IsNull() ? 0 : StringUtils::ParseInt32(user);
        if (accountType == NULL && packageName == NULL) {
            accountType = "com.google";
            syncable = "unknown";
        }

        HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt = mAuthorities.Find(id);
        if (authIt != mAuthorities.End()) {
            authority = authIt->mSecond;
        }
        // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
        //     Log.v(TAG_FILE, "Adding authority:"
        //             + " account=" + accountName
        //             + " accountType=" + accountType
        //             + " auth=" + authorityName
        //             + " package=" + packageName
        //             + " class=" + className
        //             + " user=" + userId
        //             + " enabled=" + enabled
        //             + " syncable=" + syncable);
        // }
        if (authority == NULL) {
            // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
            //     Log.v(TAG_FILE, "Creating authority entry");
            // }
            AutoPtr<EndPoint> info;
            if (accountName != NULL && authorityName != NULL) {
                AutoPtr<IAccount> account;
                assert(0 && "TODO");
                // CAccount::New(accountName, accountType, (IAccount**)&account);
                info = new EndPoint(account, authorityName, userId);
            }
            else {
                AutoPtr<IComponentName> cn;
                CComponentName::New(packageName, className, (IComponentName**)&cn);
                info = new EndPoint(cn, userId);
            }
            authority = GetOrCreateAuthorityLocked(info, id, FALSE);
            // If the version is 0 then we are upgrading from a file format that did not
            // know about periodic syncs. In that case don't clear the list since we
            // want the default, which is a daily periodic sync.
            // Otherwise clear out this default list since we will populate it later with
            // the periodic sync descriptions that are read from the configuration file.
            if (version > 0) {
                authority->mPeriodicSyncs.Clear();
            }
        }
        if (authority != NULL) {
            authority->mEnabled = enabled == NULL || StringUtils::ParseBoolean(enabled);
            if (syncable.Equals("unknown")) {
                authority->mSyncable = -1;
            }
            else {
                authority->mSyncable =
                    (syncable == NULL || StringUtils::ParseBoolean(syncable)) ? 1 : 0;
            }
        }
        else {
            Logger::W(TAG, "Failure adding authority: account=%s auth=%s enabled=%s syncable=%s",
                accountName.string(), authorityName.string(), enabled.string(), syncable.string());
        }
    }
    return authority;
}

AutoPtr<IPeriodicSync> SyncStorageEngine::ParsePeriodicSync(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ AuthorityInfo* authorityInfo)
{
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras); // Gets filled in later.
    String periodValue, flexValue, nullStr;
    parser->GetAttributeValue(nullStr, String("period"), &periodValue);
    parser->GetAttributeValue(nullStr, String("flex"), &flexValue);
    Int64 period;
    Int64 flextime;

    ECode ec = StringUtils::Parse(periodValue, &period);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "error parsing the period of a periodic sync");
        return NULL;
    }
    else if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        Logger::E(TAG, "the period of a periodic sync is NULL");
        return NULL;
    }

    ec = StringUtils::Parse(flexValue, &flextime);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        flextime = CalculateDefaultFlexTime(period);
        Logger::E(TAG, "Error formatting value parsed for periodic sync flex: %s, using default: %lld",
            flexValue.string(), flextime);
        return NULL;
    }
    else if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        flextime = CalculateDefaultFlexTime(period);
        Logger::E(TAG, "No flex time specified for this sync, using a default. period: %d, flex: %lld",
            period, flextime);
        return NULL;
    }

    AutoPtr<IPeriodicSync> periodicSync;
    if (authorityInfo->mTarget->mTarget_provider) {
        CPeriodicSync::New(authorityInfo->mTarget->mAccount,
            authorityInfo->mTarget->mProvider,
            extras,
            period, flextime, (IPeriodicSync**)&periodicSync);
    }
    else {
        Logger::E(TAG, "Unknown target.");
        return NULL;
    }

    authorityInfo->mPeriodicSyncs.PushBack(periodicSync);
    return periodicSync;
}

void SyncStorageEngine::ParseExtra(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IBundle* extras)
{
    String name, type, value1, value2, nullStr;
    parser->GetAttributeValue(nullStr, String("name"), &name);
    parser->GetAttributeValue(nullStr, String("type"), &type);
    parser->GetAttributeValue(nullStr, String("value1"), &value1);
    parser->GetAttributeValue(nullStr, String("value2"), &value2);

    ECode ec = NOERROR;
    Int64 lval;
    Int32 ival;
    Boolean bval;
    Float fval;
    Double dval;

    if (type.Equals("long")) {
        FAIL_GOTO(StringUtils::Parse(value1, &lval), _EXIT_)
        extras->PutInt64(name, lval);
    }
    else if (type.Equals("integer")) {
        FAIL_GOTO(StringUtils::Parse(value1, &ival), _EXIT_)
        extras->PutInt32(name, ival);
    }
    else if (type.Equals("double")) {
        FAIL_GOTO(StringUtils::Parse(value1, &dval), _EXIT_)
        extras->PutDouble(name, dval);
    }
    else if (type.Equals("float")) {
        FAIL_GOTO(StringUtils::Parse(value1, &fval), _EXIT_)
        extras->PutFloat(name, fval);
    }
    else if (type.Equals("boolean")) {
        bval = StringUtils::ParseBoolean(value1);
        extras->PutBoolean(name, bval);
    }
    else if (type.Equals("string")) {
        extras->PutString(name, value1);
    }
    else if (type.Equals("account")) {
        AutoPtr<IAccount> account;
        assert(0 && "TODO");
        // CAccount::New(value1, value2, (IAccount**)&account);
        extras->PutParcelable(name, IParcelable::Probe(account));
    }

_EXIT_:
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Logger::E(TAG, "error parsing bundle value");
    }
    else if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        Logger::E(TAG, "error parsing bundle value");
    }
}

void SyncStorageEngine::WriteAccountInfoLocked()
{
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG_FILE, "Writing new " + mAccountInfoFile.getBaseFile());
    // }
    AutoPtr<IFileOutputStream> fos;
    HashMap<Int32, Boolean>::Iterator it;
    HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt;
    List<AutoPtr<IPeriodicSync> >::Iterator lit;
    AutoPtr<AuthorityInfo> authority;
    AutoPtr<IPeriodicSync> periodicSync;
    AutoPtr<EndPoint> info;
    AutoPtr<IBundle> bundle;
    String strVal, nullStr, name, type, pkgName, clsName;
    Int32 userId;
    Int64 period, flexTime;
    Boolean listen;
    ECode ec = NOERROR;
    AutoPtr<IXmlSerializer> out;

    FAIL_GOTO(mAccountInfoFile->StartWrite((IFileOutputStream**)&fos), _EXIT_)
    CFastXmlSerializer::New((IXmlSerializer**)&out);
    FAIL_GOTO(out->SetOutput(IOutputStream::Probe(fos), String("utf-8")), _EXIT_)
    FAIL_GOTO(out->StartDocument(nullStr, TRUE), _EXIT_)
    FAIL_GOTO(out->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE), _EXIT_)

    FAIL_GOTO(out->WriteStartTag(nullStr, String("accounts")), _EXIT_)
    FAIL_GOTO(out->WriteAttribute(nullStr, String( "version"), StringUtils::ToString(ACCOUNTS_VERSION)), _EXIT_)
    FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_NEXT_AUTHORITY_ID, StringUtils::ToString(mNextAuthorityId)), _EXIT_)
    FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_SYNC_RANDOM_OFFSET, StringUtils::ToString(mSyncRandomOffset)), _EXIT_)

    // Write the Sync Automatically flags for each user
    for (it = mMasterSyncAutomatically.Begin(); it != mMasterSyncAutomatically.End(); ++it) {
        userId = it->mFirst;
        listen = it->mSecond;
        FAIL_GOTO(out->WriteStartTag(nullStr, XML_TAG_LISTEN_FOR_TICKLES), _EXIT_)
        FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_USER, StringUtils::ToString(userId)), _EXIT_)
        FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_ENABLED, StringUtils::BooleanToString(listen)), _EXIT_)
        FAIL_GOTO(out->WriteEndTag(nullStr, XML_TAG_LISTEN_FOR_TICKLES), _EXIT_)
    }

    for (authIt = mAuthorities.Begin(); authIt != mAuthorities.End(); ++authIt) {
        authority = authIt->mSecond;
        info = authority->mTarget;
        FAIL_GOTO(out->WriteStartTag(nullStr, String("authority")), _EXIT_)
        FAIL_GOTO(out->WriteAttribute(nullStr, String("id"), StringUtils::ToString(authority->mIdent)), _EXIT_)
        FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_USER, StringUtils::ToString(info->mUserId)), _EXIT_)
        FAIL_GOTO(out->WriteAttribute(nullStr, XML_ATTR_ENABLED, StringUtils::BooleanToString(authority->mEnabled)), _EXIT_)
        if (info->mService == NULL) {
            info->mAccount->GetName(&name);
            info->mAccount->GetType(&type);
            FAIL_GOTO(out->WriteAttribute(nullStr, String("account"), name), _EXIT_)
            FAIL_GOTO(out->WriteAttribute(nullStr, String("type"), type), _EXIT_)
            FAIL_GOTO(out->WriteAttribute(nullStr, String("authority"), info->mProvider), _EXIT_)
        }
        else {
            info->mService->GetPackageName(&pkgName);
            info->mService->GetClassName(&clsName);
            FAIL_GOTO(out->WriteAttribute(nullStr, String("package"), pkgName), _EXIT_)
            FAIL_GOTO(out->WriteAttribute(nullStr, String("class"), clsName), _EXIT_)
        }
        if (authority->mSyncable < 0) {
            FAIL_GOTO(out->WriteAttribute(nullStr, String("syncable"), String("unknown")), _EXIT_)
        }
        else {
            FAIL_GOTO(out->WriteAttribute(nullStr, String("syncable"), StringUtils::BooleanToString(authority->mSyncable != 0)), _EXIT_)
        }
        for (lit = authority->mPeriodicSyncs.Begin(); lit != authority->mPeriodicSyncs.End(); ++lit) {
            periodicSync = *lit;
            FAIL_GOTO(out->WriteStartTag(nullStr, String("periodicSync")), _EXIT_)
            periodicSync->GetPeriod(&period);
            periodicSync->GetFlexTime(&flexTime);
            periodicSync->GetExtras((IBundle**)&bundle);
            FAIL_GOTO(out->WriteAttribute(nullStr, String("period"), StringUtils::ToString(period)), _EXIT_)
            FAIL_GOTO(out->WriteAttribute(nullStr, String("flex"), StringUtils::ToString(flexTime)), _EXIT_)

            ExtrasToXml(out, bundle);
            FAIL_GOTO(out->WriteEndTag(nullStr, String("periodicSync")), _EXIT_)
        }
        FAIL_GOTO(out->WriteEndTag(nullStr, String("authority")), _EXIT_)
    }
    FAIL_GOTO(out->WriteEndTag(nullStr, String("accounts")), _EXIT_)
    FAIL_GOTO(out->EndDocument(), _EXIT_)
    FAIL_GOTO(mAccountInfoFile->FinishWrite(fos), _EXIT_)

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Error writing accounts");
        if (fos != NULL) {
            mAccountInfoFile->FailWrite(fos);
        }
    }
}

Int32 SyncStorageEngine::GetInt32Column(
    /* [in] */ ICursor* c,
    /* [in] */ const String& name)
{
    Int32 index;
    c->GetColumnIndex(name, &index);
    Int32 ival;
    c->GetInt32(index, &ival);
    return ival;
}

Int64 SyncStorageEngine::GetInt64Column(
    /* [in] */ ICursor* c,
    /* [in] */ const String& name)
{
    Int32 index;
    c->GetColumnIndex(name, &index);
    Int64 ival;
    c->GetInt64(index, &ival);
    return ival;
}

static void Put(
    /* [in] */ IHashMap* map,
    /* [in] */ const char* key,
    /* [in] */ const char* value)
{
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<ICharSequence> valueObj = CoreUtils::Convert(value);
    map->Put(keyObj.Get(), valueObj.Get());
}

void SyncStorageEngine::ReadAndDeleteLegacyAccountInfoLocked()
{
    // Look for old database to initialize from.
    AutoPtr<IFile> file;
    mContext->GetDatabasePath(String("syncmanager.db"), (IFile**)&file);
    Boolean bval;
    file->Exists(&bval);
    if (!bval) {
        return;
    }

    AutoPtr<ISQLiteDatabaseHelper> helper;
    CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
    String path;
    file->GetPath(&path);
    AutoPtr<ISQLiteDatabase> db;
    helper->OpenDatabase(path, NULL,
        ISQLiteDatabase::OPEN_READONLY, (ISQLiteDatabase**)&db);
    if (db != NULL) {
        Int32 version;
        db->GetVersion(&version);
        Boolean hasType = version >= 11;

        // Copy in all of the status information, as well as accounts.
        // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
        //     Log.v(TAG_FILE, "Reading legacy sync accounts db");
        // }
        AutoPtr<ISQLiteQueryBuilder> qb;
        CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
        qb->SetTables(String("stats, status"));
        AutoPtr<IHashMap> map;
        CHashMap::New((IHashMap**)&map);

        Put(map, "_id", "status._id as _id");
        Put(map, "account", "stats->mAccount as account");
        if (hasType) {
            Put(map, "account_type", "stats->mAccount_type as account_type");
        }
        Put(map, "authority", "stats.authority as authority");
        Put(map, "totalElapsedTime", "totalElapsedTime");
        Put(map, "numSyncs", "numSyncs");
        Put(map, "numSourceLocal", "numSourceLocal");
        Put(map, "numSourcePoll", "numSourcePoll");
        Put(map, "numSourceServer", "numSourceServer");
        Put(map, "numSourceUser", "numSourceUser");
        Put(map, "lastSuccessSource", "lastSuccessSource");
        Put(map, "lastSuccessTime", "lastSuccessTime");
        Put(map, "lastFailureSource", "lastFailureSource");
        Put(map, "lastFailureTime", "lastFailureTime");
        Put(map, "lastFailureMesg", "lastFailureMesg");
        Put(map, "pending", "pending");
        qb->SetProjectionMap(IMap::Probe(map));
        qb->AppendWhere(CoreUtils::Convert("stats._id = status.stats_id"));
        AutoPtr<ICursor> c;
        String nullStr;
        qb->Query(db, NULL, nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&c);
        Boolean bval;
        String accountName, strVal;
        Int32 index;
        while (c->MoveToNext(&bval), bval) {

            c->GetColumnIndex(String("account"), &index);
            c->GetString(index, &accountName);

            c->GetColumnIndex(String("account_type"), &index);
            String accountType;
            if (hasType) {
                c->GetString(index, &accountType);
            }
            if (accountType == NULL) {
                accountType = "com.google";
            }

            c->GetColumnIndex(String("authority"), &index);
            String authorityName;
            c->GetString(index, &authorityName);
            AutoPtr<IAccount> account;
            assert(0 && "TODO");
            // CAccount::New(accountName, accountType, (IAccount**)&account);
            AutoPtr<EndPoint> ep = new EndPoint(account,
                authorityName,
                0 /* legacy is single-user */);
            AutoPtr<AuthorityInfo> authority = GetOrCreateAuthorityLocked(ep, -1, FALSE);
            if (authority != NULL) {
                Boolean found = FALSE;
                Int32 id;
                AutoPtr<ISyncStatusInfo> st;
                HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator rit;
                for (rit = mSyncStatus.Begin(); rit != mSyncStatus.End(); ++rit) {
                    st = rit->mSecond;
                    st->GetAuthorityId(&id);
                    if (id == authority->mIdent) {
                        found = TRUE;
                        break;
                    }
                }
                if (!found) {
                    CSyncStatusInfo::New(authority->mIdent, (ISyncStatusInfo**)&st);
                    mSyncStatus[authority->mIdent] = st;
                }
                st->SetTotalElapsedTime(GetInt64Column(c, String("totalElapsedTime")));
                st->SetNumSyncs(GetInt32Column(c, String("numSyncs")));
                st->SetNumSourceLocal(GetInt32Column(c, String("numSourceLocal")));
                st->SetNumSourcePoll(GetInt32Column(c, String("numSourcePoll")));
                st->SetNumSourceServer(GetInt32Column(c, String("numSourceServer")));
                st->SetNumSourceUser(GetInt32Column(c, String("numSourceUser")));
                st->SetNumSourcePeriodic(0);
                st->SetLastSuccessSource(GetInt32Column(c, String("lastSuccessSource")));
                st->SetLastSuccessTime(GetInt64Column(c, String("lastSuccessTime")));
                st->SetLastFailureSource(GetInt32Column(c, String("lastFailureSource")));
                st->SetLastFailureTime(GetInt64Column(c, String("lastFailureTime")));
                c->GetColumnIndex(String("lastFailureMesg"), &index);
                c->GetString(index, &strVal);
                st->SetLastFailureMesg(strVal);
                st->SetPending(GetInt32Column(c, String("pending")) != 0);
            }
        }

        ICloseable::Probe(c)->Close();

        // Retrieve the settings.
        qb = NULL;
        CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
        qb->SetTables(String("settings"));
        c = NULL;
        qb->Query(db, NULL, nullStr, NULL, nullStr, nullStr, nullStr, (ICursor**)&qb);
        while (c->MoveToNext(&bval), bval) {
            String name, value;
            c->GetColumnIndex(String("name"), &index);
            c->GetString(index, &name);
            c->GetColumnIndex(String("value"), &index);
            c->GetString(index, &value);
            if (name == NULL) continue;
            if (name.Equals("listen_for_tickles")) {
                SetMasterSyncAutomatically(value == NULL || StringUtils::ParseBoolean(value), 0);
            }
            else if (name.StartWith("sync_provider_")) {
                String sync("sync_provider_");
                String provider = name.Substring(sync.GetLength(), name.GetLength());
                // TODO reserse loop
                HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator rit;
                for (rit = mAuthorities.Begin(); rit != mAuthorities.End(); ++rit) {
                    AutoPtr<AuthorityInfo> authority = rit->mSecond;
                    if (authority->mTarget->mProvider.Equals(provider)) {
                        authority->mEnabled = value == NULL || StringUtils::ParseBoolean(value);
                        authority->mSyncable = 1;
                    }
                }
            }
        }

        ICloseable::Probe(c)->Close();

        ICloseable::Probe(db)->Close();

        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        file->Delete();
    }
}

void SyncStorageEngine::ReadStatusLocked()
{
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG_FILE, "Reading " + mStatusFile.getBaseFile());
    // }
    ECode ec = NOERROR;
    HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator it;
    Int32 token, id;
    AutoPtr<IParcel> in;

    AutoPtr<ArrayOf<Byte> > data;
    ec = mStatusFile->ReadFully((ArrayOf<Byte>**)&data);
    FAIL_GOTO(ec, _EXIT_)

    CParcel::New((IParcel**)&in);
    ec = in->Unmarshall(data, 0, data->GetLength());
    FAIL_GOTO(ec, _EXIT_)

    in->SetDataPosition(0);
    FAIL_GOTO(in->ReadInt32(&token), _EXIT_)
    while (token != STATUS_FILE_END) {
        if (token == STATUS_FILE_ITEM) {
            AutoPtr<ISyncStatusInfo> status;
            CSyncStatusInfo::New((ISyncStatusInfo**)&status);
            IParcelable::Probe(status)->ReadFromParcel(in);
            status->GetAuthorityId(&id);
            it = mAuthorities.Find(id);
            if (it != mAuthorities.End()) {
                status->SetPending(FALSE);
                // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
                //     Log.v(TAG_FILE, "Adding status for id " + status->mAuthorityId);
                // }
                mSyncStatus[id] = status;
            }
        }
        else {
            // Ooops.
            Logger::W(TAG, "Unknown status token: %s", token);
            break;
        }

        FAIL_GOTO(in->ReadInt32(&token), _EXIT_)
    }

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::I(TAG, "No initial status");
    }
}

void SyncStorageEngine::WriteStatusLocked()
{
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG_FILE, "Writing new " + mStatusFile.getBaseFile());
    // }

    // The file is being written, so we don't need to have a scheduled
    // write until the next change.
    RemoveMessages(MSG_WRITE_STATUS);

    ECode ec = NOERROR;
    AutoPtr<IParcel> out;
    HashMap<Int32, AutoPtr<ISyncStatusInfo> >::Iterator it;

    AutoPtr<ArrayOf<Byte> > bytes;
    AutoPtr<IFileOutputStream> fos;
    ec = mStatusFile->StartWrite((IFileOutputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    CParcel::New((IParcel**)&out);

    for (it = mSyncStatus.Begin(); it != mSyncStatus.End(); ++it) {
        AutoPtr<ISyncStatusInfo> status = it->mSecond;
        out->WriteInt32(STATUS_FILE_ITEM);
        IParcelable::Probe(status)->WriteToParcel(out);
    }

    out->WriteInt32(STATUS_FILE_END);
    out->Marshall((ArrayOf<Byte>**)&bytes);
    IOutputStream::Probe(fos)->Write(bytes);

    mStatusFile->FinishWrite(fos);

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::I(TAG, "Error writing status: E_IO_EXCEPTION");
        if (fos != NULL) {
            mStatusFile->FailWrite(fos);
        }
    }
}

ECode SyncStorageEngine::ReadPendingOperationsLocked()
{
    AutoPtr<IFileInputStream> fis;
    AutoPtr<IFile> baseFile;
    mPendingFile->GetBaseFile((IFile**)&baseFile);
    Boolean exist;
    baseFile->Exists(&exist);
    if (!exist) {
        // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
        //     Log.v(TAG_FILE, "No pending operation file.");
        // }
        return NOERROR;
    }

    // try {
    AutoPtr<IXmlPullParser> parser;
    AutoPtr<PendingOperation> pop;
    AutoPtr<AuthorityInfo> authorityInfo;
    AutoPtr<IBundle> bundle;
    Int32 eventType, depth, authorityId, syncSource, reason;
    Boolean expedited;
    String tagName, versionString, nullStr, strVal;
    HashMap<Int32, AutoPtr<AuthorityInfo> >::Iterator authIt;
    ECode ec = NOERROR;

    ec = mPendingFile->OpenRead((IFileInputStream**)&fis);
    FAIL_GOTO(ec, _EXIT_)
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG_FILE, "Reading " + mPendingFile.getBaseFile());
    // }
    ec = Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_GOTO(ec, _EXIT_)
    ec = parser->SetInput(IInputStream::Probe(fis), nullStr);
    FAIL_GOTO(ec, _EXIT_)
    parser->GetEventType(&eventType);
    FAIL_GOTO(ec, _EXIT_)
    while (eventType != IXmlPullParser::START_TAG
        && eventType != IXmlPullParser::END_DOCUMENT) {
        ec = parser->Next(&eventType);
        FAIL_GOTO(ec, _EXIT_)
    }
    if (eventType == IXmlPullParser::END_DOCUMENT) return NOERROR; // Nothing to read.

    do {
        if (eventType == IXmlPullParser::START_TAG) {
            ec = parser->GetName(&tagName);
            FAIL_GOTO(ec, _EXIT_)

            ec = parser->GetDepth(&depth);
            FAIL_GOTO(ec, _EXIT_)
            if (depth == 1 && tagName.Equals("op")) {
                // Verify version.
                ec = parser->GetAttributeValue(nullStr, XML_ATTR_VERSION, &versionString);
                FAIL_GOTO(ec, _EXIT_)

                if (versionString == NULL || StringUtils::ParseInt32(versionString) != PENDING_OPERATION_VERSION) {
                    Logger::W(TAG, "Unknown pending operation version %s", versionString.string());
                    return E_IO_EXCEPTION;
                }

                ec = parser->GetAttributeValue(nullStr, XML_ATTR_AUTHORITYID, &strVal);
                FAIL_GOTO(ec, _EXIT_)
                authorityId = StringUtils::ParseInt32(strVal);

                ec = parser->GetAttributeValue(nullStr, XML_ATTR_EXPEDITED, &strVal);
                FAIL_GOTO(ec, _EXIT_)
                expedited = StringUtils::ParseBoolean(strVal);

                ec = parser->GetAttributeValue(nullStr, XML_ATTR_SOURCE, &strVal);
                FAIL_GOTO(ec, _EXIT_)
                syncSource = StringUtils::ParseInt32(strVal);

                ec = parser->GetAttributeValue(nullStr, XML_ATTR_REASON, &strVal);
                FAIL_GOTO(ec, _EXIT_)
                reason = StringUtils::ParseInt32(strVal);

                authIt = mAuthorities.Find(authorityId);
                if (authIt != mAuthorities.End()) {
                    authorityInfo = authIt->mSecond;
                }
                // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
                //     Log.v(TAG_FILE, authorityId + " " + expedited + " " + syncSource + " "
                //             + reason);
                // }
                if (authorityInfo != NULL) {
                    CBundle::New((IBundle**)&bundle);
                    pop = new PendingOperation(authorityInfo, reason, syncSource, bundle, expedited);
                    pop->mFlatExtras = NULL; // No longer used.
                    mPendingOperations.PushBack(pop);
                    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
                    //     Log.v(TAG_FILE, "Adding pending op: "
                    //         + pop->mTarget
                    //         + " src=" + pop.syncSource
                    //         + " reason=" + pop.reason
                    //         + " expedited=" + pop.expedited);
                    // }
                }
                else {
                    // Skip non-existent authority.
                    pop = NULL;
                    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
                    //     Log.v(TAG_FILE, "No authority found for " + authorityId
                    //             + ", skipping");
                    // }
                }
            }
            else if (depth == 2 && pop != NULL && tagName.Equals("extra")) {
                ParseExtra(parser, pop->mExtras);
            }

            // if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            //     Logger::D(TAG, "Invalid data in xml file. E_NUMBER_FORMAT_EXCEPTION");
            // }
        }
        parser->Next(&eventType);
    }
    while(eventType != IXmlPullParser::END_DOCUMENT);

_EXIT_:

    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG_FILE, "Error reading pending data.");
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG_FILE, "Error parsing pending ops xml.");
    }

    if (fis != NULL) {
        ICloseable::Probe(fis)->Close();
    }
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > SyncStorageEngine::FlattenBundle(
    /* [in] */ IBundle* bundle)
{
    AutoPtr< ArrayOf<Byte> > flatData;
    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    IParcelable::Probe(bundle)->WriteToParcel(parcel);
    parcel->Marshall((ArrayOf<Byte>**)&flatData);
    return flatData;
}

AutoPtr<IBundle> SyncStorageEngine::UnFlattenBundle(
    /* [in] */ ArrayOf<Byte>* flatData)
{
    AutoPtr<IBundle> bundle;
    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    Int32 length;

    ECode ec = parcel->Unmarshall(flatData, 0, flatData->GetLength());
    FAIL_GOTO(ec, _EXIT_)

    parcel->SetDataPosition(0);
    parcel->ReadInt32(&length);
    if (length != 0) {
        CBundle::New(length, (IBundle**)&bundle);
        IParcelable::Probe(bundle)->ReadFromParcel(parcel);
    }

_EXIT_:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        // A RuntimeException is thrown if we were unable to parse the parcel.
        // Create an empty parcel in this case.
        bundle = NULL;
        CBundle::New((IBundle**)&bundle);
    }
    return bundle;
}

void SyncStorageEngine::WritePendingOperationsLocked()
{
    List<AutoPtr<PendingOperation> >::Iterator lit;
    Int32 N = mPendingOperations.GetSize();
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IXmlSerializer> out;
    ECode ec = NOERROR;

    if (N == 0) {
        // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)){
        //     Log.v(TAG, "Truncating " + mPendingFile.getBaseFile());
        // }
        mPendingFile->Truncate();
        return;
    }
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG, "Writing new " + mPendingFile.getBaseFile());
    // }

    ec = mPendingFile->StartWrite((IFileOutputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    CFastXmlSerializer::New((IFastXmlSerializer**)&out);
    ec = out->SetOutput(IOutputStream::Probe(fos), String("utf-8"));
    FAIL_GOTO(ec, _EXIT_)

    for (lit = mPendingOperations.Begin(); lit != mPendingOperations.End(); ++lit) {
        AutoPtr<PendingOperation> pop = *lit;
        WritePendingOperationLocked(pop, out);
     }

     ec = out->EndDocument();
     FAIL_GOTO(ec, _EXIT_)

     ec = mPendingFile->FinishWrite(fos);

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Error writing pending operations");
        if (fos != NULL) {
            mPendingFile->FailWrite(fos);
        }
    }
}

void SyncStorageEngine::WritePendingOperationLocked(
    /* [in] */ PendingOperation* pop,
    /* [in] */ IXmlSerializer* out)
{
    String nullStr;
    // Pending operation.
    out->WriteStartTag(nullStr, String("op"));

    out->WriteAttribute(nullStr, XML_ATTR_VERSION, StringUtils::ToString(PENDING_OPERATION_VERSION));
    out->WriteAttribute(nullStr, XML_ATTR_AUTHORITYID, StringUtils::ToString(pop->mAuthorityId));
    out->WriteAttribute(nullStr, XML_ATTR_SOURCE, StringUtils::ToString(pop->mSyncSource));
    out->WriteAttribute(nullStr, XML_ATTR_EXPEDITED, StringUtils::BooleanToString(pop->mExpedited));
    out->WriteAttribute(nullStr, XML_ATTR_REASON, StringUtils::ToString(pop->mReason));
    ExtrasToXml(out, pop->mExtras);

    out->WriteEndTag(nullStr, String("op"));
}

void SyncStorageEngine::AppendPendingOperationLocked(
    /* [in] */ PendingOperation* op)
{
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG, "Appending to " + mPendingFile.getBaseFile());
    // }
    AutoPtr<IFileOutputStream> fos;
    ECode ec = mPendingFile->OpenAppend((IFileOutputStream**)&fos);
    if (ec == (ECode)E_IO_EXCEPTION) {
        // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
        //     Log.v(TAG, "Failed append; writing full file");
        // }
        WritePendingOperationsLocked();
        return;
    }

    AutoPtr<IXmlSerializer> out;
    CFastXmlSerializer::New((IFastXmlSerializer**)&out);
    ec = out->SetOutput(IOutputStream::Probe(fos), String("utf-8"));
    FAIL_GOTO(ec, _EXIT_)

    WritePendingOperationLocked(op, out);
    out->EndDocument();
    mPendingFile->FinishWrite(fos);

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Error writing appending operation");
        mPendingFile->FailWrite(fos);
    }

    ICloseable::Probe(fos)->Close();
}

void SyncStorageEngine::ExtrasToXml(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ IBundle* extras)
{
    AutoPtr<ISet> set;
    extras->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    String type("type");
    String value1("value1");
    String nullStr;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> keyObj, valueObj;
        it->GetNext((IInterface**)&keyObj);
        String key = Object::ToString(keyObj);

        out->WriteStartTag(nullStr, String("extra"));
        out->WriteAttribute(nullStr, String("name"), key);
        extras->Get(key, (IInterface**)&valueObj);
        String value = Object::ToString(valueObj);

        if (IInteger64::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("long"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (IInteger32::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("integer"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (IBoolean::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("boolean"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (IFloat::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("float"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (IDouble::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("double"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (ICharSequence::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("string"));
            out->WriteAttribute(nullStr, value1, value);
        }
        else if (IAccount::Probe(valueObj) != NULL) {
            out->WriteAttribute(nullStr, type, String("account"));
            IAccount* accont = IAccount::Probe(valueObj);
            String name, type;
            accont->GetName(&name);
            accont->GetType(&type);
            out->WriteAttribute(nullStr, value1, name);
            out->WriteAttribute(nullStr, String("value2"), type);
        }
        out->WriteEndTag(nullStr, String("extra"));
    }
}

void SyncStorageEngine::RequestSync(
    /* [in] */ AuthorityInfo* authorityInfo,
    /* [in] */ Int32 reason,
    /* [in] */ IBundle* extras)
{
    if (Process::MyUid() == IProcess::SYSTEM_UID
            && mSyncRequestListener != NULL) {
        mSyncRequestListener->OnSyncRequest(
            (ISyncStorageEngineEndPoint*)authorityInfo->mTarget.Get(),
            reason, extras);
    }
    else {
        AutoPtr<ISyncRequestBuilder> req;
        CSyncRequestBuilder::New((ISyncRequestBuilder**)&req);
        req->SyncOnce();
        req->SetExtras(extras);
        if (authorityInfo->mTarget->mTarget_provider) {
            req->SetSyncAdapter(authorityInfo->mTarget->mAccount, authorityInfo->mTarget->mProvider);
        }
        else {
            // if (Log.isLoggable(TAG, Log.DEBUG)) {
            //     Logger::D(TAG, "Unknown target, skipping sync request.");
            // }
            return;
        }

        AutoPtr<IContentResolverHelper> helper;
        CContentResolverHelper::AcquireSingleton((IContentResolverHelper**)&helper);
        AutoPtr<ISyncRequest> sr;
        req->Build((ISyncRequest**)&sr);
        helper->RequestSync(sr);
    }
}

void SyncStorageEngine::RequestSync(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    // If this is happening in the system process, then call the syncrequest listener
    // to make a request back to the SyncManager directly.
    // If this is probably a test instance, then call back through the ContentResolver
    // which will know which userId to apply based on the Binder id.
    if (Process::MyUid() == IProcess::SYSTEM_UID
            && mSyncRequestListener != NULL) {
        AutoPtr<EndPoint> ep = new EndPoint(account, authority, userId);
        mSyncRequestListener->OnSyncRequest(
            (ISyncStorageEngineEndPoint*)ep.Get(),
            reason,
            extras);
    }
    else {
        AutoPtr<IContentResolverHelper> helper;
        CContentResolverHelper::AcquireSingleton((IContentResolverHelper**)&helper);
        helper->RequestSync(account, authority, extras);
    }
}

void SyncStorageEngine::ReadStatisticsLocked()
{
    AutoPtr<IParcel> in;
    ECode ec = NOERROR;
    Int32 token, day;
    Int32 index = 0;
    AutoPtr<DayStats> ds;

    AutoPtr<ArrayOf<Byte> > data;
    ec = mStatisticsFile->ReadFully((ArrayOf<Byte>**)&data);
    FAIL_GOTO(ec, _EXIT_)

    ec = in->Unmarshall(data, 0, data->GetLength());
    FAIL_GOTO(ec, _EXIT_)

    in->SetDataPosition(0);
    ec = in->ReadInt32(&token);
    FAIL_GOTO(ec, _EXIT_)

    while (token != STATISTICS_FILE_END) {
        if (token == STATISTICS_FILE_ITEM || token == STATISTICS_FILE_ITEM_OLD) {
            ec = in->ReadInt32(&day);
            FAIL_GOTO(ec, _EXIT_)

            if (token == STATISTICS_FILE_ITEM_OLD) {
                day = day - 2009 + 14245;  // Magic!
            }
            ds = new DayStats(day);
            in->ReadInt32(&ds->mSuccessCount);
            in->ReadInt64(&ds->mSuccessTime);
            in->ReadInt32(&ds->mFailureCount);
            in->ReadInt64(&ds->mFailureTime);
            if (index < mDayStats->GetLength()) {
                mDayStats->Set(index, ds);
                index++;
            }
        }
        else {
            // Ooops.
            Logger::W(TAG, "Unknown stats token: %d", token);
            break;
        }

        ec = in->ReadInt32(&token);
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::I(TAG, "No initial statistics");
    }
}

void SyncStorageEngine::WriteStatisticsLocked()
{
    // if (Log.isLoggable(TAG_FILE, Log.VERBOSE)) {
    //     Log.v(TAG, "Writing new " + mStatisticsFile.getBaseFile());
    // }

    // The file is being written, so we don't need to have a scheduled
    // write until the next change.
    RemoveMessages(MSG_WRITE_STATISTICS);

    AutoPtr<IParcel> out;
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<ArrayOf<Byte> > bytes;
    ECode ec = mStatisticsFile->StartWrite((IFileOutputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    CParcel::New((IParcel**)&out);

    for (Int32 i = 0; i < mDayStats->GetLength(); i++) {
        DayStats* ds = (*mDayStats)[i];
        if (ds == NULL) {
            break;
        }
        out->WriteInt32(STATISTICS_FILE_ITEM);
        out->WriteInt32(ds->mDay);
        out->WriteInt32(ds->mSuccessCount);
        out->WriteInt64(ds->mSuccessTime);
        out->WriteInt32(ds->mFailureCount);
        out->WriteInt64(ds->mFailureTime);
    }
    out->WriteInt32(STATISTICS_FILE_END);
    out->Marshall((ArrayOf<Byte>**)&bytes);

    ec = IOutputStream::Probe(fos)->Write(bytes);
    FAIL_GOTO(ec, _EXIT_)

    ec = mStatisticsFile->FinishWrite(fos);

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Error writing stats");
        if (fos != NULL) {
            mStatisticsFile->FailWrite(fos);
        }
    }
}

void SyncStorageEngine::DumpPendingOperations(
    /* [in] */ StringBuilder* sb)
{
    sb->Append("Pending Ops: ");
    sb->Append((Int32)mPendingOperations.GetSize());
    sb->Append(" operation(s)\n");
    List<AutoPtr<PendingOperation> >::Iterator it;
    PendingOperation* pop;
    for (it = mPendingOperations.Begin(); it != mPendingOperations.End(); ++it) {
        pop = *it;
        sb->Append("(info: ");
        sb->Append(Object::ToString(pop->mTarget));
        sb->Append(", extras: ");
        sb->Append(Object::ToString(pop->mExtras));
        sb->Append(")\n");
    }
}


} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos
