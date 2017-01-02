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

#include "elastos/droid/server/hdmi/CDeviceDiscoveryAction.h"
#include "elastos/droid/server/hdmi/CDevicePowerStatusAction.h"
#include "elastos/droid/server/hdmi/CDeviceSelectAction.h"
#include "elastos/droid/server/hdmi/CHdmiControlServiceBinderService.h"
#include "elastos/droid/server/hdmi/CHotplugDetectionAction.h"
#include "elastos/droid/server/hdmi/CNewDeviceAction.h"
#include "elastos/droid/server/hdmi/COneTouchPlayAction.h"
#include "elastos/droid/server/hdmi/COneTouchRecordAction.h"
#include "elastos/droid/server/hdmi/CPowerStatusMonitorAction.h"
#include "elastos/droid/server/hdmi/CRequestArcInitiationAction.h"
#include "elastos/droid/server/hdmi/CRequestArcTerminationAction.h"
#include "elastos/droid/server/hdmi/CRoutingControlAction.h"
#include "elastos/droid/server/hdmi/CSendKeyAction.h"
#include "elastos/droid/server/hdmi/CSetArcTransmissionStateAction.h"
#include "elastos/droid/server/hdmi/CSystemAudioActionFromAvr.h"
#include "elastos/droid/server/hdmi/CSystemAudioActionFromTv.h"
#include "elastos/droid/server/hdmi/CSystemAudioAutoInitiationAction.h"
#include "elastos/droid/server/hdmi/CSystemAudioStatusAction.h"
#include "elastos/droid/server/hdmi/CTimerRecordingAction.h"
#include "elastos/droid/server/hdmi/CVolumeControlAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_OBJECT_IMPL(CDeviceDiscoveryAction)

CAR_OBJECT_IMPL(CDevicePowerStatusAction)

CAR_OBJECT_IMPL(CDeviceSelectAction)

CAR_OBJECT_IMPL(CHdmiControlServiceBinderService)

CAR_OBJECT_IMPL(CHotplugDetectionAction)

CAR_OBJECT_IMPL(CNewDeviceAction)

CAR_OBJECT_IMPL(COneTouchPlayAction)

CAR_OBJECT_IMPL(COneTouchRecordAction)

CAR_OBJECT_IMPL(CPowerStatusMonitorAction)

CAR_OBJECT_IMPL(CRequestArcInitiationAction)

CAR_OBJECT_IMPL(CRequestArcTerminationAction)

CAR_OBJECT_IMPL(CRoutingControlAction)

CAR_OBJECT_IMPL(CSendKeyAction)

CAR_OBJECT_IMPL(CSetArcTransmissionStateAction)

CAR_OBJECT_IMPL(CSystemAudioActionFromAvr)

CAR_OBJECT_IMPL(CSystemAudioActionFromTv)

CAR_OBJECT_IMPL(CSystemAudioAutoInitiationAction)

CAR_OBJECT_IMPL(CSystemAudioStatusAction)

CAR_OBJECT_IMPL(CTimerRecordingAction)

CAR_OBJECT_IMPL(CVolumeControlAction)

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
