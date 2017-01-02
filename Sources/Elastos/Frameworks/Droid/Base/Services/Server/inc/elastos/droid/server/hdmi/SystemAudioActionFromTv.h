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

#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMTV_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMTV_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/SystemAudioAction.h"

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles System Audio initiated by TV devices.
 */
class SystemAudioActionFromTv
    : public SystemAudioAction
    , public ISystemAudioActionFromTv
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor
     *
     * @param sourceAddress {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of tvAddress is invalid
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* sourceAddress,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean targetStatus,
        /* [in] */ IIHdmiControlCallback* callback);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTIONFROMTV_H__
