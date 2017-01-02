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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEBOOLEAN_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEBOOLEAN_H__

#include "_Elastos_Droid_Hardware_Camera2_Marshal_Impl_CMarshalQueryableBoolean.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableBoolean.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

CarClass(CMarshalQueryableBoolean)
    , public MarshalQueryableBoolean
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_CMARSHALQUERYABLEBOOLEAN_H__
