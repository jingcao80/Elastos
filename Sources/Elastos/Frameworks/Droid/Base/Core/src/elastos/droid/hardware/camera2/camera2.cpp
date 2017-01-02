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

#include "elastos/droid/hardware/camera2/CCameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CCaptureRequestBuilder.h"
#include "elastos/droid/hardware/camera2/CCaptureResult.h"
#include "elastos/droid/hardware/camera2/CCaptureResultKey.h"
#include "elastos/droid/hardware/camera2/CCaptureRequestKey.h"
#include "elastos/droid/hardware/camera2/CCameraCharacteristicsKey.h"
#include "elastos/droid/hardware/camera2/CCaptureFailure.h"
#include "elastos/droid/hardware/camera2/CTotalCaptureResult.h"
#include "elastos/droid/hardware/camera2/CCameraManager.h"
#include "elastos/droid/hardware/camera2/CDngCreator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_OBJECT_IMPL(CCameraCharacteristics)
CAR_OBJECT_IMPL(CCaptureRequestBuilder)
CAR_OBJECT_IMPL(CCaptureResult)
CAR_OBJECT_IMPL(CCaptureResultKey)
CAR_OBJECT_IMPL(CCaptureRequestKey)
CAR_OBJECT_IMPL(CCameraCharacteristicsKey)
CAR_OBJECT_IMPL(CCaptureFailure)
CAR_OBJECT_IMPL(CTotalCaptureResult)
CAR_OBJECT_IMPL(CCameraManager)
CAR_OBJECT_IMPL(CDngCreator)

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
