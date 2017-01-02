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

#include "elastos/droid/incallui/ProximitySensor.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/incallui/CallList.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::InCallUI::EIID_IOrientationListener;
using Elastos::Droid::InCallUI::EIID_IInCallStateListener;
using Elastos::Droid::InCallUI::EIID_IAudioModeListener;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IAudioStateHelper;
using Elastos::Droid::Telecom::CAudioStateHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const String ProximitySensor::TAG("ProximitySensor");

CAR_INTERFACE_IMPL_3(ProximitySensor, Object
        , IOrientationListener
        , IInCallStateListener
        , IAudioModeListener)

ProximitySensor::ProximitySensor(
    /* [in] */ IContext* context,
    /* [in] */ AudioModeProvider* audioModeProvider)
    : mOrientation(AccelerometerListener::ORIENTATION_UNKNOWN)
    , mUiShowing(FALSE)
    , mIsPhoneOffhook(FALSE)
    , mDialpadVisible(FALSE)
    , mIsHardKeyboardOpen(FALSE)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    mPowerManager = IPowerManager::Probe(service);
    mAccelerometerListener = new AccelerometerListener(context, this);
    mAudioModeProvider = audioModeProvider;
    mAudioModeProvider->AddListener(this);
}

void ProximitySensor::TearDown()
{
    mAudioModeProvider->RemoveListener(this);

    mAccelerometerListener->Enable(FALSE);

    TelecomAdapter::GetInstance()->TurnOffProximitySensor(TRUE);
}

ECode ProximitySensor::OrientationChanged(
    /* [in] */ Int32 orientation)
{
    mOrientation = orientation;
    UpdateProximitySensorMode();
    return NOERROR;
}

ECode ProximitySensor::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    // We ignore incoming state because we do not want to enable proximity
    // sensor during incoming call screen. We check hasLiveCall() because a disconnected call
    // can also put the in-call screen in the INCALL state.
    Boolean hasOngoingCall = InCallState_INCALL == newState && ((CallList*)callList)->HasLiveCall();
    Boolean isOffhook = (InCallState_OUTGOING == newState) || hasOngoingCall;

    if (isOffhook != mIsPhoneOffhook) {
        mIsPhoneOffhook = isOffhook;

        mOrientation = AccelerometerListener::ORIENTATION_UNKNOWN;
        mAccelerometerListener->Enable(mIsPhoneOffhook);

        UpdateProximitySensorMode();
    }
    return NOERROR;
}

ECode ProximitySensor::OnSupportedAudioMode(
    /* [in] */ Int32 modeMask)
{
    return NOERROR;
}

ECode ProximitySensor::OnMute(
    /* [in] */ Boolean muted)
{
    return NOERROR;
}

ECode ProximitySensor::OnAudioMode(
    /* [in] */ Int32 newMode)
{
    UpdateProximitySensorMode();
    return NOERROR;
}

void ProximitySensor::OnDialpadVisible(
    /* [in] */ Boolean visible)
{
    mDialpadVisible = visible;
    UpdateProximitySensorMode();
}

void ProximitySensor::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Int32 hardKeyboardHidden;
    newConfig->GetHardKeyboardHidden(&hardKeyboardHidden);
    mIsHardKeyboardOpen = hardKeyboardHidden == IConfiguration::HARDKEYBOARDHIDDEN_NO;

    // Update the Proximity sensor based on keyboard state
    UpdateProximitySensorMode();
}

void ProximitySensor::OnInCallShowing(
    /* [in] */ Boolean showing)
{
    Boolean isScreenOn;
    if (showing) {
        mUiShowing = TRUE;

    // We only consider the UI not showing for instances where another app took the foreground.
    // If we stopped showing because the screen is off, we still consider that showing.
    }
    else if (mPowerManager->IsScreenOn(&isScreenOn), isScreenOn) {
        mUiShowing = FALSE;
    }
    UpdateProximitySensorMode();
}

Boolean ProximitySensor::IsScreenReallyOff()
{
    Boolean isScreenOn;
    return (mPowerManager->IsScreenOn(&isScreenOn), !isScreenOn);
}

void ProximitySensor::UpdateProximitySensorMode()
{
    Int32 audioMode = mAudioModeProvider->GetAudioMode();

    // turn proximity sensor off and turn screen on immediately if
    // we are using a headset, the keyboard is open, or the device
    // is being held in a horizontal position.
    Boolean screenOnImmediately = (IAudioState::ROUTE_WIRED_HEADSET == audioMode
            || IAudioState::ROUTE_SPEAKER == audioMode
            || IAudioState::ROUTE_BLUETOOTH == audioMode
            || mIsHardKeyboardOpen);

    // We do not keep the screen off when the user is outside in-call screen and we are
    // horizontal, but we do not force it on when we become horizontal until the
    // proximity sensor goes negative.
    Boolean horizontal =
            (mOrientation == AccelerometerListener::ORIENTATION_HORIZONTAL);
    screenOnImmediately |= !mUiShowing && horizontal;

    // We do not keep the screen off when dialpad is visible, we are horizontal, and
    // the in-call screen is being shown.
    // At that moment we're pretty sure users want to use it, instead of letting the
    // proximity sensor turn off the screen by their hands.
    screenOnImmediately |= mDialpadVisible && horizontal;

    Logger::V(TAG, "screenonImmediately: %d", screenOnImmediately);

    AutoPtr<IAudioStateHelper> helper;
    CAudioStateHelper::AcquireSingleton((IAudioStateHelper**)&helper);
    String str;
    helper->AudioRouteToString(audioMode, &str);
    Logger::I(TAG, "%s keybrd:%d dpad:%d offhook:%d hor:%d ui:%d aud:%s",
            TO_CSTR(this), mIsHardKeyboardOpen, mDialpadVisible, mIsPhoneOffhook, horizontal, mUiShowing, str.string());

    if (mIsPhoneOffhook && !screenOnImmediately) {
        Logger::D(TAG, "Turning on proximity sensor");
        // Phone is in use!  Arrange for the screen to turn off
        // automatically when the sensor detects a close object.
        TelecomAdapter::GetInstance()->TurnOnProximitySensor();
    }
    else {
        Logger::D(TAG, "Turning off proximity sensor");
        // Phone is either idle, or ringing.  We don't want any special proximity sensor
        // behavior in either case.
        TelecomAdapter::GetInstance()->TurnOffProximitySensor(screenOnImmediately);
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
