#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/telephony/gsm/GSMSmsManager.h"
#include "elastos/droid/telephony/SmsManager.h"

using Elastos::Droid::Telephony::SmsManager;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//
//=====================================================================
AutoPtr<GSMSmsManager> GSMSmsManager::sInstance;

CAR_INTERFACE_IMPL(GSMSmsManager, Object, ISmsManager)

AutoPtr<GSMSmsManager> GSMSmsManager::GetDefault()
{
    if (sInstance == NULL) {
        sInstance = new GSMSmsManager();
    }
    return sInstance;
}

GSMSmsManager::GSMSmsManager()
{
    mSmsMgrProxy = SmsManager::GetDefault();
}

ECode GSMSmsManager::SendTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return mSmsMgrProxy->SendTextMessage(destinationAddress, scAddress, text,
            sentIntent, deliveryIntent);
}

ECode GSMSmsManager::DivideMessage(
    /* [in] */ const String& text,
    /* [out] */ IArrayList** result)//String
{
    VALIDATE_NOT_NULL(result);
    return mSmsMgrProxy->DivideMessage(text, result);
}

ECode GSMSmsManager::SendMultipartTextMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IArrayList* parts,//String
    /* [in] */ IArrayList* sentIntents,//IPendingIntent
    /* [in] */ IArrayList* deliveryIntents)//IPendingIntent
{
    return mSmsMgrProxy->SendMultipartTextMessage(
            destinationAddress, scAddress, parts, sentIntents, deliveryIntents);
}

ECode GSMSmsManager::SendDataMessage(
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    return mSmsMgrProxy->SendDataMessage(destinationAddress,
            scAddress, destinationPort, data, sentIntent, deliveryIntent);
}

ECode GSMSmsManager::CopyMessageToSim(
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsMgrProxy->CopyMessageToIcc(smsc, pdu, status, result);
}

ECode GSMSmsManager::DeleteMessageFromSim(
    /* [in] */ Int32 messageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsMgrProxy->DeleteMessageFromIcc(messageIndex, result);
}

ECode GSMSmsManager::UpdateMessageOnSim(
    /* [in] */ Int32 messageIndex,
    /* [in] */ Int32 newStatus,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSmsMgrProxy->UpdateMessageOnIcc(
            messageIndex, newStatus, pdu, result);
}

ECode GSMSmsManager::GetAllMessagesFromSim(
    /* [out] */ IArrayList** result)//SmsMessage
{
    VALIDATE_NOT_NULL(result);
    return SmsManager::GetDefault()->GetAllMessagesFromIcc(result);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

