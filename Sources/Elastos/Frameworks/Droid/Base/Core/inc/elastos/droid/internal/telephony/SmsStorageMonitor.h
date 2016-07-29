
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSSTORAGEMONITOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSSTORAGEMONITOR_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsStorageMonitor
    : public Handler
    , public ISmsStorageMonitor
{
private:
    class SmsBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        SmsBroadcastReceiver(
            /* [in] */ SmsStorageMonitor* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        SmsStorageMonitor* mHost;
    };

public:
    TO_STRING_IMPL("SmsStorageMonitor")

    CAR_INTERFACE_DECL()

    SmsStorageMonitor();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phoneBase);

    CARAPI Dispose();

    /** Returns whether or not there is storage available for an incoming SMS. */
    CARAPI IsStorageAvailable(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HandleMessage(IMessage* msg);

private:
    void CreateWakelock();

    /**
     * Called when SIM_FULL message is received from the RIL.  Notifies interested
     * parties that SIM storage for SMS messages is full.
     */
    void HandleIccFull();

private:
    static const String TAG;

    /** SIM/RUIM storage is full */
    static const Int32 EVENT_ICC_FULL;

    /** Memory status reporting is acknowledged by RIL */
    static const Int32 EVENT_REPORT_MEMORY_STATUS_DONE;

    /** Radio is ON */
    static const Int32 EVENT_RADIO_ON;

    /**
     * Hold the wake lock for 5 seconds, which should be enough time for
     * any Receiver(s) to grab its own wake lock.
     */
    static const Int32 WAKE_LOCK_TIMEOUT;

    /** Context from phone object passed to constructor. */
    AutoPtr<IContext> mContext;

    /** it is use to put in to extra value for SIM_FULL_ACTION and SMS_REJECTED_ACTION */
    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<ICommandsInterface> mCi;                // accessed from inner class

    AutoPtr<IBroadcastReceiver> mResultReceiver;

    /** Wake lock to ensure device stays awake while dispatching the SMS intent. */
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    Boolean mReportMemoryStatusPending;
    Boolean mStorageAvailable;                      // accessed from inner class
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSSTORAGEMONITOR_H__
