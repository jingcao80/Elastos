#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/telephony/gsm/GSMSmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//
//=====================================================================
AutoPtr<GSMSmsManager> GSMSmsManager::sInstance;

AutoPtr<GSMSmsManager> GSMSmsManager::GetDefault()
{
    // ==================before translated======================
    // if (sInstance == null) {
    //     sInstance = new GSMSmsManager();
    // }
    // return sInstance;
    assert(0);
    AutoPtr<GSMSmsManager> empty;
    return empty;
}

GSMSmsManager::GSMSmsManager()
{
    // ==================before translated======================
    // mSmsMgrProxy = android.telephony.GSMSmsManager.getDefault();
}

ECode GSMSmsManager::SendTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    // ==================before translated======================
    // mSmsMgrProxy.sendTextMessage(destinationAddress, scAddress, text,
    //         sentIntent, deliveryIntent);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::DivideMessage(
    /* [in] */ const String& text,
    /* [out] */ IArrayList** result)//String
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSmsMgrProxy.divideMessage(text);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::SendMultipartTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* parts,//String
    /* [in] */ IArrayList* sentIntents,//IPendingIntent
    /* [in] */ IArrayList* deliveryIntents)//IPendingIntent
{
    // ==================before translated======================
    // mSmsMgrProxy.sendMultipartTextMessage(destinationAddress, scAddress, parts,
    //         sentIntents, deliveryIntents);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::SendDataMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    // ==================before translated======================
    // mSmsMgrProxy.sendDataMessage(destinationAddress, scAddress, destinationPort,
    //         data, sentIntent, deliveryIntent);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::CopyMessageToSim(
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mSmsMgrProxy.copyMessageToIcc(smsc, pdu, status);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::DeleteMessageFromSim(
    /* [in] */ Int32 messageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mSmsMgrProxy.deleteMessageFromIcc(messageIndex);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::UpdateMessageOnSim(
    /* [in] */ Int32 messageIndex,
    /* [in] */ Int32 newStatus,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mSmsMgrProxy.updateMessageOnIcc(messageIndex, newStatus, pdu);
    assert(0);
    return NOERROR;
}

ECode GSMSmsManager::GetAllMessagesFromSim(
    /* [out] */ IArrayList** result)//SmsMessage
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return android.telephony.SmsManager.getDefault().getAllMessagesFromIcc();
    assert(0);
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

