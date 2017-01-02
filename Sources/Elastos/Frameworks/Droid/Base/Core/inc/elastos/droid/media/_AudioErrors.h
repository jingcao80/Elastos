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
