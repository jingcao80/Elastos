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

#ifndef __ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__

#include "_Elastos_Droid_Hardware_CSystemSensorManager.h"
#include "elastos/droid/hardware/SystemSensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CSystemSensorManager)
    , public SystemSensorManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__