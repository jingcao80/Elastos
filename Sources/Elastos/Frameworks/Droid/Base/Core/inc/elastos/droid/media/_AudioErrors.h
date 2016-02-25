
#ifndef ___AUDIOERRORS_H__
#define ___AUDIOERRORS_H__

#include "_Elastos.Droid.Media.h"
#include <utils/Errors.h>

namespace Elastos {
namespace Droid {
namespace Media {

static inline Int32 NativeToElastosStatus(android::status_t status)
{
    switch (status) {
    case android::NO_ERROR:
        return IAudioSystem::SUCCESS;
    case android::BAD_VALUE:
        return IAudioSystem::BAD_VALUE;
    case android::INVALID_OPERATION:
        return IAudioSystem::INVALID_OPERATION;
    case android::PERMISSION_DENIED:
        return IAudioSystem::PERMISSION_DENIED;
    case android::NO_INIT:
        return IAudioSystem::NO_INIT;
    case android::DEAD_OBJECT:
        return IAudioSystem::DEAD_OBJECT;
    default:
        return IAudioSystem::ERROR;
    }
}

} // namespace Media
} // namespace Droid
}; // namespace Elastos

#endif // ___AUDIOERRORS_H__
