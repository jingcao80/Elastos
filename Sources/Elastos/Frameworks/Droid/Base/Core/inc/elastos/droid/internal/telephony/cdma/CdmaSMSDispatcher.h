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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASMSDISPATCHER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASMSDISPATCHER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/SMSDispatcher.h"

// import android.app.Activity;
// import android.app.AppOpsManager;
// import android.app.PendingIntent.CanceledException;
// import android.content.BroadcastReceiver;
// import android.content.Intent;
// import android.content.Context;
// import android.os.Message;
// import android.os.SystemProperties;
// import android.provider.Telephony.Sms;
// import android.provider.Telephony.Sms.Intents;
// import android.telephony.Rlog;
// import android.telephony.ServiceState;
// import android.telephony.SmsManager;
// import com.android.internal.telephony.SMSDispatcher;
// import com.android.internal.telephony.SmsApplication;
// import com.android.internal.telephony.SmsConstants;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.cdma.sms.UserData;
// import java.util.HashMap;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Internal::Telephony::ISmsUsageMonitor;
using Elastos::Droid::Internal::Telephony::IImsSMSDispatcher;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::SMSDispatcher;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class CdmaSMSDispatcher
    : public SMSDispatcher
    , public ICdmaSMSDispatcher
{
public:
    TO_STRING_IMPL("CdmaSMSDispatcher")

    CAR_INTERFACE_DECL();

    CdmaSMSDispatcher();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ISmsUsageMonitor* usageMonitor,
        /* [in] */ IImsSMSDispatcher* imsSMSDispatcher);

    /**
      * Send the SMS status report to the dispatcher thread to process.
      * @param sms the CDMA SMS message containing the status report
      */
    virtual CARAPI SendStatusReportMessage(
        /* [in] */ ISmsMessage* sms);

    /**
      * Called from parent class to handle status report from {@code CdmaInboundSmsHandler}.
      * @param sms the CDMA SMS message to process
      */
    virtual CARAPI HandleCdmaStatusReport(
        /* [in] */ ISmsMessage* sms);

protected:
    // @Override
    CARAPI GetFormat(
        /* [out] */ String* result);

    // @Override
    CARAPI HandleStatusReport(
        /* [in] */ IInterface* o);

    /** {@inheritDoc} */
    // @Override
    CARAPI SendData(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /** {@inheritDoc} */
    // @Override
    CARAPI SendText(
        /* [in] */ const String& destAddr,
        /* [in] */ const String& scAddr,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& callingPkg,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    /** {@inheritDoc} */
    // @Override
    CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    /** {@inheritDoc} */
    // @Override
    CARAPI CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

    /** {@inheritDoc} */
    // @Override
    CARAPI SendNewSubmitPdu(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ const String& message,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 encoding,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Boolean lastPart,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod,
        /* [in] */ IAtomicInteger32* unsentPartCount,
        /* [in] */ IAtomicBoolean* anyPartFailed,
        /* [in] */ IUri* messageUri);

    virtual CARAPI_(void) SendSubmitPdu(
        /* [in] */ SMSDispatcher::SmsTracker* tracker);

    /** {@inheritDoc} */
    // @Override
    CARAPI_(void) SendSms(
        /* [in] */ SMSDispatcher::SmsTracker* tracker);

    /** {@inheritDoc} */
    // @Override
    CARAPI_(void) UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    /** {@inheritDoc} */
    // @Override
    CARAPI_(void) SendSmsByPstn(
        /* [in] */ SMSDispatcher::SmsTracker* tracker);

private:
    static const String TAG;
    static const Boolean VDBG;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASMSDISPATCHER_H__
