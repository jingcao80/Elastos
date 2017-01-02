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

#ifndef __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs one touch record.
 */
class OneTouchRecordAction
    : public HdmiCecFeatureAction
    , public IOneTouchRecordAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ OneTouchRecordAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        OneTouchRecordAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    OneTouchRecordAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 recorderAddress,
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

    CARAPI GetRecorderAddress(
        /* [out] */ Int32* result);

private:
    CARAPI SendRecordOn();

    CARAPI_(Boolean) HandleRecordStatus(
        /* [in] */ IHdmiCecMessage* cmd);

private:
    static const String TAG;

    // Timer out for waiting <Record Status> 120s
    static const Int32 RECORD_STATUS_TIMEOUT_MS;

    // State that waits for <Record Status> once sending <Record On>
    static const Int32 STATE_WAITING_FOR_RECORD_STATUS;

    // State that describes recording in progress.
    static const Int32 STATE_RECORDING_IN_PROGRESS;

    Int32 mRecorderAddress;

    AutoPtr<ArrayOf<Byte> > mRecordSource;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__
