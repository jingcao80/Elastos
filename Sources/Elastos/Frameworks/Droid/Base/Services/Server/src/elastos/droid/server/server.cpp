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

#include "elastos/droid/server/CAlarmManagerBinderService.h"
#include "elastos/droid/server/CBasePermissionDialog.h"
#include "elastos/droid/server/CPermissionDialog.h"
#include "elastos/droid/server/CBatteryBinderService.h"
#include "elastos/droid/server/CBluetoothManagerServiceBluetoothCallback.h"
#include "elastos/droid/server/CCommonTimeManagementServiceIfaceObserver.h"
#include "elastos/droid/server/CEntropyMixer.h"
#include "elastos/droid/server/CMasterClearReceiver.h"
#include "elastos/droid/server/CPersistentDataBlockService.h"
#include "elastos/droid/server/CPersistentDataBlockBinderService.h"
#include "elastos/droid/server/CProfileManagerService.h"
#include "elastos/droid/server/CThemeServiceSettingsObserver.h"
#include "elastos/droid/server/CTSMSUserSwitchObserver.h"
#include "elastos/droid/server/CUiModeManagerBinderService.h"
#include "elastos/droid/server/CUiModeManagerService.h"


namespace Elastos {
namespace Droid {
namespace Server {

CAR_OBJECT_IMPL(CAlarmManagerBinderService)
CAR_OBJECT_IMPL(CBasePermissionDialog)
CAR_OBJECT_IMPL(CPermissionDialog)
CAR_OBJECT_IMPL(CBatteryBinderService)
CAR_OBJECT_IMPL(CBluetoothManagerServiceBluetoothCallback)
CAR_OBJECT_IMPL(CCommonTimeManagementServiceIfaceObserver)
CAR_OBJECT_IMPL(CEntropyMixer)
CAR_OBJECT_IMPL(CMasterClearReceiver)
CAR_OBJECT_IMPL(CPersistentDataBlockService)
CAR_OBJECT_IMPL(CPersistentDataBlockBinderService)
CAR_OBJECT_IMPL(CThemeServiceSettingsObserver)
CAR_OBJECT_IMPL(CTSMSUserSwitchObserver)
CAR_OBJECT_IMPL(CUiModeManagerService)
CAR_OBJECT_IMPL(CUiModeManagerBinderService)
CAR_OBJECT_IMPL(CProfileManagerService)

} // namespace Server
} // namepsace Droid
} // namespace Elastos
