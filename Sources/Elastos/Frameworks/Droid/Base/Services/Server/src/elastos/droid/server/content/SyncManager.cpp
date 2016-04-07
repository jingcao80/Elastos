#include "elastos/droid/server/content/SyncManager.h"
#include "elastos/droid/server/content/SyncOperation.h"
#include "elastos/droid/server/content/SyncStorageEngine.h"
#include "elastos/droid/server/content/CSyncStatusObserver.h"
#include "elastos/droid/server/content/CActiveSyncContext.h"
// #include "elastos/droid/server/accounts/CAccountManagerService.h"
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Accounts.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Accounts::IAccountManager;
// using Elastos::Droid::Accounts::CAccountAndUser;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISyncStats;
using Elastos::Droid::Content::CSyncResult;
using Elastos::Droid::Content::CSyncAdaptersCache;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::ISyncAdapterTypeHelper;
using Elastos::Droid::Content::CSyncAdapterTypeHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IPeriodicSync;
using Elastos::Droid::Content::IContentResolverHelper;
using Elastos::Droid::Content::CContentResolverHelper;
using Elastos::Droid::Content::ISyncAdaptersCache;
using Elastos::Droid::Content::ISyncStatusInfo;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IISyncContext;
using Elastos::Droid::Content::EIID_IISyncStatusObserver;
// using Elastos::Droid::Content::ECLSID_CSyncActivityTooManyDeletes;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IRegisteredServicesCache;
using Elastos::Droid::Content::Pm::EIID_IRegisteredServicesCacheListener;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheServiceInfo;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Internal::Os::IBackgroundThread;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
// using Elastos::Droid::Server::Accounts::CAccountManagerService;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::IRandom;
using Elastos::Utility::CRandom;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::Set;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

const Int32 SyncManager::SyncHandler::MESSAGE_SYNC_FINISHED = 1;
const Int32 SyncManager::SyncHandler::MESSAGE_SYNC_ALARM = 2;
const Int32 SyncManager::SyncHandler::MESSAGE_CHECK_ALARMS = 3;
const Int32 SyncManager::SyncHandler::MESSAGE_SERVICE_CONNECTED = 4;
const Int32 SyncManager::SyncHandler::MESSAGE_SERVICE_DISCONNECTED = 5;
const Int32 SyncManager::SyncHandler::MESSAGE_CANCEL = 6;
const Int32 SyncManager::SyncHandler::MESSAGE_SYNC_EXPIRED = 7;

const String SyncManager::TAG("SyncManager");

const Int64 SyncManager::INITIAL_SYNC_RETRY_TIME_IN_MS = 30 * 1000; // 30 seconds
const Int64 SyncManager::DEFAULT_MAX_SYNC_RETRY_TIME_IN_SECONDS = 60 * 60; // one hour
const Int32 SyncManager::DELAY_RETRY_SYNC_IN_PROGRESS_IN_SECONDS = 10;
const Int64 SyncManager::ACTIVE_SYNC_TIMEOUT_MILLIS = 30L * 60 * 1000;  // 30 mins.

const String SyncManager::SYNC_WAKE_LOCK_PREFIX("*sync*/");
const String SyncManager::HANDLE_SYNC_ALARM_WAKE_LOCK("SyncManagerHandleSyncAlarm");
const String SyncManager::SYNC_LOOP_WAKE_LOCK("SyncLoopWakeLock");

const String SyncManager::ACTION_SYNC_ALARM("android.content.syncmanager.SYNC_ALARM");

const Int64 SyncManager::SYNC_ALARM_TIMEOUT_MIN = 30 * 1000; // 30 seconds
const Int64 SyncManager::SYNC_ALARM_TIMEOUT_MAX = 2 * 60 * 60 * 1000; // two hours

const AutoPtr<ArrayOf<IAccountAndUser*> > SyncManager::INITIAL_ACCOUNTS_ARRAY = ArrayOf<IAccountAndUser*>::Alloc(0);

Int64 SyncManager::LOCAL_SYNC_DELAY;
Int64 SyncManager::MAX_TIME_PER_SYNC;
Int64 SyncManager::SYNC_NOTIFICATION_DELAY;
Int32 SyncManager::MAX_SIMULTANEOUS_REGULAR_SYNCS;
Int32 SyncManager::MAX_SIMULTANEOUS_INITIALIZATION_SYNCS;

const SyncManager::StaticInitializer SyncManager::sInitializer;

SyncManager::StaticInitializer::StaticInitializer()
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Boolean isLargeRAM;
    amHelper->IsLowRamDeviceStatic(&isLargeRAM);
    isLargeRAM = !isLargeRAM;
    Int32 defaultMaxInitSyncs = isLargeRAM ? 5 : 2;
    Int32 defaultMaxRegularSyncs = isLargeRAM ? 2 : 1;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->GetInt32(String("sync.max_init_syncs"), defaultMaxInitSyncs,
        &SyncManager::MAX_SIMULTANEOUS_INITIALIZATION_SYNCS);
    sysProp->GetInt32(String("sync.max_regular_syncs"), defaultMaxRegularSyncs,
        &SyncManager::MAX_SIMULTANEOUS_REGULAR_SYNCS);
    sysProp->GetInt64(String("sync.local_sync_delay"), 30 * 1000 /* 30 seconds */,
        &SyncManager::LOCAL_SYNC_DELAY);
    sysProp->GetInt64(String("sync.max_time_per_sync"), 5 * 60 * 1000 /* 5 minutes */,
        &SyncManager::MAX_TIME_PER_SYNC);
    sysProp->GetInt64(String("sync.notification_delay"), 30 * 1000 /* 30 seconds */,
        &SyncManager::SYNC_NOTIFICATION_DELAY);
}

//===============================================================================
// SyncManager::SyncHandlerMessagePayload
//===============================================================================
SyncManager::SyncHandlerMessagePayload::SyncHandlerMessagePayload(
    /* [in] */ ActiveSyncContext* syncContext,
    /* [in] */ ISyncResult* syncResult)
    : mActiveSyncContext(syncContext)
    , mSyncResult(syncResult)
{
}

//===============================================================================
// SyncManager::SyncAlarmIntentReceiver
//===============================================================================
SyncManager::SyncAlarmIntentReceiver::SyncAlarmIntentReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{}

//@Override
ECode SyncManager::SyncAlarmIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mHandleAlarmWakeLock->AcquireLock();
    mHost->SendSyncAlarmMessage();
    return NOERROR;
}

//===============================================================================
// SyncManager::ActiveSyncContext
//===============================================================================

CAR_INTERFACE_IMPL_5(SyncManager::ActiveSyncContext, Object, IActiveSyncContext, \
    IISyncContext, IBinder, IServiceConnection, IProxyDeathRecipient)

SyncManager::ActiveSyncContext::ActiveSyncContext()
    : mHistoryRowId(0)
    , mStartTime(-1)
    , mTimeoutStartTime(0)
    , mBound(FALSE)
    , mSyncAdapterUid(0)
    , mIsLinkedToDeath(FALSE)
{}

SyncManager::ActiveSyncContext::~ActiveSyncContext()
{}

ECode SyncManager::ActiveSyncContext::constructor(
    /* [in] */ ISyncManager* syncMgr,
    /* [in] */ ISyncOperation* syncOperation,
    /* [in] */ Int64 historyRowId,
    /* [in] */ Int32 syncAdapterUid)
{
    mHost = (SyncManager*)syncMgr;
    mSyncAdapterUid = syncAdapterUid;
    mSyncOperation = (SyncOperation*)syncOperation;
    mHistoryRowId = historyRowId;
    mStartTime = SystemClock::GetElapsedRealtime();
    mTimeoutStartTime = mStartTime;
    mSyncWakeLock = mHost->mSyncHandler->GetSyncWakeLock(mSyncOperation);
    AutoPtr<IWorkSource> ws;
    CWorkSource::New(syncAdapterUid, (IWorkSource**)&ws);
    mSyncWakeLock->SetWorkSource(ws);
    mSyncWakeLock->AcquireLock();
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::SendHeartbeat()
{
    // heartbeats are no longer used
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::OnFinished(
    /* [in] */ ISyncResult* result)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "onFinished: " + this);
    // include "this" in the message so that the handler can ignore it if this
    // ActiveSyncContext is no longer the mActiveSyncContext at message handling
    // time
    mHost->SendSyncFinishedOrCanceledMessage(this, result);
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::ToString(
    /* [in] */ StringBuilder& sb)
{
    sb.Append("startTime ");
    sb.Append(mStartTime);
    sb.Append(", mTimeoutStartTime ");
    sb.Append(mTimeoutStartTime);
    sb.Append(", mHistoryRowId ");
    sb.Append(mHistoryRowId);
    sb.Append(", syncOperation ");
    sb.Append(Object::ToString(mSyncOperation));
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IMessage> msg;
    mHost->mSyncHandler->ObtainMessage((IMessage**)&msg);
    msg->SetWhat(SyncHandler::MESSAGE_SERVICE_CONNECTED);
    AutoPtr<ServiceConnectionData> data = new ServiceConnectionData(this, service);
    msg->SetObj(TO_IINTERFACE(data));
    Boolean bval;
    mHost->mSyncHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoPtr<IMessage> msg;
    mHost->mSyncHandler->ObtainMessage((IMessage**)&msg);
    msg->SetWhat(SyncHandler::MESSAGE_SERVICE_DISCONNECTED);
    AutoPtr<ServiceConnectionData> data = new ServiceConnectionData(this, NULL);
    msg->SetObj(TO_IINTERFACE(data));
    Boolean bval;
    mHost->mSyncHandler->SendMessage(msg, &bval);
    return NOERROR;
}

Boolean SyncManager::ActiveSyncContext::BindToSyncAdapter(
    /* [in] */ IComponentName* serviceComponent,
    /* [in] */ Int32 userId)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::D(TAG, "bindToSyncAdapter: " + serviceComponent + ", connection " + this);
    // }
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetAction(String("android.content.SyncAdapter"));
    intent->SetComponent(serviceComponent);
    intent->PutExtra(IIntent::EXTRA_CLIENT_LABEL, R::string::sync_binding_label);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IIntent> settingsIntent;
    CIntent::New(ISettings::ACTION_SYNC_SETTINGS, (IIntent**)&settingsIntent);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(userId, (IUserHandle**)&uh);
    AutoPtr<IPendingIntent> pi;
    helper->GetActivityAsUser(mHost->mContext, 0, settingsIntent, 0,
        NULL, uh, (IPendingIntent**)&pi);
    intent->PutExtra(IIntent::EXTRA_CLIENT_INTENT, IParcelable::Probe(pi));
    mBound = TRUE;
    AutoPtr<IUserHandle> user;
    CUserHandle::New(mSyncOperation->mTarget->mUserId, (IUserHandle**)&user);
    Boolean bindResult;
    mHost->mContext->BindServiceAsUser(intent, this,
        IContext::BIND_AUTO_CREATE | IContext::BIND_NOT_FOREGROUND
            | IContext::BIND_ALLOW_OOM_MANAGEMENT,
        user, &bindResult);
    if (!bindResult) {
        mBound = FALSE;
    }
    else {

        mEventName = mSyncOperation->WakeLockName();
        mHost->mBatteryStats->NoteSyncStart(mEventName, mSyncAdapterUid);
    }
    return bindResult;
}

ECode SyncManager::ActiveSyncContext::Close()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::D(TAG, "unBindFromSyncAdapter: connection " + this);
    // }
    if (mBound) {
        mBound = FALSE;
        mHost->mContext->UnbindService(this);
        mHost->mBatteryStats->NoteSyncFinish(mEventName, mSyncAdapterUid);
    }
    mSyncWakeLock->ReleaseLock();
    mSyncWakeLock->SetWorkSource(NULL);
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    ToString(sb);
    *str = sb.ToString();
    return NOERROR;
}

ECode SyncManager::ActiveSyncContext::ProxyDied()
{
    mHost->SendSyncFinishedOrCanceledMessage(this, NULL);
    return NOERROR;
}

//===============================================================================
// SyncManager::ServiceConnectionData
//===============================================================================

SyncManager::ServiceConnectionData::ServiceConnectionData(
    /* [in] */ ActiveSyncContext* activeSyncContext,
    /* [in] */ IBinder* adapter)
    : mActiveSyncContext(activeSyncContext)
    , mAdapter(adapter)
{
}

//===============================================================================
// SyncManager::PrintTable
//===============================================================================

SyncManager::PrintTable::PrintTable(
    /* [in] */ Int32 cols)
    : mCols(cols)
{
}

ECode SyncManager::PrintTable::Set(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [in] */ ArrayOf<String>* values)
{
    if (col + values->GetLength() > mCols) {
        Logger::E("SyncManager",
            "Table only has %d columns. can't set %d at column %d",
            mCols, values->GetLength(), col);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }


    Int32 size = mTable.GetSize();
    for (Int32 i = size; i <= row; ++i) {
        AutoPtr<ArrayOf<String> > list = ArrayOf<String>::Alloc(mCols);
        mTable.PushBack(list);
        for (Int32 j = 0; j < mCols; j++) {
            (*list)[j] = "";
        }
    }

    AutoPtr<ArrayOf<String> > array = mTable[row];
    array->Copy(col, values, 0, values->GetLength());
    return NOERROR;
}

void SyncManager::PrintTable::WriteTo(
    /* [in] */ IPrintWriter* out)
{
    AutoPtr< ArrayOf<String> > formats = ArrayOf<String>::Alloc(mCols);
    List<AutoPtr<ArrayOf<String> > >::Iterator it;
    Int32 totalLength = 0;
    for (Int32 col = 0; col < mCols; ++col) {
        Int32 maxLength = 0;
        for (it = mTable.Begin(); it != mTable.End(); ++it) {
            AutoPtr< ArrayOf<String> > row = *it;
            Int32 length = (*row)[col].GetLength();
            if (length > maxLength) {
                maxLength = length;
            }
        }
        totalLength += maxLength;
        String str;
        str.AppendFormat("%%-%ds", maxLength);
        (*formats)[col] = str;
    }

    (*formats)[mCols - 1] = "%s";
    PrintRow(out, formats, mTable[0]);
    totalLength += (mCols - 1) * 2;
    for (Int32 i = 0; i < totalLength; ++i) {
        out->Print(String("-"));
    }
    out->Println();

    it = mTable.Begin();
    ++it;
    for (; it != mTable.End(); ++it) {
        AutoPtr< ArrayOf<String> > row = *it;
        PrintRow(out, formats, row);
    }
}

Int32 SyncManager::PrintTable::GetNumRows()
{
    return mTable.GetSize();
}

void SyncManager::PrintTable::PrintRow(
    /* [in] */ IPrintWriter* out,
    /* [in] */ ArrayOf<String>* formats,
    /* [in] */ ArrayOf<String>* row)
{
    for (Int32 j = 0, rowLength = row->GetLength(); j < rowLength; j++) {
        String str;
        str.AppendFormat((*formats)[j].string(), (*row)[j].string());
        out->Print(String("  "));
    }
    out->Println();
}


//=================================================================================
// SyncManager::SyncHandler::SyncNotificationInfo
//=================================================================================

ECode SyncManager::SyncHandler::SyncNotificationInfo::ToString(
    /* [in] */ StringBuilder& sb)
{
    sb.Append("isActive ");
    sb.Append(mIsActive ? "TRUE" : "FALSE");
    sb.Append(", startTime ");
    sb.Append(mStartTime);
    return NOERROR;
}

//@Override
ECode SyncManager::SyncHandler::SyncNotificationInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    ToString(sb);
    *str = sb.ToString();
    return NOERROR;
}

//=================================================================================
// SyncManager::SyncHandler
//=================================================================================

SyncManager::SyncHandler::SyncHandler(
    /* [in] */ SyncManager* host)
    : mHost(host)
    , mAlarmScheduleTime(-1)
{
    mBootQueue = new List<AutoPtr<IMessage> >();
    mSyncTimeTracker = new SyncTimeTracker(host);
    mSyncNotificationInfo = new SyncNotificationInfo();
}

ECode SyncManager::SyncHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return Handler::constructor(looper);
}

ECode SyncManager::SyncHandler::OnBootCompleted()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "Boot completed, clearing boot queue.");
    // }
    mHost->DoDatabaseCleanup();

    synchronized(this) {
        // Dispatch any stashed messages.
        Boolean bval;
        List<AutoPtr<IMessage> >::Iterator it;
        for (it = mBootQueue->Begin(); it != mBootQueue->End(); ++it) {
            AutoPtr<IMessage> message;
            SendMessage(message, &bval);
        }
        mBootQueue = NULL;
        mHost->mBootCompleted = TRUE;
    }
    return NOERROR;
}

AutoPtr<IPowerManagerWakeLock> SyncManager::SyncHandler::GetSyncWakeLock(
    /* [in] */ SyncOperation* operation)
{
    String wakeLockKey = operation->WakeLockName();

    AutoPtr<IPowerManagerWakeLock> wakeLock;
    HashMap<String, AutoPtr<IPowerManagerWakeLock> >::Iterator it = mWakeLocks.Find(wakeLockKey);
    if (it != mWakeLocks.End()) {
        wakeLock = it->mSecond;
    }

    if (wakeLock == NULL) {
        String name(SyncManager::SYNC_WAKE_LOCK_PREFIX);
        name += wakeLockKey;
        mHost->mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, name, (IPowerManagerWakeLock**)&wakeLock);
        wakeLock->SetReferenceCounted(FALSE);
        mWakeLocks[wakeLockKey] = wakeLock;
    }
    return wakeLock;
}

Boolean SyncManager::SyncHandler::TryEnqueueMessageUntilReadyToRun(
    /* [in] */ IMessage* msg)
{
    synchronized(this) {
        if (!mHost->mBootCompleted) {
            // Need to copy the message bc looper will recycle it.
            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            AutoPtr<IMessage> msg;
            helper->Obtain((IMessage**)&msg);
            mBootQueue->PushBack(msg);
            return TRUE;
        }
    }
    return FALSE;
}

ECode SyncManager::SyncHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (TryEnqueueMessageUntilReadyToRun(msg)) {
        return NOERROR;
    }

    using Elastos::Core::Math;
    Int64 earliestFuturePollTime = Math::INT64_MAX_VALUE;
    Int64 nextPendingSyncTime = Math::INT64_MAX_VALUE;
    // Setting the value here instead of a method because we want the dumpsys logs
    // to have the most recent value used.

    mHost->mDataConnectionIsConnected = mHost->ReadDataConnectionState();
    mHost->mSyncManagerWakeLock->AcquireLock();
    // Always do this first so that we be sure that any periodic syncs that
    // are ready to run have been converted into pending syncs. This allows the
    // logic that considers the next steps to take based on the set of pending syncs
    // to also take into account the periodic syncs.
    earliestFuturePollTime = ScheduleReadyPeriodicSyncs();

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case SyncHandler::MESSAGE_SYNC_EXPIRED: {
            ActiveSyncContext* expiredContext = (ActiveSyncContext*)IObject::Probe(obj);
            // if (Log.isLoggable(TAG, Log.DEBUG)) {
            //     Logger::D(TAG, "handleSyncHandlerMessage: MESSAGE_SYNC_EXPIRED: expiring "
            //             + expiredContext);
            // }
            mHost->CancelActiveSync(expiredContext->mSyncOperation->mTarget,
                expiredContext->mSyncOperation->mExtras);
            nextPendingSyncTime = MaybeStartNextSyncLocked();
            break;
        }

        case SyncHandler::MESSAGE_CANCEL: {
            EndPoint* payload = (EndPoint*)IObject::Probe(obj);
            AutoPtr<IBundle> extras;
            msg->PeekData((IBundle**)&extras);
            // if (Log.isLoggable(TAG, Log.DEBUG)) {
            //     Logger::D(TAG, "handleSyncHandlerMessage: MESSAGE_SERVICE_CANCEL: "
            //             + payload + " bundle: " + extras);
            // }
            CancelActiveSyncLocked(payload, extras);
            nextPendingSyncTime = MaybeStartNextSyncLocked();
            break;
        }

        case SyncHandler::MESSAGE_SYNC_FINISHED: {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "handleSyncHandlerMessage: MESSAGE_SYNC_FINISHED");
            // }
            SyncHandlerMessagePayload* payload = (SyncHandlerMessagePayload*)IObject::Probe(obj);
            if (!mHost->IsSyncStillActive(payload->mActiveSyncContext)) {
                // Logger::D(TAG, "handleSyncHandlerMessage: dropping since the "
                //         + "sync is no longer active: "
                //         + payload.activeSyncContext);
                break;
            }
            RunSyncFinishedOrCanceledLocked(payload->mSyncResult,
                payload->mActiveSyncContext);

            // since a sync just finished check if it is time to start a new sync
            nextPendingSyncTime = MaybeStartNextSyncLocked();
            break;
        }

        case SyncHandler::MESSAGE_SERVICE_CONNECTED: {
            ServiceConnectionData* msgData = (ServiceConnectionData*)IObject::Probe(obj);
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::D(TAG, "handleSyncHandlerMessage: MESSAGE_SERVICE_CONNECTED: "
            //             + msgData.activeSyncContext);
            // }
            // check that this isn't an old message
            if (mHost->IsSyncStillActive(msgData->mActiveSyncContext)) {
                RunBoundToAdapter(msgData->mActiveSyncContext, msgData->mAdapter);
            }
            break;
        }

        case SyncHandler::MESSAGE_SERVICE_DISCONNECTED: {
            ServiceConnectionData* msgData = (ServiceConnectionData*)IObject::Probe(obj);
            ActiveSyncContext* currentSyncContext = msgData->mActiveSyncContext;
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::D(TAG, "handleSyncHandlerMessage: MESSAGE_SERVICE_DISCONNECTED: "
            //             + currentSyncContext);
            // }
            // check that this isn't an old message
            if (mHost->IsSyncStillActive(currentSyncContext)) {
                // cancel the sync if we have a syncadapter, which means one is
                // outstanding
                // try {
                if (currentSyncContext->mSyncAdapter != NULL) {
                    currentSyncContext->mSyncAdapter->CancelSync(currentSyncContext);
                }
                else if (currentSyncContext->mSyncServiceAdapter != NULL) {
                    currentSyncContext->mSyncServiceAdapter->CancelSync(currentSyncContext);
                }
                // } catch (RemoteException e) {
                //     // We don't need to retry this in this case.
                // }

                // pretend that the sync failed with an IOException,
                // which is a soft error
                AutoPtr<ISyncResult> syncResult;
                CSyncResult::New((ISyncResult**)&syncResult);
                AutoPtr<ISyncStats> stats;
                syncResult->GetStats((ISyncStats**)&stats);
                Int64 num;
                stats->GetNumIoExceptions(&num);
                stats->SetNumIoExceptions(++num);
                RunSyncFinishedOrCanceledLocked(syncResult, currentSyncContext);

                // since a sync just finished check if it is time to start a new sync
                nextPendingSyncTime = MaybeStartNextSyncLocked();
            }

            break;
        }

        case SyncHandler::MESSAGE_SYNC_ALARM: {
            // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
            // if (isLoggable) {
            //     Logger::V(TAG, "handleSyncHandlerMessage: MESSAGE_SYNC_ALARM");
            // }
            mAlarmScheduleTime = -1;
            // try {
                nextPendingSyncTime = MaybeStartNextSyncLocked();
            // } finally {
                mHost->mHandleAlarmWakeLock->ReleaseLock();
            // }
            break;
        }

        case SyncHandler::MESSAGE_CHECK_ALARMS:
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "handleSyncHandlerMessage: MESSAGE_CHECK_ALARMS");
            // }
            nextPendingSyncTime = MaybeStartNextSyncLocked();
            break;
    }

    ManageSyncNotificationLocked();
    ManageSyncAlarmLocked(earliestFuturePollTime, nextPendingSyncTime);
    mSyncTimeTracker->Update();
    mHost->mSyncManagerWakeLock->ReleaseLock();
    return NOERROR;
}

Boolean SyncManager::SyncHandler::IsDispatchable(
    /* [in] */ EndPoint* target)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    if (target->mTarget_provider) {
        // skip the sync if the account of this operation no longer exists
        AutoPtr<ArrayOf<IAccountAndUser*> > accounts = mHost->mRunningAccounts;
        if (!mHost->ContainsAccountAndUser(accounts, target->mAccount, target->mUserId)) {
            return FALSE;
        }
        if (!mHost->mSyncStorageEngine->GetMasterSyncAutomatically(target->mUserId)
                || !mHost->mSyncStorageEngine->GetSyncAutomatically(
                    target->mAccount, target->mUserId, target->mProvider)) {
            // if (isLoggable) {
            //     Logger::V(TAG, "    Not scheduling periodic operation: sync turned off.");
            // }
            return FALSE;
        }
        if (mHost->GetIsSyncable(target->mAccount, target->mUserId, target->mProvider) == 0) {
            // if (isLoggable) {
            //     Logger::V(TAG, "    Not scheduling periodic operation: isSyncable == 0.");
            // }
            return FALSE;
        }
    }
    else if (target->mTarget_service) {
        if (mHost->mSyncStorageEngine->GetIsTargetServiceActive(target->mService, target->mUserId)) {
            // if (isLoggable) {
            //     Logger::V(TAG, "   Not scheduling periodic operation: isEnabled == 0.");
            // }
            return FALSE;
        }
    }
    return TRUE;
}

Int64 SyncManager::SyncHandler::ScheduleReadyPeriodicSyncs()
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    // if (isLoggable) {
    //     Logger::V(TAG, "ScheduleReadyPeriodicSyncs");
    // }
    using Elastos::Core::Math;
    Int64 earliestFuturePollTime = Math::INT64_MAX_VALUE;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nowAbsolute;
    system->GetCurrentTimeMillis(&nowAbsolute);
    Int64 shiftedNowAbsolute = (0 < nowAbsolute - mHost->mSyncRandomOffsetMillis)
            ? (nowAbsolute - mHost->mSyncRandomOffsetMillis) : 0;

    AutoPtr<List<AutoPtr<AuthoritySyncStatusPair> > > infos;
    infos = mHost->mSyncStorageEngine->GetCopyOfAllAuthoritiesWithSyncStatus();

    List<AutoPtr<AuthoritySyncStatusPair> >::Iterator it;
    List<AutoPtr<IPeriodicSync> >::Iterator psIt;
    for (it = infos->Begin(); it != infos->End(); ++it) {
        AutoPtr<AuthoritySyncStatusPair> info = *it;
        AutoPtr<AuthorityInfo> authorityInfo = info->mFirst;
        AutoPtr<ISyncStatusInfo> status = info->mSecond;

        if (TextUtils::IsEmpty(authorityInfo->mTarget->mProvider)) {
            Logger::E(TAG, "Got an empty provider string. Skipping: %s",
                authorityInfo->mTarget->mProvider.string());
            continue;
        }

        if (!IsDispatchable(authorityInfo->mTarget)) {
            continue;
        }

        psIt = authorityInfo->mPeriodicSyncs.Begin();
        Int32 i = 0;
        for (; psIt != authorityInfo->mPeriodicSyncs.End(); ++psIt, ++i) {
            AutoPtr<IPeriodicSync> sync = *psIt;
            AutoPtr<IBundle> extras;
            sync->GetExtras((IBundle**)&extras);
            Int64 periodInMillis, flexInMillis;
            sync->GetPeriod(&periodInMillis);
            periodInMillis *= 1000;
            sync->GetFlexTime(&flexInMillis);
            flexInMillis *= 1000;
            // Skip if the period is invalid.
            if (periodInMillis <= 0) {
                continue;
            }
            // Find when this periodic sync was last scheduled to run.
            Int64 lastPollTimeAbsolute;
            status->GetPeriodicSyncTime(i, &lastPollTimeAbsolute);
            // Int64 shiftedLastPollTimeAbsolute =
            //     (0 < lastPollTimeAbsolute - mHost->mSyncRandomOffsetMillis) ?
            //             (lastPollTimeAbsolute - mHost->mSyncRandomOffsetMillis) : 0;
            Int64 remainingMillis = periodInMillis - (shiftedNowAbsolute % periodInMillis);
            Int64 timeSinceLastRunMillis = (nowAbsolute - lastPollTimeAbsolute);
            // Schedule this periodic sync to run early if it's close enough to its next
            // runtime, and far enough from its last run time.
            // If we are early, there will still be time remaining in this period.
            Boolean runEarly = remainingMillis <= flexInMillis
                && timeSinceLastRunMillis > periodInMillis - flexInMillis;
            // if (isLoggable) {
            //     Logger::V(TAG, "sync: " + i + " for " + authorityInfo.target + "."
            //     + " period: " + (periodInMillis)
            //     + " flex: " + (flexInMillis)
            //     + " remaining: " + (remainingMillis)
            //     + " time_since_last: " + timeSinceLastRunMillis
            //     + " last poll absol: " + lastPollTimeAbsolute
            //     + " last poll shifed: " + shiftedLastPollTimeAbsolute
            //     + " shifted now: " + shiftedNowAbsolute
            //     + " run_early: " + runEarly);
            // }
            /*
             * Sync scheduling strategy: Set the next periodic sync
             * based on a random offset (in seconds). Also sync right
             * now if any of the following cases hold and mark it as
             * having been scheduled
             * Case 1: This sync is ready to run now.
             * Case 2: If the lastPollTimeAbsolute is in the
             * future, sync now and reinitialize. This can happen for
             * example if the user changed the time, synced and changed
             * back.
             * Case 3: If we failed to sync at the last scheduled time.
             * Case 4: This sync is close enough to the time that we can schedule it.
             */
            if (remainingMillis == periodInMillis // Case 1
                    || lastPollTimeAbsolute > nowAbsolute // Case 2
                    || timeSinceLastRunMillis >= periodInMillis // Case 3
                    || runEarly) { // Case 4
                // Sync now
                EndPoint* target = authorityInfo->mTarget;
                AutoPtr< Pair<Int64, Int64> > backoff = mHost->mSyncStorageEngine->GetBackoff(target);
                mHost->mSyncStorageEngine->SetPeriodicSyncTime(authorityInfo->mIdent,
                    authorityInfo->mPeriodicSyncs[i], nowAbsolute);

                if (target->mTarget_provider) {
                    AutoPtr<ISyncAdapterTypeHelper> helper;
                    CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&helper);
                    String type;
                    target->mAccount->GetType(&type);
                    AutoPtr<ISyncAdapterType> sat;
                    helper->NewKey(target->mProvider, type, (ISyncAdapterType**)&sat);
                    AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
                    IRegisteredServicesCache::Probe(mHost->mSyncAdapters)->GetServiceInfo(
                        sat, target->mUserId,
                        (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);
                    if (syncAdapterInfo == NULL) {
                        continue;
                    }

                    AutoPtr<IInterface> obj;
                    syncAdapterInfo->GetType((IInterface**)&obj);
                    sat = ISyncAdapterType::Probe(obj);
                    Boolean bval;
                    sat->AllowParallelSyncs(&bval);
                    AutoPtr<SyncOperation> op = new SyncOperation(
                        target->mAccount, target->mUserId,
                        SyncOperation::REASON_PERIODIC, SyncStorageEngine::SOURCE_PERIODIC,
                        target->mProvider, extras, 0 /* runtime */, 0 /* flex */,
                        backoff != NULL ? backoff->mFirst : 0,
                        mHost->mSyncStorageEngine->GetDelayUntilTime(target), bval);
                    mHost->ScheduleSyncOperation(op);
                }
                else if (target->mTarget_service) {
                    AutoPtr<SyncOperation> op = new SyncOperation(
                        target->mService, target->mUserId,
                        SyncOperation::REASON_PERIODIC, SyncStorageEngine::SOURCE_PERIODIC,
                        extras, 0 /* runtime */, 0 /* flex */,
                        backoff != NULL ? backoff->mFirst : 0,
                        mHost->mSyncStorageEngine->GetDelayUntilTime(target));
                    mHost->ScheduleSyncOperation(op);
                }
            }

            // Compute when this periodic sync should next run.
            Int64 nextPollTimeAbsolute;
            if (runEarly) {
                // Add the time remaining so we don't get out of phase.
                nextPollTimeAbsolute = nowAbsolute + periodInMillis + remainingMillis;
            }
            else {
                nextPollTimeAbsolute = nowAbsolute + remainingMillis;
            }
            if (nextPollTimeAbsolute < earliestFuturePollTime) {
                earliestFuturePollTime = nextPollTimeAbsolute;
            }
        }
    }

    if (earliestFuturePollTime == Math::INT64_MAX_VALUE) {
        return Math::INT64_MAX_VALUE;
    }

    // convert absolute time to elapsed time
    Int64 result = SystemClock::GetElapsedRealtime() +
        ((earliestFuturePollTime < nowAbsolute) ? 0 : (earliestFuturePollTime - nowAbsolute));
    return result;
}

Int64 SyncManager::SyncHandler::MaybeStartNextSyncLocked()
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    // if (isLoggable) Logger::V(TAG, "maybeStartNextSync");

    using Elastos::Core::Math;
    // If we aren't ready to run (e.g. the data connection is down), get out.
    if (!mHost->mDataConnectionIsConnected) {
        // if (isLoggable) {
        //     Logger::V(TAG, "maybeStartNextSync: no data connection, skipping");
        // }
        return Math::INT64_MAX_VALUE;
    }

    if (mHost->mStorageIsLow) {
        // if (isLoggable) {
        //     Logger::V(TAG, "maybeStartNextSync: memory low, skipping");
        // }
        return Math::INT64_MAX_VALUE;
    }

    // If the accounts aren't known yet then we aren't ready to run. We will be kicked
    // when the account lookup request does complete.
    if (mHost->mRunningAccounts == INITIAL_ACCOUNTS_ARRAY) {
        // if (isLoggable) {
        //     Logger::V(TAG, "maybeStartNextSync: accounts not known, skipping");
        // }
        return Math::INT64_MAX_VALUE;
    }

    // Otherwise consume SyncOperations from the head of the SyncQueue until one is
    // found that is runnable (not disabled, etc). If that one is ready to run then
    // start it, otherwise just get out.
    Int64 now = SystemClock::GetElapsedRealtime();

    // will be set to the next time that a sync should be considered for running
    Int64 nextReadyToRunTime = Math::INT64_MAX_VALUE;

    // order the sync queue, dropping syncs that are not allowed
    AutoPtr<IList> operations;
    CArrayList::New((IList**)&operations);
    Object* lockObj = mHost->mSyncQueue.Get();
    synchronized(lockObj) {
        // if (isLoggable) {
        //     Logger::V(TAG, "build the operation array, syncQueue size is "
        //         + mSyncQueue.getOperations().size());
        // }
        AutoPtr<IInterface> obj;
        mHost->mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
        AutoPtr<IActivityManager> activityManager = IActivityManager::Probe(obj);

        Set<Int32> removedUsers;
        AutoPtr< HashMap<String, AutoPtr<SyncOperation> > > map = mHost->mSyncQueue->GetOperations();
        HashMap<String, AutoPtr<SyncOperation> >::Iterator it = map->Begin();
        Boolean bval;
        for (; it != map->End();) {
            SyncOperation* op = it->mSecond;

            // If the user is not running, skip the request.
            activityManager->IsUserRunning(op->mTarget->mUserId, &bval);
            if (!bval) {
                AutoPtr<IUserInfo> userInfo;
                mHost->mUserManager->GetUserInfo(op->mTarget->mUserId, (IUserInfo**)&userInfo);
                if (userInfo == NULL) {
                    removedUsers.Insert(op->mTarget->mUserId);
                }
                // if (isLoggable) {
                //     Logger::V(TAG, "    Dropping all sync operations for + "
                //             + op->mTarget->mUserId + ": user not running.");
                // }

                ++it;
                continue;
            }

            if (!IsOperationValidLocked(op)) {
                map->Erase(it++);
                mHost->mSyncStorageEngine->DeleteFromPending(op->mPendingOperation);
                continue;
            }

            ++it;

            // If the next run time is in the future, even given the flexible scheduling,
            // return the time.
            if (op->mEffectiveRunTime - op->mFlexTime > now) {
                if (nextReadyToRunTime > op->mEffectiveRunTime) {
                    nextReadyToRunTime = op->mEffectiveRunTime;
                }
                // if (isLoggable) {
                //     Logger::V(TAG, "    Not running sync operation: Sync too far in future."
                //             + "effective: " + op.effectiveRunTime + " flex: " + op.flexTime
                //             + " now: " + now);
                // }
                continue;
            }
            // Add this sync to be run.
            operations->Add(TO_IINTERFACE(op));
        }

        Set<Int32>::Iterator sit = removedUsers.Begin();
        for (; sit != removedUsers.End(); ++sit) {
            // if it's still removed
            AutoPtr<IUserInfo> ui;
            mHost->mUserManager->GetUserInfo(*sit, (IUserInfo**)&ui);
            if (ui == NULL) {
                mHost->OnUserRemoved(*sit);
            }
        }
    }

    // find the next operation to dispatch, if one is ready
    // iterate from the top, keep issuing (while potentially canceling existing syncs)
    // until the quotas are filled.
    // once the quotas are filled iterate once more to find when the next one would be
    // (also considering pre-emption reasons).
    // if (isLoggable) Logger::V(TAG, "sort the candidate operations, size " + operations.size());
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(operations);
    // if (isLoggable) Logger::V(TAG, "dispatch all ready sync operations");
    AutoPtr<IIterator> it;
    operations->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        SyncOperation* candidate = (SyncOperation*)ISyncOperation::Probe(obj);
        Boolean candidateIsInitialization = candidate->IsInitialization();

        Int32 numInit = 0;
        Int32 numRegular = 0;
        AutoPtr<ActiveSyncContext> conflict;
        AutoPtr<ActiveSyncContext> longRunning = NULL;
        AutoPtr<ActiveSyncContext> toReschedule = NULL;
        AutoPtr<ActiveSyncContext> oldestNonExpeditedRegular = NULL;

        List<AutoPtr<ActiveSyncContext> >::Iterator ascIt;
        for (ascIt = mHost->mActiveSyncContexts.Begin(); ascIt != mHost->mActiveSyncContexts.End(); ++ascIt) {
            ActiveSyncContext* activeSyncContext = *ascIt;
            SyncOperation* activeOp = activeSyncContext->mSyncOperation;
            if (activeOp->IsInitialization()) {
                numInit++;
            }
            else {
                numRegular++;
                if (!activeOp->IsExpedited()) {
                    if (oldestNonExpeditedRegular == NULL
                        || (oldestNonExpeditedRegular->mStartTime > activeSyncContext->mStartTime)) {
                        oldestNonExpeditedRegular = activeSyncContext;
                    }
                }
            }
            if (activeOp->IsConflict(candidate)) {
                conflict = activeSyncContext;
                // don't break out since we want to do a full count of the varieties.
            }
            else {
                if (candidateIsInitialization == activeOp->IsInitialization()
                        && activeSyncContext->mStartTime + MAX_TIME_PER_SYNC < now) {
                    longRunning = activeSyncContext;
                    // don't break out since we want to do a full count of the varieties
                }
            }
        }

        // if (isLoggable) {
        //     Logger::V(TAG, "candidate " + (i + 1) + " of " + N + ": " + candidate);
        //     Logger::V(TAG, "  numActiveInit=" + numInit + ", numActiveRegular=" + numRegular);
        //     Logger::V(TAG, "  longRunning: " + longRunning);
        //     Logger::V(TAG, "  conflict: " + conflict);
        //     Logger::V(TAG, "  oldestNonExpeditedRegular: " + oldestNonExpeditedRegular);
        // }

        Boolean roomAvailable = candidateIsInitialization
                ? numInit < MAX_SIMULTANEOUS_INITIALIZATION_SYNCS
                : numRegular < MAX_SIMULTANEOUS_REGULAR_SYNCS;

        if (conflict != NULL) {
            if (candidateIsInitialization && !conflict->mSyncOperation->IsInitialization()
                    && numInit < MAX_SIMULTANEOUS_INITIALIZATION_SYNCS) {
                toReschedule = conflict;
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Logger::V(TAG, "canceling and rescheduling sync since an initialization "
                //             + "takes higher priority, " + conflict);
                // }
            }
            else if (candidate->IsExpedited() && !conflict->mSyncOperation->IsExpedited()
                    && (candidateIsInitialization
                        == conflict->mSyncOperation->IsInitialization())) {
                toReschedule = conflict;
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Logger::V(TAG, "canceling and rescheduling sync since an expedited "
                //             + "takes higher priority, " + conflict);
                // }
            }
            else {
                continue;
            }
        }
        else if (roomAvailable) {
            // dispatch candidate
        }
        else if (candidate->IsExpedited() && oldestNonExpeditedRegular != NULL
                   && !candidateIsInitialization) {
            // We found an active, non-expedited regular sync. We also know that the
            // candidate doesn't conflict with this active sync since conflict
            // is NULL. Reschedule the active sync and start the candidate.
            toReschedule = oldestNonExpeditedRegular;
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "canceling and rescheduling sync since an expedited is ready to"
            //             + " run, " + oldestNonExpeditedRegular);
            // }
        }
        else if (longRunning != NULL
                && (candidateIsInitialization
                    == longRunning->mSyncOperation->IsInitialization())) {
            // We found an active, Int64-running sync. Reschedule the active
            // sync and start the candidate.
            toReschedule = longRunning;
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "canceling and rescheduling sync since it ran roo Int64, "
            //           + longRunning);
            // }
        }
        else {
            // we were unable to find or make space to run this candidate, go on to
            // the next one
            continue;
        }

        if (toReschedule != NULL) {
            RunSyncFinishedOrCanceledLocked(NULL, toReschedule);
            mHost->ScheduleSyncOperation(toReschedule->mSyncOperation);
        }

        synchronized(lockObj) {
            mHost->mSyncQueue->Remove(candidate);
        }
        DispatchSyncOperation(candidate);
    }

    return nextReadyToRunTime;
}

Boolean SyncManager::SyncHandler::IsOperationValidLocked(
    /* [in] */ SyncOperation* op)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    Int32 targetUid;
    Int32 state;
    EndPoint* target = op->mTarget;
    Boolean syncEnabled = mHost->mSyncStorageEngine->GetMasterSyncAutomatically(target->mUserId);
    if (target->mTarget_provider) {
        // Drop the sync if the account of this operation no longer exists.
        AutoPtr<ArrayOf<IAccountAndUser*> > accounts = mHost->mRunningAccounts;
        if (!mHost->ContainsAccountAndUser(accounts, target->mAccount, target->mUserId)) {
            // if (isLoggable) {
            //     Logger::V(TAG, "    Dropping sync operation: account doesn't exist.");
            // }
            return FALSE;
        }
        // Drop this sync request if it isn't syncable.
        state = mHost->GetIsSyncable(target->mAccount, target->mUserId, target->mProvider);
        if (state == 0) {
            // if (isLoggable) {
            //     Logger::V(TAG, "    Dropping sync operation: isSyncable == 0.");
            // }
            return FALSE;
        }
        syncEnabled = syncEnabled && mHost->mSyncStorageEngine->GetSyncAutomatically(
                target->mAccount, target->mUserId, target->mProvider);

        AutoPtr<ISyncAdapterTypeHelper> helper;
        CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&helper);
        String type;
        target->mAccount->GetType(&type);
        AutoPtr<ISyncAdapterType> sat;
        helper->NewKey(target->mProvider, type, (ISyncAdapterType**)&sat);

        AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
        IRegisteredServicesCache::Probe(mHost->mSyncAdapters)->GetServiceInfo(sat, target->mUserId,
            (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);
        if (syncAdapterInfo != NULL) {
            syncAdapterInfo->GetUid(&targetUid);
        }
        else {
            // if (isLoggable) {
            //     Logger::V(TAG, "    Dropping sync operation: No sync adapter registered"
            //             + "for: " + target);
            // }
            return FALSE;
        }
    }
    else if (target->mTarget_service) {
        state = mHost->mSyncStorageEngine->GetIsTargetServiceActive(target->mService, target->mUserId)
                    ? 1 : 0;
        if (state == 0) {
            // TODO: Change this to not drop disabled syncs - keep them in the pending queue.
            // if (isLoggable) {
            //     Logger::V(TAG, "    Dropping sync operation: isActive == 0.");
            // }
            return FALSE;
        }

        AutoPtr<IPackageManager> pkgMgr;
        mHost->mContext->GetPackageManager((IPackageManager**)&pkgMgr);
        AutoPtr<IServiceInfo> si;
        AutoPtr<IApplicationInfo> ai;
        ECode ec = pkgMgr->GetServiceInfo(target->mService, 0, (IServiceInfo**)&si);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) { return FALSE; }
        ai->GetUid(&targetUid);
    }
    else {
        Logger::E(TAG, "Unknown target for Sync Op: %s", TO_CSTR(target));
        return FALSE;
    }

    // We ignore system settings that specify the sync is invalid if:
    // 1) It's manual - we try it anyway. When/if it fails it will be rescheduled.
    //      or
    // 2) it's an initialisation sync - we just need to connect to it.
    Boolean ignoreSystemConfiguration, bval;
    op->mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &ignoreSystemConfiguration);
    ignoreSystemConfiguration |= (state < 0);

    // Sync not enabled.
    if (!syncEnabled && !ignoreSystemConfiguration) {
        // if (isLoggable) {
        //     Logger::V(TAG, "    Dropping sync operation: disallowed by settings/network.");
        // }
        return FALSE;
    }
    // Network down.
    AutoPtr<IConnectivityManager> connMgr = mHost->GetConnectivityManager();
    AutoPtr<INetworkInfo> networkInfo;
    connMgr->GetActiveNetworkInfoForUid(targetUid, (INetworkInfo**)&networkInfo);
    Boolean uidNetworkConnected = FALSE;
    if (networkInfo != NULL) {
        networkInfo->IsConnected(&uidNetworkConnected);
    }
    if (!uidNetworkConnected && !ignoreSystemConfiguration) {
        // if (isLoggable) {
        //     Logger::V(TAG, "    Dropping sync operation: disallowed by settings/network.");
        // }
        return FALSE;
    }

    // Metered network.
    connMgr->IsActiveNetworkMetered(&bval);
    if (op->IsNotAllowedOnMetered() && bval && !ignoreSystemConfiguration) {
        // if (isLoggable) {
        //     Logger::V(TAG, "    Dropping sync operation: not allowed on metered network.");
        // }
        return FALSE;
    }
    return TRUE;
}

Boolean SyncManager::SyncHandler::DispatchSyncOperation(
    /* [in] */ SyncOperation* op)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "DispatchSyncOperation: we are going to sync " + op);
    //     Logger::V(TAG, "num active syncs: " + mActiveSyncContexts.size());
    //     for (ActiveSyncContext syncContext : mActiveSyncContexts) {
    //         Logger::V(TAG, syncContext.toString());
    //     }
    // }
    // Connect to the sync adapter.
    Int32 targetUid;
    AutoPtr<IComponentName> targetComponent;
    EndPoint* info = op->mTarget;
    if (info->mTarget_provider) {
        AutoPtr<ISyncAdapterTypeHelper> helper;
        CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&helper);
        String type;
        info->mAccount->GetType(&type);
        AutoPtr<ISyncAdapterType> syncAdapterType;
        helper->NewKey(info->mProvider, type, (ISyncAdapterType**)&syncAdapterType);
        AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
        IRegisteredServicesCache::Probe(mHost->mSyncAdapters)->GetServiceInfo(syncAdapterType, info->mUserId,
            (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);
        if (syncAdapterInfo == NULL) {
            // Logger::D(TAG, "can't find a sync adapter for " + syncAdapterType
            //         + ", removing settings for it");
            mHost->mSyncStorageEngine->RemoveAuthority(info);
            return FALSE;
        }
        syncAdapterInfo->GetUid(&targetUid);
        syncAdapterInfo->GetComponentName((IComponentName**)&targetComponent);
    }
    else {
        // TODO: Store the uid of the service as part of the authority info in order to
        // avoid this call?
        AutoPtr<IPackageManager> pkgMgr;
        mHost->mContext->GetPackageManager((IPackageManager**)&pkgMgr);
        AutoPtr<IServiceInfo> si;
        AutoPtr<IApplicationInfo> ai;
        ECode ec = pkgMgr->GetServiceInfo(info->mService, 0, (IServiceInfo**)&si);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            Logger::D(TAG, "Can't find a service for %s, removing settings for it",
                TO_CSTR(info->mService));
            return FALSE;
        }
        ai->GetUid(&targetUid);
        targetComponent = info->mService;
    }

    AutoPtr<IActiveSyncContext> activeSyncContextObj;
    CActiveSyncContext::New((ISyncManager*)mHost, op, InsertStartSyncEvent(op), targetUid,
        (IActiveSyncContext**)&activeSyncContextObj);
    AutoPtr<ActiveSyncContext> activeSyncContext = (ActiveSyncContext*)activeSyncContextObj.Get();
    activeSyncContext->mSyncInfo = mHost->mSyncStorageEngine->AddActiveSync(activeSyncContext);
    mHost->mActiveSyncContexts.PushBack(activeSyncContext);
    if (!activeSyncContext->mSyncOperation->IsInitialization() &&
        !activeSyncContext->mSyncOperation->IsExpedited() &&
        !activeSyncContext->mSyncOperation->IsManual() &&
        !activeSyncContext->mSyncOperation->IsIgnoreSettings()) {
        // Post message to expire this sync if it runs for too Int64.
        mHost->PostSyncExpiryMessage(activeSyncContext);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "DispatchSyncOperation: starting " + activeSyncContext);
    // }
    if (!activeSyncContext->BindToSyncAdapter(targetComponent, info->mUserId)) {
        Logger::E(TAG, "Bind attempt failed - target: %s", TO_CSTR(targetComponent));
        CloseActiveSyncContext(activeSyncContext);
        return FALSE;
    }

    return TRUE;
}

void SyncManager::SyncHandler::RunBoundToAdapter(
    /* [in] */ ActiveSyncContext* activeSyncContext,
    /* [in] */ IBinder* syncAdapter)
{
    AutoPtr<SyncOperation> syncOperation = activeSyncContext->mSyncOperation;
    // try {
    activeSyncContext->mIsLinkedToDeath = TRUE;

    AutoPtr<IProxy> proxy = (IProxy*)syncAdapter->Probe(EIID_IProxy);
    IProxyDeathRecipient* pdr = (IProxyDeathRecipient*)activeSyncContext->Probe(EIID_IProxyDeathRecipient);
    if (proxy != NULL) proxy->LinkToDeath(pdr, 0);

    ECode ec = NOERROR;
    if (syncOperation->mTarget->mTarget_provider) {
        activeSyncContext->mSyncAdapter = IISyncAdapter::Probe(syncAdapter);
        ec = activeSyncContext->mSyncAdapter->StartSync(activeSyncContext,
            syncOperation->mTarget->mProvider,
            syncOperation->mTarget->mAccount, syncOperation->mExtras);
    }
    else if (syncOperation->mTarget->mTarget_service) {
        activeSyncContext->mSyncServiceAdapter = IISyncServiceAdapter::Probe(syncAdapter);
        ec = activeSyncContext->mSyncServiceAdapter->StartSync(activeSyncContext, syncOperation->mExtras);
    }

    // } catch (RemoteException remoteExc) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::D(TAG, "maybeStartNextSync: caught a RemoteException, rescheduling");
        CloseActiveSyncContext(activeSyncContext);
        mHost->IncreaseBackoffSetting(syncOperation);
        AutoPtr<SyncOperation> so = new SyncOperation(syncOperation, 0L /* newRunTimeFromNow */);
        mHost->ScheduleSyncOperation(so);
    }
    // } catch (RuntimeException exc) {
    else if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        CloseActiveSyncContext(activeSyncContext);
        Logger::E(TAG, "Caught RuntimeException while starting the sync %s",
            TO_CSTR(syncOperation));
    }
    // }
}

void SyncManager::SyncHandler::CancelActiveSyncLocked(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    List<AutoPtr<ActiveSyncContext> > activeSyncs(mHost->mActiveSyncContexts);
    List<AutoPtr<ActiveSyncContext> >::Iterator it;
    for (it = activeSyncs.Begin(); it != activeSyncs.End(); ++it) {
        ActiveSyncContext* activeSyncContext = *it;
        if (activeSyncContext != NULL) {
            EndPoint* opInfo = activeSyncContext->mSyncOperation->mTarget;
            if (!opInfo->MatchesSpec(info)) {
                continue;
            }
            if (extras != NULL &&
                !SyncExtrasEquals(activeSyncContext->mSyncOperation->mExtras,
                    extras, FALSE /* no config settings */)) {
                continue;
            }
            RunSyncFinishedOrCanceledLocked(
                NULL /* no result since this is a cancel */,
                activeSyncContext);
        }
    }
}

void SyncManager::SyncHandler::RunSyncFinishedOrCanceledLocked(
    /* [in] */ ISyncResult* syncResult,
    /* [in] */ ActiveSyncContext* activeSyncContext)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);

    AutoPtr<SyncOperation> syncOperation = activeSyncContext->mSyncOperation;
    EndPoint* info = syncOperation->mTarget;

    if (activeSyncContext->mIsLinkedToDeath) {
        IProxyDeathRecipient* pdr = (IProxyDeathRecipient*)activeSyncContext->Probe(EIID_IProxyDeathRecipient);
        Boolean result;
        if (info->mTarget_provider) {
            AutoPtr<IProxy> proxy = (IProxy*)activeSyncContext->mSyncAdapter->Probe(EIID_IProxy);
            assert(proxy != NULL);
            proxy->UnlinkToDeath(pdr, 0, &result);
        }
        else {
            AutoPtr<IProxy> proxy = (IProxy*)activeSyncContext->mSyncServiceAdapter->Probe(EIID_IProxy);
            assert(proxy != NULL);
            proxy->UnlinkToDeath(pdr, 0, &result);
        }
        activeSyncContext->mIsLinkedToDeath = FALSE;
    }

    CloseActiveSyncContext(activeSyncContext);
    Int64 elapsedTime = SystemClock::GetElapsedRealtime() - activeSyncContext->mStartTime;
    String historyMessage;
    Int32 downstreamActivity;
    Int32 upstreamActivity;
    Boolean bval;
    if (syncResult != NULL) {
        // if (isLoggable) {
        //     Logger::V(TAG, "runSyncFinishedOrCanceled [finished]: "
        //             + syncOperation + ", result " + syncResult);
        // }
        syncResult->HasError(&bval);
        if (!bval) {
            historyMessage = SyncStorageEngine::MESG_SUCCESS;
            // TODO: set these correctly when the SyncResult is extended to include it
            downstreamActivity = 0;
            upstreamActivity = 0;
            mHost->ClearBackoffSetting(syncOperation);
        }
        else {
            Logger::D(TAG, "failed sync operation %s, %s",
                TO_CSTR(syncOperation),
                TO_CSTR(syncResult));

            // the operation failed so increase the backoff time
            mHost->IncreaseBackoffSetting(syncOperation);
            // reschedule the sync if so indicated by the syncResult
            mHost->MaybeRescheduleSync(syncResult, syncOperation);
            AutoPtr<IContentResolverHelper> helper;
            CContentResolverHelper::AcquireSingleton((IContentResolverHelper**)&helper);
            helper->SyncErrorToString(SyncResultToErrorNumber(syncResult), &historyMessage);
            // TODO: set these correctly when the SyncResult is extended to include it
            downstreamActivity = 0;
            upstreamActivity = 0;
        }

        Int64 until;
        syncResult->GetDelayUntil(&until);
        mHost->SetDelayUntilTime(syncOperation, until);
    }
    else {
        // if (isLoggable) {
        //     Logger::V(TAG, "runSyncFinishedOrCanceled [canceled]: " + syncOperation);
        // }
        if (activeSyncContext->mSyncAdapter != NULL) {
            // try {
                activeSyncContext->mSyncAdapter->CancelSync(activeSyncContext);
            // } catch (RemoteException e) {
            //     // we don't need to retry this in this case
            // }
        }
        else if (activeSyncContext->mSyncServiceAdapter != NULL) {
            // try {
                activeSyncContext->mSyncServiceAdapter->CancelSync(activeSyncContext);
            // } catch (RemoteException e) {
            //     // we don't need to retry this in this case
            // }
        }
        historyMessage = SyncStorageEngine::MESG_CANCELED;
        downstreamActivity = 0;
        upstreamActivity = 0;
    }

    StopSyncEvent(activeSyncContext->mHistoryRowId, syncOperation, historyMessage,
            upstreamActivity, downstreamActivity, elapsedTime);

    // Check for full-resync and schedule it after closing off the last sync.
    if (info->mTarget_provider) {
        bval = FALSE;
        if (syncResult != NULL) {
            syncResult->GetTooManyDeletions(&bval);
        }
        if (bval) {
            AutoPtr<ISyncStats> stats;
            syncResult->GetStats((ISyncStats**)&stats);
            Int64 deletes;
            stats->GetNumDeletes(&deletes);
            InstallHandleTooManyDeletesNotification(
                info->mAccount, info->mProvider, deletes, info->mUserId);
        }
        else {
            AutoPtr<IUserHandle> handle;
            CUserHandle::New(info->mUserId, (IUserHandle**)&handle);
            mHost->mNotificationMgr->CancelAsUser(String(NULL),
                Object::GetHashCode(info->mAccount) ^ info->mProvider.GetHashCode(),
                handle);
        }

        bval = FALSE;
        if (syncResult) {
            syncResult->GetFullSyncRequested(&bval);
        }
        if (bval) {
            AutoPtr<IBundle> bundle;
            CBundle::New((IBundle**)&bundle);
            AutoPtr<SyncOperation> op =  new SyncOperation(
                info->mAccount, info->mUserId,
                syncOperation->mReason,
                syncOperation->mSyncSource, info->mProvider, bundle,
                0 /* delay */, 0 /* flex */,
                syncOperation->mBackoff, syncOperation->mDelayUntil,
                syncOperation->mAllowParallelSyncs);
            mHost->ScheduleSyncOperation(op);
        }
    }
    else {
        bval = FALSE;
        if (syncResult) {
            syncResult->GetFullSyncRequested(&bval);
        }
        if (bval) {
            AutoPtr<IBundle> bundle;
            CBundle::New((IBundle**)&bundle);
            AutoPtr<SyncOperation> op = new SyncOperation(
                info->mService, info->mUserId, syncOperation->mReason,
                syncOperation->mSyncSource, bundle,
                0 /* delay */, 0 /* flex */,
                syncOperation->mBackoff, syncOperation->mDelayUntil);
            mHost->ScheduleSyncOperation(op);
        }
    }
    // no need to schedule an alarm, as that will be done by our caller.
}

void SyncManager::SyncHandler::CloseActiveSyncContext(
    /* [in] */ ActiveSyncContext* activeSyncContext)
{
    activeSyncContext->Close();
    AutoPtr<ActiveSyncContext> asc = activeSyncContext;
    List<AutoPtr<ActiveSyncContext> >::Iterator it;
    it = Find(mHost->mActiveSyncContexts.Begin(), mHost->mActiveSyncContexts.End(), asc);
    if (it != mHost->mActiveSyncContexts.End()) {
        mHost->mActiveSyncContexts.Erase(it);
    }

    mHost->mSyncStorageEngine->RemoveActiveSync(
        activeSyncContext->mSyncInfo,
        activeSyncContext->mSyncOperation->mTarget->mUserId);
    mHost->RemoveSyncExpiryMessage(activeSyncContext);
}

Int32 SyncManager::SyncHandler::SyncResultToErrorNumber(
    /* [in] */ ISyncResult* syncResult)
{
    Boolean bval;
    syncResult->GetSyncAlreadyInProgress(&bval);
    if (bval)
        return IContentResolver::SYNC_ERROR_SYNC_ALREADY_IN_PROGRESS;

    AutoPtr<ISyncStats> stats;
    syncResult->GetStats((ISyncStats**)&stats);
    Int64 ival;
    stats->GetNumAuthExceptions(&ival);
    if (ival > 0)
        return IContentResolver::SYNC_ERROR_AUTHENTICATION;

    stats->GetNumIoExceptions(&ival);
    if (ival > 0)
        return IContentResolver::SYNC_ERROR_IO;

    stats->GetNumParseExceptions(&ival);
    if (ival > 0)
        return IContentResolver::SYNC_ERROR_PARSE;

    stats->GetNumConflictDetectedExceptions(&ival);
    if (ival > 0)
        return IContentResolver::SYNC_ERROR_CONFLICT;

    syncResult->GetTooManyDeletions(&bval);
    if (bval)
        return IContentResolver::SYNC_ERROR_TOO_MANY_DELETIONS;

    syncResult->GetTooManyRetries(&bval);
    if (bval)
        return IContentResolver::SYNC_ERROR_TOO_MANY_RETRIES;

    syncResult->GetDatabaseError(&bval);
    if (bval)
        return IContentResolver::SYNC_ERROR_INTERNAL;

    Logger::E(TAG, "we are not in an error state, %s",
        TO_CSTR(syncResult));
    return 0;
}

void SyncManager::SyncHandler::ManageSyncNotificationLocked()
{
    Boolean shouldCancel;
    Boolean shouldInstall;

    if (mHost->mActiveSyncContexts.IsEmpty()) {
        mSyncNotificationInfo->mStartTime = -1;

        // we aren't syncing. if the notification is active then remember that we need
        // to cancel it and then clear out the info
        shouldCancel = mSyncNotificationInfo->mIsActive;
        shouldInstall = FALSE;
    }
    else {
        // we are syncing
        Int64 now = SystemClock::GetElapsedRealtime();
        if (mSyncNotificationInfo->mStartTime == -1) {
            mSyncNotificationInfo->mStartTime = now;
        }

        // there are three cases:
        // - the notification is up: do nothing
        // - the notification is not up but it isn't time yet: don't install
        // - the notification is not up and it is time: need to install

        if (mSyncNotificationInfo->mIsActive) {
            shouldInstall = shouldCancel = FALSE;
        }
        else {
            // it isn't currently up, so there is nothing to cancel
            shouldCancel = FALSE;

            Boolean timeToShowNotification =
                    now > mSyncNotificationInfo->mStartTime + SYNC_NOTIFICATION_DELAY;
            if (timeToShowNotification) {
                shouldInstall = TRUE;
            }
            else {
                // show the notification immediately if this is a manual sync
                shouldInstall = FALSE;
                Boolean manualSync;
                List<AutoPtr<ActiveSyncContext> >::Iterator it;
                for (it = mHost->mActiveSyncContexts.Begin(); it != mHost->mActiveSyncContexts.End(); ++it) {
                    (*it)->mSyncOperation->mExtras->GetBoolean(
                        IContentResolver::SYNC_EXTRAS_MANUAL, FALSE, &manualSync);
                    if (manualSync) {
                        shouldInstall = TRUE;
                        break;
                    }
                }
            }
        }
    }

    if (shouldCancel && !shouldInstall) {
        mHost->mNeedSyncActiveNotification = FALSE;
        SendSyncStateIntent();
        mSyncNotificationInfo->mIsActive = FALSE;
    }

    if (shouldInstall) {
        mHost->mNeedSyncActiveNotification = TRUE;
        SendSyncStateIntent();
        mSyncNotificationInfo->mIsActive = TRUE;
    }
}

void SyncManager::SyncHandler::ManageSyncAlarmLocked(
    /* [in] */ Int64 nextPeriodicEventElapsedTime,
    /* [in] */ Int64 nextPendingEventElapsedTime)
{
    // in each of these cases the sync loop will be kicked, which will cause this
    // method to be called again
    if (!mHost->mDataConnectionIsConnected) return;
    if (mHost->mStorageIsLow) return;

    using Elastos::Core::Math;
    // When the status bar notification should be raised
    Int64 notificationTime =
        (!mHost->mSyncHandler->mSyncNotificationInfo->mIsActive
                && mHost->mSyncHandler->mSyncNotificationInfo->mStartTime != -1)
                ? mHost->mSyncHandler->mSyncNotificationInfo->mStartTime + SYNC_NOTIFICATION_DELAY
                : Math::INT64_MAX_VALUE;

    // When we should consider canceling an active sync
    Int64 earliestTimeoutTime = Math::INT64_MAX_VALUE;
    List<AutoPtr<ActiveSyncContext> >::Iterator it;
    for (it = mHost->mActiveSyncContexts.Begin(); it != mHost->mActiveSyncContexts.End(); ++it) {
        Int64 currentSyncTimeoutTime = (*it)->mTimeoutStartTime + MAX_TIME_PER_SYNC;
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "manageSyncAlarm: active sync, mTimeoutStartTime + MAX is "
        //             + currentSyncTimeoutTime);
        // }
        if (earliestTimeoutTime > currentSyncTimeoutTime) {
            earliestTimeoutTime = currentSyncTimeoutTime;
        }
    }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "manageSyncAlarm: notificationTime is " + notificationTime);
    // }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "manageSyncAlarm: earliestTimeoutTime is " + earliestTimeoutTime);
    // }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "manageSyncAlarm: nextPeriodicEventElapsedTime is "
    //             + nextPeriodicEventElapsedTime);
    // }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "manageSyncAlarm: nextPendingEventElapsedTime is "
    //             + nextPendingEventElapsedTime);
    // }

    Int64 alarmTime = Math::Min(notificationTime, earliestTimeoutTime);
    alarmTime = Math::Min(alarmTime, nextPeriodicEventElapsedTime);
    alarmTime = Math::Min(alarmTime, nextPendingEventElapsedTime);

    // Bound the alarm time.
    Int64 now = SystemClock::GetElapsedRealtime();
    if (alarmTime < now + SYNC_ALARM_TIMEOUT_MIN) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "manageSyncAlarm: the alarmTime is too small, "
        //             + alarmTime + ", setting to " + (now + SYNC_ALARM_TIMEOUT_MIN));
        // }
        alarmTime = now + SYNC_ALARM_TIMEOUT_MIN;
    }
    else if (alarmTime > now + SYNC_ALARM_TIMEOUT_MAX) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "manageSyncAlarm: the alarmTime is too large, "
        //             + alarmTime + ", setting to " + (now + SYNC_ALARM_TIMEOUT_MIN));
        // }
        alarmTime = now + SYNC_ALARM_TIMEOUT_MAX;
    }

    // determine if we need to set or cancel the alarm
    Boolean shouldSet = FALSE;
    Boolean shouldCancel = FALSE;
    Boolean alarmIsActive = (mAlarmScheduleTime != -1) && (now < mAlarmScheduleTime);
    Boolean needAlarm = alarmTime != Math::INT64_MAX_VALUE;
    if (needAlarm) {
        // Need the alarm if
        //  - it's currently not set
        //  - if the alarm is set in the past.
        if (!alarmIsActive || alarmTime < mAlarmScheduleTime) {
            shouldSet = TRUE;
        }
    }
    else {
        shouldCancel = alarmIsActive;
    }

    // Set or cancel the alarm as directed.
    mHost->EnsureAlarmService();
    if (shouldSet) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "requesting that the alarm manager wake us up at elapsed time "
        //             + alarmTime + ", now is " + now + ", " + ((alarmTime - now) / 1000)
        //             + " secs from now");
        // }
        mAlarmScheduleTime = alarmTime;
        mHost->mAlarmService->SetExact(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
            alarmTime, mHost->mSyncAlarmIntent);
    }
    else if (shouldCancel) {
        mAlarmScheduleTime = -1;
        mHost->mAlarmService->Cancel(mHost->mSyncAlarmIntent);
    }
}

void SyncManager::SyncHandler::SendSyncStateIntent()
{
    AutoPtr<IIntent> syncStateIntent;
    CIntent::New(IIntent::ACTION_SYNC_STATE_CHANGED, (IIntent**)&syncStateIntent);
    syncStateIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    syncStateIntent->PutExtra(String("active"), mHost->mNeedSyncActiveNotification);
    syncStateIntent->PutExtra(String("failing"), FALSE);
    mHost->mContext->SendBroadcastAsUser(syncStateIntent, UserHandle::OWNER);
}

void SyncManager::SyncHandler::InstallHandleTooManyDeletesNotification(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int64 numDeletes,
    /* [in] */ Int32 userId)
{
    if (mHost->mNotificationMgr == NULL) return;

    AutoPtr<IProviderInfo> providerInfo;
    AutoPtr<IPackageManager> pkgMgr;
    mHost->mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    pkgMgr->ResolveContentProvider(authority, 0 /* flags */, (IProviderInfo**)&providerInfo);
    if (providerInfo == NULL) {
        return;
    }
    AutoPtr<ICharSequence> authorityName;
    IPackageItemInfo::Probe(providerInfo)->LoadLabel(pkgMgr, (ICharSequence**)&authorityName);

    AutoPtr<IIntent> clickIntent;
    assert(0 && "TODO0");
    // CIntent::New(mHost->mContext, ECLSID_CSyncActivityTooManyDeletes, (IIntent**)&clickIntent);
    clickIntent->PutExtra(String("account"), IParcelable::Probe(account));
    clickIntent->PutExtra(String("authority"), authority);
    clickIntent->PutExtra(String("provider"), Object::ToString(authorityName));
    clickIntent->PutExtra(String("numDeletes"), numDeletes);

    if (!IsActivityAvailable(clickIntent)) {
        Logger::W(TAG, "No activity found to handle too many deletes.");
        return;
    }

    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetActivityAsUser(mHost->mContext, 0, clickIntent,
        IPendingIntent::FLAG_CANCEL_CURRENT, NULL, user, (IPendingIntent**)&pendingIntent);

    AutoPtr<IResources> resources;
    mHost->mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> tooManyDeletesDescFormat;
    resources->GetText(R::string::contentServiceTooManyDeletesNotificationDesc,
        (ICharSequence**)&tooManyDeletesDescFormat);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 ms;
    system->GetCurrentTimeMillis(&ms);
    AutoPtr<IContext> contextForUser = mHost->GetContextForUser(user);
    String str;
    mHost->mContext->GetString(R::string::contentServiceSync, &str);
    AutoPtr<INotification> notification;
    CNotification::New(R::drawable::stat_notify_sync_error,
        CoreUtils::Convert(str), ms, (INotification**)&notification);

    AutoPtr<IResources> cfuRes;
    contextForUser->GetResources((IResources**)&cfuRes);
    Int32 color;
    cfuRes->GetColor(R::color::system_notification_accent_color, &color);
    contextForUser->GetString(R::string::contentServiceSyncNotificationTitle, &str);
    String text;
    text.AppendFormat(
        TO_CSTR(tooManyDeletesDescFormat),
        TO_CSTR(authorityName));
    notification->SetColor(color);
    notification->SetLatestEventInfo(contextForUser,
        CoreUtils::Convert(str), CoreUtils::Convert(text), pendingIntent);
    Int32 flags;
    notification->GetFlags(&flags);
    notification->SetFlags(flags | INotification::FLAG_ONGOING_EVENT);
    mHost->mNotificationMgr->NotifyAsUser(String(NULL),
        Object::GetHashCode(account) ^ authority.GetHashCode(),
        notification, user);
}

Boolean SyncManager::SyncHandler::IsActivityAvailable(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IPackageManager> pm;
    mHost->mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> list;
    pm->QueryIntentActivities(intent, 0, (IList**)&list);
    Int32 listSize, flags;
    list->GetSize(&listSize);
    for (Int32 i = 0; i < listSize; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> ai;
        resolveInfo->GetActivityInfo((IActivityInfo**)&ai);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        appInfo->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
            return TRUE;
        }
    }

    return FALSE;
}

Int64 SyncManager::SyncHandler::InsertStartSyncEvent(
    /* [in] */ SyncOperation* syncOperation)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    Int64 now;
    system->GetCurrentTimeMillis(&now);
    // EventLogger::WriteEvent(2720,
    //         syncOperation.toEventLog(SyncStorageEngine::EVENT_START));
    mHost->mSyncStorageEngine->InsertStartSyncEvent(syncOperation, now);
    return NOERROR;
}

void SyncManager::SyncHandler::StopSyncEvent(
    /* [in] */ Int64 rowId,
    /* [in] */ SyncOperation* syncOperation,
    /* [in] */ const String& resultMessage,
    /* [in] */ Int32 upstreamActivity,
    /* [in] */ Int32 downstreamActivity,
    /* [in] */ Int64 elapsedTime)
{
    // EventLogger::WriteEvent(2720,
    //         syncOperation.toEventLog(SyncStorageEngine::EVENT_STOP));
    mHost->mSyncStorageEngine->StopSyncEvent(rowId, elapsedTime,
        resultMessage, downstreamActivity, upstreamActivity);
}

//==========================================================================================
// SyncManager::StorageIntentReceiver
//==========================================================================================

SyncManager::StorageIntentReceiver::StorageIntentReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::StorageIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_DEVICE_STORAGE_LOW.Equals(action)) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "Internal storage is low.");
        // }
        mHost->mStorageIsLow = TRUE;
        mHost->CancelActiveSync(
            EndPoint::USER_ALL_PROVIDER_ALL_ACCOUNTS_ALL,
            NULL /* any sync */);
    }
    else if (IIntent::ACTION_DEVICE_STORAGE_OK.Equals(action)) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "Internal storage is ok.");
        // }
        mHost->mStorageIsLow = FALSE;
        mHost->SendCheckAlarmsMessage();
    }
    return NOERROR;
}

//==========================================================================================
// SyncManager::BootCompletedReceiver
//==========================================================================================

SyncManager::BootCompletedReceiver::BootCompletedReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mSyncHandler->OnBootCompleted();
    return NOERROR;
}

//==========================================================================================
// SyncManager::AccountsUpdatedReceiver
//==========================================================================================

SyncManager::AccountsUpdatedReceiver::AccountsUpdatedReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::AccountsUpdatedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->UpdateRunningAccounts();

    // Kick off sync for everyone, since this was a radical account change
    mHost->ScheduleSync(NULL, IUserHandle::USER_ALL,
        SyncOperation::REASON_ACCOUNTS_UPDATED, String(NULL),
        NULL, 0 /* no delay */, 0/* no delay */, FALSE);
    return NOERROR;
}

//==========================================================================================
// SyncManager::ConnectivityIntentReceiver
//==========================================================================================

SyncManager::ConnectivityIntentReceiver::ConnectivityIntentReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::ConnectivityIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean wasConnected = mHost->mDataConnectionIsConnected;

    // don't use the intent to figure out if network is connected, just check
    // ConnectivityManager directly.
    mHost->mDataConnectionIsConnected = mHost->ReadDataConnectionState();
    if (mHost->mDataConnectionIsConnected) {
        if (!wasConnected) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "Reconnection detected: clearing all backoffs");
            // }
            Object* obj = mHost->mSyncQueue.Get();
            synchronized(obj) {
                mHost->mSyncStorageEngine->ClearAllBackoffsLocked(mHost->mSyncQueue);
            }
        }
        mHost->SendCheckAlarmsMessage();
    }
    return NOERROR;
}

//==========================================================================================
// SyncManager::ShutdownIntentReceiver
//==========================================================================================
SyncManager::ShutdownIntentReceiver::ShutdownIntentReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::ShutdownIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logger::W(SyncManager::TAG, "Writing sync state before shutdown...");
    mHost->GetSyncStorageEngine()->WriteAllState();
    return NOERROR;
}

//==========================================================================================
// SyncManager::UserIntentReceiver
//==========================================================================================

SyncManager::UserIntentReceiver::UserIntentReceiver(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

//@Override
ECode SyncManager::UserIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
    if (userId == IUserHandle::USER_NULL) return NOERROR;

    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        mHost->OnUserRemoved(userId);
    }
    else if (IIntent::ACTION_USER_STARTING.Equals(action)) {
        mHost->OnUserStarting(userId);
    }
    else if (IIntent::ACTION_USER_STOPPING.Equals(action)) {
        mHost->OnUserStopping(userId);
    }
    return NOERROR;
}

//==========================================================================================
// SyncManager::AuthoritySyncStats
//==========================================================================================

SyncManager::AuthoritySyncStats::AuthoritySyncStats(
    /* [in] */ const String& name)
    : mName(name)
    , mElapsedTime(0)
    , mTimes(0)
{
}

//==========================================================================================
// SyncManager::AccountSyncStats
//==========================================================================================

SyncManager::AccountSyncStats::AccountSyncStats(
    /* [in] */ const String& name)
    : mName(name)
    , mElapsedTime(0)
    , mTimes(0)
{
}

//==========================================================================================
// SyncManager::SyncTimeTracker
//==========================================================================================

SyncManager::SyncTimeTracker::SyncTimeTracker(
    /* [in] */ SyncManager* host)
    : mLastWasSyncing(FALSE)
    , mWhenSyncStarted(0)
    , mTimeSpentSyncing(0)
    , mHost(host)
{
}

/** Call to let the tracker know that the sync state may have changed */
void SyncManager::SyncTimeTracker::Update()
{
    synchronized(this) {
        Boolean isSyncInProgress = !mHost->mActiveSyncContexts.IsEmpty();
        if (isSyncInProgress == mLastWasSyncing) return;
        Int64 now = SystemClock::GetElapsedRealtime();
        if (isSyncInProgress) {
            mWhenSyncStarted = now;
        } else {
            mTimeSpentSyncing += now - mWhenSyncStarted;
        }
        mLastWasSyncing = isSyncInProgress;
    }
}

/** Get how Int64 we have been syncing, in ms */
 Int64 SyncManager::SyncTimeTracker::TimeSpentSyncing()
 {
    Int64 result = 0;
    synchronized(this) {
        if (!mLastWasSyncing) return mTimeSpentSyncing;

        Int64 now = SystemClock::GetElapsedRealtime();
        result = mTimeSpentSyncing + (now - mWhenSyncStarted);
    }
    return result;
}

//==========================================================================================
// SyncManager::MyOnSyncRequestListener
//==========================================================================================
CAR_INTERFACE_IMPL(SyncManager::MyOnSyncRequestListener, Object, IOnSyncRequestListener)

SyncManager::MyOnSyncRequestListener::MyOnSyncRequestListener(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

ECode SyncManager::MyOnSyncRequestListener::OnSyncRequest(
    /* [in] */ ISyncStorageEngineEndPoint* ep,
    /* [in] */ Int32 reason,
    /* [in] */ IBundle* extras)
{
    EndPoint* info = (EndPoint*)ep;
    if (info->mTarget_provider) {
        mHost->ScheduleSync(info->mAccount, info->mUserId, reason, info->mProvider, extras,
            0 /* no flex */,
            0 /* run immediately */,
            FALSE);
    }
    else if (info->mTarget_service) {
        mHost->ScheduleSync(info->mService, info->mUserId, reason, extras,
                0 /* no flex */,
                0 /* run immediately */);
    }
    return NOERROR;
}

//==========================================================================================
// SyncManager::MyRegisteredServicesCacheListener
//==========================================================================================
CAR_INTERFACE_IMPL(SyncManager::MyRegisteredServicesCacheListener, Object, IRegisteredServicesCacheListener)

SyncManager::MyRegisteredServicesCacheListener::MyRegisteredServicesCacheListener(
    /* [in] */ SyncManager* host)
    : mHost(host)
{
}

ECode SyncManager::MyRegisteredServicesCacheListener::OnServiceChanged(
    /* [in] */ IInterface* type,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean removed)
{
    if (!removed) {
        ISyncAdapterType* sat = ISyncAdapterType::Probe(type);
        String authority;
        sat->GetAuthority(&authority);
        mHost->ScheduleSync(NULL, IUserHandle::USER_ALL,
            SyncOperation::REASON_SERVICE_CHANGED,
            authority, NULL, 0 /* no delay */, 0 /* no delay */,
            FALSE /* onlyThoseWithUnkownSyncableState */);
    }
    return NOERROR;
}

//==========================================================================================
// SyncManager::SyncStatusObserver
//==========================================================================================
CAR_INTERFACE_IMPL_2(SyncManager::SyncStatusObserver, Object, IISyncStatusObserver, IBinder)

ECode SyncManager::SyncStatusObserver::constructor(
    /* [in] */ ISyncManager* host)
{
    mHost = (SyncManager*)host;
    return NOERROR;
}

ECode SyncManager::SyncStatusObserver::OnStatusChanged(
    /* [in] */ Int32 which)
{
    // force the sync loop to run if the settings change
    mHost->SendCheckAlarmsMessage();
    return NOERROR;
}

//==========================================================================================
// SyncManager
//==========================================================================================

AutoPtr<IList> SyncManager::GetAllUsers()
{
    AutoPtr<IList> list;
    mUserManager->GetUsers((IList**)&list);
    return list;
}

Boolean SyncManager::ContainsAccountAndUser(
    /* [in] */ ArrayOf<IAccountAndUser*>* accounts,
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId)
{
    Boolean found = FALSE;
    for (Int32 i = 0; i < accounts->GetLength(); i++) {
        Int32 userId;
        (*accounts)[i]->GetUserId(&userId);
        AutoPtr<IAccount> acnt;
        (*accounts)[i]->GetAccount((IAccount**)&acnt);
        if (userId == userId && Object::Equals(acnt, account)) {
            found = TRUE;
            break;
        }
    }
    return found;
}

void SyncManager::UpdateRunningAccounts()
{
    assert(0 && "TODO");
    // CAccountManagerService::GetSingleton()->GetRunningAccounts(
    //     (ArrayOf<IAccountAndUser*>**)&mRunningAccounts);

    if (mBootCompleted) {
        DoDatabaseCleanup();
    }

    ArrayOf<IAccountAndUser*>* accounts = mRunningAccounts;
    List<AutoPtr<ActiveSyncContext> >::Iterator it;
    for (it = mActiveSyncContexts.Begin(); it != mActiveSyncContexts.End(); ++it) {
        ActiveSyncContext* currentSyncContext = *it;
        if (!ContainsAccountAndUser(accounts,
            currentSyncContext->mSyncOperation->mTarget->mAccount,
            currentSyncContext->mSyncOperation->mTarget->mUserId)) {
            Logger::D(TAG, "canceling sync since the account is no longer running");
            SendSyncFinishedOrCanceledMessage(currentSyncContext,
                NULL /* no result since this is a cancel */);
        }
    }
    // we must do this since we don't bother scheduling alarms when
    // the accounts are not set yet
    SendCheckAlarmsMessage();
}

void SyncManager::DoDatabaseCleanup()
{
    AutoPtr<IList> list;
    mUserManager->GetUsers(TRUE, (IList**)&list);
    AutoPtr<IIterator> it;
    Boolean hasNext, bval;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IUserInfo* user = IUserInfo::Probe(obj);

        // Skip any partially created/removed users
        user->GetPartial(&bval);
        if (bval) continue;
        Int32 id;
        user->GetId(&id);
        AutoPtr<ArrayOf<IAccount*> > accountsForUser;
        // CAccountManagerService::GetSingleton()->GetAccounts(id, (ArrayOf<IAccount*>**)&accountsForUser);
        mSyncStorageEngine->DoDatabaseCleanup(accountsForUser, id);
    }
}

Boolean SyncManager::ReadDataConnectionState()
{
    AutoPtr<IConnectivityManager> connMgr = GetConnectivityManager();
    AutoPtr<INetworkInfo> networkInfo;
    connMgr->GetActiveNetworkInfo((INetworkInfo**)&networkInfo);
    Boolean bval = FALSE;
    if (networkInfo != NULL) {
        networkInfo->IsConnected(&bval);
    }
    return bval;
}

AutoPtr<IConnectivityManager> SyncManager::GetConnectivityManager()
{
    synchronized(this) {
        if (mConnManagerDoNotUseDirectly == NULL) {
            AutoPtr<IInterface> obj;
            mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
            mConnManagerDoNotUseDirectly = IConnectivityManager::Probe(obj);
        }
    }
    return mConnManagerDoNotUseDirectly;
}

SyncManager::SyncManager(
    /* [in] */ IContext* context,
    /* [in] */ Boolean factoryTest)
    : mDataConnectionIsConnected(FALSE)
    , mStorageIsLow(FALSE)
    , mNeedSyncActiveNotification(FALSE)
    , mBootCompleted(FALSE)
    , mSyncRandomOffsetMillis(0)
{
    mRunningAccounts = INITIAL_ACCOUNTS_ARRAY;

    // Initialize the SyncStorageEngine first, before registering observers
    // and creating threads and so on; it may fail if the disk is full.
    mContext = context;

    SyncStorageEngine::Init(context);
    mSyncStorageEngine = SyncStorageEngine::GetSingleton();
    AutoPtr<IOnSyncRequestListener> listener = new MyOnSyncRequestListener(this);
    mSyncStorageEngine->SetOnSyncRequestListener(listener);

    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    CSyncAdaptersCache::New(mContext, (ISyncAdaptersCache**)&mSyncAdapters);
    mSyncQueue = new SyncQueue(pkgMgr, mSyncStorageEngine, mSyncAdapters);

    AutoPtr<IBackgroundThreadHelper> btHelper;
    CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&btHelper);
    AutoPtr<IBackgroundThread> bt;
    btHelper->GetInstance((IBackgroundThread**)&bt);
    AutoPtr<ILooper> looper;
    IHandlerThread::Probe(bt)->GetLooper((ILooper**)&looper);
    mSyncHandler = new SyncHandler(this);
    mSyncHandler->constructor(looper);

    AutoPtr<IRegisteredServicesCacheListener> rscListener = new MyRegisteredServicesCacheListener(this);
    IRegisteredServicesCache::Probe(mSyncAdapters)->SetListener(rscListener, mSyncHandler);

    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IIntent> alarmIntent;
    CIntent::New(ACTION_SYNC_ALARM, (IIntent**)&alarmIntent);
    piHelper->GetBroadcast(mContext, 0 /* ignored */, alarmIntent, 0, (IPendingIntent**)&mSyncAlarmIntent);

    AutoPtr<IIntent> stickyIntent;
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IConnectivityManager::CONNECTIVITY_ACTION, (IIntentFilter**)&intentFilter);
    context->RegisterReceiver(mConnectivityIntentReceiver, intentFilter, (IIntent**)&stickyIntent);

    if (!factoryTest) {
        intentFilter = NULL;
        stickyIntent = NULL;
        CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&intentFilter);
        intentFilter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
        context->RegisterReceiver(mBootCompletedReceiver, intentFilter, (IIntent**)&stickyIntent);
    }

    intentFilter = NULL;
    stickyIntent = NULL;
    CIntentFilter::New(IIntent::ACTION_DEVICE_STORAGE_LOW, (IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_DEVICE_STORAGE_OK);
    context->RegisterReceiver(mStorageIntentReceiver, intentFilter, (IIntent**)&stickyIntent);

    intentFilter = NULL;
    stickyIntent = NULL;
    CIntentFilter::New(IIntent::ACTION_SHUTDOWN, (IIntentFilter**)&intentFilter);
    intentFilter->SetPriority(100);
    context->RegisterReceiver(mShutdownIntentReceiver, intentFilter, (IIntent**)&stickyIntent);

    intentFilter = NULL;
    stickyIntent = NULL;
    String nullStr;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    intentFilter->AddAction(IIntent::ACTION_USER_STARTING);
    intentFilter->AddAction(IIntent::ACTION_USER_STOPPING);
    mContext->RegisterReceiverAsUser(
        mUserIntentReceiver, UserHandle::ALL, intentFilter, nullStr, NULL, (IIntent**)&stickyIntent);

    if (!factoryTest) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        mNotificationMgr = INotificationManager::Probe(obj);
        intentFilter = NULL;
        stickyIntent = NULL;
        CIntentFilter::New(ACTION_SYNC_ALARM, (IIntentFilter**)&intentFilter);
        AutoPtr<IBroadcastReceiver> br = new SyncAlarmIntentReceiver(this);
        context->RegisterReceiver(br, intentFilter, (IIntent**)&stickyIntent);
    }
    else {
        mNotificationMgr = NULL;
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    obj = NULL;
    AutoPtr<IServiceManager> srvMgr;
    CServiceManager::AcquireSingleton((IServiceManager**)&srvMgr);
    srvMgr->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&obj);
    mBatteryStats = IIBatteryStats::Probe(obj);

    // This WakeLock is used to ensure that we stay awake between the time that we receive
    // a sync alarm notification and when we finish processing it. We need to do this
    // because we don't do the work in the alarm handler, rather we do it in a message
    // handler.
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
        HANDLE_SYNC_ALARM_WAKE_LOCK, (IPowerManagerWakeLock**)&mHandleAlarmWakeLock);
    mHandleAlarmWakeLock->SetReferenceCounted(FALSE);

    // This WakeLock is used to ensure that we stay awake while running the sync loop
    // message handler. Normally we will hold a sync adapter wake lock while it is being
    // synced but during the execution of the sync loop it might finish a sync for
    // one sync adapter before starting the sync for the other sync adapter and we
    // don't want the device to go to sleep during that window.
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
        SYNC_LOOP_WAKE_LOCK, (IPowerManagerWakeLock**)&mSyncManagerWakeLock);
    mSyncManagerWakeLock->SetReferenceCounted(FALSE);

    AutoPtr<IISyncStatusObserver> oberservr;
    CSyncStatusObserver::New(this, (IISyncStatusObserver**)&oberservr);
    mSyncStorageEngine->AddStatusChangeListener(
        IContentResolver::SYNC_OBSERVER_TYPE_SETTINGS, oberservr);

    if (!factoryTest) {
        intentFilter = NULL;
        stickyIntent = NULL;
        CIntentFilter::New(IAccountManager::LOGIN_ACCOUNTS_CHANGED_ACTION, (IIntentFilter**)&intentFilter);
        // Register for account list updates for all users
        mContext->RegisterReceiverAsUser(mAccountsUpdatedReceiver,
            UserHandle::ALL, intentFilter, nullStr, NULL, (IIntent**)&stickyIntent);
    }

    // Pick a random second in a day to seed all periodic syncs
    mSyncRandomOffsetMillis = mSyncStorageEngine->GetSyncRandomOffset() * 1000;
}

Int64 SyncManager::Jitterize(
    /* [in] */ Int64 minValue,
    /* [in] */ Int64 maxValue)
{
    AutoPtr<IRandom> random;
    CRandom::New(SystemClock::GetElapsedRealtime(), (IRandom**)&random);
    Int64 spread = maxValue - minValue;
    if (spread > Elastos::Core::Math::INT64_MAX_VALUE) {
        Logger::E(TAG, "IllegalArgumentException: the difference between the maxValue and the ",
            "minValue must be less than Integer.MAX_VALUE");
        // throw new IllegalArgumentException("the difference between the maxValue and the "
        //         + "minValue must be less than " + Integer.MAX_VALUE);
        return 0;
    }
    Int32 ival;
    random->NextInt32((Int32)spread, &ival);
    return minValue + ival;
}

AutoPtr<SyncStorageEngine> SyncManager::GetSyncStorageEngine()
{
    return mSyncStorageEngine;
}

Int32 SyncManager::GetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ const String& providerName)
{
    Int32 isSyncable = mSyncStorageEngine->GetIsSyncable(account, userId, providerName);
    AutoPtr<IUserManagerHelper> umHelper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&umHelper);
    AutoPtr<IUserManager> userManager;
    AutoPtr<IUserInfo> userInfo;
    umHelper->Get(mContext, (IUserManager**)&userManager);
    userManager->GetUserInfo(userId, (IUserInfo**)&userInfo);

    // If it's not a restricted user, return isSyncable
    Boolean bval;
    if (userInfo == NULL || (userInfo->IsRestricted(&bval), bval)) return isSyncable;

    // Else check if the sync adapter has opted-in or not
    AutoPtr<ISyncAdapterTypeHelper> satHelper;
    CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&satHelper);
    String type;
    account->GetType(&type);
    AutoPtr<ISyncAdapterType> sat;
    satHelper->NewKey(providerName, type, (ISyncAdapterType**)&sat);
    AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
    IRegisteredServicesCache::Probe(mSyncAdapters)->GetServiceInfo(sat, userId,
        (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);

    if (syncAdapterInfo == NULL) return isSyncable;

    AutoPtr<IComponentName> cn;
    syncAdapterInfo->GetComponentName((IComponentName**)&cn);
    String pkgName;
    cn->GetPackageName(&pkgName);

    AutoPtr<IPackageInfo> pInfo;
    ECode ec = AppGlobals::GetPackageManager()->GetPackageInfo(pkgName, 0, userId, (IPackageInfo**)&pInfo);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // Shouldn't happen
        return isSyncable;
    }
    if (pInfo == NULL) return isSyncable;

    String rattype;
    pInfo->GetRestrictedAccountType(&rattype);
    if (rattype != NULL && rattype.Equals(type)) {
        return isSyncable;
    }

    return 0;
}

void SyncManager::EnsureAlarmService()
{
    if (mAlarmService == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
        mAlarmService = IAlarmManager::Probe(obj);
    }
}

ECode SyncManager::ScheduleSync(
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 uid,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 beforeRunTimeMillis,
    /* [in] */ Int64 runtimeMillis)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    // if (isLoggable) {
    //     Logger::D(TAG, "one off sync for: " + cname + " " + extras->toString());
    // }

    Boolean expedited;
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, FALSE, &expedited);
    if (expedited) {
        runtimeMillis = -1; // this means schedule at the front of the queue
    }

    Boolean ignoreSettings;
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &ignoreSettings);
    Int32 source = SyncStorageEngine::SOURCE_SERVICE;
    Boolean isEnabled = mSyncStorageEngine->GetIsTargetServiceActive(cname, userId);
    // Only schedule this sync if
    //   - we've explicitly been told to ignore settings.
    //   - global sync is enabled for this user.
    Boolean syncAllowed =
        ignoreSettings || mSyncStorageEngine->GetMasterSyncAutomatically(userId);
    if (!syncAllowed) {
        // if (isLoggable) {
        //     Logger::D(TAG, "ScheduleSync: sync of " + cname + " not allowed, dropping request.");
        // }
        return NOERROR;
    }
    if (!isEnabled) {
        // if (isLoggable) {
        //     Logger::D(TAG, "ScheduleSync: " + cname + " is not enabled, dropping request");
        // }
        return NOERROR;
    }
    EndPoint* info = new EndPoint(cname, userId);
    AutoPtr< Pair<Int64, Int64> > backoff = mSyncStorageEngine->GetBackoff(info);
    Int64 delayUntil = mSyncStorageEngine->GetDelayUntilTime(info);
    Int64 backoffTime = backoff != NULL ? backoff->mFirst : 0;
    // if (isLoggable) {
    //         Logger::V(TAG, "schedule Sync:"
    //                 + ", delay until " + delayUntil
    //                 + ", run by " + runtimeMillis
    //                 + ", flex " + beforeRunTimeMillis
    //                 + ", source " + source
    //                 + ", sync service " + cname
    //                 + ", extras " + extras);
    // }
    AutoPtr<SyncOperation> so = new SyncOperation(
        cname, userId, uid, source, extras,
        runtimeMillis /* runtime */, beforeRunTimeMillis /* flextime */,
        backoffTime, delayUntil);
    ScheduleSyncOperation(so);
    return NOERROR;
}

CARAPI SyncManager::ScheduleSync(
    /* [in] */ IAccount* requestedAccount,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ const String& requestedAuthority,
    /* [in] */ IBundle* inExtras,
    /* [in] */ Int64 beforeRuntimeMillis,
    /* [in] */ Int64 runtimeMillis,
    /* [in] */ Boolean onlyThoseWithUnkownSyncableState)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    AutoPtr<IBundle> extras = inExtras;
    if (extras == NULL) {
        CBundle::New((IBundle**)&extras);
    }
    // if (isLoggable) {
    //     Logger::D(TAG, "one-time sync for: " + requestedAccount + " " + extras->toString() + " "
    //             + requestedAuthority);
    // }
    Boolean expedited;
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, FALSE, &expedited);
    if (expedited) {
        runtimeMillis = -1; // this means schedule at the front of the queue
    }

    AutoPtr<ArrayOf<IAccountAndUser*> > accounts;
    if (requestedAccount != NULL && userId != IUserHandle::USER_ALL) {
        AutoPtr<IAccountAndUser> aau;
        assert(0 && "TODO");
        // CAccountAndUser::New(requestedAccount, userId, (IAccountAndUser**)&aau);

        accounts = ArrayOf<IAccountAndUser*>::Alloc(1);
        accounts->Set(0, aau);
    }
    else {
        accounts = mRunningAccounts;
        if (accounts->GetLength() == 0) {
            // if (isLoggable) {
            //     Logger::V(TAG, "ScheduleSync: no accounts configured, dropping");
            // }
            return NOERROR;
        }
    }

    Boolean uploadOnly, manualSync;
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_UPLOAD, FALSE, &uploadOnly);
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, FALSE, &manualSync);
    if (manualSync) {
        extras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, TRUE);
        extras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, TRUE);
    }
    Boolean ignoreSettings;
    extras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &ignoreSettings);

    Int32 source;
    if (uploadOnly) {
        source = SyncStorageEngine::SOURCE_LOCAL;
    }
    else if (manualSync) {
        source = SyncStorageEngine::SOURCE_USER;
    }
    else if (requestedAuthority == NULL) {
        source = SyncStorageEngine::SOURCE_POLL;
    }
    else {
        // this isn't strictly server, since arbitrary callers can (and do) request
        // a non-forced two-way sync on a specific url
        source = SyncStorageEngine::SOURCE_SERVER;
    }

    IRegisteredServicesCache* syncAdapters = IRegisteredServicesCache::Probe(mSyncAdapters);
    AutoPtr<ISyncAdapterTypeHelper> satHelper;
    CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&satHelper);
    Int32 uid;
    Boolean allowParallelSyncs, isAlwaysSyncable, bval;

    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        IAccountAndUser* accountAndUser = (*accounts)[i];
        accountAndUser->GetUserId(&uid);
        AutoPtr<IAccount> account;
        accountAndUser->GetAccount((IAccount**)&account);

        // Compile a list of authorities that have sync adapters.
        // For each authority sync each account that matches a sync adapter.
        HashSet<String> syncableAuthorities;
        AutoPtr<IList> services;
        syncAdapters->GetAllServices(uid, (IList**)&services);
        AutoPtr<IIterator> sit;
        services->GetIterator((IIterator**)&sit);
        Boolean hasNext;
        IRegisteredServicesCacheServiceInfo* info;
        String authority;
        while (sit->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            sit->GetNext((IInterface**)&obj);
            info = IRegisteredServicesCacheServiceInfo::Probe(obj);
            AutoPtr<IInterface> typeObj;
            info->GetType((IInterface**)&typeObj);
            ISyncAdapterType::Probe(typeObj)->GetAuthority(&authority);
            syncableAuthorities.Insert(authority);
        }

        // if the url was specified then replace the list of authorities
        // with just this authority or clear it if this authority isn't
        // syncable
        if (requestedAuthority != NULL) {
            Boolean hasSyncAdapter = syncableAuthorities.Find(requestedAuthority) != syncableAuthorities.End();
            syncableAuthorities.Clear();
            if (hasSyncAdapter) syncableAuthorities.Insert(requestedAuthority);
        }

        HashSet<String>::Iterator it;
        for (it = syncableAuthorities.Begin(); it != syncableAuthorities.End(); ++it) {
            authority = *it;
            Int32 isSyncable = GetIsSyncable(account, uid, authority);
            if (isSyncable == 0) {
                continue;
            }

            String type;
            account->GetType(&type);
            AutoPtr<ISyncAdapterType> sat;
            satHelper->NewKey(authority, type, (ISyncAdapterType**)&sat);
            AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
            syncAdapters->GetServiceInfo(sat, uid,
                (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);
            if (syncAdapterInfo == NULL) {
                continue;
            }

            AutoPtr<IInterface> typeObj;
            info->GetType((IInterface**)&typeObj);
            ISyncAdapterType* satObj = ISyncAdapterType::Probe(typeObj);
            satObj->AllowParallelSyncs(&allowParallelSyncs);
            satObj->IsAlwaysSyncable(&isAlwaysSyncable);

            if (isSyncable < 0 && isAlwaysSyncable) {
                mSyncStorageEngine->SetIsSyncable(account, uid, authority, 1);
                isSyncable = 1;
            }
            if (onlyThoseWithUnkownSyncableState && isSyncable >= 0) {
                continue;
            }

            satObj->SupportsUploading(&bval);
            if (!bval && uploadOnly) {
                continue;
            }

            Boolean syncAllowed =
                (isSyncable < 0) // always allow if the isSyncable state is unknown
                || ignoreSettings
                || (mSyncStorageEngine->GetMasterSyncAutomatically(uid)
                    && mSyncStorageEngine->GetSyncAutomatically(account, uid, authority));
            if (!syncAllowed) {
                // if (isLoggable) {
                //     Logger::D(TAG, "ScheduleSync: sync of " + account + ", " + authority
                //             + " is not allowed, dropping request");
                // }
                continue;
            }

            AutoPtr<EndPoint> info = new EndPoint(account, authority, uid);
            AutoPtr<Pair<Int64, Int64> > backoff = mSyncStorageEngine->GetBackoff(info);
            Int64 delayUntil = mSyncStorageEngine->GetDelayUntilTime(info);
            Int64 backoffTime = backoff != NULL ? backoff->mFirst : 0;
            if (isSyncable < 0) {
                // Initialisation sync.
                AutoPtr<IBundle> newExtras;
                CBundle::New((IBundle**)&newExtras);
                newExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_INITIALIZE, TRUE);
                // if (isLoggable) {
                //     Logger::V(TAG, "schedule initialisation Sync:"
                //             + ", delay until " + delayUntil
                //             + ", run by " + 0
                //             + ", flex " + 0
                //             + ", source " + source
                //             + ", account " + account
                //             + ", authority " + authority
                //             + ", extras " + newExtras);
                // }
                AutoPtr<SyncOperation> so = new SyncOperation(account, uid, reason, source,
                    authority, newExtras, 0 /* immediate */, 0 /* No flex time*/,
                    backoffTime, delayUntil, allowParallelSyncs);
                ScheduleSyncOperation(so);
            }

            if (!onlyThoseWithUnkownSyncableState) {
                // if (isLoggable) {
                //     Logger::V(TAG, "ScheduleSync:"
                //             + " delay until " + delayUntil
                //             + " run by " + runtimeMillis
                //             + " flex " + beforeRuntimeMillis
                //             + ", source " + source
                //             + ", account " + account
                //             + ", authority " + authority
                //             + ", extras " + extras);
                // }
                AutoPtr<SyncOperation> so =  new SyncOperation(account, uid, reason, source,
                    authority, extras, runtimeMillis, beforeRuntimeMillis,
                    backoffTime, delayUntil, allowParallelSyncs);
                ScheduleSyncOperation(so);
            }
        }
    }
    return NOERROR;
}

ECode SyncManager::ScheduleLocalSync(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ const String& authority)
{
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    extras->PutBoolean(IContentResolver::SYNC_EXTRAS_UPLOAD, TRUE);
    return ScheduleSync(account, userId, reason, authority, extras,
        LOCAL_SYNC_DELAY /* earliest run time */,
        2 * LOCAL_SYNC_DELAY /* latest sync time. */,
        FALSE /* onlyThoseWithUnkownSyncableState */);
}

AutoPtr<ArrayOf<ISyncAdapterType*> > SyncManager::GetSyncAdapterTypes(
    /* [in] */ Int32 userId)
{
    AutoPtr<IList> serviceInfos;
    IRegisteredServicesCache::Probe(mSyncAdapters)->GetAllServices(userId, (IList**)&serviceInfos);

    Int32 size;
    serviceInfos->GetSize(&size);
    AutoPtr<ArrayOf<ISyncAdapterType*> > types = ArrayOf<ISyncAdapterType*>::Alloc(size);
    Int32 i = 0;
    AutoPtr<IIterator> it;
    serviceInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IRegisteredServicesCacheServiceInfo* info;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        info = IRegisteredServicesCacheServiceInfo::Probe(obj);
        AutoPtr<IInterface> typeObj;
        info->GetType((IInterface**)&typeObj);
        types->Set(i++, ISyncAdapterType::Probe(typeObj));
    }

    return types;
}

void SyncManager::SendSyncAlarmMessage()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "sending MESSAGE_SYNC_ALARM");
    Boolean bval;
    mSyncHandler->SendEmptyMessage(SyncHandler::MESSAGE_SYNC_ALARM, &bval);
}

void SyncManager::SendCheckAlarmsMessage()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "sending MESSAGE_CHECK_ALARMS");
    Boolean bval;
    mSyncHandler->RemoveMessages(SyncHandler::MESSAGE_CHECK_ALARMS);
    mSyncHandler->SendEmptyMessage(SyncHandler::MESSAGE_CHECK_ALARMS, &bval);
}

void SyncManager::SendSyncFinishedOrCanceledMessage(
    /* [in] */ ActiveSyncContext* syncContext,
    /* [in] */ ISyncResult* syncResult)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "sending MESSAGE_SYNC_FINISHED");
    AutoPtr<IMessage> msg;
    mSyncHandler->ObtainMessage((IMessage**)&msg);
    msg->SetWhat(SyncHandler::MESSAGE_SYNC_FINISHED);
    AutoPtr<SyncHandlerMessagePayload> payload = new SyncHandlerMessagePayload(syncContext, syncResult);
    msg->SetObj(TO_IINTERFACE(payload));
    Boolean bval;
    mSyncHandler->SendMessage(msg, &bval);
}

void SyncManager::SendCancelSyncsMessage(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "sending MESSAGE_CANCEL");
    AutoPtr<IMessage> msg;
    mSyncHandler->ObtainMessage((IMessage**)&msg);
    msg->SetWhat(SyncHandler::MESSAGE_CANCEL);
    msg->SetData(extras);
    msg->SetObj(TO_IINTERFACE(info));
    Boolean bval;
    mSyncHandler->SendMessage(msg, &bval);
}

void SyncManager::PostSyncExpiryMessage(
    /* [in] */ ActiveSyncContext* activeSyncContext)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "posting MESSAGE_SYNC_EXPIRED in " +
    //             (ACTIVE_SYNC_TIMEOUT_MILLIS/1000) + "s");
    // }
    AutoPtr<IMessage> msg;
    mSyncHandler->ObtainMessage((IMessage**)&msg);
    msg->SetWhat(SyncHandler::MESSAGE_SYNC_EXPIRED);
    msg->SetObj(TO_IINTERFACE(activeSyncContext));
    Boolean bval;
    mSyncHandler->SendMessageDelayed(msg, ACTIVE_SYNC_TIMEOUT_MILLIS, &bval);
}

void SyncManager::RemoveSyncExpiryMessage(
    /* [in] */ ActiveSyncContext* activeSyncContext)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Logger::V(TAG, "removing all MESSAGE_SYNC_EXPIRED for " + activeSyncContext->toString());
    // }
    mSyncHandler->RemoveMessages(SyncHandler::MESSAGE_SYNC_EXPIRED, TO_IINTERFACE(activeSyncContext));
}

void SyncManager::ClearBackoffSetting(
    /* [in] */ SyncOperation* op)
{
    mSyncStorageEngine->SetBackoff(op->mTarget,
        SyncStorageEngine::NOT_IN_BACKOFF_MODE,
        SyncStorageEngine::NOT_IN_BACKOFF_MODE);
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->OnBackoffChanged(op->mTarget, 0);
    }
}

void SyncManager::IncreaseBackoffSetting(
    /* [in] */ SyncOperation* op)
{
    // TODO: Use this function to align it to an already scheduled sync
    //       operation in the specified window
    Int64 now = SystemClock::GetElapsedRealtime();

    AutoPtr< Pair<Int64, Int64> > previousSettings = mSyncStorageEngine->GetBackoff(op->mTarget);
    Int64 newDelayInMs = -1;
    if (previousSettings != NULL) {
        // don't increase backoff before current backoff is expired. This will happen for op's
        // with ignoreBackoff set.
        if (now < previousSettings->mFirst) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Logger::V(TAG, "Still in backoff, do not increase it. "
            //         + "Remaining: " + ((previousSettings.first - now) / 1000) + " seconds.");
            // }
            return;
        }
        // Subsequent delays are the double of the previous delay
        newDelayInMs = previousSettings->mSecond * 2;
    }
    if (newDelayInMs <= 0) {
        // The initial delay is the jitterized INITIAL_SYNC_RETRY_TIME_IN_MS
        newDelayInMs = Jitterize(INITIAL_SYNC_RETRY_TIME_IN_MS,
                (Int64)(INITIAL_SYNC_RETRY_TIME_IN_MS * 1.1));
    }

    // Cap the delay
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int64 maxSyncRetryTimeInSeconds;
    AutoPtr<IContentResolver>  resolve;
    mContext->GetContentResolver((IContentResolver**)&resolve);
    global->GetInt64(resolve,
        ISettingsGlobal::SYNC_MAX_RETRY_DELAY_IN_SECONDS,
        DEFAULT_MAX_SYNC_RETRY_TIME_IN_SECONDS, &maxSyncRetryTimeInSeconds);
    if (newDelayInMs > maxSyncRetryTimeInSeconds * 1000) {
        newDelayInMs = maxSyncRetryTimeInSeconds * 1000;
    }

    Int64 backoff = now + newDelayInMs;

    mSyncStorageEngine->SetBackoff(op->mTarget, backoff, newDelayInMs);
    op->mBackoff = backoff;
    op->UpdateEffectiveRunTime();

    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->OnBackoffChanged(op->mTarget, backoff);
    }
}

void SyncManager::SetDelayUntilTime(
    /* [in] */ SyncOperation* op,
    /* [in] */ Int64 delayUntilSeconds)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 delayUntil = delayUntilSeconds * 1000;
    Int64 absoluteNow;
    system->GetCurrentTimeMillis(&absoluteNow);
    Int64 newDelayUntilTime;
    if (delayUntil > absoluteNow) {
        newDelayUntilTime = SystemClock::GetElapsedRealtime() + (delayUntil - absoluteNow);
    } else {
        newDelayUntilTime = 0;
    }
    mSyncStorageEngine->SetDelayUntilTime(op->mTarget, newDelayUntilTime);
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->OnDelayUntilTimeChanged(op->mTarget, newDelayUntilTime);
    }
}

void SyncManager::CancelActiveSync(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    SendCancelSyncsMessage(info, extras);
}

void SyncManager::ScheduleSyncOperation(
    /* [in] */ SyncOperation* syncOperation)
{
    Boolean queueChanged;
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        queueChanged = mSyncQueue->Add(syncOperation);
    }

    if (queueChanged) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "ScheduleSyncOperation: enqueued " + syncOperation);
        // }
        SendCheckAlarmsMessage();
    }
    else {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Logger::V(TAG, "ScheduleSyncOperation: dropping duplicate sync operation "
        //             + syncOperation);
        // }
    }
}

void SyncManager::ClearScheduledSyncOperations(
    /* [in] */ EndPoint* info)
{
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->Remove(info, NULL /* all operations */);
    }
    mSyncStorageEngine->SetBackoff(info,
            SyncStorageEngine::NOT_IN_BACKOFF_MODE, SyncStorageEngine::NOT_IN_BACKOFF_MODE);
}

void SyncManager::CancelScheduledSyncOperation(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->Remove(info, extras);
    }
    // Reset the back-off if there are no more syncs pending.
    if (!mSyncStorageEngine->IsSyncPending(info)) {
        mSyncStorageEngine->SetBackoff(info,
            SyncStorageEngine::NOT_IN_BACKOFF_MODE, SyncStorageEngine::NOT_IN_BACKOFF_MODE);
    }
}

void SyncManager::MaybeRescheduleSync(
    /* [in] */ ISyncResult* syncResult,
    /* [in] */ SyncOperation* op)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.DEBUG);
    // if (isLoggable) {
    //     Logger::D(TAG, "encountered error(s) during the sync: " + syncResult + ", " + operation);
    // }

    AutoPtr<SyncOperation> operation = new SyncOperation(op, 0 /* newRunTimeFromNow */);

    // The SYNC_EXTRAS_IGNORE_BACKOFF only applies to the first attempt to sync a given
    // request. Retries of the request will always honor the backoff, so clear the
    // flag in case we retry this request.
    Boolean bval, bval2;
    operation->mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, FALSE, &bval);
    if (bval) {
        operation->mExtras->Remove(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF);
    }

    operation->mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY, FALSE, &bval);
    if (bval) {
        // if (isLoggable) {
        //     Logger::D(TAG, "not retrying sync operation because SYNC_EXTRAS_DO_NOT_RETRY was specified "
        //             + operation);
        // }
    }
    else if ((operation->mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_UPLOAD, FALSE, &bval), bval)
            && (syncResult->GetSyncAlreadyInProgress(&bval2), !bval2)) {
        // If this was an upward sync then schedule a two-way sync immediately.
        operation->mExtras->Remove(IContentResolver::SYNC_EXTRAS_UPLOAD);
        // if (isLoggable) {
        //     Logger::D(TAG, "retrying sync operation as a two-way sync because an upload-only sync "
        //             + "encountered an error: " + operation);
        // }
        ScheduleSyncOperation(operation);
    }
    else if (syncResult->GetTooManyRetries(&bval), bval) {
        // If this sync aborted because the internal sync loop retried too many times then
        //   don't reschedule. Otherwise we risk getting into a retry loop.
        // if (isLoggable) {
        //     Logger::D(TAG, "not retrying sync operation because it retried too many times: "
        //             + operation);
        // }
    }
    else if (syncResult->MadeSomeProgress(&bval), bval) {
        // If the operation succeeded to some extent then retry immediately.
        // if (isLoggable) {
        //     Logger::D(TAG, "retrying sync operation because even though it had an error "
        //             + "it achieved some success");
        // }
        ScheduleSyncOperation(operation);
    }
    else if (syncResult->GetSyncAlreadyInProgress(&bval), bval) {
        // if (isLoggable) {
        //     Logger::D(TAG, "retrying sync operation that failed because there was already a "
        //             + "sync in progress: " + operation);
        // }
        AutoPtr<SyncOperation> so = new SyncOperation(
            operation,
            DELAY_RETRY_SYNC_IN_PROGRESS_IN_SECONDS * 1000 /* newRunTimeFromNow */);

        ScheduleSyncOperation(so);
    }
    else if (syncResult->HasSoftError(&bval), bval) {
        // If this was a two-way sync then retry soft errors with an exponential backoff.
        // if (isLoggable) {
        //     Logger::D(TAG, "retrying sync operation because it encountered a soft error: "
        //             + operation);
        // }
        ScheduleSyncOperation(operation);
    }
    else {
        // Otherwise do not reschedule.
        // Logger::D(TAG, "not retrying sync operation because the error is a hard error: "
        //         + operation);
    }
}

void SyncManager::OnUserStarting(
    /* [in] */ Int32 userId)
{
    // Make sure that accounts we're about to use are valid
    ECode ec = NOERROR;
    assert(0 && "TODO");
    // ec = CAccountManagerService::GetSingleton()->ValidateAccounts(userId);
    if (FAILED(ec)) return;

    IRegisteredServicesCache::Probe(mSyncAdapters)->InvalidateCache(userId);

    UpdateRunningAccounts();

    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->AddPendingOperations(userId);
    }

    // Schedule sync for any accounts under started user
    String nullStr;
    AutoPtr<ArrayOf<IAccount*> > accounts;
    assert(0 && "TODO");
    //CAccountManagerService->GetSingleton()->GetAccounts(userId, (ArrayOf<IAccount*>**)&accounts);
    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        IAccount* account = (*accounts)[i];
        ScheduleSync(account, userId, SyncOperation::REASON_USER_START, nullStr, NULL,
            0 /* no delay */, 0 /* No flex */,
            TRUE /* onlyThoseWithUnknownSyncableState */);
    }

    SendCheckAlarmsMessage();
}

void SyncManager::OnUserStopping(
    /* [in] */ Int32 userId)
{
    UpdateRunningAccounts();

    AutoPtr<EndPoint> ep = new EndPoint(NULL /* any account */, String(NULL) /* any authority */, userId);
    CancelActiveSync(ep, NULL /* any sync. */);
}

void SyncManager::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    UpdateRunningAccounts();

    // Clean up the storage engine database
    AutoPtr<ArrayOf<IAccount*> > accounts = ArrayOf<IAccount*>::Alloc(0);
    mSyncStorageEngine->DoDatabaseCleanup(accounts, userId);
    Object* obj = mSyncQueue.Get();
    synchronized(obj) {
        mSyncQueue->RemoveUserLocked(userId);
    }
}

void SyncManager::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw)
{
    // IndentingPrintWriter ipw = new IndentingPrintWriter(pw, "  ");
    // DumpSyncState(ipw);
    // DumpSyncHistory(ipw);
    // DumpSyncAdapters(ipw);
}

String SyncManager::FormatTime(
    /* [in] */ Int64 time)
{
    AutoPtr<ITime> tobj;
    CTime::New((ITime**)&tobj);
    tobj->Set(time);
    String result;
    tobj->Format(String("%Y-%m-%d %H:%M:%S"), &result);
    return result;
}

void SyncManager::DumpSyncState(
    /* [in] */ IPrintWriter* pw)
{
    // pw->Print("data connected: "); pw->Println(mDataConnectionIsConnected);
    // pw->Print("auto sync: ");
    // List<UserInfo> users = GetAllUsers();
    // if (users != NULL) {
    //     for (UserInfo user : users) {
    //         pw->Print("u" + user.id + "="
    //                 + mSyncStorageEngine->GetMasterSyncAutomatically(user.id) + " ");
    //     }
    //     pw->Println();
    // }
    // pw->Print("memory low: "); pw->Println(mStorageIsLow);

    // AccountAndUser[] accounts = AccountManagerService.getSingleton().getAllAccounts();

    // pw->Print("accounts: ");
    // if (accounts != INITIAL_ACCOUNTS_ARRAY) {
    //     pw->Println(accounts.length);
    // } else {
    //     pw->Println("not known yet");
    // }
    // Int64 now = SystemClock::GetElapsedRealtime();
    // pw->Print("now: "); pw->Print(now);
    // pw->Println(" (" + FormatTime(System.currentTimeMillis()) + ")");
    // pw->Print("offset: "); pw->Print(DateUtils.formatElapsedTime(mSyncRandomOffsetMillis/1000));
    // pw->Println(" (HH:MM:SS)");
    // pw->Print("uptime: "); pw->Print(DateUtils.formatElapsedTime(now/1000));
    //         pw->Println(" (HH:MM:SS)");
    // pw->Print("time spent syncing: ");
    //         pw->Print(DateUtils.formatElapsedTime(
    //                 mSyncHandler->mSyncTimeTracker.TimeSpentSyncing() / 1000));
    //         pw->Print(" (HH:MM:SS), sync ");
    //         pw->Print(mSyncHandler->mSyncTimeTracker.mLastWasSyncing ? "" : "not ");
    //         pw->Println("in progress");
    // if (mSyncHandler->mAlarmScheduleTime != -1) {
    //     pw->Print("next alarm time: "); pw->Print(mSyncHandler->mAlarmScheduleTime);
    //             pw->Print(" (");
    //             pw->Print(DateUtils.formatElapsedTime((mSyncHandler->mAlarmScheduleTime-now)/1000));
    //             pw->Println(" (HH:MM:SS) from now)");
    // } else {
    //     pw->Println("no alarm is scheduled (there had better not be any pending syncs)");
    // }

    // pw->Print("notification info: ");
    // StringBuilder sb = new StringBuilder();
    // mSyncHandler->mSyncNotificationInfo.toString(sb);
    // pw->Println(sb.toString());

    // pw->Println();
    // pw->Println("Active Syncs: " + mActiveSyncContexts.size());
    // PackageManager pm = mContext->GetPackageManager();
    // for (SyncManager.ActiveSyncContext activeSyncContext : mActiveSyncContexts) {
    //     Int64 durationInSeconds = (now - activeSyncContext->mStartTime) / 1000;
    //     pw->Print("  ");
    //     pw->Print(DateUtils.formatElapsedTime(durationInSeconds));
    //     pw->Print(" - ");
    //     pw->Print(activeSyncContext->mSyncOperation.dump(pm, FALSE));
    //     pw->Println();
    // }

    // Object* obj = mSyncQueue.Get();
    // synchronized(obj) {
    //     sb.setLength(0);
    //     mSyncQueue.dump(sb);
    //     // Dump Pending Operations.
    //     GetSyncStorageEngine().dumpPendingOperations(sb);
    // }

    // pw->Println();
    // pw->Print(sb.toString());

    // // join the installed sync adapter with the accounts list and emit for everything
    // pw->Println();
    // pw->Println("Sync Status");
    // for (AccountAndUser account : accounts) {
    //     pw->Printf("Account %s u%d %s\n",
    //             account->mAccount.name, account->mUserId, account->mAccount.type);

    //     pw->Println("=======================================================================");
    //     PrintTable table = new PrintTable(13);
    //     table.set(0, 0,
    //             "Authority", // 0
    //             "Syncable",  // 1
    //             "Enabled",   // 2
    //             "Delay",     // 3
    //             "Loc",       // 4
    //             "Poll",      // 5
    //             "Per",       // 6
    //             "Serv",      // 7
    //             "User",      // 8
    //             "Tot",       // 9
    //             "Time",      // 10
    //             "Last Sync", // 11
    //             "Periodic"   // 12
    //     );

    //     List<RegisteredServicesCache.ServiceInfo<SyncAdapterType>> sorted =
    //             Lists.newArrayList();
    //     sorted.addAll(IRegisteredServicesCache::Probe(mSyncAdapters)->getAllServices(account->mUserId));
    //     Collections.sort(sorted,
    //             new Comparator<RegisteredServicesCache.ServiceInfo<SyncAdapterType>>() {
    //         //@Override
    //         Int32 compare(RegisteredServicesCache.ServiceInfo<SyncAdapterType> lhs,
    //                 RegisteredServicesCache.ServiceInfo<SyncAdapterType> rhs) {
    //             return lhs.type.authority.compareTo(rhs.type.authority);
    //         }
    //     });
    //     for (RegisteredServicesCache.ServiceInfo<SyncAdapterType> syncAdapterType : sorted) {
    //         if (!syncAdapterType.type->mAccountType.Equals(account->mAccount.type)) {
    //             continue;
    //         }
    //         Int32 row = table.getNumRows();
    //         Pair<AuthorityInfo, SyncStatusInfo> syncAuthoritySyncStatus =
    //                 mSyncStorageEngine->getCopyOfAuthorityWithSyncStatus(
    //                         new EndPoint*(
    //                                 account->mAccount,
    //                                 syncAdapterType.type.authority,
    //                                 account->mUserId));
    //         SyncStorageEngine::AuthorityInfo settings = syncAuthoritySyncStatus.first;
    //         SyncStatusInfo status = syncAuthoritySyncStatus.second;
    //         String authority = settings.target->mProvider;
    //         if (authority.length() > 50) {
    //             authority = authority.substring(authority.length() - 50);
    //         }
    //         table.set(row, 0, authority, settings.syncable, settings.enabled);
    //         table.set(row, 4,
    //                 status.numSourceLocal,
    //                 status.numSourcePoll,
    //                 status.numSourcePeriodic,
    //                 status.numSourceServer,
    //                 status.numSourceUser,
    //                 status.numSyncs,
    //                 DateUtils.formatElapsedTime(status.totalElapsedTime / 1000));


    //         for (Int32 i = 0; i < settings.periodicSyncs.size(); i++) {
    //             PeriodicSync sync = settings.periodicSyncs.get(i);
    //             String period =
    //                     String.format("[p:%d s, f: %d s]", sync.period, sync.flexTime);
    //             String extras =
    //                     sync.extras->size() > 0 ?
    //                             sync.extras->toString() : "Bundle[]";
    //             String next = "Next sync: " + FormatTime(status.getPeriodicSyncTime(i)
    //                     + sync.period * 1000);
    //             table.set(row + i * 2, 12, period + " " + extras);
    //             table.set(row + i * 2 + 1, 12, next);
    //         }

    //         Int32 row1 = row;
    //         if (settings.delayUntil > now) {
    //             table.set(row1++, 12, "D: " + (settings.delayUntil - now) / 1000);
    //             if (settings.backoffTime > now) {
    //                 table.set(row1++, 12, "B: " + (settings.backoffTime - now) / 1000);
    //                 table.set(row1++, 12, settings.backoffDelay / 1000);
    //             }
    //         }

    //         if (status.lastSuccessTime != 0) {
    //             table.set(row1++, 11, SyncStorageEngine::SOURCES[status.lastSuccessSource]
    //                     + " " + "SUCCESS");
    //             table.set(row1++, 11, FormatTime(status.lastSuccessTime));
    //         }
    //         if (status.lastFailureTime != 0) {
    //             table.set(row1++, 11, SyncStorageEngine::SOURCES[status.lastFailureSource]
    //                     + " " + "FAILURE");
    //             table.set(row1++, 11, FormatTime(status.lastFailureTime));
    //             //noinspection UnusedAssignment
    //             table.set(row1++, 11, status.lastFailureMesg);
    //         }
    //     }
    //     table.writeTo(pw);
    // }
}

String SyncManager::GetLastFailureMessage(
    /* [in] */ Int32 code)
{
    switch (code) {
        case IContentResolver::SYNC_ERROR_SYNC_ALREADY_IN_PROGRESS:
            return String("sync already in progress");

        case IContentResolver::SYNC_ERROR_AUTHENTICATION:
            return String("authentication error");

        case IContentResolver::SYNC_ERROR_IO:
            return String("I/O error");

        case IContentResolver::SYNC_ERROR_PARSE:
            return String("parse error");

        case IContentResolver::SYNC_ERROR_CONFLICT:
            return String("conflict error");

        case IContentResolver::SYNC_ERROR_TOO_MANY_DELETIONS:
            return String("too many deletions error");

        case IContentResolver::SYNC_ERROR_TOO_MANY_RETRIES:
            return String("too many retries error");

        case IContentResolver::SYNC_ERROR_INTERNAL:
            return String("internal error");
    }
    return String("unknown");
}

void SyncManager::DumpTimeSec(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 time)
{
    // pw->Print(time/1000); pw->Print('.'); pw->Print((time/100)%10);
    // pw->Print('s');
}

void SyncManager::DumpDayStatistic(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DayStats* ds)
{
    // pw->Print("Success ("); pw->Print(ds.successCount);
    // if (ds.successCount > 0) {
    //     pw->Print(" for "); DumpTimeSec(pw, ds.successTime);
    //     pw->Print(" avg="); DumpTimeSec(pw, ds.successTime/ds.successCount);
    // }
    // pw->Print(") Failure ("); pw->Print(ds.failureCount);
    // if (ds.failureCount > 0) {
    //     pw->Print(" for "); DumpTimeSec(pw, ds.failureTime);
    //     pw->Print(" avg="); DumpTimeSec(pw, ds.failureTime/ds.failureCount);
    // }
    // pw->Println(")");
}

void SyncManager::DumpSyncHistory(
    /* [in] */ IPrintWriter* pw)
{
    DumpRecentHistory(pw);
    DumpDayStatistics(pw);
}

void SyncManager::DumpRecentHistory(
    /* [in] */ IPrintWriter* pw)
{
    // ArrayList<SyncStorageEngine.SyncHistoryItem> items
    //         = mSyncStorageEngine->getSyncHistory();
    // if (items != NULL && items.size() > 0) {
    //     Map<String, AuthoritySyncStats> authorityMap = Maps.newHashMap();
    //     Int64 totalElapsedTime = 0;
    //     Int64 totalTimes = 0;
    //     Int32 N = items.size();

    //     Int32 maxAuthority = 0;
    //     Int32 maxAccount = 0;
    //     for (SyncStorageEngine.SyncHistoryItem item : items) {
    //         SyncStorageEngine.AuthorityInfo authorityInfo
    //                 = mSyncStorageEngine->GetAuthority(item.authorityId);
    //         String authorityName;
    //         String accountKey;
    //         if (authorityInfo != NULL) {
    //             if (authorityInfo.target->mTarget_provider) {
    //                 authorityName = authorityInfo.target->mProvider;
    //                 accountKey = authorityInfo.target->mAccount.name + "/"
    //                         + authorityInfo.target->mAccount.type
    //                         + " u" + authorityInfo.target->mUserId;
    //             } else if (authorityInfo.target->mTarget_service) {
    //                 authorityName = authorityInfo.target->mService.getPackageName() + "/"
    //                         + authorityInfo.target->mService.getClassName()
    //                         + " u" + authorityInfo.target->mUserId;
    //                 accountKey = "no account";
    //             } else {
    //                 authorityName = "Unknown";
    //                 accountKey = "Unknown";
    //             }
    //         } else {
    //             authorityName = "Unknown";
    //             accountKey = "Unknown";
    //         }

    //         Int32 length = authorityName.length();
    //         if (length > maxAuthority) {
    //             maxAuthority = length;
    //         }
    //         length = accountKey.length();
    //         if (length > maxAccount) {
    //             maxAccount = length;
    //         }

    //         Int64 elapsedTime = item.elapsedTime;
    //         totalElapsedTime += elapsedTime;
    //         totalTimes++;
    //         AuthoritySyncStats authoritySyncStats = authorityMap.get(authorityName);
    //         if (authoritySyncStats == NULL) {
    //             authoritySyncStats = new AuthoritySyncStats(authorityName);
    //             authorityMap.put(authorityName, authoritySyncStats);
    //         }
    //         authoritySyncStats.elapsedTime += elapsedTime;
    //         authoritySyncStats.times++;
    //         Map<String, AccountSyncStats> accountMap = authoritySyncStats->mAccountMap;
    //         AccountSyncStats accountSyncStats = accountMap.get(accountKey);
    //         if (accountSyncStats == NULL) {
    //             accountSyncStats = new AccountSyncStats(accountKey);
    //             accountMap.put(accountKey, accountSyncStats);
    //         }
    //         accountSyncStats.elapsedTime += elapsedTime;
    //         accountSyncStats.times++;

    //     }

    //     if (totalElapsedTime > 0) {
    //         pw->Println();
    //         pw->Printf("Detailed Statistics (Recent history):  "
    //                 + "%d (# of times) %ds (sync time)\n",
    //                 totalTimes, totalElapsedTime / 1000);

    //         List<AuthoritySyncStats> sortedAuthorities =
    //                 new ArrayList<AuthoritySyncStats>(authorityMap.values());
    //         Collections.sort(sortedAuthorities, new Comparator<AuthoritySyncStats>() {
    //             //@Override
    //             Int32 compare(AuthoritySyncStats lhs, AuthoritySyncStats rhs) {
    //                 // reverse order
    //                 Int32 compare = Integer.compare(rhs.times, lhs.times);
    //                 if (compare == 0) {
    //                     compare = Int64.compare(rhs.elapsedTime, lhs.elapsedTime);
    //                 }
    //                 return compare;
    //             }
    //         });

    //         Int32 maxLength = Math::Max(maxAuthority, maxAccount + 3);
    //         Int32 padLength = 2 + 2 + maxLength + 2 + 10 + 11;
    //         char chars[] = new char[padLength];
    //         Arrays.fill(chars, '-');
    //         String separator = new String(chars);

    //         String authorityFormat =
    //                 String.format("  %%-%ds: %%-9s  %%-11s\n", maxLength + 2);
    //         String accountFormat =
    //                 String.format("    %%-%ds:   %%-9s  %%-11s\n", maxLength);

    //         pw->Println(separator);
    //         for (AuthoritySyncStats authoritySyncStats : sortedAuthorities) {
    //             String name = authoritySyncStats.name;
    //             Int64 elapsedTime;
    //             Int32 times;
    //             String timeStr;
    //             String timesStr;

    //             elapsedTime = authoritySyncStats.elapsedTime;
    //             times = authoritySyncStats.times;
    //             timeStr = String.format("%ds/%d%%",
    //                     elapsedTime / 1000,
    //                     elapsedTime * 100 / totalElapsedTime);
    //             timesStr = String.format("%d/%d%%",
    //                     times,
    //                     times * 100 / totalTimes);
    //             pw->Printf(authorityFormat, name, timesStr, timeStr);

    //             List<AccountSyncStats> sortedAccounts =
    //                     new ArrayList<AccountSyncStats>(
    //                             authoritySyncStats->mAccountMap.values());
    //             Collections.sort(sortedAccounts, new Comparator<AccountSyncStats>() {
    //                 //@Override
    //                 Int32 compare(AccountSyncStats lhs, AccountSyncStats rhs) {
    //                     // reverse order
    //                     Int32 compare = Integer.compare(rhs.times, lhs.times);
    //                     if (compare == 0) {
    //                         compare = Int64.compare(rhs.elapsedTime, lhs.elapsedTime);
    //                     }
    //                     return compare;
    //                 }
    //             });
    //             for (AccountSyncStats stats: sortedAccounts) {
    //                 elapsedTime = stats.elapsedTime;
    //                 times = stats.times;
    //                 timeStr = String.format("%ds/%d%%",
    //                         elapsedTime / 1000,
    //                         elapsedTime * 100 / totalElapsedTime);
    //                 timesStr = String.format("%d/%d%%",
    //                         times,
    //                         times * 100 / totalTimes);
    //                 pw->Printf(accountFormat, stats.name, timesStr, timeStr);
    //             }
    //             pw->Println(separator);
    //         }
    //     }

    //     pw->Println();
    //     pw->Println("Recent Sync History");
    //     String format = "  %-" + maxAccount + "s  %-" + maxAuthority + "s %s\n";
    //     Map<String, Int64> lastTimeMap = Maps.newHashMap();
    //     PackageManager pm = mContext->GetPackageManager();
    //     for (Int32 i = 0; i < N; i++) {
    //         SyncStorageEngine.SyncHistoryItem item = items.get(i);
    //         SyncStorageEngine.AuthorityInfo authorityInfo
    //                 = mSyncStorageEngine->GetAuthority(item.authorityId);
    //         String authorityName;
    //         String accountKey;
    //         if (authorityInfo != NULL) {
    //             if (authorityInfo.target->mTarget_provider) {
    //                 authorityName = authorityInfo.target->mProvider;
    //                 accountKey = authorityInfo.target->mAccount.name + "/"
    //                         + authorityInfo.target->mAccount.type
    //                         + " u" + authorityInfo.target->mUserId;
    //             } else if (authorityInfo.target->mTarget_service) {
    //                 authorityName = authorityInfo.target->mService.getPackageName() + "/"
    //                         + authorityInfo.target->mService.getClassName()
    //                         + " u" + authorityInfo.target->mUserId;
    //                 accountKey = "none";
    //             } else {
    //                 authorityName = "Unknown";
    //                 accountKey = "Unknown";
    //             }
    //         } else {
    //             authorityName = "Unknown";
    //             accountKey = "Unknown";
    //         }
    //         Int64 elapsedTime = item.elapsedTime;
    //         Time time = new Time();
    //         Int64 eventTime = item.eventTime;
    //         time.set(eventTime);

    //         String key = authorityName + "/" + accountKey;
    //         Int64 lastEventTime = lastTimeMap.get(key);
    //         String diffString;
    //         if (lastEventTime == NULL) {
    //             diffString = "";
    //         } else {
    //             Int64 diff = (lastEventTime - eventTime) / 1000;
    //             if (diff < 60) {
    //                 diffString = String.valueOf(diff);
    //             } else if (diff < 3600) {
    //                 diffString = String.format("%02d:%02d", diff / 60, diff % 60);
    //             } else {
    //                 Int64 sec = diff % 3600;
    //                 diffString = String.format("%02d:%02d:%02d",
    //                         diff / 3600, sec / 60, sec % 60);
    //             }
    //         }
    //         lastTimeMap.put(key, eventTime);

    //         pw->Printf("  #%-3d: %s %8s  %5.1fs  %8s",
    //                 i + 1,
    //                 FormatTime(eventTime),
    //                 SyncStorageEngine::SOURCES[item.source],
    //                 ((float) elapsedTime) / 1000,
    //                 diffString);
    //         pw->Printf(format, accountKey, authorityName,
    //                 SyncOperation.reasonToString(pm, item.reason));

    //         if (item.event != SyncStorageEngine::EVENT_STOP
    //                 || item.upstreamActivity != 0
    //                 || item.downstreamActivity != 0) {
    //             pw->Printf("    event=%d upstreamActivity=%d downstreamActivity=%d\n",
    //                     item.event,
    //                     item.upstreamActivity,
    //                     item.downstreamActivity);
    //         }
    //         if (item.mesg != NULL
    //                 && !SyncStorageEngine::MESG_SUCCESS.Equals(item.mesg)) {
    //             pw->Printf("    mesg=%s\n", item.mesg);
    //         }
    //     }
    //     pw->Println();
    //     pw->Println("Recent Sync History Extras");
    //     for (Int32 i = 0; i < N; i++) {
    //         SyncStorageEngine.SyncHistoryItem item = items.get(i);
    //         Bundle extras = item.extras;
    //         if (extras == NULL || extras->size() == 0) {
    //             continue;
    //         }
    //         SyncStorageEngine.AuthorityInfo authorityInfo
    //                 = mSyncStorageEngine->GetAuthority(item.authorityId);
    //         String authorityName;
    //         String accountKey;
    //         if (authorityInfo != NULL) {
    //             if (authorityInfo.target->mTarget_provider) {
    //                 authorityName = authorityInfo.target->mProvider;
    //                 accountKey = authorityInfo.target->mAccount.name + "/"
    //                         + authorityInfo.target->mAccount.type
    //                         + " u" + authorityInfo.target->mUserId;
    //             } else if (authorityInfo.target->mTarget_service) {
    //                 authorityName = authorityInfo.target->mService.getPackageName() + "/"
    //                         + authorityInfo.target->mService.getClassName()
    //                         + " u" + authorityInfo.target->mUserId;
    //                 accountKey = "none";
    //             } else {
    //                 authorityName = "Unknown";
    //                 accountKey = "Unknown";
    //             }
    //         } else {
    //             authorityName = "Unknown";
    //             accountKey = "Unknown";
    //         }
    //         Time time = new Time();
    //         Int64 eventTime = item.eventTime;
    //         time.set(eventTime);

    //         pw->Printf("  #%-3d: %s %8s ",
    //                 i + 1,
    //                 FormatTime(eventTime),
    //                 SyncStorageEngine::SOURCES[item.source]);

    //         pw->Printf(format, accountKey, authorityName, extras);
    //     }
    // }
}

void SyncManager::DumpDayStatistics(
    /* [in] */ IPrintWriter* pw)
{
    // SyncStorageEngine.DayStats dses[] = mSyncStorageEngine->getDayStatistics();
    // if (dses != NULL && dses[0] != NULL) {
    //     pw->Println();
    //     pw->Println("Sync Statistics");
    //     pw->Print("  Today:  "); DumpDayStatistic(pw, dses[0]);
    //     Int32 today = dses[0].day;
    //     Int32 i;
    //     SyncStorageEngine.DayStats ds;

    //     // Print each day in the current week.
    //     for (i=1; i<=6 && i < dses.length; i++) {
    //         ds = dses[i];
    //         if (ds == NULL) break;
    //         Int32 delta = today-ds.day;
    //         if (delta > 6) break;

    //         pw->Print("  Day-"); pw->Print(delta); pw->Print(":  ");
    //         DumpDayStatistic(pw, ds);
    //     }

    //     // Aggregate all following days into weeks and print totals.
    //     Int32 weekDay = today;
    //     while (i < dses.length) {
    //         SyncStorageEngine.DayStats aggr = NULL;
    //         weekDay -= 7;
    //         while (i < dses.length) {
    //             ds = dses[i];
    //             if (ds == NULL) {
    //                 i = dses.length;
    //                 break;
    //             }
    //             Int32 delta = weekDay-ds.day;
    //             if (delta > 6) break;
    //             i++;

    //             if (aggr == NULL) {
    //                 aggr = new SyncStorageEngine.DayStats(weekDay);
    //             }
    //             aggr.successCount += ds.successCount;
    //             aggr.successTime += ds.successTime;
    //             aggr.failureCount += ds.failureCount;
    //             aggr.failureTime += ds.failureTime;
    //         }
    //         if (aggr != NULL) {
    //             pw->Print("  Week-"); pw->Print((today-weekDay)/7); pw->Print(": ");
    //             DumpDayStatistic(pw, aggr);
    //         }
    //     }
    // }
}

void SyncManager::DumpSyncAdapters(
    /* [in] */ IIndentingPrintWriter* pw)
{
    // pw->Println();
    // List<UserInfo> users = GetAllUsers();
    // if (users != NULL) {
    //     for (UserInfo user : users) {
    //         pw->Println("Sync adapters for " + user + ":");
    //         pw.increaseIndent();
    //         for (RegisteredServicesCache.ServiceInfo<?> info :
    //                 IRegisteredServicesCache::Probe(mSyncAdapters)->getAllServices(user.id)) {
    //             pw->Println(info);
    //         }
    //         pw.decreaseIndent();
    //         pw->Println();
    //     }
    // }
}

Boolean SyncManager::IsSyncStillActive(
    /* [in] */ ActiveSyncContext* activeSyncContext)
{
    List<AutoPtr<ActiveSyncContext> >::Iterator it;
    for (it != mActiveSyncContexts.Begin(); it != mActiveSyncContexts.End(); ++it) {
        ActiveSyncContext* sync = *it;
        if (sync == activeSyncContext) {
            return TRUE;
        }
    }

    return FALSE;
}

Boolean SyncManager::SyncExtrasEquals(
    /* [in] */ IBundle* b1,
    /* [in] */ IBundle* b2,
    /* [in] */ Boolean includeSyncSettings)
{
    if (b1 == b2) {
        return TRUE;
    }
    // Exit early if we can.
    Int32 s1, s2;
    b1->GetSize(&s1);
    b2->GetSize(&s2);
    if (includeSyncSettings && s1 != s2) {
        return FALSE;
    }
    AutoPtr<IBundle> bigger = s1 > s2 ? b1 : b2;
    AutoPtr<IBundle> smaller = s1 > s2 ? b2 : b1;

    AutoPtr<ISet> keyset;
    bigger->GetKeySet((ISet**)&keyset);
    Boolean hasNext, bval;
    AutoPtr<IIterator> it;
    keyset->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String key = Object::ToString(obj);

        if (!includeSyncSettings && IsSyncSetting(key)) {
            continue;
        }
        if (smaller->ContainsKey(key, &bval), !bval) {
            return FALSE;
        }

        AutoPtr<IInterface> o1, o2;
        bigger->Get(key, (IInterface**)&o1);
        smaller->Get(key, (IInterface**)&o2);
        if (!Object::Equals(o1, o2)) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean SyncManager::IsSyncSetting(
    /* [in] */ const String& key)
{
    if (key.Equals(IContentResolver::SYNC_EXTRAS_EXPEDITED)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_MANUAL)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_UPLOAD)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_OVERRIDE_TOO_MANY_DELETIONS)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_DISCARD_LOCAL_DELETIONS)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_EXPECTED_UPLOAD)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_EXPECTED_DOWNLOAD)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_PRIORITY)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_DISALLOW_METERED)) {
        return TRUE;
    }
    if (key.Equals(IContentResolver::SYNC_EXTRAS_INITIALIZE)) {
        return TRUE;
    }
    return FALSE;
}

AutoPtr<IContext> SyncManager::GetContextForUser(
    /* [in] */ IUserHandle* user)
{
    String pkgName;
    mContext->GetPackageName(&pkgName);
    AutoPtr<IContext> ctx;
    ECode ec = mContext->CreatePackageContextAsUser(pkgName, 0, user, (IContext**)&ctx);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        // Default to mContext, not finding the package system is running as is unlikely.
        return mContext;
    }
    return ctx;
}


} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos
