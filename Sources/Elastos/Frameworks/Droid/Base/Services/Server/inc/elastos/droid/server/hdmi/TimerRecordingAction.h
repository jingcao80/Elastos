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

#ifndef __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs timer recording.
 */
class TimerRecordingAction
    : public HdmiCecFeatureAction
    , public ITimerRecordingAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ TimerRecordingAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        TimerRecordingAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TimerRecordingAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource);

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
    CARAPI SendTimerMessage();

    CARAPI_(Boolean) HandleTimerStatus(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI_(Boolean) HandleFeatureAbort(
        /* [in] */ IHdmiCecMessage* cmd);

    // Convert byte array to int.
    static CARAPI_(Int32) BytesToInt32(
        /* [in] */ ArrayOf<Byte>* data);

private:
    static const String TAG;

    // Timer out for waiting <Timer Status> 120s.
    static const Int32 TIMER_STATUS_TIMEOUT_MS;

    // State that waits for <Timer Status> once sending <Set XXX Timer>
    static const Int32 STATE_WAITING_FOR_TIMER_STATUS;

    Int32 mRecorderAddress;

    Int32 mSourceType;

    AutoPtr<ArrayOf<Byte> > mRecordSource;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__
