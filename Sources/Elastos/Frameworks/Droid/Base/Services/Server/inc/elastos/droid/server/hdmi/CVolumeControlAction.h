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

#ifndef __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CVolumeControlAction.h"
#include "elastos/droid/server/hdmi/VolumeControlAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that transmits volume change to Audio Receiver.
 * <p>
 * This action is created when a user pressed volume up/down. However, Android only provides a
 * listener for delta of some volume change instead of individual key event. Also it's hard to know
 * Audio Receiver's number of volume steps for a single volume control key. Because of this, it
 * sends key-down event until IRT timeout happens, and it will send key-up event if no additional
 * volume change happens; otherwise, it will send again key-down as press and hold feature does.
 */
CarClass(CVolumeControlAction)
    , public VolumeControlAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CVOLUMECONTROLACTION_H__
