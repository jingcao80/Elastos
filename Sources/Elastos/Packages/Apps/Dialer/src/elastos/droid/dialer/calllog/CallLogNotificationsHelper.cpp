
#include "elastos/droid/dialer/calllog/CallLogNotificationsHelper.h"
#include "_Elastos.Apps.Dialer.h"
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Apps::Dialer::CallLog::ECLSID_CCallLogNotificationsService;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

void CallLogNotificationsHelper::RemoveMissedCallNotifications(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    ITelecomManager* telecomManager = ITelecomManager::Probe(service);
    telecomManager->CancelMissedCallsNotification();
}

void CallLogNotificationsHelper::UpdateVoicemailNotifications(
        /* [in] */ IContext* context)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context,
            ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
    serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
    AutoPtr<IComponentName> name;
    context->StartService(serviceIntent, (IComponentName**)&name);
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
