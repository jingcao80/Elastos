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

#ifndef __ELASTOS_DROID_SERVER_HDMI_SETARCTRANSMISSIONSTATEACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SETARCTRANSMISSIONSTATEACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDevice;
/**
 * Feature action that handles enabling/disabling of ARC transmission channel.
 * Once TV gets &lt;Initiate ARC&gt;, TV sends &lt;Report ARC Initiated&gt; to AV Receiver.
 * If it fails or it gets &lt;Terminate ARC&gt;, TV just disables ARC.
 */
class SetArcTransmissionStateAction
    : public HdmiCecFeatureAction
    , public ISetArcTransmissionStateAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ SetArcTransmissionStateAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        SetArcTransmissionStateAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SetArcTransmissionStateAction();

    /**
     * @Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param enabled whether to enable ARC Transmission channel
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean enabled);

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
    CARAPI SendReportArcInitiated();

    CARAPI SetArcStatus(
        /* [in] */ Boolean enabled);

private:
    static const String TAG;

    // State in which the action sent <Rerpot Arc Initiated> and
    // is waiting for time out. If it receives <Feature Abort> within timeout
    // ARC should be disabled.
    static const Int32 STATE_WAITING_TIMEOUT;

    Boolean mEnabled;

    Int32 mAvrAddress;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SETARCTRANSMISSIONSTATEACTION_H__
