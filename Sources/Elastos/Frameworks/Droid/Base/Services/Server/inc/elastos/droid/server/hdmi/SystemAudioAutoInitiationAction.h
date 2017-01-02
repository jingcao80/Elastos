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

#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Action to initiate system audio once AVR is detected on Device discovery action.
 */
// Seq #27
class SystemAudioAutoInitiationAction
    : public HdmiCecFeatureAction
    , public ISystemAudioAutoInitiationAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ SystemAudioAutoInitiationAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        SystemAudioAutoInitiationAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SystemAudioAutoInitiationAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress);

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
    CARAPI SendGiveSystemAudioModeStatus();

    CARAPI HandleSystemAudioModeStatusMessage();

    CARAPI HandleSystemAudioModeStatusTimeout();

    CARAPI CanChangeSystemAudio(
        /* [out] */ Boolean* result);

private:
    Int32 mAvrAddress;

    // State that waits for <System Audio Mode Status> once send
    // <Give System Audio Mode Status> to AV Receiver.
    static const Int32 STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__
