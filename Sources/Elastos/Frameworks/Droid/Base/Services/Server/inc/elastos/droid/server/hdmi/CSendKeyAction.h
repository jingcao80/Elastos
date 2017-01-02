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

#ifndef __ELASTOS_DROID_SERVER_HDMI_CSENDKEYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_CSENDKEYACTION_H__

#include "_Elastos_Droid_Server_Hdmi_CSendKeyAction.h"
#include "elastos/droid/server/hdmi/SendKeyAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that transmits remote control key command (User Control Press/
 * User Control Release) to CEC bus.
 *
 * <p>This action is created when a new key event is passed to CEC service. It optionally
 * does key repeat (a.k.a. press-and-hold) operation until it receives a key release event.
 * If another key press event is received before the key in use is released, CEC service
 * does not create a new action but recycles the current one by updating the key used
 * for press-and-hold operation.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
CarClass(CSendKeyAction)
    , public SendKeyAction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CSENDKEYACTION_H__
