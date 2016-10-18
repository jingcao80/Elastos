
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
        Logger::E(TAG, "Unknown error %d" + errorFlag);
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
