#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSMSDispatcher.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/uicc/SIMRecords.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::SmsHeader;
using Elastos::Droid::Internal::Telephony::CSmsApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::ISIMRecords;
using Elastos::Droid::Internal::Telephony::Uicc::SIMRecords;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Manifest;
using Elastos::Droid::Provider::Telephony;
//using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CGsmSMSDispatcher::TAG("GsmSMSDispatcher");
const Boolean CGsmSMSDispatcher::VDBG;
const Int32 CGsmSMSDispatcher::EVENT_NEW_SMS_STATUS_REPORT;

CAR_INTERFACE_IMPL(CGsmSMSDispatcher, SMSDispatcher, IGsmSMSDispatcher)

CAR_OBJECT_IMPL(CGsmSMSDispatcher)

CGsmSMSDispatcher::CGsmSMSDispatcher()
{
    CAtomicReference::New((IAtomicReference**)&mIccRecords);
    CAtomicReference::New((IAtomicReference**)&mUiccApplication);
}

ECode CGsmSMSDispatcher::constructor()
{
    return NOERROR;
}

ECode CGsmSMSDispatcher::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ISmsUsageMonitor* usageMonitor,
    /* [in] */ IImsSMSDispatcher* imsSMSDispatcher,
    /* [in] */ IGsmInboundSmsHandler* gsmInboundSmsHandler)
{
    SMSDispatcher::constructor(phone, usageMonitor, imsSMSDispatcher);
    mCi->SetOnSmsStatus(this, EVENT_NEW_SMS_STATUS_REPORT, NULL);
    mCi->SetOnSmsOnSim(this, EVENT_SMS_ON_ICC, NULL);
    mGsmInboundSmsHandler = gsmInboundSmsHandler;
    mUiccController = UiccController::GetInstance();
    mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
    Logger::D(TAG, "GsmSMSDispatcher created");
    return NOERROR;
}

ECode CGsmSMSDispatcher::Dispose()
{
    SMSDispatcher::Dispose();
    mCi->UnSetOnSmsStatus(this);
    mUiccController->UnregisterForIccChanged(this);
    mCi->UnSetOnSmsOnSim(this);

    AutoPtr<IInterface> obj;
    mIccRecords->Get((IInterface**)&obj);
    AutoPtr<IIccRecords> rec = IIccRecords::Probe(obj);
    if (rec != NULL) {
        rec->UnregisterForNewSms(this);
    }
    return NOERROR;
}

ECode CGsmSMSDispatcher::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
    case EVENT_NEW_SMS_STATUS_REPORT:
        HandleStatusReport((AsyncResult*)(IObject*)obj.Get());
        break;

    case EVENT_NEW_ICC_SMS:
        // pass to InboundSmsHandler to process
        IStateMachine::Probe(mGsmInboundSmsHandler)->SendMessage(IInboundSmsHandler::EVENT_NEW_SMS, obj);
    break;

    case EVENT_ICC_CHANGED:
        OnUpdateIccAvailability();
        break;

    case EVENT_SMS_ON_ICC: {
        AutoPtr<IInterface> obj;
        mIccRecords->Get((IInterface**)&obj);
        AutoPtr<IIccRecords> rec = IIccRecords::Probe(obj);
        if (rec != NULL) {
            ISIMRecords* isimrecords = ISIMRecords::Probe(rec);
            SIMRecords* simrecords = (SIMRecords*)(isimrecords);
            simrecords->HandleSmsOnIcc((AsyncResult*)(IObject*)obj.Get());
        }
        break;
    }
    default:
        SMSDispatcher::HandleMessage(msg);
    }
    return NOERROR;
}

ECode CGsmSMSDispatcher::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISmsConstants::FORMAT_3GPP;
    return NOERROR;
}

ECode CGsmSMSDispatcher::SendData(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<ISmsMessageSubmitPdu> pdu;
    CSmsMessage::GetSubmitPdu(scAddr, destAddr, destPort, origPort, data, (deliveryIntent != NULL),
            (ISmsMessageSubmitPdu**)&pdu);
    if (pdu != NULL) {
        AutoPtr<IHashMap> map;
        map = GetSmsTrackerMap(destAddr, scAddr, destPort, origPort, data,
                (SmsMessageBase::SubmitPduBase*)pdu.Get());
        AutoPtr<SmsTracker> tracker;
        String str;
        GetFormat(&str);
        tracker = GetSmsTracker(map, sentIntent, deliveryIntent, str, NULL /*messageUri*/, FALSE);
        SendRawPdu(tracker);
    }
    else {
        Logger::E(TAG, "GsmSMSDispatcher.sendData(): getSubmitPdu() returned NULL");
    }
    return NOERROR;
}

ECode CGsmSMSDispatcher::SendText(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& callingPkg,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod )
{
    AutoPtr<ISmsMessageSubmitPdu> pdu;
    CSmsMessage::GetSubmitPdu(scAddr, destAddr, text, (deliveryIntent != NULL), validityPeriod,
            (ISmsMessageSubmitPdu**)&pdu);
    if (pdu != NULL) {
        if (messageUri == NULL) {
            Boolean b;
            AutoPtr<ISmsApplication> helper;
            CSmsApplication::AcquireSingleton((ISmsApplication**)&helper);
            if (helper->ShouldWriteMessageForPackage(callingPkg, mContext, &b), b) {
                messageUri = WriteOutboxMessage(
                        GetSubId(),
                        destAddr,
                        text,
                        deliveryIntent != NULL,
                        callingPkg);
            }
        }
        else {
            MoveToOutbox(GetSubId(), messageUri, callingPkg);
        }
        AutoPtr<IHashMap> map;
        map = GetSmsTrackerMap(destAddr, scAddr, text,
                (SmsMessageBase::SubmitPduBase*)pdu.Get());
        AutoPtr<SmsTracker> tracker;
        String str;
        GetFormat(&str);
        tracker = GetSmsTracker(map, sentIntent, deliveryIntent, str, messageUri, isExpectMore, validityPeriod);
        SendRawPdu(tracker);
    }
    else {
        Logger::E(TAG, "GsmSMSDispatcher.sendText(): getSubmitPdu() returned NULL");
    }
    return NOERROR;
}

ECode CGsmSMSDispatcher::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    // throw new IllegalStateException("This method must be called only on ImsSMSDispatcher");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CGsmSMSDispatcher::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    return CSmsMessage::CalculateLength(messageBody, use7bitOnly, result);
}

ECode CGsmSMSDispatcher::SendNewSubmitPdu(
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
    AutoPtr<ArrayOf<Byte> > bytes;
    SmsHeader::ToByteArray(smsHeader, (ArrayOf<Byte>**)&bytes);
    AutoPtr<ISmsMessageSubmitPdu> pdu;
// TODO: Need SmsHeader
    // CSmsMessage::GetSubmitPdu(scAddress, destinationAddress,
    //         message, deliveryIntent != NULL, bytes,
    //         encoding, smsHeader->mLanguageTable, smsHeader->mLanguageShiftTable, validityPeriod,
    //         (ISmsMessageSubmitPdu**)&pdu);

    if (pdu != NULL) {
        AutoPtr<IHashMap> map;
        map = GetSmsTrackerMap(destinationAddress, scAddress, message,
                (SmsMessageBase::SubmitPduBase*)pdu.Get());
        AutoPtr<SmsTracker> tracker;
        String str;
        GetFormat(&str);
        tracker = GetSmsTracker(map, sentIntent, deliveryIntent, str, unsentPartCount,
                anyPartFailed, messageUri, smsHeader, (!lastPart || isExpectMore), validityPeriod);
        SendRawPdu(tracker);
    }
    else {
        Logger::E(TAG, "GsmSMSDispatcher.sendNewSubmitPdu(): getSubmitPdu() returned NULL");
    }
    return NOERROR;
}

void CGsmSMSDispatcher::SendSms(
    /* [in] */ SmsTracker* tracker)
{
    AutoPtr<IHashMap> map = tracker->mData;

    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(String("smsc")), (IInterface**)&obj);
    AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);
    Int32 size;
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > smsc = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        iarray->Get(i, (IInterface**)&tmp);
        IByte::Probe(tmp)->GetValue(&(*smsc)[i]);
    }

    obj = NULL;
    map->Get(CoreUtils::Convert(String("pdu")), (IInterface**)&obj);
    iarray = NULL;
    iarray = IArrayOf::Probe(obj);
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        iarray->Get(i, (IInterface**)&tmp);
        IByte::Probe(tmp)->GetValue(&(*pdu)[i]);
    }

    if (tracker->mRetryCount > 0) {
        // Logger::D(TAG, "sendSms: "
        //         + " mRetryCount=" + tracker->mRetryCount
        //         + " mMessageRef=" + tracker->mMessageRef
        //         + " SS=" + mPhone.getServiceState().getState());

        // per TS 23.040 Section 9.2.3.6:  If TP-MTI SMS-SUBMIT (0x01) type
        //   TP-RD (bit 2) is 1 for retry
        //   and TP-MR is set to previously failed sms TP-MR
        if (((0x01 & (*pdu)[0]) == 0x01)) {
            (*pdu)[0] |= 0x04; // TP-RD
            (*pdu)[1] = (byte) tracker->mMessageRef; // TP-MR
        }
    }
    // Logger::D(TAG, "sendSms: "
    //         + " isIms()=" + isIms()
    //         + " mRetryCount=" + tracker->mRetryCount
    //         + " mImsRetry=" + tracker->mImsRetry
    //         + " mMessageRef=" + tracker->mMessageRef
    //         + " SS=" + mPhone.getServiceState().getState());

    // Send SMS via the carrier app.
    AutoPtr<SMSDispatcherReceiver> resultReceiver = new SMSDispatcherReceiver(tracker, this);

    AutoPtr<IIntent> intent;
    CIntent::New(String("android.provider.Telephony.SMS_SEND")/*IIntents::SMS_SEND_ACTION*/, (IIntent**)&intent);
    String carrierPackage = GetCarrierAppPackageName(intent);
    if (carrierPackage != NULL) {
        intent->SetPackage(carrierPackage);
        intent->PutByteArrayExtra(String("pdu"), pdu);
        intent->PutByteArrayExtra(String("smsc"), smsc);
        String str;
        intent->PutExtra(String("format"), (GetFormat(&str), str));
// TODO: Need SmsHeader
        // if (tracker->mSmsHeader != NULL && tracker->mSmsHeader->concatRef != NULL) {
        //     AutoPtr<SmsHeaderConcatRef> concatRef = tracker->mSmsHeader->concatRef;
        //     intent->PutExtra(String("concat.refNumber"), concatRef->refNumber);
        //     intent->PutExtra(String("concat.seqNumber"), concatRef->seqNumber);
        //     intent->PutExtra(String("concat.msgCount"), concatRef->msgCount);
        // }
        intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
        Logger::D(TAG, "Sending SMS by carrier app.");
        mContext->SendOrderedBroadcast(intent, Manifest::permission::RECEIVE_SMS,
                                      IAppOpsManager::OP_RECEIVE_SMS, resultReceiver,
                                      NULL, IActivity::RESULT_CANCELED, String(NULL), NULL);
    }
    else {
        SendSmsByPstn(tracker);
    }
}

void CGsmSMSDispatcher::SendSmsByPstn(
    /* [in] */ SmsTracker* tracker)
{
    AutoPtr<IServiceState> serviceState;
    IPhone::Probe(mPhone)->GetServiceState((IServiceState**)&serviceState);
    Int32 ss;
    serviceState->GetState(&ss);

    // if sms over IMS is not supported on data and voice is not available...
    Boolean b;
    if ((IsIms(&b), b) && ss != IServiceState::STATE_IN_SERVICE) {
        tracker->OnFailed(mContext, GetNotInServiceError(ss), 0/*errorCode*/);
        return;
    }

    AutoPtr<IHashMap> map = tracker->mData;

    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert(String("smsc")), (IInterface**)&obj);
    AutoPtr<IArrayOf> iarray = IArrayOf::Probe(obj);
    Int32 size;
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > smsc = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        iarray->Get(i, (IInterface**)&tmp);
        IByte::Probe(tmp)->GetValue(&(*smsc)[i]);
    }

    obj = NULL;
    map->Get(CoreUtils::Convert(String("pdu")), (IInterface**)&obj);
    iarray = NULL;
    iarray = IArrayOf::Probe(obj);
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        iarray->Get(i, (IInterface**)&tmp);
        IByte::Probe(tmp)->GetValue(&(*pdu)[i]);
    }

    AutoPtr<IMessage> reply;
    ObtainMessage(EVENT_SEND_SMS_COMPLETE, (IInterface*)(IObject*)tracker, (IMessage**)&reply);

    // sms over gsm is used:
    //   if sms over IMS is not supported AND
    //   this is not a retry case after sms over IMS failed
    //     indicated by mImsRetry > 0
    if (0 == tracker->mImsRetry && (IsIms(&b), !b)) {
        if (tracker->mRetryCount > 0) {
            // per TS 23.040 Section 9.2.3.6:  If TP-MTI SMS-SUBMIT (0x01) type
            //   TP-RD (bit 2) is 1 for retry
            //   and TP-MR is set to previously failed sms TP-MR
            if (((0x01 & (*pdu)[0]) == 0x01)) {
                (*pdu)[0] |= 0x04; // TP-RD
                (*pdu)[1] = (byte) tracker->mMessageRef; // TP-MR
            }
        }
        if (tracker->mRetryCount == 0 && tracker->mExpectMore) {
            mCi->SendSMSExpectMore(IccUtils::BytesToHexString(smsc),
                    IccUtils::BytesToHexString(pdu), reply);
        }
        else {
            mCi->SendSMS(IccUtils::BytesToHexString(smsc),
                    IccUtils::BytesToHexString(pdu), reply);
        }
    }
    else {
        mCi->SendImsGsmSms(IccUtils::BytesToHexString(smsc),
                IccUtils::BytesToHexString(pdu), tracker->mImsRetry,
                tracker->mMessageRef, reply);
        // increment it here, so in case of SMS_FAIL_RETRY over IMS
        // next retry will be sent using IMS request again.
        tracker->mImsRetry++;
    }
}

void CGsmSMSDispatcher::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    // This function should be defined in ImsDispatcher.
    Logger::E(TAG, "updateSmsSendStatus should never be called from here!");
}

AutoPtr<IUiccCardApplication> CGsmSMSDispatcher::GetUiccCardApplication()
{
    Int32 pid;
    Int64 sid;
    mPhone->GetSubId(&sid);
    mPhone->GetPhoneId(&pid);
    Logger::D(TAG, "GsmSMSDispatcher: subId = %ld slotId = %d", sid, pid);
    AutoPtr<IUiccCardApplication> app;
    mUiccController->GetUiccCardApplication(pid,
            IUiccController::APP_FAM_3GPP, (IUiccCardApplication**)&app);
    return app;
}

void CGsmSMSDispatcher::HandleStatusReport(
    /* [in] */ AsyncResult* ar)
{
    String pduString;
    ICharSequence::Probe(ar->mResult)->ToString(&pduString);
    AutoPtr<ISmsMessage> sms;
    CSmsMessage::NewFromCDS(pduString, (ISmsMessage**)&sms);

    if (sms != NULL) {
        Int32 tpStatus;
        ((SmsMessage*)sms.Get())->GetStatus(&tpStatus);
        Int32 messageRef = ((SmsMessage*)sms.Get())->mMessageRef;

        Int32 size;
        mDeliveryPendingList->GetSize(&size);
        for (Int32 i = 0, count = size; i < count; i++) {
            AutoPtr<IInterface> obj;
            mDeliveryPendingList->Get(i, (IInterface**)&obj);
            AutoPtr<SmsTracker> tracker = (SmsTracker*)(IObject*)obj.Get();
            if (tracker->mMessageRef == messageRef) {
                // Found it.  Remove from list and broadcast.
                if(tpStatus >= ITelephonyTextBasedSmsColumns::STATUS_FAILED || tpStatus < ITelephonyTextBasedSmsColumns::STATUS_PENDING ) {
                   mDeliveryPendingList->Remove(i);
                   // Update the message status (COMPLETE or FAILED)
                   tracker->UpdateSentMessageStatus(mContext, tpStatus);
                }
                AutoPtr<IPendingIntent> intent = tracker->mDeliveryIntent;
                AutoPtr<IIntent> fillIn;
                CIntent::New((IIntent**)&fillIn);
                fillIn->PutByteArrayExtra(String("pdu"), IccUtils::HexStringToBytes(pduString));
                String str;
                GetFormat(&str);
                fillIn->PutExtra(String("format"), str);
                // try {
                intent->Send(mContext, IActivity::RESULT_OK, fillIn);
                // } catch (CanceledException ex) {}

                // Only expect to see one tracker matching this messageref
                break;
            }
        }
    }
    mCi->AcknowledgeLastIncomingGsmSms(true, 1/*IIntents::RESULT_SMS_HANDLED*/, NULL);
}

void CGsmSMSDispatcher::OnUpdateIccAvailability()
{
    if (mUiccController == NULL ) {
        return;
    }

    AutoPtr<IUiccCardApplication> newUiccApplication = GetUiccCardApplication();

    AutoPtr<IInterface> obj;
    mUiccApplication->Get((IInterface**)&obj);
    AutoPtr<IUiccCardApplication> app = IUiccCardApplication::Probe(obj);
    if (app != newUiccApplication) {
        if (app != NULL) {
            Logger::D(TAG, "Removing stale icc objects.");
            obj = NULL;
            mIccRecords->Get((IInterface**)&obj);
            if (obj != NULL) {
                IIccRecords::Probe(obj)->UnregisterForNewSms(this);
            }
            mIccRecords->Set(NULL);
            mUiccApplication->Set(NULL);
        }
        if (newUiccApplication != NULL) {
            Logger::D(TAG, "New Uicc application found");
            mUiccApplication->Set(newUiccApplication);
            AutoPtr<IIccRecords> rec;
            newUiccApplication->GetIccRecords((IIccRecords**)&rec);
            mIccRecords->Set(rec);
            obj = NULL;
            mIccRecords->Get((IInterface**)&obj);
            if (obj != NULL) {
                IIccRecords::Probe(obj)->RegisterForNewSms(this, EVENT_NEW_ICC_SMS, NULL);
            }
        }
    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
