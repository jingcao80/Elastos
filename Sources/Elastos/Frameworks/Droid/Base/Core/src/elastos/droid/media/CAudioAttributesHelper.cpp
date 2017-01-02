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

#include "elastos/droid/media/CAudioAttributesHelper.h"
#include "elastos/droid/media/CAudioAttributes.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioAttributesHelper, Singleton, IAudioAttributesHelper)

CAR_SINGLETON_IMPL(CAudioAttributesHelper)

ECode CAudioAttributesHelper::UsageToString(
    /* [in] */ Int32 usage,
    /* [out] */ String* result)
{
    return CAudioAttributes::UsageToString(usage, result);
}

ECode CAudioAttributesHelper::UsageForLegacyStreamType(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    return CAudioAttributes::UsageForLegacyStreamType(streamType, result);
}

ECode CAudioAttributesHelper::ToLegacyStreamType(
    /* [in] */ IAudioAttributes* aa,
    /* [out] */ Int32* result)
{
    return CAudioAttributes::ToLegacyStreamType(aa, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
