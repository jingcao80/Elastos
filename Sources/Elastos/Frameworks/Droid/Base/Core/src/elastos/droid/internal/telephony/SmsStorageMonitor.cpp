

#include "elastos/droid/internal/telephony/SmsStorageMonitor.h"
#include "elastos/droid/os/AsyncResult.h"
//#include "elastos/droid/R.h"
//#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
//using Elastos::Droid::Provider::Telephony::Sms::IIntents;
//using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

ECode SmsStorageMonitor::BroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    assert(0 && "TODO");
//    if (intent->GetAction()->Equals(IIntent::ACTION_DEVICE_STORAGE_FULL)) {
//        mStorageAvailable = FALSE;
//        mCi->ReportSmsMemoryStatus(FALSE, ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
//    } else if (intent->GetAction()->Equals(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL)) {
//        mStorageAvailable = TRUE;
//        mCi->ReportSmsMemoryStatus(TRUE, ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
//    }
}

/**
 * Monitors the device and ICC storage, and sends the appropriate events.
 *
 * This code was formerly part of {@link SMSDispatcher}, and has been moved
 * into a separate class to support instantiation of multiple SMSDispatchers on
 * dual-mode devices that require support for both 3GPP and 3GPP2 format messages.
 */
CAR_INTERFACE_IMPL(SmsStorageMonitor, Handler, ISmsStorageMonitor)

const String SmsStorageMonitor::TAG("SmsStorageMonitor");

/** SIM/RUIM storage is full */
const Int32 SmsStorageMonitor::EVENT_ICC_FULL = 1;

/** Memory status reporting is acknowledged by RIL */
const Int32 SmsStorageMonitor::EVENT_REPORT_MEMORY_STATUS_DONE = 2;

/** Radio is ON */
const Int32 SmsStorageMonitor::EVENT_RADIO_ON = 3;

/**
 * Hold the wake lock for 5 seconds, which should be enough time for
 * any Receiver(s) to grab its own wake lock.
 */
const Int32 SmsStorageMonitor::WAKE_LOCK_TIMEOUT = 5000;

SmsStorageMonitor::SmsStorageMonitor()
{
    mStorageAvailable = TRUE;
    mResultReceiver = new BroadcastReceiver();
}

/**
 * Creates an SmsStorageMonitor and registers for events.
 * @param phone the Phone to use
 */
ECode SmsStorageMonitor::constructor(IPhoneBase* phone)
{
    assert(0 && "TODO");
//    mPhone = phone;
//    mContext = phone->GetContext();
//    mCi = phone.mCi;
//
//    CreateWakelock();
//
//    mCi->SetOnIccSmsFull(this, EVENT_ICC_FULL, NULL);
//    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
//
//    // Register for device storage intents.  Use these to notify the RIL
//    // that storage for SMS is or is not available.
//    IntentFilter filter = new IntentFilter();
//    filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_FULL);
//    filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL);
//    mContext->RegisterReceiver(mResultReceiver, filter);
}

ECode SmsStorageMonitor::Dispose()
{
    mCi->UnSetOnIccSmsFull(this);
    mCi->UnregisterForOn(this);
    assert(0 && "TODO");
//    return mContext->UnregisterReceiver(mResultReceiver);
}

/**
 * Handles events coming from the phone stack. Overridden from handler.
 * @param msg the message to handle
 */
//@Override
ECode SmsStorageMonitor::HandleMessage(IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    assert(0 && "TODO");
//    switch (what) {
//        case EVENT_ICC_FULL:
//            HandleIccFull();
//            break;
//
//        case EVENT_REPORT_MEMORY_STATUS_DONE:
//            IAsyncResult* ar = IAsyncResult::Probe(obj);
//            if (ar.exception != NULL) {
//                mReportMemoryStatusPending = TRUE;
//                Rlog->V(TAG, "Memory status report to modem pending : mStorageAvailable = "
//                        + mStorageAvailable);
//            } else {
//                mReportMemoryStatusPending = FALSE;
//            }
//            break;
//
//        case EVENT_RADIO_ON:
//            if (mReportMemoryStatusPending) {
//                Rlog->V(TAG, "Sending pending memory status report : mStorageAvailable = "
//                        + mStorageAvailable);
//                mCi->ReportSmsMemoryStatus(mStorageAvailable,
//                        ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE));
//            }
//            break;
//    }
    return NOERROR;
}

void SmsStorageMonitor::CreateWakelock()
{
    assert(0 && "TODO");
//    PowerManager pm = (PowerManager)mContext->GetSystemService(Context.POWER_SERVICE);
//    mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "SmsStorageMonitor");
//    mWakeLock->SetReferenceCounted(TRUE);
}

/**
 * Called when SIM_FULL message is received from the RIL.  Notifies interested
 * parties that SIM storage for SMS messages is full.
 */
void SmsStorageMonitor::HandleIccFull()
{
    assert(0 && "TODO");
    // broadcast SIM_FULL intent
//    Intent intent = new Intent(Intents.SIM_FULL_ACTION);
//    mWakeLock->Acquire(WAKE_LOCK_TIMEOUT);
//    SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
//    mContext->SendBroadcast(intent, Manifest::permission::RECEIVE_SMS);
}

/** Returns whether or not there is storage available for an incoming SMS. */
ECode SmsStorageMonitor::IsStorageAvailable(Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStorageAvailable;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
