#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/telephony/gsm/SmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//
//=====================================================================
AutoPtr<SmsManager> SmsManager::sInstance;

AutoPtr<SmsManager> SmsManager::GetDefault()
{
    // ==================before translated======================
    // if (sInstance == null) {
    //     sInstance = new SmsManager();
    // }
    // return sInstance;
    assert(0);
    AutoPtr<SmsManager> empty;
    return empty;
}

SmsManager::SmsManager()
{
    // ==================before translated======================
    // mSmsMgrProxy = android.telephony.SmsManager.getDefault();
}

ECode SmsManager::SendTextMessage(
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

ECode SmsManager::DivideMessage(
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

ECode SmsManager::SendMultipartTextMessage(
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

ECode SmsManager::SendDataMessage(
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

ECode SmsManager::CopyMessageToSim(
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

ECode SmsManager::DeleteMessageFromSim(
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

ECode SmsManager::UpdateMessageOnSim(
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

ECode SmsManager::GetAllMessagesFromSim(
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

