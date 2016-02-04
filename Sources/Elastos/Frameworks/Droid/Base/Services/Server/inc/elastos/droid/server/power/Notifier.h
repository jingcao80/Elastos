#ifndef __ELASTOS_DROID_SERVER_POWER_NOTIFIER_H__
#define __ELASTOS_DROID_SERVER_POWER_NOTIFIER_H__

#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.View.h>
#include <_Elastos.Droid.Server.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::IActivityManagerInternal;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Hardware::Input::IInputManagerInternal;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Server::Power::ISuspendBlocker;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::View::IWindowManagerPolicy;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

/**
 * Sends broadcasts about important power state changes.
 * <p>
 * This methods of this class may be called by the power manager service while
 * its lock is being held.  Internally it takes care of sending broadcasts to
 * notify other components of the system or applications asynchronously.
 * </p><p>
 * The notifier is designed to collapse unnecessary broadcasts when it is not
 * possible for the system to have observed an intermediate state.
 * </p><p>
 * For example, if the device wakes up, goes to sleep, wakes up again and goes to
 * sleep again before the wake up notification is sent, then the system will
 * be told about only one wake up and sleep.  However, we always notify the
 * fact that at least one transition occurred.  It is especially important to
 * tell the system when we go to sleep so that it can lock the keyguard if needed.
 * </p>
 */
class Notifier
    : public Object
{
private:
    class WakeUpBroadcastDone
        : public BroadcastReceiver
    {
    public:
        WakeUpBroadcastDone(
            /* [in] */ Notifier* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "Notifier::WakeUpBroadcastDone: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        Notifier* mHost;
    };

    class GoToSleepBroadcastDone
        : public BroadcastReceiver
    {
    public:
        GoToSleepBroadcastDone(
            /* [in] */ Notifier* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "Notifier::GoToSleepBroadcastDone: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        Notifier* mHost;
    };

    class NotifierHandler
        : public Handler
    {
    public:
        NotifierHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ Notifier* service)
            : Handler(looper, NULL, TRUE /*async*/)
            , mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        Notifier* mHost;
    };

    class StartedRunnable
        : public Runnable
    {
    public:
        StartedRunnable(
            /* [in] */ Notifier* host);

        // @Override
        CARAPI Run();

    private:
        Notifier* mHost;
    };

    class FinishedRunnable
        : public Runnable
    {
    public:
        FinishedRunnable(
            /* [in] */ Notifier* host);

        // @Override
        CARAPI Run();

    private:
        Notifier* mHost;
    };

public:
    Notifier(
        /* [in] */ ILooper* looper,
        /* [in] */ IContext* context,
        /* [in] */ IIBatteryStats* batteryStats,
        /* [in] */ IIAppOpsService* appOps,
        /* [in] */ ISuspendBlocker* suspendBlocker,
        /* [in] */ IWindowManagerPolicy* policy);

    /**
     * Called when a wake lock is acquired.
     */
    CARAPI_(void) OnWakeLockAcquired(
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Int32 ownerPid,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ const String& historyTag);

    /**
     * Called when a wake lock is changing.
     */
    CARAPI_(void) OnWakeLockChanging(
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Int32 ownerPid,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ const String& historyTag,
        /* [in] */ Int32 newFlags,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPackageName,
        /* [in] */ Int32 newOwnerUid,
        /* [in] */ Int32 newOwnerPid,
        /* [in] */ IWorkSource* newWorkSource,
        /* [in] */ const String& newHistoryTag);

    /**
     * Called when a wake lock is released.
     */
    CARAPI_(void) OnWakeLockReleased(
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Int32 ownerPid,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ const String& historyTag);

    /**
     * Notifies that the device is changing interactive state.
     */
    CARAPI_(void) OnInteractiveStateChangeStarted(
        /* [in] */ Boolean interactive,
        /* [in] */ Int32 reason);

    /**
     * Notifies that the device has finished changing interactive state.
     */
    CARAPI_(void) OnInteractiveStateChangeFinished(
        /* [in] */ Boolean interactive);

    /**
     * Called when there has been user activity.
     */
    CARAPI_(void) OnUserActivity(
        /* [in] */ Int32 event,
        /* [in] */ Int32 uid);

    CARAPI_(void) OnWirelessChargingStarted();

private:
    static CARAPI_(Int32) GetBatteryStatsWakeLockMonitorType(
        /* [in] */ Int32 flags);

    CARAPI_(void) UpdatePendingBroadcastLocked();

    CARAPI_(void) FinishPendingBroadcastLocked();

    CARAPI_(void) SendUserActivity();

    CARAPI_(void) SendNextBroadcast();

    CARAPI_(void) SendWakeUpBroadcast();

    CARAPI_(void) SendGoToSleepBroadcast();

    CARAPI_(void) PlayWirelessChargingStartedSound();

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const Int32 POWER_STATE_UNKNOWN;
    static const Int32 POWER_STATE_AWAKE;
    static const Int32 POWER_STATE_ASLEEP;

    static const Int32 MSG_USER_ACTIVITY;// = 1;
    static const Int32 MSG_BROADCAST;// = 2;
    static const Int32 MSG_WIRELESS_CHARGING_STARTED;// = 3;

    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IIAppOpsService> mAppOps;
    AutoPtr<ISuspendBlocker> mSuspendBlocker;
    AutoPtr<IWindowManagerPolicy> mPolicy;

    AutoPtr<IActivityManagerInternal> mActivityManagerInternal;
    AutoPtr<IInputManagerInternal> mInputManagerInternal;

    AutoPtr<NotifierHandler> mHandler;
    AutoPtr<IIntent> mScreenOnIntent;
    AutoPtr<IIntent> mScreenOffIntent;

    // The current power state.
    Int32 mActualPowerState;
    Int32 mLastGoToSleepReason;

    // True if there is a pending transition that needs to be reported.
    Boolean mPendingWakeUpBroadcast;
    Boolean mPendingGoToSleepBroadcast;

    // The currently broadcasted power state.  This reflects what other parts of the
    // system have observed.
    Int32 mBroadcastedPowerState;
    Boolean mBroadcastInProgress;
    Int64 mBroadcastStartTime;

    // True if a user activity message should be sent.
    Boolean mUserActivityPending;

    AutoPtr<IBroadcastReceiver> mWakeUpBroadcastDone;
    AutoPtr<IBroadcastReceiver> mGoToSleepBroadcastDone;

    friend class WakeUpBroadcastDone;
    friend class GoToSleepBroadcastDone;
};

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_NOTIFIER_H__
