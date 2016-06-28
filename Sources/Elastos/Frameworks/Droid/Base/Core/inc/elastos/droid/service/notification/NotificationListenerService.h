#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONLISTENERERVICE_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONLISTENERERVICE_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

/**
 * A service that receives calls from the system when new notifications are
 * posted or removed, or their ranking changed.
 * <p>To extend this class, you must declare the service in your manifest file with
 * the {@link android.Manifest.permission#BIND_NOTIFICATION_LISTENER_SERVICE} permission
 * and include an intent filter with the {@link #SERVICE_INTERFACE} action. For example:</p>
 * <pre>
 * &lt;service android:name=".NotificationListener"
 *          android:label="&#64;string/service_name"
 *          android:permission="android.permission.BIND_NOTIFICATION_LISTENER_SERVICE">
 *     &lt;intent-filter>
 *         &lt;action android:name="android.service.notification.NotificationListenerService" />
 *     &lt;/intent-filter>
 * &lt;/service></pre>
 */
class ECO_PUBLIC NotificationListenerService
    : public Elastos::Droid::App::Service
    , public INotificationListenerService
{
public:
    /**
     * Provides access to ranking information on currently active
     * notifications.
     *
     * <p>
     * Note that this object represents a ranking snapshot that only applies to
     * notifications active at the time of retrieval.
     */
    class RankingMap
        : public Object
        , public IParcelable
        , public INotificationListenerServiceRankingMap
    {
    public:
        CAR_INTERFACE_DECL()

        RankingMap();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

    // private:
        CARAPI constructor(
            /* [in] */ INotificationRankingUpdate* rankingUpdate);

        /**
         * Request the list of notification keys in their current ranking
         * order.
         *
         * @return An array of active notification keys, in their ranking order.
         */
        CARAPI GetOrderedKeys(
            /* [out, callee] */ ArrayOf<String>** keys);

        /**
         * Populates outRanking with ranking information for the notification
         * with the given key.
         *
         * @return true if a valid key has been passed and outRanking has
         *     been populated; false otherwise
         */
        CARAPI GetRanking(
            /* [in] */ const String& key,
            /* [in] */ INotificationListenerServiceRanking* outRanking,
            /* [out] */ Boolean* ranking);

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    private:
        CARAPI_(Int32) GetRank(
            /* [in] */ const String& key);

        CARAPI_(Boolean) IsAmbient(
            /* [in] */ const String& key);

        CARAPI_(Boolean) IsIntercepted(
            /* [in] */ const String& key);

        CARAPI_(Int32) GetVisibilityOverride(
            /* [in] */ const String& key);

        // Locked by 'this'
        CARAPI_(void) BuildRanksLocked();

        // Locked by 'this'
        CARAPI_(void) BuildInterceptedSetLocked();

        // Locked by 'this'
        CARAPI_(void) BuildVisibilityOverridesLocked();

    private:
        AutoPtr<INotificationRankingUpdate> mRankingUpdate;
        AutoPtr<IArrayMap> mRanks;
        AutoPtr<IArraySet> mIntercepted;
        AutoPtr<IArrayMap> mVisibilityOverrides;
    };

    /**
     * Stores ranking related information on a currently active notification.
     *
     * <p>
     * Ranking objects aren't automatically updated as notification events
     * occur. Instead, ranking information has to be retrieved again via the
     * current {@link RankingMap}.
     */
    class Ranking
        : public Object
        , public INotificationListenerServiceRanking
    {
        friend class RankingMap;
    public:
        CAR_INTERFACE_DECL()

        Ranking();

        CARAPI constructor();

        /**
         * Returns the key of the notification this Ranking applies to.
         */
        CARAPI GetKey(
            /* [out] */ String* key);

        /**
         * Returns the rank of the notification.
         *
         * @return the rank of the notification, that is the 0-based index in
         *     the list of active notifications.
         */
        CARAPI GetRank(
            /* [out] */ Int32* rank);

        /**
         * Returns whether the notification is an ambient notification, that is
         * a notification that doesn't require the user's immediate attention.
         */
        CARAPI IsAmbient(
            /* [out] */ Boolean* isAmbient);

        /**
         * Returns the user specificed visibility for the package that posted
         * this notification, or
         * {@link NotificationListenerService.Ranking#VISIBILITY_NO_OVERRIDE} if
         * no such preference has been expressed.
         * @hide
         */
        CARAPI GetVisibilityOverride(
            /* [out] */ Int32* vo);

        /**
         * Returns whether the notification matches the user's interruption
         * filter.
         *
         * @return {@code true} if the notification is allowed by the filter, or
         * {@code false} if it is blocked.
         */
        CARAPI MatchesInterruptionFilter(
            /* [out] */ Boolean* isMatches);

    private:
        CARAPI_(void) Populate(
            /* [in] */ const String& key,
            /* [in] */ Int32 rank,
            /* [in] */ Boolean isAmbient,
            /* [in] */ Boolean matchesInterruptionFilter,
            /* [in] */ Int32 visibilityOverride);

    private:
        String mKey;
        Int32 mRank;
        Boolean mIsAmbient;
        Boolean mMatchesInterruptionFilter;
        Int32 mVisibilityOverride;
    };

    class ECO_LOCAL INotificationListenerWrapper
        : public Object
        , public IINotificationListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ INotificationListenerService* host);

        // @Override
        CARAPI OnNotificationPosted(
            /* [in] */ IIStatusBarNotificationHolder* sbnHolder,
            /* [in] */ INotificationRankingUpdate* update);

        // @Override
        CARAPI OnNotificationRemoved(
            /* [in] */ IIStatusBarNotificationHolder* sbnHolder,
            /* [in] */ INotificationRankingUpdate* update);

        // @Override
        CARAPI OnListenerConnected(
            /* [in] */ INotificationRankingUpdate* update);

        // @Override
        CARAPI OnNotificationRankingUpdate(
            /* [in] */ INotificationRankingUpdate* update);

        // @Override
        CARAPI OnListenerHintsChanged(
            /* [in] */ Int32 hints);

        // @Override
        CARAPI OnInterruptionFilterChanged(
            /* [in] */ Int32 interruptionFilter);

        TO_STRING_IMPL("INotificationListenerWrapper")

    private:
        NotificationListenerService* mHost;
    };

    friend class INotificationListenerWrapper;
public:
    CAR_INTERFACE_DECL()

    NotificationListenerService();

    CARAPI constructor();

    /**
     * Implement this method to learn about new notifications as they are posted by apps.
     *
     * @param sbn A data structure encapsulating the original {@link android.app.Notification}
     *            object as well as its identifying information (tag and id) and source
     *            (package name).
     */
    CARAPI OnNotificationPosted(
        /* [in] */ IStatusBarNotification* sbn);

    /**
     * Implement this method to learn about new notifications as they are posted by apps.
     *
     * @param sbn A data structure encapsulating the original {@link android.app.Notification}
     *            object as well as its identifying information (tag and id) and source
     *            (package name).
     * @param rankingMap The current ranking map that can be used to retrieve ranking information
     *                   for active notifications, including the newly posted one.
     */
    CARAPI OnNotificationPosted(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

    /**
     * Implement this method to learn when notifications are removed.
     * <P>
     * This might occur because the user has dismissed the notification using system UI (or another
     * notification listener) or because the app has withdrawn the notification.
     * <P>
     * NOTE: The {@link StatusBarNotification} object you receive will be "light"; that is, the
     * result from {@link StatusBarNotification#getNotification} may be missing some heavyweight
     * fields such as {@link android.app.Notification#contentView} and
     * {@link android.app.Notification#largeIcon}. However, all other fields on
     * {@link StatusBarNotification}, sufficient to match this call with a prior call to
     * {@link #onNotificationPosted(StatusBarNotification)}, will be intact.
     *
     * @param sbn A data structure encapsulating at least the original information (tag and id)
     *            and source (package name) used to post the {@link android.app.Notification} that
     *            was just removed.
     */
    CARAPI OnNotificationRemoved(
        /* [in] */ IStatusBarNotification* sbn);

    /**
     * Implement this method to learn when notifications are removed.
     * <P>
     * This might occur because the user has dismissed the notification using system UI (or another
     * notification listener) or because the app has withdrawn the notification.
     * <P>
     * NOTE: The {@link StatusBarNotification} object you receive will be "light"; that is, the
     * result from {@link StatusBarNotification#getNotification} may be missing some heavyweight
     * fields such as {@link android.app.Notification#contentView} and
     * {@link android.app.Notification#largeIcon}. However, all other fields on
     * {@link StatusBarNotification}, sufficient to match this call with a prior call to
     * {@link #onNotificationPosted(StatusBarNotification)}, will be intact.
     *
     * @param sbn A data structure encapsulating at least the original information (tag and id)
     *            and source (package name) used to post the {@link android.app.Notification} that
     *            was just removed.
     * @param rankingMap The current ranking map that can be used to retrieve ranking information
     *                   for active notifications.
     *
     */
    CARAPI OnNotificationRemoved(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

    /**
     * Implement this method to learn about when the listener is enabled and connected to
     * the notification manager.  You are safe to call {@link #getActiveNotifications()}
     * at this time.
     */
    CARAPI OnListenerConnected();

    /**
     * Implement this method to be notified when the notification ranking changes.
     *
     * @param rankingMap The current ranking map that can be used to retrieve ranking information
     *                   for active notifications.
     */
    CARAPI OnNotificationRankingUpdate(
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

    /**
     * Implement this method to be notified when the
     * {@link #getCurrentListenerHints() Listener hints} change.
     *
     * @param hints The current {@link #getCurrentListenerHints() listener hints}.
     */
    CARAPI OnListenerHintsChanged(
        /* [in] */ Int32 hints);

    /**
     * Implement this method to be notified when the
     * {@link #getCurrentInterruptionFilter() interruption filter} changed.
     *
     * @param interruptionFilter The current
     *     {@link #getCurrentInterruptionFilter() interruption filter}.
     */
    CARAPI OnInterruptionFilterChanged(
        /* [in] */ Int32 interruptionFilter);

    /**
     * Inform the notification manager about dismissal of a single notification.
     * <p>
     * Use this if your listener has a user interface that allows the user to dismiss individual
     * notifications, similar to the behavior of Android's status bar and notification panel.
     * It should be called after the user dismisses a single notification using your UI;
     * upon being informed, the notification manager will actually remove the notification
     * and you will get an {@link #onNotificationRemoved(StatusBarNotification)} callback.
     * <P>
     * <b>Note:</b> If your listener allows the user to fire a notification's
     * {@link android.app.Notification#contentIntent} by tapping/clicking/etc., you should call
     * this method at that time <i>if</i> the Notification in question has the
     * {@link android.app.Notification#FLAG_AUTO_CANCEL} flag set.
     *
     * @param pkg Package of the notifying app.
     * @param tag Tag of the notification as specified by the notifying app in
     *     {@link android.app.NotificationManager#notify(String, int, android.app.Notification)}.
     * @param id  ID of the notification as specified by the notifying app in
     *     {@link android.app.NotificationManager#notify(String, int, android.app.Notification)}.
     * <p>
     * @deprecated Use {@link #cancelNotification(String key)}
     * instead. Beginning with {@link android.os.Build.VERSION_CODES#LOLLIPOP} this method will no longer
     * cancel the notification. It will continue to cancel the notification for applications
     * whose {@code targetSdkVersion} is earlier than {@link android.os.Build.VERSION_CODES#LOLLIPOP}.
     */
    CARAPI CancelNotification(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id);

    /**
     * Inform the notification manager about dismissal of a single notification.
     * <p>
     * Use this if your listener has a user interface that allows the user to dismiss individual
     * notifications, similar to the behavior of Android's status bar and notification panel.
     * It should be called after the user dismisses a single notification using your UI;
     * upon being informed, the notification manager will actually remove the notification
     * and you will get an {@link #onNotificationRemoved(StatusBarNotification)} callback.
     * <P>
     * <b>Note:</b> If your listener allows the user to fire a notification's
     * {@link android.app.Notification#contentIntent} by tapping/clicking/etc., you should call
     * this method at that time <i>if</i> the Notification in question has the
     * {@link android.app.Notification#FLAG_AUTO_CANCEL} flag set.
     * <p>
     * @param key Notification to dismiss from {@link StatusBarNotification#getKey()}.
     */
    CARAPI CancelNotification(
        /* [in] */ const String& key);

    /**
     * Inform the notification manager about dismissal of all notifications.
     * <p>
     * Use this if your listener has a user interface that allows the user to dismiss all
     * notifications, similar to the behavior of Android's status bar and notification panel.
     * It should be called after the user invokes the "dismiss all" function of your UI;
     * upon being informed, the notification manager will actually remove all active notifications
     * and you will get multiple {@link #onNotificationRemoved(StatusBarNotification)} callbacks.
     *
     * {@see #cancelNotification(String, String, int)}
     */
    CARAPI CancelAllNotifications();

    /**
     * Inform the notification manager about dismissal of specific notifications.
     * <p>
     * Use this if your listener has a user interface that allows the user to dismiss
     * multiple notifications at once.
     *
     * @param keys Notifications to dismiss, or {@code null} to dismiss all.
     *
     * {@see #cancelNotification(String, String, int)}
     */
    CARAPI CancelNotifications(
        /* [in] */ ArrayOf<String>* keys);

    /**
     * Sets the notification trim that will be received via {@link #onNotificationPosted}.
     *
     * <p>
     * Setting a trim other than {@link #TRIM_FULL} enables listeners that don't need access to the
     * full notification features right away to reduce their memory footprint. Full notifications
     * can be requested on-demand via {@link #getActiveNotifications(int)}.
     *
     * <p>
     * Set to {@link #TRIM_FULL} initially.
     *
     * @hide
     *
     * @param trim trim of the notifications to be passed via {@link #onNotificationPosted}.
     *             See <code>TRIM_*</code> constants.
     */
    // @SystemApi
    CARAPI SetOnNotificationPostedTrim(
        /* [in] */ Int32 trim);

    /**
     * Request the list of outstanding notifications (that is, those that are visible to the
     * current user). Useful when you don't know what's already been posted.
     *
     * @return An array of active notifications, sorted in natural order.
     */
    CARAPI GetActiveNotifications(
        /* [out] */ ArrayOf<IStatusBarNotification*>** ans);

    /**
     * Request the list of outstanding notifications (that is, those that are visible to the
     * current user). Useful when you don't know what's already been posted.
     *
     * @hide
     *
     * @param trim trim of the notifications to be returned. See <code>TRIM_*</code> constants.
     * @return An array of active notifications, sorted in natural order.
     */
    // @SystemApi
    CARAPI GetActiveNotifications(
        /* [in] */ Int32 trim,
        /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans);

    /**
     * Request one or more notifications by key. Useful if you have been keeping track of
     * notifications but didn't want to retain the bits, and now need to go back and extract
     * more data out of those notifications.
     *
     * @param keys the keys of the notifications to request
     * @return An array of notifications corresponding to the requested keys, in the
     * same order as the key list.
     */
    CARAPI GetActiveNotifications(
        /* [in] */ ArrayOf<String>* keys,
        /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans);

    /**
     * Request one or more notifications by key. Useful if you have been keeping track of
     * notifications but didn't want to retain the bits, and now need to go back and extract
     * more data out of those notifications.
     *
     * @hide
     *
     * @param keys the keys of the notifications to request
     * @param trim trim of the notifications to be returned. See <code>TRIM_*</code> constants.
     * @return An array of notifications corresponding to the requested keys, in the
     * same order as the key list.
     */
    // @SystemApi
    CARAPI GetActiveNotifications(
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ Int32 trim,
        /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ans);

    /**
     * Gets the set of hints representing current state.
     *
     * <p>
     * The current state may differ from the requested state if the hint represents state
     * shared across all listeners or a feature the notification host does not support or refuses
     * to grant.
     *
     * @return Zero or more of the HINT_ constants.
     */
    CARAPI GetCurrentListenerHints(
        /* [out] */ Int32* hints);

    /**
     * Gets the current notification interruption filter active on the host.
     *
     * <p>
     * The interruption filter defines which notifications are allowed to interrupt the user
     * (e.g. via sound &amp; vibration) and is applied globally. Listeners can find out whether
     * a specific notification matched the interruption filter via
     * {@link Ranking#matchesInterruptionFilter()}.
     * <p>
     * The current filter may differ from the previously requested filter if the notification host
     * does not support or refuses to apply the requested filter, or if another component changed
     * the filter in the meantime.
     * <p>
     * Listen for updates using {@link #onInterruptionFilterChanged(int)}.
     *
     * @return One of the INTERRUPTION_FILTER_ constants, or 0 on errors.
     */
    CARAPI GetCurrentInterruptionFilter(
        /* [out] */ Int32* filter);

    /**
     * Sets the desired {@link #getCurrentListenerHints() listener hints}.
     *
     * <p>
     * This is merely a request, the host may or may not choose to take action depending
     * on other listener requests or other global state.
     * <p>
     * Listen for updates using {@link #onListenerHintsChanged(int)}.
     *
     * @param hints One or more of the HINT_ constants.
     */
    CARAPI RequestListenerHints(
        /* [in] */ Int32 hints);

    /**
     * Sets the desired {@link #getCurrentInterruptionFilter() interruption filter}.
     *
     * <p>
     * This is merely a request, the host may or may not choose to apply the requested
     * interruption filter depending on other listener requests or other global state.
     * <p>
     * Listen for updates using {@link #onInterruptionFilterChanged(int)}.
     *
     * @param interruptionFilter One of the INTERRUPTION_FILTER_ constants.
     */
    CARAPI RequestInterruptionFilter(
        /* [in] */ Int32 interruptionFilter);

    /**
     * Returns current ranking information.
     *
     * <p>
     * The returned object represents the current ranking snapshot and only
     * applies for currently active notifications.
     * <p>
     * Generally you should use the RankingMap that is passed with events such
     * as {@link #onNotificationPosted(StatusBarNotification, RankingMap)},
     * {@link #onNotificationRemoved(StatusBarNotification, RankingMap)}, and
     * so on. This method should only be used when needing access outside of
     * such events, for example to retrieve the RankingMap right after
     * initialization.
     *
     * @return A {@link RankingMap} object providing access to ranking information
     */
    CARAPI GetCurrentRanking(
        /* [out] */ INotificationListenerServiceRankingMap** cr);

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    /**
     * Directly register this service with the Notification Manager.
     *
     * <p>Only system services may use this call. It will fail for non-system callers.
     * Apps should ask the user to add their listener in Settings.
     *
     * @param context Context required for accessing resources. Since this service isn't
     *    launched as a real Service when using this method, a context has to be passed in.
     * @param componentName the component that will consume the notification information
     * @param currentUser the user to use as the stream filter
     * @hide
     */
    // @SystemApi
    CARAPI RegisterAsSystemService(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 currentUser);

    /**
     * Directly unregister this service from the Notification Manager.
     *
     * <P>This method will fail for listeners that were not registered
     * with (@link registerAsService).
     * @hide
     */
    // @SystemApi
    CARAPI UnregisterAsSystemService();

private:
    CARAPI GetNotificationInterface(
        /* [out] */ IINotificationManager** nm);

    CARAPI_(Boolean) IsBound();

    CARAPI ApplyUpdate(
        /*  [in] */ INotificationRankingUpdate* update);

    CARAPI GetContext(
        /* [out] */ IContext** ctx);

private:
    AutoPtr<IINotificationListener> mWrapper;
    AutoPtr<RankingMap> mRankingMap;

    AutoPtr<IINotificationManager> mNoMan;

    /** Only valid after a successful call to (@link registerAsService}. */
    Int32 mCurrentUser;


    // This context is required for system services since NotificationListenerService isn't
    // started as a real Service and hence no context is available.
    AutoPtr<IContext> mSystemContext;
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_NOTIFICATIONLISTENERERVICE_H__
