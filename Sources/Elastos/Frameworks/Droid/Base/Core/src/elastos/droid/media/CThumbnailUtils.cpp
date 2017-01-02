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

#include "elastos/droid/media/CThumbnailUtils.h"
#include "elastos/droid/media/ThumbnailUtils.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CThumbnailUtils)

CAR_INTERFACE_IMPL(CThumbnailUtils, Singleton, IThumbnailUtils)

ECode CThumbnailUtils::CreateImageThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::CreateImageThumbnail(filePath, kind, result);
}

ECode CThumbnailUtils::CreateVideoThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::CreateVideoThumbnail(filePath, kind, result);
}

ECode CThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::ExtractThumbnail(source, width, height, result);
}

ECode CThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 options,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::ExtractThumbnail(source, width, height, options, result);
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
