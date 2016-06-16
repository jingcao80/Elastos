
#include "elastos/apps/dialer/calllog/CallLogReceiver.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const String CallLogReceiver::TAG("CallLogReceiver");

ECode CallLogReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IVoicemailContract::ACTION_NEW_VOICEMAIL.Equals(action)) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(context,
                ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
        serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);

        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        serviceIntent->PutExtra(
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, data);
        context->StartService(serviceIntent);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(context,
                ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
        serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
        context->StartService(serviceIntent);
    }
    else {
        String str;
        intentt->ToString(&str)
        Logger::W(TAG, "onReceive: could not handle: %s", str.string());
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
