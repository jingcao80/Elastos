
#include "elastos/droid/incallui/InCallApp.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// InCallApp::NotificationBroadcastReceiver
//================================================================
ECode InCallApp::NotificationBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::I("InCallApp", "Broadcast from Notification: %s", action.string());

    // TODO: Commands of this nature should exist in the CallList.
    if (action.Equals(ACTION_ANSWER_VIDEO_INCOMING_CALL)) {
        InCallPresenter::GetInstance()->AnswerIncomingCall(context, IVideoProfileVideoState::BIDIRECTIONAL);
    }
    else if (action.Equals(ACTION_ANSWER_VOICE_INCOMING_CALL)) {
        InCallPresenter::GetInstance()->AnswerIncomingCall(context, IVideoProfileVideoState::AUDIO_ONLY);
    }
    else if (action.Equals(ACTION_DECLINE_INCOMING_CALL)) {
        InCallPresenter::GetInstance()->DeclineIncomingCall(context);
    }
    else if (action.Equals(ACTION_HANG_UP_ONGOING_CALL)) {
        InCallPresenter::GetInstance()->HangUpOngoingCall(context);
    }
    else if (action.Equals(ACTION_ACCEPT_VIDEO_UPGRADE_REQUEST)) {
        InCallPresenter::GetInstance()->AcceptUpgradeRequest(context);
    }
    else if (action.Equals(ACTION_DECLINE_VIDEO_UPGRADE_REQUEST)) {
        InCallPresenter::GetInstance()->DeclineUpgradeRequest(context);
    }
    return NOERROR;
}


//================================================================
// InCallApp
//================================================================
const String InCallApp::ACTION_DECLINE_INCOMING_CALL("com.android.incallui.ACTION_DECLINE_INCOMING_CALL");
const String InCallApp::ACTION_HANG_UP_ONGOING_CALL("com.android.incallui.ACTION_HANG_UP_ONGOING_CALL");
const String InCallApp::ACTION_ANSWER_VIDEO_INCOMING_CALL("com.android.incallui.ACTION_ANSWER_VIDEO_INCOMING_CALL");
const String InCallApp::ACTION_ANSWER_VOICE_INCOMING_CALL("com.android.incallui.ACTION_ANSWER_VOICE_INCOMING_CALL");
const String InCallApp::ACTION_ACCEPT_VIDEO_UPGRADE_REQUEST("com.android.incallui.ACTION_ACCEPT_VIDEO_UPGRADE_REQUEST");
const String InCallApp::ACTION_DECLINE_VIDEO_UPGRADE_REQUEST("com.android.incallui.ACTION_DECLINE_VIDEO_UPGRADE_REQUEST");

ECode InCallApp::OnCreate()
{
    return NOERROR;
}

ECode InCallApp::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    return Application::OnConfigurationChanged(newConfig);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
