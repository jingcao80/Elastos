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

#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOSTATUSACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOSTATUSACTION_H__

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

/**
 * Action to update audio status (volume or mute) of audio amplifier
 */
class SystemAudioStatusAction
    : public HdmiCecFeatureAction
    , public ISystemAudioStatusAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ SystemAudioStatusAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        SystemAudioStatusAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SystemAudioStatusAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ IIHdmiControlCallback* callback);

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
    CARAPI SendGiveAudioStatus();

    CARAPI HandleSendGiveAudioStatusFailure();

    CARAPI HandleReportAudioStatus(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI FinishWithCallback(
        /* [in] */ Int32 returnCode);

private:
    static const String TAG;

    // State that waits for <ReportAudioStatus>.
    static const Int32 STATE_WAIT_FOR_REPORT_AUDIO_STATUS;

    Int32 mAvrAddress;

    AutoPtr<IIHdmiControlCallback> mCallback;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOSTATUSACTION_H__
