
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/dialer/calllog/CallLogNotificationsHelper.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Dialer::CallLog::ECLSID_CCallLogNotificationsService;
using Elastos::Droid::Telecom::ITelecomManager;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

void CallLogNotificationsHelper::RemoveMissedCallNotifications(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    AutoPtr<ITelecomManager> telecomManager = ITelecomManager::Probe(service);
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
