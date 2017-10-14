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

#include "elastos/droid/hardware/camera2/legacy/LegacyExceptionUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Utils::ICameraBinderDecorator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(LegacyExceptionUtils, Object, ILegacyExceptionUtils)

const String LegacyExceptionUtils::TAG("LegacyExceptionUtils");

ECode LegacyExceptionUtils::ThrowOnError(
    /* [in] */ Int32 errorFlag,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    switch (errorFlag) {
        case ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR: {
            *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
            return NOERROR;
        }
        case ICameraBinderDecorator::ICameraBinderDecorator_ENODEV: {
            //throw new BufferQueueAbandonedException();
            Logger::E(TAG, "BufferQueueAbandonedException");
            return E_BUFFER_QUEUE_ABANDONED_EXCEPTION;
        }
    }

    if (errorFlag < 0) {
        //throw new UnsupportedOperationException("Unknown error " + errorFlag);
        Logger::E(TAG, "Unknown error %d", errorFlag);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    *result = errorFlag;
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
