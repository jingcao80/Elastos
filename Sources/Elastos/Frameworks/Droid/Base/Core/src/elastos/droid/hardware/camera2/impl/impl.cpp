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

#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCallbackProxiesDeviceStateCallbackProxy.h"
#include "elastos/droid/hardware/camera2/impl/CCallbackProxiesSessionStateCallbackProxy.h"
#include "elastos/droid/hardware/camera2/impl/CCallbackProxiesDeviceCaptureCallbackProxy.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImpl.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplCaptureCallbackHolder.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplFrameNumberTracker.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplCameraDeviceCallbacks.h"
#include "elastos/droid/hardware/camera2/impl/CCameraCaptureSessionImpl.h"
#include "elastos/droid/hardware/camera2/impl/CCaptureResultExtras.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CAR_OBJECT_IMPL(CCameraMetadataNative)
CAR_OBJECT_IMPL(CCallbackProxiesDeviceStateCallbackProxy)
CAR_OBJECT_IMPL(CCallbackProxiesSessionStateCallbackProxy)
CAR_OBJECT_IMPL(CCallbackProxiesDeviceCaptureCallbackProxy)
CAR_OBJECT_IMPL(CCameraDeviceImpl)
CAR_OBJECT_IMPL(CCameraDeviceImplCaptureCallbackHolder)
CAR_OBJECT_IMPL(CCameraDeviceImplFrameNumberTracker)
CAR_OBJECT_IMPL(CCameraDeviceImplCameraDeviceCallbacks)
CAR_OBJECT_IMPL(CCameraCaptureSessionImpl)
CAR_OBJECT_IMPL(CCaptureResultExtras)


} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos