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

#ifndef __ELASTOS_DROID_SERVER_HDMI_REQUESTARCACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_REQUESTARCACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Base feature action class for &lt;Request ARC Initiation&gt;/&lt;Request ARC Termination&gt;.
 */
class RequestArcAction
    : public HdmiCecFeatureAction
    , public IRequestArcAction
{
public:
    CAR_INTERFACE_DECL()

    RequestArcAction();

    /**
     * @Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress address of AV receiver. It should be AUDIO_SYSTEM type
     * @throw IllegalArugmentException if device type of sourceAddress and avrAddress
     *                      is invalid
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    CARAPI DisableArcTransmission();

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

public:
    // State in which waits for ARC response.
    static const Int32 STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE;

    // Logical address of AV Receiver.
    Int32 mAvrAddress;

private:
    static const String TAG;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_REQUESTARCACTION_H__
