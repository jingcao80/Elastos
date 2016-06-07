
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLAUDIOMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLAUDIOMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/BluetoothManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include "elastos/droid/server/telecom/StatusBarNotifier.h"
#include "elastos/droid/server/telecom/WiredHeadsetManager.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Telecomm::Telecom::IAudioState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * This class manages audio modes, streams and other properties.
 */
class CallAudioManager
    : public CallsManagerListenerBase
    , public IWiredHeadsetManagerListener
{
public:
    CAR_INTERFACE_DECL()

    CallAudioManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ StatusBarNotifier* statusBarNotifier,
        /* [in] */ WiredHeadsetManager* wiredHeadsetManager);

    CARAPI GetAudioState(
        /* [out] */ IAudioState** result);

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
    CARAPI OnIncomingCallAnswered(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnForegroundCallChanged(
        /* [in] */ ICall* oldForegroundCall,
        /* [in] */ ICall* newForegroundCall);

    // @Override
    CARAPI OnIsVoipAudioModeChanged(
        /* [in] */ ICall* call);

    /**
      * Updates the audio route when the headset plugged in state changes. For example, if audio is
      * being routed over speakerphone and a headset is plugged in then switch to wired headset.
      */
    // @Override
    CARAPI OnWiredHeadsetPluggedInChanged(
        /* [in] */ Boolean oldIsPluggedIn,
        /* [in] */ Boolean newIsPluggedIn);

    CARAPI ToggleMute();

    CARAPI Mute(
        /* [in] */ Boolean shouldMute);

    /**
     * Changed the audio route, for example from earpiece to speaker phone.
     *
     * @param route The new audio route to use. See {@link AudioState}.
     */
    CARAPI SetAudioRoute(
        /* [in] */ Int32 route);

    CARAPI SetIsRinging(
        /* [in] */ Boolean isRinging);

    /**
     * Sets the tone playing status. Some tones can play even when there are no live calls and this
     * status indicates that we should keep audio focus even for tones that play beyond the life of
     * calls.
     *
     * @param isPlayingNew The status to set.
     */
    CARAPI SetIsTonePlaying(
        /* [in] */ Boolean isPlayingNew);

    /**
     * Updates the audio routing according to the bluetooth state.
     */
    CARAPI OnBluetoothStateChange(
        /* [in] */ BluetoothManager* bluetoothManager);

    CARAPI IsBluetoothAudioOn(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothDeviceAvailable(
        /* [out] */ Boolean* result);

private:
    CARAPI SaveAudioState(
        /* [in] */ IAudioState* audioState);

    CARAPI OnCallUpdated(
        /* [in] */ ICall* call);

    CARAPI SetSystemAudioState(
        /* [in] */ Boolean isMuted,
        /* [in] */ Int32 route,
        /* [in] */ Int32 supportedRouteMask);

    CARAPI SetSystemAudioState(
        /* [in] */ Boolean force,
        /* [in] */ Boolean isMuted,
        /* [in] */ Int32 route,
        /* [in] */ Int32 supportedRouteMask);

    CARAPI TurnOnSpeaker(
        /* [in] */ Boolean on);

    CARAPI TurnOnBluetooth(
        /* [in] */ Boolean on);

    CARAPI UpdateAudioStreamAndMode();

    CARAPI RequestAudioFocusAndSetMode(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 mode);

    CARAPI AbandonAudioFocus();

    /**
     * Sets the audio mode.
     *
     * @param newMode Mode constant from AudioManager.MODE_*.
     */
    CARAPI SetMode(
        /* [in] */ Int32 newMode);

    CARAPI SelectWiredOrEarpiece(
        /* [in] */ Int32 route,
        /* [in] */ Int32 supportedRouteMask,
        /* [out] */ Int32* result);

    CARAPI CalculateSupportedRoutes(
        /* [out] */ Int32* result);

    CARAPI GetInitialAudioState(
        /* [in] */ ICall* call,
        /* [out] */ IAudioState** result);

    CARAPI SetInitialAudioState(
        /* [in] */ ICall* call,
        /* [in] */ Boolean force);

    CARAPI UpdateAudioForForegroundCall();

    /**
     * Returns the current foreground call in order to properly set the audio mode.
     */
    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    CARAPI HasRingingForegroundCall(
        /* [out] */ Boolean* result);

    CARAPI HasFocus(
        /* [out] */ Boolean* result);

private:
    static const Int32 STREAM_NONE;

    AutoPtr<StatusBarNotifier> mStatusBarNotifier;

    AutoPtr<IAudioManager> mAudioManager;

    AutoPtr<BluetoothManager> mBluetoothManager;

    AutoPtr<WiredHeadsetManager> mWiredHeadsetManager;

    AutoPtr<IAudioState> mAudioState;

    Int32 mAudioFocusStreamType;

    Boolean mIsRinging;

    Boolean mIsTonePlaying;

    Boolean mWasSpeakerOn;

    Int32 mMostRecentlyUsedMode;

    Boolean mSpeedUpAudioForMtCall;

    AutoPtr<IContext> mContext;

    String mSubId;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLAUDIOMANAGER_H__
