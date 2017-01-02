//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__
#define __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/AccelerometerListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Class manages the proximity sensor for the in-call UI.
 * We enable the proximity sensor while the user in a phone call. The Proximity sensor turns off
 * the touchscreen and display when the user is close to the screen to prevent user's cheek from
 * causing touch events.
 * The class requires special knowledge of the activity and device state to know when the proximity
 * sensor should be enabled and disabled. Most of that state is fed into this class through
 * public methods.
 */
class ProximitySensor
    : public Object
    , public IOrientationListener
    , public IInCallStateListener
    , public IAudioModeListener
{
public:
    CAR_INTERFACE_DECL();

    ProximitySensor(
        /* [in] */ IContext* context,
        /* [in] */ AudioModeProvider* audioModeProvider);

    CARAPI_(void) TearDown();

    /**
     * Called to identify when the device is laid down flat.
     */
    // @Override
    CARAPI OrientationChanged(
        /* [in] */ Int32 orientation);

    /**
     * Called to keep track of the overall UI state.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    // @Override
    CARAPI OnSupportedAudioMode(
        /* [in] */ Int32 modeMask);

    // @Override
    CARAPI OnMute(
        /* [in] */ Boolean muted);

    // @Override
    CARAPI OnAudioMode(
        /* [in] */ Int32 newMode);

    CARAPI_(void) OnDialpadVisible(
        /* [in] */ Boolean visible);

    /**
     * Called by InCallActivity to listen for hard keyboard events.
     */
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * Used to save when the UI goes in and out of the foreground.
     */
    CARAPI_(void) OnInCallShowing(
        /* [in] */ Boolean showing);

    /**
     * TODO: There is no way to determine if a screen is off due to proximity or if it is
     * legitimately off, but if ever we can do that in the future, it would be useful here.
     * Until then, this function will simply return true of the screen is off.
     */
    CARAPI_(Boolean) IsScreenReallyOff();

private:
    /**
     * Updates the wake lock used to control proximity sensor behavior,
     * based on the current state of the phone.
     *
     * On devices that have a proximity sensor, to avoid false touches
     * during a call, we hold a PROXIMITY_SCREEN_OFF_WAKE_LOCK wake lock
     * whenever the phone is off hook.  (When held, that wake lock causes
     * the screen to turn off automatically when the sensor detects an
     * object close to the screen.)
     *
     * This method is a no-op for devices that don't have a proximity
     * sensor.
     *
     * Proximity wake lock will *not* be held if any one of the
     * conditions is true while on a call:
     * 1) If the audio is routed via Bluetooth
     * 2) If a wired headset is connected
     * 3) if the speaker is ON
     * 4) If the slider is open(i.e. the hardkeyboard is *not* hidden)
     */
    CARAPI_(void) UpdateProximitySensorMode();

private:
    static const String TAG;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<AudioModeProvider> mAudioModeProvider;
    AutoPtr<AccelerometerListener> mAccelerometerListener;
    Int32 mOrientation;// = AccelerometerListener.ORIENTATION_UNKNOWN;
    Boolean mUiShowing;
    Boolean mIsPhoneOffhook;
    Boolean mDialpadVisible;

    // True if the keyboard is currently *not* hidden
    // Gets updated whenever there is a Configuration change
    Boolean mIsHardKeyboardOpen;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__
