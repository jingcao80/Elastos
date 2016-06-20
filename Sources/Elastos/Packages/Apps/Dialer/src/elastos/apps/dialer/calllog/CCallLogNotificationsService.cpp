#include "elastos/apps/dialer/calllog/CCallLogNotificationsService.h"
#include "elastos/apps/dialer/calllog/CCallLogQueryHandler.h"
#include "elastos/apps/dialer/calllog/DefaultVoicemailNotifier.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const String CCallLogNotificationsService::TAG("CallLogNotificationsService");

CAR_INTERFACE_IMPL(CCallLogNotificationsService, IntentService, ICallLogNotificationsService);

CAR_OBJECT_IMPL(CCallLogNotificationsService);

ECode CCallLogNotificationsService::constructor()
{
    return IntentService::constructor(TAG);
}

ECode CCallLogNotificationsService::OnCreate()
{
    IntentService::OnCreate();

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    return CCallLogQueryHandler::New(resolver, NULL /*listener*/,
            (ICallLogQueryHandler**)&mCallLogQueryHandler);
}

ECode CCallLogNotificationsService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        Logger::D(TAG, "onHandleIntent: could not handle null intent");
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    if (ICallLogNotificationsService::ACTION_MARK_NEW_VOICEMAILS_AS_OLD.Equals(action)) {
        mCallLogQueryHandler->MarkNewVoicemailsAsOld();
    }
    else if (ICallLogNotificationsService::ACTION_UPDATE_NOTIFICATIONS.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, (IParcelable**)&parcelable);
        IUri* voicemailUri = IUri::Probe(parcelable);
        IVoicemailNotifier::Probe(DefaultVoicemailNotifier::GetInstance(this))->UpdateNotification(voicemailUri);
    }
    else {
        String str;
        intent->ToString(&str);
        Logger::D(TAG, "onHandleIntent: could not handle: %s", str.string());
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos

