
#ifndef __ELASTOS_DROID_SERVER_TELECOM_RINGBACKPLAYER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_RINGBACKPLAYER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Plays ringback tones. Ringback is different from other tones because it operates as the current
 * audio for a call, whereas most tones play as simple timed events. This means ringback must be
 * able to turn off and on as the user switches between calls. This is why it is implemented as its
 * own class.
 */
class RingbackPlayer
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ CallsManager* callsManager,
        /* [in] */ InCallTonePlayer::Factory* playerFactory);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnForegroundCallChanged(
        /* [in] */ ICall* oldForegroundCall,
        /* [in] */ ICall* newForegroundCall);

    // @Override
    CARAPI OnConnectionServiceChanged(
        /* [in] */ ICall* call,
        /* [in] */ IConnectionServiceWrapper* oldService,
        /* [in] */ IConnectionServiceWrapper* newService);

    // @Override
    CARAPI OnRingbackRequested(
        /* [in] */ ICall* call,
        /* [in] */ Boolean ignored);

private:
    /**
     * Starts ringback for the specified dialing call as needed.
     *
     * @param call The call for which to ringback.
     */
    CARAPI StartRingbackForCall(
        /* [in] */ ICall* call);

    /**
     * Stops the ringback for the specified dialing call as needed.
     *
     * @param call The call for which to stop ringback.
     */
    CARAPI StopRingbackForCall(
        /* [in] */ ICall* call);

    CARAPI ShouldStartRinging(
        /* [in] */ ICall* call,
        /* [out] */ Boolean* result);

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<InCallTonePlayer::Factory> mPlayerFactory;

    /**
     * The current call for which the ringback tone is being played.
     */
    AutoPtr<ICall> mCall;

    /**
     * The currently active player.
     */
    AutoPtr<InCallTonePlayer> mTonePlayer;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_RINGBACKPLAYER_H__
