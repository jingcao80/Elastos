
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONUSAGESTATS_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONUSAGESTATS_H__

#include <_Elastos.Droid.Server.h>
#include "elastos/droid/server/notification/ManagedServices.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Os::Handler;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class NotificationUsageStats;

/**
 * Tracks usage of an individual notification that is currently active.
 */
class SingleNotificationStats
    : public Object
{
    friend class AggregatedStats;
public:
    SingleNotificationStats();

    ~SingleNotificationStats();

    CARAPI_(Int64) GetCurrentPosttimeMs();

    CARAPI_(Int64) GetCurrentAirtimeMs();

    CARAPI_(Int64) GetCurrentAirtimeExpandedMs();

    /**
     * Called when the user clicked the notification.
     */
    CARAPI_(void) OnClick();

    /**
     * Called when the user removed the notification.
     */
    CARAPI_(void) OnDismiss();

    CARAPI_(void) OnCancel();

    CARAPI_(void) OnRemoved();

    CARAPI_(void) OnVisibilityChanged(
        /* [in] */ Boolean visible);

    CARAPI_(void) OnExpansionChanged(
        /* [in] */ Boolean userAction,
        /* [in] */ Boolean expanded);

    /** The notification is leaving the system. Finalize. */
    CARAPI_(void) Finish();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) UpdateVisiblyExpandedStats();

public:
    /** SystemClock.elapsedRealtime() when the notification was posted. */
    Int64 mPosttimeElapsedMs;
    /** Elapsed time since the notification was posted until it was first clicked, or -1. */
    Int64 mPosttimeToFirstClickMs;
    /** Elpased time since the notification was posted until it was dismissed by the user. */
    Int64 mPosttimeToDismissMs;
    /** Number of times the notification has been made visible. */
    Int64 mAirtimeCount;
    /** Time in ms between the notification was posted and first shown; -1 if never shown. */
    Int64 mPosttimeToFirstAirtimeMs;
    /**
     * If currently visible, SystemClock.elapsedRealtime() when the notification was made
     * visible; -1 otherwise.
     */
    Int64 mCurrentAirtimeStartElapsedMs;
    /** Accumulated visible time. */
    Int64 mAirtimeMs;
    /**
     * Time in ms between the notification being posted and when it first
     * became visible and expanded; -1 if it was never visibly expanded.
     */
    Int64 mPosttimeToFirstVisibleExpansionMs;
    /**
     * If currently visible, SystemClock.elapsedRealtime() when the notification was made
     * visible; -1 otherwise.
     */
    Int64 mCurrentAirtimeExpandedStartElapsedMs;
    /** Accumulated visible expanded time. */
    Int64 mAirtimeExpandedMs;
    /** Number of times the notification has been expanded by the user. */
    Int64 mUserExpansionCount;

private:
    Boolean mIsVisible;
    Boolean mIsExpanded;
};

/**
 * Aggregates long samples to sum and averages.
 */
class AggregateSamples
    : public Object
{
    friend class AggregatedStats;
public:
    AggregateSamples();

    ~AggregateSamples();

    CARAPI_(void) AddSample(
        /* [in] */ Int64 sample);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

protected:
    Int64 mNumSamples;
    Double mAvg;
    Double mSum2;
    Double mVar;
};

/**
 * Aggregated notification stats.
 */
class AggregatedStats
    : public Object
{
    friend class NotificationUsageStats;
public:
    AggregatedStats(
        /* [in] */ const String& key);

    ~AggregatedStats();

    CARAPI_(void) Collect(
        /* [in] */ SingleNotificationStats* singleNotificationStats);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& indent);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(String) ToStringWithIndent(
        /* [in] */ const String& indent);

public:
    String mKey;

    // ---- Updated as the respective events occur.
    Int32 mNumPostedByApp;
    Int32 mNumUpdatedByApp;
    Int32 mNumRemovedByApp;
    Int32 mNumClickedByUser;
    Int32 mNumDismissedByUser;

    // ----  Updated when a notification is canceled.
    AutoPtr<AggregateSamples> mPosttimeMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mPosttimeToDismissMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mPosttimeToFirstClickMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mAirtimeCount;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mAirtimeMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mPosttimeToFirstAirtimeMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mUserExpansionCount;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mAirtimeExpandedMs;// = new AggregateSamples();
    AutoPtr<AggregateSamples> mPosttimeToFirstVisibleExpansionMs;// = new AggregateSamples();
};

} // Notification
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Notification::AggregatedStats, IInterface);

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * Keeps track of notification activity, display, and user interaction.
 *
 * <p>This class receives signals from NoMan and keeps running stats of
 * notification usage. Some metrics are updated as events occur. Others, namely
 * those involving durations, are updated as the notification is canceled.</p>
 *
 * <p>This class is thread-safe.</p>
 *
 * {@hide}
 */
class NotificationRecord;

class NotificationUsageStats
    : public Object
{
    friend class AggregatedStats;
private:
    class SQLiteLog
        : public Object
    {
        friend class SQLiteLogHandler;
    public:
        SQLiteLog(
            /* [in] */ IContext* context);

        ~SQLiteLog();

        CARAPI_(void) LogPosted(
            /* [in] */ NotificationRecord* notification);

        CARAPI_(void) LogClicked(
            /* [in] */ NotificationRecord* notification);

        CARAPI_(void) LogRemoved(
            /* [in] */ NotificationRecord* notification);

        CARAPI_(void) LogDismissed(
            /* [in] */ NotificationRecord* notification);

        CARAPI_(void) PrintPostFrequencies(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& indent,
            /* [in] */ DumpFilter* filter);

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& indent,
            /* [in] */ DumpFilter* filter);

        CARAPI_(void) WriteEvent(
            /* [in] */ Int64 eventTimeMs,
            /* [in] */ Int32 eventType,
            /* [in] */ NotificationRecord* r);

    private:
        CARAPI_(void) PruneIfNecessary(
            /* [in] */ ISQLiteDatabase* db);

        static CARAPI_(void) PutNotificationIdentifiers(
            /* [in] */ NotificationRecord* r,
            /* [in] */ IContentValues* outCv);

        static CARAPI_(void) PutNotificationDetails(
            /* [in] */ NotificationRecord* r,
            /* [in] */ IContentValues* outCv);

        static CARAPI_(void) PutPosttimeVisibility(
            /* [in] */ NotificationRecord* r,
            /* [in] */ IContentValues* outCv);

    public:
        static const String TAG;// = "NotificationSQLiteLog";

        // Message types passed to the background handler.
        static const Int32 MSG_POST = 1;
        static const Int32 MSG_CLICK = 2;
        static const Int32 MSG_REMOVE = 3;
        static const Int32 MSG_DISMISS = 4;

        static const String DB_NAME;// = "notification_log.db";
        static const Int32 DB_VERSION;

        /** Age in ms after which events are pruned from the DB. */
        static const Int64 HORIZON_MS;// = 7 * 24 * 60 * 60 * 1000L;  // 1 week
        /** Delay between pruning the DB. Used to throttle pruning. */
        static const Int64 PRUNE_MIN_DELAY_MS;// = 6 * 60 * 60 * 1000L;  // 6 hours
        /** Mininum number of writes between pruning the DB. Used to throttle pruning. */
        static const Int64 PRUNE_MIN_WRITES;// = 1024;

        // Table 'log'
        static const String TAB_LOG;
        static const String COL_EVENT_USER_ID;
        static const String COL_EVENT_TYPE;
        static const String COL_EVENT_TIME;
        static const String COL_KEY;
        static const String COL_PKG;
        static const String COL_NOTIFICATION_ID;
        static const String COL_TAG;
        static const String COL_WHEN_MS;
        static const String COL_DEFAULTS;
        static const String COL_FLAGS;
        static const String COL_PRIORITY;
        static const String COL_CATEGORY;
        static const String COL_ACTION_COUNT;
        static const String COL_POSTTIME_MS;
        static const String COL_AIRTIME_MS;
        static const String COL_FIRST_EXPANSIONTIME_MS;
        static const String COL_AIRTIME_EXPANDED_MS;
        static const String COL_EXPAND_COUNT;

        static const Int32 EVENT_TYPE_POST;
        static const Int32 EVENT_TYPE_CLICK;
        static const Int32 EVENT_TYPE_REMOVE;
        static const Int32 EVENT_TYPE_DISMISS;

        static Int64 sLastPruneMs;
        static Int64 sNumWrites;

        AutoPtr<ISQLiteOpenHelper> mHelper;
        AutoPtr<IHandler> mWriteHandler;

        static const Int64 DAY_MS;
    };

    class SQLiteLogHandler
        : public Handler
    {
    public:
        SQLiteLogHandler(
            /* [in] */ SQLiteLog* host);

        ~SQLiteLogHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        SQLiteLog* mHost;
        NotificationUsageStats* mMainHost;
    };

    class SQLiteLogSQLiteOpenHelper
        : public SQLiteOpenHelper
    {
    public:
        SQLiteLogSQLiteOpenHelper(
            /* [in] */ SQLiteLog* host);

        ~SQLiteLogSQLiteOpenHelper();

        // @Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        // @Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    private:
        SQLiteLog* mHost;
        NotificationUsageStats* mMainHost;
    };

public:
    NotificationUsageStats(
        /* [in] */ IContext* context);

    ~NotificationUsageStats();

    /**
     * Called when a notification has been posted.
     */
    CARAPI_(void) RegisterPostedByApp(
        /* [in] */ NotificationRecord* notification);

    /**
     * Called when a notification has been updated.
     */
    CARAPI_(void) RegisterUpdatedByApp(
        /* [in] */ NotificationRecord* notification,
        /* [in] */ NotificationRecord* old);

    /**
     * Called when the originating app removed the notification programmatically.
     */
    CARAPI_(void) RegisterRemovedByApp(
        /* [in] */ NotificationRecord* notification);

    /**
     * Called when the user dismissed the notification via the UI.
     */
    CARAPI_(void) RegisterDismissedByUser(
        /* [in] */ NotificationRecord* notification);

    /**
     * Called when the user clicked the notification in the UI.
     */
    CARAPI_(void) RegisterClickedByUser(
        /* [in] */ NotificationRecord* notification);

    /**
     * Called when the notification is canceled because the user clicked it.
     *
     * <p>Called after {@link #registerClickedByUser(NotificationRecord)}.</p>
     */
    CARAPI_(void) RegisterCancelDueToClick(
        /* [in] */ NotificationRecord* notification);

    /**
     * Called when the notification is canceled due to unknown reasons.
     *
     * <p>Called for notifications of apps being uninstalled, for example.</p>
     */
    CARAPI_(void) RegisterCancelUnknown(
        /* [in] */ NotificationRecord* notification);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& indent,
        /* [in] */ DumpFilter* filter);

private:
    // Locked by this.
    CARAPI_(AutoPtr< ArrayOf<AggregatedStats*> >) GetAggregatedStatsLocked(
        /* [in] */ NotificationRecord* record);

    // Locked by this.
    CARAPI_(AutoPtr<AggregatedStats>) GetOrCreateAggregatedStatsLocked(
        /* [in] */ const String& key);

    static AutoPtr< ArrayOf<AggregatedStats*> > InitEMPTY_AGGREGATED_STATS();

private:
    // WARNING: Aggregated stats can grow unboundedly with pkg+id+tag.
    // Don't enable on production builds.
    static const Boolean ENABLE_AGGREGATED_IN_MEMORY_STATS;
    static const Boolean ENABLE_SQLITE_LOG;

    static const AutoPtr< ArrayOf<AggregatedStats*> > EMPTY_AGGREGATED_STATS;

    // Guarded by synchronized(this).
    // private final Map<String, AggregatedStats> mStats = new HashMap<String, AggregatedStats>();
    AutoPtr<IMap> mStats;
    AutoPtr<SQLiteLog> mSQLiteLog;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONUSAGESTATS_H__
