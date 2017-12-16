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

#include "elastos/droid/graphics/NBitmapFactory.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

String NBitmapFactory::EncodedFormatToString(
    /* [in] */ SkEncodedImageFormat format)
{
    switch (format) {
        case SkEncodedImageFormat::kBMP:
            return String("image/bmp");
        case SkEncodedImageFormat::kGIF:
            return String("image/gif");
        case SkEncodedImageFormat::kICO:
            return String("image/x-ico");
        case SkEncodedImageFormat::kJPEG:
            return String("image/jpeg");
        case SkEncodedImageFormat::kPNG:
            return String("image/png");
        case SkEncodedImageFormat::kWEBP:
            return String("image/webp");
        case SkEncodedImageFormat::kWBMP:
            return String("image/vnd.wap.wbmp");
        case SkEncodedImageFormat::kDNG:
            return String("image/x-adobe-dng");
        default:
            return String(NULL);
    }
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
