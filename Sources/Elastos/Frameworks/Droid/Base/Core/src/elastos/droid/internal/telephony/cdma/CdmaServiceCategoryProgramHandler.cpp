#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaServiceCategoryProgramHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//      CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver
//=====================================================================
CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ CdmaServiceCategoryProgramHandler* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // sendScpResults();
    // if (DBG) log("mScpResultsReceiver finished");
    // sendMessage(EVENT_BROADCAST_COMPLETE);
    assert(0);
    return NOERROR;
}

void CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::SendScpResults()
{
    // ==================before translated======================
    // int resultCode = getResultCode();
    // if ((resultCode != Activity.RESULT_OK) && (resultCode != Intents.RESULT_SMS_HANDLED)) {
    //     loge("SCP results error: result code = " + resultCode);
    //     return;
    // }
    // Bundle extras = getResultExtras(false);
    // if (extras == null) {
    //     loge("SCP results error: missing extras");
    //     return;
    // }
    // String sender = extras.getString("sender");
    // if (sender == null) {
    //     loge("SCP results error: missing sender extra.");
    //     return;
    // }
    // ArrayList<CdmaSmsCbProgramResults> results
    //         = extras.getParcelableArrayList("results");
    // if (results == null) {
    //     loge("SCP results error: missing results extra.");
    //     return;
    // }
    //
    // BearerData bData = new BearerData();
    // bData.messageType = BearerData.MESSAGE_TYPE_SUBMIT;
    // bData.messageId = SmsMessage.getNextMessageId();
    // bData.serviceCategoryProgramResults = results;
    // byte[] encodedBearerData = BearerData.encode(bData);
    //
    // ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
    // DataOutputStream dos = new DataOutputStream(baos);
    // try {
    //     dos.writeInt(SmsEnvelope.TELESERVICE_SCPT);
    //     dos.writeInt(0); //servicePresent
    //     dos.writeInt(0); //serviceCategory
    //     CdmaSmsAddress destAddr = CdmaSmsAddress.parse(
    //             PhoneNumberUtils.cdmaCheckAndProcessPlusCodeForSms(sender));
    //     dos.write(destAddr.digitMode);
    //     dos.write(destAddr.numberMode);
    //     dos.write(destAddr.ton); // number_type
    //     dos.write(destAddr.numberPlan);
    //     dos.write(destAddr.numberOfDigits);
    //     dos.write(destAddr.origBytes, 0, destAddr.origBytes.length); // digits
    //     // Subaddress is not supported.
    //     dos.write(0); //subaddressType
    //     dos.write(0); //subaddr_odd
    //     dos.write(0); //subaddr_nbr_of_digits
    //     dos.write(encodedBearerData.length);
    //     dos.write(encodedBearerData, 0, encodedBearerData.length);
    //     // Ignore the RIL response. TODO: implement retry if SMS send fails.
    //     mCi.sendCdmaSms(baos.toByteArray(), null);
    // } catch (IOException e) {
    //     loge("exception creating SCP results PDU", e);
    // } finally {
    //     try {
    //         dos.close();
    //     } catch (IOException ignored) {
    //     }
    // }
    assert(0);
}

//=====================================================================
//                  CdmaServiceCategoryProgramHandler
//=====================================================================
CAR_INTERFACE_IMPL(CdmaServiceCategoryProgramHandler, /*TODO WakeLockStateMachine*/ Object, ICdmaServiceCategoryProgramHandler);
CdmaServiceCategoryProgramHandler::CdmaServiceCategoryProgramHandler(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* commandsInterface)
{
    // ==================before translated======================
    // super("CdmaServiceCategoryProgramHandler", context, null);
    // mContext = context;
    // mCi = commandsInterface;
}

AutoPtr<ICdmaServiceCategoryProgramHandler> CdmaServiceCategoryProgramHandler::MakeScpHandler(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* commandsInterface)
{
    // ==================before translated======================
    // CdmaServiceCategoryProgramHandler handler = new CdmaServiceCategoryProgramHandler(
    //         context, commandsInterface);
    // handler.start();
    // return handler;
    assert(0);
    AutoPtr<ICdmaServiceCategoryProgramHandler> empty;
    return empty;
}

ECode CdmaServiceCategoryProgramHandler::HandleSmsMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // if (message.obj instanceof SmsMessage) {
    //     return handleServiceCategoryProgramData((SmsMessage) message.obj);
    // } else {
    //     loge("handleMessage got object of type: " + message.obj.getClass().getName());
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

Boolean CdmaServiceCategoryProgramHandler::HandleServiceCategoryProgramData(
    /* [in] */ ISmsMessage* sms)
{
    // ==================before translated======================
    // ArrayList<CdmaSmsCbProgramData> programDataList = sms.getSmsCbProgramData();
    // if (programDataList == null) {
    //     loge("handleServiceCategoryProgramData: program data list is null!");
    //     return false;
    // }
    //
    // Intent intent = new Intent(Intents.SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED_ACTION);
    // intent.putExtra("sender", sms.getOriginatingAddress());
    // intent.putParcelableArrayListExtra("program_data", programDataList);
    // SubscriptionManager.putPhoneIdAndSubIdExtra(intent, mPhone.getPhoneId());
    // mContext.sendOrderedBroadcast(intent, Manifest.permission.RECEIVE_SMS,
    //         AppOpsManager.OP_RECEIVE_SMS, mScpResultsReceiver,
    //         getHandler(), Activity.RESULT_OK, null, null);
    // return true;
    assert(0);
    return FALSE;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
