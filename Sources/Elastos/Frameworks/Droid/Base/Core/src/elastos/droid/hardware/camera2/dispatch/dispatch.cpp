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

#include "elastos/droid/hardware/camera2/dispatch/CNullDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CInvokeDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CHandlerDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CArgumentReplacingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CDuckTypingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CBroadcastDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CMethodNameInvoker.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_OBJECT_IMPL(CNullDispatcher)
CAR_OBJECT_IMPL(CInvokeDispatcher)
CAR_OBJECT_IMPL(CHandlerDispatcher)
CAR_OBJECT_IMPL(CArgumentReplacingDispatcher)
CAR_OBJECT_IMPL(CDuckTypingDispatcher)
CAR_OBJECT_IMPL(CBroadcastDispatcher)
CAR_OBJECT_IMPL(CMethodNameInvoker)

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
