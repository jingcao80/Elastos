
#ifndef __ELASTOS_DROID_CONTENT_CSYNCREQUESTBUILDER_H__
#define __ELASTOS_DROID_CONTENT_CSYNCREQUESTBUILDER_H__

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Content_CSyncRequestBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Content {

class CSyncRequest;

/**
 * Builder class for a @link SyncRequest. As you build your SyncRequest this class will also
 * perform validation.
 */
CarClass(CSyncRequestBuilder)
    , public Object
    , public ISyncRequestBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncRequestBuilder();

    CARAPI constructor();

    /**
     * Request that a sync occur immediately.
     *
     * Example
     * <pre>
     *     SyncRequest.Builder builder = (new SyncRequest.Builder()).syncOnce();
     * </pre>
     */
    CARAPI SyncOnce();

    /**
     * Build a periodic sync. Either this or syncOnce() <b>must</b> be called for this builder.
     * Syncs are identified by target {@link android.provider} and by the
     * contents of the extras bundle.
     * You cannot reuse the same builder for one-time syncs after having specified a periodic
     * sync (by calling this function). If you do, an <code>IllegalArgumentException</code>
     * will be thrown.
     * <p>The bundle for a periodic sync can be queried by applications with the correct
     * permissions using
     * {@link ContentResolver#getPeriodicSyncs(Account account, String provider)}, so no
     * sensitive data should be transferred here.
     *
     * Example usage.
     *
     * <pre>
     *     Request a periodic sync every 5 hours with 20 minutes of flex.
     *     SyncRequest.Builder builder =
     *         (new SyncRequest.Builder()).syncPeriodic(5 * HOUR_IN_SECS, 20 * MIN_IN_SECS);
     *
     *     Schedule a periodic sync every hour at any point in time during that hour.
     *     SyncRequest.Builder builder =
     *         (new SyncRequest.Builder()).syncPeriodic(1 * HOUR_IN_SECS, 1 * HOUR_IN_SECS);
     * </pre>
     *
     * N.B.: Periodic syncs are not allowed to have any of
     * {@link ContentResolver#SYNC_EXTRAS_DO_NOT_RETRY},
     * {@link ContentResolver#SYNC_EXTRAS_IGNORE_BACKOFF},
     * {@link ContentResolver#SYNC_EXTRAS_IGNORE_SETTINGS},
     * {@link ContentResolver#SYNC_EXTRAS_INITIALIZE},
     * {@link ContentResolver#SYNC_EXTRAS_FORCE},
     * {@link ContentResolver#SYNC_EXTRAS_EXPEDITED},
     * {@link ContentResolver#SYNC_EXTRAS_MANUAL}
     * set to true. If any are supplied then an <code>IllegalArgumentException</code> will
     * be thrown.
     *
     * @param pollFrequency the amount of time in seconds that you wish
     *            to elapse between periodic syncs.
     * @param beforeSeconds the amount of flex time in seconds before
     *            {@code pollFrequency} that you permit for the sync to take
     *            place. Must be less than {@code pollFrequency}.
     */
    CARAPI SyncPeriodic(
        /* [in] */ Int64 pollFrequency,
        /* [in] */ Int64 beforeSeconds);

    /**
     * Will throw an <code>IllegalArgumentException</code> if called and
     * {@link #setIgnoreSettings(boolean ignoreSettings)} has already been called.
     * @param disallow true to allow this transfer on metered networks. Default false.
     *
     */
    CARAPI SetDisallowMetered(
        /* [in] */ Boolean disallow);

    /**
     * Specify an authority and account for this transfer.
     *
     * @param authority A String identifying the content provider to be synced.
     * @param account Account to sync. Can be null unless this is a periodic
     *            sync, for which verification by the ContentResolver will
     *            fail. If a sync is performed without an account, the
     */
    CARAPI SetSyncAdapter(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority);

    /**
     * Developer-provided extras handed back when sync actually occurs. This bundle is copied
     * into the SyncRequest returned by {@link #build()}.
     *
     * Example:
     * <pre>
     *   String[] syncItems = {"dog", "cat", "frog", "child"};
     *   SyncRequest.Builder builder =
     *     new SyncRequest.Builder()
     *       .setSyncAdapter(dummyAccount, dummyProvider)
     *       .syncOnce();
     *
     *   for (String syncData : syncItems) {
     *     Bundle extras = new Bundle();
     *     extras.setString("data", syncData);
     *     builder.setExtras(extras);
     *     ContentResolver.sync(builder.build()); // Each sync() request creates a unique sync.
     *   }
     * </pre>
     * Only values of the following types may be used in the extras bundle:
     * <ul>
     * <li>Integer</li>
     * <li>Long</li>
     * <li>Boolean</li>
     * <li>Float</li>
     * <li>Double</li>
     * <li>String</li>
     * <li>Account</li>
     * <li>null</li>
     * </ul>
     * If any data is present in the bundle not of this type, build() will
     * throw a runtime exception.
     *
     * @param bundle extras bundle to set.
     */
    CARAPI SetExtras(
        /* [in] */ IBundle* bundle);

    /**
     * Convenience function for setting {@link ContentResolver#SYNC_EXTRAS_DO_NOT_RETRY}.
     *
     * A one-off sync operation that fails will be retried with exponential back-off unless
     * this is set to false. Not valid for periodic sync and will throw an
     * <code>IllegalArgumentException</code> in build().
     *
     * @param noRetry true to not retry a failed sync. Default false.
     */
    CARAPI SetNoRetry(
        /* [in] */ Boolean noRetry);

    /**
     * Convenience function for setting {@link ContentResolver#SYNC_EXTRAS_IGNORE_SETTINGS}.
     *
     * Not valid for periodic sync and will throw an <code>IllegalArgumentException</code> in
     * {@link #build()}.
     * <p>Throws <code>IllegalArgumentException</code> if called and
     * {@link #setDisallowMetered(boolean)} has been set.
     *
     *
     * @param ignoreSettings true to ignore the sync automatically settings. Default false.
     */
    CARAPI SetIgnoreSettings(
        /* [in] */ Boolean ignoreSettings);

    /**
     * Convenience function for setting {@link ContentResolver#SYNC_EXTRAS_IGNORE_BACKOFF}.
     *
     * Ignoring back-off will force the sync scheduling process to ignore any back-off that was
     * the result of a failed sync, as well as to invalidate any {@link SyncResult#delayUntil}
     * value that may have been set by the adapter. Successive failures will not honor this
     * flag. Not valid for periodic sync and will throw an <code>IllegalArgumentException</code>
     * in {@link #build()}.
     *
     * @param ignoreBackoff ignore back off settings. Default false.
     */
    CARAPI SetIgnoreBackoff(
        /* [in] */ Boolean ignoreBackoff);

    /**
     * Convenience function for setting {@link ContentResolver#SYNC_EXTRAS_MANUAL}.
     *
     * Not valid for periodic sync and will throw an <code>IllegalArgumentException</code> in
     * {@link #build()}.
     *
     * @param isManual User-initiated sync or not. Default false.
     */
    CARAPI SetManual(
        /* [in] */ Boolean isManual);

    /**
     * An expedited sync runs immediately and can preempt other non-expedited running syncs.
     *
     * Not valid for periodic sync and will throw an <code>IllegalArgumentException</code> in
     * {@link #build()}.
     *
     * @param expedited whether to run expedited. Default false.
     */
    CARAPI SetExpedited(
        /* [in] */ Boolean expedited);

    /**
     * Performs validation over the request and throws the runtime exception
     * <code>IllegalArgumentException</code> if this validation fails.
     *
     * @return a SyncRequest with the information contained within this
     *         builder.
     */
    CARAPI Build(
        /* [out] */ ISyncRequest** request);

private:
    CARAPI SetupInterval(
        /* [in] */ Int64 at,
        /* [in] */ Int64 before);

private:
    friend class CSyncRequest;

    /** Unknown sync type. */
    static const Int32 SYNC_TYPE_UNKNOWN;// = 0;
    /** Specify that this is a periodic sync. */
    static const Int32 SYNC_TYPE_PERIODIC;// = 1;
    /** Specify that this is a one-time sync. */
    static const Int32 SYNC_TYPE_ONCE;// = 2;
    /** Unknown sync target. */
    static const Int32 SYNC_TARGET_UNKNOWN;// = 0;
    /** Specify that this is a sync with a provider. */
    static const Int32 SYNC_TARGET_ADAPTER;// = 2;
    /**
     * Earliest point of displacement into the future at which this sync can
     * occur.
     */
    Int64 mSyncFlexTimeSecs;
    /** Displacement into the future at which this sync must occur. */
    Int64 mSyncRunTimeSecs;
    /**
     * Sync configuration information - custom user data explicitly provided by the developer.
     * This data is handed over to the sync operation.
     */
    AutoPtr<IBundle> mCustomExtras;
    /**
     * Sync system configuration -  used to store system sync configuration. Corresponds to
     * ContentResolver.SYNC_EXTRAS_* flags.
     * TODO: Use this instead of dumping into one bundle. Need to decide if these flags should
     * discriminate between equivalent syncs.
     */
    AutoPtr<IBundle> mSyncConfigExtras;
    /** Whether or not this sync can occur on metered networks. Default false. */
    Boolean mDisallowMetered;
    /**
     * Whether this builder is building a periodic sync, or a one-time sync.
     */
    Int32 mSyncType;// = SYNC_TYPE_UNKNOWN;
    /** Whether this will go to a sync adapter. */
    Int32 mSyncTarget;// = SYNC_TARGET_UNKNOWN;
    /** Whether this is a user-activated sync. */
    Boolean mIsManual;
    /**
     * Whether to retry this one-time sync if the sync fails. Not valid for
     * periodic syncs. See {@link ContentResolver#SYNC_EXTRAS_DO_NOT_RETRY}.
     */
    Boolean mNoRetry;
    /**
     * Whether to respect back-off for this one-time sync. Not valid for
     * periodic syncs. See
     * {@link ContentResolver#SYNC_EXTRAS_IGNORE_BACKOFF};
     */
    Boolean mIgnoreBackoff;

    /** Ignore sync system settings and perform sync anyway. */
    Boolean mIgnoreSettings;

    /** This sync will run in preference to other non-expedited syncs. */
    Boolean mExpedited;

    /**
     * The Account object that together with an Authority name define the SyncAdapter (if
     * this sync is bound to a provider), otherwise null.
     */
    AutoPtr<IAccount> mAccount;
    /**
     * The Authority name that together with an Account define the SyncAdapter (if
     * this sync is bound to a provider), otherwise null.
     */
    String mAuthority;
};


}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCREQUESTBUILDER_H__
