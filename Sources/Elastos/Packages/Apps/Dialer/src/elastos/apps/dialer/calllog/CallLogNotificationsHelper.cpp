
#include "elastos/apps/dialer/calllog/CallLogNotificationsHelper.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

void CallLogNotificationsHelper::RemoveMissedCallNotifications(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    ITelecomManager* telecomManager = ITelecomManager*::Probe(service);

    telecomManager->CancelMissedCallsNotification();
}

void CallLogNotificationsHelper::UpdateVoicemailNotifications(
        /* [in] */ IContext* context)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context,
            ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
    serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
    context->StartService(serviceIntent);
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
