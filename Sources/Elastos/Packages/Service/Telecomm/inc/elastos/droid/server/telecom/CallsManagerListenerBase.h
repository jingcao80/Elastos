
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGERLISTENERBASE_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGERLISTENERBASE_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Telecom::IAudioState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Provides a default implementation for listeners of CallsManager.
 */
class CallsManagerListenerBase
    : public Object
    , public ICallsManagerListener
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    // @Override
    CARAPI OnCallExtrasUpdated(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnConnectionServiceChanged(
        /* [in] */ ICall* call,
        /* [in] */ IConnectionServiceWrapper* oldService,
        /* [in] */ IConnectionServiceWrapper* newService);

    // @Override
    CARAPI OnIncomingCallAnswered(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIncomingCallRejected(
        /* [in] */ ICall* call,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    // @Override
    CARAPI OnForegroundCallChanged(
        /* [in] */ ICall* oldForegroundCall,
        /* [in] */ ICall* newForegroundCall);

    // @Override
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* oldAudioState,
        /* [in] */ IAudioState* newAudioState);

    // @Override
    CARAPI OnRingbackRequested(
        /* [in] */ ICall* call,
        /* [in] */ Boolean ringback);

    // @Override
    CARAPI OnIsConferencedChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIsVoipAudioModeChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnVideoStateChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallSubstateChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCanAddCallChanged(
        /* [in] */ Boolean canAddCall);

    // @Override
    CARAPI OnProcessIncomingCall(
        /* [in] */ ICall* call);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGERLISTENERBASE_H__
