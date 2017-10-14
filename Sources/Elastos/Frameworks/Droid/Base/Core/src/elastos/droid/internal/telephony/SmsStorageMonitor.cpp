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

#include "elastos/droid/internal/telephony/SmsStorageMonitor.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

SmsStorageMonitor::SmsBroadcastReceiver::SmsBroadcastReceiver(
    /* [in] */ SmsStorageMonitor* host)
    : mHost(host)
{}

ECode SmsStorageMonitor::SmsBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
   if (action.Equals(IIntent::ACTION_DEVICE_STORAGE_FULL)) {
       mHost->mStorageAvailable = FALSE;
       AutoPtr<IMessage> msg;
       mHost->ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE, (IMessage**)&msg);
       mHost->mCi->ReportSmsMemoryStatus(FALSE, msg);
   }
   else if (action.Equals(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL)) {
       mHost->mStorageAvailable = TRUE;
       AutoPtr<IMessage> msg;
       mHost->ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE, (IMessage**)&msg);
       mHost->mCi->ReportSmsMemoryStatus(TRUE, msg);
   }
   return NOERROR;
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
    : mReportMemoryStatusPending(FALSE)
{
    mStorageAvailable = TRUE;
    mResultReceiver = new SmsBroadcastReceiver(this);
}

/**
 * Creates an SmsStorageMonitor and registers for events.
 * @param phone the Phone to use
 */
ECode SmsStorageMonitor::constructor(
    /* [in] */ IPhoneBase* phone)
{
    Handler::constructor();
    mPhone = phone;
    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    mCi = ((PhoneBase*)phone)->mCi;

    CreateWakelock();

    mCi->SetOnIccSmsFull(this, EVENT_ICC_FULL, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);

    // Register for device storage intents.  Use these to notify the RIL
    // that storage for SMS is or is not available.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_FULL);
    filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_NOT_FULL);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mResultReceiver, filter, (IIntent**)&i);
    return NOERROR;
}

ECode SmsStorageMonitor::Dispose()
{
    mCi->UnSetOnIccSmsFull(this);
    mCi->UnregisterForOn(this);
    return mContext->UnregisterReceiver(mResultReceiver);
}

/**
 * Handles events coming from the phone stack. Overridden from handler.
 * @param msg the message to handle
 */
//@Override
ECode SmsStorageMonitor::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
       case EVENT_ICC_FULL:
           HandleIccFull();
           break;

       case EVENT_REPORT_MEMORY_STATUS_DONE: {
           AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
           if (ar->mException != NULL) {
               mReportMemoryStatusPending = TRUE;
               Logger::V(TAG, "Memory status report to modem pending : mStorageAvailable = %d",
                         mStorageAvailable);
           }
           else {
               mReportMemoryStatusPending = FALSE;
           }
           break;
       }

       case EVENT_RADIO_ON: {
           if (mReportMemoryStatusPending) {
               Logger::V(TAG, "Sending pending memory status report : mStorageAvailable = %d",
                         mStorageAvailable);
               AutoPtr<IMessage> msg;
               ObtainMessage(EVENT_REPORT_MEMORY_STATUS_DONE, (IMessage**)&msg);
               mCi->ReportSmsMemoryStatus(mStorageAvailable, msg);
           }
           break;
        }
    }
    return NOERROR;
}

void SmsStorageMonitor::CreateWakelock()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("SmsStorageMonitor"),
            (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);
}

/**
 * Called when SIM_FULL message is received from the RIL.  Notifies interested
 * parties that SIM storage for SMS messages is full.
 */
void SmsStorageMonitor::HandleIccFull()
{
    // broadcast SIM_FULL intent
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::SIM_FULL_ACTION, (IIntent**)&intent);
    mWakeLock->AcquireLock(WAKE_LOCK_TIMEOUT);
    Int32 id = 0;
    mPhone->GetPhoneId(&id);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, id);
    mContext->SendBroadcast(intent, Manifest::permission::RECEIVE_SMS);
}

/** Returns whether or not there is storage available for an incoming SMS. */
ECode SmsStorageMonitor::IsStorageAvailable(
    /* [in] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStorageAvailable;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
