#ifndef __ELASTOS_DROID_SERVER_CONTENT_SYNCMANAGER_H__
#define __ELASTOS_DROID_SERVER_CONTENT_SYNCMANAGER_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include "elastos/droid/server/content/SyncQueue.h"
#include "elastos/droid/server/content/SyncStorageEngine.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountAndUser;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IISyncAdapter;
using Elastos::Droid::Content::IISyncContext;
using Elastos::Droid::Content::IISyncServiceAdapter;
using Elastos::Droid::Content::IISyncStatusObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::ISyncAdaptersCache;
using Elastos::Droid::Content::ISyncInfo;
using Elastos::Droid::Content::ISyncResult;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheListener;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Text::Format::ITime;

using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;

using Elastos::Core::IComparator;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IRandom;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {


//===============================================================================
// SyncManager
//===============================================================================

/**
 * @hide
 */
class SyncManager
    : public Object
    , public ISyncManager
{
class SyncTimeTracker;
class AccountSyncStats;

public:

    class ActiveSyncContext;
    class SyncHandler;

    class SyncHandlerMessagePayload
        : public Object
    {
    public:

        SyncHandlerMessagePayload(
            /* [in] */ ActiveSyncContext* syncContext,
            /* [in] */ ISyncResult* syncResult);

    public:
        AutoPtr<ActiveSyncContext> mActiveSyncContext;
        AutoPtr<ISyncResult>  mSyncResult;
    };

    class SyncAlarmIntentReceiver
        : public BroadcastReceiver
    {
    public:
        SyncAlarmIntentReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };


    /**
     * @hide
     */
    class ActiveSyncContext
        : public Object
        , public IActiveSyncContext
        , public IISyncContext
        , public IBinder
        , public IServiceConnection
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ActiveSyncContext();

        virtual ~ActiveSyncContext();

        /**
         * Create an ActiveSyncContext for an impending sync and grab the wakelock for that
         * sync adapter. Since this grabs the wakelock you need to be sure to call
         * close() when you are done with this ActiveSyncContext, whether the sync succeeded
         * or not.
         * @param syncOperation the SyncOperation we are about to sync
         * @param historyRowId the row in which to record the history info for this sync
         * @param syncAdapterUid the UID of the application that contains the sync adapter
         * for this sync. This is used to attribute the wakelock hold to that application.
         */
        CARAPI constructor(
            /* [in] */ ISyncManager* syncMgr,
            /* [in] */ ISyncOperation* syncOperation,
            /* [in] */ Int64 historyRowId,
            /* [in] */ Int32 syncAdapterUid);

        CARAPI SendHeartbeat();

        CARAPI OnFinished(
            /* [in] */ ISyncResult* result);

        CARAPI ToString(
            /* [in] */ StringBuilder& sb);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        Boolean BindToSyncAdapter(
            /* [in] */ IComponentName* serviceComponent,
            /* [in] */ Int32 userId);

        /**
         * Performs the required cleanup, which is the releasing of the wakelock and
         * unbinding from the sync adapter (if actually bound).
         */
        CARAPI Close();

        CARAPI ToString(
            /* [out] */ String* str);

        //@Override
        CARAPI ProxyDied();

    private:
        friend class SyncManager;
        friend class SyncStorageEngine;

        SyncManager* mHost;
        AutoPtr<SyncOperation> mSyncOperation;
        Int64 mHistoryRowId;
        AutoPtr<IISyncAdapter> mSyncAdapter;
        AutoPtr<IISyncServiceAdapter> mSyncServiceAdapter;
        Int64 mStartTime;
        Int64 mTimeoutStartTime;
        Boolean mBound;
        AutoPtr<IPowerManagerWakeLock> mSyncWakeLock;
        Int32 mSyncAdapterUid;
        AutoPtr<ISyncInfo> mSyncInfo;
        Boolean mIsLinkedToDeath;
        String mEventName;
    };

    class ServiceConnectionData
        : public Object
    {
    public:
        ServiceConnectionData(
            /* [in] */ ActiveSyncContext* activeSyncContext,
            /* [in] */ IBinder* adapter);
    private:
        friend class SyncHandler;
        AutoPtr<ActiveSyncContext> mActiveSyncContext;
        AutoPtr<IBinder> mAdapter;
    };

    class PrintTable
        : public Object
    {
    public:
        PrintTable(
            /* [in] */ Int32 cols);

        CARAPI Set(
            /* [in] */ Int32 row,
            /* [in] */ Int32 col,
            /* [in] */ ArrayOf<String>* values);

        void WriteTo(
            /* [in] */ IPrintWriter* out);

        Int32 GetNumRows();

    private:
        void PrintRow(
            /* [in] */ IPrintWriter* out,
            /* [in] */ ArrayOf<String>* formats,
            /* [in] */ ArrayOf<String>* row);

    private:
        List<AutoPtr<ArrayOf<String> > > mTable;
        Int32 mCols;
    };

    //=================================================================================
    // SyncHandler
    //=================================================================================

    /**
     * Handles SyncOperation Messages that are posted to the associated
     * HandlerThread.
     */
    class SyncHandler
        : public Handler
    {
    public:
        /**
         * Used to keep track of whether a sync notification is active and who it is for.
         */
        class SyncNotificationInfo
            : public Object
        {
        public:
            SyncNotificationInfo()
                : mIsActive(FALSE)
                , mStartTime(0)
            {}

            CARAPI ToString(
                /* [in] */ StringBuilder& sb);

            //@Override
            CARAPI ToString(
                /* [out] */ String* str);
        public:
            // true iff the notification manager has been asked to send the notification
            Boolean mIsActive;

            // Set when we transition from not running a sync to running a sync, and cleared on
            // the opposite transition.
            Int64 mStartTime;
        };

    public:

        CARAPI OnBootCompleted();

        SyncHandler(
            /* [in] */ SyncManager* host);

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        Int64 InsertStartSyncEvent(
            /* [in] */ SyncOperation* syncOperation);

        void StopSyncEvent(
            /* [in] */ Int64 rowId,
            /* [in] */ SyncOperation* syncOperation,
            /* [in] */ const String& resultMessage,
            /* [in] */ Int32 upstreamActivity,
            /* [in] */ Int32 downstreamActivity,
            /* [in] */ Int64 elapsedTime);

    private:

        AutoPtr<IPowerManagerWakeLock> GetSyncWakeLock(
            /* [in] */ SyncOperation* operation);

        /**
         * Stash any messages that come to the handler before boot is complete.
         * {@link #OnBootCompleted()} will disable this and dispatch all the messages collected.
         * @param msg Message to dispatch at a later point.
         * @return true if a message was enqueued, FALSE otherwise. This is to avoid losing the
         * message if we manage to acquire the lock but by the time we do boot has completed.
         */
        Boolean TryEnqueueMessageUntilReadyToRun(
            /* [in] */ IMessage* msg);

        Boolean IsDispatchable(
            /* [in] */ EndPoint* target);

        /**
         * Turn any periodic sync operations that are ready to run into pending sync operations.
         * @return the desired start time of the earliest future periodic sync operation,
         * in milliseconds since boot
         */
        Int64 ScheduleReadyPeriodicSyncs();

        Int64 MaybeStartNextSyncLocked();

        /**
         * Determine if a sync is no longer valid and should be dropped from the sync queue and its
         * pending op deleted.
         * @param op operation for which the sync is to be scheduled.
         */
        Boolean IsOperationValidLocked(
            /* [in] */ SyncOperation* op);

        Boolean DispatchSyncOperation(
            /* [in] */ SyncOperation* op);

        void RunBoundToAdapter(
            /* [in] */ ActiveSyncContext* activeSyncContext,
            /* [in] */ IBinder* syncAdapter);

        /**
         * Cancel the sync for the provided target that matches the given bundle.
         * @param info can have null fields to indicate all the active syncs for that field.
         */
        void CancelActiveSyncLocked(
            /* [in] */ EndPoint* info,
            /* [in] */ IBundle* extras);

        void RunSyncFinishedOrCanceledLocked(
            /* [in] */ ISyncResult* syncResult,
            /* [in] */ ActiveSyncContext* activeSyncContext);

        void CloseActiveSyncContext(
            /* [in] */ ActiveSyncContext* activeSyncContext);

        /**
         * Convert the error-containing SyncResult into the Sync.History error number. Since
         * the SyncResult may indicate multiple errors at once, this method just returns the
         * most "serious" error.
         * @param syncResult the SyncResult from which to read
         * @return the most "serious" error set in the SyncResult
         * @throws IllegalStateException if the SyncResult does not indicate any errors.
         *   If SyncResult.error() is true then it is safe to call this.
         */
        Int32 SyncResultToErrorNumber(
            /* [in] */ ISyncResult* syncResult);

        void ManageSyncNotificationLocked();

        void ManageSyncAlarmLocked(
            /* [in] */ Int64 nextPeriodicEventElapsedTime,
            /* [in] */ Int64 nextPendingEventElapsedTime);

        void SendSyncStateIntent();

        void InstallHandleTooManyDeletesNotification(
            /* [in] */ IAccount* account,
            /* [in] */ const String& authority,
            /* [in] */ Int64 numDeletes,
            /* [in] */ Int32 userId);

        /**
         * Checks whether an activity exists on the system image for the given intent.
         *
         * @param intent The intent for an activity.
         * @return Whether or not an activity exists.
         */
        Boolean IsActivityAvailable(
            /* [in] */ IIntent* intent);

    public:

        AutoPtr<SyncNotificationInfo> mSyncNotificationInfo;

        AutoPtr<SyncTimeTracker> mSyncTimeTracker;

    private:
        SyncManager* mHost;
        friend class SyncManager;

        // Messages that can be sent on mHandler
        static const Int32 MESSAGE_SYNC_FINISHED;
        static const Int32 MESSAGE_SYNC_ALARM;
        static const Int32 MESSAGE_CHECK_ALARMS;
        static const Int32 MESSAGE_SERVICE_CONNECTED;
        static const Int32 MESSAGE_SERVICE_DISCONNECTED;
        static const Int32 MESSAGE_CANCEL;
        /** Posted delayed in order to expire syncs that are Int64-running. */
        static const Int32 MESSAGE_SYNC_EXPIRED;

        Int64 mAlarmScheduleTime;
        HashMap<String, AutoPtr<IPowerManagerWakeLock> > mWakeLocks;

        AutoPtr< List<AutoPtr<IMessage> > > mBootQueue;
    };

    class SyncStatusObserver
        : public Object
        , public IISyncStatusObserver
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISyncManager* host);

        CARAPI OnStatusChanged(
            /* [in] */ Int32 which);
    private:
        SyncManager* mHost;
    };
//==========================================================================================

private:

    class StorageIntentReceiver
        : public BroadcastReceiver
    {
    public:
        StorageIntentReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class BootCompletedReceiver
        : public BroadcastReceiver
    {
    public:
        BootCompletedReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class AccountsUpdatedReceiver
        : public BroadcastReceiver
    {
    public:
        AccountsUpdatedReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class ConnectivityIntentReceiver
        : public BroadcastReceiver
    {
    public:
        ConnectivityIntentReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class ShutdownIntentReceiver
        : public BroadcastReceiver
    {
    public:
        ShutdownIntentReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class UserIntentReceiver
        : public BroadcastReceiver
    {
    public:
        UserIntentReceiver(
            /* [in] */ SyncManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        SyncManager* mHost;
    };

    class AuthoritySyncStats
        : public Object
    {
    public:
        String mName;
        Int64 mElapsedTime;
        Int32 mTimes;
        HashMap<String, AutoPtr<AccountSyncStats> > mAccountMap;

    private:
        AuthoritySyncStats(
            /* [in] */ const String& name);
    };

    class AccountSyncStats
        : public Object
    {
    public:
        String mName;
        Int64 mElapsedTime;
        Int32 mTimes;

    private:
        AccountSyncStats(
            /* [in] */ const String& name);
    };

    /**
     * A helper object to keep track of the time we have spent syncing since the last boot
     */
    class SyncTimeTracker
        : public Object
    {
    public:
        SyncTimeTracker(
            /* [in] */ SyncManager* host);

        /** Call to let the tracker know that the sync state may have changed */
        void Update();

        /** Get how Int64 we have been syncing, in ms */
         Int64 TimeSpentSyncing();

    public:
        /** True if a sync was in progress on the most recent call to update() */
        Boolean mLastWasSyncing;
        /** Used to track when lastWasSyncing was last set */
        Int64 mWhenSyncStarted;

    private:
        /** The cumulative time we have spent syncing */
        Int64 mTimeSpentSyncing;
        SyncManager* mHost;
    };

    class MyOnSyncRequestListener
        : public Object
        , public IOnSyncRequestListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnSyncRequestListener(
            /* [in] */ SyncManager* host);

        CARAPI OnSyncRequest(
            /* [in] */ ISyncStorageEngineEndPoint* info,
            /* [in] */ Int32 reason,
            /* [in] */ IBundle* extras);
    private:
        SyncManager* mHost;
    };

    class MyRegisteredServicesCacheListener
        : public Object
        , public IRegisteredServicesCacheListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyRegisteredServicesCacheListener(
            /* [in] */ SyncManager* host);

        CARAPI OnServiceChanged(
            /* [in] */ IInterface* type,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean removed);
    private:
        SyncManager* mHost;
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    void UpdateRunningAccounts();

    /**
     * Should only be created after {@link ContentService#systemReady()} so that
     * {@link PackageManager} is ready to query.
     */
    SyncManager(
        /* [in] */ IContext* context,
        /* [in] */ Boolean factoryTest);

    AutoPtr<SyncStorageEngine> GetSyncStorageEngine();

    Int32 GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName);

    /**
     * Initiate a sync using the new anonymous service API.
     * @param cname SyncService component bound to in order to perform the sync.
     * @param userId the id of the user whose accounts are to be synced. If userId is USER_ALL,
     *          then all users' accounts are considered.
     * @param uid Linux uid of the application that is performing the sync.
     * @param extras a Map of SyncAdapter-specific information to control
     *          syncs of a specific provider. Cannot be null.
     * @param beforeRunTimeMillis milliseconds before <code>runtimeMillis</code> that this sync may
     * be run.
     * @param runtimeMillis milliseconds from now by which this sync must be run.
     */
    CARAPI ScheduleSync(
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 uid,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 beforeRunTimeMillis,
        /* [in] */ Int64 runtimeMillis);

    /**
     * Initiate a sync. This can start a sync for all providers
     * (pass null to url, set onlyTicklable to FALSE), only those
     * providers that are marked as ticklable (pass null to url,
     * set onlyTicklable to true), or a specific provider (set url
     * to the content url of the provider).
     *
     * <p>If the IContentResolver::SYNC_EXTRAS_UPLOAD Boolean in extras is
     * true then initiate a sync that just checks for local changes to send
     * to the server, otherwise initiate a sync that first gets any
     * changes from the server before sending local changes back to
     * the server.
     *
     * <p>If a specific provider is being synced (the url is non-null)
     * then the extras can contain SyncAdapter-specific information
     * to control what gets synced (e.g. which specific feed to sync).
     *
     * <p>You'll start getting callbacks after this.
     *
     * @param requestedAccount the account to sync, may be null to signify all accounts
     * @param userId the id of the user whose accounts are to be synced. If userId is USER_ALL,
     *          then all users' accounts are considered.
     * @param reason for sync request. If this is a positive integer, it is the Linux uid
     * assigned to the process that requested the sync. If it's negative, the sync was requested by
     * the SyncManager itself and could be one of the following:
     *      {@link SyncOperation#REASON_BACKGROUND_DATA_SETTINGS_CHANGED}
     *      {@link SyncOperation#REASON_ACCOUNTS_UPDATED}
     *      {@link SyncOperation#REASON_SERVICE_CHANGED}
     *      {@link SyncOperation#REASON_PERIODIC}
     *      {@link SyncOperation#REASON_IS_SYNCABLE}
     *      {@link SyncOperation#REASON_SYNC_AUTO}
     *      {@link SyncOperation#REASON_MASTER_SYNC_AUTO}
     *      {@link SyncOperation#REASON_USER_START}
     * @param requestedAuthority the authority to sync, may be null to indicate all authorities
     * @param extras a Map of SyncAdapter-specific information to control
     *          syncs of a specific provider. Can be null. Is ignored
     *          if the url is null.
     * @param beforeRuntimeMillis milliseconds before runtimeMillis that this sync can run.
     * @param runtimeMillis maximum milliseconds in the future to wait before performing sync.
     * @param onlyThoseWithUnkownSyncableState Only sync authorities that have unknown state.
     */
    CARAPI ScheduleSync(
        /* [in] */ IAccount* requestedAccount,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ const String& requestedAuthority,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 beforeRuntimeMillis,
        /* [in] */ Int64 runtimeMillis,
        /* [in] */ Boolean onlyThoseWithUnkownSyncableState);

    /**
     * Schedule sync based on local changes to a provider. Occurs within interval
     * [LOCAL_SYNC_DELAY, 2*LOCAL_SYNC_DELAY].
     */
    CARAPI ScheduleLocalSync(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ const String& authority);

    AutoPtr<ArrayOf<ISyncAdapterType*> > GetSyncAdapterTypes(
        /* [in] */ Int32 userId);

    /**
     * Cancel the active sync if it matches the target.
     * @param info object containing info about which syncs to cancel. The target can
     * have null account/provider info to specify all accounts/providers.
     * @param extras if non-null, specifies the exact sync to remove.
     */
    void CancelActiveSync(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    /**
     * Create and schedule a SyncOperation.
     *
     * @param syncOperation the SyncOperation to schedule
     */
    void ScheduleSyncOperation(
        /* [in] */ SyncOperation* syncOperation);

    /**
     * Remove scheduled sync operations.
     * @param info limit the removals to operations that match this target. The target can
     * have null account/provider info to specify all accounts/providers.
     */
    void ClearScheduledSyncOperations(
        /* [in] */ EndPoint* info);

    /**
     * Remove a specified sync, if it exists.
     * @param info Authority for which the sync is to be removed.
     * @param extras extras bundle to uniquely identify sync.
     */
    void CancelScheduledSyncOperation(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    void MaybeRescheduleSync(
        /* [in] */ ISyncResult* syncResult,
        /* [in] */ SyncOperation* operation);

    static String FormatTime(
        /* [in] */ Int64 time);

    /**
     * Sync extra comparison function.
     * @param b1 bundle to compare
     * @param b2 other bundle to compare
     * @param includeSyncSettings if FALSE, ignore system settings in bundle.
     */
    static Boolean SyncExtrasEquals(
        /* [in] */ IBundle* b1,
        /* [in] */ IBundle* b2,
        /* [in] */ Boolean includeSyncSettings);

private:

    AutoPtr<IList> GetAllUsers();

    Boolean ContainsAccountAndUser(
        /* [in] */ ArrayOf<IAccountAndUser*>* accounts,
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId);

    void DoDatabaseCleanup();

    Boolean ReadDataConnectionState();

    AutoPtr<IConnectivityManager> GetConnectivityManager();

    /**
     * Return a random value v that satisfies minValue <= v < maxValue. The difference between
     * maxValue and minValue must be less than Integer.MAX_VALUE.
     */
    Int64 Jitterize(
        /* [in] */ Int64 minValue,
        /* [in] */ Int64 maxValue);

    void EnsureAlarmService();

    void SendSyncAlarmMessage();

    void SendCheckAlarmsMessage();

    void SendSyncFinishedOrCanceledMessage(
        /* [in] */ ActiveSyncContext* syncContext,
        /* [in] */ ISyncResult* syncResult);

    void SendCancelSyncsMessage(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    /**
     * Post a delayed message to the handler that will result in the cancellation of the provided
     * running sync's context.
     */
    void PostSyncExpiryMessage(
        /* [in] */ ActiveSyncContext* activeSyncContext);

    /**
     * Remove any time-outs previously posted for the provided active sync.
     */
    void RemoveSyncExpiryMessage(
        /* [in] */ ActiveSyncContext* activeSyncContext);


    void ClearBackoffSetting(
        /* [in] */ SyncOperation* op);

    void IncreaseBackoffSetting(
        /* [in] */ SyncOperation* op);

    void SetDelayUntilTime(
        /* [in] */ SyncOperation* op,
        /* [in] */ Int64 delayUntilSeconds);

    void OnUserStarting(
        /* [in] */ Int32 userId);

    void OnUserStopping(
        /* [in] */ Int32 userId);

    void OnUserRemoved(
        /* [in] */ Int32 userId);

    void Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw);

    void DumpSyncState(
        /* [in] */ IPrintWriter* pw);

    String GetLastFailureMessage(
        /* [in] */ Int32 code);

    void DumpTimeSec(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 time);

    void DumpDayStatistic(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DayStats* ds);

    void DumpSyncHistory(
        /* [in] */ IPrintWriter* pw);

    void DumpRecentHistory(
        /* [in] */ IPrintWriter* pw);

    void DumpDayStatistics(
        /* [in] */ IPrintWriter* pw);

    void DumpSyncAdapters(
        /* [in] */ IIndentingPrintWriter* pw);

    Boolean IsSyncStillActive(
        /* [in] */ ActiveSyncContext* activeSyncContext);

    /**
     * TODO: Get rid of this when we separate sync settings extras from dev specified extras.
     * @return true if the provided key is used by the SyncManager in scheduling the sync.
     */
    static Boolean IsSyncSetting(
        /* [in] */ const String& key);

    AutoPtr<IContext> GetContextForUser(
        /* [in] */ IUserHandle* user);

private:

    static const String TAG;

    /** Delay a sync due to local changes this Int64. In milliseconds */
    static Int64 LOCAL_SYNC_DELAY;

    /**
     * If a sync takes longer than this and the sync queue is not empty then we will
     * cancel it and add it back to the end of the sync queue. In milliseconds.
     */
    static Int64 MAX_TIME_PER_SYNC;

    static Int64 SYNC_NOTIFICATION_DELAY;

    /**
     * When retrying a sync for the first time use this delay. After that
     * the retry time will double until it reached MAX_SYNC_RETRY_TIME.
     * In milliseconds.
     */
    static const Int64 INITIAL_SYNC_RETRY_TIME_IN_MS; // 30 seconds

    /**
     * Default the max sync retry time to this value.
     */
    static const Int64 DEFAULT_MAX_SYNC_RETRY_TIME_IN_SECONDS; // one hour

    /**
     * How Int64 to wait before retrying a sync that failed due to one already being in progress.
     */
    static const Int32 DELAY_RETRY_SYNC_IN_PROGRESS_IN_SECONDS;

    /**
     * How Int64 to wait before considering an active sync to have timed-out, and cancelling it.
     */
    static const Int64 ACTIVE_SYNC_TIMEOUT_MILLIS;  // 30 mins.

    static const String SYNC_WAKE_LOCK_PREFIX;
    static const String HANDLE_SYNC_ALARM_WAKE_LOCK;
    static const String SYNC_LOOP_WAKE_LOCK;

    static Int32 MAX_SIMULTANEOUS_REGULAR_SYNCS;
    static Int32 MAX_SIMULTANEOUS_INITIALIZATION_SYNCS;

    AutoPtr<IContext> mContext;

    static const AutoPtr<ArrayOf<IAccountAndUser*> > INITIAL_ACCOUNTS_ARRAY;

    // TODO: add better locking around mRunningAccounts
    /* volatile */ AutoPtr<ArrayOf<IAccountAndUser*> > mRunningAccounts;

    /* volatile */ AutoPtr<IPowerManagerWakeLock> mHandleAlarmWakeLock;
    /* volatile */ AutoPtr<IPowerManagerWakeLock> mSyncManagerWakeLock;
    /* volatile */ Boolean mDataConnectionIsConnected;
    /* volatile */ Boolean mStorageIsLow;

    AutoPtr<INotificationManager> mNotificationMgr;
    AutoPtr<IAlarmManager> mAlarmService;
    AutoPtr<IIBatteryStats> mBatteryStats;

    AutoPtr<SyncStorageEngine> mSyncStorageEngine;

    // @GuardedBy("mSyncQueue")
    AutoPtr<SyncQueue> mSyncQueue;

    List<AutoPtr<ActiveSyncContext> > mActiveSyncContexts;

    // set if the sync active indicator should be reported
    Boolean mNeedSyncActiveNotification;

    AutoPtr<IPendingIntent> mSyncAlarmIntent;
    // Synchronized on "this". Instead of using this directly one should instead call
    // its accessor, getConnManager().
    AutoPtr<IConnectivityManager> mConnManagerDoNotUseDirectly;

    AutoPtr<ISyncAdaptersCache> mSyncAdapters;

    AutoPtr<IBroadcastReceiver> mStorageIntentReceiver;
    AutoPtr<IBroadcastReceiver> mBootCompletedReceiver;
    AutoPtr<IBroadcastReceiver> mAccountsUpdatedReceiver;
    AutoPtr<IBroadcastReceiver> mConnectivityIntentReceiver;
    AutoPtr<IBroadcastReceiver> mShutdownIntentReceiver;
    AutoPtr<IBroadcastReceiver> mUserIntentReceiver;

    static const String ACTION_SYNC_ALARM;
    AutoPtr<SyncHandler> mSyncHandler;

    volatile Boolean mBootCompleted;

    AutoPtr<IPowerManager> mPowerManager;

    // Use this as a random offset to seed all periodic syncs.
    Int32 mSyncRandomOffsetMillis;

    AutoPtr<IUserManager> mUserManager;

    static const Int64 SYNC_ALARM_TIMEOUT_MIN; // 30 seconds
    static const Int64 SYNC_ALARM_TIMEOUT_MAX; // two hours

    static const StaticInitializer sInitializer;
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_SYNCMANAGER_H__
