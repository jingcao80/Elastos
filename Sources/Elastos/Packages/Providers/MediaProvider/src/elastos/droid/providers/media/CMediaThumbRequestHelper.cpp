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

#include "elastos/droid/providers/media/CMediaThumbRequestHelper.h"
#include "elastos/droid/providers/media/MediaThumbRequest.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CAR_SINGLETON_IMPL(CMediaThumbRequestHelper)

CAR_INTERFACE_IMPL(CMediaThumbRequestHelper, Singleton, IMediaThumbRequestHelper)

ECode CMediaThumbRequestHelper::GetComparator(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<MediaThumbRequest::MediaThumbRequestComparator> mtr = new MediaThumbRequest::MediaThumbRequestComparator();
    *result = IComparator::Probe(mtr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos