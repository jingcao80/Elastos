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

#ifndef __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClientDisplayStatusCallback;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDevice;
/**
 * Feature action that queries the power status of other device. This action is initiated via
 * {@link HdmiPlaybackClient#queryDisplayStatus(DisplayStatusCallback)} from the Android system
 * working as playback device to get the power status of TV device.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
class DevicePowerStatusAction
    : public HdmiCecFeatureAction
    , public IDevicePowerStatusAction
{
public:
    CAR_INTERFACE_DECL()

    DevicePowerStatusAction();

    static CARAPI Create(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback,
        /* [out] */ DevicePowerStatusAction** result);

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
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* localDevice,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI QueryDevicePowerStatus();

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // State in which the action is waiting for <Report Power Status>.
    static const Int32 STATE_WAITING_FOR_REPORT_POWER_STATUS;

    Int32 mTargetAddress;

    AutoPtr<IIHdmiControlCallback> mCallback;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__
