#include "elastos/droid/internal/telephony/CSmsApplication.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/internal/telephony/CSyntheticSmsMessage.h"
#include "elastos/droid/internal/telephony/UiccSmsController.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CBinderHelper.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;

using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  UiccSmsController::MyBroadcastReceiver::
//==============================================================

UiccSmsController::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ UiccSmsController* host)
    : mHost(host)
{}

ECode UiccSmsController::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // check if the message was aborted
    Int32 code = 0;
    GetResultCode(&code);
    if (code != IActivity::RESULT_OK) {
        return NOERROR;
    }
    String destAddr;
    GetResultData(&destAddr);
    String scAddr;
    intent->GetStringExtra(String("scAddr"), &scAddr);
    Int64 subId = 0;
    intent->GetInt64Extra(String("subId"), mHost->GetDefaultSmsSubId(), &subId);
    String callingPackage;
    intent->GetStringExtra(String("callingPackage"), &callingPackage);
    AutoPtr<IArrayList> parts;
    intent->GetStringArrayListExtra(String("parts"), (IArrayList**)&parts);
    AutoPtr<IArrayList> sentIntents;
    intent->GetParcelableArrayListExtra(String("sentIntents"), (IArrayList**)&sentIntents);
    AutoPtr<IArrayList> deliveryIntents;
    intent->GetParcelableArrayListExtra(String("deliveryIntents"), (IArrayList**)&deliveryIntents);

    Int32 extra = 0;
    intent->GetInt32Extra(String("callingUid"), 0, &extra);
    if (extra != 0) {
        StringBuilder sb(callingPackage);
        sb += "\\"; sb += extra;
        callingPackage = sb.ToString();
    }

    Boolean bExtra = FALSE;
    intent->GetBooleanExtra(String("multipart"), FALSE, &bExtra);
    if (bExtra) {
        // if (Rlog::IsLoggable("SMS", Log.VERBOSE)) {
        //     Log("ProxiedMultiPartSms destAddr: " + destAddr +
        //             "\n scAddr= " + scAddr +
        //             "\n subId= " + subId +
        //             "\n callingPackage= " + callingPackage +
        //             "\n partsSize= " + parts->Size());
        // }
        AutoPtr<IIccSmsInterfaceManager> p = mHost->GetIccSmsInterfaceManager(subId);
        p->SendMultipartText(callingPackage, destAddr, scAddr, IList::Probe(parts),
                        IList::Probe(sentIntents), IList::Probe(deliveryIntents));
        return NOERROR;
    }

    AutoPtr<IPendingIntent> sentIntent;
    Int32 size = 0;
    if (sentIntents != NULL && (sentIntents->GetSize(&size), size) > 0) {
        AutoPtr<IInterface> p;
        sentIntents->Get(0, (IInterface**)&p);
        sentIntent = IPendingIntent::Probe(p);
    }
    AutoPtr<IPendingIntent> deliveryIntent;
    if (deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > 0) {
        AutoPtr<IInterface> p;
        deliveryIntents->Get(0, (IInterface**)&p);
        deliveryIntent = IPendingIntent::Probe(p);
    }
    String text(NULL);
    if (parts != NULL && (parts->GetSize(&size), size) > 0) {
        AutoPtr<IInterface> p;
        parts->Get(0, (IInterface**)&p);
        ICharSequence::Probe(p)->ToString(&text);
    }
    // if (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
    //     Log("ProxiedSms destAddr: " + destAddr +
    //             "\n scAddr=" + scAddr +
    //             "\n subId= " + subId +
    //             "\n callingPackage=" + callingPackage);
    // }
    AutoPtr<IIccSmsInterfaceManager> p = mHost->GetIccSmsInterfaceManager(subId);
    p->SendText(callingPackage, destAddr,
            scAddr, text, sentIntent, deliveryIntent);
    return NOERROR;
}

//==============================================================
//  UiccSmsController::
//==============================================================
const String UiccSmsController::LOGTAG("RIL_UiccSmsController");

const Int32 UiccSmsController::WAKE_LOCK_TIMEOUT = 5000;

CAR_INTERFACE_IMPL_2(UiccSmsController, Object, IISms, IUiccSmsController)

UiccSmsController::UiccSmsController()
{
    mReceiver = new MyBroadcastReceiver(this);

    CHandler::New((IHandler**)&mHandler);
}

ECode UiccSmsController::constructor(
    /* [in] */ ArrayOf<IPhone*>* phone,
    /* [in] */ IContext* context)
{
    mPhone = phone;
    mContext = context;

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> serv;
    sm->GetService(String("isms"), (IInterface**)&serv);
    if (serv == NULL) {
        sm->AddService(String("isms"), IISms::Probe(this));
    }

    CreateWakelock();
    return NOERROR;
}

void UiccSmsController::CreateWakelock()
{
    AutoPtr<IInterface> p;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&p);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(p);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("IccSmsInterfaceManager"), (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);
}

void UiccSmsController::DispatchPdus(
    /* [in] */ ArrayOf<ArrayOf<Byte>* >* pdus)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::SMS_DELIVER_ACTION, (IIntent**)&intent);
    // Direct the intent to only the default SMS app. if we can't find a default SMS app
    // then send it to all broadcast receivers.
    AutoPtr<ISmsApplication> application;
    CSmsApplication::AcquireSingleton((ISmsApplication**)&application);
    AutoPtr<IComponentName> componentName;
    application->GetDefaultSmsApplication(mContext, TRUE, (IComponentName**)&componentName);
    if (componentName == NULL)
        return;

    // if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
    //     Log("dispatchPdu pdus: " + pdus +
    //             "\n componentName=" + componentName +
    //             "\n format=" + SmsMessage.FORMAT_SYNTHETIC);
    // }

    // Deliver SMS message only to this receiver
    intent->SetComponent(componentName);
    //intent->PutParcelableArrayListExtra(String("pdus"), pdus);
    //NOTE: here we will convert the 2-D arrayof to 1-D arrayof
    //      every SMS will not more than 140 bytes,
    //      so Byte type will be ok for the length of the SMS
    //      below show how the convert work
    //      a 2-D arrayof have 3 element:
    //         XXXXXXXX|YYYYYYYYYYYY|ZZZZZZ
    //      convert to 1-D arrayof
    //         3 8 XXXXXXXX 12 YYYYYYYYYYYY 6 ZZZZZZ
    //      3 parts: the 1st part have 8 bytes, 2nd have 12 bytes, 3rd have 6 bytes
    Int32 arrayLen = pdus->GetLength();
    Int32 allByteLen = 0;
    for (Int32 i = 0; i < arrayLen; i++) {
        AutoPtr<ArrayOf<Byte> > ab = (*pdus)[i];
        Int32 len = ab->GetLength();
        allByteLen += len;
        //TODO debug log should remove
        Logger::E("leliang:UiccSmsController", "index: %d, len:%d", i, len);
        for (Int32 j = 0; j < len; ++j) {
            Logger::E("leliang:UiccSmsController", "    %d value: 0x%x", j, (*ab)[j]);
        }
    }
    AutoPtr<ArrayOf<Byte> > arrayBytes = ArrayOf<Byte>::Alloc(1 + arrayLen + allByteLen);
    Int32 pos = 0;
    arrayBytes->Set(pos++, (Byte)arrayLen);
    for (Int32 i = 0; i < arrayLen; i++) {
        AutoPtr<ArrayOf<Byte> > aob = (*pdus)[i];
        Int32 len = aob->GetLength();
        if (len > 160) {
            Logger::E("UiccSmsController", "wrong length of the SMS, should never be over 160");
            assert(0);
        }
        arrayBytes->Set(pos++, (Byte)len);
        memcpy(arrayBytes->GetPayload() + pos, aob->GetPayload(), len);
        pos += len;
    }
    intent->PutByteArrayExtra(String("pdus"), arrayBytes);// GetByteArrayExtra

    intent->PutExtra(String("format"), ISmsMessage::FORMAT_SYNTHETIC);
    Dispatch(intent, Manifest::permission::RECEIVE_SMS);

    intent->SetAction(ITelephonySmsIntents::SMS_RECEIVED_ACTION);
    intent->SetComponent(NULL);
    Dispatch(intent, Manifest::permission::RECEIVE_SMS);
}

void UiccSmsController::Dispatch(
    /* [in] */ IIntent* intent,
    /* [in] */ String permission)
{
    // Hold a wake lock for WAKE_LOCK_TIMEOUT seconds, enough to give any
    // receivers time to take their own wake locks.
    mWakeLock->AcquireLock(WAKE_LOCK_TIMEOUT);
    intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
    mContext->SendOrderedBroadcast(intent, permission, IAppOpsManager::OP_RECEIVE_SMS, NULL,
            mHandler, IActivity::RESULT_OK, String(NULL), NULL);
}

void UiccSmsController::BroadcastOutgoingSms(
    /* [in] */ Int64 subId,
    /* [in] */ String callingPackage,
    /* [in] */ String destAddr,
    /* [in] */ String scAddr,
    /* [in] */ Boolean multipart,
    /* [in] */ IArrayList* parts,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    AutoPtr<IIntent> broadcast;
    CIntent::New(IIntent::ACTION_NEW_OUTGOING_SMS, (IIntent**)&broadcast);
    broadcast->PutExtra(String("destAddr"), destAddr);
    broadcast->PutExtra(String("scAddr"), scAddr);
    broadcast->PutExtra(String("subId"), subId);
    broadcast->PutBooleanExtra(String("multipart"), multipart);
    broadcast->PutExtra(String("callingPackage"), callingPackage);
    AutoPtr<IBinderHelper> bhlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&bhlp);
    Int32 uid = 0;
    bhlp->GetCallingUid(&uid);
    broadcast->PutExtra(String("callingUid"), uid);
    broadcast->PutStringArrayListExtra(String("parts"), parts);
    broadcast->PutParcelableArrayListExtra(String("sentIntents"), sentIntents);
    broadcast->PutParcelableArrayListExtra(String("deliveryIntents"), deliveryIntents);
    broadcast->PutExtra(String("priority"), priority);
    broadcast->PutExtra(String("isExpectMore"), isExpectMore);
    broadcast->PutExtra(String("validityPeriod"), validityPeriod);

    // if (Rlog::IsLoggable("SMS", Log::VERBOSE)) {
    //     Log("Broadcasting sms destAddr: " + destAddr +
    //             "\n scAddr= " + scAddr +
    //             "\n subId= " + subId +
    //             "\n multipart= " + multipart +
    //             "\n callingPackager= " + callingPackage +
    //             "\n callingUid= " + android.os.Binder->GetCallingUid() +
    //             "\n parts= " + parts->Size() +
    //             "\n sentIntents= " + sentIntents->Size() +
    //             "\n deliveryIntents= " + deliveryIntents->Size() +
    //             "\n priority= " + priority +
    //             "\n isExpectMore= " + isExpectMore +
    //             "\n validityPeriod= " + validityPeriod);
    // }
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> owner;
    uhhlp->GetOWNER((IUserHandle**)&owner);
    mContext->SendOrderedBroadcastAsUser(broadcast, owner,
            Manifest::permission::INTERCEPT_SMS,
            mReceiver, NULL, IActivity::RESULT_OK, destAddr, NULL);
}

ECode UiccSmsController::UpdateMessageOnIccEf(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 index,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return UpdateMessageOnIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage,
            index, status, pdu, result);
}

ECode UiccSmsController::UpdateMessageOnIccEfForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 index,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        return iccSmsIntMgr->UpdateMessageOnIccEf(callingPackage, index, status, pdu, result);
    }
    else {
        // Rlog::E(LOG_TAG, "updateMessageOnIccEf iccSmsIntMgr is NULL" +
        //               " for Subscription: " + subId);
        *result = FALSE;
        return NOERROR;
    }
}

ECode UiccSmsController::CopyMessageToIccEf(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return CopyMessageToIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage, status,
            pdu, smsc, result);
}

ECode UiccSmsController::CopyMessageToIccEfForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        return iccSmsIntMgr->CopyMessageToIccEf(callingPackage, status, pdu, smsc, result);
    }
    else {
        // Rlog::E(LOG_TAG, "copyMessageToIccEf iccSmsIntMgr is NULL" +
        //               " for Subscription: " + subId);
        *result = FALSE;
        return NOERROR;
    }
}

ECode UiccSmsController::GetAllMessagesFromIccEf(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return GetAllMessagesFromIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage, result);
}

ECode UiccSmsController::GetAllMessagesFromIccEfForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        return iccSmsIntMgr->GetAllMessagesFromIccEf(callingPackage, result);
    }
    else {
        // Rlog::E(LOG_TAG,"getAllMessagesFromIccEf iccSmsIntMgr is" +
        //               " NULL for Subscription: " + subId);
        *result = NULL;
        return NOERROR;
    }
}

ECode UiccSmsController::SendData(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return SendDataForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr, scAddr,
            destPort, data, sentIntent, deliveryIntent);
}

ECode UiccSmsController::SendDataForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        iccSmsIntMgr->SendData(callingPackage, destAddr, scAddr, destPort, data,
                sentIntent, deliveryIntent);
    }
    else {
        // Rlog->E(LOG_TAG,"sendText iccSmsIntMgr is NULL for" +
        //               " Subscription: " + subId);
    }
    return NOERROR;
}

ECode UiccSmsController::SendDataWithOrigPort(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return SendDataWithOrigPortUsingSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr,
             scAddr, destPort, origPort, data, sentIntent, deliveryIntent);
}

ECode UiccSmsController::SendDataWithOrigPortUsingSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        iccSmsIntMgr->SendDataWithOrigPort(callingPackage, destAddr, scAddr, destPort,
                origPort, data, sentIntent, deliveryIntent);
    }
    else {
        // Rlog->E(LOG_TAG,"sendTextWithOrigPort iccSmsIntMgr is NULL for" +
        //               " Subscription: " + subId);
    }
    return NOERROR;
}

ECode UiccSmsController::SendText(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return SendTextForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr, scAddr,
        text, sentIntent, deliveryIntent);
}

ECode UiccSmsController::SendTextForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return SendTextWithOptionsUsingSubscriber(subId, callingPackage, destAddr, scAddr, text,
            sentIntent, deliveryIntent, -1, FALSE, -1);
}

ECode UiccSmsController::SendTextWithOptionsUsingSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    mContext->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    Boolean bCode = FALSE;
    iccSmsIntMgr->IsShortSMSCode(destAddr, &bCode);
    if (bCode) {
        iccSmsIntMgr->SendTextWithOptions(callingPackage, destAddr, scAddr, text,
                sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
        return NOERROR;
    }
    AutoPtr<IArrayList> parts;
    CArrayList::New((IArrayList**)&parts);
    parts->Add(CoreUtils::Convert(text));
    AutoPtr<IArrayList> sentIntents;
    CArrayList::New((IArrayList**)&sentIntents);
    sentIntents->Add(sentIntent);
    AutoPtr<IArrayList> deliveryIntents;
    CArrayList::New((IArrayList**)&deliveryIntents);
    deliveryIntents->Add(deliveryIntent);
    BroadcastOutgoingSms(subId, callingPackage, destAddr, scAddr, FALSE, parts, sentIntents,
            deliveryIntents, priority, isExpectMore, validityPeriod);
    return NOERROR;
}

ECode UiccSmsController::SendMultipartText(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IList* parts,
    /* [in] */ IList* sentIntents,
    /* [in] */ IList* deliveryIntents)
{
    return SendMultipartTextForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr,
             scAddr, parts, sentIntents, deliveryIntents);
}

ECode UiccSmsController::SendMultipartTextForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IList* parts,
    /* [in] */ IList* sentIntents,
    /* [in] */ IList* deliveryIntents)
{
    return SendMultipartTextWithOptionsUsingSubscriber(subId, callingPackage, destAddr,
            scAddr, parts, sentIntents, deliveryIntents, -1, FALSE, -1);
}

ECode UiccSmsController::SendMultipartTextWithOptionsUsingSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IList* parts,
    /* [in] */ IList* sentIntents,
    /* [in] */ IList* deliveryIntents,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    mContext->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    Boolean bCode = FALSE;
    iccSmsIntMgr->IsShortSMSCode(destAddr, &bCode);
    if (bCode) {
        iccSmsIntMgr->SendMultipartTextWithOptions(callingPackage, destAddr,
                scAddr, parts, sentIntents, deliveryIntents, -1, FALSE, -1);
        return NOERROR;
    }

    AutoPtr<IArrayList> pParts, pSentIntents, pDeliveryIntents;
    if (parts != NULL) {
        CArrayList::New(ICollection::Probe(parts), (IArrayList**)&pParts);
    }

    if (sentIntents != NULL) {
        CArrayList::New(ICollection::Probe(sentIntents), (IArrayList**)&pParts);
    }

    if (deliveryIntents != NULL) {
        CArrayList::New(ICollection::Probe(deliveryIntents), (IArrayList**)&pParts);
    }
    BroadcastOutgoingSms(subId, callingPackage, destAddr, scAddr, TRUE,
            pParts,
            pSentIntents,
            pDeliveryIntents,
            -1, FALSE, -1);
    return NOERROR;
}

ECode UiccSmsController::EnableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return EnableCellBroadcastForSubscriber(GetDefaultSmsSubId(), messageIdentifier, result);
}

ECode UiccSmsController::EnableCellBroadcastForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return EnableCellBroadcastRangeForSubscriber(subId, messageIdentifier, messageIdentifier, result);
}

ECode UiccSmsController::EnableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return EnableCellBroadcastRangeForSubscriber(GetDefaultSmsSubId(), startMessageId,
            endMessageId, result);
}

ECode UiccSmsController::EnableCellBroadcastRangeForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->EnableCellBroadcastRange(startMessageId, endMessageId, result);
    }
    else {
        // Rlog->E(LOG_TAG,"enableCellBroadcast iccSmsIntMgr is NULL for" +
        //               " Subscription: " + subId);
    }
    *result = FALSE;
    return NOERROR;
}

ECode UiccSmsController::DisableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DisableCellBroadcastForSubscriber(GetDefaultSmsSubId(), messageIdentifier, result);
}

ECode UiccSmsController::DisableCellBroadcastForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DisableCellBroadcastRangeForSubscriber(subId, messageIdentifier, messageIdentifier, result);
}

ECode UiccSmsController::DisableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return DisableCellBroadcastRangeForSubscriber(GetDefaultSmsSubId(), startMessageId,
            endMessageId, result);
}

ECode UiccSmsController::DisableCellBroadcastRangeForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->DisableCellBroadcastRange(startMessageId, endMessageId, result);
    }
    else {
        // Rlog->E(LOG_TAG,"disableCellBroadcast iccSmsIntMgr is NULL for" +
        //               " Subscription:"+subId);
    }
    *result = FALSE;
    return NOERROR;
}

ECode UiccSmsController::GetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetPremiumSmsPermissionForSubscriber(GetDefaultSmsSubId(), packageName, result);
}

ECode UiccSmsController::GetPremiumSmsPermissionForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->GetPremiumSmsPermission(packageName, result);
    }
    else {
        // Rlog->E(LOG_TAG, "getPremiumSmsPermission iccSmsIntMgr is NULL");
    }
    //TODO Rakesh
    *result = 0;
    return NOERROR;
}

ECode UiccSmsController::SetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 permission)
{
    return SetPremiumSmsPermissionForSubscriber(GetDefaultSmsSubId(), packageName, permission);
}

ECode UiccSmsController::SetPremiumSmsPermissionForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 permission)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        iccSmsIntMgr->SetPremiumSmsPermission(packageName, permission);
    }
    else {
        // Rlog->E(LOG_TAG, "setPremiumSmsPermission iccSmsIntMgr is NULL");
    }
    return NOERROR;
}

ECode UiccSmsController::IsImsSmsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IsImsSmsSupportedForSubscriber(GetDefaultSmsSubId(), result);
}

ECode UiccSmsController::IsImsSmsSupportedForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->IsImsSmsSupported(result);
    }
    else {
        // Rlog->E(LOG_TAG, "isImsSmsSupported iccSmsIntMgr is NULL");
    }
    *result = FALSE;
    return NOERROR;
}

ECode UiccSmsController::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetImsSmsFormatForSubscriber(GetDefaultSmsSubId(), result);
}

ECode UiccSmsController::GetImsSmsFormatForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->GetImsSmsFormat(result);
    }
    else {
        // Rlog->E(LOG_TAG, "getImsSmsFormat iccSmsIntMgr is NULL");
    }
    *result = String(NULL);
    return NOERROR;
}

ECode UiccSmsController::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(GetDefaultSmsSubId());
    iccSmsIntMgr->UpdateSmsSendStatus(messageRef, success);
    return NOERROR;
}

ECode UiccSmsController::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    return InjectSmsPduForSubscriber(GetDefaultSmsSubId(), pdu, format, receivedIntent);
}

ECode UiccSmsController::InjectSmsPduForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    iccSmsIntMgr->InjectSmsPdu(pdu, format, receivedIntent);
    return NOERROR;
}

ECode UiccSmsController::SynthesizeMessages(
    /* [in] */ const String& originatingAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IList* messages,
    /* [in] */ Int64 timestampMillis)
{
    mContext->EnforceCallingPermission(
            Manifest::permission::BROADCAST_SMS, String(""));
    Int32 size = 0;
    messages->GetSize(&size);
    AutoPtr<ArrayOf<ArrayOf<Byte>* > > pdus = ArrayOf<ArrayOf<Byte>* >::Alloc(size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        messages->Get(i, (IInterface**)&p);
        String str;
        ICharSequence::Probe(p)->ToString(&str);
        AutoPtr<ISyntheticSmsMessage> message;
        CSyntheticSmsMessage::New(originatingAddress,
                scAddress, str, timestampMillis,
                (ISyntheticSmsMessage**)&message);
        AutoPtr<ArrayOf<Byte> > array;
        ISmsMessageBase::Probe(message)->GetPdu((ArrayOf<Byte>**)&array);
        pdus->Set(i, array);
    }
    DispatchPdus(pdus);
    return NOERROR;
}

AutoPtr<IIccSmsInterfaceManager> UiccSmsController::GetIccSmsInterfaceManager(
    /* [in] */ Int64 subId)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    //Fixme: for multi-subscription case
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValidId = FALSE;
    sm->IsValidPhoneId(phoneId, &bValidId);
    if (!bValidId
            || phoneId == ISubscriptionManager::DEFAULT_PHONE_ID) {
        phoneId = 0;
    }

    // try {
    AutoPtr<IPhoneProxy> p = IPhoneProxy::Probe((*mPhone)[phoneId]);
    AutoPtr<IIccSmsInterfaceManager> res;
    p->GetIccSmsInterfaceManager((IIccSmsInterfaceManager**)&res);
    return res;
    // } Catch (NullPointerException e) {
    //     Rlog->E(LOG_TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
    //     e->PrintStackTrace(); //This will print stact trace
    //     return NULL;
    // } Catch (ArrayIndexOutOfBoundsException e) {
    //     Rlog->E(LOG_TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
    //     e->PrintStackTrace(); //This will print stack trace
    //     return NULL;
    // }
}

Int64 UiccSmsController::GetDefaultSmsSubId()
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int64 res = 0;
    IISub::Probe(sc)->GetDefaultSmsSubId(&res);
    return res;
}

ECode UiccSmsController::SendStoredText(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL) {
        iccSmsIntMgr->SendStoredText(callingPkg, messageUri, scAddress, sentIntent,
                deliveryIntent);
    }
    else {
        // Rlog->E(LOG_TAG,"sendStoredText iccSmsIntMgr is NULL for subscription: " + subId);
    }
    return NOERROR;
}

ECode UiccSmsController::SendStoredMultipartText(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IList* sentIntents,
    /* [in] */ IList* deliveryIntents)
{
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
    if (iccSmsIntMgr != NULL ) {
        iccSmsIntMgr->SendStoredMultipartText(callingPkg, messageUri, scAddress, sentIntents,
                deliveryIntents);
    }
    else {
        // Rlog->E(LOG_TAG,"sendStoredMultipartText iccSmsIntMgr is NULL for subscription: "
        //         + subId);
    }
    return NOERROR;
}

ECode UiccSmsController::GetSmsCapacityOnIccForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccSmsInterfaceManager> iccSmsIntMgr = GetIccSmsInterfaceManager(subId);

    if (iccSmsIntMgr != NULL ) {
        return iccSmsIntMgr->GetSmsCapacityOnIcc(result);
    }
    else {
        // Rlog->E(LOG_TAG, "iccSmsIntMgr is NULL for " + " subId: " + subId);
        *result = -1;
        return NOERROR;
    }
}

ECode UiccSmsController::Log(
    /* [in] */ String msg)
{
    Logger::D(LOG_TAG, "[UiccSmsController] %s", (const char*)msg);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
