
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/telephony/SmsManager.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IIMms;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsRawData;
using Elastos::Droid::Internal::Telephony::IUiccSmsController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ITelephonyMmsIntents;
using Elastos::Droid::Telephony::SmsMessage;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                              SmsManager
//=====================================================================
CAR_INTERFACE_IMPL(SmsManager, Object, ISmsManager);
static AutoPtr<IMap> InitArrayMap()
{
    AutoPtr<IMap> map;
    CArrayMap::New((IMap**)&map);
    return map;
}
const Int32 SmsManager::DEFAULT_SUB_ID;
AutoPtr<ISmsManager> SmsManager::sInstance = new SmsManager(DEFAULT_SUB_ID);
Object SmsManager::sLockObject;
AutoPtr<IMap> SmsManager::sSubInstances = InitArrayMap();//<Long, SmsManager
const String SmsManager::PHONE_PACKAGE_NAME("com.android.phone");

ECode SmsManager::SendTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (TextUtils::IsEmpty(text)) {
        // throw new IllegalArgumentException("Invalid message body");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IISms> iccISms;
    GetISmsServiceOrThrow((IISms**)&iccISms);
    // use sub based apis
    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iccISms->SendTextForSubscriber(sid, pkgName,
            destinationAddress, scAddress, text, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SendTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (TextUtils::IsEmpty(text)) {
        // throw new IllegalArgumentException("Invalid message body");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IISms> iccISms;
    GetISmsServiceOrThrow((IISms**)&iccISms);
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        IUiccSmsController::Probe(iccISms)->SendTextWithOptionsUsingSubscriber(
                sid, pkgName, destinationAddress, scAddress, text,
                sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    if (!format.Equals(ISmsMessage::FORMAT_3GPP) && !format.Equals(ISmsMessage::FORMAT_3GPP2)) {
        // Format must be either 3gpp or 3gpp2.
        // throw new IllegalArgumentException(
        //         "Invalid pdu format. format must be either 3gpp or 3gpp2");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("isms"));
    AutoPtr<IISms> iccISms = IISms::Probe(obj);
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        IUiccSmsController::Probe(iccISms)->InjectSmsPduForSubscriber(
                sid, pdu, format, receivedIntent);
    }
    // } catch (RemoteException ex) {
    //   // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("isms"));
    AutoPtr<IISms> iccISms = IISms::Probe(obj);
    if (iccISms != NULL) {
        iccISms->UpdateSmsSendStatus(messageRef, success);
    }
    // } catch (RemoteException ex) {
    //   // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::DivideMessage(
    /* [in] */ const String& text,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == text) {
        // throw new IllegalArgumentException("text is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IArrayList> list = SmsMessage::FragmentText(text);
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsManager::SendMultipartTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* parts,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 size;
    if (parts == NULL || (parts->GetSize(&size), size) < 1) {
        // throw new IllegalArgumentException("Invalid message body");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((parts->GetSize(&size), size) > 1) {
        // try {
        AutoPtr<IISms> iccISms;
        GetISmsServiceOrThrow((IISms**)&iccISms);

        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        iccISms->SendMultipartTextForSubscriber(sid,
                pkgName, destinationAddress, scAddress, IList::Probe(parts),
                IList::Probe(sentIntents), IList::Probe(deliveryIntents));
        // } catch (RemoteException ex) {
        //     // ignore it
        // }
    }
    else {
        AutoPtr<IPendingIntent> sentIntent;
        AutoPtr<IPendingIntent> deliveryIntent;
        AutoPtr<IInterface> obj;
        if (sentIntents != NULL && (sentIntents->GetSize(&size), size) > 0) {
            sentIntents->Get(0, (IInterface**)&obj);
            sentIntent = IPendingIntent::Probe(obj);
        }
        if (deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > 0) {
            deliveryIntents->Get(0, (IInterface**)&obj);
            deliveryIntent = IPendingIntent::Probe(obj);
        }
        parts->Get(0, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        SendTextMessage(destinationAddress, scAddress, str,
                sentIntent, deliveryIntent);
    }
    return NOERROR;
}

ECode SmsManager::SendMultipartTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* parts,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 size;
    if (parts == NULL || (parts->GetSize(&size), size) < 1) {
        // throw new IllegalArgumentException("Invalid message body");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((parts->GetSize(&size), size) > 1) {
        // try {
        AutoPtr<IISms> iccISms;
        GetISmsServiceOrThrow((IISms**)&iccISms);
        if (iccISms != NULL) {
            Int64 sid;
            GetSubId(&sid);
            String pkgName = CActivityThread::GetCurrentPackageName();
            IUiccSmsController::Probe(iccISms)->SendMultipartTextWithOptionsUsingSubscriber(
                    sid, pkgName, destinationAddress, scAddress,
                    IList::Probe(parts), IList::Probe(sentIntents),
                    IList::Probe(deliveryIntents), priority, isExpectMore,
                    validityPeriod);
        }
        // } catch (RemoteException ex) {
        //     // ignore it
        // }
    }
    else {
        AutoPtr<IPendingIntent> sentIntent;
        AutoPtr<IPendingIntent> deliveryIntent;
        AutoPtr<IInterface> obj;
        if (sentIntents != NULL && (sentIntents->GetSize(&size), size) > 0) {
            sentIntents->Get(0, (IInterface**)&obj);
            sentIntent = IPendingIntent::Probe(obj);
        }
        if (deliveryIntents != NULL && (deliveryIntents->GetSize(&size), size) > 0) {
            deliveryIntents->Get(0, (IInterface**)&obj);
            deliveryIntent = IPendingIntent::Probe(obj);
        }

        parts->Get(0, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        SendTextMessage(destinationAddress, scAddress, str,
                sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
    }
    return NOERROR;
}

ECode SmsManager::SendDataMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (data == NULL || data->GetLength() == 0) {
        // throw new IllegalArgumentException("Invalid message data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IISms> iccISms;
    GetISmsServiceOrThrow((IISms**)&iccISms);
    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iccISms->SendDataForSubscriber(sid, pkgName,
            destinationAddress, scAddress, destinationPort & 0xFFFF,
            data, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SendDataMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ Int16 originatorPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    if (TextUtils::IsEmpty(destinationAddress)) {
        // throw new IllegalArgumentException("Invalid destinationAddress");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (data == NULL || data->GetLength() == 0) {
        // throw new IllegalArgumentException("Invalid message data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("isms"));
    AutoPtr<IISms> iccISms = IISms::Probe(obj);
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        IUiccSmsController::Probe(iccISms)->SendDataWithOrigPortUsingSubscriber(
                sid, pkgName,
                destinationAddress, scAddress, destinationPort & 0xFFFF,
                originatorPort & 0xFFFF, data, sentIntent, deliveryIntent);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

AutoPtr<ISmsManager> SmsManager::GetDefault()
{
    return sInstance;
}

AutoPtr<ISmsManager> SmsManager::GetSmsManagerForSubscriber(
    /* [in] */ Int64 subId)
{
    // TODO(shri): Add javadoc link once SubscriptionManager is made public api
    AutoLock lock(sLockObject);
    AutoPtr<IInterface> obj;
    sSubInstances->Get(CoreUtils::Convert(subId), (IInterface**)&obj);
    AutoPtr<ISmsManager> smsManager = ISmsManager::Probe(obj);
    if (smsManager == NULL) {
        smsManager = new SmsManager(subId);
        sSubInstances->Put(CoreUtils::Convert(subId), smsManager);
    }
    return smsManager;
}

ECode SmsManager::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO(shri): Add javadoc link once SubscriptionManager is made public api
    if (mSubId == DEFAULT_SUB_ID) {
        *result = GetDefaultSmsSubId();
    }
    return NOERROR;
}

ECode SmsManager::CopyMessageToIcc(
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (NULL == pdu) {
        // throw new IllegalArgumentException("pdu is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iccISms->CopyMessageToIccEfForSubscriber(sid,
            pkgName, status, pdu, smsc, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::DeleteMessageFromIcc(
    /* [in] */ Int32 messageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(IIccConstants::SMS_RECORD_LENGTH - 1);
    Arrays::Fill(pdu, (Byte)0xff);

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        iccISms->UpdateMessageOnIccEfForSubscriber(sid,
                pkgName, messageIndex, STATUS_ON_ICC_FREE, pdu, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::UpdateMessageOnIcc(
    /* [in] */ Int32 messageIndex,
    /* [in] */ Int32 newStatus,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        iccISms->UpdateMessageOnIccEfForSubscriber(sid,
                pkgName, messageIndex, newStatus, pdu, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::GetAllMessagesFromIcc(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> records;

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        iccISms->GetAllMessagesFromIccEfForSubscriber(sid,
                pkgName, (IList**)&records);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    AutoPtr<IArrayList> list = CreateMessageListFromRawRecords(records);
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsManager::EnableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->EnableCellBroadcastForSubscriber(sid, messageIdentifier, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::DisableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->DisableCellBroadcastForSubscriber(sid, messageIdentifier, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::EnableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (endMessageId < startMessageId) {
        // throw new IllegalArgumentException("endMessageId < startMessageId");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->EnableCellBroadcastRangeForSubscriber(sid,
                startMessageId, endMessageId, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::DisableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (endMessageId < startMessageId) {
        // throw new IllegalArgumentException("endMessageId < startMessageId");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->DisableCellBroadcastRangeForSubscriber(sid,
                startMessageId, endMessageId, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    return NOERROR;
}

ECode SmsManager::IsImsSmsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->IsImsSmsSupportedForSubscriber(sid, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ISmsConstants::FORMAT_UNKNOWN;
    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        iccISms->GetImsSmsFormatForSubscriber(sid, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

Int64 SmsManager::GetDefaultSmsSubId()
{
    Int64 val;
    SubscriptionManager::GetDefaultSmsSubId(&val);
    return val;
}

ECode SmsManager::GetSmsCapacityOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    // try {
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        IUiccSmsController::Probe(iccISms)->GetSmsCapacityOnIccForSubscriber(sid, result);
    }
    // } catch (RemoteException ex) {
    //     //ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SendMultimediaMessage(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& locationUrl,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    if (contentUri == NULL) {
        // throw new IllegalArgumentException("Uri contentUri NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms == NULL) {
        return NOERROR;
    }
    context->GrantUriPermission(PHONE_PACKAGE_NAME, contentUri,
            IIntent::FLAG_GRANT_READ_URI_PERMISSION);
    GrantCarrierPackageUriPermission(context, contentUri,
            ITelephonyMmsIntents::MMS_SEND_ACTION, IIntent::FLAG_GRANT_READ_URI_PERMISSION);

    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iMms->SendMessage(sid, pkgName, contentUri,
            locationUrl, configOverrides, sentIntent);
    // } catch (RemoteException e) {
    //     // Ignore it
    // }
    return NOERROR;
}

ECode SmsManager::DownloadMultimediaMessage(
    /* [in] */ IContext* context,
    /* [in] */ const String& locationUrl,
    /* [in] */ IUri* contentUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* downloadedIntent)
{
    if (TextUtils::IsEmpty(locationUrl)) {
        // throw new IllegalArgumentException("Empty MMS location URL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (contentUri == NULL) {
        // throw new IllegalArgumentException("Uri contentUri NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms == NULL) {
        return NOERROR;
    }
    context->GrantUriPermission(PHONE_PACKAGE_NAME, contentUri,
            IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    GrantCarrierPackageUriPermission(context, contentUri,
            ITelephonyMmsIntents::MMS_DOWNLOAD_ACTION,
            IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iMms->DownloadMessage(sid, pkgName, locationUrl,
            contentUri, configOverrides, downloadedIntent);
    // } catch (RemoteException e) {
    //     // Ignore it
    // }
    return NOERROR;
}

ECode SmsManager::UpdateMmsSendStatus(
    /* [in] */ IContext* context,
    /* [in] */ Int32 messageRef,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [in] */ IUri* contentUri)
{
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms == NULL) {
        return NOERROR;
    }
    iMms->UpdateMmsSendStatus(messageRef, pdu, status);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    if (contentUri != NULL) {
        context->RevokeUriPermission(contentUri, IIntent::FLAG_GRANT_READ_URI_PERMISSION);
    }
    return NOERROR;
}

ECode SmsManager::UpdateMmsDownloadStatus(
    /* [in] */ IContext* context,
    /* [in] */ Int32 messageRef,
    /* [in] */ Int32 status,
    /* [in] */ IUri* contentUri)
{
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms == NULL) {
        return NOERROR;
    }
    iMms->UpdateMmsDownloadStatus(messageRef, status);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }

    if (contentUri != NULL) {
        context->RevokeUriPermission(contentUri, IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
    }
    return NOERROR;
}

ECode SmsManager::ImportTextMessage(
    /* [in] */ const String& address,
    /* [in] */ Int32 type,
    /* [in] */ const String& text,
    /* [in] */ Int64 timestampMillis,
    /* [in] */ Boolean seen,
    /* [in] */ Boolean read,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->ImportTextMessage(pkgName,
                address, type, text, timestampMillis, seen, read, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = NULL;
    return NOERROR;
}

ECode SmsManager::ImportMultimediaMessage(
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& messageId,
    /* [in] */ Int64 timestampSecs,
    /* [in] */ Boolean seen,
    /* [in] */ Boolean read,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (contentUri == NULL) {
        // throw new IllegalArgumentException("Uri contentUri NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->ImportMultimediaMessage(pkgName,
                contentUri, messageId, timestampSecs, seen, read, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = NULL;
    return NOERROR;
}

ECode SmsManager::DeleteStoredMessage(
    /* [in] */ IUri* messageUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (messageUri == NULL) {
        // throw new IllegalArgumentException("Empty message URI");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->DeleteStoredMessage(pkgName, messageUri, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::DeleteStoredConversation(
    /* [in] */ Int64 conversationId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
    String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->DeleteStoredConversation(
                pkgName, conversationId, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = FALSE;
    return NOERROR;
}

ECode SmsManager::UpdateStoredMessageStatus(
    /* [in] */ IUri* messageUri,
    /* [in] */ IContentValues* statusValues,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (messageUri == NULL) {
        // throw new IllegalArgumentException("Empty message URI");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->UpdateStoredMessageStatus(pkgName,
                messageUri, statusValues, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = FALSE;
    return NOERROR;
}

ECode SmsManager::ArchiveStoredConversation(
    /* [in] */ Int64 conversationId,
    /* [in] */ Boolean archived,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
        if (iMms != NULL) {
            String pkgName = CActivityThread::GetCurrentPackageName();
            return iMms->ArchiveStoredConversation(pkgName,
                    conversationId, archived, result);
        }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = FALSE;
    return NOERROR;
}

ECode SmsManager::AddTextMessageDraft(
    /* [in] */ const String& address,
    /* [in] */ const String& text,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->AddTextMessageDraft(pkgName, address, text, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = NULL;
    return NOERROR;
}

ECode SmsManager::AddMultimediaMessageDraft(
    /* [in] */ IUri* contentUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (contentUri == NULL) {
        // throw new IllegalArgumentException("Uri contentUri NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        return iMms->AddMultimediaMessageDraft(pkgName,
                contentUri, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = NULL;
    return NOERROR;
}

ECode SmsManager::SendStoredTextMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    if (messageUri == NULL) {
        // throw new IllegalArgumentException("Empty message URI");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IISms> iccISms;
    GetISmsServiceOrThrow((IISms**)&iccISms);
    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iccISms->SendStoredText(sid, pkgName, messageUri,
            scAddress, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SendStoredMultipartTextMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents)
{
    if (messageUri == NULL) {
        // throw new IllegalArgumentException("Empty message URI");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IISms> iccISms;
    GetISmsServiceOrThrow((IISms**)&iccISms);
    Int64 sid;
    GetSubId(&sid);
    String pkgName = CActivityThread::GetCurrentPackageName();
    iccISms->SendStoredMultipartText(sid, pkgName, messageUri,
            scAddress, IList::Probe(sentIntents), IList::Probe(deliveryIntents));
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SendStoredMultimediaMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    if (messageUri == NULL) {
        // throw new IllegalArgumentException("Empty message URI");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        String pkgName = CActivityThread::GetCurrentPackageName();
        iMms->SendStoredMessage(sid, pkgName, messageUri,
                configOverrides, sentIntent);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::SetAutoPersisting(
    /* [in] */ Boolean enabled)
{
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        String pkgName = CActivityThread::GetCurrentPackageName();
        iMms->SetAutoPersisting(pkgName, enabled);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    return NOERROR;
}

ECode SmsManager::GetAutoPersisting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        return iMms->GetAutoPersisting(result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = FALSE;
    return NOERROR;
}

ECode SmsManager::GetCarrierConfigValues(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("imms"));
    AutoPtr<IIMms> iMms = IIMms::Probe(obj);
    if (iMms != NULL) {
        Int64 sid;
        GetSubId(&sid);
        return iMms->GetCarrierConfigValues(sid, result);
    }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    *result = NULL;
    return NOERROR;
}

SmsManager::SmsManager(
    /* [in] */ Int64 subId)
{
    mSubId = subId;
}

ECode SmsManager::GetISmsServiceOrThrow(
    /* [out] */ IISms** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IISms> iccISms = GetISmsService();
    if (iccISms == NULL) {
        // throw new UnsupportedOperationException("Sms is not supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    *result = iccISms;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IISms> SmsManager::GetISmsService()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("isms"));
    return IISms::Probe(obj);
}

AutoPtr<IArrayList> SmsManager::CreateMessageListFromRawRecords(
    /* [in] */ IList* records)
{
    AutoPtr<IArrayList> messages;
    CArrayList::New((IArrayList**)&messages);

    if (records != NULL) {
        Int32 count;
        records->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            records->Get(i, (IInterface**)&obj);
            AutoPtr<ISmsRawData> data = ISmsRawData::Probe(obj);
            // List contains all records, including "free" records (NULL)
            if (data != NULL) {
                Int64 sid;
                GetSubId(&sid);
                AutoPtr<ArrayOf<Byte> > bytes;
                data->GetBytes((ArrayOf<Byte>**)&bytes);
                AutoPtr<ISmsMessage> sms = SmsMessage::CreateFromEfRecord(
                        i + 1, bytes, sid);
                if (sms != NULL) {
                    messages->Add(sms);
                }
            }
        }
    }
    return messages;
}

void SmsManager::GrantCarrierPackageUriPermission(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& action,
    /* [in] */ Int32 permission)
{
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
    AutoPtr<IList> carrierPackages;
    telephonyManager->GetCarrierPackageNamesForIntent(intent, (IList**)&carrierPackages);

    Int32 size;
    if (carrierPackages != NULL && (carrierPackages->GetSize(&size), size) == 1) {
        AutoPtr<IInterface> obj;
        carrierPackages->Get(0, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        context->GrantUriPermission(str, contentUri, permission);
    }
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
