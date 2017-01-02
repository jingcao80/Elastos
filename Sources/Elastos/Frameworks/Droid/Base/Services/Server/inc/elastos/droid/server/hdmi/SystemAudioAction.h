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

#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Base feature action class for SystemAudioActionFromTv and SystemAudioActionFromAvr.
 */
class SystemAudioAction
    : public HdmiCecFeatureAction
    , public ISystemAudioAction
{
private:
    class InnerSub_Runnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_Runnable(
            /* [in] */ SystemAudioAction* host);

        //@Override
        CARAPI Run();

    private:
        SystemAudioAction* mHost;
    };

    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ SystemAudioAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        SystemAudioAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SystemAudioAction();

    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of sourceAddress and avrAddress is invalid
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean targetStatus,
        /* [in] */ IIHdmiControlCallback* callback);

    // Seq #27
    CARAPI SendSystemAudioModeRequest();

    CARAPI SetSystemAudioMode(
        /* [in] */ Boolean mode);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    CARAPI StartAudioStatusAction();

    CARAPI RemoveSystemAudioActionInProgress();

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

    // TODO: if IHdmiControlCallback is general to other FeatureAction,
    //       move it into FeatureAction.
    CARAPI FinishWithCallback(
        /* [in] */ Int32 returnCode);

private:
    CARAPI SendSystemAudioModeRequestInternal();

    CARAPI HandleSendSystemAudioModeRequestTimeout();

public:
    // Logical address of AV Receiver.
    Int32 mAvrLogicalAddress;

    // The target audio status of the action, whether to enable the system audio mode or not.
    Boolean mTargetAudioStatus;

private:
    static const String TAG;

    // Transient state to differentiate with STATE_NONE where the on-finished callback
    // will not be called.
    static const Int32 STATE_CHECK_ROUTING_IN_PRGRESS;

    // State in which waits for <SetSystemAudioMode>.
    static const Int32 STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE;

    static const Int32 MAX_SEND_RETRY_COUNT;

    static const Int32 ON_TIMEOUT_MS;

    static const Int32 OFF_TIMEOUT_MS;

    AutoPtr<IIHdmiControlCallback> mCallback;

    Int32 mSendRetryCount;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__
