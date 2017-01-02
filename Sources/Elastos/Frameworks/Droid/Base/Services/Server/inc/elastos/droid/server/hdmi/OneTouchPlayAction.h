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

#ifndef __ELASTOS_DROID_SERVER_HDMI_ONETOUCHPLAYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_ONETOUCHPLAYACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDevicePlayback;
/**
 * Feature action that performs one touch play against TV/Display device. This action is initiated
 * via {@link android.hardware.hdmi.HdmiPlaybackClient#oneTouchPlay(OneTouchPlayCallback)} from the
 * Android system working as playback device to turn on the TV, and switch the input.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
class OneTouchPlayAction
    : public HdmiCecFeatureAction
    , public IOneTouchPlayAction
{
public:
    CAR_INTERFACE_DECL()

    OneTouchPlayAction();

    // Factory method. Ensures arguments are valid.
    static CARAPI Create(
        /* [in] */ HdmiCecLocalDevicePlayback* source,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback,
        /* [out] */ OneTouchPlayAction** result);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

private:
    OneTouchPlayAction(
        /* [in] */ IHdmiCecLocalDevice* localDevice,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI BroadcastActiveSource();

    CARAPI QueryDevicePowerStatus();

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // State in which the action is waiting for <Report Power Status>. In normal situation
    // source device can simply send <Text|Image View On> and <Active Source> in succession
    // since the standard requires that the TV/Display should buffer the <Active Source>
    // if the TV is brought of out standby state.
    //
    // But there are TV's that fail to buffer the <Active Source> while getting out of
    // standby mode, and do not accept the command until their power status becomes 'ON'.
    // For a workaround, we send <Give Device Power Status> commands periodically to make sure
    // the device switches its status to 'ON'. Then we send additional <Active Source>.
    static const Int32 STATE_WAITING_FOR_REPORT_POWER_STATUS;

    // The maximum number of times we send <Give Device Power Status> before we give up.
    // We wait up to RESPONSE_TIMEOUT_MS * LOOP_COUNTER_MAX = 20 seconds.
    static const Int32 LOOP_COUNTER_MAX;

    Int32 mTargetAddress;

    AutoPtr<IIHdmiControlCallback> mCallback;

    Int32 mPowerStatusCounter;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ONETOUCHPLAYACTION_H__
