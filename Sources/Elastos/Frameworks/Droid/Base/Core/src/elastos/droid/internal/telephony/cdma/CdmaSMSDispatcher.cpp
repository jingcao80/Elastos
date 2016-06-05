#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaSMSDispatcher.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                          CdmaSMSDispatcher
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSMSDispatcher, /*TODO SMSDispatcher*/ Object, ICdmaSMSDispatcher);

const String CdmaSMSDispatcher::TAG("CdmaSMSDispatcher");
const Boolean CdmaSMSDispatcher::VDBG = FALSE;

CdmaSMSDispatcher::CdmaSMSDispatcher(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ISmsUsageMonitor* usageMonitor,
    /* [in] */ IImsSMSDispatcher* imsSMSDispatcher)
{
    // ==================before translated======================
    // super(phone, usageMonitor, imsSMSDispatcher);
    // Rlog.d(TAG, "CdmaSMSDispatcher created");
}

ECode CdmaSMSDispatcher::SendStatusReportMessage(
    /* [in] */ ISmsMessage* sms)
{
    VALIDATE_NOT_NULL(sms);
    // ==================before translated======================
    // if (VDBG) Rlog.d(TAG, "sending EVENT_HANDLE_STATUS_REPORT message");
    // sendMessage(obtainMessage(EVENT_HANDLE_STATUS_REPORT, sms));
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::HandleCdmaStatusReport(
    /* [in] */ ISmsMessage* sms)
{
    VALIDATE_NOT_NULL(sms);
    // ==================before translated======================
    // for (int i = 0, count = deliveryPendingList.size(); i < count; i++) {
    //     SmsTracker tracker = deliveryPendingList.get(i);
    //     if (tracker.mMessageRef == sms.mMessageRef) {
    //         // Found it.  Remove from list and broadcast.
    //         deliveryPendingList.remove(i);
    //         // Update the message status (COMPLETE)
    //         tracker.updateSentMessageStatus(mContext, Sms.STATUS_COMPLETE);
    //
    //         PendingIntent intent = tracker.mDeliveryIntent;
    //         Intent fillIn = new Intent();
    //         fillIn.putExtra("pdu", sms.getPdu());
    //         fillIn.putExtra("format", getFormat());
    //         try {
    //             intent.send(mContext, Activity.RESULT_OK, fillIn);
    //         } catch (CanceledException ex) {}
    //         break;  // Only expect to see one tracker matching this message.
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return SmsConstants.FORMAT_3GPP2;
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::HandleStatusReport(
    /* [in] */ IInterface* o)
{
    // ==================before translated======================
    // if (o instanceof SmsMessage) {
    //     if (VDBG) Rlog.d(TAG, "calling handleCdmaStatusReport()");
    //     handleCdmaStatusReport((SmsMessage) o);
    // } else {
    //     Rlog.e(TAG, "handleStatusReport() called for object type " + o.getClass().getName());
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendData(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    // ==================before translated======================
    // SmsMessage.SubmitPdu pdu = SmsMessage.getSubmitPdu(
    //         scAddr, destAddr, destPort, origPort, data, (deliveryIntent != null));
    // HashMap map = getSmsTrackerMap(destAddr, scAddr, destPort, origPort, data, pdu);
    // SmsTracker tracker = getSmsTracker(map, sentIntent, deliveryIntent, getFormat(),
    //         null /*messageUri*/, false);
    // sendSubmitPdu(tracker);
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendText(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& callingPkg,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    // ==================before translated======================
    // SmsMessage.SubmitPdu pdu = SmsMessage.getSubmitPdu(
    //         scAddr, destAddr, text, (deliveryIntent != null), null, priority);
    // if (pdu != null) {
    //     if (messageUri == null) {
    //         if (SmsApplication.shouldWriteMessageForPackage(callingPkg, mContext)) {
    //             messageUri = writeOutboxMessage(
    //                     getSubId(),
    //                     destAddr,
    //                     text,
    //                     deliveryIntent != null,
    //                     callingPkg);
    //         }
    //     } else {
    //         moveToOutbox(getSubId(), messageUri, callingPkg);
    //     }
    //     HashMap map = getSmsTrackerMap(destAddr, scAddr, text, pdu);
    //     SmsTracker tracker = getSmsTracker(map, sentIntent, deliveryIntent, getFormat(),
    //             messageUri, isExpectMore, validityPeriod);
    //     sendSubmitPdu(tracker);
    // } else {
    //     Rlog.e(TAG, "CdmaSMSDispatcher.sendText(): getSubmitPdu() returned null");
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    // ==================before translated======================
    // throw new IllegalStateException("This method must be called only on ImsSMSDispatcher");
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return SmsMessage.calculateLength(messageBody, use7bitOnly);
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendNewSubmitPdu(
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
    /* [in] */ IUri* messageUri)
{
    // ==================before translated======================
    // UserData uData = new UserData();
    // uData.payloadStr = message;
    // uData.userDataHeader = smsHeader;
    // if (encoding == SmsConstants.ENCODING_7BIT) {
    //     uData.msgEncoding = UserData.ENCODING_GSM_7BIT_ALPHABET;
    //     Context context = mPhone.getContext();
    //     boolean ascii7bitForLongMsg = context.getResources().
    //         getBoolean(com.android.internal.R.bool.config_ascii_7bit_support_for_long_message);
    //     if (ascii7bitForLongMsg) {
    //         Rlog.d(TAG, "ascii7bitForLongMsg = " + ascii7bitForLongMsg);
    //         uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
    //     }
    // } else { // assume UTF-16
    //     uData.msgEncoding = UserData.ENCODING_UNICODE_16;
    // }
    // uData.msgEncodingSet = true;
    //
    // /* By setting the statusReportRequested bit only for the
    //  * last message fragment, this will result in only one
    //  * callback to the sender when that last fragment delivery
    //  * has been acknowledged. */
    // SmsMessage.SubmitPdu submitPdu = SmsMessage.getSubmitPdu(destinationAddress,
    //         uData, (deliveryIntent != null) && lastPart, priority);
    //
    // HashMap map = getSmsTrackerMap(destinationAddress, scAddress,
    //         message, submitPdu);
    // SmsTracker tracker = getSmsTracker(map, sentIntent,
    //         deliveryIntent, getFormat(), unsentPartCount, anyPartFailed, messageUri, smsHeader,
    //         (!lastPart || isExpectMore), validityPeriod);
    // sendSubmitPdu(tracker);
    assert(0);
    return NOERROR;
}

void CdmaSMSDispatcher::SendSubmitPdu(
    /* [in] */ /*TODO SMSDispatcher::SmsTracker*/IInterface* tracker)
{
    // ==================before translated======================
    // if (SystemProperties.getBoolean(TelephonyProperties.PROPERTY_INECM_MODE, false)) {
    //     if (VDBG) {
    //         Rlog.d(TAG, "Block SMS in Emergency Callback mode");
    //     }
    //     tracker.onFailed(mContext, SmsManager.RESULT_ERROR_NO_SERVICE, 0/*errorCode*/);
    //     return;
    // }
    // sendRawPdu(tracker);
    assert(0);
}

ECode CdmaSMSDispatcher::SendSms(
    /* [in] */ /*TODO SMSDispatcher::SmsTracker*/IInterface* tracker)
{
    // ==================before translated======================
    // HashMap<String, Object> map = tracker.mData;
    //
    // // byte[] smsc = (byte[]) map.get("smsc");  // unused for CDMA
    // byte[] pdu = (byte[]) map.get("pdu");
    //
    // Rlog.d(TAG, "sendSms: "
    //         + " isIms()=" + isIms()
    //         + " mRetryCount=" + tracker.mRetryCount
    //         + " mImsRetry=" + tracker.mImsRetry
    //         + " mMessageRef=" + tracker.mMessageRef
    //         + " SS=" + mPhone.getServiceState().getState());
    //
    // // Send SMS via the carrier app.
    // BroadcastReceiver resultReceiver = new SMSDispatcherReceiver(tracker);
    //
    // // Direct the intent to only the default carrier app.
    // Intent intent = new Intent(Intents.SMS_SEND_ACTION);
    // String carrierPackage = getCarrierAppPackageName(intent);
    // if (carrierPackage != null) {
    //     intent.setPackage(getCarrierAppPackageName(intent));
    //     intent.putExtra("pdu", pdu);
    //     intent.putExtra("format", getFormat());
    //     if (tracker.mSmsHeader != null && tracker.mSmsHeader.concatRef != null) {
    //         SmsHeader.ConcatRef concatRef = tracker.mSmsHeader.concatRef;
    //         intent.putExtra("concat.refNumber", concatRef.refNumber);
    //         intent.putExtra("concat.seqNumber", concatRef.seqNumber);
    //         intent.putExtra("concat.msgCount", concatRef.msgCount);
    //     }
    //     intent.addFlags(Intent.FLAG_RECEIVER_NO_ABORT);
    //     Rlog.d(TAG, "Sending SMS by carrier app.");
    //     mContext.sendOrderedBroadcast(intent, android.Manifest.permission.RECEIVE_SMS,
    //                                   AppOpsManager.OP_RECEIVE_SMS, resultReceiver,
    //                                   null, Activity.RESULT_CANCELED, null, null);
    // } else {
    //     sendSmsByPstn(tracker);
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    // ==================before translated======================
    // // This function should be defined in ImsDispatcher.
    // Rlog.e(TAG, "updateSmsSendStatus should never be called from here!");
    assert(0);
    return NOERROR;
}

ECode CdmaSMSDispatcher::SendSmsByPstn(
    /* [in] */ /*TODO SMSDispatcher::SmsTracker*/IInterface* tracker)
{
    // ==================before translated======================
    // int ss = mPhone.getServiceState().getState();
    // // if sms over IMS is not supported on data and voice is not available...
    // if (!isIms() && ss != ServiceState.STATE_IN_SERVICE) {
    //     tracker.onFailed(mContext, getNotInServiceError(ss), 0/*errorCode*/);
    //     return;
    // }
    //
    // Message reply = obtainMessage(EVENT_SEND_SMS_COMPLETE, tracker);
    // byte[] pdu = (byte[]) tracker.mData.get("pdu");
    //
    // // sms over cdma is used:
    // //   if sms over IMS is not supported AND
    // //   this is not a retry case after sms over IMS failed
    // //     indicated by mImsRetry > 0
    // if (0 == tracker.mImsRetry && !isIms()) {
    //     mCi.sendCdmaSms(pdu, reply);
    // }
    // // If sending SMS over IMS is not enabled, send SMS over cdma. Simply
    // // calling shouldSendSmsOverIms() to check for that here might yield a
    // // different result if the conditions of UE being attached to eHRPD and
    // // active 1x voice call have changed since we last called it in
    // // ImsSMSDispatcher.isCdmaMo()
    // else if (!mImsSMSDispatcher.isImsSmsEnabled()) {
    //     mCi.sendCdmaSms(pdu, reply);
    //     mImsSMSDispatcher.enableSendSmsOverIms(true);
    // }
    // else {
    //     mCi.sendImsCdmaSms(pdu, tracker.mImsRetry, tracker.mMessageRef, reply);
    //     // increment it here, so in case of SMS_FAIL_RETRY over IMS
    //     // next retry will be sent using IMS request again.
    //     tracker.mImsRetry++;
    // }
    assert(0);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
