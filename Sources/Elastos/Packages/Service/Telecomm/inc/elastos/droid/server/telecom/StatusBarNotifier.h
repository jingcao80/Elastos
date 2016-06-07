
#ifndef __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallsManager;
/**
 * Manages the special status bar notifications used by the phone app.
 */
class StatusBarNotifier
    : public CallsManagerListenerBase
{
public:
    StatusBarNotifier();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ CallsManager* callsManager);

    /** ${inheritDoc} */
    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    CARAPI NotifyMute(
        /* [in] */ Boolean isMuted);

    CARAPI NotifySpeakerphone(
        /* [in] */ Boolean isSpeakerphone);

private:
    static const String SLOT_MUTE;

    static const String SLOT_SPEAKERPHONE;

    AutoPtr<IContext> mContext;

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<IStatusBarManager> mStatusBarManager;

    Boolean mIsShowingMute;

    Boolean mIsShowingSpeakerphone;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_STATUSBARNOTIFIER_H__
