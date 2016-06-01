
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/telephony/SmsManager.h"

using Elastos::Droid::Utility::CArrayMap;

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
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    //
    // if (TextUtils.isEmpty(text)) {
    //     throw new IllegalArgumentException("Invalid message body");
    // }
    //
    // try {
    //     ISms iccISms = getISmsServiceOrThrow();
    //     // use sub based apis
    //     iccISms.sendTextForSubscriber(getSubId(), ActivityThread.currentPackageName(),
    //             destinationAddress, scAddress, text, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
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
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    //
    // if (TextUtils.isEmpty(text)) {
    //     throw new IllegalArgumentException("Invalid message body");
    // }
    //
    // try {
    //      ISms iccISms = getISmsServiceOrThrow();
    //     if (iccISms != null) {
    //         iccISms.sendTextWithOptionsUsingSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(), destinationAddress, scAddress, text,
    //                 sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    // ==================before translated======================
    // if (!format.equals(SmsMessage.FORMAT_3GPP) && !format.equals(SmsMessage.FORMAT_3GPP2)) {
    //     // Format must be either 3gpp or 3gpp2.
    //     throw new IllegalArgumentException(
    //             "Invalid pdu format. format must be either 3gpp or 3gpp2");
    // }
    // try {
    //     ISms iccISms = ISms.Stub.asInterface(ServiceManager.getService("isms"));
    //     if (iccISms != null) {
    //         iccISms.injectSmsPduForSubscriber(getSubId(), pdu, format, receivedIntent);
    //     }
    // } catch (RemoteException ex) {
    //   // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    // ==================before translated======================
    // try {
    //     ISms iccISms = ISms.Stub.asInterface(ServiceManager.getService("isms"));
    //     if (iccISms != null) {
    //         iccISms.updateSmsSendStatus(messageRef, success);
    //     }
    // } catch (RemoteException ex) {
    //   // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::DivideMessage(
    /* [in] */ const String& text,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (null == text) {
    //     throw new IllegalArgumentException("text is null");
    // }
    // return SmsMessage.fragmentText(text);
    assert(0);
    return NOERROR;
}

ECode SmsManager::SendMultipartTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* parts,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents)
{
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    // if (parts == null || parts.size() < 1) {
    //     throw new IllegalArgumentException("Invalid message body");
    // }
    //
    // if (parts.size() > 1) {
    //     try {
    //         ISms iccISms = getISmsServiceOrThrow();
    //
    //         iccISms.sendMultipartTextForSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(), destinationAddress, scAddress, parts,
    //
    //                 sentIntents, deliveryIntents);
    //     } catch (RemoteException ex) {
    //         // ignore it
    //     }
    // } else {
    //     PendingIntent sentIntent = null;
    //     PendingIntent deliveryIntent = null;
    //     if (sentIntents != null && sentIntents.size() > 0) {
    //         sentIntent = sentIntents.get(0);
    //     }
    //     if (deliveryIntents != null && deliveryIntents.size() > 0) {
    //         deliveryIntent = deliveryIntents.get(0);
    //     }
    //     sendTextMessage(destinationAddress, scAddress, parts.get(0),
    //             sentIntent, deliveryIntent);
    // }
    assert(0);
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
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    // if (parts == null || parts.size() < 1) {
    //     throw new IllegalArgumentException("Invalid message body");
    // }
    //
    // if (parts.size() > 1) {
    //     try {
    //          ISms iccISms = getISmsServiceOrThrow();
    //         if (iccISms != null) {
    //             iccISms.sendMultipartTextWithOptionsUsingSubscriber(getSubId(),
    //                     ActivityThread.currentPackageName(), destinationAddress, scAddress,
    //                     parts, sentIntents, deliveryIntents, priority, isExpectMore,
    //                     validityPeriod);
    //         }
    //     } catch (RemoteException ex) {
    //         // ignore it
    //     }
    // } else {
    //     PendingIntent sentIntent = null;
    //     PendingIntent deliveryIntent = null;
    //     if (sentIntents != null && sentIntents.size() > 0) {
    //         sentIntent = sentIntents.get(0);
    //     }
    //     if (deliveryIntents != null && deliveryIntents.size() > 0) {
    //         deliveryIntent = deliveryIntents.get(0);
    //     }
    //     sendTextMessage(destinationAddress, scAddress, parts.get(0),
    //             sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
    // }
    assert(0);
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
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    //
    // if (data == null || data.length == 0) {
    //     throw new IllegalArgumentException("Invalid message data");
    // }
    //
    // try {
    //     ISms iccISms = getISmsServiceOrThrow();
    //     iccISms.sendDataForSubscriber(getSubId(), ActivityThread.currentPackageName(),
    //             destinationAddress, scAddress, destinationPort & 0xFFFF,
    //             data, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
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
    // ==================before translated======================
    // if (TextUtils.isEmpty(destinationAddress)) {
    //     throw new IllegalArgumentException("Invalid destinationAddress");
    // }
    //
    // if (data == null || data.length == 0) {
    //     throw new IllegalArgumentException("Invalid message data");
    // }
    // try {
    //     ISms iccISms = ISms.Stub.asInterface(ServiceManager.getService("isms"));
    //     if (iccISms != null) {
    //         iccISms.sendDataWithOrigPortUsingSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(),
    //                 destinationAddress, scAddress, destinationPort & 0xFFFF,
    //                 originatorPort & 0xFFFF, data, sentIntent, deliveryIntent);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<ISmsManager> SmsManager::GetDefault()
{
    // ==================before translated======================
    // return sInstance;
    assert(0);
    AutoPtr<ISmsManager> empty;
    return empty;
}

AutoPtr<ISmsManager> SmsManager::GetSmsManagerForSubscriber(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // // TODO(shri): Add javadoc link once SubscriptionManager is made public api
    // synchronized(sLockObject) {
    //     SmsManager smsManager = sSubInstances.get(subId);
    //     if (smsManager == null) {
    //         smsManager = new SmsManager(subId);
    //         sSubInstances.put(subId, smsManager);
    //     }
    //     return smsManager;
    // }
    assert(0);
    AutoPtr<ISmsManager> empty;
    return empty;
}

ECode SmsManager::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // TODO(shri): Add javadoc link once SubscriptionManager is made public api
    // if (mSubId == DEFAULT_SUB_ID) {
    //     return getDefaultSmsSubId();
    // }
    // return mSubId;
    assert(0);
    return NOERROR;
}

ECode SmsManager::CopyMessageToIcc(
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // if (null == pdu) {
    //     throw new IllegalArgumentException("pdu is NULL");
    // }
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.copyMessageToIccEfForSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(), status, pdu, smsc);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::DeleteMessageFromIcc(
    /* [in] */ Int32 messageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    // byte[] pdu = new byte[IccConstants.SMS_RECORD_LENGTH-1];
    // Arrays.fill(pdu, (byte)0xff);
    //
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.updateMessageOnIccEfForSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(),messageIndex, STATUS_ON_ICC_FREE, pdu);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::UpdateMessageOnIcc(
    /* [in] */ Int32 messageIndex,
    /* [in] */ Int32 newStatus,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.updateMessageOnIccEfForSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName(), messageIndex, newStatus, pdu);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::GetAllMessagesFromIcc(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // List<SmsRawData> records = null;
    //
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         records = iccISms.getAllMessagesFromIccEfForSubscriber(getSubId(),
    //                 ActivityThread.currentPackageName());
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return createMessageListFromRawRecords(records);
    assert(0);
    return NOERROR;
}

ECode SmsManager::EnableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.enableCellBroadcastForSubscriber(getSubId(), messageIdentifier);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::DisableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.disableCellBroadcastForSubscriber(getSubId(), messageIdentifier);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::EnableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // if (endMessageId < startMessageId) {
    //     throw new IllegalArgumentException("endMessageId < startMessageId");
    // }
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.enableCellBroadcastRangeForSubscriber(getSubId(),
    //                 startMessageId, endMessageId);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::DisableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean success = false;
    //
    // if (endMessageId < startMessageId) {
    //     throw new IllegalArgumentException("endMessageId < startMessageId");
    // }
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         success = iccISms.disableCellBroadcastRangeForSubscriber(getSubId(),
    //                 startMessageId, endMessageId);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    //
    // return success;
    assert(0);
    return NOERROR;
}

ECode SmsManager::IsImsSmsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean boSupported = false;
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         boSupported = iccISms.isImsSmsSupportedForSubscriber(getSubId());
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return boSupported;
    assert(0);
    return NOERROR;
}

ECode SmsManager::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String format = com.android.internal.telephony.SmsConstants.FORMAT_UNKNOWN;
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         format = iccISms.getImsSmsFormatForSubscriber(getSubId());
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return format;
    assert(0);
    return NOERROR;
}

Int64 SmsManager::GetDefaultSmsSubId()
{
    // ==================before translated======================
    // return SubscriptionManager.getDefaultSmsSubId();
    assert(0);
    return 0;
}

ECode SmsManager::GetSmsCapacityOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int ret = -1;
    // try {
    //     ISms iccISms = getISmsService();
    //     if (iccISms != null) {
    //         ret = iccISms.getSmsCapacityOnIccForSubscriber(getSubId());
    //     }
    // } catch (RemoteException ex) {
    //     //ignore it
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode SmsManager::SendMultimediaMessage(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& locationUrl,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    // ==================before translated======================
    // if (contentUri == null) {
    //     throw new IllegalArgumentException("Uri contentUri null");
    // }
    // try {
    //     final IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms == null) {
    //         return;
    //     }
    //     context.grantUriPermission(PHONE_PACKAGE_NAME, contentUri,
    //             Intent.FLAG_GRANT_READ_URI_PERMISSION);
    //     grantCarrierPackageUriPermission(context, contentUri,
    //             Telephony.Mms.Intents.MMS_SEND_ACTION, Intent.FLAG_GRANT_READ_URI_PERMISSION);
    //
    //     iMms.sendMessage(getSubId(), ActivityThread.currentPackageName(), contentUri,
    //             locationUrl, configOverrides, sentIntent);
    // } catch (RemoteException e) {
    //     // Ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::DownloadMultimediaMessage(
    /* [in] */ IContext* context,
    /* [in] */ const String& locationUrl,
    /* [in] */ IUri* contentUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* downloadedIntent)
{
    // ==================before translated======================
    // if (TextUtils.isEmpty(locationUrl)) {
    //     throw new IllegalArgumentException("Empty MMS location URL");
    // }
    // if (contentUri == null) {
    //     throw new IllegalArgumentException("Uri contentUri null");
    // }
    // try {
    //     final IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms == null) {
    //         return;
    //     }
    //     context.grantUriPermission(PHONE_PACKAGE_NAME, contentUri,
    //             Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
    //
    //     grantCarrierPackageUriPermission(context, contentUri,
    //             Telephony.Mms.Intents.MMS_DOWNLOAD_ACTION,
    //             Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
    //
    //     iMms.downloadMessage(getSubId(), ActivityThread.currentPackageName(), locationUrl,
    //             contentUri, configOverrides, downloadedIntent);
    // } catch (RemoteException e) {
    //     // Ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::UpdateMmsSendStatus(
    /* [in] */ IContext* context,
    /* [in] */ Int32 messageRef,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [in] */ IUri* contentUri)
{
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms == null) {
    //         return;
    //     }
    //     iMms.updateMmsSendStatus(messageRef, pdu, status);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // if (contentUri != null) {
    //     context.revokeUriPermission(contentUri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::UpdateMmsDownloadStatus(
    /* [in] */ IContext* context,
    /* [in] */ Int32 messageRef,
    /* [in] */ Int32 status,
    /* [in] */ IUri* contentUri)
{
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms == null) {
    //         return;
    //     }
    //     iMms.updateMmsDownloadStatus(messageRef, status);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // if (contentUri != null) {
    //     context.revokeUriPermission(contentUri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
    // }
    assert(0);
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
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.importTextMessage(ActivityThread.currentPackageName(),
    //                 address, type, text, timestampMillis, seen, read);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return null;
    assert(0);
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
    // ==================before translated======================
    // if (contentUri == null) {
    //     throw new IllegalArgumentException("Uri contentUri null");
    // }
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.importMultimediaMessage(ActivityThread.currentPackageName(),
    //                 contentUri, messageId, timestampSecs, seen, read);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode SmsManager::DeleteStoredMessage(
    /* [in] */ IUri* messageUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (messageUri == null) {
    //     throw new IllegalArgumentException("Empty message URI");
    // }
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.deleteStoredMessage(ActivityThread.currentPackageName(), messageUri);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsManager::DeleteStoredConversation(
    /* [in] */ Int64 conversationId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.deleteStoredConversation(
    //                 ActivityThread.currentPackageName(), conversationId);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsManager::UpdateStoredMessageStatus(
    /* [in] */ IUri* messageUri,
    /* [in] */ IContentValues* statusValues,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (messageUri == null) {
    //     throw new IllegalArgumentException("Empty message URI");
    // }
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.updateStoredMessageStatus(ActivityThread.currentPackageName(),
    //                 messageUri, statusValues);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsManager::ArchiveStoredConversation(
    /* [in] */ Int64 conversationId,
    /* [in] */ Boolean archived,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.archiveStoredConversation(ActivityThread.currentPackageName(),
    //                 conversationId, archived);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsManager::AddTextMessageDraft(
    /* [in] */ const String& address,
    /* [in] */ const String& text,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.addTextMessageDraft(ActivityThread.currentPackageName(), address, text);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode SmsManager::AddMultimediaMessageDraft(
    /* [in] */ IUri* contentUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (contentUri == null) {
    //     throw new IllegalArgumentException("Uri contentUri null");
    // }
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.addMultimediaMessageDraft(ActivityThread.currentPackageName(),
    //                 contentUri);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode SmsManager::SendStoredTextMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    // ==================before translated======================
    // if (messageUri == null) {
    //     throw new IllegalArgumentException("Empty message URI");
    // }
    // try {
    //     ISms iccISms = getISmsServiceOrThrow();
    //     iccISms.sendStoredText(getSubId(), ActivityThread.currentPackageName(), messageUri,
    //             scAddress, sentIntent, deliveryIntent);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::SendStoredMultipartTextMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* sentIntents,
    /* [in] */ IArrayList* deliveryIntents)
{
    // ==================before translated======================
    // if (messageUri == null) {
    //     throw new IllegalArgumentException("Empty message URI");
    // }
    // try {
    //     ISms iccISms = getISmsServiceOrThrow();
    //     iccISms.sendStoredMultipartText(getSubId(), ActivityThread.currentPackageName(), messageUri,
    //             scAddress, sentIntents, deliveryIntents);
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::SendStoredMultimediaMessage(
    /* [in] */ IUri* messageUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    // ==================before translated======================
    // if (messageUri == null) {
    //     throw new IllegalArgumentException("Empty message URI");
    // }
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         iMms.sendStoredMessage(getSubId(), ActivityThread.currentPackageName(), messageUri,
    //                 configOverrides, sentIntent);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::SetAutoPersisting(
    /* [in] */ Boolean enabled)
{
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         iMms.setAutoPersisting(ActivityThread.currentPackageName(), enabled);
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    assert(0);
    return NOERROR;
}

ECode SmsManager::GetAutoPersisting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.getAutoPersisting();
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsManager::GetCarrierConfigValues(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     IMms iMms = IMms.Stub.asInterface(ServiceManager.getService("imms"));
    //     if (iMms != null) {
    //         return iMms.getCarrierConfigValues(getSubId());
    //     }
    // } catch (RemoteException ex) {
    //     // ignore it
    // }
    // return null;
    assert(0);
    return NOERROR;
}

SmsManager::SmsManager(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // mSubId = subId;
}

AutoPtr<IISms> SmsManager::GetISmsServiceOrThrow()
{
    // ==================before translated======================
    // ISms iccISms = getISmsService();
    // if (iccISms == null) {
    //     throw new UnsupportedOperationException("Sms is not supported");
    // }
    // return iccISms;
    assert(0);
    AutoPtr<IISms> empty;
    return empty;
}

AutoPtr<IISms> SmsManager::GetISmsService()
{
    // ==================before translated======================
    // return ISms.Stub.asInterface(ServiceManager.getService("isms"));
    assert(0);
    AutoPtr<IISms> empty;
    return empty;
}

AutoPtr<IArrayList> SmsManager::CreateMessageListFromRawRecords(
    /* [in] */ IList* records)
{
    // ==================before translated======================
    // ArrayList<SmsMessage> messages = new ArrayList<SmsMessage>();
    // if (records != null) {
    //     int count = records.size();
    //     for (int i = 0; i < count; i++) {
    //         SmsRawData data = records.get(i);
    //         // List contains all records, including "free" records (null)
    //         if (data != null) {
    //             SmsMessage sms = SmsMessage.createFromEfRecord(i+1, data.getBytes(),
    //                     getSubId());
    //             if (sms != null) {
    //                 messages.add(sms);
    //             }
    //         }
    //     }
    // }
    // return messages;
    assert(0);
    AutoPtr<IArrayList> empty;
    return empty;
}

void SmsManager::GrantCarrierPackageUriPermission(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& action,
    /* [in] */ Int32 permission)
{
    // ==================before translated======================
    // Intent intent = new Intent(action);
    // TelephonyManager telephonyManager =
    //     (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    // List<String> carrierPackages = telephonyManager.getCarrierPackageNamesForIntent(
    //         intent);
    // if (carrierPackages != null && carrierPackages.size() == 1) {
    //     context.grantUriPermission(carrierPackages.get(0), contentUri, permission);
    // }
    assert(0);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
