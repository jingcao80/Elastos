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

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Stub class that models a logical mhl device hosted in this system.
 */
class HdmiMhlLocalDeviceStub
    : public Object
{
public:
    HdmiMhlLocalDeviceStub();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service,
        /* [in] */ Int32 portId);

    CARAPI OnDeviceRemoved();

    CARAPI GetInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    CARAPI SetBusMode(
        /* [in] */ Int32 cbusmode);

    CARAPI OnBusOvercurrentDetected(
        /* [in] */ Boolean on);

    CARAPI SetDeviceStatusChange(
        /* [in] */ Int32 adopterId,
        /* [in] */ Int32 deviceId);

    CARAPI GetPortId(
        /* [out] */ Int32* result);

    CARAPI TurnOn(
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI SendKeyEvent(
        /* [in] */ Int32 keycode,
        /* [in] */ Boolean isPressed);

private:
    static CARAPI_(AutoPtr<IHdmiDeviceInfo>) InitINFO();

private:
    static const AutoPtr<IHdmiDeviceInfo> INFO;

    AutoPtr<IHdmiControlService> mService;

    Int32 mPortId;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__
