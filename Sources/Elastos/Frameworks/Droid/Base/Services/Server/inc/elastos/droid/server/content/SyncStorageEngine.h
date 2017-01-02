//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_CONTENT_SYNC_STORAGE_ENGINE_H__
#define __ELASTOS_DROID_SERVER_CONTENT_SYNC_STORAGE_ENGINE_H__

#include <elastos/droid/ext/frameworkext.h>
#define HASH_FOR_ACCOUNTS
#define HASH_FOR_CONTENT
#include <elastos/droid/ext/frameworkhash.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos.Droid.Server.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Content::IISyncStatusObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IPeriodicSync;
using Elastos::Droid::Content::ISyncInfo;
using Elastos::Droid::Content::ISyncStatusInfo;
using Elastos::Droid::Content::ISyncAdaptersCache;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountAndUser;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFile;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

class SyncQueue;
class SyncOperation;

class DayStats
    : public Object
{
public:
    Int32 mDay;
    Int32 mSuccessCount;
    Int64 mSuccessTime;
    Int32 mFailureCount;
    Int64 mFailureTime;

    DayStats(
        /* [in] */ Int32 day);
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Content::DayStats, IInterface)

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

class EndPoint;
class AuthorityInfo;

class PendingOperation
    : public Object
{
public:
    PendingOperation(
        /* [in] */ AuthorityInfo* authority,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 source,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean expedited);

    PendingOperation(
        /* [in] */ PendingOperation* other);

    /**
     * Considered equal if they target the same sync adapter (A
     * {@link android.content.SyncService}
     * is considered an adapter), for the same userId.
     * @param other PendingOperation to compare.
     * @return true if the two pending ops are the same.
     */
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<EndPoint> mTarget;
    Int32 mReason;
    Int32 mSyncSource;
    AutoPtr<IBundle> mExtras;        // note: read-only.
    Boolean mExpedited;

    Int32 mAuthorityId;
    // No longer used.
    // Keep around for sake up updating from pending.bin to pending.xml
    AutoPtr<ArrayOf<Byte> > mFlatExtras;
};

class AccountInfo
    : public Object
{
public:
    AutoPtr<IAccountAndUser> mAccountAndUser;
    HashMap<String, AutoPtr<AuthorityInfo> > mAuthorities;

    AccountInfo(
        /* [in] */ IAccountAndUser* accountAndUser);
};

/**  Bare bones representation of a sync target. */
class EndPoint
    : public Object
    , public ISyncStorageEngineEndPoint
{
public:
    CAR_INTERFACE_DECL()

    EndPoint(
        /* [in] */ IComponentName* service,
        /* [in] */ Int32 userId);

    EndPoint(
        /* [in] */ IAccount* account,
        /* [in] */ const String& provider,
        /* [in] */ Int32 userId);

    /**
     * An Endpoint for a sync matches if it targets the same sync adapter for the same user.
     *
     * @param spec the Endpoint to match. If the spec has null fields, they indicate a wildcard
     * and match any.
     */
    CARAPI_(Boolean) MatchesSpec(
        /* [in] */ ISyncStorageEngineEndPoint* spec);

    CARAPI MatchesSpec(
        /* [in] */ ISyncStorageEngineEndPoint* spec,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static const AutoPtr<EndPoint> USER_ALL_PROVIDER_ALL_ACCOUNTS_ALL;
    AutoPtr<IComponentName> mService;
    AutoPtr<IAccount> mAccount;
    Int32 mUserId;
    String mProvider;
    Boolean mTarget_service;
    Boolean mTarget_provider;
};

class AuthorityInfo
    : public Object
{
public:

    /**
     * Copy constructor for making deep-ish copies. Only the bundles stored
     * in periodic syncs can make unexpected changes.
     *
     * @param toCopy AuthorityInfo to be copied.
     */
    AuthorityInfo(
        /* [in] */ AuthorityInfo* toCopy);

    AuthorityInfo(
        /* [in] */ EndPoint* info,
        /* [in] */ Int32 id);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    void DefaultInitialisation();

public:
    AutoPtr<EndPoint> mTarget;
    Int32 mIdent;
    Boolean mEnabled;
    Int32 mSyncable;
    /** Time at which this sync will run, taking into account backoff. */
    Int64 mBackoffTime;
    /** Amount of delay due to backoff. */
    Int64 mBackoffDelay;
    /** Time offset to add to any requests coming to this target. */
    Int64 mDelayUntil;

    List<AutoPtr<IPeriodicSync> > mPeriodicSyncs;
};

class SyncHistoryItem
    : public Object
{
public:
    SyncHistoryItem();

    Int32 mAuthorityId;
    Int32 mHistoryId;
    Int64 mEventTime;
    Int64 mElapsedTime;
    Int32 mSource;
    Int32 mEvent;
    Int64 mUpstreamActivity;
    Int64 mDownstreamActivity;
    String mMsg;
    Boolean mInitialization;
    AutoPtr<IBundle> mExtras;
    Int32 mReason;
};

//=========================================================================================
// typedefs
//=========================================================================================

typedef Pair<AutoPtr<AuthorityInfo>, AutoPtr<ISyncStatusInfo> > AuthoritySyncStatusPair;
typedef HashMap<AutoPtr<IComponentName>, AutoPtr<HashMap<Int32, AutoPtr<AuthorityInfo> > > > ServiceMap;
typedef typename ServiceMap::Iterator ServiceMapIterator;

//=========================================================================================
// SyncStorageEngine
//=========================================================================================

class SyncStorageEngine
    : public Handler
{
public:
    TO_STRING_IMPL("SyncStorageEngine")

    SyncStorageEngine(
        /* [in] */ IContext* context,
        /* [in] */ IFile* dataDir);

    static AutoPtr<SyncStorageEngine> NewTestInstance(
        /* [in] */ IContext* context);

    static void Init(
        /* [in] */ IContext* context);

    static AutoPtr<SyncStorageEngine> GetSingleton();

    void SetOnSyncRequestListener(
        /* [in] */ IOnSyncRequestListener* listener);

    //@Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    Int32 GetSyncRandomOffset();

    void AddStatusChangeListener(
        /* [in] */ Int32 mask,
        /* [in] */ IISyncStatusObserver* callback);

    void RemoveStatusChangeListener(
        /* [in] */ IISyncStatusObserver* callback);

    /**
     * Figure out a reasonable flex time for cases where none is provided (old api calls).
     * @param syncTimeSeconds requested sync time from now.
     * @return amount of seconds before syncTimeSeconds that the sync can occur.
     *      I.e.
     *      earliest_sync_time = syncTimeSeconds - CalculateDefaultFlexTime(syncTimeSeconds)
     * The flex time is capped at a percentage of the {@link #DEFAULT_POLL_FREQUENCY_SECONDS}.
     */
    static Int64 CalculateDefaultFlexTime(
        /* [in] */ Int64 syncTimeSeconds);

    Boolean GetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName);

    void SetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName,
        /* [in] */ Boolean sync);

    Int32 GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName);

    void SetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 syncable);

    Boolean GetIsTargetServiceActive(
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId);

    void SetIsTargetServiceActive(
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean active);

    AutoPtr< Pair<Int64, Int64> > GetBackoff(
        /* [in] */ EndPoint* info);

    /**
     * Update the backoff for the given endpoint. The endpoint may be for a provider/account and
     * the account or provider info be null, which signifies all accounts or providers.
     */
    void SetBackoff(
        /* [in] */ EndPoint* info,
        /* [in] */ Int64 nextSyncTime,
        /* [in] */ Int64 nextDelay);

    void ClearAllBackoffsLocked(
        /* [in] */ SyncQueue* syncQueue);

    Int64 GetDelayUntilTime(
        /* [in] */ EndPoint* info);

    void SetDelayUntilTime(
        /* [in] */ EndPoint* info,
        /* [in] */ Int64 delayUntil);

    void UpdateOrAddPeriodicSync(
        /* [in] */ EndPoint* info,
        /* [in] */ Int64 period,
        /* [in] */ Int64 flextime,
        /* [in] */ IBundle* extras);

    void RemovePeriodicSync(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    /**
     * @return list of periodic syncs for a target. Never null. If no such syncs exist, returns an
     * empty list.
     */
    AutoPtr<List<AutoPtr<IPeriodicSync> > > GetPeriodicSyncs(
        /* [in] */ EndPoint* info);

    void SetMasterSyncAutomatically(
        /* [in] */ Boolean flag,
        /* [in] */ Int32 userId);

    Boolean GetMasterSyncAutomatically(
        /* [in] */ Int32 userId);

    AutoPtr<AuthorityInfo> GetAuthority(
        /* [in] */ Int32 authorityId);

    /**
     * Returns true if there is currently a sync operation being actively processed for the given
     * target.
     */
    Boolean IsSyncActive(
        /* [in] */ EndPoint* info);

    AutoPtr<PendingOperation> InsertIntoPending(
        /* [in] */ SyncOperation* op);

    /**
     * Remove from list of pending operations. If successful, search through list for matching
     * authorities. If there are no more pending syncs for the same target,
     * update the SyncStatusInfo for that target.
     * @param op Pending op to delete.
     */
    Boolean DeleteFromPending(
        /* [in] */ PendingOperation* op);

    /**
     * Return a copy of the current array of pending operations.  The
     * PendingOperation objects are the real objects stored inside, so that
     * they can be used with DeleteFromPending().
     */
    AutoPtr<List<AutoPtr<PendingOperation> > > GetPendingOperations();

    /**
     * Return the number of currently pending operations.
     */
    Int32 GetPendingOperationCount();

    /**
     * Called when the set of account has changed, given the new array of
     * active accounts.
     */
    void DoDatabaseCleanup(
        /* [in] */ ArrayOf<IAccount*>* accounts,
        /* [in] */ Int32 userId);

    /**
     * Called when a sync is starting. Supply a valid ActiveSyncContext with information
     * about the sync.
     */
    AutoPtr<ISyncInfo> AddActiveSync(
        /* [in] */ IActiveSyncContext* activeSyncContext);

    /**
     * Called to indicate that a previously active sync is no longer active.
     */
    void RemoveActiveSync(
        /* [in] */ ISyncInfo* syncInfo,
        /* [in] */ Int32 userId);

    /**
     * To allow others to send active change reports, to poke clients.
     */
    void ReportActiveChange();

    /**
     * Note that sync has started for the given operation.
     */
    Int64 InsertStartSyncEvent(
        /* [in] */ SyncOperation* op,
        /* [in] */ Int64 now);

    void StopSyncEvent(
        /* [in] */ Int64 historyId,
        /* [in] */ Int64 elapsedTime,
        /* [in] */ const String& resultMessage,
        /* [in] */ Int64 downstreamActivity,
        /* [in] */ Int64 upstreamActivity);

    /**
     * @return a copy of the current syncs data structure. Will not return
     * null.
     */
    AutoPtr<IList> GetCurrentSyncsCopy(
        /* [in] */ Int32 userId);

    /**
     * Return an array of the current sync status for all authorities.  Note
     * that the objects inside the array are the real, live status objects,
     * so be careful what you do with them.
     */
    AutoPtr<List<AutoPtr<ISyncStatusInfo> > > GetSyncStatus();

    /**
     * Return a copy of the specified target with the corresponding sync status
     */
    AutoPtr<AuthoritySyncStatusPair> GetCopyOfAuthorityWithSyncStatus(
        /* [in] */ EndPoint* info);

    /**
     * Return a copy of all authorities with their corresponding sync status
     */
    AutoPtr<List<AutoPtr<AuthoritySyncStatusPair> > > GetCopyOfAllAuthoritiesWithSyncStatus();

    /**
     * Returns the status that matches the target.
     *
     * @param info the endpoint target we are querying status info for.
     * @return the SyncStatusInfo for the endpoint.
     */
    AutoPtr<ISyncStatusInfo> GetStatusByAuthority(
        /* [in] */ EndPoint* info);

    /** Return true if the pending status is true of any matching authorities. */
    Boolean IsSyncPending(
        /* [in] */ EndPoint* info);

    /**
     * Return an array of the current sync status for all authorities.  Note
     * that the objects inside the array are the real, live status objects,
     * so be careful what you do with them.
     */
    AutoPtr<List<AutoPtr<SyncHistoryItem> > > GetSyncHistory();

    /**
     * Return an array of the current per-day statistics.  Note
     * that the objects inside the array are the real, live status objects,
     * so be careful what you do with them.
     */
    AutoPtr<ArrayOf<DayStats*> > GetDayStatistics();


    void RemoveAuthority(
        /* [in] */ EndPoint* info);

    /**
     * Updates (in a synchronized way) the periodic sync time of the specified
     * target id and target periodic sync
     */
    void SetPeriodicSyncTime(
        /* [in] */ Int32 authorityId,
        /* [in] */ IPeriodicSync* targetPeriodicSync,
        /* [in] */ Int64 when);

    void WriteAllState();

    /**
     * public for testing
     */
    void ClearAndReadState();

    static Int32 GetInt32Column(
        /* [in] */ ICursor* c,
        /* [in] */ const String& name);

    static Int64 GetInt64Column(
        /* [in] */ ICursor* c,
        /* [in] */ const String& name);

    /**
     * Dump state of PendingOperations.
     */
    void DumpPendingOperations(
        /* [in] */ StringBuilder* sb);

private:

    void ReportChange(
        /* [in] */ Int32 which);

    /**
     * An enabled sync service and a syncable provider's adapter both get resolved to the same
     * persisted variable - namely the "syncable" attribute for an AuthorityInfo in accounts.xml.
     * @param target target to set value for.
     * @param syncable 0 indicates unsyncable, <0 unknown, >0 is active/syncable.
     */
    void SetSyncableStateForEndPoint(
        /* [in] */ EndPoint* target,
        /* [in] */ Int32 syncable);

    /**
     * Either set backoff for a specific authority, or set backoff for all the
     * accounts on a specific adapter/all adapters.
     *
     * @param account account for which to set backoff. Null to specify all accounts.
     * @param userId id of the user making this request.
     * @param providerName provider for which to set backoff. Null to specify all providers.
     * @return true if a change occured.
     */
    Boolean SetBackoffLocked(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& providerName,
        /* [in] */ Int64 nextSyncTime,
        /* [in] */ Int64 nextDelay);

    /**
     * Return a list of the currently active syncs. Note that the returned
     * items are the real, live active sync objects, so be careful what you do
     * with it.
     */
    AutoPtr<List<AutoPtr<ISyncInfo> > > GetCurrentSyncs(
        /* [in] */ Int32 userId);

    AutoPtr<List<AutoPtr<ISyncInfo> > > GetCurrentSyncsLocked(
        /* [in] */ Int32 userId);

    AutoPtr<ISyncStatusInfo> GetOrCreateSyncStatusLocked(
        /* [in] */ Int32 authorityId);

    AutoPtr<AuthoritySyncStatusPair> CreateCopyPairOfAuthorityWithSyncStatusLocked(
        /* [in] */ AuthorityInfo* authorityInfo);

    Int32 GetCurrentDayLocked();

    /**
     * Retrieve a target's full info, returning null if one does not exist.
     *
     * @param info info of the target to look up.
     * @param tag If non-null, this will be used in a log message if the
     * requested target does not exist.
     */
    AutoPtr<AuthorityInfo> GetAuthorityLocked(
        /* [in] */ EndPoint* info,
        /* [in] */ const String& tag);

    /**
     * @param info info identifying target.
     * @param ident unique identifier for target. -1 for none.
     * @param doWrite if true, update the accounts.xml file on the disk.
     * @return the authority that corresponds to the provided sync target, creating it if none
     * exists.
     */
    AutoPtr<AuthorityInfo> GetOrCreateAuthorityLocked(
        /* [in] */ EndPoint* info,
        /* [in] */ Int32 ident,
        /* [in] */ Boolean doWrite);

    AutoPtr<AuthorityInfo> CreateAuthorityLocked(
        /* [in] */ EndPoint* info,
        /* [in] */ Int32 ident,
        /* [in] */ Boolean doWrite);

    /**
     * Remove an authority associated with a provider. Needs to be a standalone function for
     * backward compatibility.
     */
    void RemoveAuthorityLocked(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ const String& authorityName,
        /* [in] */ Boolean doWrite);

    /**
     * Read all account information back in to the initial engine state.
     */
    void ReadAccountInfoLocked();

    /**
     * Ensure the old pending.bin is deleted, as it has been changed to pending.xml.
     * pending.xml was used starting in KLP.
     * @param syncDir directory where the sync files are located.
     */
    void MaybeDeleteLegacyPendingInfoLocked(
        /* [in] */ IFile* syncDir);

    /**
     * some authority names have changed. copy over their settings and delete the old ones
     * @return true if a change was made
     */
    Boolean MaybeMigrateSettingsForRenamedAuthorities();

    void ParseListenForTickles(
        /* [in] */ IXmlPullParser* parser);

    AutoPtr<AuthorityInfo> ParseAuthority(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 version);

    /**
     * Parse a periodic sync from accounts.xml. Sets the bundle to be empty.
     */
    AutoPtr<IPeriodicSync> ParsePeriodicSync(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ AuthorityInfo* authorityInfo);

    void ParseExtra(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IBundle* extras);

    /**
     * Write all account information to the account file.
     */
    void WriteAccountInfoLocked();

    /**
     * Load sync engine state from the old syncmanager database, and then
     * erase it.  Note that we don't deal with pending operations, active
     * sync, or history.
     */
    void ReadAndDeleteLegacyAccountInfoLocked();

    /**
     * Read all sync status back in to the initial engine state.
     */
    void ReadStatusLocked();

    /**
     * Write all sync status to the sync status file.
     */
    void WriteStatusLocked();

    /** Read all pending operations back in to the initial engine state. */
    CARAPI ReadPendingOperationsLocked();

    /**
     * Write all currently pending ops to the pending ops file.
     */
    void WritePendingOperationsLocked();

    /** Write all currently pending ops to the pending ops file. */
    void WritePendingOperationLocked(
        /* [in] */ PendingOperation* pop,
        /* [in] */ IXmlSerializer* out);

    /**
     * Append the given operation to the pending ops file; if unable to,
     * write all pending ops.
     */
    void AppendPendingOperationLocked(
        /* [in] */ PendingOperation* op);

    void ExtrasToXml(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ IBundle* extras);

    void RequestSync(
        /* [in] */ AuthorityInfo* authorityInfo,
        /* [in] */ Int32 reason,
        /* [in] */ IBundle* extras);

    void RequestSync(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    /**
     * Read all sync statistics back in to the initial engine state.
     */
    void ReadStatisticsLocked();

    /**
     * Write all sync statistics to the sync status file.
     */
    void WriteStatisticsLocked();

    static AutoPtr< ArrayOf<Byte> > FlattenBundle(
        /* [in] */ IBundle* bundle);

    static AutoPtr<IBundle> UnFlattenBundle(
        /* [in] */ ArrayOf<Byte>* flatData);

public:

    // @VisibleForTesting
    static const Int64 MILLIS_IN_4WEEKS;

    /** Enum value for a sync start event. */
    static const Int32 EVENT_START;

    /** Enum value for a sync stop event. */
    static const Int32 EVENT_STOP;

    // TODO: i18n -- grab these out of resources.
    /** String names for the sync event types. */
    static const AutoPtr< ArrayOf<String> > EVENTS;

    /** Enum value for a server-initiated sync. */
    static const Int32 SOURCE_SERVER;

    /** Enum value for a local-initiated sync. */
    static const Int32 SOURCE_LOCAL;
    /** Enum value for a poll-based sync (e.g., upon connection to network) */
    static const Int32 SOURCE_POLL;

    /** Enum value for a user-initiated sync. */
    static const Int32 SOURCE_USER;

    /** Enum value for a periodic sync. */
    static const Int32 SOURCE_PERIODIC;

    /** Enum value for a sync started for a service. */
    static const Int32 SOURCE_SERVICE;

    static const Int64 NOT_IN_BACKOFF_MODE;

    // TODO: i18n -- grab these out of resources.
    /** String names for the sync source types. */
    static const AutoPtr< ArrayOf<String> > SOURCES;

    // The MESG column will contain one of these or one of the Error types.
    static const String MESG_SUCCESS;
    static const String MESG_CANCELED;

    static const Int32 MAX_HISTORY;

    static const Int32 STATUS_FILE_END;
    static const Int32 STATUS_FILE_ITEM;
    static const Int32 PENDING_OPERATION_VERSION;

    static const Int32 STATISTICS_FILE_END;
    static const Int32 STATISTICS_FILE_ITEM_OLD;
    static const Int32 STATISTICS_FILE_ITEM;

    static const Int32 PENDING_FINISH_TO_WRITE;

private:
    friend class AuthorityInfo;

    static const String XML_ATTR_VERSION;
    static const String XML_ATTR_AUTHORITYID;
    static const String XML_ATTR_SOURCE;
    static const String XML_ATTR_EXPEDITED;
    static const String XML_ATTR_REASON;

    static const String TAG;
    static const String TAG_FILE;

    static const String XML_ATTR_NEXT_AUTHORITY_ID;
    static const String XML_ATTR_LISTEN_FOR_TICKLES;
    static const String XML_ATTR_SYNC_RANDOM_OFFSET;
    static const String XML_ATTR_ENABLED;
    static const String XML_ATTR_USER;
    static const String XML_TAG_LISTEN_FOR_TICKLES;

    /** Default time for a periodic sync. */
    static const Int64 DEFAULT_POLL_FREQUENCY_SECONDS; // One day

    /** Percentage of period that is flex by default, if no flex is set. */
    static const Double DEFAULT_FLEX_PERCENT_SYNC;

    /** Lower bound on sync time from which we assign a default flex time. */
    static const Int64 DEFAULT_MIN_FLEX_ALLOWED_SECS;


    static const Int32 MSG_WRITE_STATUS;
    static const Int64 WRITE_STATUS_DELAY; // 10 minutes

    static const Int32 MSG_WRITE_STATISTICS;
    static const Int64 WRITE_STATISTICS_DELAY; // 1/2 hour

    static const Boolean SYNC_ENABLED_DEFAULT;

    // the version of the accounts xml file format
    static const Int32 ACCOUNTS_VERSION;

    static AutoPtr<HashMap<String, String> > sAuthorityRenames;

    // Primary list of all syncable authorities.  Also our global lock.
    HashMap<Int32, AutoPtr<AuthorityInfo> > mAuthorities;
    Object mAuthoritiesLock;

    HashMap< AutoPtr<IAccountAndUser>, AutoPtr<AccountInfo> > mAccounts;

    List<AutoPtr<PendingOperation> > mPendingOperations;

    HashMap<Int32, AutoPtr<List<AutoPtr<ISyncInfo> > > > mCurrentSyncs;

    HashMap<Int32, AutoPtr<ISyncStatusInfo> > mSyncStatus;

    List< AutoPtr<SyncHistoryItem> > mSyncHistory;

    AutoPtr<IRemoteCallbackList> mChangeListeners;

    /** Reverse mapping for component name -> <userid -> target id>. */
    ServiceMap mServices; //new ArrayMap<ComponentName, SparseArray<AuthorityInfo>>();

    Int32 mNextAuthorityId;

    // We keep 4 weeks of stats.
    AutoPtr<ArrayOf<DayStats*> > mDayStats;// = new DayStats[7*4];
    AutoPtr<ICalendar> mCal;
    Int32 mYear;
    Int32 mYearInDays;

    AutoPtr<IContext> mContext;

    static /*volatile*/ AutoPtr<SyncStorageEngine> sSyncStorageEngine;

    Int32 mSyncRandomOffset;

    /**
     * This file contains the core engine state: all accounts and the
     * settings for them.  It must never be lost, and should be changed
     * infrequently, so it is stored as an XML file.
     */
    AutoPtr<IAtomicFile> mAccountInfoFile;

    /**
     * This file contains the current sync status.  We would like to retain
     * it across boots, but its loss is not the end of the world, so we store
     * this information as binary data.
     */
    AutoPtr<IAtomicFile> mStatusFile;

    /**
     * This file contains sync statistics.  This is purely debugging information
     * so is written infrequently and can be thrown away at any time.
     */
    AutoPtr<IAtomicFile> mStatisticsFile;

    /**
     * This file contains the pending sync operations.  It is a binary file,
     * which must be updated every time an operation is added or removed,
     * so we have special handling of it.
     */
    AutoPtr<IAtomicFile> mPendingFile;
    Int32 mNumPendingFinished;

    Int32 mNextHistoryId;
    HashMap<Int32, Boolean>  mMasterSyncAutomatically;
    Boolean mDefaultMasterSyncAutomatically;

    AutoPtr<IOnSyncRequestListener> mSyncRequestListener;
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_SYNC_STORAGE_ENGINE_H__
