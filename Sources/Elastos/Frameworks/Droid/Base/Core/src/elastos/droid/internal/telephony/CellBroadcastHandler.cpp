
#include "elastos/droid/internal/telephony/CellBroadcastHandler.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::SubscriptionManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CellBroadcastHandler::CellBroadcastHandler()
{}

ECode CellBroadcastHandler::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    return constructor(String("CellBroadcastHandler"), context, phone);
}

ECode CellBroadcastHandler::constructor(
    /* [in] */ const String& debugTag,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    return WakeLockStateMachine::constructor(debugTag, context, phone);
}

/**
 * Create a new CellBroadcastHandler.
 * @param context the context to use for dispatching Intents
 * @return the new handler
 */
AutoPtr<CellBroadcastHandler> CellBroadcastHandler::MakeCellBroadcastHandler(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    AutoPtr<CellBroadcastHandler> handler = new CellBroadcastHandler();
    handler->constructor(context, phone);
    handler->Start();
    return handler;
}

/**
 * Handle Cell Broadcast messages from {@code CdmaInboundSmsHandler}.
 * 3GPP-format Cell Broadcast messages sent from radio are handled in the subclass.
 *
 * @param message the message to process
 * @return true if an ordered broadcast was sent; false on failure
 */
// @Override
ECode CellBroadcastHandler::HandleSmsMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    if (ISmsCbMessage::Probe(obj)) {
        HandleBroadcastSms(ISmsCbMessage::Probe(obj));
        *result = TRUE;
        return NOERROR;
    }

    Loge(String("handleMessage got object of type: ") + TO_CSTR(obj));
    *result = FALSE;
    return NOERROR;
}

/**
 * Dispatch a Cell Broadcast message to listeners.
 * @param message the Cell Broadcast to broadcast
 */
void CellBroadcastHandler::HandleBroadcastSms(
    /* [in] */ ISmsCbMessage* message)
{
    String receiverPermission;
    Int32 appOp = 0;
    AutoPtr<IIntent> intent;
    Boolean tmp = FALSE;
    if (message->IsEmergencyMessage(&tmp), tmp) {
        Log(String("Dispatching emergency SMS CB"));
        CIntent::New(ITelephonySmsIntents::SMS_EMERGENCY_CB_RECEIVED_ACTION, (IIntent**)&intent);
        receiverPermission = Manifest::permission::RECEIVE_EMERGENCY_BROADCAST;
        appOp = IAppOpsManager::OP_RECEIVE_EMERGECY_SMS;
    }
    else {
        Log(String("Dispatching SMS CB"));
        CIntent::New(ITelephonySmsIntents::SMS_CB_RECEIVED_ACTION, (IIntent**)&intent);
        receiverPermission = Manifest::permission::RECEIVE_SMS;
        appOp = IAppOpsManager::OP_RECEIVE_SMS;
    }
    intent->PutExtra(String("message"), IParcelable::Probe(message));
    Int32 phoneId = 0;
    mPhone->GetPhoneId(&phoneId);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, phoneId);
    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);
    mContext->SendOrderedBroadcast(intent, receiverPermission, appOp, mReceiver,
            h, IActivity::RESULT_OK, String(NULL), NULL);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
