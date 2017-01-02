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

#include "elastos/droid/media/CAudioServiceHelper.h"
#include "elastos/droid/media/CAudioService.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioServiceHelper, Singleton, IAudioServiceHelper)

CAR_SINGLETON_IMPL(CAudioServiceHelper)

ECode CAudioServiceHelper::StreamToString(
    /* [in] */ Int32 stream,
    /* [out] */ String* result)
{
    return CAudioService::StreamToString(stream, result);
}

ECode CAudioServiceHelper::GetValueForVibrateSetting(
    /* [in] */ Int32 existingValue,
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting,
    /* [out] */ Int32* result)
{
    return CAudioService::GetValueForVibrateSetting(existingValue, vibrateType, vibrateSetting, result);
}

} //namespace Media
} //namespace Droid
} //namespace Elastos
