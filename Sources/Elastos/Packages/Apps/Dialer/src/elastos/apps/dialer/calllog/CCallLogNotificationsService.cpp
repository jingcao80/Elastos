#include "elastos/apps/dialer/calllog/CCallLogNotificationsService.h"

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
    GetContentResolver((IContentResolver**)&resolver)
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
        AutoPtr<IParcelabe> parcelable;
        intent->GetParcelableExtra(
                ICallLogNotificationsService::EXTRA_NEW_VOICEMAIL_URI, &parcelable);
        IUri* voicemailUri = IUri::Probe(parcelable);
        DefaultVoicemailNotifier::GetInstance(this)->UpdateNotification(voicemailUri);
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

