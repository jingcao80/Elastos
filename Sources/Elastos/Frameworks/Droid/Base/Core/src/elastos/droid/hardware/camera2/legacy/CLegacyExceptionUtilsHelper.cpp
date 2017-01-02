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

#include "elastos/droid/hardware/camera2/legacy/CLegacyExceptionUtilsHelper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyExceptionUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacyExceptionUtilsHelper, Singleton, ILegacyExceptionUtilsHelper)

CAR_SINGLETON_IMPL(CLegacyExceptionUtilsHelper)

ECode CLegacyExceptionUtilsHelper::ThrowOnError(
    /* [in] */ Int32 errorFlag,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return LegacyExceptionUtils::ThrowOnError(errorFlag, result);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
