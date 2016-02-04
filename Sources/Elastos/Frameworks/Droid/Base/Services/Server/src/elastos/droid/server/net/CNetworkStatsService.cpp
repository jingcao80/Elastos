
#include "elastos/droid/server/net/CNetworkStatsService.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
// #include "elastos/droid/server/CNetworkManagementService.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/internal/utility/ArrayUtils.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CObject;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::CFileRotator;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::CNetworkIdentity;
using Elastos::Droid::Net::CNetworkIdentityHelper;
using Elastos::Droid::Net::CNetworkStatsHelper;
using Elastos::Droid::Net::CNetworkTemplate;
using Elastos::Droid::Net::CNetworkTemplateHelper;
using Elastos::Droid::Net::EIID_IINetworkStatsService;
using Elastos::Droid::Net::EIID_IINetworkStatsSession;
using Elastos::Droid::Net::EIID_INetworkStatsNonMonotonicObserver;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkIdentityHelper;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::INetworkState;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::INetworkStatsHelper;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
// using Elastos::Droid::Server::CNetworkManagementService;
// using Elastos::Droid::Server::CNetworkManagementSocketTaggerHelper;
// using Elastos::Droid::Server::INetworkManagementSocketTaggerHelper;
// using Elastos::Droid::Telephony::CTelephonyManager;
// using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CNtpTrustedTimeHelper;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::INtpTrustedTime;
using Elastos::Droid::Utility::INtpTrustedTimeHelper;
using Elastos::IO::CFile;
using Elastos::IO::IWriter;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CNetworkStatsService::ConnReceiver::ConnReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::ConnReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // on background handler thread, and verified CONNECTIVITY_INTERNAL
    // permission above.
    mOwner->UpdateIfaces();
    return NOERROR;
}


CNetworkStatsService::TetherReceiver::TetherReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::TetherReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // on background handler thread, and verified CONNECTIVITY_INTERNAL
    // permission above.
    mOwner->PerformPoll(FLAG_PERSIST_NETWORK);
    return NOERROR;
}


CNetworkStatsService::PollReceiver::PollReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::PollReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // on background handler thread, and verified UPDATE_DEVICE_STATS
    // permission above.
    mOwner->PerformPoll(FLAG_PERSIST_ALL);

    // verify that we're watching global alert
    mOwner->RegisterGlobalAlert();
    return NOERROR;
}


CNetworkStatsService::RemovedReceiver::RemovedReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::RemovedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // on background handler thread, and UID_REMOVED is protected
    // broadcast.

    Int32 uid = -1;
    intent->GetInt32Extra(IIntent::EXTRA_UID, -1, &uid);
    if (uid == -1) return NOERROR;

    AutoPtr<IObject> obj = mOwner->mStatsLock;
    synchronized(obj) {
        mOwner->mWakeLock->AcquireLock();
        // try {
        AutoPtr< ArrayOf<Int32> > uids = ArrayOf<Int32>::Alloc(1);
        (*uids)[0] = uid;
        mOwner->RemoveUidsLocked(uids);
        // } finally {
        mOwner->mWakeLock->ReleaseLock();
        // }
    }
    return NOERROR;
}


CNetworkStatsService::UserReceiver::UserReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::UserReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // On background handler thread, and USER_REMOVED is protected
    // broadcast.

    Int32 userId = -1;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);
    if (userId == -1) return NOERROR;

    AutoPtr<IObject> obj = mOwner->mStatsLock;
    synchronized(obj) {
        mOwner->mWakeLock->AcquireLock();
        // try {
        mOwner->RemoveUserLocked(userId);
        // } finally {
        mOwner->mWakeLock->ReleaseLock();
        // }
    }
    return NOERROR;
}


CNetworkStatsService::ShutdownReceiver::ShutdownReceiver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::ShutdownReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // SHUTDOWN is protected broadcast.
    AutoPtr<IObject> obj = mOwner->mStatsLock;
    synchronized(obj) {
        mOwner->ShutdownLocked();
    }
    return NOERROR;
}

//=============================================================================
// CNetworkStatsService::DropBoxNonMonotonicObserver
//=============================================================================
CAR_INTERFACE_IMPL(CNetworkStatsService::DropBoxNonMonotonicObserver, Object, INetworkStatsNonMonotonicObserver)

CNetworkStatsService::DropBoxNonMonotonicObserver::DropBoxNonMonotonicObserver(
    /* [in] */ CNetworkStatsService* owner)
    : mOwner(owner)
{}

ECode CNetworkStatsService::DropBoxNonMonotonicObserver::FoundNonMonotonic(
    /* [in] */ INetworkStats* left,
    /* [in] */ Int32 leftIndex,
    /* [in] */ INetworkStats* right,
    /* [in] */ Int32 rightIndex,
    /* [in] */ IInterface* cookie)
{
    Slogger::W(TAG, "found non-monotonic values; saving to dropbox");

    // record error for debugging
    StringBuilder builder("found non-monotonic ");
    builder += cookie;
    builder += " values at left[";
    builder += leftIndex;
    builder += "] - right[";
    builder += rightIndex;
    builder += "]\n";
    builder += "left=";
    builder += left;
    builder += '\n';
    builder += "right=";
    builder += right;
    builder += '\n';

    AutoPtr<IDropBoxManager> dropBox;
    mOwner->mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&dropBox);
    return dropBox->AddText(TAG_NETSTATS_ERROR, builder.ToString());
}

//=============================================================================
// CNetworkStatsService::DefaultNetworkStatsSettings
//=============================================================================
CAR_INTERFACE_IMPL(CNetworkStatsService::DefaultNetworkStatsSettings, Object, INetworkStatsSettings)

CNetworkStatsService::DefaultNetworkStatsSettings::DefaultNetworkStatsSettings(
    /* [in] */ IContext* context)
{
    context->GetContentResolver((IContentResolver**)&mResolver);
    assert(mResolver != NULL);
    // TODO: adjust these timings for production builds
}

Int64 CNetworkStatsService::DefaultNetworkStatsSettings::GetGlobalInt64(
    /* [in] */ const String& name,
    /* [in] */ Int64 def)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int64 value;
    global->GetInt64(mResolver, name, def, &value);
    return value;
}

Boolean CNetworkStatsService::DefaultNetworkStatsSettings::GetGlobalBoolean(
    /* [in] */ const String& name,
    /* [in] */ Boolean def)
{
    Int32 defInt = def ? 1 : 0;
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 value;
    global->GetInt32(mResolver, name, defInt, &value);
    return value;
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetPollInterval(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetGlobalInt64(ISettingsGlobal::NETSTATS_POLL_INTERVAL, 30 * IDateUtils::MINUTE_IN_MILLIS);
    return NOERROR;
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetTimeCacheMaxAge(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetGlobalInt64(ISettingsGlobal::NETSTATS_TIME_CACHE_MAX_AGE, IDateUtils::DAY_IN_MILLIS);
    return NOERROR;
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetGlobalAlertBytes(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(GetGlobalInt64(ISettingsGlobal::NETSTATS_GLOBAL_ALERT_BYTES, def))
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetSampleEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetGlobalBoolean(ISettingsGlobal::NETSTATS_SAMPLE_ENABLED, TRUE);
    return NOERROR;
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetReportXtOverDev(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = GetGlobalBoolean(ISettingsGlobal::NETSTATS_REPORT_XT_OVER_DEV, TRUE);
    return NOERROR;
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetDevConfig(
    /* [out] */ INetworkStatsSettingsConfig** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CNetworkStatsService::NetworkStatsSettingsConfig> config = new NetworkStatsSettingsConfig(
            GetGlobalInt64(ISettingsGlobal::NETSTATS_DEV_BUCKET_DURATION, IDateUtils::HOUR_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_DEV_ROTATE_AGE, 15 * IDateUtils::DAY_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_DEV_DELETE_AGE, 90 * IDateUtils::DAY_IN_MILLIS));
    FUNC_RETURN(config)
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetXtConfig(
    /* [out] */ INetworkStatsSettingsConfig** result)
{
    return GetDevConfig(result);
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetUidConfig(
    INetworkStatsSettingsConfig** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CNetworkStatsService::NetworkStatsSettingsConfig> config = new NetworkStatsSettingsConfig(
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_BUCKET_DURATION, IDateUtils::HOUR_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_ROTATE_AGE, 15 * IDateUtils::DAY_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_DELETE_AGE, 90 * IDateUtils::DAY_IN_MILLIS));
    FUNC_RETURN(config)
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetUidTagConfig(
    /* [out] */ INetworkStatsSettingsConfig** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CNetworkStatsService::NetworkStatsSettingsConfig> config = new NetworkStatsSettingsConfig(
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_TAG_BUCKET_DURATION, IDateUtils::HOUR_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_TAG_ROTATE_AGE, 5 * IDateUtils::DAY_IN_MILLIS),
            GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_TAG_DELETE_AGE, 15 * IDateUtils::DAY_IN_MILLIS));
    FUNC_RETURN(config)
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetDevPersistBytes(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(GetGlobalInt64(ISettingsGlobal::NETSTATS_DEV_PERSIST_BYTES, def))
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetXtPersistBytes(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    return GetDevPersistBytes(def, result);
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetUidPersistBytes(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_PERSIST_BYTES, def))
}

ECode CNetworkStatsService::DefaultNetworkStatsSettings::GetUidTagPersistBytes(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(GetGlobalInt64(ISettingsGlobal::NETSTATS_UID_TAG_PERSIST_BYTES, def))
}


const String CNetworkStatsService::ACTION_NETWORK_STATS_POLL("com.android.server.action.NETWORK_STATS_POLL");
const String CNetworkStatsService::ACTION_NETWORK_STATS_UPDATED("com.android.server.action.NETWORK_STATS_UPDATED");
const String CNetworkStatsService::TAG("NetworkStats");
const Boolean CNetworkStatsService::LOGV;
const Int32 CNetworkStatsService::MSG_PERFORM_POLL = 1;
const Int32 CNetworkStatsService::MSG_UPDATE_IFACES = 2;
const Int32 CNetworkStatsService::MSG_REGISTER_GLOBAL_ALERT = 3;
const Int32 CNetworkStatsService::FLAG_PERSIST_NETWORK;
const Int32 CNetworkStatsService::FLAG_PERSIST_UID;
const Int32 CNetworkStatsService::FLAG_PERSIST_ALL;
const Int32 CNetworkStatsService::FLAG_PERSIST_FORCE;
const String CNetworkStatsService::TAG_NETSTATS_ERROR("netstats_error");
const String CNetworkStatsService::PREFIX_DEV("dev");
const String CNetworkStatsService::PREFIX_XT("xt");
const String CNetworkStatsService::PREFIX_UID("uid");
const String CNetworkStatsService::PREFIX_UID_TAG("uid_tag");

CAR_INTERFACE_IMPL(CNetworkStatsService::MyHandlerCallback, Object, IHandlerCallback)

ECode CNetworkStatsService::MyHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;

    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
    case CNetworkStatsService::MSG_PERFORM_POLL: {
        Int32 arg1;
        msg->GetArg1(&arg1);
        mHost->PerformPoll(arg1);
        break;
    }
    case CNetworkStatsService::MSG_UPDATE_IFACES: {
        mHost->UpdateIfaces();
        break;
    }
    case CNetworkStatsService::MSG_REGISTER_GLOBAL_ALERT: {
        mHost->RegisterGlobalAlert();
        break;
    }
    default:
        *result = FALSE;
        break;
    }
    return NOERROR;
}

//======================================================================
// CNetworkStatsService::InnerSub_INetworkStatsSession
//======================================================================
CAR_INTERFACE_IMPL(CNetworkStatsService::InnerSub_INetworkStatsSession, Object, IINetworkStatsSession)

CNetworkStatsService::InnerSub_INetworkStatsSession::InnerSub_INetworkStatsSession(
    /* [in] */ CNetworkStatsService* host)
    : mHost(host)
{}

AutoPtr<NetworkStatsCollection> CNetworkStatsService::InnerSub_INetworkStatsSession::GetUidComplete()
{
    AutoPtr<IObject> obj = mHost->mStatsLock;
    synchronized(obj) {
        if (mUidComplete == NULL) {
            mHost->mUidRecorder->GetOrLoadCompleteLocked((NetworkStatsCollection**)&mUidComplete);
        }
    }
    return mUidComplete;
}

AutoPtr<NetworkStatsCollection> CNetworkStatsService::InnerSub_INetworkStatsSession::GetUidTagComplete()
{
    AutoPtr<IObject> obj = mHost->mStatsLock;
    synchronized(obj) {
        if (mUidTagComplete == NULL) {
            mHost->mUidTagRecorder->GetOrLoadCompleteLocked((NetworkStatsCollection**)&mUidTagComplete);
        }
    }
    return mUidTagComplete;
}

ECode CNetworkStatsService::InnerSub_INetworkStatsSession::GetSummaryForNetwork(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [out] */ INetworkStats** result)
{
    FUNC_RETURN(mHost->InternalGetSummaryForNetwork(templ, start, end))
}

ECode CNetworkStatsService::InnerSub_INetworkStatsSession::GetHistoryForNetwork(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int32 fields,
    /* [out] */ INetworkStatsHistory** result)
{
    FUNC_RETURN(mHost->InternalGetHistoryForNetwork(templ, fields))
}

ECode CNetworkStatsService::InnerSub_INetworkStatsSession::GetSummaryForAllUid(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [in] */ Boolean includeTags,
    /* [out] */ INetworkStats** result)
{
    AutoPtr<INetworkStats> stats = GetUidComplete()->GetSummary(templ, start, end);
    if (includeTags) {
        AutoPtr<INetworkStats> tagStats = GetUidTagComplete()->GetSummary(templ, start, end);
        stats->CombineAllValues(tagStats);
    }
    FUNC_RETURN(stats)
}

ECode CNetworkStatsService::InnerSub_INetworkStatsSession::GetHistoryForUid(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 set,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 fields,
    /* [out] */ INetworkStatsHistory** result)
{
    if (tag == INetworkStats::TAG_NONE) {
        FUNC_RETURN(GetUidComplete()->GetHistory(templ, uid, set, tag, fields))
    } else {
        FUNC_RETURN(GetUidTagComplete()->GetHistory(templ, uid, set, tag, fields))
    }
    return NOERROR;
}

ECode CNetworkStatsService::InnerSub_INetworkStatsSession::Close()
{
    mUidComplete = NULL;
    mUidTagComplete = NULL;
    return NOERROR;
}

//======================================================================
// CNetworkStatsService::InnerSub_BaseNetworkObserver
//======================================================================
CNetworkStatsService::InnerSub_BaseNetworkObserver::InnerSub_BaseNetworkObserver(
    /* [in] */ CNetworkStatsService* host)
    : mHost(host)
{}

ECode CNetworkStatsService::InnerSub_BaseNetworkObserver::LimitReached(
    /* [in] */ const String& limitName,
    /* [in] */ const String& iface)
{
    // only someone like NMS should be calling us
    mHost->mContext->EnforceCallingOrSelfPermission(Manifest::permission::CONNECTIVITY_INTERNAL, mHost->TAG);

    // TODO: Waiting for CNetworkManagementService
    assert(0);
    // if (CNetworkManagementService::LIMIT_GLOBAL_ALERT->Equals(limitName)) {
    //     // kick off background poll to collect network stats; UID stats
    //     // are handled during normal polling interval.
    //     Int32 flags = mHost->FLAG_PERSIST_NETWORK;
    //     AutoPtr<IMessage> msg;
    //     mHost->mHandler->ObtainMessage(mHost->MSG_PERFORM_POLL, flags, 0, (IMessage**)&msg);
    //     msg->SendToTarget();

    //     // re-arm global alert for next update
    //     msg = NULL;
    //     mHost->mHandler->ObtainMessage(MSG_REGISTER_GLOBAL_ALERT, (IMessage**)&msg);
    //     msg->SendToTarget();
    // }
    return NOERROR;
}

//=============================================================================
// CNetworkStatsService::NetworkStatsSettingsConfig
//=============================================================================
CAR_INTERFACE_IMPL(CNetworkStatsService::NetworkStatsSettingsConfig, Object, INetworkStatsSettingsConfig)

ECode CNetworkStatsService::NetworkStatsSettingsConfig::GetBucketDuration(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBucketDuration;
    return NOERROR;
}

ECode CNetworkStatsService::NetworkStatsSettingsConfig::GetRotateAgeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRotateAgeMillis;
    return NOERROR;
}

ECode CNetworkStatsService::NetworkStatsSettingsConfig::GetDeleteAgeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDeleteAgeMillis;
    return NOERROR;
}

//======================================================================
// CNetworkStatsService
//======================================================================
CAR_OBJECT_IMPL(CNetworkStatsService)

CAR_INTERFACE_IMPL(CNetworkStatsService, Object, IINetworkStatsService)

CNetworkStatsService::CNetworkStatsService()
    : mSystemReady(FALSE)
    , mPersistThreshold(2 * ITrafficStats::MB_IN_BYTES)
    , mGlobalAlertBytes(0)
    , mLastPhoneState(ITelephonyManager::DATA_UNKNOWN)
    , mLastPhoneNetworkType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
{
    Elastos::Core::CObject::New((IObject**)&mStatsLock);
    CArrayMap::New((IArrayMap**)&mActiveIfaces);
    CArrayMap::New((IArrayMap**)&mActiveUidIfaces);
    mMobileIfaces = ArrayOf<String>::Alloc(0);

    mNonMonotonicObserver = new DropBoxNonMonotonicObserver(this);
    CNetworkStats::New(0L, 10, (INetworkStats**)&mUidOperations);

    mConnReceiver = new ConnReceiver(this);
    mTetherReceiver = new TetherReceiver(this);
    mPollReceiver = new PollReceiver(this);
    mRemovedReceiver = new RemovedReceiver(this);
    mUserReceiver = new UserReceiver(this);
    mShutdownReceiver = new ShutdownReceiver(this);
    mAlertObserver = new InnerSub_BaseNetworkObserver(this);
}

ECode CNetworkStatsService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* networkManager,
    /* [in] */ IIAlarmManager* alarmManager)
{
    AutoPtr<INtpTrustedTimeHelper> helper;
    CNtpTrustedTimeHelper::AcquireSingleton((INtpTrustedTimeHelper**)&helper);
    AutoPtr<INtpTrustedTime> ntt;
    helper->GetInstance(context, (INtpTrustedTime**)&ntt);
    AutoPtr<DefaultNetworkStatsSettings> settings = new DefaultNetworkStatsSettings(context);
    AutoPtr<IFile> file = GetDefaultSystemDir();
    return constructor(context, networkManager, alarmManager, ITrustedTime::Probe(ntt),
            file, INetworkStatsSettings::Probe(settings));
}

AutoPtr<IFile> CNetworkStatsService::GetDefaultSystemDir()
{
    AutoPtr<IFile> defaultSystemDir;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    CFile::New(dataDir, String("system"), (IFile**)&defaultSystemDir);
    return defaultSystemDir;
}

ECode CNetworkStatsService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* networkManager,
    /* [in] */ IIAlarmManager* alarmManager,
    /* [in] */ ITrustedTime* time,
    /* [in] */ IFile* systemDir,
    /* [in] */ INetworkStatsSettings* settings)
{
    VALIDATE_NOT_NULL(context)
    mContext = context;
    VALIDATE_NOT_NULL(networkManager)
    mNetworkManager = networkManager;
    VALIDATE_NOT_NULL(alarmManager)
    mAlarmManager = IAlarmManager::Probe(alarmManager);
    VALIDATE_NOT_NULL(time)
    mTime = time;

    AutoPtr<ITelephonyManagerHelper> helper;
    // TODO: Waiting for CTelephonyManagerHelper
    assert(0);
    // CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    helper->GetDefault((ITelephonyManager**)&mTeleManager);
    VALIDATE_NOT_NULL(mTeleManager);

    VALIDATE_NOT_NULL(settings)
    mSettings = settings;

    AutoPtr<IPowerManager> powerManager;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&powerManager);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);

    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(TAG, (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    mHandlerCallback = new MyHandlerCallback(this);
    CHandler::New(looper, mHandlerCallback, TRUE, (IHandler**)&mHandler);

    VALIDATE_NOT_NULL(systemDir)
    mSystemDir = systemDir;
    CFile::New(systemDir, String("netstats"), (IFile**)&mBaseDir);
    Boolean succeeded;
    mBaseDir->Mkdirs(&succeeded);
    return NOERROR;
}

ECode CNetworkStatsService::BindConnectivityManager(
    /* [in] */ IIConnectivityManager* connManager)
{
    VALIDATE_NOT_NULL(connManager)
    mConnManager = connManager;
    return NOERROR;
}

void CNetworkStatsService::SystemReady()
{
    mSystemReady = TRUE;

    if (!IsBandwidthControlEnabled()) {
        Slogger::W(TAG, "bandwidth controls disabled, unable to track stats");
        return;
    }

    // create data recorders along with historical rotators
    AutoPtr<INetworkStatsSettingsConfig> devConfig;
    mSettings->GetDevConfig((INetworkStatsSettingsConfig**)&devConfig);
    AutoPtr<INetworkStatsSettingsConfig> xtConfig;
    mSettings->GetXtConfig((INetworkStatsSettingsConfig**)&xtConfig);
    AutoPtr<INetworkStatsSettingsConfig> uidConfig;
    mSettings->GetUidConfig((INetworkStatsSettingsConfig**)&uidConfig);
    AutoPtr<INetworkStatsSettingsConfig> uidTagConfig;
    mSettings->GetUidTagConfig((INetworkStatsSettingsConfig**)&uidTagConfig);

    mDevRecorder = BuildRecorder(PREFIX_DEV, (NetworkStatsSettingsConfig*)devConfig.Get(), FALSE);
    mXtRecorder = BuildRecorder(PREFIX_XT, (NetworkStatsSettingsConfig*)xtConfig.Get(), FALSE);
    mUidRecorder = BuildRecorder(PREFIX_UID, (NetworkStatsSettingsConfig*)uidConfig.Get(), FALSE);
    mUidTagRecorder = BuildRecorder(PREFIX_UID_TAG, (NetworkStatsSettingsConfig*)uidTagConfig.Get(), TRUE);

    UpdatePersistThresholds();

    synchronized(this) {
        // upgrade any legacy stats, migrating them to rotated files
        MaybeUpgradeLegacyStatsLocked();

        // read historical network stats from disk, since policy service
        // might need them right away.
        mDevRecorder->GetOrLoadCompleteLocked((NetworkStatsCollection**)&mDevStatsCached);
        mXtRecorder->GetOrLoadCompleteLocked((NetworkStatsCollection**)&mXtStatsCached);

        // bootstrap initial stats to prevent double-counting later
        BootstrapStatsLocked();
    }

    // watch for network interfaces to be claimed
    AutoPtr<IIntentFilter> connFilter;
    CIntentFilter::New(IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE, (IIntentFilter**)&connFilter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mConnReceiver, connFilter, Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL, mHandler, (IIntent**)&intent);

    // watch for tethering changes
    AutoPtr<IIntentFilter> tetherFilter;
    CIntentFilter::New(IConnectivityManager::ACTION_TETHER_STATE_CHANGED, (IIntentFilter**)&tetherFilter);
    AutoPtr<IIntent> intent2;
    mContext->RegisterReceiver(mTetherReceiver, tetherFilter, String(NULL), mHandler, (IIntent**)&intent2);

    // listen for periodic polling events
    AutoPtr<IIntentFilter> pollFilter;
    CIntentFilter::New(ACTION_NETWORK_STATS_POLL, (IIntentFilter**)&pollFilter);
    AutoPtr<IIntent> intent3;
    mContext->RegisterReceiver(mPollReceiver, pollFilter, Elastos::Droid::Manifest::permission::READ_NETWORK_USAGE_HISTORY, mHandler, (IIntent**)&intent3);

    // listen for uid removal to clean stats
    AutoPtr<IIntentFilter> removedFilter;
    CIntentFilter::New(IIntent::ACTION_UID_REMOVED, (IIntentFilter**)&removedFilter);
    AutoPtr<IIntent> intent4;
    mContext->RegisterReceiver(mRemovedReceiver, removedFilter, String(NULL), mHandler, (IIntent**)&intent4);

    // listen for user changes to clean stats
    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New(IIntent::ACTION_USER_REMOVED, (IIntentFilter**)&userFilter);
    AutoPtr<IIntent> intent5;
    mContext->RegisterReceiver(mUserReceiver, userFilter, String(NULL), mHandler, (IIntent**)&intent5);

    // persist stats during clean shutdown
    AutoPtr<IIntentFilter> shutdownFilter;
    CIntentFilter::New(IIntent::ACTION_SHUTDOWN, (IIntentFilter**)&shutdownFilter);
    AutoPtr<IIntent> intent6;
    mContext->RegisterReceiver(mShutdownReceiver, shutdownFilter, (IIntent**)&intent6);

    // try {
    mNetworkManager->RegisterObserver(mAlertObserver);
    // } catch (RemoteException e) {
    //     // ignored; service lives in system_server
    // }

    // watch for networkType changes that aren't broadcast through
    // CONNECTIVITY_ACTION_IMMEDIATE above.
    // if (!COMBINE_SUBTYPE_ENABLED) {
    //     mTeleManager.listen(mPhoneListener, LISTEN_DATA_CONNECTION_STATE);
    // }

    RegisterPollAlarmLocked();
    RegisterGlobalAlert();
}

AutoPtr<NetworkStatsRecorder> CNetworkStatsService::BuildRecorder(
    /* [in] */ const String& prefix,
    /* [in] */ NetworkStatsSettingsConfig* config,
    /* [in] */ Boolean includeTags)
{
    AutoPtr<IDropBoxManager> dropBox;
    mContext->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&dropBox);
    AutoPtr<IFileRotator> fileRotator;
    CFileRotator::New(mBaseDir, prefix, Ptr(config)->Func(config->GetRotateAgeMillis),
            Ptr(config)->Func(config->GetDeleteAgeMillis), (IFileRotator**)&fileRotator);
    AutoPtr<NetworkStatsRecorder> recorder = new NetworkStatsRecorder(fileRotator, mNonMonotonicObserver,
            dropBox, prefix, Ptr(config)->Func(config->GetBucketDuration), includeTags);
    return recorder;
}

void CNetworkStatsService::ShutdownLocked()
{
    mContext->UnregisterReceiver(mConnReceiver);
    mContext->UnregisterReceiver(mTetherReceiver);
    mContext->UnregisterReceiver(mPollReceiver);
    mContext->UnregisterReceiver(mRemovedReceiver);
    mContext->UnregisterReceiver(mShutdownReceiver);

    // if (!INetworkIdentity::COMBINE_SUBTYPE_ENABLED) {
    //     mTeleManager->Listen(mPhoneListener, CPhoneStateListener::LISTEN_NONE);
    // }

    Boolean hasCache;
    mTime->HasCache(&hasCache);
    Int64 currentTime;
    if (hasCache) {
        mTime->GetCurrentTimeMillis(&currentTime);
    }
    else {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTime);
    }

    // persist any pending stats
    mDevRecorder->ForcePersistLocked(currentTime);
    mXtRecorder->ForcePersistLocked(currentTime);
    mUidRecorder->ForcePersistLocked(currentTime);
    mUidTagRecorder->ForcePersistLocked(currentTime);

    mDevRecorder = NULL;
    mXtRecorder = NULL;
    mUidRecorder = NULL;
    mUidTagRecorder = NULL;

    mDevStatsCached = NULL;
    mXtStatsCached = NULL;

    mSystemReady = FALSE;
}

ECode CNetworkStatsService::MaybeUpgradeLegacyStatsLocked()
{
    AutoPtr<IFile> file;
    //try {
    ECode ec;
    do {
        ec = CFile::New(mSystemDir, String("netstats.bin"), (IFile**)&file);
        if (FAILED(ec)) break;
        Boolean exists = FALSE;
        if (file->Exists(&exists), exists) {
            ec = mDevRecorder->ImportLegacyNetworkLocked(file);
            if (FAILED(ec)) break;
            Boolean isDeleted;
            file->Delete(&isDeleted);
        }

        file = NULL;
        ec = CFile::New(mSystemDir, String("netstats_xt.bin"), (IFile**)&file);
        if (FAILED(ec)) break;
        if (file->Exists(&exists), exists) {
            Boolean isDeleted;
            file->Delete(&isDeleted);
        }

        file = NULL;
        ec = CFile::New(mSystemDir, String("netstats_uid.bin"), (IFile**)&file);
        if (FAILED(ec)) break;
        if (file->Exists(&exists), exists) {
            ec = mUidRecorder->ImportLegacyUidLocked(file);
            if (FAILED(ec)) break;
            ec = mUidTagRecorder->ImportLegacyUidLocked(file);
            if (FAILED(ec)) break;
            Boolean isDeleted;
            file->Delete(&isDeleted);
        }
    } while(FALSE);
    if (FAILED(ec)) {
        //} catch (IOException e) {
        //}
        // } catch (OutOfMemoryError e) {
        //     Log.wtf(TAG, "problem during legacy upgrade", e);
        // }
        if ((ECode)E_IO_EXCEPTION == ec) {
            Logger::W(TAG, "problem during legacy upgrade %d", ec);
        }
        else if ((ECode)E_OUT_OF_MEMORY_ERROR == ec) {
            Logger::W(TAG, "problem during legacy upgrade %d", ec);
        }
        else
            return ec;
    }
    return NOERROR;
}

void CNetworkStatsService::RegisterPollAlarmLocked()
{
    if (mPollIntent != NULL) {
        mAlarmManager->Cancel(mPollIntent);
    }
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_NETWORK_STATS_POLL, (IIntent**)&intent);
    mPollIntent = NULL;
    pendingIntentHelper->GetBroadcast(mContext, 0, intent, 0, (IPendingIntent**)&mPollIntent);
    mAlarmManager->SetInexactRepeating(IAlarmManager::ELAPSED_REALTIME, SystemClock::GetElapsedRealtime(),
            Ptr(mSettings)->Func(mSettings->GetPollInterval), mPollIntent);
}

void CNetworkStatsService::RegisterGlobalAlert()
{
    // try {
    ECode ec = mNetworkManager->SetGlobalAlert(mGlobalAlertBytes);
    if (FAILED(ec)) {
        Slogger::W(TAG, "problem registering for global alert: 0x%08x", ec);
    }
    // } catch (IllegalStateException e) {
    //     Slog.w(TAG, "problem registering for global alert: " + e);
    // } catch (RemoteException e) {
    //     // ignored; service lives in system_server
    // }
}

ECode CNetworkStatsService::OpenSession(
    /* [out] */ IINetworkStatsSession** comple)
{
    VALIDATE_NOT_NULL(comple)
    *comple = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::READ_NETWORK_USAGE_HISTORY, TAG))
    FAIL_RETURN(AssertBandwidthControlEnabled())

    // return an IBinder which holds strong references to any loaded stats
    // for its lifetime; when caller closes only weak references remain.

    AutoPtr<IINetworkStatsSession> session = new InnerSub_INetworkStatsSession(this);
    *comple = session;
    REFCOUNT_ADD(*comple)
    return NOERROR;
}

AutoPtr<INetworkStats> CNetworkStatsService::InternalGetSummaryForNetwork(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    if (Ptr(mSettings)->Func(mSettings->GetReportXtOverDev)) {
        // shortcut when XT reporting disabled
        return mDevStatsCached->GetSummary(templ, start, end);
    }

    // splice stats between DEV and XT, switching over from DEV to XT at
    // first atomic bucket.
    Int64 firstAtomicBucket = mXtStatsCached->GetFirstAtomicBucketMillis();
    AutoPtr<INetworkStats> dev = mDevStatsCached->GetSummary(
            templ, Elastos::Core::Math::Min(start, firstAtomicBucket), Elastos::Core::Math::Min(end, firstAtomicBucket));
    AutoPtr<INetworkStats> xt = mXtStatsCached->GetSummary(
            templ, Elastos::Core::Math::Max(start, firstAtomicBucket), Elastos::Core::Math::Max(end, firstAtomicBucket));

    xt->CombineAllValues(dev);
    return xt;
}

AutoPtr<INetworkStatsHistory> CNetworkStatsService::InternalGetHistoryForNetwork(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int32 fields)
{
    if (Ptr(mSettings)->Func(mSettings->GetReportXtOverDev)) {
        // shortcut when XT reporting disabled
        return mDevStatsCached->GetHistory(templ, INetworkStats::UID_ALL,
                INetworkStats::SET_ALL, INetworkStats::TAG_NONE, fields);
    }

    // splice stats between DEV and XT, switching over from DEV to XT at
    // first atomic bucket.
    Int64 firstAtomicBucket = mXtStatsCached->GetFirstAtomicBucketMillis();
    AutoPtr<INetworkStatsHistory> dev = mDevStatsCached->GetHistory(
            templ, INetworkStats::UID_ALL, INetworkStats::SET_ALL, INetworkStats::TAG_NONE,
            fields, Elastos::Core::Math::INT64_MIN_VALUE, firstAtomicBucket);
    AutoPtr<INetworkStatsHistory> xt = mXtStatsCached->GetHistory(
            templ, INetworkStats::UID_ALL, INetworkStats::SET_ALL, INetworkStats::TAG_NONE, fields,
            firstAtomicBucket, Elastos::Core::Math::INT64_MAX_VALUE);

    xt->RecordEntireHistory(dev);
    return xt;
}

ECode CNetworkStatsService::GetNetworkTotalBytes(
    /* [in] */ INetworkTemplate* templ,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end,
    /* [out] */ Int64* totalBytes)
{
    VALIDATE_NOT_NULL(totalBytes)
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::READ_NETWORK_USAGE_HISTORY, TAG))
    FAIL_RETURN(AssertBandwidthControlEnabled())
    return InternalGetSummaryForNetwork(templ, start, end)->GetTotalBytes(totalBytes);
}

ECode CNetworkStatsService::GetDataLayerSnapshotForUid(
    /* [in] */ Int32 uid,
    /* [out] */ INetworkStats** datalayerOut)
{
    VALIDATE_NOT_NULL(datalayerOut)
    *datalayerOut = NULL;

    if (Binder::GetCallingUid() != uid) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE, TAG))
    }
    FAIL_RETURN(AssertBandwidthControlEnabled())

    // TODO: switch to data layer stats once kernel exports
    // for now, read network layer stats and flatten across all ifaces
    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<INetworkStats> networkLayer;
    // try {
    mNetworkManager->GetNetworkStatsUidDetail(uid, (INetworkStats**)&networkLayer);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }

    // splice in operation counts
    networkLayer->SpliceOperationsFrom(mUidOperations);

    Int64 time;
    networkLayer->GetElapsedRealtime(&time);
    Int32 size;
    networkLayer->GetSize(&size);
    AutoPtr<INetworkStats> dataLayer;
    CNetworkStats::New(time, size, (INetworkStats**)&dataLayer);
    AutoPtr<INetworkStatsEntry> entry;
    for (Int32 i = 0; i < size; i++) {
        entry = NULL;
        networkLayer->GetValues(i, entry, (INetworkStatsEntry**)&entry);
        String ifaceAll;
        AutoPtr<INetworkStatsHelper> helper;
        CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
        helper->GetIFACE_ALL(&ifaceAll);
        entry->SetIface(ifaceAll);
        dataLayer->CombineValues(entry);
    }
    *datalayerOut = dataLayer;
    REFCOUNT_ADD(*datalayerOut)
    return NOERROR;
}

ECode CNetworkStatsService::GetMobileIfaces(
    /* [out, callee] */ ArrayOf<String>** mobileIfaces)
{
    VALIDATE_NOT_NULL(mobileIfaces)
    *mobileIfaces = mMobileIfaces;
    REFCOUNT_ADD(*mobileIfaces)
    return NOERROR;
}

ECode CNetworkStatsService::IncrementOperationCount(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 operationCount)
{
    if (Binder::GetCallingUid() != uid) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MODIFY_NETWORK_ACCOUNTING, TAG))
    }

    if (operationCount < 0) {
        //throw new IllegalArgumentException("operation count can only be incremented");
        Slogger::E(TAG, "operation count can only be incremented");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (tag == INetworkStats::TAG_NONE) {
        //throw new IllegalArgumentException("operation count must have specific tag");
        Slogger::E(TAG, "operation count must have specific tag");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    synchronized(this) {
        Int32 set = INetworkStats::SET_DEFAULT;
        HashMap<Int32, Int32>::Iterator iter = mActiveUidCounterSet.Find(uid);
        if(iter != mActiveUidCounterSet.End()) {
            set = iter->mSecond;
        }
        mUidOperations->CombineValues(mActiveIface, uid, set, tag, 0, 0, 0, 0, operationCount);
        mUidOperations->CombineValues(
                mActiveIface, uid, set, INetworkStats::TAG_NONE, 0, 0, 0, 0, operationCount);
    }
    return NOERROR;
}

ECode CNetworkStatsService::SetUidForeground(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean uidForeground)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MODIFY_NETWORK_ACCOUNTING, TAG))

    synchronized(this) {
        Int32 set = uidForeground ? INetworkStats::SET_FOREGROUND : INetworkStats::SET_DEFAULT;
        Int32 oldSet = INetworkStats::SET_DEFAULT;
        HashMap<Int32, Int32>::Iterator iter = mActiveUidCounterSet.Find(uid);
        if(iter != mActiveUidCounterSet.End()) {
            oldSet = iter->mSecond;
        }

        if (oldSet != set) {
            mActiveUidCounterSet[uid] = set;
            assert(0 && "TODO");
            //NetworkManagementSocketTagger::SetKernelCounterSet(uid, set);
        }
    }
    return NOERROR;
}

ECode CNetworkStatsService::ForceUpdate()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::READ_NETWORK_USAGE_HISTORY, TAG))
    FAIL_RETURN(AssertBandwidthControlEnabled())

    Int64 token = Binder::ClearCallingIdentity();
    PerformPoll(FLAG_PERSIST_ALL);
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

static Int64 MathUtilsConstrain(
    /* [in] */ Int64 amount,
    /* [in] */ Int64 low,
    /* [in] */ Int64 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

ECode CNetworkStatsService::AdvisePersistThreshold(
    /* [in] */ Int64 thresholdBytes)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MODIFY_NETWORK_ACCOUNTING, TAG))
    FAIL_RETURN(AssertBandwidthControlEnabled())

    // clamp threshold into safe range
    mPersistThreshold = MathUtilsConstrain(thresholdBytes, 128 * ITrafficStats::KB_IN_BYTES, 2 * ITrafficStats::MB_IN_BYTES);
    if (LOGV) {
        Slogger::V(TAG, "advisePersistThreshold() given %d, clamped to %d", thresholdBytes, mPersistThreshold);
    }

    // update and persist if beyond new thresholds
    Boolean hasCache;
    mTime->HasCache(&hasCache);
    Int64 currentTime;
    if (hasCache) {
        mTime->GetCurrentTimeMillis(&currentTime);
    }
    else {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTime);
    }

    synchronized(this) {
        if (!mSystemReady) return NOERROR;

        UpdatePersistThresholds();

        mDevRecorder->MaybePersistLocked(currentTime);
        mXtRecorder->MaybePersistLocked(currentTime);
        mUidRecorder->MaybePersistLocked(currentTime);
        mUidTagRecorder->MaybePersistLocked(currentTime);
    }

    // re-arm global alert
    RegisterGlobalAlert();
    return NOERROR;
}

void CNetworkStatsService::UpdatePersistThresholds()
{
    Int64 temp;
    mSettings->GetDevPersistBytes(mPersistThreshold, &temp);
    mDevRecorder->SetPersistThreshold(temp);
    mSettings->GetXtPersistBytes(mPersistThreshold, &temp);
    mXtRecorder->SetPersistThreshold(temp);
    mSettings->GetUidPersistBytes(mPersistThreshold, &temp);
    mUidRecorder->SetPersistThreshold(temp);
    mSettings->GetUidTagPersistBytes(mPersistThreshold, &temp);
    mUidTagRecorder->SetPersistThreshold(temp);
    mSettings->GetGlobalAlertBytes(mPersistThreshold, &mPersistThreshold);
}

void CNetworkStatsService::UpdateIfaces()
{
    synchronized(this) {
        mWakeLock->AcquireLock();
        UpdateIfacesLocked();
        mWakeLock->ReleaseLock();
    }
}

void CNetworkStatsService::UpdateIfacesLocked()
{
    if (!mSystemReady) return;
    if (LOGV) Slogger::V(TAG, "updateIfacesLocked()");

    // take one last stats snapshot before updating iface mapping. this
    // isn't perfect, since the kernel may already be counting traffic from
    // the updated network.

    // poll, but only persist network stats to keep codepath fast. UID stats
    // will be persisted during next alarm poll event.
    PerformPollLocked(FLAG_PERSIST_NETWORK);

    AutoPtr< ArrayOf<INetworkState*> > states;
    AutoPtr<ILinkProperties> activeLink;
    // try {
    if (FAILED(mConnManager->GetAllNetworkState((ArrayOf<INetworkState*>**)&states))) {
        return;
    }
    if (FAILED(mConnManager->GetActiveLinkProperties((ILinkProperties**)&activeLink))) {
        return;
    }
    // } catch (RemoteException e) {
    //     // ignored; service lives in system_server
    //     return;
    // }

    if(activeLink != NULL) {
        activeLink->GetInterfaceName(&mActiveIface);
    }
    else {
        mActiveIface = String(NULL);
    }

    // Rebuild active interfaces based on connected networks
    mActiveIfaces->Clear();
    mActiveUidIfaces->Clear();

    AutoPtr<IArraySet> mobileIfaces;
    CArraySet::New((IArraySet**)&mobileIfaces);
    for (Int32 i = 0; i < states->GetLength(); ++i) {
        AutoPtr<INetworkState> state = (*states)[i];
        if (Ptr(state)->GetPtr(state->GetNetworkInfo)->Func(INetworkInfo::IsConnected)) {
            Boolean isMobile;
            AutoPtr<IConnectivityManagerHelper> cmHelper;
            CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&cmHelper);
            cmHelper->IsNetworkTypeMobile(Ptr(state)->GetPtr(state->GetNetworkInfo)->Func(INetworkInfo::GetType), &isMobile);
            AutoPtr<INetworkIdentityHelper> helper;
            CNetworkIdentityHelper::AcquireSingleton((INetworkIdentityHelper**)&helper);
            AutoPtr<INetworkIdentity> ident;
            helper->BuildNetworkIdentity(mContext, state, (INetworkIdentity**)&ident);

            // Traffic occurring on the base interface is always counted for
            // both total usage and UID details.
            String baseIface;
            Ptr(state)->Func(state->GetLinkProperties)->GetInterfaceName(&baseIface);
            if (baseIface != NULL) {
                FindOrCreateNetworkIdentitySet(mActiveIfaces, StringUtils::ParseCharSequence(baseIface))->Add(ident);
                FindOrCreateNetworkIdentitySet(mActiveUidIfaces, StringUtils::ParseCharSequence(baseIface))->Add(ident);
                if (isMobile) {
                    ISet::Probe(mobileIfaces)->Add(StringUtils::ParseCharSequence(baseIface));
                }
            }

            // Traffic occurring on stacked interfaces is usually clatd,
            // which is already accounted against its final egress interface
            // by the kernel. Thus, we only need to collect stacked
            // interface stats at the UID level.
            AutoPtr<IList> stackedLinks;
            Ptr(state)->Func(state->GetLinkProperties)->GetStackedLinks((IList**)&stackedLinks);
            FOR_EACH(iter, stackedLinks) {
                AutoPtr<ILinkProperties> stackedLink = ILinkProperties::Probe(Ptr(iter)->Func(iter->GetNext));
                String stackedIface;
                stackedLink->GetInterfaceName(&stackedIface);
                if (stackedIface != NULL) {
                    FindOrCreateNetworkIdentitySet(mActiveUidIfaces, StringUtils::ParseCharSequence(stackedIface))->Add(ident);
                    if (isMobile) {
                        ISet::Probe(mobileIfaces)->Add(StringUtils::ParseCharSequence(stackedIface));
                    }
                }
            }
        }
    }

    AutoPtr<ArrayOf<IInterface*> > temp;
    ISet::Probe(mobileIfaces)->ToArray(ArrayOf<IInterface*>::Alloc(Ptr(ISet::Probe(mobileIfaces))->Func(ISet::GetSize)), (ArrayOf<IInterface*>**)&temp);
    mMobileIfaces = ArrayOf<String>::Alloc(temp->GetLength());
    for (Int32 i = 0; i < temp->GetLength(); ++i) {
        IObject::Probe((*temp)[i])->ToString(&(*mMobileIfaces)[i]);
    }
}

void CNetworkStatsService::BootstrapStatsLocked()
{
    Boolean hasCache;
    mTime->HasCache(&hasCache);
    Int64 currentTime;
    if (hasCache) {
        mTime->GetCurrentTimeMillis(&currentTime);
    }
    else {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTime);
    }

    //try {
    // snapshot and record current counters; read UID stats first to
    // avoid overcounting dev stats.
    AutoPtr<INetworkStats> uidSnapshot;
    GetNetworkStatsUidDetail((INetworkStats**)&uidSnapshot);
    AutoPtr<INetworkStats> xtSnapshot;
    ECode ec = mNetworkManager->GetNetworkStatsSummaryXt((INetworkStats**)&xtSnapshot);
    if (FAILED(ec)) {
        Slogger::W(TAG, "problem reading network stats: 0x%08x", ec);
        return;
    }
    AutoPtr<INetworkStats> devSnapshot;
    ec = mNetworkManager->GetNetworkStatsSummaryDev((INetworkStats**)&devSnapshot);
    if (FAILED(ec)) {
        Slogger::W(TAG, "problem reading network stats: 0x%08x", ec);
        return;
    }

    mDevRecorder->RecordSnapshotLocked(devSnapshot, IMap::Probe(mActiveIfaces), currentTime);
    mXtRecorder->RecordSnapshotLocked(xtSnapshot, IMap::Probe(mActiveIfaces), currentTime);
    mUidRecorder->RecordSnapshotLocked(uidSnapshot, IMap::Probe(mActiveUidIfaces), currentTime);
    mUidTagRecorder->RecordSnapshotLocked(uidSnapshot, IMap::Probe(mActiveUidIfaces), currentTime);
    //} catch (IllegalStateException e) {
    //    Slog.w(TAG, "problem reading network stats: " + e);
    //} catch (RemoteException e) {
        // ignored; service lives in system_server
    //}
}

void CNetworkStatsService::PerformPoll(
    /* [in] */ Int32 flags)
{
    // try refreshing time source when stale
    Int64 cacheAge;
    if (mTime->GetCacheAge(&cacheAge), cacheAge > Ptr(mSettings)->Func(mSettings->GetTimeCacheMaxAge)) {
        Boolean result;
        mTime->ForceRefresh(&result);
    }

    synchronized(mStatsLock) {
        mWakeLock->AcquireLock();

        PerformPollLocked(flags);
        mWakeLock->ReleaseLock();
    }
}

void CNetworkStatsService::PerformPollLocked(
    /* [in] */ Int32 flags)
{
    if (!mSystemReady) return;
    if (LOGV) Slogger::V(TAG, "performPollLocked(flags=0x%s)", StringUtils::ToHexString(flags).string());

    Int64 startRealtime = SystemClock::GetElapsedRealtime();

    Boolean persistNetwork = (flags & FLAG_PERSIST_NETWORK) != 0;
    Boolean persistUid = (flags & FLAG_PERSIST_UID) != 0;
    Boolean persistForce = (flags & FLAG_PERSIST_FORCE) != 0;

    // TODO: consider marking "untrusted" times in historical stats
    Boolean hasCache;
    mTime->HasCache(&hasCache);
    Int64 currentTime;
    if (hasCache) {
        mTime->GetCurrentTimeMillis(&currentTime);
    }
    else {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&currentTime);
    }

    //try {
    // snapshot and record current counters; read UID stats first to
    // avoid overcounting dev stats.
    AutoPtr<INetworkStats> uidSnapshot;
    GetNetworkStatsUidDetail((INetworkStats**)&uidSnapshot);
    AutoPtr<INetworkStats> xtSnapshot;
    ECode ec = mNetworkManager->GetNetworkStatsSummaryXt((INetworkStats**)&xtSnapshot);
    if (FAILED(ec)) {
        Slogger::W(TAG, "problem reading network stats: 0x%08x", ec);
        return;
    }
    AutoPtr<INetworkStats> devSnapshot;
    ec = mNetworkManager->GetNetworkStatsSummaryDev((INetworkStats**)&devSnapshot);
    if (FAILED(ec)) {
        Slogger::W(TAG, "problem reading network stats: 0x%08x", ec);
        return;
    }

    mDevRecorder->RecordSnapshotLocked(devSnapshot, IMap::Probe(mActiveIfaces), currentTime);
    mXtRecorder->RecordSnapshotLocked(xtSnapshot, IMap::Probe(mActiveIfaces), currentTime);
    mUidRecorder->RecordSnapshotLocked(uidSnapshot, IMap::Probe(mActiveUidIfaces), currentTime);
    mUidTagRecorder->RecordSnapshotLocked(uidSnapshot, IMap::Probe(mActiveUidIfaces), currentTime);
    //} catch (IllegalStateException e) {
    //    Log.wtf(TAG, "problem reading network stats", e);
    //    return;
    //} catch (RemoteException e) {
        // ignored; service lives in system_server
    //    return;
    //}

    // persist any pending data depending on requested flags
    if (persistForce) {
        mDevRecorder->ForcePersistLocked(currentTime);
        mXtRecorder->ForcePersistLocked(currentTime);
        mUidRecorder->ForcePersistLocked(currentTime);
        mUidTagRecorder->ForcePersistLocked(currentTime);
    }
    else {
        if (persistNetwork) {
            mDevRecorder->MaybePersistLocked(currentTime);
            mXtRecorder->MaybePersistLocked(currentTime);
        }
        if (persistUid) {
            mUidRecorder->MaybePersistLocked(currentTime);
            mUidTagRecorder->MaybePersistLocked(currentTime);
        }
    }

    if (LOGV) {
        Int64 duration = SystemClock::GetElapsedRealtime() - startRealtime;
        Slogger::V(TAG, "performPollLocked() took %dms", duration);
    }

    if (Ptr(mSettings)->Func(mSettings->GetSampleEnabled)) {
        // sample stats after each full poll
        PerformSampleLocked();
    }

    // finally, dispatch updated event to any listeners
    AutoPtr<IIntent> updatedIntent;
    CIntent::New(ACTION_NETWORK_STATS_UPDATED, (IIntent**)&updatedIntent);
    updatedIntent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    AutoPtr<IUserHandleHelper> userHandleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    AutoPtr<IUserHandle> userHandle;
    userHandleHelper->GetALL((IUserHandle**)&userHandle);
    mContext->SendBroadcastAsUser(updatedIntent, userHandle, Elastos::Droid::Manifest::permission::READ_NETWORK_USAGE_HISTORY);
}

void CNetworkStatsService::PerformSampleLocked()
{
    // TODO: migrate trustedtime fixes to separate binary log events
    Boolean hasCache;
    mTime->HasCache(&hasCache);
    // Int64 time;
    // Int64 trustedTime = hasCache ? (mTime->GetCurrentTimeMillis(&time), time) : -1;

    AutoPtr<INetworkTemplate> netTemplate;
    AutoPtr<INetworkStatsEntry> devTotal;
    AutoPtr<INetworkStatsEntry> xtTotal;
    AutoPtr<INetworkStatsEntry> uidTotal;

    AutoPtr<INetworkTemplateHelper> helper;
    CNetworkTemplateHelper::AcquireSingleton((INetworkTemplateHelper**)&helper);

    // collect mobile sample
    helper->BuildTemplateMobileWildcard((INetworkTemplate**)&netTemplate);
    mDevRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&devTotal);
    mXtRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&xtTotal);
    mUidRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&uidTotal);

    // EventLogTags::WriteNetstatsMobileSample(
    //         devTotal->rxBytes, devTotal->rxPackets, devTotal->txBytes, devTotal->txPackets,
    //         xtTotal->rxBytes, xtTotal->rxPackets, xtTotal->txBytes, xtTotal->txPackets,
    //         uidTotal->rxBytes, uidTotal->rxPackets, uidTotal->txBytes, uidTotal->txPackets,
    //         trustedTime);

    // collect wifi sample
    helper->BuildTemplateWifiWildcard((INetworkTemplate**)&netTemplate);
    mDevRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&devTotal);
    mXtRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&xtTotal);
    mUidRecorder->GetTotalSinceBootLocked(netTemplate, (INetworkStatsEntry**)&uidTotal);

    // EventLogTags::WriteNetstatsWifiSample(
    //         devTotal->rxBytes, devTotal->rxPackets, devTotal->txBytes, devTotal->txPackets,
    //         xtTotal->rxBytes, xtTotal->rxPackets, xtTotal->txBytes, xtTotal->txPackets,
    //         uidTotal->rxBytes, uidTotal->rxPackets, uidTotal->txBytes, uidTotal->txPackets,
    //         trustedTime);
}

void CNetworkStatsService::RemoveUidsLocked(
    /* [in] */ ArrayOf<Int32>* uids)
{
    if (LOGV) {
        Slogger::V(TAG, "removeUidsLocked() for UIDs ");
        for (Int32 i = 0; i < uids->GetLength(); i++) {
            Slogger::V(TAG, "%d ", (*uids)[i]);
        }
    }

    // Perform one last poll before removing
    PerformPollLocked(FLAG_PERSIST_ALL);

    mUidRecorder->RemoveUidsLocked(uids);
    mUidTagRecorder->RemoveUidsLocked(uids);

    // Clear kernel stats associated with UID
    // TODO: Waiting for INetworkManagementSocketTaggerHelper
    assert(0);
    // AutoPtr<INetworkManagementSocketTaggerHelper> helper;
    // CNetworkManagementSocketTaggerHelper::AcquireSingleton((INetworkManagementSocketTaggerHelper**)&helper);
    // for(Int32 i = 0; i < uids->GetLength(); i++) {
    //     Int32 uid = (*uids)[i];
        // helper->ResetKernelUidStats(uid);
    // }
}

void CNetworkStatsService::RemoveUserLocked(
    /* [in] */ Int32 userId)
{
    if (LOGV) Slogger::V(TAG, "removeUserLocked() for userId=%d", userId);

    // Build list of UIDs that we should clean up
    AutoPtr<ArrayOf<Int32> > uids = ArrayOf<Int32>::Alloc(0);
    AutoPtr<IPackageManager> packageMgr;
    mContext->GetPackageManager((IPackageManager**)&packageMgr);
    AutoPtr<IList> apps;
    packageMgr->GetInstalledApplications(
            IPackageManager::GET_UNINSTALLED_PACKAGES | IPackageManager::GET_DISABLED_COMPONENTS,
            (IList**)&apps);
    FOR_EACH(iter, apps) {
        AutoPtr<IApplicationInfo> app = IApplicationInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        Int32 uid = UserHandle::GetUid(userId, Ptr(app)->Func(app->GetUid));
        uids = ArrayUtils::AppendInt32(uids, uid);
    }
    RemoveUidsLocked(uids);
}

void CNetworkStatsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ const ArrayOf<String>& args)
{
    if (FAILED(mContext->EnforceCallingOrSelfPermission(String("android.permission.DUMP"), TAG))) {
        return;
    }

    AutoPtr<IHashSet> argSet;
    CHashSet::New((IHashSet**)&argSet);
    for(Int32 i = 0; i < args.GetLength(); i++) {
        argSet->Add(StringUtils::ParseCharSequence(args[i]));
    }

    // usage: dumpsys netstats --full --uid --tag --poll --checkin
    Boolean poll;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("--poll")), &poll);
    Boolean temp;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("poll")), &temp);
    poll = poll || temp;
    Boolean checkin;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("--checkin")), &checkin);
    Boolean fullHistory;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("--full")), &fullHistory);
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("full")), &temp);
    fullHistory = fullHistory || temp;
    Boolean includeUid;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("--uid")), &includeUid);
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("detail")), &temp);
    includeUid = includeUid || temp;
    Boolean includeTag;
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("--tag")), &includeTag);
    ISet::Probe(argSet)->Contains(StringUtils::ParseCharSequence(String("detail")), &temp);
    includeTag = includeTag || temp;

    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&pw);

    synchronized(mStatsLock) {
        if (poll) {
            PerformPollLocked(FLAG_PERSIST_ALL | FLAG_PERSIST_FORCE);
            IPrintWriter::Probe(pw)->Println(String("Forced poll"));
            return;
        }

        if (checkin) {
            // list current stats files to verify rotation
            IPrintWriter::Probe(pw)->Println(String("Current files:"));
            pw->IncreaseIndent();
            AutoPtr<ArrayOf<String> > files;
            mBaseDir->List((ArrayOf<String>**)&files);
            for(Int32 i = 0; i < files->GetLength(); i++) {
                String file = (*files)[i];
                IPrintWriter::Probe(pw)->Println(file);
            }
            pw->DecreaseIndent();
            return;
        }

        IPrintWriter::Probe(pw)->Println(String("Active interfaces:"));
        for (Int32 i = 0; i < Ptr(mActiveIfaces)->Func(mActiveIfaces->GetSize); i++) {
            AutoPtr<IInterface> key, value;
            mActiveIfaces->GetKeyAt(i, (IInterface**)&key);
            pw->PrintPair(String("iface"), key);
            mActiveIfaces->GetValueAt(i, (IInterface**)&value);
            pw->PrintPair(String("ident"), value);
            IPrintWriter::Probe(pw)->Println();
        }
        pw->DecreaseIndent();

        IPrintWriter::Probe(pw)->Println(String("Active UID interfaces:"));
        pw->IncreaseIndent();
        for (Int32 i = 0; i < Ptr(mActiveUidIfaces)->Func(mActiveUidIfaces->GetSize); i++) {
            AutoPtr<IInterface> key, value;
            mActiveUidIfaces->GetKeyAt(i, (IInterface**)&key);
            pw->PrintPair(String("iface"), key);
            mActiveUidIfaces->GetValueAt(i, (IInterface**)&value);
            pw->PrintPair(String("ident"), value);
            IPrintWriter::Probe(pw)->Println();
        }
        pw->IncreaseIndent();

        pw->DecreaseIndent();

        IPrintWriter::Probe(pw)->Println(String("Dev stats:"));
        pw->IncreaseIndent();
        mDevRecorder->DumpLocked(pw, fullHistory);
        pw->DecreaseIndent();

        IPrintWriter::Probe(pw)->Println(String("Xt stats:"));
        pw->IncreaseIndent();
        mXtRecorder->DumpLocked(pw, fullHistory);
        pw->DecreaseIndent();

        if (includeUid) {
            IPrintWriter::Probe(pw)->Println(String("UID stats:"));
            pw->IncreaseIndent();
            mUidRecorder->DumpLocked(pw, fullHistory);
            pw->DecreaseIndent();
        }

        if (includeTag) {
            IPrintWriter::Probe(pw)->Println(String("UID tag stats:"));
            pw->IncreaseIndent();
            mUidTagRecorder->DumpLocked(pw, fullHistory);
            pw->DecreaseIndent();
        }
    }
}

ECode CNetworkStatsService::GetNetworkStatsUidDetail(
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = NULL;
    AutoPtr<INetworkStats> uidSnapshot;
    FAIL_RETURN(mNetworkManager->GetNetworkStatsUidDetail(INetworkStats::UID_ALL, (INetworkStats**)&uidSnapshot))

    // fold tethering stats and operations into uid snapshot
    AutoPtr<INetworkStats> tetherSnapshot;
    FAIL_RETURN(GetNetworkStatsTethering((INetworkStats**)&tetherSnapshot))
    uidSnapshot->CombineAllValues(tetherSnapshot);
    uidSnapshot->CombineAllValues(mUidOperations);

    *stats = uidSnapshot;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode CNetworkStatsService::GetNetworkStatsTethering(
    /* [out] */ INetworkStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = NULL;
    //try {
    ECode ec = mNetworkManager->GetNetworkStatsTethering(stats);
    if (FAILED(ec)) {
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Slogger::E(TAG, "problem reading network stats");
            return CNetworkStats::New(0L, 10, stats);
        }
        return ec;
    }
    return NOERROR;
    //} catch (IllegalStateException e) {
    //    Log.wtf(TAG, "problem reading network stats", e);
    //    return new NetworkStats(0L, 10);
    //}
}

void CNetworkStatsService::HandleMsgPerformPoll(
    /* [in] */ Int32 flags)
{
    PerformPoll(flags);
}

void CNetworkStatsService::HandleMsgUpdateIfaces()
{
    UpdateIfaces();
}

void CNetworkStatsService::HandleMsgRegisterGlobalAlert()
{
    RegisterGlobalAlert();
}

ECode CNetworkStatsService::AssertBandwidthControlEnabled()
{
    if (!IsBandwidthControlEnabled()) {
        //throw new IllegalStateException("Bandwidth module disabled");
        Slogger::E(TAG, "Bandwidth module disabled");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Boolean CNetworkStatsService::IsBandwidthControlEnabled()
{
    Int64 token = Binder::ClearCallingIdentity();
    Boolean bol;
    ECode ec = mNetworkManager->IsBandwidthControlEnabled(&bol);
    Binder::RestoreCallingIdentity(token);
    if (FAILED(ec)) {
        // ignored; service lives in system_server
        return FALSE;
    }
    return bol;
}

AutoPtr<NetworkIdentitySet> CNetworkStatsService::FindOrCreateNetworkIdentitySet(
    /* [in] */ IArrayMap* map,
    /* [in] */ IInterface* key)
{
    AutoPtr<IInterface> value;
    map->Get(key, (IInterface**)&value);
    AutoPtr<NetworkIdentitySet> ident = (NetworkIdentitySet*)IHashSet::Probe(value);
    if (ident == NULL) {
        ident = new NetworkIdentitySet();
        ident->constructor();
        map->Put(key, TO_IINTERFACE(ident));
    }
    return ident;
}

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos
