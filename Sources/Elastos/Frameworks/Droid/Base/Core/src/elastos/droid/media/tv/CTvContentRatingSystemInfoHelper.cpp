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

#include "elastos/droid/media/tv/CTvContentRatingSystemInfoHelper.h"
#include "elastos/droid/media/tv/CTvContentRatingSystemInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvContentRatingSystemInfoHelper, Singleton, ITvContentRatingSystemInfoHelper)

CAR_SINGLETON_IMPL(CTvContentRatingSystemInfoHelper)

ECode CTvContentRatingSystemInfoHelper::CreateTvContentRatingSystemInfo(
    /* [in] */ Int32 xmlResourceId,
    /* [in] */ IApplicationInfo * applicationInfo,
    /* [out] */ ITvContentRatingSystemInfo ** result)
{
    return CTvContentRatingSystemInfo::CreateTvContentRatingSystemInfo(xmlResourceId, applicationInfo, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
