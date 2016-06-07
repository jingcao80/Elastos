
#include "elastos/droid/server/telecom/StatusBarNotifier.h"
#include "R.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.App.h>
#include <elastos/droid/R.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String StatusBarNotifier::SLOT_MUTE("mute");
const String StatusBarNotifier::SLOT_SPEAKERPHONE("speakerphone");

StatusBarNotifier::StatusBarNotifier()
    : mIsShowingMute(FALSE)
    , mIsShowingSpeakerphone(FALSE)
{}

ECode StatusBarNotifier::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CallsManager* callsManager)
{
    mContext = context;
    mCallsManager = callsManager;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mStatusBarManager = IStatusBarManager::Probe(obj);
    return NOERROR;
}

ECode StatusBarNotifier::OnCallRemoved(
    /* [in] */ ICall* call)
{
    Boolean hasAnyCalls;
    mCallsManager->HasAnyCalls(&hasAnyCalls);
    if (!hasAnyCalls) {
        NotifyMute(FALSE);
        NotifySpeakerphone(FALSE);
    }
    return NOERROR;
}

ECode StatusBarNotifier::NotifyMute(
    /* [in] */ Boolean isMuted)
{
    // Never display anything if there are no calls.
    Boolean hasAnyCalls;
    mCallsManager->HasAnyCalls(&hasAnyCalls);
    if (!hasAnyCalls) {
        isMuted = FALSE;
    }
    if (mIsShowingMute == isMuted) {
        return NOERROR;
    }
    Log::D("StatusBarNotifier", "Mute status bar icon being set to %d", isMuted);
    if (isMuted) {
        String resStr;
        mContext->GetString(R::string::accessibility_call_muted, &resStr);
        mStatusBarManager->SetIcon(
                SLOT_MUTE,
                Elastos::Droid::R::drawable::stat_notify_call_mute,
                0,  /* iconLevel */
                resStr);
    } else {
        mStatusBarManager->RemoveIcon(SLOT_MUTE);
    }
    mIsShowingMute = isMuted;
    return NOERROR;
}

ECode StatusBarNotifier::NotifySpeakerphone(
    /* [in] */ Boolean isSpeakerphone)
{
    // Never display anything if there are no calls.
    Boolean hasAnyCalls;
    mCallsManager->HasAnyCalls(&hasAnyCalls);
    if (!hasAnyCalls) {
        isSpeakerphone = FALSE;
    }
    if (mIsShowingSpeakerphone == isSpeakerphone) {
        return NOERROR;
    }
    Log::D("StatusBarNotifier", "Speakerphone status bar icon being set to %d", isSpeakerphone);
    if (isSpeakerphone) {
        String resStr;
        mContext->GetString(R::string::accessibility_speakerphone_enabled, &resStr);
        mStatusBarManager->SetIcon(
                SLOT_SPEAKERPHONE,
                Elastos::Droid::R::drawable::stat_sys_speakerphone,
                0,  /* iconLevel */
                resStr);
    } else {
        mStatusBarManager->RemoveIcon(SLOT_SPEAKERPHONE);
    }
    mIsShowingSpeakerphone = isSpeakerphone;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
