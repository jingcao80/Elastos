
#ifndef __ELASTOS_DROID_SERVER_CNOTIFICATIONMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CNOTIFICATIONMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CNotificationManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include "CStatusBarManagerService.h"
#include "LightsService.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IITransientNotification;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::Server::CStatusBarManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

class CStatusBarManagerService;

CarClass(CNotificationManagerService)
{
private:
    class NotificationRecord : public ElRefBase
    {
    public:
        NotificationRecord(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 initialPid,
            /* [in] */ Int32 userId,
            /* [in] */ Int32 score,
            /* [in] */ INotification* notification);

    public:
        String mPkg;
        String mTag;
        Int32 mId;
        Int32 mUid;
        Int32 mInitialPid;
        Int32 mUserId;
        Int32 mScore;
        AutoPtr<INotification> mNotification;
        AutoPtr<IBinder> mStatusBarKey;
    };

    class WorkerHandler : public HandlerBase
    {
    public:
         WorkerHandler(
            /* [in] */ CNotificationManagerService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CNotificationManagerService* mHost;
    };

    class ToastRecord
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        ToastRecord(
            /* [in] */ Int32 pid,
            /* [in] */ const String& pkg,
            /* [in] */ IITransientNotification* callback,
            /* [in] */ Int32 duration);

        CARAPI_(void) Update(
            /* [in] */ Int32 duration);

    public:
        Int32 mPid;
        String mPkg;
        AutoPtr<IITransientNotification> mCallback;
        Int32 mDuration;
    };

    class MyNotificationCallbacks
        : public ElRefBase
        , public CStatusBarManagerService::INotificationCallbacks
    {
    public:
        MyNotificationCallbacks(
            /* [in] */ CNotificationManagerService* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnSetDisabled(
            /* [in] */ Int32 status);

        CARAPI OnClearAll();

        CARAPI OnNotificationClick(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id);

        CARAPI OnNotificationClear(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id);

        CARAPI OnPanelRevealed();

        CARAPI OnNotificationError(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 initialPid,
            /* [in] */ const String& message);

    private:
        CNotificationManagerService* mHost;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CNotificationManagerService* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNotificationManagerService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNotificationManagerService* mHost;
    };

    class SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ CNotificationManagerService* host,
            /* [in] */ IHandler* handler);

        CARAPI_(void) Observe();

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI_(void) Update();

    private:
        CNotificationManagerService* mHost;
    };

public:
    CNotificationManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Handle32 statusBar,
        /* [in] */ Handle32 lights);

    CARAPI CancelAllNotifications(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI EnqueueToast(
        /* [in] */ const String& pkg,
        /* [in] */ IITransientNotification* callback,
        /* [in] */ Int32 duration);

    CARAPI CancelToast(
        /* [in] */ const String& pkg,
        /* [in] */ IITransientNotification* callback);

    CARAPI EnqueueNotificationWithTag(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [out, callee] */ ArrayOf<Int32>** idReceived,
        /* [in] */ Int32 userId);

    CARAPI CancelNotificationWithTag(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 userId);

    CARAPI SetNotificationsEnabledForPackage(
        /* [in] */ const String& pkg,
        /* [in] */ Boolean enabled);

    CARAPI AreNotificationsEnabledForPackage(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);
public:
    CARAPI SystemReady();

    /*
     * Not exposed via Binder; for system use only (otherwise malicious apps could spoof the
     * uid/pid of another application)
     */
    CARAPI EnqueueNotificationInternal(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 callingPid,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [out, callee] */ ArrayOf<Int32>** idOut,
        /* [in] */ Int32 userId);

private:
    CARAPI_(void) LoadBlockDb();

    CARAPI_(void) WriteBlockDb();

    /*
     * Unchecked. Not exposed via Binder, but can be called in the course of enqueue*().
     */
    CARAPI_(Boolean) AreNotificationsEnabledForPackageInt(
        /* [in] */ const String& pkg);

    CARAPI_(void) ShowNextToastLocked();

    CARAPI_(void) CancelToastLocked(
        /* [in] */ Int32 index);

    CARAPI_(void) ScheduleTimeoutLocked(
        /* [in] */ CNotificationManagerService::ToastRecord* record,
        /* [in] */ Boolean immediate);

    CARAPI_(void) HandleTimeout(
        /* [in] */ CNotificationManagerService::ToastRecord* record);

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

    CARAPI_(void) SendAccessibilityEvent(
        /* [in] */ INotification* notification,
        /* [in] */ ICharSequence* packageName);

    CARAPI_(void) CancelNotificationLocked(
        /* [in] */ CNotificationManagerService::NotificationRecord* record,
        /* [in] */ Boolean sendDelete);

    /**
     * Cancels a notification ONLY if it has all of the {@code mustHaveFlags}
     * and none of the {@code mustNotHaveFlags}.
     */
    CARAPI_(void) CancelNotification(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 mustHaveFlags,
        /* [in] */ Int32 mustNotHaveFlags,
        /* [in] */ Boolean sendDelete,
        /* [in] */ Int32 userId);

    /**
     * Determine whether the userId applies to the notification in question, either because
     * they match exactly, or one of them is USER_ALL (which is treated as a wildcard).
     */
    CARAPI_(Boolean) NotificationMatchesUserId(
        /* [in] */ CNotificationManagerService::NotificationRecord* record,
        /* [in] */ Int32 userId);

    /**
     * Cancels all notifications from a given package that have all of the
     * {@code mustHaveFlags}.
     */
    CARAPI_(Boolean) CancelAllNotificationsInt(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 mustHaveFlags,
        /* [in] */ Int32 mustNotHaveFlags,
        /* [in] */ Boolean doit,
        /* [in] */ Int32 userId);

    CARAPI CheckCallerIsSystem();

    CARAPI CheckCallerIsSystemOrSameApp(
        /* [in] */ const String& pkg);

    CARAPI_(void) CancelAll(
        /* [in] */ Int32 userId);

    /*
     * lock on mNotificationList
     */
    CARAPI_(void) UpdateLightsLocked();

    /*
     * lock on mNotificationList
     */
    CARAPI_(Int32) IndexOfNotificationLocked(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 userId);

    CARAPI_(void) UpdateNotificationPulse();

private:
    static CARAPI_(String) IdDebugString(
        /* [in] */ IContext* baseContext,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 id);

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) GetLongArray(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resid,
        /* [in] */ Int32 maxlen,
        /* [in] */ ArrayOf<Int64>* def);

    static CARAPI_(Int32) Clamp(
        /* [in] */ Int32 x,
        /* [in] */ Int32 low,
        /* [in] */ Int32 high);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 MAX_PACKAGE_NOTIFICATIONS;

    static const Int32 MESSAGE_TIMEOUT;

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

    static const Int32 DB_VERSION;

    static const String TAG_BODY;
    static const String ATTR_VERSION;

    static const String TAG_BLOCKED_PKGS;
    static const String TAG_PACKAGE;
    static const String ATTR_NAME;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IIActivityManager> mAm;
    AutoPtr<IBinder> mForegroundToken;

    AutoPtr<IHandler> mHandler;
    CStatusBarManagerService* mStatusBar;
    AutoPtr<LightsService::Light> mNotificationLight;
    AutoPtr<LightsService::Light> mAttentionLight;
    AutoPtr<MyBroadcastReceiver> mIntentReceiver;
    Int32 mDefaultNotificationColor;
    Int32 mDefaultNotificationLedOn;
    Int32 mDefaultNotificationLedOff;

    AutoPtr< ArrayOf<Int64> > mDefaultVibrationPattern;
    AutoPtr< ArrayOf<Int64> > mFallbackVibrationPattern;

    Boolean mSystemReady;
    Int64 mDisabledNotifications;

    AutoPtr<NotificationRecord> mSoundNotification;
    AutoPtr<NotificationRecord> mVibrateNotification;

    AutoPtr<IIAudioService> mAudioService;
    AutoPtr<IVibrator> mVibrator;

    AutoPtr<MyNotificationCallbacks> mNotificationCallbacks;

    // for enabling and disabling notification pulse behavior
    Boolean mScreenOn;
    Boolean mInCall;
    Boolean mNotificationPulseEnabled;

    typedef List< AutoPtr<NotificationRecord> > NotificationRecordList;
    typedef typename List< AutoPtr<NotificationRecord> >::Iterator NotificationRecordIterator;

    NotificationRecordList mNotificationList;

    List< AutoPtr<ToastRecord> > mToastQueue;

    NotificationRecordList mLights;
    AutoPtr<NotificationRecord> mLedNotification;

    // Notification control database. For now just contains disabled packages.
    AutoPtr<IAtomicFile> mPolicyFile;
    HashSet<String> mBlockedPackages;

    Object mToastQueueLock;
    Object mBlockedPackagesLock;
    Object mNotificationListLock;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CNOTIFICATIONMANAGERSERVICE_H__
