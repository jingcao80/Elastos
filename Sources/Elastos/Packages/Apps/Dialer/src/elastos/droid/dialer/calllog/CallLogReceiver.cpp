
#include "_Elastos.Apps.Dialer.h"
#include "elastos/droid/dialer/calllog/CallLogReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IVoicemailContract;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
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
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, IParcelable::Probe(data));
        AutoPtr<IComponentName> name;
        context->StartService(serviceIntent, (IComponentName**)&name);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(context,
                ECLSID_CCallLogNotificationsService, (IIntent**)&serviceIntent);
        serviceIntent->SetAction(ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS);
        AutoPtr<IComponentName> name;
        context->StartService(serviceIntent, (IComponentName**)&name);
    }
    else {
        String str;
        IObject::Probe(intent)->ToString(&str);
        Logger::W(TAG, "onReceive: could not handle: %s", str.string());
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
