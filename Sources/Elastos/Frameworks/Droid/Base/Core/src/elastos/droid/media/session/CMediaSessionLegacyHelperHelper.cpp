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

#include "elastos/droid/media/session/CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaSessionLegacyHelperHelper, Singleton, IMediaSessionLegacyHelperHelper)

CAR_SINGLETON_IMPL(CMediaSessionLegacyHelperHelper)

ECode CMediaSessionLegacyHelperHelper::GetHelper(
    /* [in] */ IContext * context,
    /* [out] */ IMediaSessionLegacyHelper ** result)
{
    return CMediaSessionLegacyHelper::GetHelper(context, result);
}

ECode CMediaSessionLegacyHelperHelper::GetOldMetadata(
    /* [in] */ IMediaMetadata * metadata,
    /* [in] */ Int32 artworkWidth,
    /* [in] */ Int32 artworkHeight,
    /* [out] */ IBundle ** result)
{
    return CMediaSessionLegacyHelper::GetOldMetadata(metadata, artworkWidth, artworkHeight, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
