#ifndef __ELASTOS_DROID_SERVER_CTHROTTLESERVICE_H__
#define __ELASTOS_DROID_SERVER_CTHROTTLESERVICE_H__

#include "_Elastos_Droid_Server_CThrottleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Net::INetworkManagementEventObserver;
using Elastos::Droid::Utility::ITrustedTime;
using Elastos::IO::IFile;
using Elastos::Utility::ICalendar;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger64;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CThrottleService)
{
private:
    class InterfaceObserver
        : public ElRefBase
        , public INetworkManagementEventObserver
        , public IBinder
    {
    public:
        InterfaceObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ Int32 msg,
            /* [in] */ const String& iface);

        CAR_INTERFACE_DECL();

        CARAPI InterfaceStatusChanged(
            /* [in] */ const String& iface,
            /* [in] */ Boolean up);

        CARAPI InterfaceLinkStateChanged(
            /* [in] */ const String& iface,
            /* [in] */ Boolean up);

        CARAPI InterfaceAdded(
            /* [in] */ const String& iface);

        CARAPI InterfaceRemoved(
            /* [in] */ const String& iface);

        CARAPI LimitReached(
            /* [in] */ const String& limitName,
            /* [in] */ const String& iface);

        CARAPI InterfaceClassDataActivityChanged(
            /* [in] */ const String& label,
            /* [in] */ Boolean active);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        Int32 mMsg;
        AutoPtr<IHandler> mHandler;
        String mIface;
    };//class InterfaceObserver

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ Int32 msg);

        CARAPI_(void) Register(
            /* [in] */ IContext* context);

        CARAPI_(void) Unregister(
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnChange(
            /* [in] */  Boolean selfChange);

    private:
        Int32 mMsg;
        AutoPtr<IHandler> mHandler;
    };//class SettingObserver

    class BroadcastReceiverSelfDefine1
        : public BroadcastReceiver
    {
    public:
        BroadcastReceiverSelfDefine1(
            /* [in] */ CThrottleService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CThrottleService::BroadcastReceiverSelfDefine1: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CThrottleService* mHost;
    };//class BroadcastReceiverSelfDefine1

    class BroadcastReceiverSelfDefine2
        : public BroadcastReceiver
    {
    public:
        BroadcastReceiverSelfDefine2(
            /* [in] */ CThrottleService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CThrottleService::BroadcastReceiverSelfDefine2: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CThrottleService* mHost;
    };//class BroadcastReceiverSelfDefine2

    class MyHandler
        : public HandlerBase
    {
        friend class InterfaceObserver;
        friend class SettingsObserver;
        friend class CThrottleService;
    public:
        MyHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CThrottleService* host);

    private:
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI OnRebootRecovery();

        CARAPI OnPolicyChanged();

        CARAPI OnPollAlarm();

        CARAPI OnIfaceUp();

        CARAPI_(void) CheckThrottleAndPostNotification(
            /* [in] */ Int64 currentTotal);

        CARAPI_(void) PostNotification(
            /* [in] */ Int32 titleInt,
            /* [in] */ Int32 messageInt,
            /* [in] */ Int32 icon,
            /* [in] */ Int32 flags);

        CARAPI_(void) ClearThrottleAndNotification();

        CARAPI_(AutoPtr<ICalendar>) CalculatePeriodEnd(
            /* [in] */ Int64 now);

        CARAPI_(AutoPtr<ICalendar>) CalculatePeriodStart(
            /* [in] */ ICalendar* end);

        CARAPI OnResetAlarm();

    private:
        CThrottleService* mHost;

    };//class MyHandler

    // records bytecount data for a given time and accumulates it into larger time windows
    // for logging and other purposes
    //
    // since time can be changed (user or network action) we will have to track the time of the
    // last recording and deal with it.
    class DataRecorder
        : public ElRefBase
    {
        friend class CThrottleService;
    public:
        DataRecorder(
            /* [in] */ IContext* context,
            /* [in] */ CThrottleService* parent);

        CARAPI_(Boolean) SetNextPeriod(
            /* [in] */ ICalendar* start,
            /* [in] */ ICalendar* end);

        CARAPI GetPeriodEnd(
            /* [out] */ Int64* result);

        CARAPI_(void) SetPeriodEnd(
            /* [in] */ ICalendar* end);

        CARAPI GetPeriodStart(
            /* [out] */ Int64* result);

        CARAPI_(void) SetPeriodStart(
            /* [in] */ ICalendar* start);

        CARAPI GetPeriodCount(
            /* [out] */ Int32* result);

        CARAPI_(void) ZeroData(
            /* [in] */ Int32 field);

        // if time moves backward accumulate all read/write that's lost into the now
        // otherwise time moved forward.
        CARAPI_(void) AddData(
            /* [in] */ Int64 bytesRead,
            /* [in] */ Int64 bytesWritten);

    private:
        CARAPI_(AutoPtr<IFile>) GetDataFile();

        CARAPI_(void) CheckForSubscriberId();

        CARAPI_(void) CheckAndDeleteLRUDataFile(
            /* [in] */ IFile* dir);

        CARAPI_(AutoPtr<IFile>) UseMRUFile(
            /* [in] */ IFile* dir);

        CARAPI_(void) Record();

        CARAPI_(void) Retrieve();

        CARAPI_(Int64) GetPeriodRx(
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetPeriodTx(
            /* [in] */ Int32 which);
    public:
        AutoPtr<ArrayOf<Int64> > mPeriodRxData;
        AutoPtr<ArrayOf<Int64> > mPeriodTxData;
        Int32 mCurrentPeriod;
        Int32 mPeriodCount;

        AutoPtr<ICalendar> mPeriodStart;
        AutoPtr<ICalendar> mPeriodEnd;

        CThrottleService* mParent;
        AutoPtr<IContext> mContext;
        String mImsi;

//        AutoPtr<ITelephonyManager> mTelephonyManager;

    private:
        static const Int32 MAX_SIMS_SUPPORTED = 3;
        static const Int32 DATA_FILE_VERSION = 1;

        Object mParentLock;

    }; //class DataRecorder

public:
    CThrottleService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */  IContext* context,
        /* [in] */  IINetworkManagementService* nmService,
        /* [in] */  ITrustedTime* time,
        /* [in] */  const String& iface);

    // TODO - fetch for the iface
    // return time in the local, system wall time, correcting for the use of ntp

    CARAPI GetResetTime(
        /* [in] */  const String& iface,
        /* [out] */ Int64* result);

    // TODO - fetch for the iface
    // return time in the local, system wall time, correcting for the use of ntp
    CARAPI GetPeriodStartTime(
        /* [in] */  const String& iface,
        /* [out] */ Int64* result);

    //TODO - a better name?  getCliffByteCountThreshold?
    // TODO - fetch for the iface
    CARAPI GetCliffThreshold(
        /* [in] */ const String& iface,
        /* [in] */ Int32 cliff,
        /* [out] */ Int64* result);

    // TODO - a better name? getThrottleRate?
    // TODO - fetch for the iface
    CARAPI GetCliffLevel(
        /* [in] */ const String& iface,
        /* [in] */ Int32 cliff,
        /* [out] */ Int32* result);

    CARAPI GetHelpUri(
        /* [out] */ String* result);

    // TODO - fetch for the iface
    CARAPI GetByteCount(
        /* [in] */ const String& iface,
        /* [in] */ Int32 dir,
        /* [in] */ Int32 period,
        /* [in] */ Int32 ago,
        /* [out] */ Int64* result);

    // TODO - a better name - getCurrentThrottleRate?
    // TODO - fetch for the iface
    CARAPI GetThrottle(
        /* [in] */ const String& iface,
        /* [out] */ Int32* result);

    CARAPI_(void) SystemReady();

    CARAPI_(void) Shutdown();

    CARAPI_(void) DispatchPoll();

    CARAPI_(void) DispatchReset();

    //@Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static CARAPI_(AutoPtr<IINetworkManagementService>) GetNetworkManagementService();

    CARAPI EnforceAccessPermission();

    CARAPI_(Int64) NtpToWallTime(
        /* [in] */ Int64 ntpTime);

private:
    static const String TESTING_ENABLED_PROPERTY;

    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<MyHandler> mHandler;
    AutoPtr<IHandlerThread> mThread;

    AutoPtr<IContext> mContext;

    static const Int32 INITIAL_POLL_DELAY_SEC = 90;
    static const Int32 TESTING_POLLING_PERIOD_SEC = 60 * 1;
    static const Int32 TESTING_RESET_PERIOD_SEC = 60 * 10;
    static const Int64 TESTING_THRESHOLD;

    static const Int64 MAX_NTP_CACHE_AGE;

    Int64 mMaxNtpCacheAge;

    Int32 mPolicyPollPeriodSec;
    AutoPtr<IAtomicInteger64> mPolicyThreshold;
    AutoPtr<IAtomicInteger32> mPolicyThrottleValue;
    Int32 mPolicyResetDay; // 1-28
    Int32 mPolicyNotificationsAllowedMask;

    Int64 mLastRead; // read byte count from last poll
    Int64 mLastWrite; // write byte count from last poll

    static const String ACTION_POLL;
    static Int32 POLL_REQUEST;
    AutoPtr<IPendingIntent> mPendingPollIntent;
    static const String ACTION_RESET;
    static Int32 RESET_REQUEST;
    AutoPtr<IPendingIntent> mPendingResetIntent;

    AutoPtr<IINetworkManagementService> mNMService;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<INotificationManager> mNotificationManager;

    AutoPtr<DataRecorder> mRecorder;

    String mIface;

    static const Int32 NOTIFICATION_WARNING = 2;

    AutoPtr<INotification> mThrottlingNotification;
    Boolean mWarningNotificationSent;

    AutoPtr<InterfaceObserver> mInterfaceObserver;
    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<IAtomicInteger32> mThrottleIndex; // 0 for none, 1 for first throttle val, 2 for next, etc //check
    static const Int32 THROTTLE_INDEX_UNINITIALIZED = -1;
    static const Int32 THROTTLE_INDEX_UNTHROTTLED   =  0;

    AutoPtr<IIntent> mPollStickyBroadcast;

    AutoPtr<ITrustedTime> mTime;

    static const Int32 EVENT_REBOOT_RECOVERY = 0;
    static const Int32 EVENT_POLICY_CHANGED  = 1;
    static const Int32 EVENT_POLL_ALARM = 2;
    static const Int32 EVENT_RESET_ALARM = 3;
    static const Int32 EVENT_IFACE_UP = 4;
};//CThrottleService

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CTHROTTLESERVICE_H__
