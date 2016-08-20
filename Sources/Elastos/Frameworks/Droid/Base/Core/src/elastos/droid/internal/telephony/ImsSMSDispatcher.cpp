
#include "elastos/droid/internal/telephony/ImsSMSDispatcher.h"
#include "elastos/droid/internal/telephony/CSmsBroadcastUndelivered.h"
#include "elastos/droid/internal/telephony/SmsMessageBase.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaSMSDispatcher.h"
#include "elastos/droid/internal/telephony/cdma/CdmaInboundSmsHandler.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSMSDispatcher.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/GsmInboundSmsHandler.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include <elastos/droid/R.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::Cdma::CCdmaSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Cdma::CdmaInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Gsm::CGsmSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Gsm::GsmInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::SmsMessageMessageClass;
using Elastos::Droid::Telephony::SmsMessage_MessageClass_CLASS_1;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::CThread;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(ImsSMSDispatcher, SMSDispatcher, IImsSMSDispatcher);

const String ImsSMSDispatcher::TAG("RIL_ImsSms");
ImsSMSDispatcher::ImsSMSDispatcher(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ SmsStorageMonitor* storageMonitor,
    /* [in] */ SmsUsageMonitor* usageMonitor)
    : mIms(FALSE)
    , mImsSmsFormat(ISmsConstants::FORMAT_UNKNOWN)
    , mImsSmsEnabled(TRUE)
{
    SMSDispatcher::constructor(phone, usageMonitor, NULL);
    Logger::D(TAG, "ImsSMSDispatcher created");

    // Create dispatchers, inbound SMS handlers and
    // broadcast undelivered messages in raw table.
    //TODO attention the circular reference here
    CCdmaSMSDispatcher::New(phone, (ISmsUsageMonitor*)usageMonitor
            , IImsSMSDispatcher::Probe(this), (ISMSDispatcher**)&mCdmaDispatcher);
    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    GsmInboundSmsHandler::MakeInboundSmsHandler(ctx,
            storageMonitor, phone, (IGsmInboundSmsHandler**)&mGsmInboundSmsHandler);
    mCdmaInboundSmsHandler = CdmaInboundSmsHandler::MakeInboundSmsHandler(ctx,
            storageMonitor, phone, ICdmaSMSDispatcher::Probe(mCdmaDispatcher));
    CGsmSMSDispatcher::New(phone, (ISmsUsageMonitor*)usageMonitor
            , IImsSMSDispatcher::Probe(this), mGsmInboundSmsHandler
                , (ISMSDispatcher**)&mGsmDispatcher);
    AutoPtr<IThread> broadcastThread;
    AutoPtr<IRunnable> run;
    CSmsBroadcastUndelivered::New(ctx, mGsmInboundSmsHandler, mCdmaInboundSmsHandler, (IRunnable**)&run);
    CThread::New(run, (IThread**)&broadcastThread);
    broadcastThread->Start();

    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCi->RegisterForImsNetworkStateChanged(this, EVENT_IMS_STATE_CHANGED, NULL);
}

void ImsSMSDispatcher::UpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    Logger::D(TAG, "In IMS updatePhoneObject ");
    SMSDispatcher::UpdatePhoneObject(phone);
    ((SMSDispatcher*)mCdmaDispatcher.Get())->UpdatePhoneObject(phone);
    ((SMSDispatcher*)mGsmDispatcher.Get())->UpdatePhoneObject(phone);
    ((GsmInboundSmsHandler*)mGsmInboundSmsHandler.Get())->UpdatePhoneObject(phone);
    ((CdmaInboundSmsHandler*)mCdmaInboundSmsHandler.Get())->UpdatePhoneObject(phone);
}

ECode ImsSMSDispatcher::Dispose()
{
    mCi->UnregisterForOn(this);
    mCi->UnregisterForImsNetworkStateChanged(this);
    mGsmDispatcher->Dispose();
    mCdmaDispatcher->Dispose();
    ((GsmInboundSmsHandler*)mGsmInboundSmsHandler.Get())->Dispose();
    ((CdmaInboundSmsHandler*)mCdmaInboundSmsHandler.Get())->Dispose();
    return NOERROR;
}

/**
 * Handles events coming from the phone stack. Overridden from handler.
 *
 * @param msg the message to handle
 */
// @Override
ECode ImsSMSDispatcher::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;

    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_RADIO_ON:
        case EVENT_IMS_STATE_CHANGED: {// received unsol
            AutoPtr<IMessage> m;
            ObtainMessage(EVENT_IMS_STATE_DONE, (IMessage**)&m);
            mCi->GetImsRegistrationState(m);
            break;
        }

        case EVENT_IMS_STATE_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*) IAsyncResult::Probe(obj);

            if (ar->mException == NULL) {
                UpdateImsInfo(ar);
            }
            else {
                Logger::E(TAG, "IMS State query failed with exp %s", TO_CSTR(ar->mException));
            }
            break;
        }

        default: {
            SMSDispatcher::HandleMessage(msg);
        }
    }
    return NOERROR;
}

void ImsSMSDispatcher::SetImsSmsFormat(
    /* [in] */ Int32 format)
{
    // valid format?
    switch (format) {
        case IPhoneConstants::PHONE_TYPE_GSM:
            mImsSmsFormat = String("3gpp");
            break;
        case IPhoneConstants::PHONE_TYPE_CDMA:
            mImsSmsFormat = String("3gpp2");
            break;
        default:
            mImsSmsFormat = String("unknown");
            break;
    }
}

void ImsSMSDispatcher::UpdateImsInfo(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > responseArray = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&(*responseArray)[i]);
    }

    mIms = FALSE;
    if ((*responseArray)[0] == 1) {  // IMS is registered
        Logger::D(TAG, "IMS is registered!");
        mIms = TRUE;
    }
    else {
        Logger::D(TAG, "IMS is NOT registered!");
    }

    SetImsSmsFormat((*responseArray)[1]);

    if (String("unknown").Equals(mImsSmsFormat)) {
        Logger::E(TAG, "IMS format was unknown!");
        // failed to retrieve valid IMS SMS format info, set IMS to unregistered
        mIms = FALSE;
    }
}

// @Override
ECode ImsSMSDispatcher::SendData(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    if (IsCdmaMo()) {
        ((SMSDispatcher*)mCdmaDispatcher.Get())->SendData(destAddr, scAddr, destPort, origPort,
                data, sentIntent, deliveryIntent);
    }
    else {
        ((SMSDispatcher*)mGsmDispatcher.Get())->SendData(destAddr, scAddr, destPort, origPort,
                data, sentIntent, deliveryIntent);
    }
    return NOERROR;
}

// @Override
void ImsSMSDispatcher::SendMultipartText(
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IArrayList/*<String>*/* parts,
    /* [in] */ IArrayList/*<PendingIntent>*/* sentIntents,
    /* [in] */ IArrayList/*<PendingIntent>*/* deliveryIntents,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& callingPkg,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    if (IsCdmaMo()) {
        ((SMSDispatcher*)mCdmaDispatcher.Get())->SendMultipartText(destAddr, scAddr,
                parts, sentIntents, deliveryIntents, messageUri, callingPkg,
                priority, isExpectMore, validityPeriod);
    }
    else {
        ((SMSDispatcher*)mGsmDispatcher.Get())->SendMultipartText(destAddr, scAddr,
                parts, sentIntents, deliveryIntents, messageUri, callingPkg,
                priority, isExpectMore, validityPeriod);
    }
}

// @Override
void ImsSMSDispatcher::SendSms(
    /* [in] */ SmsTracker* tracker)
{
    //  sendSms is a helper function to other send functions, sendText/Data...
    //  it is not part of ISms.stub
    Logger::E(TAG, "sendSms should never be called from here!");
}

// @Override
void ImsSMSDispatcher::SendSmsByPstn(
    /* [in] */ SmsTracker* tracker)
{
    // This function should be defined in Gsm/CdmaDispatcher.
    Logger::E(TAG, "sendSmsByPstn should never be called from here!");
}

// @Override
void ImsSMSDispatcher::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    if (IsCdmaMo()) {
        UpdateSmsSendStatusHelper(messageRef, ((SMSDispatcher*)mCdmaDispatcher.Get())->mSendPendingList,
                                  ((SMSDispatcher*)mCdmaDispatcher.Get()), success);
        UpdateSmsSendStatusHelper(messageRef, ((SMSDispatcher*)mGsmDispatcher.Get())->mSendPendingList,
                                  NULL, success);
    }
    else {
        UpdateSmsSendStatusHelper(messageRef, ((SMSDispatcher*)mGsmDispatcher.Get())->mSendPendingList,
                                  ((SMSDispatcher*)mGsmDispatcher.Get()), success);
        UpdateSmsSendStatusHelper(messageRef, ((SMSDispatcher*)mCdmaDispatcher.Get())->mSendPendingList,
                                  NULL, success);
    }
}

/**
 * Find a tracker in a list to update its status. If the status is successful,
 * send an EVENT_SEND_SMS_COMPLETE message. Otherwise, resend the message by PSTN if
 * feasible.
 *
 * @param messageRef the reference number of the tracker.
 * @param sendPendingList the list of trackers to look into.
 * @param smsDispatcher the dispatcher for resending the message by PSTN.
 * @param success TRUE iff the message was sent successfully.
 */
void ImsSMSDispatcher::UpdateSmsSendStatusHelper(
    /* [in] */ Int32 messageRef,
    /* [in] */ IList/*<SmsTracker>*/* sendPendingList,
    /* [in] */ SMSDispatcher* smsDispatcher,
    /* [in] */ Boolean success)
{
    AutoLock lock(sendPendingList);
    Int32 len = 0;
    sendPendingList->GetSize(&len);
    for (Int32 i = 0, count = len; i < count; i++) {
        AutoPtr<IInterface> o;
        sendPendingList->Get(i, (IInterface**)&o);
        SmsTracker* tracker = (SmsTracker*)IObject::Probe(o);
        if (tracker->mMessageRef == messageRef) {
            // Found it.  Remove from list and broadcast.
            sendPendingList->Remove(i);
            if (success) {
                Logger::D(TAG, "Sending SMS by IP succeeded.");
                AutoPtr<AsyncResult> ar = new AsyncResult(tracker->Probe(EIID_IInterface), NULL, NULL);
                AutoPtr<IMessage> m;
                ObtainMessage(EVENT_SEND_SMS_COMPLETE, ar->Probe(EIID_IInterface), (IMessage**)&m);
                Boolean tmp = FALSE;
                SendMessage(m, &tmp);
            }
            else {
                Logger::D(TAG, "Sending SMS by IP failed.");
                if (smsDispatcher != NULL) {
                    smsDispatcher->SendSmsByPstn(tracker);
                }
                else {
                    Logger::E(TAG, "No feasible way to send this SMS.");
                }
            }
            // Only expect to see one tracker matching this messageref.
            break;
        }
    }
}

// @Override
ECode ImsSMSDispatcher::SendText(
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
    Logger::D(TAG, "sendText");
    if (IsCdmaMo()) {
        ((SMSDispatcher*)mCdmaDispatcher.Get())->SendText(destAddr, scAddr,
                text, sentIntent, deliveryIntent, messageUri, callingPkg,
                priority, isExpectMore, validityPeriod);
    }
    else {
        ((SMSDispatcher*)mGsmDispatcher.Get())->SendText(destAddr, scAddr,
                text, sentIntent, deliveryIntent, messageUri, callingPkg,
                priority, isExpectMore, validityPeriod);
    }
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    Logger::D(TAG, "ImsSMSDispatcher:injectSmsPdu");
    ECode ec = NOERROR;
    do {
        // TODO We need to decide whether we should allow injecting GSM(3gpp)
        // SMS pdus when the phone is camping on CDMA(3gpp2) network and vice versa.
        AutoPtr<ISmsMessage> msg = Elastos::Droid::Telephony::SmsMessage::CreateFromPdu(pdu, format);
        if (msg == NULL) {
            ec = E_NULL_POINTER_EXCEPTION;
            break;
        }

        // Only class 1 SMS are allowed to be injected.
        SmsMessageMessageClass mc;
        msg->GetMessageClass(&mc);
        if (mc != SmsMessage_MessageClass_CLASS_1) {
            if (receivedIntent != NULL) {
                ec = receivedIntent->Send(ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR);
                if (FAILED(ec)) {
                    break;
                }
            }
            return NOERROR;
        }

        AutoPtr<AsyncResult> ar = new AsyncResult(receivedIntent, msg, NULL);

        if (format.Equals(ISmsConstants::FORMAT_3GPP)) {
            Logger::I(TAG, "ImsSMSDispatcher:injectSmsText Sending msg=%s, format=%sto mGsmInboundSmsHandler"
                , TO_CSTR(msg), format.string());
            IStateMachine::Probe(mGsmInboundSmsHandler)->SendMessage(IInboundSmsHandler::EVENT_INJECT_SMS
                    , ar->Probe(EIID_IInterface));
        }
        else if (format.Equals(ISmsConstants::FORMAT_3GPP2)) {
            Logger::I(TAG, "ImsSMSDispatcher:injectSmsText Sending msg=%s, format=%sto mCdmaInboundSmsHandler"
                , TO_CSTR(msg), format.string());
            IStateMachine::Probe(mCdmaInboundSmsHandler)->SendMessage(IInboundSmsHandler::EVENT_INJECT_SMS
                    , ar->Probe(EIID_IInterface));
        }
        else {
            // Invalid pdu format.
            Logger::E(TAG, "Invalid pdu format: %s", format.string());
            if (receivedIntent != NULL) {
                ec = receivedIntent->Send(ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR);
                if (FAILED(ec)) {
                    break;
                }
            }
        }
    } while (0);
    if (FAILED(ec)) {
        Logger::E(TAG, "injectSmsPdu failed: ");
        // try {
        if (receivedIntent != NULL) {
            receivedIntent->Send(ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR);
        }
        // } catch (CanceledException ex) {}
    }
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::SendRetrySms(
    /* [in] */ SmsTracker* tracker)
{
    String oldFormat = tracker->mFormat;

    // newFormat will be based on voice technology
    Int32 type = 0;
    IPhone::Probe(mPhone)->GetPhoneType(&type);
    String newFormat;
    if (IPhoneConstants::PHONE_TYPE_CDMA == type) {
        ((SMSDispatcher*)mCdmaDispatcher.Get())->GetFormat(&newFormat);
    }
    else {
        ((SMSDispatcher*)mGsmDispatcher.Get())->GetFormat(&newFormat);
    }

    // was previously sent sms format match with voice tech?
    if (oldFormat.Equals(newFormat)) {
        if (IsCdmaFormat(newFormat)) {
            Logger::D(TAG, "old format matched new format (cdma)");
            Boolean tmp = FALSE;
            ShouldSendSmsOverIms(&tmp);
            ((SMSDispatcher*)mCdmaDispatcher.Get())->SendSms(tracker);
            return NOERROR;
        }
        else {
            Logger::D(TAG, "old format matched new format (gsm)");
            ((SMSDispatcher*)mGsmDispatcher.Get())->SendSms(tracker);
            return NOERROR;
        }
    }

    // format didn't match, need to re-encode.
    AutoPtr<IHashMap> map = tracker->mData;

    // to re-encode, fields needed are:  scAddr, destAddr, and
    //   text if originally sent as sendText or
    //   data and destPort if originally sent as sendData.
    Boolean scAddrFlag, destAddrFlag, textFlag, dataFlag, destPortFlag;
    map->ContainsKey(CoreUtils::Convert("scAddr"), &scAddrFlag);
    map->ContainsKey(CoreUtils::Convert("destAddr"), &destAddrFlag);
    map->ContainsKey(CoreUtils::Convert("text"), &textFlag);
    map->ContainsKey(CoreUtils::Convert("data"), &dataFlag);
    map->ContainsKey(CoreUtils::Convert("destPort"), &destPortFlag);
    if (!(scAddrFlag && destAddrFlag &&
           (textFlag || (dataFlag && destPortFlag)))) {
        // should never come here...
        Logger::E(TAG, "sendRetrySms failed to re-encode per missing fields!");
        tracker->OnFailed(mContext, ISmsManager::RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    map->Get(CoreUtils::Convert("scAddr"), (IInterface**)&obj);
    String scAddr = TO_STR(obj);
    obj = NULL;
    map->Get(CoreUtils::Convert("destAddr"), (IInterface**)&obj);
    String destAddr = TO_STR(obj);

    AutoPtr<SmsMessageBase::SubmitPduBase> pdu;
    //    figure out from tracker if this was sendText/Data
    if (textFlag) {
        Logger::D(TAG, "sms failed was text");
        obj = NULL;
        map->Get(CoreUtils::Convert("text"), (IInterface**)&obj);
        String text = TO_STR(obj);

        if (IsCdmaFormat(newFormat)) {
            Logger::D(TAG, "old format (gsm) ==> new format (cdma)");
            pdu = Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(
                    scAddr, destAddr, text, (tracker->mDeliveryIntent != NULL), NULL);
            Boolean tmp = FALSE;
            ShouldSendSmsOverIms(&tmp);
        }
        else {
            Logger::D(TAG, "old format (cdma) ==> new format (gsm)");
            AutoPtr<ISmsMessageSubmitPdu> ssp;
            Elastos::Droid::Internal::Telephony::Gsm::CSmsMessage::GetSubmitPdu(
                    scAddr, destAddr, text, (tracker->mDeliveryIntent != NULL), (ArrayOf<Byte>*)NULL
                    , (ISmsMessageSubmitPdu**)&ssp);
            pdu = (SmsMessageBase::SubmitPduBase*)ISmsMessageBaseSubmitPduBase::Probe(ssp);
        }
    }
    else if (dataFlag) {
        Logger::D(TAG, "sms failed was data");
        obj = NULL;
        map->Get(CoreUtils::Convert("data"), (IInterface**)&obj);
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        Int32 len = 0;
        array->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> o;
            array->Get(i, (IInterface**)&o);
            IByte::Probe(o)->GetValue(&(*data)[i]);
        }

        obj = NULL;
        map->Get(CoreUtils::Convert("destPort"), (IInterface**)&obj);
        AutoPtr<IInteger32> destPort = IInteger32::Probe(obj);
        Int32 intValue = 0;
        destPort->GetValue(&intValue);

        if (IsCdmaFormat(newFormat)) {
            Logger::D(TAG, "old format (gsm) ==> new format (cdma)");
            pdu = Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(
                        scAddr, destAddr, intValue, data,
                        (tracker->mDeliveryIntent != NULL));
            Boolean tmp = FALSE;
            ShouldSendSmsOverIms(&tmp);
        }
        else {
            Logger::D(TAG, "old format (cdma) ==> new format (gsm)");
            AutoPtr<ISmsMessageSubmitPdu> ssp;
            Elastos::Droid::Internal::Telephony::Gsm::CSmsMessage::GetSubmitPdu(
                        scAddr, destAddr, intValue, data,
                        (tracker->mDeliveryIntent != NULL)
                        , (ISmsMessageSubmitPdu**)&ssp);
            pdu = (SmsMessageBase::SubmitPduBase*)ISmsMessageBaseSubmitPduBase::Probe(ssp);
        }
    }

    // replace old smsc and pdu with newly encoded ones
    map->Put(CoreUtils::Convert("smsc"), CoreUtils::ConvertByteArray(pdu->mEncodedScAddress));
    map->Put(CoreUtils::Convert("pdu"), CoreUtils::ConvertByteArray(pdu->mEncodedMessage));

    AutoPtr<SMSDispatcher> dispatcher = (IsCdmaFormat(newFormat)) ?
            (SMSDispatcher*)mCdmaDispatcher.Get() : (SMSDispatcher*)mGsmDispatcher.Get();

    dispatcher->GetFormat(&tracker->mFormat);
    dispatcher->SendSms(tracker);
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // this function should be defined in Gsm/CdmaDispatcher.
    Logger::E(TAG, "getFormat should never be called from here!");
    *result = String("unknown");
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Logger::E(TAG, "Error! Not implemented for IMS.");
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::SendNewSubmitPdu(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ const String& message,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Int32 format,
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
    Logger::E(TAG, "Error! Not implemented for IMS.");
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::IsIms(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIms;
    return NOERROR;
}

// @Override
ECode ImsSMSDispatcher::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImsSmsFormat;
    return NOERROR;
}

/**
 * Determines whether or not to use CDMA format for MO SMS.
 * If SMS over IMS is supported, then format is based on IMS SMS format,
 * otherwise format is based on current phone type.
 *
 * @return TRUE if Cdma format should be used for MO SMS, FALSE otherwise.
 */
Boolean ImsSMSDispatcher::IsCdmaMo()
{
    Boolean isIms = FALSE, tmp = FALSE;
    IsIms(&isIms);
    if (!isIms || (ShouldSendSmsOverIms(&tmp), !tmp)) {
        // Either IMS is not registered or there is an active 1x voice call
        // while on eHRPD, use Voice technology to determine SMS format.
        Int32 type = 0;
        IPhone::Probe(mPhone)->GetPhoneType(&type);
        return (IPhoneConstants::PHONE_TYPE_CDMA == type);
    }
    // IMS is registered with SMS support
    return IsCdmaFormat(mImsSmsFormat);
}

/**
 * Determines whether or not format given is CDMA format.
 *
 * @param format
 * @return TRUE if format given is CDMA format, FALSE otherwise.
 */
Boolean ImsSMSDispatcher::IsCdmaFormat(
    /* [in] */ const String& format)
{
    String f;
    ((SMSDispatcher*)mCdmaDispatcher.Get())->GetFormat(&f);
    return (f.Equals(format));
}

/**
 * Enables MO SMS over IMS
 *
 * @param enable
 */
ECode ImsSMSDispatcher::EnableSendSmsOverIms(
    /* [in] */ Boolean enable)
{
    mImsSmsEnabled = enable;
    return NOERROR;
}

/**
 * Determines whether MO SMS over IMS is currently enabled.
 *
 * @return TRUE if MO SMS over IMS is enabled, FALSE otherwise.
 */
ECode ImsSMSDispatcher::IsImsSmsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImsSmsEnabled;
    return NOERROR;
}

/**
 * Determines whether SMS should be sent over IMS if UE is attached to eHRPD
 * and there is an active voice call
 *
 * @return TRUE if SMS should be sent over IMS based on value in config.xml
 *         or system property FALSE otherwise
 */
ECode ImsSMSDispatcher::ShouldSendSmsOverIms(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean sendSmsOn1x = FALSE;
    res->GetBoolean(R::bool_::config_send_sms1x_on_voice_call, &sendSmsOn1x);
    Int32 currentCallState = 0;
    mTelephonyManager->GetCallState(&currentCallState);
    Int32 currentVoiceNetwork = 0;
    mTelephonyManager->GetVoiceNetworkType(&currentVoiceNetwork);
    Int32 currentDataNetwork = 0;
    mTelephonyManager->GetDataNetworkType(&currentDataNetwork);

    Logger::D(TAG, "data = %d voice = %d call state = %d"
            , currentDataNetwork, currentVoiceNetwork, currentCallState);

    if (sendSmsOn1x) {
        // The UE shall use 1xRTT for SMS if the UE is attached to an eHRPD
        // network and there is an active 1xRTT voice call.
        if (currentDataNetwork == ITelephonyManager::NETWORK_TYPE_EHRPD
                && currentVoiceNetwork == ITelephonyManager::NETWORK_TYPE_1xRTT
                && currentCallState != ITelephonyManager::CALL_STATE_IDLE) {
            EnableSendSmsOverIms(FALSE);
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
