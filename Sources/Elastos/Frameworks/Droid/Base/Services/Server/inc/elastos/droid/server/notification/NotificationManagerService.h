
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONMANAGERSERVICE_H__

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/lights/Light.h"
#include "elastos/droid/server/notification/ConditionProviders.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include "elastos/droid/server/notification/RankingHelper.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/LruCache.h"
#include <elastos/droid/telephony/PhoneStateListener.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IITransientNotification;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::Session::IMediaSessionManagerOnActiveSessionsChangedListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Server::Lights::Light;
using Elastos::Droid::Server::Notification::ManagedServices;
using Elastos::Droid::Server::StatusBar::IStatusBarManagerInternal;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IINotificationListener;
using Elastos::Droid::Service::Notification::IIStatusBarNotificationHolder;
using Elastos::Droid::Service::Notification::INotificationRankingUpdate;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::LruCache;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrayDeque;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Concurrent::IExecutorService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class NotificationManagerService
    : public SystemService
{
    friend class ConditionProviders;
    friend class RankingHelper;
public:
    class NotificationLedValues
        : public Object
    {
    public:
        Int32 mColor;
        Int32 mOnMS;
        Int32 mOffMS;
    };

    class FilterCacheInfo
        : public Object
    {
    public:
        String mPackageName;
        Int32 mNotificationId;
    };

    class SpamExecutorRunnable
        : public Runnable
    {
    public:
        SpamExecutorRunnable(
            /* [in] */ Int32 notifId,
            /* [in] */ NotificationManagerService* host);

        CARAPI Run();

    public:
        NotificationManagerService* mHost;
        Int32 mNotifId;
    };

    class BinderService
        : public Object
        , public IINotificationManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL();

        BinderService();

        ~BinderService();

        CARAPI constructor(
            /* [in] */ ISystemService* host);

        // Toasts
        // ============================================================================

        //@Override
        CARAPI EnqueueToast(
            /* [in] */ const String& pkg,
            /* [in] */ IITransientNotification* callback,
            /* [in] */ Int32 duration);

        //@Override
        CARAPI CancelToast(
            /* [in] */ const String& pkg,
            /* [in] */ IITransientNotification* callback);

        //@Override
        CARAPI EnqueueNotificationWithTag(
            /* [in] */ const String& pkg,
            /* [in] */ const String& opPkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ INotification* notification,
            /* [in] */ ArrayOf<Int32>* inIdReceived,
            /* [in] */ Int32 userId,
            /* [out, callee] */ ArrayOf<Int32>** outIdReceived);

        //@Override
        CARAPI CancelNotificationWithTag(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 userId);

        //@Override
        CARAPI CancelAllNotifications(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId);

        //@Override
        CARAPI SetNotificationsEnabledForPackage(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean enabled);

        /**
         * Use this when you just want to know if notifications are OK for this package.
         */
        //@Override
        CARAPI AreNotificationsEnabledForPackage(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [out] */ Boolean* res);

        //@Override
        CARAPI SetPackagePriority(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 priority);

        //@Override
        CARAPI GetPackagePriority(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* result);

        //@Override
        CARAPI SetPackageVisibilityOverride(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 visibility);

        //@Override
        CARAPI GetPackageVisibilityOverride(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* result);

        //@Override
        CARAPI SetShowNotificationForPackageOnKeyguard(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 status);

        //@Override
        CARAPI GetShowNotificationForPackageOnKeyguard(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* result);

        /**
         * System-only API for getting a list of current (i.e. not cleared) notifications.
         *
         * Requires ACCESS_NOTIFICATIONS which is signature|system.
         * @returns A list of all the notifications, in natural order.
         */
        //@Override
        CARAPI GetActiveNotifications(
            /* [in] */ const String& callingPkg,
            /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ns);

        /**
         * System-only API for getting a list of recent (cleared, no longer shown) notifications.
         *
         * Requires ACCESS_NOTIFICATIONS which is signature|system.
         */
        //@Override
        CARAPI GetHistoricalNotifications(
            /* [in] */ const String& callingPkg,
            /* [in] */ Int32 count,
            /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ns);

        /**
         * Register a listener binder directly with the notification manager.
         *
         * Only works with system callers. Apps should extend
         * {@link android.service.notification.NotificationListenerService}.
         */
        //@Override
        CARAPI RegisterListener(
            /* [in] */ IINotificationListener* listener,
            /* [in] */ IComponentName* component,
            /* [in] */ Int32 userid);

        /**
         * Remove a listener binder directly
         */
        //@Override
        CARAPI UnregisterListener(
            /* [in] */ IINotificationListener* listener,
            /* [in] */ Int32 userid);

        /**
         * Allow an INotificationListener to simulate a "clear all" operation.
         *
         * {@see com.android.server.StatusBarManagerService.NotificationCallbacks#onClearAllNotifications}
         *
         * @param token The binder for the listener, to check that the caller is allowed
         */
        //@Override
        CARAPI CancelNotificationsFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ ArrayOf<String>* keys);

        /**
         * Allow an INotificationListener to simulate clearing (dismissing) a single notification.
         *
         * {@see com.android.server.StatusBarManagerService.NotificationCallbacks#onNotificationClear}
         *
         * @param token The binder for the listener, to check that the caller is allowed
         */
        //@Override
        CARAPI CancelNotificationFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id);

        /**
         * Allow an INotificationListener to request the list of outstanding notifications seen by
         * the current user. Useful when starting up, after which point the listener callbacks
         * should be used.
         *
         * @param token The binder for the listener, to check that the caller is allowed
         * @param keys An array of notification keys to fetch, or null to fetch everything
         * @returns The return value will contain the notifications specified in keys, in that
         *      order, or if keys is null, all the notifications, in natural order.
         */
        //@Override
        CARAPI GetActiveNotificationsFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ ArrayOf<String>* keys,
            /* [in] */ Int32 trim,
            /* [out] */ IParceledListSlice** slice);

        //@Override
        CARAPI RequestHintsFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ Int32 hints);

        //@Override
        CARAPI GetHintsFromListener(
            /* [in] */ IINotificationListener* token,
            /* [out] */ Int32* result);

        //@Override
        CARAPI RequestInterruptionFilterFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ Int32 interruptionFilter);

        //@Override
        CARAPI GetInterruptionFilterFromListener(
            /* [in] */ IINotificationListener* token,
            /* [out] */ Int32* result);

        //@Override
        CARAPI SetOnNotificationPostedTrimFromListener(
            /* [in] */ IINotificationListener* token,
            /* [in] */ Int32 trim);

        //@Override
        CARAPI GetZenModeConfig(
            /* [out] */ IZenModeConfig** config);

        //@Override
        CARAPI SetZenModeConfig(
            /* [in] */ IZenModeConfig* config,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI NotifyConditions(
            /* [in] */ const String& pkg,
            /* [in] */ IIConditionProvider* provider,
            /* [in] */ ArrayOf<ICondition*>* conditions);

        //@Override
        CARAPI RequestZenModeConditions(
            /* [in] */ IIConditionListener* callback,
            /* [in] */ Int32 relevance);

        //@Override
        CARAPI SetZenModeCondition(
            /* [in] */ ICondition* condition);

        //@Override
        CARAPI SetAutomaticZenModeConditions(
            /* [in] */ ArrayOf<IUri*>* conditionIds);

        //@Override
        CARAPI GetAutomaticZenModeConditions(
            /* [out, callee] */ ArrayOf<ICondition*>** condition);

        //@Override
        CARAPI GetEffectsSuppressor(
            /* [out] */ IComponentName** name);

        //@Override
        CARAPI MatchesCallFilter(
            /* [in] */ IBundle* extras,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        CARAPI_(void) Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

    private:
        CARAPI_(void) CancelNotificationFromListenerLocked(
            /* [in] */ ManagedServices::ManagedServiceInfo* info,
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 userId);

        CARAPI_(void) EnforceSystemOrSystemUI(
            /* [in] */ const String& message);

    private:
        NotificationManagerService* mHost;
    };

    class NotificationListeners
        : public ManagedServices
    {
        friend class NotificationManagerService;
    public:
        NotificationListeners();

        ~NotificationListeners();

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI_(AutoPtr<ManagedServices::Config>) GetConfig();

        // @Override
        CARAPI OnServiceAdded(
        /* [in] */ ManagedServiceInfo* info);

        // @Override
        CARAPI OnServiceRemovedLocked(
        /* [in] */ ManagedServiceInfo* removed);

        CARAPI_(void) SetOnNotificationPostedTrimLocked(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ Int32 trim);

        CARAPI_(Int32) GetOnNotificationPostedTrim(
            /* [in] */ ManagedServiceInfo* info);

        /**
         * asynchronously notify all listeners about a new notification
         *
         * <p>
         * Also takes care of removing a notification that has been visible to a listener before,
         * but isn't anymore.
         */
        CARAPI NotifyPostedLocked(
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ IStatusBarNotification* oldSbn);

        /**
         * asynchronously notify all listeners about a removed notification
         */
        CARAPI NotifyRemovedLocked(
            /* [in] */ IStatusBarNotification* sbn);

        /**
         * asynchronously notify all listeners about a reordering of notifications
         */
        CARAPI NotifyRankingUpdateLocked();

        CARAPI NotifyListenerHintsChangedLocked(
            /* [in] */ Int32 hints);

        CARAPI NotifyInterruptionFilterChanged(
            /* [in] */ Int32 interruptionFilter);

    private:
        CARAPI NotifyPosted(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ INotificationRankingUpdate* rankingUpdate);

        CARAPI NotifyRemoved(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ INotificationRankingUpdate* rankingUpdate);

        CARAPI NotifyRankingUpdate(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ INotificationRankingUpdate* rankingUpdate);

        CARAPI NotifyListenerHintsChanged(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ Int32 hints);

        CARAPI NotifyInterruptionFilterChanged(
            /* [in] */ ManagedServiceInfo* info,
            /* [in] */ Int32 interruptionFilter);

        CARAPI_(Boolean) IsListenerPackage(
            /* [in] */ const String& packageName);

    private:
        /* private final ArraySet<ManagedServiceInfo> mLightTrimListeners = new ArraySet<>(); */
        AutoPtr<IArraySet> mLightTrimListeners;
        NotificationManagerService* mHost;
    };

    // class DumpFilter( remove to file "ManagedServices.h" )

    /**
     * Wrapper for a StatusBarNotification object that allows transfer across a oneway
     * binder without sending large amounts of data over a oneway transaction.
     */
    class StatusBarNotificationHolder
        : public Object
        , public IIStatusBarNotificationHolder
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL();

        StatusBarNotificationHolder();

        ~StatusBarNotificationHolder();

        CARAPI constructor(
            /* [in] */ IStatusBarNotification* value);

        /** Get the held value and clear it. This function should only be called once per holder */
        // @Override
        CARAPI Get(
            /* [out] */ IStatusBarNotification** ret);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IStatusBarNotification> mValue;
    };

    class MediaSessionManagerOnActiveSessionsChangedListener
        : public Object
        , public IMediaSessionManagerOnActiveSessionsChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaSessionManagerOnActiveSessionsChangedListener(
            /* [in] */ NotificationManagerService* host);

        CARAPI OnActiveSessionsChanged(
            /* [in] */ IList* controllers);

    public:
        NotificationManagerService* mHost;
    };

protected:
    // used to define mSettingsObserver
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("NotificationManagerService::SettingsObserver")

        SettingsObserver(
            /* [in] */ NotificationManagerService* host,
            /* [in] */ IHandler* handler);

        ~SettingsObserver();

        CARAPI_(void) Observe();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI_(void) Update(
            /* [in] */ IUri* uri);

    private:
        NotificationManagerService* mHost;
        AutoPtr<IUri> NOTIFICATION_LIGHT_PULSE_URI;
        AutoPtr<IUri> ENABLED_NOTIFICATION_LISTENERS_URI;
    };

private:
    // used in class Archive
    class IteratorInner
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL();

        IteratorInner(
            /* [in] */ IIterator* iter,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId);

        ~IteratorInner();

        // @Override
        CARAPI HasNext(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetNext(
            /* [out] */ IInterface** object);

        // @Override
        CARAPI Remove();

    private:
        CARAPI_(AutoPtr<IStatusBarNotification>) FindNext();

    protected:
        AutoPtr<IStatusBarNotification> mNext;

    private:
        AutoPtr<IIterator> mIter;
        String mPkg;
        Int32 mUserId;
    };

    class Archive
        : public Object
    {
    public:
        Archive(
            /* [in] */ Int32 size);

        ~Archive();

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI Record(
            /* [in] */ IStatusBarNotification* nr);

        CARAPI Clear();

        CARAPI DescendingIterator(
            /* [out] */ IIterator** iterator);

        CARAPI AscendingIterator(
            /* [out] */ IIterator** iterator);

        CARAPI Filter(
            /* [in] */ IIterator* iter,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId,
            /* [out] */IIterator** iterator);

        CARAPI GetArray(
            /* [in] */ Int32 count,
            /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ation);

        CARAPI GetArray(
            /* [in] */ Int32 count,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 userId,
            /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ation);

    protected:
        Int32 mBufferSize;
        /* final ArrayDeque<StatusBarNotification> mBuffer; */
        AutoPtr<IArrayDeque> mBuffer;
    };

    class ToastRecord
        : public Object
    {
    public:
        ToastRecord(
            /* [in] */ Int32 pid,
            /* [in] */ const String& pkg,
            /* [in] */ IITransientNotification* callback,
            /* [in] */ Int32 duration);

        ~ToastRecord();

        CARAPI_(void) Update(
            /* [in] */ Int32 duration);

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& prefix,
            /* [in] */ DumpFilter* filter);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mPid;
        String mPkg;
        AutoPtr<IITransientNotification> mCallback;
        Int32 mDuration;
    };

    // used to define mNotificationDelegate
    class MyNotificationDelegate
        : public Object
        , public INotificationDelegate
    {
    public:
        CAR_INTERFACE_DECL();

        MyNotificationDelegate(
            /* [in] */ NotificationManagerService* host);

        ~MyNotificationDelegate();

        // @Override
        CARAPI OnSetDisabled(
            /* [in] */ Int32 status);

        // @Override
        CARAPI OnClearAll(
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnNotificationClick(
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& key);

        // @Override
        CARAPI OnNotificationClear(
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnPanelRevealed();

        // @Override
        CARAPI OnPanelHidden();

        // @Override
        CARAPI OnNotificationError(
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 initialPid,
            /* [in] */ const String& message,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI OnNotificationVisibilityChanged(
            /* [in] */ ArrayOf<String>* newlyVisibleKeys,
            /* [in] */ ArrayOf<String>* noLongerVisibleKeys);

        // @Override
        CARAPI OnNotificationExpansionChanged(
            /* [in] */ const String& key,
            /* [in] */ Boolean userAction,
            /* [in] */ Boolean expanded);

    private:
        NotificationManagerService* mHost;
    };

    // used to define mIntentReceiver
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ NotificationManagerService* host);

        ~MyBroadcastReceiver();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "NotificationManagerService::MyBroadcastReceiver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        NotificationManagerService* mHost;
    };

    // used to define mBuzzBeepBlinked
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ NotificationManagerService* host);

        ~MyRunnable();

        // @Override
        CARAPI Run();

    private:
        NotificationManagerService* mHost;
    };

    // used to define mInternalService;
    class MyNotificationManagerInternal
        : public Object
        , public INotificationManagerInternal
    {
    public:
        CAR_INTERFACE_DECL();

        MyNotificationManagerInternal(
            /* [in] */ NotificationManagerService* host);

        ~MyNotificationManagerInternal();

        // @Override
        CARAPI EnqueueNotification(
            /* [in] */ const String& pkg,
            /* [in] */ const String& opPkg,
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ INotification* notification,
            /* [in] */ ArrayOf<Int32>* idReceived,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RemoveForegroundServiceFlagFromNotification(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 notificationId,
            /* [in] */ Int32 userId);

    private:
        NotificationManagerService* mHost;
    };

    class WorkerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("NotificationManagerService::WorkerHandler")

        WorkerHandler(
            /* [in] */ NotificationManagerService* host);

        ~WorkerHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        NotificationManagerService* mHost;
    };

    class RankingWorkerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("NotificationManagerService::RankingWorkerHandler")

        RankingWorkerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ NotificationManagerService* host);

        ~RankingWorkerHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        NotificationManagerService* mHost;
    };

    // used to class NotificationListeners
    class NotificationListenersRunnable
        : public Runnable
    {
    public:
        NotificationListenersRunnable(
            /* [in] */ Int32 id,
            /* [in] */ NotificationListeners* host,
            /* [in] */ NotificationManagerService* mainHost,
            /* [in] */ ManagedServices::ManagedServiceInfo* info,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ INotificationRankingUpdate* update,
            /* [in] */ Int32 data);

        ~NotificationListenersRunnable();

        // @Override
        CARAPI Run();

    private:
        Int32 mId;
        NotificationListeners* mHost;
        NotificationManagerService* mMainHost;
        AutoPtr<ManagedServices::ManagedServiceInfo> mInfo;
        AutoPtr<IStatusBarNotification> mSbn;
        AutoPtr<INotificationRankingUpdate> mUpdate;
        Int32 mData;
    };

    // used to function EnqueueNotificationInternal()
    class EnqueueNotificationInternalRunnable
        : public Runnable
    {
    public:
        EnqueueNotificationInternalRunnable(
            /* [in] */ NotificationManagerService* host,
            /* [in] */ const String& pkg,
            /* [in] */ const String& opPkg,
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ INotification* notification,
            /* [in] */ IUserHandle* user,
            /* [in] */ Boolean isSystemNotification);

        ~EnqueueNotificationInternalRunnable();

        // @Override
        CARAPI Run();

    private:
        NotificationManagerService* mHost;
        String mPkg;
        String mOpPkg;
        Int32 mCallingUid;
        Int32 mCallingPid;
        String mTag;
        Int32 mId;
        AutoPtr<INotification> mNotification;
        AutoPtr<IUserHandle> mUser;
        Boolean mIsSystemNotification;
    };

    // used to function CancelNotification()
    class CancelNotificationRunnable
        : public Runnable
    {
    public:
        CancelNotificationRunnable(
            /* [in] */ NotificationManagerService* host,
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 callingPid,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 mustHaveFlags,
            /* [in] */ Int32 mustNotHaveFlags,
            /* [in] */ Boolean sendDelete,
            /* [in] */ Int32 userId,
            /* [in] */ Int32 reason,
            /* [in] */ ManagedServices::ManagedServiceInfo* listener);

        ~CancelNotificationRunnable();

        // @Override
        CARAPI Run();

    private:
        NotificationManagerService* mHost;
        Int32 mCallingUid;
        Int32 mCallingPid;
        String mPkg;
        String mTag;
        Int32 mId;
        Int32 mMustHaveFlags;
        Int32 mMustNotHaveFlags;
        Boolean mSendDelete;
        Int32 mUserId;
        Int32 mReason;
        AutoPtr<ManagedServices::ManagedServiceInfo> mListener;
    };

    class ZenModeHelperCallback
        : public ZenModeHelper::Callback
    {
    public:
        ZenModeHelperCallback(
            /* [in] */ NotificationManagerService* host);

        ~ZenModeHelperCallback();

        // @Override
        CARAPI_(void) OnConfigChanged();

        // @Override
        CARAPI_(void) OnZenModeChanged();

    private:
        NotificationManagerService* mHost;
    };

    class InnerSub_PhoneStateListener
        : public PhoneStateListener
    {
    public:
        InnerSub_PhoneStateListener(
            /* [in] */ NotificationManagerService* host);

        // @Override
        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

    private:
        NotificationManagerService* mHost;
        const static String TAG;
    };

public:
    NotificationManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SavePolicyFile();

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

protected:
    static CARAPI_(AutoPtr< ArrayOf<Int64> >) GetLongArray(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resid,
        /* [in] */ Int32 maxlen,
        /* [in] */ ArrayOf<Int64>* def);

    CARAPI_(void) SetNotificationsEnabledForPackageImpl(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean enabled);

    CARAPI_(void) DumpImpl(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ DumpFilter* filter);

    CARAPI EnqueueNotificationInternal(
        /* [in] */ const String& pkg,
        /* [in] */ const String& opPkg,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [in] */ ArrayOf<Int32>* idOut,
        /* [in] */ Int32 incomingUserId,
        /* [out, callee] */ ArrayOf<Int32>** outIdReceived);

    CARAPI ShowNextToastLocked();

    CARAPI CancelToastLocked(
        /* [in] */ Int32 index);

    /*
     * lock on mToastQueue
     */
    CARAPI_(Int32) IndexOfToastLocked(
        /* [in] */ const String& pkg,
        /* [in] */ IITransientNotification* callback);

    /*
     * lock on mToastQueue
     */
    CARAPI_(void) KeepProcessAliveLocked(
        /* [in] */ Int32 pid);

    static CARAPI_(Int32) Clamp(
        /* [in] */ Int32 x,
        /* [in] */ Int32 low,
        /* [in] */ Int32 high);

    CARAPI_(void) SendAccessibilityEvent(
        /* [in] */ INotification* notification,
        /* [in] */ ICharSequence* packageName);

    /**
     * Cancels a notification ONLY if it has all of the {@code mustHaveFlags}
     * and none of the {@code mustNotHaveFlags}.
     */
    CARAPI_(void) CancelNotification(
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 mustHaveFlags,
        /* [in] */ Int32 mustNotHaveFlags,
        /* [in] */ Boolean sendDelete,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ ManagedServices::ManagedServiceInfo* listener);

    /**
     * Cancels all notifications from a given package that have all of the
     * {@code mustHaveFlags}.
     */
    CARAPI_(Boolean) CancelAllNotificationsInt(
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 mustHaveFlags,
        /* [in] */ Int32 mustNotHaveFlags,
        /* [in] */ Boolean doit,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ ManagedServices::ManagedServiceInfo* listener);

    CARAPI_(void) CancelAllLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ ManagedServices::ManagedServiceInfo* listener,
        /* [in] */ Boolean includeCurrentProfiles);

    // lock on mNotificationList
    CARAPI_(void) UpdateLightsLocked();

    // lock on mNotificationList
    CARAPI_(Int32) IndexOfNotificationLocked(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 userId);

    CARAPI_(void) ParseNotificationPulseCustomValuesString(
        /* [in] */ const String& customLedValuesString);

    CARAPI_(AutoPtr<NotificationLedValues>) GetLedValuesForNotification(
        /* [in] */ INotificationRecord* ledNotification);

    CARAPI_(String) MapPackage(
        /* [in] */ const String& pkg);

    // lock on mNotificationList
    CARAPI_(Int32) IndexOfNotificationLocked(
        /* [in] */ const String& key);

private:
    CARAPI_(void) LoadPolicyFile();

    CARAPI_(void) HandleSavePolicyFile();

    /** Use this when you actually want to post a notification or toast.
     *
     * Unchecked. Not exposed via Binder, but can be called in the course of enqueue*().
     */
    CARAPI_(Boolean) NoteNotificationOp(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 uid);

    /**
     * Read the old XML-based app block database and import those blockages into the AppOps system.
     */
    CARAPI_(void) ImportOldBlockDb();

    CARAPI_(void) UpdateListenerHintsLocked();

    CARAPI_(void) UpdateEffectsSuppressorLocked();

    CARAPI_(void) UpdateInterruptionFilterLocked();

    CARAPI_(AutoPtr< ArrayOf<String> >) GetActiveNotificationKeys(
        /* [in] */ IINotificationListener* token);

    CARAPI_(String) DisableNotificationEffects(
        /* [in] */ NotificationRecord* record);

    CARAPI BuzzBeepBlinkLocked(
        /* [in] */ NotificationRecord* record);

    static AutoPtr<IAudioAttributes> AudioAttributesForNotification(
        /* [in] */ INotification* n);

    CARAPI_(void) ScheduleTimeoutLocked(
        /* [in] */ ToastRecord* r);

    CARAPI_(void) HandleTimeout(
        /* [in] */ ToastRecord* record);

    CARAPI_(void) HandleRankingReconsideration(
        /* [in] */ IMessage* message);

    CARAPI_(void) HandleRankingConfigChange();

    // let zen mode evaluate this record
    CARAPI_(void) ApplyZenModeLocked(
        /* [in] */ NotificationRecord* record);

    // lock on mNotificationList
    CARAPI_(Int32) FindNotificationRecordIndexLocked(
        /* [in] */ NotificationRecord* target);

    CARAPI_(void) ScheduleSendRankingUpdate();

    CARAPI_(void) HandleSendRankingUpdate();

    CARAPI_(void) ScheduleListenerHintsChanged(
        /* [in] */ Int32 state);

    CARAPI_(void) ScheduleInterruptionFilterChanged(
        /* [in] */ Int32 listenerInterruptionFilter);

    CARAPI_(void) HandleListenerHintsChanged(
        /* [in] */ Int32 hints);

    CARAPI_(void) HandleListenerInterruptionFilterChanged(
        /* [in] */ Int32 interruptionFilter);

    CARAPI_(void) CancelNotificationLocked(
        /* [in] */ NotificationRecord* record,
        /* [in] */ Boolean sendDelete,
        /* [in] */ Int32 reason);

    /**
     * Determine whether the userId applies to the notification in question, either because
     * they match exactly, or one of them is USER_ALL (which is treated as a wildcard).
     */
    CARAPI_(Boolean) NotificationMatchesUserId(
        /* [in] */ NotificationRecord* record,
        /* [in] */ Int32 userId);

    /**
     * Determine whether the userId applies to the notification in question, either because
     * they match exactly, or one of them is USER_ALL (which is treated as a wildcard) or
     * because it matches one of the users profiles.
     */
    CARAPI_(Boolean) NotificationMatchesCurrentProfiles(
        /* [in] */ NotificationRecord* record,
        /* [in] */ Int32 userId);

    // Warning: The caller is responsible for invoking updateLightsLocked().
    CARAPI_(void) CancelGroupChildrenLocked(
        /* [in] */ NotificationRecord* r,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ const String& listenerName);

    CARAPI_(Boolean) IsLedNotificationForcedOn(
        /* [in] */ INotificationRecord* r);

    CARAPI_(void) UpdateNotificationPulse();

    static CARAPI_(Boolean) IsUidSystem(
        /* [in] */ Int32 uid);

    static CARAPI_(Boolean) IsCallerSystem();

    static CARAPI CheckCallerIsSystem();

    static CARAPI CheckCallerIsSystemOrSameApp(
        /* [in] */ const String& pkg);

    static CARAPI_(String) CallStateToString(
        /* [in] */ Int32 state);

    CARAPI_(void) ListenForCallState();

    /**
     * Generates a NotificationRankingUpdate from 'sbns', considering only
     * notifications visible to the given listener.
     *
     * <p>Caller must hold a lock on mNotificationList.</p>
     */
    CARAPI_(AutoPtr<INotificationRankingUpdate>) MakeRankingUpdateLocked(
        /* [in] */ ManagedServices::ManagedServiceInfo* info);

    CARAPI_(Boolean) IsVisibleToListener(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ ManagedServices::ManagedServiceInfo* listener);

    CARAPI_(void) UpdateDisableDucking();

    CARAPI_(Int32) GetNotificationHash(
        /* [in] */ INotification* notification,
        /* [in] */ const String& packageName);

    CARAPI_(Boolean) IsNotificationSpam(
        /* [in] */ INotification* notification,
        /* [in] */ const String& basePkg);

protected:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 MAX_PACKAGE_NOTIFICATIONS;

    static const Int32 MESSAGE_TIMEOUT = 2;
    static const Int32 MESSAGE_SAVE_POLICY_FILE = 3;
    static const Int32 MESSAGE_RECONSIDER_RANKING = 4;
    static const Int32 MESSAGE_RANKING_CONFIG_CHANGE = 5;
    static const Int32 MESSAGE_SEND_RANKING_UPDATE = 6;
    static const Int32 MESSAGE_LISTENER_HINTS_CHANGED = 7;
    static const Int32 MESSAGE_LISTENER_NOTIFICATION_FILTER_CHANGED = 8;

    static const Int32 LONG_DELAY;
    static const Int32 SHORT_DELAY;

    static const AutoPtr<ArrayOf<Int64> > DEFAULT_VIBRATE_PATTERN;
    static const Int32 VIBRATE_PATTERN_MAXLEN;

    static const Int32 DEFAULT_STREAM_TYPE;
    static const Boolean SCORE_ONGOING_HIGHER;

    static const Int32 JUNK_SCORE;
    static const Int32 NOTIFICATION_PRIORITY_MULTIPLIER;
    static const Int32 SCORE_DISPLAY_THRESHOLD;

    // Notifications with scores below this will not interrupt the user, either via LED or
    // sound or vibration
    static const Int32 SCORE_INTERRUPTION_THRESHOLD;// = Notification.PRIORITY_LOW * NOTIFICATION_PRIORITY_MULTIPLIER;

    static const Boolean ENABLE_BLOCKED_NOTIFICATIONS;
    static const Boolean ENABLE_BLOCKED_TOASTS;

    // When #matchesCallFilter is called from the ringer, wait at most
    // 3s to resolve the contacts. This timeout is required since
    // ContactsProvider might take a long time to start up.
    //
    // Return STARRED_CONTACT when the timeout is hit in order to avoid
    // missed calls in ZEN mode "Important".
    static const Int32 MATCHES_CALL_FILTER_CONTACTS_TIMEOUT_MS;
    static const Float MATCHES_CALL_FILTER_TIMEOUT_AFFINITY;

    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IStatusBarManagerInternal> mStatusBar;
    AutoPtr<IVibrator> mVibrator;

    AutoPtr<IBinder> mForegroundToken;

    AutoPtr<Light> mAttentionLight;

    AutoPtr<NotificationRecord> mSoundNotification;
    AutoPtr<NotificationRecord> mVibrateNotification;

    // used as a mutex for access to all active notifications & listeners
    /* final ArrayList<NotificationRecord> mNotificationList = new ArrayList<NotificationRecord>(); */
    AutoPtr<IArrayList> mNotificationList;
    /* final ArrayMap<String, NotificationRecord> mNotificationsByKey = new ArrayMap<String, NotificationRecord>(); */
    AutoPtr<IArrayMap> mNotificationsByKey;
    /* final ArrayList<ToastRecord> mToastQueue = new ArrayList<ToastRecord>(); */
    AutoPtr<IArrayList> mToastQueue;

    /* ArrayList<String> mLights = new ArrayList<String>(); */
    AutoPtr<IArrayList> mLights;
    AutoPtr<NotificationRecord> mLedNotification;

public:
    static const Int32 DB_VERSION;

    static const String TAG_BODY;
    static const String ATTR_VERSION;

    static const String TAG_BLOCKED_PKGS;
    static const String TAG_PACKAGE;
    static const String ATTR_NAME;

    static const Int32 MY_UID;
    static const Int32 MY_PID;
    static const Int32 REASON_DELEGATE_CLICK;
    static const Int32 REASON_DELEGATE_CANCEL;
    static const Int32 REASON_DELEGATE_CANCEL_ALL;
    static const Int32 REASON_DELEGATE_ERROR;
    static const Int32 REASON_PACKAGE_CHANGED;
    static const Int32 REASON_USER_STOPPED;
    static const Int32 REASON_PACKAGE_BANNED;
    static const Int32 REASON_NOMAN_CANCEL;
    static const Int32 REASON_NOMAN_CANCEL_ALL;
    static const Int32 REASON_LISTENER_CANCEL;
    static const Int32 REASON_LISTENER_CANCEL_ALL;
    static const Int32 REASON_GROUP_SUMMARY_CANCELED;
    static const String IS_FILTERED_QUERY;

    static AutoPtr<IUri> FILTER_MSG_URI;

    static AutoPtr<IUri> UPDATE_MSG_URI;

    AutoPtr<IIActivityManager> mAm;

    AutoPtr<WorkerHandler> mHandler;
    AutoPtr<IHandlerThread> mRankingThread;// = new HandlerThread("ranker", Process.THREAD_PRIORITY_BACKGROUND);

    AutoPtr<Light> mNotificationLight;

    Int32 mDefaultNotificationColor;
    Int32 mDefaultNotificationLedOn;

    Int32 mDefaultNotificationLedOff;
    AutoPtr< ArrayOf<Int64> > mDefaultVibrationPattern;

    AutoPtr< ArrayOf<Int64> > mFallbackVibrationPattern;
    Boolean mUseAttentionLight;
    Boolean mSystemReady;

    AutoPtr<LruCache<Int32, AutoPtr<FilterCacheInfo> > > mSpamCache;
    AutoPtr<IExecutorService> mSpamExecutor;

    Boolean mDisableNotificationEffects;
    Int32 mCallState;

    /* final ArraySet<ManagedServiceInfo> mListenersDisablingEffects = new ArraySet<>();*/
    AutoPtr<IArraySet> mListenersDisablingEffects;
    AutoPtr<IComponentName> mEffectsSuppressor;
    Int32 mListenerHints;  // right now, all hints are global
    Int32 mInterruptionFilter;  // current ZEN mode as communicated to listeners

    // for enabling and disabling notification pulse behavior
    Boolean mScreenOn;
    Boolean mInCall;
    Boolean mNotificationPulseEnabled;
    AutoPtr<IHashMap> mNotificationPulseCustomLedValues;
    AutoPtr<IMap> mPackageNameMappings;

    // for checking lockscreen status
    AutoPtr<IKeyguardManager> mKeyguardManager;

    AutoPtr<IAppOpsManager> mAppOps;

    AutoPtr<Archive> mArchive;

    // Notification control database. For now just contains disabled packages.
    AutoPtr<IAtomicFile> mPolicyFile;
    /* private HashSet<String> mBlockedPackages = new HashSet<String>(); */
    AutoPtr<IHashSet> mBlockedPackages;

    AutoPtr<RankingHelper> mRankingHelper;

    AutoPtr<ManagedServices::UserProfiles> mUserProfiles;// = new UserProfiles();
    AutoPtr<NotificationListeners> mListeners;
    AutoPtr<ConditionProviders> mConditionProviders;
    AutoPtr<NotificationUsageStats> mUsageStats;
    Boolean mDisableDuckingWhileMedia;
    Boolean mActiveMedia;

    AutoPtr<MyNotificationDelegate> mNotificationDelegate;
    AutoPtr<MyBroadcastReceiver> mIntentReceiver;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<ZenModeHelper> mZenModeHelper;
    AutoPtr<MyRunnable> mBuzzBeepBlinked;
    AutoPtr<IBinder> mService;
    AutoPtr<MyNotificationManagerInternal> mInternalService;

    AutoPtr<MediaSessionManagerOnActiveSessionsChangedListener> mSessionListener;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONMANAGERSERVICE_H__
